/*
 * task_throttle.c
 *
 *  Created on: Mar 31, 2020
 *      Author: gabriel
 */

#include <Phantom/Drivers/Throttle/throttle.h>   // Added THROTTLE DRIVER - jaypacamarra (5/5/2021)
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

#include "throttle.h"

extern State state;
extern TimerHandle_t xTimers[];                 //jaypacamarra

/*********************************************************************************
  ADC FOOT PEDAL AND APPS STUFF (SHOULD GENERALIZE THIS)
 *********************************************************************************/
// variables to store foot pedal adc values
extern unsigned int volatile BSE_sensor_sum;
extern unsigned int volatile FP_sensor_1_sum;
extern unsigned int volatile FP_sensor_2_sum;

extern bool THROTTLE_AVAILABLE;

extern SemaphoreHandle_t vcuKey;    // mutex

extern data *VCUDataPtr;

bool previous_brake_light_state = 1;    // Default = 1. Holds previous brake light state, 1 = ON, 0 = OFF - jaypacamarra

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
    const TickType_t xFrequency = 5; // task frequency in ms - need to come up with this number better

    // Initialize the xLastWakeTime variable with the current time;
    xLastWakeTime = xTaskGetTickCount();

    while (true)
    {
        // Wait for the next cycle
        vTaskDelayUntil(&xLastWakeTime, xFrequency);

        // Get pedal readings
        getPedalReadings();

        // Signal conditioning - jaypacamarra
        applyLowPassFilter();

        if(xSemaphoreTake(vcuKey,pdMS_TO_TICKS(1))==1){ // Protect vcuStructure & wait for 1 milliseconds, if key not available, skip

            /*********************************************************************************
              Update pedal inputs in vcu data structure
             *********************************************************************************/

            VCUDataPtr->AnalogIn.APPS1_percentage.value = get_APPS1_Pedal_Percent();
            VCUDataPtr->AnalogIn.APPS2_percentage.value = get_APPS2_Pedal_Percent();
            VCUDataPtr->AnalogIn.BSE_percentage.value = get_BSE_Pedal_Percent();


            /*********************************************************************************
              check for short to GND/VCC on APPS sensor 1
             *********************************************************************************/

             VCUDataPtr->DigitalVal.APPS1_SEVERE_RANGE_FAULT = check_APPS1_Range_Fault();


            /*********************************************************************************
              check for short to GND/VCC on APPS sensor 2
             *********************************************************************************/

             VCUDataPtr->DigitalVal.APPS2_SEVERE_RANGE_FAULT = check_APPS2_Range_Fault();


            /*********************************************************************************
              check for short to GND/VCC on BSE
             *********************************************************************************/

             VCUDataPtr->DigitalVal.BSE_SEVERE_RANGE_FAULT = check_BSE_Range_Fault();


            /*********************************************************************************
              brake light
             *********************************************************************************/
            if (previous_brake_light_state == 0 &&
                BSE_sensor_sum > BRAKING_THRESHOLD + HYSTERESIS)
            {
                // turn on brake lights
                gioSetBit(BRAKE_LIGHT_PORT, BRAKE_LIGHT_PIN, BRAKE_LIGHT_ON);

                // update brake light enable in the vcu data structure
                VCUDataPtr->DigitalOut.BRAKE_LIGHT_ENABLE = 1;

                // update brake light state
                previous_brake_light_state = 1;
            }
            else if (previous_brake_light_state == 1 &&
                    BSE_sensor_sum < BRAKING_THRESHOLD - HYSTERESIS)
            {
                // turn off brake lights
                gioSetBit(BRAKE_LIGHT_PORT, BRAKE_LIGHT_PIN, BRAKE_LIGHT_OFF);

                // update brake light enable in the vcu data structure
                VCUDataPtr->DigitalOut.BRAKE_LIGHT_ENABLE = 0;

                // update brake light state
                previous_brake_light_state = 0;
            }

            /*********************************************************************************
              Check if APPS1 and APPS2 are within 10% of each other
             *********************************************************************************/

            VCUDataPtr->DigitalVal.APPS_SEVERE_10DIFF_FAULT = check_10PercentAPPS_Fault();


            /*********************************************************************************
              Check if brakes are pressed and accelerator pedal
              is pressed greater than or equal to 25%
             *********************************************************************************/

            VCUDataPtr->DigitalVal.BSE_APPS_MINOR_SIMULTANEOUS_FAULT = check_Brake_Plausibility_Fault();

            // Give vcu key back
            xSemaphoreGive(vcuKey);
        }


        // debugging - jaypacamarra
        // manually setting state to RUNNING and setting THROTTLE_AVAILABLE to true to test DAC - jaypacamarra
        state = RUNNING;
        THROTTLE_AVAILABLE = true;

        /*********************************************************************************
          Set Throttle
         *********************************************************************************/
        if (state == RUNNING && THROTTLE_AVAILABLE)
        {
            // update throttle percentage in vcu data structure
            float apps_percent_avg = (get_APPS1_Pedal_Percent() + get_APPS2_Pedal_Percent()) / 2;

            if(xSemaphoreTake(vcuKey,pdMS_TO_TICKS(1))==1){
                VCUDataPtr->AnalogOut.throttle_percentage.value = apps_percent_avg;
                xSemaphoreGive(vcuKey); // Give vcu key back
            }

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
    }
}
