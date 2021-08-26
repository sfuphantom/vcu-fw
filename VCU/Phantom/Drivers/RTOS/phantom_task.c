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

#define MAX_NUMBER_OF_TASKS 7
static TaskHandle_t taskHandles[MAX_NUMBER_OF_TASKS];
static int taskHandlesSize = 0;

int Phantom_createTask(TaskFunction_t taskFnPtr,
					   char* const taskName,
					   uint16 stackSize,
					   void* const fnParameters,
					   uint32 taskPriority)
{
    if (taskHandlesSize >= MAX_NUMBER_OF_TASKS) {
        // TODO: Print to console an error message
        return -1;
    }

    BaseType_t result = xTaskCreate(taskFnPtr, taskName, stackSize, fnParameters, taskPriority, &taskHandles[taskHandlesSize]);
    if (result != pdPASS) {
        // TODO: Print to console an error message
        return -1;
    }

    if (taskHandles[taskHandlesSize]) {
        // TODO: Print to console an error message
        return -1;
    }

    // TODO: Print to console a success message
    return taskHandlesSize++;
}