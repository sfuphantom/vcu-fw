/*
 * priorities.h
 *
 *  Created on: Mar 16, 2020
 *      Author: gabriel
 */

#ifndef TASKS_PRIORITIES_H_
#define TASKS_PRIORITIES_H_

//++ Added by jjkhan - For readability - use these MACROS with pdMS_TO_TICKS(MillisecondTime)

#define THROTTLE_TASK_PERIOD_MS         pdMS_TO_TICKS(10)
#define SENSOR_READ_TASK_PERIOD_MS      pdMS_TO_TICKS(10)
#define STATE_MACHINE_TASK_PERIOD_MS    pdMS_TO_TICKS(10)
#define EEPROM_TASK_PERIOD_MS           pdMS_TO_TICKS(10)
#define DATA_LOGGING_TASK_PERIOD_MS     pdMS_TO_TICKS(10)
#define WATCHDOG_TASK__PERIOD_MS        pdMS_TO_TICKS(10)

// -- Added by jjkhan
/*********************************************************************************
 *                              TASK PRIORITIES
 *********************************************************************************/

#define THROTTLE_TASK_PRIORITY         4
#define SENSOR_READ_TASK_PRIORITY      3
#define STATE_MACHINE_TASK_PRIORITY    2
#define EEPROM_TASK_PRIORITY           2
#define DATA_LOGGING_TASK_PRIORITY     0    // same as idle task
#define WATCHDOG_TASK_PRIORITY         0    // same as idle task


// there may also be interrupt/ISR priorities for:
// CAN messages
// GPIOs (ready to drive, shutdown circuit GPIOs..)

#endif /* TASKS_PRIORITIES_H_ */
