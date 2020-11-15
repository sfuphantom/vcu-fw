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
//#include "sys_main.h"

#include "Phantom_sci.h"

#include "task_data_logging.h"
#include "task_sensor_read.h"
#include "task_statemachine.h"
#include "task_throttle.h"
#include "task_watchdog.h"

#include "board_hardware.h"

#include "priorities.h" // holds the task priorities

#define NUMBER_OF_TIMERS   2

/* array to hold handles to the created timers*/
extern TimerHandle_t xTimers[NUMBER_OF_TIMERS];

xQueueHandle VCUDataQueue;

/* This timer is used to debounce the interrupts for the RTDS and SDC signals */
extern bool INTERRUPT_AVAILABLE;
extern bool THROTTLE_AVAILABLE; // used to only enable throttle after the buzzer has gone for 2 seconds

void Timer_300ms(TimerHandle_t xTimers);
void Timer_2s(TimerHandle_t xTimers);

void phantom_freeRTOSInit(void);
void phantom_freeRTOStimerInit(void);
void phantom_freeRTOStaskInit(void);


#endif /* PHANTOM_DRIVERS_RTOS_PHANTOM_FREERTOS_H_ */
