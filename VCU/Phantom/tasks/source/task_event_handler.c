/*
 * task_event_handler.c
 *
 *  Created on: Feb 11, 2023
 *      Author: rafgu
 */


#include "task_event_handler.h"
#include "Phantom_sci.h"

#include "task_config.h"
#include "task_logger.h"

typedef struct event_t
{
	event_handler_t callback;
	uint16_t data;
}event_t;

static PipeTask_t rtos_handles;

static Task task;
static void ThreadEventHandler(void* pvParams);

typedef enum ePriority{
	CRITICAL,
	BACKGROUND
}ePriority;

#define MAX_DATA_LENGTH 8

TaskHandle_t EventHandlerInit()
{
	xTaskCreate(
		ThreadEventHandler,
		"EventHandler",
		128,
		NULL,
        EVENT_HANDLER_PRIORITY,
		&rtos_handles.taskHandle
	);

    rtos_handles.q = xQueueCreate(16, sizeof(event_t));

	return rtos_handles.taskHandle;
}

void QueueEvent(event_t event, eSource source, ePriority priority)
{	
	uint8_t ret;
	BaseType_t xHigherPriorityTaskWoken;

	if (priority == BACKGROUND && source == FROM_SCHEDULER)
	{
		ret = xQueueSendToBack(rtos_handles.q, &event, 1) == pdPASS;
	}
	else if (priority == BACKGROUND && source == FROM_ISR)
	{
		ret = xQueueSendToBackFromISR(rtos_handles.q, &event, &xHigherPriorityTaskWoken) == pdPASS;

	}
	else if (priority == CRITICAL && source == FROM_SCHEDULER){

		ret = xQueueSendToFront(rtos_handles.q, &event, 1) == pdPASS;

	} else if (priority == CRITICAL && source == FROM_ISR)
	{
		BaseType_t xHigherPriorityTaskWoken;

		ret = xQueueSendToFrontFromISR(rtos_handles.q, &event, &xHigherPriorityTaskWoken) == pdPASS;
	}
	else
	{
		ret = 0; 
	}
}


void HandleToFront(event_handler_t callback, uint16_t data, eSource source)
{	
	event_t event;
	event.callback = callback;
	event.data = data;

	QueueEvent(event, source, CRITICAL);
}

void HandleToBack(event_handler_t callback, uint16_t data, eSource source)
{
	event_t event;
	event.callback = callback;
	event.data = data;

	QueueEvent(event, source, BACKGROUND);
}

/* Internal Implementation*/
static void ThreadEventHandler(void* pvParams)
{
	event_t event;

	Log("Starting thread");

	while(1)
	{
		if (xQueueReceive(rtos_handles.q, &event, portMAX_DELAY) == pdFALSE)
		{
			continue;	
		}
		
		event.callback(&event.data);
	}
}



