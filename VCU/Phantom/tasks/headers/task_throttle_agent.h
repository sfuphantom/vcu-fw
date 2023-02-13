/*
 * task_throttle_actor.h
 *
 *  Created on: July 15, 2022
 *      Author: josh
 */

#ifndef PHANTOM_TASKS_TASK_THROTTLE_AGENT_H_
#define PHANTOM_TASKS_TASK_THROTTLE_AGENT_H_

#include "vcu_common.h"


#define HYSTERESIS      (200U)

TaskHandle_t throttleAgentInit(void);
uint8_t receivePedalReadings(pedal_reading_t* pdreading, TickType_t wait_time_ms);


#endif
