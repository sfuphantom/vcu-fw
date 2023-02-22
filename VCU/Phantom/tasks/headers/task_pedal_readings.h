/*
 * task_throttle_actor.h
 *
 *  Created on: July 15, 2022
 *      Author: josh
 */

#ifndef PHANTOM_TASKS_task_pedal_readings_H_
#define PHANTOM_TASKS_task_pedal_readings_H_

#include "vcu_common.h"


#define HYSTERESIS      (200U)

TaskHandle_t PedalReadingsInit(void);
uint8_t ReceivePedalReadings(pedal_reading_t* pdreading, TickType_t wait_time_ms);


#endif
