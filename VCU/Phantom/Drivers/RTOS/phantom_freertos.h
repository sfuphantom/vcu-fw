/*
 * phantom_freertos.h
 *
 *  Created on: Nov 14, 2020
 *      Author: gabriel
 */

#ifndef PHANTOM_DRIVERS_RTOS_PHANTOM_FREERTOS_H_
#define PHANTOM_DRIVERS_RTOS_PHANTOM_FREERTOS_H_

#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "os_task.h"
#include "os_queue.h"
#include "os_semphr.h"
#include "os_timer.h"


#include "Phantom_sci.h"

#include "task_data_logging.h"
#include "task_sensor_read.h"
#include "task_statemachine.h"
#include "task_throttle.h"
#include "task_watchdog.h"
#include "task_eeprom.h"

#include "board_hardware.h"
#include "priorities.h" // holds the task priorities
#include "vcu_data.h"

//++ Added by jjkhan
#include "../test_eeprom/testEeprom.h" // Included for testing eeprom -> should remove later.
//-- Added by jjkhan

#define NUMBER_OF_TIMERS   2

/* This timer is used to debounce the interrupts for the RTDS and SDC signals */
void Timer_300ms(TimerHandle_t xTimers);
void Timer_2s(TimerHandle_t xTimers);

void phantom_freeRTOSInit(void);
void phantom_freeRTOStimerInit(void);
void phantom_freeRTOStaskInit(void);


#endif /* PHANTOM_DRIVERS_RTOS_PHANTOM_FREERTOS_H_ */
