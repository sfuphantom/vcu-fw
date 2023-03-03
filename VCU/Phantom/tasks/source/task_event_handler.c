/*
 * task_event_handler.c
 *
 *  Created on: Feb 11, 2023
 *      Author: rafgu
 */

#include "task_event_handler.h"

/* Phantom tasks */
#include "task_config.h"
#include "task_logger.h"

typedef struct event_t
{
	event_handler_t callback;
	uint16_t data;
}event_t;


static PipeTask_t rtos_handles;

static void ThreadEventHandler(void* pvParams);


/* Public API */

TaskHandle_t EventHandlerInit()
{
	BaseType_t ret = xTaskCreate(
		ThreadEventHandler,
		"EventHandler",
		EVENT_HANDLER_STACK_SIZE,
		NULL,
        EVENT_HANDLER_PRIORITY,
		&rtos_handles.taskHandle
	);

    rtos_handles.q = xQueueCreate(16, sizeof(event_t));

	return ret == pdPASS && rtos_handles.q ? rtos_handles.taskHandle : NULL;
}


bool HandleEvent(event_handler_t callback, uint16_t data)
{	
	event_t event;
	event.callback = callback;
	event.data = data;

	return xQueueSendToBack(rtos_handles.q, &event, 1) == pdPASS;
}


bool  HandleEventFromISR(event_handler_t callback, uint16_t data)
{
	BaseType_t xHigherPriorityTaskWoken;

	event_t event;
	event.callback = callback;
	event.data = data;

	return xQueueSendToBackFromISR(rtos_handles.q, &event, &xHigherPriorityTaskWoken) == pdPASS;
}


/* Internal Implementation */

static void ThreadEventHandler(void* pvParams)
{
	static event_t event;

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
