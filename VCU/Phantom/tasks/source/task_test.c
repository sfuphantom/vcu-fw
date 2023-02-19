/*
 *  task_test.c
 *
 *  Test Task for Testing 
 * 
 *  Created on: Sept. 3, 2021
 *      Author: Joshua Guo
 */
#include "phantom_task.h"
#include "phantom_timer.h"
#include "vcu_data.h"
#include "RGB_LED.h"

#include "task_config.h"
#include "task_test.h"

// pre-define static functions first
static void vTestTask(void* arg);
static void testTimerCallback(TimerHandle_t timer);

static Task testTask;
// static TaskHandle_t taskHandle;

static TimerHandle_t testTimer;
static long testTimerCounter = 0;

void Task_testInit(void)
{
    testTask = (Task) {vTestTask, TEST_TASK_PERIOD_MS};
    // Phantom_createTask should block infinitely if task creation failed
    /*taskHandle = */Phantom_createTask(&testTask, "TestTask", TEST_TASK_STACK_SIZE, TEST_TASK_PRIORITY);

    // create recurring timers here...
    testTimer = Phantom_createTimer("testTimer", 1000, AUTO_RELOAD, &testTimerCounter, testTimerCallback);
    Phantom_startTimer(testTimer, 0);

    // so compiler won't complain about unused variables
    // taskHandle;
    // testTimer;
}

static void vTestTask(void* arg)
{
    uint8 faults = VCUData_readFaults(ALL_FAULTS);

    if (faults) {
        RGB_drive(RGB_YELLOW);
    } else {
        RGB_drive(RGB_OFF);
    }
}

static void testTimerCallback(TimerHandle_t timer)
{
    uint8 faults = VCUData_readFaults(ALL_FAULTS);
    if (!faults) {  // prefer to avoid anything with mutexes in a timer callback
        VCUData_turnOnFaults(BMS_GPIO_SEVERE_FAULT | BSPD_SEVERE_FAULT | BSE_APPS_FAULT_GROUP);
    } else {
        VCUData_turnOffFaults(BMS_GPIO_SEVERE_FAULT | BSPD_SEVERE_FAULT | BSE_APPS_FAULT_GROUP);
    }
}