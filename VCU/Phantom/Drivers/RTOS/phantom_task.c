/*
 * phantom_task.c
 *
 *  Created on: Aug. 25, 2021
 *      Author: Josh Guo
 */
#include "sys_common.h"
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "os_task.h"

#include "phantom_task.h"

static void periodicTaskSkeleton(void* task);
static void continuousTaskSkeleton(void* task);

TaskHandle_t Phantom_createTask(Task* task,
					   char* const taskName,
					   uint16 stackSize,
					   uint32 taskPriority)
{
    TaskHandle_t taskHandle = NULL;

    TaskFunction_t taskSkeleton = periodicTaskSkeleton;
    if (task->frequencyMs == 0) {
        taskSkeleton = continuousTaskSkeleton;
    }

    xTaskCreate(taskSkeleton, taskName, stackSize, task, taskPriority, &taskHandle);

    return taskHandle;
}

void Phantom_startTaskScheduler(void)
{
    vTaskStartScheduler();
}

void Phantom_endTaskScheduler(void)
{
    vTaskEndScheduler();
}

void Phantom_suspendTask(TaskHandle_t taskHandle)
{
    vTaskSuspend(taskHandle);
}

void Phantom_resumeTask(TaskHandle_t taskHandle)
{
    vTaskResume(taskHandle);
}

static void periodicTaskSkeleton(void* task)
{
    const TaskFunction_t taskFnPtr = ((Task*) task)->functionPtr;
    const TickType_t xFrequency = pdMS_TO_TICKS(((Task*) task)->frequencyMs);
    
    TickType_t xLastWakeTime = xTaskGetTickCount();
    while (1) {
        vTaskDelayUntil(&xLastWakeTime, xFrequency);

        taskFnPtr(NULL);
    }
}

static void continuousTaskSkeleton(void* task)
{
    const TaskFunction_t taskFnPtr = ((Task*) task)->functionPtr;

    while (1) {
        taskFnPtr(NULL);
    }
}
