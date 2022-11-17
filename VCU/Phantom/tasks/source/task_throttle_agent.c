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
    return Phantom_receive(footPedals.pipeline.q, pdreading, wait_time_ms) == pdTRUE;
}

void throttleAgentInit(void)
{
    footPedals.pipeline.task = (Task) {vThrottleAgentTask, THROTTLE_AGT_PERIOD_MS};

    // blocks indefinitely if task creation failed
    footPedals.pipeline.taskHandle = Phantom_createTask(&footPedals.pipeline.task, "ThrottleAgentTask", THROTTLE_AGT_STACK_SIZE, THROTTLE_AGT_PRIORITY);

    footPedals.pipeline.q = Phantom_createMailBox(sizeof(pedal_reading_t));
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

    // send filtered values to mailbox
    Phantom_overwrite(footPedals.pipeline.q, &footPedals.readings);
}

static pedal_reading_t readPedals()
{

    #ifndef SIM_MODE 
    // Get pedal readings from ADC
    adcData_t FP_data[3];
    adcStartConversion(adcREG1, adcGROUP1);
    while (!adcIsConversionComplete(adcREG1, adcGROUP1)); // prefer another method of waiting, maybe an interrupt with TaskSuspend/Resume/YIELD?
    adcGetData(adcREG1, adcGROUP1, FP_data);

    // must map each value explicitly because compiler may not have the same mem packing rules for struct as arrays
    return (pedal_reading_t) {FP_data[0].value, FP_data[1].value, FP_data[2].value};

    #else
    unsigned char* serialBuffer = getSimData();
    
    
    /* Parse serial data */
    uint16_t readings[3];
    
    int i;
    int next_index = 0;
    for (i = 0; i < 3; ++i)
    {   
        next_index = parse_data(serialBuffer, readings+i, next_index);
    }
    
    return (pedal_reading_t) {readings[0], readings[1], readings[2]};
    #endif
}

#ifdef SIM_MODE
#define MAX_SIM_DIGITS 16
static uint16_t parse_data(char* data, uint16_t* buffer, uint8_t offset)
{
    /* 
    Used to extract a number from a comma separated data set represented as a string, given a starting index  
    Args:
        data: full csv data set as a char*
        buffer: int buffer to write parsed number into
        start: index of first digit of number in the data
    */
    char* startingAddress = data + offset;

    char* tmp = startingAddress;

    char numberBuffer[MAX_SIM_DIGITS]; // each element is a digit as a char 

    int i;
    for (i = 0; ; ++i)
    {
        if( *tmp == ',' || *tmp == '\n' || *tmp == '\0')
        {
            break;
        }

        numberBuffer[i] = *tmp; // push digit onto buffer

        tmp++;
    }

    int expectedDigits = tmp - startingAddress;

    // not sure why, but it sometimes pads the next calls of this function's
    // number variable with the last few digits of previous calls
    // let's add protection by simply cutting off the digits we weren't expecting
        
    // convert from string to integer
    uint16_t value = (int)strtol(numberBuffer, (char**)NULL, 10);

    // find number of digits
    uint8_t actualDigits = 1 + log10(value);

    // divide the value by 10^digitsToCut to remove thousands, hundreds, tens place, etc
    int digitsToCut = actualDigits - expectedDigits;

    *buffer = value / (int)(pow(10, digitsToCut));

    return expectedDigits + offset + 1; // index of comma + 1
}
#endif 
