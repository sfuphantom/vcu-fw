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

// #define DEBUG    // uncomment this line for ASSERT (not talking about configASSERT) statements to work

static void taskSkeleton(void* task);

TaskHandle_t Phantom_createTask(Task* task,
					   char* const taskName,
					   uint16 stackSize,
					   uint32 taskPriority)
{
    TaskHandle_t taskHandle = NULL;
    BaseType_t result = xTaskCreate(taskSkeleton, taskName, stackSize, task, taskPriority, &taskHandle);

    if (result != pdPASS) {
        // TODO: Print to console an error message
        return -1;
    }

    // TODO: Print to console a success message
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
    const TickType_t xFrequency = ((Task*) task)->frequencyMs;
    
    TickType_t xLastWakeTime = xTaskGetTickCount();
    while (1) {
        vTaskDelayUntil(&xLastWakeTime, xFrequency);

        taskFnPtr(NULL);
    }
}