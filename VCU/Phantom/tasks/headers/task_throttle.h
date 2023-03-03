/*
 * task_throttle.h
 *
 *  Created on: July 15, 2022
 *      Author: josh
 */

#ifndef PHANTOM_TASKS_throttle_H_
#define PHANTOM_TASKS_throttle_H_

TaskHandle_t ThrottleInit(void);
void SuspendThrottle(TaskHandle_t);

#endif
