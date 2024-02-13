/*
 *  task_pedal_readings.c
 * 
 *  Created on: July 15, 2022
 *      Author: Joshua Guo
 */

#include "vcu_common.h"

/* C standard libs */
#include "stdlib.h"
#include "string.h"
#include "sci.h"
#include "stdio.h"
#include "math.h"

/* Halcogen drivers */
#include "sys_common.h"
#include "adc.h"
#include "gio.h"

/* Phantom modules */
#include "Phantom_sci.h"
#include "board_hardware.h"

/* Phantom tasks */
#include "task_pedal_readings.h"
#include "task_logger.h"
#include "task_event_handler.h"


typedef struct PedalReadings_t{
	PipeTask_t pipeline;
	pedal_reading_t readings;
	pedal_reading_t prevReadings;
}PedalReadings_t;

static PedalReadings_t footPedals;

static void vPedalReadingsTask(void* arg);
static pedal_reading_t ReadPedals();
static void SetBrakeLight(uint8_t value);

/* Public API */
uint8_t ReceivePedalReadings(pedal_reading_t* pdreading, TickType_t wait_time_ms)
{
	if (footPedals.pipeline.q)
	{
		return xQueueReceive(footPedals.pipeline.q, pdreading, wait_time_ms) == pdTRUE;
	}
	else
	{
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

	return ret == pdPASS && footPedals.pipeline.q ? footPedals.pipeline.taskHandle : NULL;
}

/* Internal Implementation */

static void vPedalReadingsTask(void* arg)
{
	Log("Starting thread");

	while(true)
	{
		footPedals.readings = ReadPedals();

		// apply a low pass filter with ALPHA of 0.5
		footPedals.readings.fp1 = (footPedals.readings.fp1 + footPedals.prevReadings.fp1) >> 1;
		footPedals.readings.fp2 = (footPedals.readings.fp2 + footPedals.prevReadings.fp2) >> 1;
		footPedals.readings.bse = (footPedals.readings.bse + footPedals.prevReadings.bse) >> 1;

		uint8_t brakelight_value = footPedals.readings.bse  > (BRAKING_THRESHOLD + HYSTERESIS);
		SetBrakeLight(brakelight_value);
	   
		// send to throttle 
		xQueueSend(footPedals.pipeline.q, &footPedals.readings, 1);

		footPedals.prevReadings =  footPedals.readings;
	}
}


static pedal_reading_t ReadPedals()
{

	#ifndef VCU_SIM_MODE 
	// delay must occur here because sim mode has no delay (frequency determined by incoming serial data)
	vTaskDelay(pdMS_TO_TICKS(100)); 

	// Get pedal readings from ADC
	adcData_t FP_data[3];
	adcStartConversion(adcREG1, adcGROUP1);
	while (!adcIsConversionComplete(adcREG1, adcGROUP1)); // prefer another method of waiting, maybe an interrupt with TaskSuspend/Resume/YIELD?
	adcGetData(adcREG1, adcGROUP1, FP_data);

	// must map each value explicitly because compiler may not have the same mem packing rules for struct as arrays
	return (pedal_reading_t) {FP_data[0].value, FP_data[1].value, FP_data[2].value};

	#else
	return getSerialPedalData();
	#endif
}

static void SetBrakeLight(uint8_t value)
{
	if (gioGetBit(BRAKE_LIGHT) != value)
	{
		char buffer[32];
		sprintf(buffer, "Setting brakelight: %d", value);
		Log(buffer);

		gioSetBit(BRAKE_LIGHT, value);
	}
}
