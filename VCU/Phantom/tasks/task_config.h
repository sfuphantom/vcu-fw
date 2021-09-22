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
#define THROTTLE_TASK_PERIOD_MS         5    // 10ms
#define SENSOR_READ_TASK_PERIOD_MS      100   // 100ms
#define STATE_MACHINE_TASK_PERIOD_MS    100
#define EEPROM_TASK_PERIOD_MS           10
#define WATCHDOG_TASK__PERIOD_MS        300  // 300ms
#define DATA_LOGGING_TASK_PERIOD_MS     500  // 500ms

#define TEST_TASK_PERIOD_MS             500


/*********************************************************************************
 *                              TASK PRIORITIES
 *********************************************************************************/
#define THROTTLE_TASK_PRIORITY          4
#define SENSOR_READ_TASK_PRIORITY       3
#define STATE_MACHINE_TASK_PRIORITY     2
#define EEPROM_TASK_PRIORITY            0
#define DATA_LOGGING_TASK_PRIORITY      0    // same as idle task
#define WATCHDOG_TASK_PRIORITY          0    // same as idle task

#define TEST_TASK_PRIORITY              1

// there may also be interrupt/ISR priorities for:
// CAN messages
// GPIOs (ready to drive, shutdown circuit GPIOs..)

/*********************************************************************************
 *                              TASK STACK SIZES
 *********************************************************************************/
#define THROTTLE_TASK_STACK_SIZE        150
#define SENSOR_READ_TASK_STACK_SIZE     150
#define STATE_MACHINE_TASK_STACK_SIZE   150
#define EEPROM_TASK_STACK_SIZE          150
#define DATA_LOGGING_TASK_STACK_SIZE    150    // same as idle task
#define WATCHDOG_TASK_STACK_SIZE        150    // same as idle task

#define TEST_TASK_STACK_SIZE            150


#endif /* TASKS_PRIORITIES_H_ */
