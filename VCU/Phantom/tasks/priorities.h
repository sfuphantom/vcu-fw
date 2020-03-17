/*
 * priorities.h
 *
 *  Created on: Mar 16, 2020
 *      Author: gabriel
 */

#ifndef TASKS_PRIORITIES_H_
#define TASKS_PRIORITIES_H_

/*********************************************************************************
 *                              TASK PRIORITIES
 *********************************************************************************/

#define THROTTLE_TASK_PRIORITY         3
#define SENSOR_READ_TASK_PRIORITY      2
#define STATE_MACHINE_TASK_PRIORITY    1
#define DATA_LOGGING_TASK_PRIORITY     0 // same as idle task
#define WATCHDOG_TASK_PRIORITY         0 // same as idle task

// there may also be interrupt/ISR priorities for:
// CAN messages
// GPIOs (ready to drive, shutdown circuit GPIOs..)

#endif /* TASKS_PRIORITIES_H_ */
