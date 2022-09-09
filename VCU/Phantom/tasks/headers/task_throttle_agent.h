/*
 * task_throttle_actor.h
 *
 *  Created on: July 15, 2022
 *      Author: josh
 */

#ifndef PHANTOM_TASKS_TASK_THROTTLE_AGENT_H_
#define PHANTOM_TASKS_TASK_THROTTLE_AGENT_H_

#include "vcu_common.h"

QueueHandle_t ThrottleAgent_receive(pedal_reading_t* pdreadings, TickType_t wait_time_ms);

#ifndef VCU_SIM_MODE

void Task_throttleAgentSimInit(void);

#else

void Task_throttleAgentInit(void);

#endif

#endif
