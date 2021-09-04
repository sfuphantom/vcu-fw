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
#include "priorities.h"

#include "task_test.h"

// pre-define static functions first
static void vTestTask(void* arg);
static void testTimerCallback(TimerHandle_t timer);

static Task testTask = {vTestTask, TEST_TASK_PERIOD_MS};
static TaskHandle_t taskHandle;

static TimerHandle_t testTimer;
static long testTimerCounter = 0;

void testTaskInit(void)
{
    // Phantom_createTask should block infinitely if task creation failed
    taskHandle = Phantom_createTask(&testTask, "TestTask", TEST_TASK_STACK_SIZE, TEST_TASK_PRIORITY);

    // create recurring timers here...
    testTimer = Phantom_createTimer("testTimer", 1000, true, &testTimerCounter, testTimerCallback);
}

static void testTimerCallback(TimerHandle_t timer)
{
    // do something to VCU Data
}

static void vTestTask(void* arg)
{
    // check VCU Data for changes
}