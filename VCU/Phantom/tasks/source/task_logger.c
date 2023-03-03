/*
 * task_logger.c
 *
 *  Created on: Feb 11, 2023
 *      Author: rafgu
 */

#include "task_logger.h"

/* C standard libs */
#include "math.h"

/* Phantom modules */
#include "Phantom_sci.h"

#include "task_event_handler.h"

typedef uint64_t segment_t;
#define SEGMENT_SIZE sizeof(segment_t)

static PipeTask_t rtos_handles;

static void LoggerThread(void* pvParams);
static uint8_t LogMessage(const char* color, const char* str, eSource source);
static uint8_t AsyncPrint(eSource source, const char* str);


/* Public API */

TaskHandle_t LoggerInit()
{
	BaseType_t ret = xTaskCreate(
		LoggerThread,
		"LoggerThread",
		LOGGER_STACK_SIZE,
		0,
		LOGGER_PRIORITY,
		&rtos_handles.taskHandle
	);

    rtos_handles.q = xQueueCreate(128, sizeof(segment_t));

	return ret == pdPASS && rtos_handles.q ? rtos_handles.taskHandle : NULL;
}
 

uint8_t Log(const char* str)
{
	return LogMessage(WHT, str, FROM_SCHEDULER);
}


uint8_t LogColor(const char* color, const char* str)
{
	return LogMessage(color, str, FROM_SCHEDULER);
}


uint8_t LogFromISR(const char* color, const char* str)
{
	return LogMessage(color, str, FROM_ISR);
}


void FlushLogger(uint16_t waitms)
{
	vTaskPrioritySet(rtos_handles.taskHandle, configMAX_PRIORITIES-1);

	vTaskDelay(pdMS_TO_TICKS(waitms)); 

	vTaskPrioritySet(rtos_handles.taskHandle, LOGGER_PRIORITY);
}


void GetLogHeader(eSource source, const char* color, char* str)
{
	switch (source)
	{
	case FROM_ISR:
		sprintf(str,  "\r%s[FromISR:%.2f] ", color, (float)xTaskGetTickCountFromISR()/configTICK_RATE_HZ);
		break;
	
	case FROM_SCHEDULER:
		sprintf(str,  "\r%s[%s:%.2f] ", color, pcTaskGetName(NULL), (float)xTaskGetTickCount()/configTICK_RATE_HZ);
		break;

	default:
		break;
	}
}


/* Internal Implementation*/

static void LoggerThread(void* pvParams)
{
	segment_t segment;

	Log("Starting thread");

	while(1)
	{
		if (xQueueReceive(rtos_handles.q, &segment, portMAX_DELAY) == pdFALSE)
		{
			continue;	
		}

		vTaskPrioritySet(rtos_handles.taskHandle, configMAX_PRIORITIES-1);

		UARTprintf((char*)&segment);

		vTaskPrioritySet(rtos_handles.taskHandle, LOGGER_PRIORITY);
	}
}

static uint8_t LogMessage(const char* color, const char* str, eSource source)
{
	// prepare timestamp 
	char header[64];
	GetLogHeader(source, color, header);
	AsyncPrint(source, header);

	// queue user string 
	AsyncPrint(source, str);
	
	// queue strings to reset console position and format
	char buffer[16];
	sprintf(buffer, "%s\n", reset);

	return AsyncPrint(source, buffer);
}


uint8_t QueueSegment(eSource source, segment_t segment)
{
	uint8_t ret;

	switch(source)
	{
		case FROM_ISR:
		{
			BaseType_t xHigherPriorityTaskWoken;

			ret = xQueueSendToBackFromISR(rtos_handles.q, &segment, &xHigherPriorityTaskWoken) == pdPASS;

			break;
		}
		case FROM_SCHEDULER:
		{
			ret = xQueueSendToBack(rtos_handles.q, &segment, 1) == pdPASS;

			break;
		}
		default:
		{
			ret = false;
			break;
		}
	}

	return ret;
}

uint8_t AsyncPrint(eSource source, const char* str)
{
	/* Initialize tmp to copy segments into */
	segment_t tmp;
	memset(&tmp, ' ', SEGMENT_SIZE);

	uint16_t total_num_bytes = strlen(str);
	uint8_t num_segments = total_num_bytes/SEGMENT_SIZE; 

	uint8_t segment_index = 0;

	/* Copy all full segments */
	for (segment_index = 0; segment_index < num_segments; segment_index++)
	{
		memcpy(&tmp, str + (segment_index*SEGMENT_SIZE), SEGMENT_SIZE);

		QueueSegment(source, tmp);
	}

	uint16_t bytes_left = total_num_bytes - (segment_index*SEGMENT_SIZE);
	
	/* Queue remaining segments */
	if (bytes_left != 0) 
	{
		memset(&tmp, ' ', SEGMENT_SIZE);
		memcpy(&tmp, str + (segment_index*SEGMENT_SIZE), bytes_left);
		
		QueueSegment(source, tmp);
	}

	return 1;
}
