/*
 * task_config.h
 *
 *  Created on: Mar 16, 2020
 *      Author: gabriel, junaid, josh
 */

#ifndef TASKS_CONFIG_H_
#define TASKS_CONFIG_H_

/*********************************************************************************
 *                              TASK TIMINGS
 *********************************************************************************/
#define THROTTLE_AGT_PERIOD_MS          50  // 20Hz
#define THROTTLE_AGT_SIM_PERIOD_MS      50  // 20Hz
#define STATE_MACHINE_TASK_PERIOD_MS    10
#define EEPROM_TASK_PERIOD_MS           10
#define WATCHDOG_TASK__PERIOD_MS        300 // 300ms

#define TEST_TASK_PERIOD_MS             500


/*********************************************************************************
 *                              TASK PRIORITIES
 *********************************************************************************/
#define PEDAL_READINGS_PRIORITY         2
#define THROTTLE_ACT_PRIORITY           4
#define EVENT_HANDLER_PRIORITY          5
#define LOGGER_PRIORITY                 1

// #define EEPROM_TASK_PRIORITY            0
// #define WATCHDOG_TASK_PRIORITY          0    // same as idle task
// #define TEST_TASK_PRIORITY              1

// there may also be interrupt/ISR priorities for:
// CAN messages
// GPIOs (ready to drive, shutdown circuit GPIOs..)

/*********************************************************************************
 *                              TASK STACK SIZES
 *********************************************************************************/
#define THROTTLE_AGT_STACK_SIZE         200
#define THROTTLE_ACT_STACK_SIZE         500
#define LOGGER_STACK_SIZE               700
#define EVENT_HANDLER_STACK_SIZE        128
// #define EEPROM_TASK_STACK_SIZE          750
// #define WATCHDOG_TASK_STACK_SIZE        750    // same as idle task

#define TEST_TASK_STACK_SIZE            150


#endif /* TASKS_PRIORITIES_H_ */
