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

#include "task_config.h"

#include "task_throttle_agent.h"    // for access to mailbox
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
static bool brake_light_state = BRAKE_LIGHT_ON; // Default = BRAKE_LIGHT_ON

static QueueHandle_t throttleAgentMailBox = NULL;
static bool isThrottleAvailable = false; // is this necessary anymore? TODO
static uint32_t faultCode = 0;

static void vThrottleActorTask(void* arg);

static float calculatePedalPercent(uint32_t pedalValue, float minValue, float maxValue);
static bool check_Pedal_Range_Fault(uint32_t pedalValue, uint32_t minValue, uint32_t maxValue, TimerHandle_t faultTimer, bool* timerFlag);
static bool check_10PercentAPPS_Fault(float Percent_APPS1_Pressed, float Percent_APPS2_Pressed);
static bool check_Brake_Plausibility_Fault(uint32_t BSE_sensor_sum, float Percent_APPS1_Pressed, float Percent_APPS2_Pressed);

static void RTDS_CALLBACK(TimerHandle_t xTimers);
static void APPS1_SEVERE_RANGE_FAULT_CALLBACK(TimerHandle_t xTimers);
static void APPS2_SEVERE_RANGE_FAULT_CALLBACK(TimerHandle_t xTimers);
static void BSE_SEVERE_RANGE_FAULT_CALLBACK(TimerHandle_t xTimers);
static void FP_DIFF_SEVERE_FAULT_CALLBACK(TimerHandle_t xTimers);

void Task_throttleActorInit(void)
{
    task = (Task) {vThrottleActorTask, 0};

    // Phantom_createTask should block infinitely if task creation failed
    taskHandle = Phantom_createTask(&task, "ThrottleActorTask", THROTTLE_ACT_STACK_SIZE, THROTTLE_ACT_PRIORITY);

    APPS1RangeFaultTimer = Phantom_createTimer("APPS1_RANGE_FAULT_Timer", 100, NO_RELOAD, NULL, APPS1_SEVERE_RANGE_FAULT_CALLBACK);
    APPS2RangeFaultTimer = Phantom_createTimer("APPS2_RANGE_FAULT_Timer", 100, NO_RELOAD, NULL, APPS2_SEVERE_RANGE_FAULT_CALLBACK);
    BSERangeFaultTimer = Phantom_createTimer("BSE_RANGE_FAULT_Timer", 100, NO_RELOAD, NULL, BSE_SEVERE_RANGE_FAULT_CALLBACK);
    FPDiffFaultTimer = Phantom_createTimer("FP_DIFF_FAULT_Timer", 100, NO_RELOAD, NULL, FP_DIFF_SEVERE_FAULT_CALLBACK);

    RTDSTimer = Phantom_createTimer("RTDS_Timer", 2000, NO_RELOAD, NULL, RTDS_CALLBACK);
    // any other init code you want to put goes here...

    (void) taskHandle;
}

static void vThrottleActorTask(void* arg)
{
    // arg will always be NULL, so ignore it.

    // Get pedal readings
    pedal_reading_t pedalReadings = (pedal_reading_t) {0, 0, 0};

    ThrottleAgent_receive(&pedalReadings, portMAX_DELAY)
    Phantom_receive(throttleAgentMailBox, &pedalReadings, portMAX_DELAY);

    float apps1PedalPercent = calculatePedalPercent(pedalReadings.fp1, PADDED_APPS1_MIN_VALUE, PADDED_APPS2_MAX_VALUE);
    float apps2PedalPercent = calculatePedalPercent(pedalReadings.fp2, PADDED_APPS2_MIN_VALUE, PADDED_APPS2_MAX_VALUE);
    float bsePedalPercent = calculatePedalPercent(pedalReadings.bse, PADDED_BSE_MIN_VALUE, PADDED_BSE_MAX_VALUE);

    // check for short to GND/VCC on APPS sensor 1
    bool apps1Fault = check_Pedal_Range_Fault(pedalReadings.fp1, APPS1_MIN_VALUE, APPS1_MAX_VALUE, APPS1RangeFaultTimer, &APPS1_RANGE_FAULT_TIMER_EXPIRED);
    // check for short to GND/VCC on APPS sensor 2
    bool apps2Fault = check_Pedal_Range_Fault(pedalReadings.fp2, APPS2_MIN_VALUE, APPS2_MAX_VALUE, APPS2RangeFaultTimer, &APPS2_RANGE_FAULT_TIMER_EXPIRED);
    // check for short to GND/VCC on BSE
    bool bseFault   = check_Pedal_Range_Fault(pedalReadings.bse, BSE_MIN_VALUE, BSE_MAX_VALUE, BSERangeFaultTimer, &BSE_RANGE_FAULT_TIMER_EXPIRED);
    // Check if APPS1 and APPS2 are within 10% of each other
    bool diffFault  = check_10PercentAPPS_Fault(apps1PedalPercent, apps2PedalPercent);
    // Check if brakes are pressed and accelerator pedal is pressed greater than or equal to 25%
    bool simulFault = check_Brake_Plausibility_Fault(pedalReadings.bse, apps1PedalPercent, apps2PedalPercent);

    // Fill the unrelevant bits with flags from vcu data
    uint32_t currentFaults = 0;

    currentFaults |= ((apps1Fault && 1) * APPS1_RANGE_SEVERE_FAULT);
    currentFaults |= (apps2Fault && 1) * APPS2_RANGE_SEVERE_FAULT;
    currentFaults |= (bseFault && 1) * BSE_RANGE_SEVERE_FAULT;
    currentFaults |= (diffFault && 1) * APPS_10DIFF_SEVERE_FAULT;
    currentFaults |= (simulFault && 1) * BSE_APPS_SIMULTANEOUS_MINOR_FAULT;

    faultCode = currentFaults;

    /*********************************************************************************
      brake light
     *********************************************************************************/
    uint32_t BSESensorSum = pedalReadings.bse;
    if (brake_light_state == BRAKE_LIGHT_OFF && BSESensorSum > (BRAKING_THRESHOLD + HYSTERESIS))
    {
        // turn on brake lights
        gioSetBit(BRAKE_LIGHT_PORT, BRAKE_LIGHT_PIN, BRAKE_LIGHT_ON);
        // update brake light enable in the vcu data structure
        brake_light_state = BRAKE_LIGHT_ON;
    }
    else if (brake_light_state == BRAKE_LIGHT_ON && BSESensorSum < (BRAKING_THRESHOLD - HYSTERESIS))
    {
        // turn off brake lights
        gioSetBit(BRAKE_LIGHT_PORT, BRAKE_LIGHT_PIN, BRAKE_LIGHT_OFF);
        // update brake light enable in the vcu data structure
        brake_light_state = BRAKE_LIGHT_OFF;
    }



    /*********************************************************************************
      Set Throttle
     *********************************************************************************/
    State state = StateMachine_getState();

    // debugging - jaypacamarra
    // manually setting state to RUNNING and setting THROTTLE_AVAILABLE to true to test DAC - jaypacamarra
    state = RUNNING;
    isThrottleAvailable = true;

    if (state == RUNNING && isThrottleAvailable)
    {
        // update throttle percentage in vcu data structure
        float apps_percent_avg = (apps1PedalPercent + apps2PedalPercent) / 2;

        // VCUData_setThrottlePercentage(apps_percent_avg);
        // send DAC to inverter
        int16_t throttle = 390 * apps_percent_avg + 60;        // equation mapping the averaged signals to 0->500 for the DAC driver
        // ^ this equation may need to be modified for the curtis voltage lower limit and upper limit
        // i.e. map from 0.6V (60) to 4.5V (450) or something like that, instead of 0->500 (0V -> 5V)
        MCP48FV_Set_Value(throttle); // send throttle value to DAC driver
    }
    else
    {
        // send 0 to DAC
        MCP48FV_Set_Value(0);
        isThrottleAvailable = false;
    }
}

// Other helper functions and callbacks goes here...

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
