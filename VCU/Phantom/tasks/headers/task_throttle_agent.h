/*
 * task_throttle_actor.h
 *
 *  Created on: July 15, 2022
 *      Author: josh
 */

#ifndef PHANTOM_TASKS_TASK_THROTTLE_AGENT_H_
#define PHANTOM_TASKS_TASK_THROTTLE_AGENT_H_

QueueHandle_t ThrottleAgent_getMailBoxHandle();

#ifndef VCU_SIM_MODE

void Task_throttleAgentSimInit(void);

#else

void Task_throttleAgentInit(void);

#endif

#endif
