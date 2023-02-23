/*
 *  task_task_throttle.c 
 *  refactored for 2022
 * 
 *  Created on: Sept 18, 2021
 *      Author: jaypacamarra, Joshua Guo
 */

#include "phantom_timer.h"

#include <math.h>           // for fabsf function
#include "hal_stdtypes.h"
#include "gio.h"
#include <adc.h>
#include "MCP48FV_DAC_SPI.h"

// #include "vcu_data.h"       // deprecated
#include "vcu_common.h"
#include "board_hardware.h"
#include "Phantom_sci.h"

#include "task_config.h"

#include "task_pedal_readings.h"    // for access to mailbox
#include "task_throttle.h"    // for access to mailbox
#include "state_machine.h"      // for access to mailbox & queue
#include "task_logger.h"      

#define APPS_SENSOR_TIMEOUT 1000

static TaskHandle_t taskHandle;

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

/* For calculating throttle padding */
/* Padding will eliminate unintended range faults at 0% or 100% pedal presses */
#define PADDING_PERCENT         (0.08f) // Must be between 0.0 and 1.0
#define PADDED_BSE_MIN_VALUE    (BSE_MIN_VALUE * (1U + PADDING_PERCENT))
#define PADDED_BSE_MAX_VALUE    (BSE_MAX_VALUE * (1U - PADDING_PERCENT))
#define PADDED_APPS1_MIN_VALUE  (APPS1_MIN_VALUE * (1U + PADDING_PERCENT))
#define PADDED_APPS1_MAX_VALUE  (APPS1_MAX_VALUE * (1U - PADDING_PERCENT))
#define PADDED_APPS2_MIN_VALUE  (APPS2_MIN_VALUE * (1U + PADDING_PERCENT))
#define PADDED_APPS2_MAX_VALUE  (APPS2_MAX_VALUE * (1U - PADDING_PERCENT))

static void vThrottleTask(void* arg);

static float calculatePedalPercent(uint32_t pedalValue, float minValue, float maxValue);
static void UpdatePedalRangeFaultTimer(uint32_t pedalValue, uint32_t minValue, uint32_t maxValue, TimerHandle_t faultTimer);
static void UpdateAPPS10PercentFaultTimer(float Percent_APPS1_Pressed, float Percent_APPS2_Pressed);
static void CheckBrakePlausibility(uint32_t BSE_sensor_sum, float Percent_APPS1_Pressed, float Percent_APPS2_Pressed);

void SuspendThrottle(TaskHandle_t self)
{
    #ifndef VCU_SIM_MODE
    if (self == taskHandle)
    {
        MCP48FV_Set_Value(0); // send throttle value to DAC driver
        LogColor(RED, "Turning off throttle");

        vTaskSuspend(self);
    }
    #else
    LogColor(GRN, "Cannot suspend throttle in VCU Sim Mode.");
    #endif
}

static void CheckFaultConditions(const pedal_reading_t* pedalReadings, float apps1_percent, float apps2_percent)
{
    // check for short to GND/VCC on APPS sensor 1
    UpdatePedalRangeFaultTimer(pedalReadings->fp1, APPS1_MIN_VALUE, APPS1_MAX_VALUE, faultTimers.APPS1Range);

    // TODO: Add range fault checks
    // check for short to GND/VCC on APPS sensor 2
    UpdatePedalRangeFaultTimer(pedalReadings->fp2, APPS2_MIN_VALUE, APPS2_MAX_VALUE, faultTimers.APPS2Range);
    // check for short to GND/VCC on BSE
    UpdatePedalRangeFaultTimer(pedalReadings->bse, BSE_MIN_VALUE, BSE_MAX_VALUE, faultTimers.BSERange);
    // Check if APPS1 and APPS2 are within 10% of each other
    UpdateAPPS10PercentFaultTimer(apps1_percent, apps2_percent);
    // Check if brakes are pressed and accelerator pedal is pressed greater than or equal to 25%
    CheckBrakePlausibility(pedalReadings->bse, apps1_percent, apps2_percent);
}

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

static void vThrottleTask(void* arg)
{
    SuspendThrottle(taskHandle);

    while(true)
    { 
        pedal_reading_t pedalReadings;

        if (!ReceivePedalReadings(&pedalReadings, APPS_SENSOR_TIMEOUT))
        {
            SuspendThrottle(taskHandle);

            continue; 
        }

		/* Get pedal percentages */
        float apps1PedalPercent = calculatePedalPercent(pedalReadings.fp1, PADDED_APPS1_MIN_VALUE, PADDED_APPS2_MAX_VALUE);
        float apps2PedalPercent = calculatePedalPercent(pedalReadings.fp2, PADDED_APPS2_MIN_VALUE, PADDED_APPS2_MAX_VALUE);

        CheckFaultConditions(&pedalReadings, apps1PedalPercent, apps2PedalPercent);

        float apps_percent_avg = (apps1PedalPercent + apps2PedalPercent) / 2;

        /* Set throttle voltage value */ 
        int16_t throttle = MAP_PERCENT_TO_VOLTAGE(apps_percent_avg);
        MCP48FV_Set_Value(throttle); 

        #ifdef VCU_SIM_MODE
        char buffer[32];
        snprintf(buffer, 32, "throttle=%d", throttle);
        Log(buffer);
        #endif
    }
}

/** @fn void calculatePedalPercents(void)
*   @brief Calculates the percent pressed of the brake pedal
*          and the accelerator pedal
*   @Return This function does not return anything, it only
*           updates the VCU data structure
*/
static float calculatePedalPercent(uint32_t pedalValue, float minValue, float maxValue) {
    if (pedalValue < minValue)
        return 0.0F;
    else if (pedalValue > maxValue)
        return 1.0F;
    else
        return (pedalValue - minValue) / (maxValue - minValue);
}

/** @fn bool UpdatePedalRangeFaultTimer(void)
*   @brief Checks if the Pedal is in the right voltage range
*   @Return This function returns:
*           True -> Fault
*           False -> No Fault
*/
static void UpdatePedalRangeFaultTimer(uint32_t pedalValue, uint32_t minValue, uint32_t maxValue, TimerHandle_t faultTimer) {

    if (pedalValue < minValue || pedalValue > maxValue) // BSE assumed shorted to GND or shorted to VCC
    {
        Phantom_startTimer(faultTimer, 50); // start software timer for bse range fault
    }
    else
    {
        Phantom_stopTimer(faultTimer, portMAX_DELAY);
    }
}

/** @fn bool check10PercentAPPS(void)
*   @brief Checks if APPS1 and APPS2 are within 10% of each other.
*          A fault means they are not within 10% of each other.
*   @Return This function returns:
*           True -> Fault
*           False -> No Fault
*/
static void UpdateAPPS10PercentFaultTimer(float Percent_APPS1_Pressed, float Percent_APPS2_Pressed) {

    float FP_sensor_diff = fabsf(Percent_APPS2_Pressed - Percent_APPS1_Pressed); // Calculate absolute difference between APPS1 and APPS2 readings

    if (FP_sensor_diff > 0.10)
    {
        Phantom_startTimer(faultTimers.FPDiff, 50); 
    }
    else
    {
        Phantom_stopTimer(faultTimers.FPDiff, portMAX_DELAY);
    }
}

/** @fn bool CheckBrakePlausibility(void)
*   @brief Checks if the brakes and accelerator are pressed at the same time.
*          A fault means the brake is pressed and the APPS reads 25% or higher.
*   @Return This function returns:
*           True -> Fault
*           False -> No Fault
*/
static void CheckBrakePlausibility(uint32_t BSE_sensor_sum, float Percent_APPS1_Pressed, float Percent_APPS2_Pressed) {

    if (BSE_sensor_sum >= BRAKING_THRESHOLD + HYSTERESIS &&
        Percent_APPS1_Pressed >= 0.25 && Percent_APPS2_Pressed >= 0.25)
    {
        NotifyStateMachine(EVENT_BRAKE_PLAUSIBILITY_FAULT);
    }
    else if (Percent_APPS1_Pressed < 0.05 && Percent_APPS2_Pressed < 0.05)
    {
        // APPS/Brake plausibility fault only clears if APPS returns to less than 5% pedal position
        // with or without brake operation (see EV.5.7.2) - jaypacamarra
        NotifyStateMachine(EVENT_BRAKE_PLAUSIBILITY_CLEARED);
    }
}

