/*
 * phantom_task.h
 *
 *  Created on: Aug. 25, 2021
 *      Author: Josh Guo
 */
#ifndef PHANTOM_TASK_H_
#define PHANTOM_TASK_H_

#include "hal_stdtypes.h"

typedef void (*TaskFunction_t)(void *);
typedef void *TaskHandle_t;

typedef struct {
    TaskFunction_t functionPtr;
    uint32 frequencyMs;
} Task;

TaskHandle_t Phantom_createTask(Task* task, char* const taskName, uint16 stackSize, uint32 taskPriority);

void Phantom_startTaskScheduler(void);
void Phantom_endTaskScheduler(void);

#endif