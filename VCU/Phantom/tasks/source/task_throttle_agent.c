/*
 *  task_throttle_agent.c
 * 
 *  Created on: July 15, 2022
 *      Author: Joshua Guo
 */
#include "phantom_task.h"
#include "phantom_queue.h"

#include "task_throttle_agent.h"
#include "task_config.h"

#include "vcu_common.h"
#include "board_hardware.h"
#include "adc.h"

typedef struct ThrottleAgent_t{
    PipeTask_t pipeline;
    pedal_reading_t readings;
    pedal_reading_t prevReadings;
}ThrottleAgent_t;

static ThrottleAgent_t footPedals;

static void vThrottleAgentTask(void* arg);

static pedal_reading_t readPedals();


/* Public API */
uint8_t receivePedalReadings(pedal_reading_t* pdreading, TickType_t wait_time_ms)
{
    return xQueueReceive(footPedals.pipeline.q, pdreading, wait_time_ms) == pdTRUE;
}

uint8_t throttleAgentInit(void)
{
    footPedals.pipeline.task = (Task) {vThrottleAgentTask, 0};

    // blocks indefinitely if task creation failed
    footPedals.pipeline.taskHandle = Phantom_createTask(&footPedals.pipeline.task, "ThrottleAgentTask", THROTTLE_AGT_STACK_SIZE, THROTTLE_AGT_PRIORITY);

    footPedals.pipeline.q = xQueueCreate(50, sizeof(pedal_reading_t));
}

static void vThrottleAgentTask(void* arg)
{

    footPedals.readings = readPedals();

    // apply a low pass filter with ALPHA of 0.5
    footPedals.readings.bse = (footPedals.readings.bse + footPedals.prevReadings.bse) >> 1;
    footPedals.readings.fp1 = (footPedals.readings.fp1 + footPedals.prevReadings.fp1) >> 1;
    footPedals.readings.fp2 = (footPedals.readings.fp2 + footPedals.prevReadings.fp2) >> 1;

    // update prev filtered values
    footPedals.prevReadings =  footPedals.readings;

    // send filtered values to mailbox (task_throttle_actor.c)
   xQueueSend(footPedals.pipeline.q, &footPedals.readings, 1000);
}

static pedal_reading_t readPedals()
{

    #if VCU_SIM_MODE == 0 
    // Get pedal readings from ADC
    adcData_t FP_data[3];
    adcStartConversion(adcREG1, adcGROUP1);
    while (!adcIsConversionComplete(adcREG1, adcGROUP1)); // prefer another method of waiting, maybe an interrupt with TaskSuspend/Resume/YIELD?
    adcGetData(adcREG1, adcGROUP1, FP_data);

    // must map each value explicitly because compiler may not have the same mem packing rules for struct as arrays
    return (pedal_reading_t) {FP_data[0].value, FP_data[1].value, FP_data[2].value};

    #else
    uint32_t data = getSimData();

    /* extract and parse the byte message. See VCU Firmware Simulation document */
    uint16_t apps1 = (data & 0b111111111111) + 1500; 
    uint16_t apps2 = ((data & 0b1111111111 << 12) >> 12) + 500; 
    
    pedal_reading_t throttleData = {
      .bse=1300,
      .fp1=apps1,
      .fp2=apps2
    };

    return throttleData;
    #endif
}
