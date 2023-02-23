/*
 *  task_pedal_readings.c
 * 
 *  Created on: July 15, 2022
 *      Author: Joshua Guo
 */

#include "task_pedal_readings.h"
#include "task_config.h"

#include "vcu_common.h"
#include "board_hardware.h"
#include "adc.h"
#include "gio.h"

#include "Phantom_sci.h"

#include <stdlib.h>
#include "sys_common.h"
#include "string.h"
#include "sci.h"
#include "stdio.h"
#include "math.h"

#include "task_logger.h"
#include "task_event_handler.h"

#define BRAKE_LIGHT BRAKE_LIGHT_PORT, BRAKE_LIGHT_PIN

typedef struct PedalReadings_t{
    PipeTask_t pipeline;
    pedal_reading_t readings;
    pedal_reading_t prevReadings;
}PedalReadings_t;

static PedalReadings_t footPedals;

static void vPedalReadingsTask(void* arg);

static pedal_reading_t readPedals();

/* Public API */
uint8_t ReceivePedalReadings(pedal_reading_t* pdreading, TickType_t wait_time_ms)
{
    if (footPedals.pipeline.q)
    {
        return xQueueReceive(footPedals.pipeline.q, pdreading, wait_time_ms) == pdTRUE;
    }
    else
    {
        vTaskDelay(pdMS_TO_TICKS(1000));

        return 0;
    }
}

TaskHandle_t PedalReadingsInit(void)
{
	BaseType_t ret = xTaskCreate(
		vPedalReadingsTask,
		"PedalReadings",
		THROTTLE_AGT_STACK_SIZE,
		NULL,
        1,
		&footPedals.pipeline.taskHandle 
	);

    footPedals.pipeline.q = xQueueCreate(10, sizeof(pedal_reading_t));

    footPedals.prevReadings = (pedal_reading_t) {0, 0 ,0};

	return ret == pdPASS && !footPedals.pipeline.q ? footPedals.pipeline.taskHandle : NULL;
}

/* Internal Implementation */

static void SetBrakeLight(uint8_t value)
{
    char buffer[32];
    sprintf(buffer, "Setting brakelight: %d", value);
    Log(buffer);

    gioSetBit(BRAKE_LIGHT, value);
}


static void vPedalReadingsTask(void* arg)
{
	Log("Starting thread");

    while(true)
    {
        footPedals.readings = readPedals();

        // apply a low pass filter with ALPHA of 0.5
        footPedals.readings.bse = (footPedals.readings.bse + footPedals.prevReadings.bse) >> 1;
        footPedals.readings.fp1 = (footPedals.readings.fp1 + footPedals.prevReadings.fp1) >> 1;
        footPedals.readings.fp2 = (footPedals.readings.fp2 + footPedals.prevReadings.fp2) >> 1;

        // update prev filtered values
        footPedals.prevReadings =  footPedals.readings;

        uint32_t BSESensorSum = footPedals.readings.bse; 

        uint8_t brakelight_value = BSESensorSum > (BRAKING_THRESHOLD + HYSTERESIS);

        if (gioGetBit(BRAKE_LIGHT) != brakelight_value)
        {
            SetBrakeLight(brakelight_value);
        }
        
        // send filtered values to mailbox (task_throttle.c)
        xQueueSend(footPedals.pipeline.q, &footPedals.readings, 1);
    }
}

static pedal_reading_t readPedals()
{
    vTaskDelay(pdMS_TO_TICKS(20));

    #ifndef VCU_SIM_MODE 
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
    // TODO: Add the rest of the direct VCU inputs to the encoding scheme
    
    pedal_reading_t throttleData = {
      .bse=1300,
      .fp1=apps1,
      .fp2=apps2
    };

    return throttleData;
    #endif
}
