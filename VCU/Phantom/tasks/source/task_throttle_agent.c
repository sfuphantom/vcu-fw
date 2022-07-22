/*
 *  task_throttle_agent.c
 * 
 *  Created on: July 15, 2022
 *      Author: Joshua Guo
 */
#include "phantom_task.h"
#include "phantom_queue.h"
// #include "phantom_timer.h"   // if you need to use timers

// #include your_task_header_file
// Any other .h files you need goes here...

static Task task;
static TaskHandle_t taskHandle; 
static QueueHandle_t sendQueue;

// Any other module-scope variables goes here... (make sure they have the 'static' keyword)
// static TimerHandle_t yourTimerHandle;
// static long yourTimerCounter;

// Pre-define your static functions here...
static void vYourTaskFunction(void* arg);
// static void yourTimerCallbackFunc(TimerHandle_t timer);

void Task_throttleAgentSetSendQueue(QueueHandle_t queue_handle)
{
    sendQueue = queue_handle;
}

void Task_throttleAgentInit(void)
{
    // task = (Task) {vYourTaskFunction, YOUR_TASK_PERIOD_MS};

    // Phantom_createTask should block infinitely if task creation failed
    // taskHandle = Phantom_createTask(&task, TASK_NAME, YOUR_TASK_STACK_SIZE, YOUR_TASK_PRIORITY);

    // create your persistent timers here...
    // yourTimerHandle = Phantom_createTimer(YOUR_TIMER_NAME, YOUR_TIMER_PERIOD_MS, AUTO_RELOAD, &yourTimerCounter, yourTimerCallbackFunc);
    // Phantom_startTimer(yourTimerHandle);

    // any other init code you want to put goes here...
}

static void vthrottleAgentTask(void* arg)
{
    // arg will always be NULL, so ignore it.

    // your task stuff goes in here...
}

// Other helper functions and callbacks goes here...
// static void yourTimerCallbackFunc(TimerHandle_t timer) {}
