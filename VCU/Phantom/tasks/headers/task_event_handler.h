/*
 * task_event_handler.h
 *
 *  Created on: Feb 11, 2023
 *      Author: rafgu
 */

#ifndef PHANTOM_TASKS_HEADERS_TASK_EVENT_HANDLER_H_
#define PHANTOM_TASKS_HEADERS_TASK_EVENT_HANDLER_H_

#include "vcu_common.h"

typedef void (*event_handler_t)(void*);

TaskHandle_t EventHandlerInit();

bool HandleEvent(event_handler_t callback, uint16_t data);
bool HandleEventFromISR(event_handler_t callback, uint16_t data);

#endif /* PHANTOM_TASKS_HEADERS_TASK_EVENT_HANDLER_H_ */
