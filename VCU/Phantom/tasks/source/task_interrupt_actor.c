/*
 *  Task Template made by Joshua Guo
 * 
 *  Created on:
 *      Author: Joshua Guo, 
 */
#include "phantom_task.h"
// #include "phantom_timer.h"   // if you need to use timers
#include "phantom_queue.h"

#include "task_interrupt_actor.h"
#include "task_statemachine.h"
// Any other .h files you need goes here...
#include "vcu_common.h"
#include "os_portmacro.h"
#include "stdarg.h"

static Task task;
static TaskHandle_t taskHandle;
static QueueHandle_t interruptActionsQueue;
static QueueHandle_t interruptActorOutQueue = NULL;

// Any other module-scope variables goes here... (make sure they have the 'static' keyword)
// static TimerHandle_t yourTimerHandle;
// static long yourTimerCounter;

// Pre-define your static functions here...
static void vInterruptActorTask(void* arg);
// static void yourTimerCallbackFunc(TimerHandle_t timer);

void Task_InterruptActorSetOutQueue(QueueHandle_t queueHandle)
{
    interruptActorOutQueue = queueHandle;
}

QueueHandle_t InterruptActor_getActionQueueHandle()
{
    return interruptActionsQueue;
}

// this function should be in your header file (aside from include guards and other comments ofc)
void Task_InterruptActorInit(void)
{
    task = (Task) {vInterruptActorTask, 0};

    // Phantom_createTask should block infinitely if task creation failed
    taskHandle = Phantom_createTask(&task, "InterruptActorTask", 150, 5);

    // create your persistent timers here...
    // yourTimerHandle = Phantom_createTimer(YOUR_TIMER_NAME, YOUR_TIMER_PERIOD_MS, AUTO_RELOAD, &yourTimerCounter, yourTimerCallbackFunc);
    // Phantom_startTimer(yourTimerHandle);

    // queue length 4 for the 4 interrupts we have
    interruptActionsQueue = Phantom_createQueue(4, sizeof(isr_action_t));

    // any other init code you want to put goes here...
}

static void vInterruptActorTask(void* arg)
{
    // arg will always be NULL, so ignore it.

    // your task stuff goes in here...
    isr_action_t buffer;
    int state;

    Phantom_receive(interruptActionsQueue, &buffer, portMAX_DELAY);

    state = StateMachine_getState();

    buffer.isr_func(state, interruptActorOutQueue); // and any other params?
    
}

// Other helper functions and callbacks goes here...
// static void yourTimerCallbackFunc(TimerHandle_t timer) {}
