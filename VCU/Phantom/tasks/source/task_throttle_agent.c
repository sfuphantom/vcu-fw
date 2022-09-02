/*
 *  task_throttle_agent.c
 * 
 *  Created on: July 15, 2022
 *      Author: Joshua Guo
 */
#include "phantom_task.h"
#include "phantom_queue.h"
// #include "phantom_timer.h"   // if you need to use timers

#include "task_throttle_agent.h"
#include "task_config.h"
// Any other .h files you need goes here...
#include "vcu_common.h"
#include "board_hardware.h"
#include "adc.h"

static Task task;
static TaskHandle_t taskHandle; 

// Any other module-scope variables goes here... (make sure they have the 'static' keyword)
static QueueHandle_t throttleMailBox = NULL;
static pedal_reading_t prevFilteredPedalRd = (pedal_reading_t) {0, 0, 0};

// Pre-define your static functions here...
static void vThrottleAgentTask(void* arg);
static void vThrottleAgentSimTask(void* arg);

QueueHandle_t ThrottleAgent_getMailBoxHandle()
{
    return throttleMailBox;
}

void Task_throttleAgentInit(void)
{
    task = (Task) {vThrottleAgentTask, THROTTLE_AGT_PERIOD_MS};

    // Phantom_createTask should block infinitely if task creation failed
    taskHandle = Phantom_createTask(&task, "ThrottleAgentTask", THROTTLE_AGT_STACK_SIZE, THROTTLE_AGT_PRIORITY);

    // any other init code you want to put goes here...
    throttleMailBox = Phantom_createMailBox(sizeof(pedal_reading_t));
}

static void vThrottleAgentTask(void* arg)
{
    // arg will always be NULL, so ignore it.

    // Get pedal readings from ADC
    adcData_t FP_data[3];
    adcStartConversion(adcREG1, adcGROUP1);
    while (!adcIsConversionComplete(adcREG1, adcGROUP1)); // prefer another method of waiting, maybe an interrupt with TaskSuspend/Resume/YIELD?
    adcGetData(adcREG1, adcGROUP1, FP_data);

    // must map each value explicitly because compiler may not have the same mem packing rules for struct as arrays
    pedal_reading_t pedalRd = (pedal_reading_t) {FP_data[0].value, FP_data[1].value, FP_data[2].value};

    // apply a low pass filter with ALPHA of 0.5
    pedalRd.bse = (pedalRd.bse + prevFilteredPedalRd.bse) >> 1;
    pedalRd.fp1 = (pedalRd.fp1 + prevFilteredPedalRd.fp1) >> 1;
    pedalRd.fp2 = (pedalRd.fp2 + prevFilteredPedalRd.fp2) >> 1;

    // update prev filtered values
    prevFilteredPedalRd = pedalRd;

    // send filtered values to mailbox
    Phantom_overwrite(throttleMailBox, &pedalRd);
}


/* SIMULATION MODE VARIANT */

void Task_throttleAgentSimInit(void)
{
    task = (Task) {vThrottleAgentSimTask, THROTTLE_AGT_SIM_PERIOD_MS};

    // Phantom_createTask should block infinitely if task creation failed
    taskHandle = Phantom_createTask(&task, "ThrottleAgentSimTask", THROTTLE_AGT_SIM_STACK_SIZE, THROTTLE_AGT_SIM_PRIORITY);

    // any other init code you want to put goes here...
    throttleMailBox = Phantom_createMailBox(sizeof(pedal_reading_t));
}

static void vThrottleAgentSimTask(void* arg)
{
    // TODO
}

// Other helper functions and callbacks goes here...
