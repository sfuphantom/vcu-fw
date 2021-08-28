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

static void taskSkeleton(void* task);

TaskHandle_t Phantom_createTask(Task* task,
					   char* const taskName,
					   uint16 stackSize,
					   uint32 taskPriority)
{
    TaskHandle_t taskHandle = NULL;
    xTaskCreate(taskSkeleton, taskName, stackSize, task, taskPriority, &taskHandle);

    // if taskHandle is NULL, configASSERT will block the program indefinitely
    configASSERT(taskHandle);

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

static void taskSkeleton(void* task)
{
    const TaskFunction_t taskFnPtr = ((Task*) task)->functionPtr;
    const TickType_t xFrequency = pdMS_TO_TICKS(((Task*) task)->frequencyMs);
    
    TickType_t xLastWakeTime = xTaskGetTickCount();
    while (1) {
        vTaskDelayUntil(&xLastWakeTime, xFrequency);

        taskFnPtr(NULL);
    }
}