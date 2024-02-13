/*
 *  task_task_throttle.c 
 *  refactored for 2022
 * 
 *  Created on: Sept 18, 2021
 *      Author: jaypacamarra, Joshua Guo
 */

#include "vcu_common.h"

/* C standard libs */
#include "math.h"           

/* Halcogen drivers */
#include "hal_stdtypes.h"
#include "gio.h"
#include "adc.h"

/* Phantom modules*/
#include "board_hardware.h"
#include "MCP48FV_DAC_SPI.h"
#include "phantom_timer.h"
#include "Phantom_sci.h"

/* Phantom tasks */
#include "task_pedal_readings.h"    
#include "task_throttle.h"    
#include "state_machine.h"    
#include "task_logger.h"      


/* For calculating throttle padding */
/* Padding will eliminate unintended range faults at 0% or 100% pedal presses */
#define PADDING_PERCENT         (0.08f) // Must be between 0.0 and 1.0
#define PADDED_BSE_MIN_VALUE    (BSE_MIN_VALUE * (1U + PADDING_PERCENT))
#define PADDED_BSE_MAX_VALUE    (BSE_MAX_VALUE * (1U - PADDING_PERCENT))
#define PADDED_APPS1_MIN_VALUE  (APPS1_MIN_VALUE * (1U + PADDING_PERCENT))
#define PADDED_APPS1_MAX_VALUE  (APPS1_MAX_VALUE * (1U - PADDING_PERCENT))
#define PADDED_APPS2_MIN_VALUE  (APPS2_MIN_VALUE * (1U + PADDING_PERCENT))
#define PADDED_APPS2_MAX_VALUE  (APPS2_MAX_VALUE * (1U - PADDING_PERCENT))

#define APPS_SENSOR_TIMEOUT 1000

// ^ this equation may need to be modified for the curtis voltage lower limit and upper limit
// i.e. map from 0.6V (60) to 4.5V (450) or something like that, instead of 0->500 (0V -> 5V)
#define MAP_PERCENT_TO_VOLTAGE(x) 390 * apps_percent_avg + 60  // equation mapping the averaged signals to 0->500 for the DAC driver

typedef struct FaultTimers_t{
    TimerHandle_t APPS1Range;
    TimerHandle_t APPS2Range;
    TimerHandle_t BSERange;
    TimerHandle_t FPDiff;
    TimerHandle_t RTDS;
} FaultTimers_t;

static FaultTimers_t faultTimers;
static TaskHandle_t taskHandle;


static void vThrottleTask(void* arg);
static void CheckFaultConditions(const pedal_reading_t* pedalReadings, float apps1_percent, float apps2_percent);
static float CalculatePedalPercent(uint32_t pedalValue, float minValue, float maxValue);
static bool UpdatePedalRangeFaultTimer(uint32_t pedalValue, uint32_t minValue, uint32_t maxValue, TimerHandle_t faultTimer);
static bool UpdateAPPS10PercentFaultTimer(float Percent_APPS1_Pressed, float Percent_APPS2_Pressed);
static void CheckBrakePlausibility(uint32_t BSE_sensor_sum, float Percent_APPS1_Pressed, float Percent_APPS2_Pressed);


/* Public API*/

TaskHandle_t ThrottleInit(void)
{
    BaseType_t ret = xTaskCreate(
		vThrottleTask,
		"Throttle",
		THROTTLE_ACT_STACK_SIZE,
		0,
        THROTTLE_ACT_PRIORITY,
		&taskHandle
	);

    faultTimers.APPS1Range = Phantom_createTimer("Apps1RangeCheck", 100, NO_RELOAD, EVENT_APPS1_RANGE_FAULT, NotifyStateMachineFromTimer);
    faultTimers.APPS2Range = Phantom_createTimer("Apps2RangeCheck", 100, NO_RELOAD, EVENT_APPS2_RANGE_FAULT, NotifyStateMachineFromTimer);
    faultTimers.BSERange = Phantom_createTimer("BseRangeCheck", 100, NO_RELOAD, EVENT_BSE_RANGE_FAULT, NotifyStateMachineFromTimer);
    faultTimers.FPDiff = Phantom_createTimer("FpDiffCheck", 100, NO_RELOAD, EVENT_FP_DIFF_FAULT, NotifyStateMachineFromTimer);
    faultTimers.RTDS = Phantom_createTimer("RTDSSwitch", 2000, NO_RELOAD, 0, NotifyStateMachineFromTimer); 

    MCP48FV_Init();

    return ret == pdPASS ? taskHandle : NULL;
}


void SuspendThrottle(TaskHandle_t self)
{
    // #ifndef VCU_SIM_MODE
    // if (self == taskHandle)
    // {
    //     MCP48FV_Set_Value(0); // send throttle value to DAC driver
    //     LogColor(RED, "Turning off throttle");

	// 	LogColor(RED, "Suspending throttle task.");
    //     vTaskSuspend(self);
    // }
    // #else
    // LogColor(GRN, "Cannot suspend throttle in VCU Sim Mode.");
    // #endif
}


/* Internal implementation */

static void vThrottleTask(void* arg)
{
    // suspend on startup
    SuspendThrottle(taskHandle);

    static pedal_reading_t pedalReadings;

    while(true)
    { 
        if (!ReceivePedalReadings(&pedalReadings, APPS_SENSOR_TIMEOUT))
        {
            SuspendThrottle(taskHandle);

            continue; 
        }

        float apps1PedalPercent = CalculatePedalPercent(pedalReadings.fp1, PADDED_APPS1_MIN_VALUE, PADDED_APPS1_MAX_VALUE);
        float apps2PedalPercent = CalculatePedalPercent(pedalReadings.fp2, PADDED_APPS2_MIN_VALUE, PADDED_APPS2_MAX_VALUE);

        // CheckFaultConditions(&pedalReadings, apps1PedalPercent, apps2PedalPercent);

        /* Set throttle voltage value */ 
        float apps_percent_avg = (apps1PedalPercent + apps2PedalPercent) / 2;
        int16_t throttle = MAP_PERCENT_TO_VOLTAGE(apps_percent_avg);

        MCP48FV_Set_Value(throttle); 

        // output to the screen for 
        char buffer[32];

        // sprintf(buffer, "%d\n\r", pedalReadings.fp1);
        sprintf(buffer, "%d %d %d\n\r", pedalReadings.fp1, pedalReadings.fp2, throttle);
		UARTSend(PC_UART, buffer);
    }
}


static void CheckFaultConditions(const pedal_reading_t* pedalReadings, float apps1_percent, float apps2_percent)
{
    // check for short to GND/VCC on APPS sensor 1
    UpdatePedalRangeFaultTimer(pedalReadings->fp1, APPS1_MIN_VALUE, APPS1_MAX_VALUE, faultTimers.APPS1Range);

    // check for short to GND/VCC on APPS sensor 2
    UpdatePedalRangeFaultTimer(pedalReadings->fp2, APPS2_MIN_VALUE, APPS2_MAX_VALUE, faultTimers.APPS2Range);

    // check for short to GND/VCC on BSE
    UpdatePedalRangeFaultTimer(pedalReadings->bse, BSE_MIN_VALUE, BSE_MAX_VALUE, faultTimers.BSERange);

    // check if APPS1 and APPS2 are within 10% of each other
    UpdateAPPS10PercentFaultTimer(apps1_percent, apps2_percent);

    // check if brakes are pressed and accelerator pedal is pressed greater than or equal to 25%
    CheckBrakePlausibility(pedalReadings->bse, apps1_percent, apps2_percent);
}


static float CalculatePedalPercent(uint32_t pedalValue, float minValue, float maxValue) {
    if (pedalValue < minValue)
        return 0.0F;
    else if (pedalValue > maxValue)
        return 1.0F;
    else
        return (pedalValue - minValue) / (maxValue - minValue);
}


static bool UpdatePedalRangeFaultTimer(uint32_t pedalValue, uint32_t minValue, uint32_t maxValue, TimerHandle_t faultTimer) {

	bool ret = false;

    if (pedalValue < minValue || pedalValue > maxValue) // BSE assumed shorted to GND or shorted to VCC
    {
        Phantom_startTimer(faultTimer, 50); // start software timer for bse range fault

        ret = true;
    }
    else
    {
        Phantom_stopTimer(faultTimer, 50);
    }

    return ret;
}


static bool UpdateAPPS10PercentFaultTimer(float Percent_APPS1_Pressed, float Percent_APPS2_Pressed) {

	bool ret = false;

    float FP_sensor_diff = fabsf(Percent_APPS2_Pressed - Percent_APPS1_Pressed); 

    if (FP_sensor_diff > 0.10)
    {
        Phantom_startTimer(faultTimers.FPDiff, 50); 

        ret = true;
    }
    else
    {
        Phantom_stopTimer(faultTimers.FPDiff, portMAX_DELAY);
    }

    return ret;
}


static void CheckBrakePlausibility(uint32_t BSE_sensor_sum, float Percent_APPS1_Pressed, float Percent_APPS2_Pressed) {

    if (BSE_sensor_sum >= BRAKING_THRESHOLD + HYSTERESIS &&
        Percent_APPS1_Pressed >= 0.25 && Percent_APPS2_Pressed >= 0.25)
    {
        // brakes and accelerator are both pressed
        NotifyStateMachine(EVENT_BRAKE_PLAUSIBILITY_FAULT);
    }
    else if (Percent_APPS1_Pressed < 0.05 && Percent_APPS2_Pressed < 0.05)
    {
        // APPS/Brake plausibility fault only clears if APPS returns to less than 5% pedal position
        // with or without brake operation (see EV.5.7.2) - jaypacamarra
        NotifyStateMachine(EVENT_BRAKE_PLAUSIBILITY_CLEARED);
    }
}

