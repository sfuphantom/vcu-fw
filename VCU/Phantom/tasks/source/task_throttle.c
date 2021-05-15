/*
 * task_throttle.c
 *
 *  Created on: Mar 31, 2020
 *      Author: gabriel
 */

#include <Phantom/Drivers/Driver_controls/throttle.h>   // Added THROTTLE DRIVER - jaypacamarra (5/5/2021)
#include "adc.h"
#include "FreeRTOS.h"
#include "os_queue.h"
#include "os_semphr.h"
#include "os_task.h"
#include "os_timer.h"
#include "FreeRTOSConfig.h"

#include "stdlib.h" // stdlib.h has ltoa() which we use for our simple SCI printing routine.

#include "board_hardware.h" // contains hardware defines for specific board used (i.e. VCU or launchpad)

#include "MCP48FV_DAC_SPI.h"
#include "Phantom_sci.h"
#include "gio.h"

#include "vcu_data.h"


extern State state;
extern TimerHandle_t xTimers[];                 //jaypacamarra

/*********************************************************************************
  ADC FOOT PEDAL AND APPS STUFF (SHOULD GENERALIZE THIS)
 *********************************************************************************/
// variables to store foot pedal adc values
extern unsigned int volatile BSE_sensor_sum;
extern unsigned int volatile FP_sensor_1_sum;
extern unsigned int volatile FP_sensor_2_sum;

// timer started is false to begin with
extern bool APPS1_range_fault_timer_started = false;
extern bool APPS2_range_fault_timer_started = false;
extern bool BSE_range_fault_timer_started   = false;
extern bool FP_diff_fault_timer_started     = false;

// To store percent pressed of foot pedals
extern float Percent_APPS1_Pressed;
extern float Percent_APPS2_Pressed;
extern float Percent_BSE_Pressed;

extern bool THROTTLE_AVAILABLE;

extern data *VCUDataPtr;

//++ Added by jaypacamarra for execution time measurement

#include <Phantom/support/execution_timer.h>

#define CPU_CLOCK_MHz (float) 160.0  // System clock is 180 Mhz, RTI Clock is 80 Mhz
volatile unsigned long cycles_PMU_start;    // CPU cycle count at start
volatile float time_PMU_code_uSecond;   // the calculated time in uSecond.

//++ Added by jaypacamarra for execution time measurement

bool previous_brake_light_state = 1;    // Default = 1. Holds previous brake light state, 1 = ON, 0 = OFF - jaypacamarra
uint16_t hysteresis = 200;          // change this to tweak hysteresis threshhold - jaypacamarra

// for calculating throttle to the DAC
float apps_percent_avg;
unsigned int throttle;

/***********************************************************
 * @function                - vThrottleTask
 *
 * @brief                   - This task reads the APPS, performs signal plausibility, and controls the inverter through a DAC
 *
 * @param[in]               - pvParameters
 *
 * @return                  - None
 * @Note                    - None
 ***********************************************************/
void vThrottleTask(void *pvParameters)
{

    TickType_t xLastWakeTime;         // will hold the timestamp at which the task was last unblocked
    const TickType_t xFrequency = 10; // task frequency in ms

    // Initialize the xLastWakeTime variable with the current time;
    xLastWakeTime = xTaskGetTickCount();

    // timer started is false to begin with
    APPS1_range_fault_timer_started = false;
    APPS2_range_fault_timer_started = false;
    BSE_range_fault_timer_started   = false;
    FP_diff_fault_timer_started     = false;

    while (true)
    {
        // Wait for the next cycle
        vTaskDelayUntil(&xLastWakeTime, xFrequency);

#ifdef PMU_CYCLE
        //Start timer.
        cycles_PMU_start = timer_Start();
        gioSetBit(gioPORTA, 5, 1);
#endif

        // Get pedal readings
        getPedalReadings();

        // Update pedal inputs in vcu data structure
        VCUDataPtr->AnalogIn.APPS1_percentage.value = Percent_APPS1_Pressed;
        VCUDataPtr->AnalogIn.APPS2_percentage.value = Percent_APPS2_Pressed;
        VCUDataPtr->AnalogIn.BSE_percentage.value = Percent_BSE_Pressed;

        // Signal conditioning - jaypacamarra
        applyLowPassFilter();

        /*********************************************************************************
          check for short to GND/VCC on APPS sensor 1
         *********************************************************************************/
        if(check_APPS1_Range_Fault())
        {
            VCUDataPtr->DigitalVal.APPS1_SEVERE_RANGE_FAULT = 1;
        }
        else
        {
            VCUDataPtr->DigitalVal.APPS1_SEVERE_RANGE_FAULT = 0;
        }

        /*********************************************************************************
          check for short to GND/VCC on APPS sensor 2
         *********************************************************************************/
        if(check_APPS2_Range_Fault())
        {
            VCUDataPtr->DigitalVal.APPS2_SEVERE_RANGE_FAULT = 1;
        }
        else
        {
            VCUDataPtr->DigitalVal.APPS2_SEVERE_RANGE_FAULT = 0;
        }

        /*********************************************************************************
          check for short to GND/VCC on BSE
         *********************************************************************************/
        if(check_BSE_Range_Fault())
        {
            VCUDataPtr->DigitalVal.BSE_SEVERE_RANGE_FAULT = 1;
        }
        else
        {
            VCUDataPtr->DigitalVal.BSE_SEVERE_RANGE_FAULT = 0;
        }

        /*********************************************************************************
          brake light
         *********************************************************************************/
        if (previous_brake_light_state == 0 &&
            BSE_sensor_sum > BRAKING_THRESHOLD + hysteresis)
        {
            // turn on brake lights
            gioSetBit(BRAKE_LIGHT_PORT, BRAKE_LIGHT_PIN, 0);

            // update brake light enable in the vcu data structure
            VCUDataPtr->DigitalOut.BRAKE_LIGHT_ENABLE = 1;

            // update brake light state
            previous_brake_light_state = 1;
        }
        else if (previous_brake_light_state == 1 &&
                 BSE_sensor_sum < BRAKING_THRESHOLD - hysteresis)
        {
            // turn off brake lights
            gioSetBit(BRAKE_LIGHT_PORT, BRAKE_LIGHT_PIN, 1);

            // update brake light enable in the vcu data structure
            VCUDataPtr->DigitalOut.BRAKE_LIGHT_ENABLE = 0;

            // update brake light state
            previous_brake_light_state = 0;
        }
        
        /*********************************************************************************
          Check if APPS1 and APPS2 are within 10% of each other
         *********************************************************************************/
        if(check_10PercentAPPS_Fault())
        {
            VCUDataPtr->DigitalVal.APPS_SEVERE_10DIFF_FAULT = 1; // Set fault flag in vcu data structure;
        }
        else
        {
            VCUDataPtr->DigitalVal.APPS_SEVERE_10DIFF_FAULT = 0;
        }

        /*********************************************************************************
          Check if brakes are pressed and accelerator pedal
          is pressed greater than or equal to 25%
         *********************************************************************************/
        if(check_Brake_Plausibility_Fault())
        {
            VCUDataPtr->DigitalVal.BSE_APPS_MINOR_SIMULTANEOUS_FAULT = 1;
        }
        else
        {
            VCUDataPtr->DigitalVal.BSE_APPS_MINOR_SIMULTANEOUS_FAULT = 0;
        }


        // debugging - jaypacamarra
        // manually setting state to RUNNING and setting THROTTLE_AVAILABLE to true to test DAC - jaypacamarra
//        state = RUNNING;
//        THROTTLE_AVAILABLE = true;

        /*********************************************************************************
          Set Throttle
         *********************************************************************************/
        if (state == RUNNING && THROTTLE_AVAILABLE)
        {
            // update throttle percentage in vcu data structure
            apps_percent_avg = (Percent_APPS1_Pressed + Percent_APPS2_Pressed) / 2;
            VCUDataPtr->AnalogOut.throttle_percentage.value = apps_percent_avg;

            // send DAC to inverter
            throttle = 390 * apps_percent_avg + 60;        // equation mapping the averaged signals to 0->500 for the DAC driver
            // ^ this equation may need to be modified for the curtis voltage lower limit and upper limit
            // i.e. map from 0.6V (60) to 4.5V (450) or something like that, instead of 0->500 (0V -> 5V)
            MCP48FV_Set_Value(throttle); // send throttle value to DAC driver
        }
        else
        {
            // send 0 to DAC
            MCP48FV_Set_Value(0);
            THROTTLE_AVAILABLE = false;
        }

#ifdef PMU_CYCLE
        //Start timer.
        cycles_PMU_start = timer_Stop(cycles_PMU_start, CPU_CLOCK_MHz);
        gioSetBit(gioPORTA, 5, 0);
#endif
    }
}
