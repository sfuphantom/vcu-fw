/*
 *  task_throttle.c 
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

#include "task_throttle_agent.h"    // for access to mailbox
#include "task_throttle_actor.h"    // for access to mailbox
#include "state_machine.h"      // for access to mailbox & queue
#include "task_logger.h"      



static TaskHandle_t taskHandle;

typedef struct FaultTimers_t{
    TimerHandle_t APPS1Range;
    TimerHandle_t APPS2Range;
    TimerHandle_t BSERange;
    TimerHandle_t FPDiff;
    TimerHandle_t RTDS;
} FaultTimers_t;

static TimerHandle_t AgentSoftwareWatchdog;
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

static void vThrottleActorTask(void* arg);

static float calculatePedalPercent(uint32_t pedalValue, float minValue, float maxValue);
static void UpdatePedalRangeFaultTimer(uint32_t pedalValue, uint32_t minValue, uint32_t maxValue, TimerHandle_t faultTimer);
static void UpdateAPPS10PercentFaultTimer(float Percent_APPS1_Pressed, float Percent_APPS2_Pressed);
static void CheckBrakePlausibility(uint32_t BSE_sensor_sum, float Percent_APPS1_Pressed, float Percent_APPS2_Pressed);

static void TurnOffThrottle(TimerHandle_t timer)
{
	static char buffer[64];
	snprintf(buffer, 64, "(%s): Expired after %dms", pcTimerGetTimerName(timer), xTimerGetPeriod(timer));
	LogColor(YEL, buffer);

    MCP48FV_Set_Value(0); // send throttle value to DAC driver
	LogColor(RED, "Turning off throttle");
}

#define APPS_SENSOR_TIMEOUT 1000
TaskHandle_t ThrottleInit(void)
{

    xTaskCreate(
		vThrottleActorTask,
		"ThrottleActor",
		THROTTLE_ACT_STACK_SIZE,
		0,
        THROTTLE_ACT_PRIORITY,
		&taskHandle
	);


    faultTimers.APPS1Range = Phantom_createTimer("Apps1RangeCheck", 100, NO_RELOAD, EVENT_APPS1_RANGE_FAULT, NotifyStateMachineFromTimer);
    // faultTimers.APPS2Range = Phantom_createTimer("Apps2RangeCheck", 100, NO_RELOAD, EVENT_APPS2_RANGE_FAULT, NotifyStateMachineFromTimer);
    // faultTimers.BSERange = Phantom_createTimer("BseRangeCheck", 100, NO_RELOAD, EVENT_BSE_RANGE_FAULT, NotifyStateMachineFromTimer);
    // faultTimers.FPDiff = Phantom_createTimer("FpDiffCheck", 100, NO_RELOAD, EVENT_FP_DIFF_FAULT, NotifyStateMachineFromTimer);
    // faultTimers.RTDS = Phantom_createTimer("RTDSSwitch", 2000, NO_RELOAD, 0, NotifyStateMachineFromTimer); 

    AgentSoftwareWatchdog = Phantom_createTimer("AgentSoftwareWatchdog", APPS_SENSOR_TIMEOUT, NO_RELOAD, EVENT_APPS1_RANGE_FAULT, TurnOffThrottle);

    MCP48FV_Init();

    return taskHandle;
}

static void vThrottleActorTask(void* arg)
{
	Log("Suspending thread on initialization");

    vTaskSuspend(NULL);

    while(true)
    { 
        TimerStart(AgentSoftwareWatchdog, 1);

        pedal_reading_t pedalReadings;

        if (!receivePedalReadings(&pedalReadings, portMAX_DELAY))
        {
            continue; 
        }

        float apps1PedalPercent = calculatePedalPercent(pedalReadings.fp1, PADDED_APPS1_MIN_VALUE, PADDED_APPS2_MAX_VALUE);
        float apps2PedalPercent = calculatePedalPercent(pedalReadings.fp2, PADDED_APPS2_MIN_VALUE, PADDED_APPS2_MAX_VALUE);
        float bsePedalPercent = calculatePedalPercent(pedalReadings.bse, PADDED_BSE_MIN_VALUE, PADDED_BSE_MAX_VALUE);

        // TODO: Add range fault checks

        // // check for short to GND/VCC on APPS sensor 1
        UpdatePedalRangeFaultTimer(pedalReadings.fp1, APPS1_MIN_VALUE, APPS1_MAX_VALUE, faultTimers.APPS1Range);
        // // check for short to GND/VCC on APPS sensor 2
        // UpdatePedalRangeFaultTimer(pedalReadings.fp2, APPS2_MIN_VALUE, APPS2_MAX_VALUE, APPS2RangeFaultTimer, &APPS2_RANGE_FAULT_TIMER_EXPIRED);
        // // check for short to GND/VCC on BSE
        // UpdatePedalRangeFaultTimer(pedalReadings.bse, BSE_MIN_VALUE, BSE_MAX_VALUE, BSERangeFaultTimer, &BSE_RANGE_FAULT_TIMER_EXPIRED);
        // // Check if APPS1 and APPS2 are within 10% of each other
        // UpdateAPPS10PercentFaultTimer(apps1PedalPercent, apps2PedalPercent);
        // // Check if brakes are pressed and accelerator pedal is pressed greater than or equal to 25%
        // CheckBrakePlausibility(pedalReadings.bse, apps1PedalPercent, apps2PedalPercent);

        /*********************************************************************************
         Set Throttle
        *********************************************************************************/

        float apps_percent_avg = (apps1PedalPercent + apps2PedalPercent) / 2;

        // send DAC to inverter
        int16_t throttle = 390 * apps_percent_avg + 60;        // equation mapping the averaged signals to 0->500 for the DAC driver

        // ^ this equation may need to be modified for the curtis voltage lower limit and upper limit
        // i.e. map from 0.6V (60) to 4.5V (450) or something like that, instead of 0->500 (0V -> 5V)
        MCP48FV_Set_Value(throttle); // send throttle value to DAC driver

        char buffer[32];
        snprintf(buffer, 32, "Running throttle actor");
        Log(buffer);

        #ifdef VCU_SIM_MODE
        UARTprintln("%d", throttle);
        #endif


        #ifdef VCU_SIM_MODE
        UARTprintln("0");
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

