/*
 * task_throttle.c
 *
 *  Created on: Mar 31, 2020
 *      Author: gabriel
 */

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

#include "throttle.h"   // Added THROTTLE DRIVER - jaypacamarra (5/5/2021)

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

extern bool THROTTLE_AVAILABLE;

extern data *VCUDataPtr;

bool previous_brake_light_state = 1;    // Default = 1. Holds previous brake light state, 1 = ON, 0 = OFF - jaypacamarra
uint16_t hysteresis = 200;          // change this to tweak hysteresis threshhold - jaypacamarra

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

        // ADC conversions
        getPedalReadings();

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

            // update brake light state
            previous_brake_light_state = 1;
        }
        else if (previous_brake_light_state == 1 &&
                 BSE_sensor_sum < BRAKING_THRESHOLD - hysteresis)
        {
            // turn off brake lights
            gioSetBit(BRAKE_LIGHT_PORT, BRAKE_LIGHT_PIN, 1);

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
            // send DAC to inverter
            unsigned int apps_avg = 0.5 * (FP_sensor_1_sum + FP_sensor_2_sum); // averaging the two foot pedal signals
            unsigned int throttle = 0.23640662 * apps_avg - 88.6524825;        // equation mapping the averaged signals to 0->500 for the DAC driver
            // ^ this equation may need to be modified for the curtis voltage lower limit and upper limit
            // i.e. map from 0.6V (60) to 4.5V (450) or something like that, instead of 0->500 (0V -> 5V)

            MCP48FV_Set_Value(throttle); // send throttle value to DAC driver
        }
        else
        {
            // send 0 to DAC
            MCP48FV_Set_Value(450);
            THROTTLE_AVAILABLE = false;
        }
    }
}
