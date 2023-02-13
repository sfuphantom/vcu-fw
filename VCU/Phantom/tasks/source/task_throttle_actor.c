/*
 *  task_throttle.c 
 *  refactored for 2022
 * 
 *  Created on: Sept 18, 2021
 *      Author: jaypacamarra, Joshua Guo
 */
#include "phantom_task.h"
#include "phantom_timer.h"
#include "phantom_queue.h"

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
#include "task_statemachine.h"      // for access to mailbox & queue

static Task task;
static TaskHandle_t taskHandle;

static TimerHandle_t APPS1RangeFaultTimer;
static TimerHandle_t APPS2RangeFaultTimer;
static TimerHandle_t BSERangeFaultTimer;
static TimerHandle_t FPDiffFaultTimer;
static TimerHandle_t RTDSTimer;

/* For calculating throttle padding */
/* Padding will eliminate unintended range faults at 0% or 100% pedal presses */
#define PADDING_PERCENT         (0.08f) // Must be between 0.0 and 1.0
#define PADDED_BSE_MIN_VALUE    (BSE_MIN_VALUE * (1U + PADDING_PERCENT))
#define PADDED_BSE_MAX_VALUE    (BSE_MAX_VALUE * (1U - PADDING_PERCENT))
#define PADDED_APPS1_MIN_VALUE  (APPS1_MIN_VALUE * (1U + PADDING_PERCENT))
#define PADDED_APPS1_MAX_VALUE  (APPS1_MAX_VALUE * (1U - PADDING_PERCENT))
#define PADDED_APPS2_MIN_VALUE  (APPS2_MIN_VALUE * (1U + PADDING_PERCENT))
#define PADDED_APPS2_MAX_VALUE  (APPS2_MAX_VALUE * (1U - PADDING_PERCENT))

#define HYSTERESIS      (200U)

#define THROTTLE_FAULTS_MASK (APPS1_RANGE_SEVERE_FAULT | APPS2_RANGE_SEVERE_FAULT | BSE_RANGE_SEVERE_FAULT | APPS_10DIFF_SEVERE_FAULT | BSE_APPS_SIMULTANEOUS_MINOR_FAULT)
static bool APPS1_RANGE_FAULT_TIMER_EXPIRED = false;   //added by jaypacamarra
static bool APPS2_RANGE_FAULT_TIMER_EXPIRED = false;   //added by jaypacamarra
static bool BSE_RANGE_FAULT_TIMER_EXPIRED = false;     //added by jaypacamarra
static bool FP_DIFF_FAULT_TIMER_EXPIRED = false;       //added by jaypacamarra

/* Brake Light readability */
#define BRAKE_LIGHT_ON      0
#define BRAKE_LIGHT_OFF     1
static bool brake_light_state = BRAKE_LIGHT_ON; 

static bool isThrottleAvailable = false; 
static uint32_t faultCode = 0;

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


    APPS1RangeFaultTimer = Phantom_createTimer("Apps1RangeCheck", 100, NO_RELOAD, EVENT_APPS1_RANGE_FAULT, NotifyStateMachineFromTimer);
    // APPS2RangeFaultTimer = Phantom_createTimer("Apps2RangeCheck", 100, NO_RELOAD, EVENT_APPS2_RANGE_FAULT, NotifyStateMachineFromTimer);
    // BSERangeFaultTimer = Phantom_createTimer("BseRangeCheck", 100, NO_RELOAD, EVENT_BSE_RANGE_FAULT, NotifyStateMachineFromTimer);
    // FPDiffFaultTimer = Phantom_createTimer("FpDiffCheck", 100, NO_RELOAD, EVENT_FP_DIFF_FAULT, NotifyStateMachineFromTimer);
    // RTDSTimer = Phantom_createTimer("RTDSSwitch", 2000, NO_RELOAD, 0, NotifyStateMachineFromTimer); 

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
        xTimerStart(AgentSoftwareWatchdog, 1);

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
        UpdatePedalRangeFaultTimer(pedalReadings.fp1, APPS1_MIN_VALUE, APPS1_MAX_VALUE, APPS1RangeFaultTimer);
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

        isThrottleAvailable = false;
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

/** @fn bool check_Pedal_Range_Fault(void)
*   @brief Checks if the Pedal is in the right voltage range
*   @Return This function returns:
*           True -> Fault
*           False -> No Fault
*/
static bool check_Pedal_Range_Fault(uint32_t pedalValue, uint32_t minValue, uint32_t maxValue, TimerHandle_t faultTimer, bool* timerFlag) {
    bool is_there_pedal_range_fault = false;

    if (pedalValue < minValue || pedalValue > maxValue) // BSE assumed shorted to GND or shorted to VCC
    {
        if(!Phantom_isTimerActive(faultTimer))
        {
            if(!Phantom_startTimer(faultTimer, 50)) // start software timer for bse range fault
            {
                for(;;); // TODO: when watchdog is worked on, remember to also change this
            }
        }
        is_there_pedal_range_fault = *timerFlag;
    }
    else
    {
        // Stop fault timer
        Phantom_stopTimer(faultTimer, MAX_WAIT_TIME_MS);
        *timerFlag = false;
    }

    return is_there_pedal_range_fault;
}

/** @fn bool check10PercentAPPS(void)
*   @brief Checks if APPS1 and APPS2 are within 10% of each other.
*          A fault means they are not within 10% of each other.
*   @Return This function returns:
*           True -> Fault
*           False -> No Fault
*/
static bool check_10PercentAPPS_Fault(float Percent_APPS1_Pressed, float Percent_APPS2_Pressed) {
    bool is_there_10DIFF_fault = false;
    float FP_sensor_diff = fabsf(Percent_APPS2_Pressed - Percent_APPS1_Pressed); // Calculate absolute difference between APPS1 and APPS2 readings

    if (FP_sensor_diff > 0.10)
    {
        if(!Phantom_isTimerActive(FPDiffFaultTimer))
        {
            if(!Phantom_startTimer(FPDiffFaultTimer, 50)) // start software timer for apps1 range fault
            {
                for(;;); // TODO: when watchdog is worked on, remember to also change this
            }
        }
        is_there_10DIFF_fault = FP_DIFF_FAULT_TIMER_EXPIRED;
    }
    else
    {
        // Stop the fault timer
        Phantom_stopTimer(FPDiffFaultTimer, MAX_WAIT_TIME_MS);
        FP_DIFF_FAULT_TIMER_EXPIRED = false;
    }

    return is_there_10DIFF_fault;
}

/** @fn bool check_Brake_Plausibility_Fault(void)
*   @brief Checks if the brakes and accelerator are pressed at the same time.
*          A fault means the brake is pressed and the APPS reads 25% or higher.
*   @Return This function returns:
*           True -> Fault
*           False -> No Fault
*/
static bool check_Brake_Plausibility_Fault(uint32_t BSE_sensor_sum, float Percent_APPS1_Pressed, float Percent_APPS2_Pressed) {
    static bool is_there_brake_plausibility_fault = false;

    if (BSE_sensor_sum >= BRAKING_THRESHOLD + HYSTERESIS &&
        Percent_APPS1_Pressed >= 0.25 && Percent_APPS2_Pressed >= 0.25)
    {
        // Set fault
        is_there_brake_plausibility_fault = true;
    }
    else if (Percent_APPS1_Pressed < 0.05 && Percent_APPS2_Pressed < 0.05)
    {
        // APPS/Brake plausibility fault only clears if APPS returns to less than 5% pedal position
        // with or without brake operation (see EV.5.7.2) - jaypacamarra
    
        // No fault
        is_there_brake_plausibility_fault = false;
    }

    return is_there_brake_plausibility_fault;
}

static void RTDS_CALLBACK(TimerHandle_t xTimers)
{
    isThrottleAvailable = true;
}

//++ Added by Jay Pacamarra
/* Timer callback when APPS1 Range fault occurs for 100 ms*/
static void APPS1_SEVERE_RANGE_FAULT_CALLBACK(TimerHandle_t xTimers)
{
    APPS1_RANGE_FAULT_TIMER_EXPIRED = true;
}
/* Timer callback when APPS2 Range fault occurs for 100 ms*/
static void APPS2_SEVERE_RANGE_FAULT_CALLBACK(TimerHandle_t xTimers)
{
    APPS2_RANGE_FAULT_TIMER_EXPIRED = true;
}
/* Timer callback when BSE Range fault occurs for 100 ms*/
static void BSE_SEVERE_RANGE_FAULT_CALLBACK(TimerHandle_t xTimers)
{
    BSE_RANGE_FAULT_TIMER_EXPIRED = true;
}
/* Timer callback when APPS1 and APPS2 differ by 10% or more for 100 ms*/
static void FP_DIFF_SEVERE_FAULT_CALLBACK(TimerHandle_t xTimers)
{
    FP_DIFF_FAULT_TIMER_EXPIRED = true;
}
//++ Added by Jay Pacamarra
