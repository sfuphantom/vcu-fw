/*
 * task_throttle.c
 *
 *  Created on: Mar 31, 2020
 *      Author: gabriel
 */

#include "adc.h"
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "os_queue.h"
#include "os_semphr.h"
#include "os_task.h"
#include "os_timer.h"
#include "task_throttle.h" //jaypacamarra

#include "stdlib.h" // stdlib.h has ltoa() which we use for our simple SCI printing routine.

#include "board_hardware.h" // contains hardware defines for specific board used (i.e. VCU or launchpad)

#include "MCP48FV_DAC_SPI.h"
#include "Phantom_sci.h"
#include "gio.h"
#include "sys_common.h"

#include "vcu_data.h"

extern State state;
extern TimerHandle_t xTimers[];   // jaypacamarra
extern bool APPS1_RANGE_FAULT_TIMER_EXPIRED;   //jaypacamarra
extern bool APPS2_RANGE_FAULT_TIMER_EXPIRED;   //jaypacamarra
extern bool BSE_RANGE_FAULT_TIMER_EXPIRED;  //jaypacamarra
extern bool FP_DIFF_FAULT_TIMER_EXPIRED;    //jaypacamarra

/*********************************************************************************
  ADC FOOT PEDAL AND APPS STUFF (SHOULD GENERALIZE THIS)
 *********************************************************************************/
extern adcData_t FP_data[3];
extern adcData_t *FP_data_ptr;       // = &FP_data[0];
extern unsigned int volatile FP_sensor_1_sum; // = 0;
extern unsigned int FP_sensor_1_avg;
extern unsigned int volatile FP_sensor_2_sum; // = 0;
extern unsigned int FP_sensor_2_avg;

extern unsigned int BSE_sensor_sum; //  = 0;
extern unsigned int BSE_sensor_avg; //  = 0;
extern unsigned int NumberOfChars;

extern uint16 FP_sensor_1_min; // = 0;
extern uint16 FP_sensor_2_min; // = 0;

extern uint16 FP_sensor_1_max; // = 4095; // 12-bit ADC
extern uint16 FP_sensor_2_max; // = 4095; // 12-bit ADC
extern uint16 FP_sensor_1_percentage;
extern uint16 FP_sensor_2_percentage;
extern float FP_sensor_diff;

extern char command[8]; // used for ADC printing.. this is an array of 8 chars, each char is 8 bits

extern data *VCUDataPtr;

extern bool THROTTLE_AVAILABLE;

uint32_t volatile fault_10DIFF_counter_ms = 0;      // hold duration of fault in milliseconds - jaypacamarra
uint32_t fault_BSE_Range_counter_ms = 0;   // hold duration of fault in milliseconds - jaypacamarra
uint32_t fault_APPS1_Range_counter_ms = 0; // hold duration of fault in milliseconds - jaypacamarra
uint32_t fault_APPS2_Range_counter_ms = 0; // hold duration of fault in milliseconds - jaypacamarra

float volatile Percent_APPS1_pressed; // hold percentage foot pedal1 (APPS1) is pressed, 0-1 -jaypacamarra
float volatile Percent_APPS2_pressed; // hold percentage foot pedal2 (APPS2) is pressed, 0-1 -jaypacamarra

float alpha = 0.5;                               // Change this to tweak lowpass filter response - jaypacamarra
float BSE_filtered_sensor_value;                 // filtered BSE sensor value - jaypacamarra
float BSE_previous_filtered_sensor_values = 0;   // previous BSE filtered output - jaypacamarra
float APPS1_filtered_sensor_value;               // filtered APPS1 sensor value - jaypacamarra
float APPS1_previous_filtered_sensor_values = 0; // previous APPS1 filtered output - jaypacamarra
float APPS2_filtered_sensor_value;               // filtered APPS2 sensor value - jaypacamarra
float APPS2_previous_filtered_sensor_values = 0; // previous BSE filtered output - jaypacamarra

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
    bool APPS1_range_fault_timer_started = false;
    bool APPS2_range_fault_timer_started = false;
    bool BSE_range_fault_timer_started   = false;
    bool FP_diff_fault_timer_started     = false;

    while (true)
    {
        // Wait for the next cycle
        vTaskDelayUntil(&xLastWakeTime, xFrequency);

        // for timing:
        gioSetBit(hetPORT1, 5, 1);

        // read APPS signals
//        if (TASK_PRINT)
//        {
//            UARTSend(PC_UART, "THROTTLE CONTROL\r\n");
//        }
        //        UARTSend(scilinREG, xTaskGetTickCount());

        // how was this i from 0 to 10 selected?  //        for(i=0; i<10; i++)
        //        {
        //            adcStartConversion(adcREG1, adcGROUP1);
        //            while(!adcIsConversionComplete(adcREG1, adcGROUP1));
        //            adcGetData(adcREG1, 1U, FP_data_ptr);
        //            FP_sensor_1_sum += (unsigned int)FP_data[0].value;
        //            FP_sensor_2_sum += (unsigned int)FP_data[1].value;
        //            BSE_sensor_sum  += (unsigned int)FP_data[2].value;
        //        }

        adcStartConversion(adcREG1, adcGROUP1);
        while (!adcIsConversionComplete(adcREG1, adcGROUP1));
        adcGetData(adcREG1, adcGROUP1, FP_data_ptr);
        BSE_sensor_sum = (unsigned int)FP_data[0].value;  // AD1IN[0] -jaypacamarra
        FP_sensor_1_sum = (unsigned int)FP_data[1].value; // AD1IN[1] -jaypacamarra
        FP_sensor_2_sum = (unsigned int)FP_data[2].value; // AD1IN[2] -jaypacamarra

        /******** Signal conditioning - lowpass filter - jaypacamarra *********/
        // Filter the raw BSE,APPS1, and APPS2 sensor values
//        BSE_sensor_sum = BSE_previous_filtered_sensor_values + alpha * (BSE_sensor_sum - BSE_previous_filtered_sensor_values);
//        FP_sensor_1_sum = APPS1_previous_filtered_sensor_values + alpha * (FP_sensor_1_sum - APPS1_previous_filtered_sensor_values);
//        FP_sensor_2_sum = APPS2_previous_filtered_sensor_values + alpha * (FP_sensor_2_sum - APPS2_previous_filtered_sensor_values);
//
//        // Set previous filtered values to current filtered values
//        BSE_previous_filtered_sensor_values = BSE_sensor_sum;
//        APPS1_previous_filtered_sensor_values = FP_sensor_1_sum;
//        APPS2_previous_filtered_sensor_values = FP_sensor_2_sum;

        // check for short to GND/5V on APPS sensor 1
        if (FP_sensor_1_sum < APPS1_MIN_VALUE) // APPS1 is assumed shorted to GND
        {

            if(APPS1_range_fault_timer_started == false)
            {
                if(xTimerStart(xTimers[2], pdMS_TO_TICKS(50)) != pdPASS) // start software timer for apps1 range fault
                {
                    for(;;);
                }
                else
                {
                    APPS1_range_fault_timer_started = true;
                }
            }

            if(APPS1_RANGE_FAULT_TIMER_EXPIRED)
            {
                VCUDataPtr->DigitalVal.APPS1_SEVERE_RANGE_FAULT = 1; // Set fault flag in vcu data structure
            }

        }
        else if (FP_sensor_1_sum > APPS1_MAX_VALUE) // APPS1 is assumed shorted to 5V
        {

            if(APPS1_range_fault_timer_started == false)
            {
                if(xTimerStart(xTimers[2], pdMS_TO_TICKS(50)) != pdPASS) // start software timer for apps1 range fault
                {
                    for(;;);
                }
                else
                {
                    APPS1_range_fault_timer_started = true;
                }
            }

            if(APPS1_RANGE_FAULT_TIMER_EXPIRED)
            {
                VCUDataPtr->DigitalVal.APPS1_SEVERE_RANGE_FAULT = 1; // Set fault flag in vcu data structure
            }

        }
        else
        {
            // should be in normal range so no fault
            VCUDataPtr->DigitalVal.APPS1_SEVERE_RANGE_FAULT = 0;

            // reset fault timer
            xTimerStop(xTimers[2], portMAX_DELAY);
            APPS1_range_fault_timer_started = false;
        }

        // check for short to GND/3V3 on APPS sensor 2
        if (FP_sensor_2_sum < APPS2_MIN_VALUE) // APPS2 assumed shorted to GND
        {
            if(APPS2_range_fault_timer_started == false)
            {
                if(xTimerStart(xTimers[3], pdMS_TO_TICKS(50)) != pdPASS) // start software timer for apps1 range fault
                {
                    for(;;);
                }
                else
                {
                    APPS2_range_fault_timer_started = true;
                }
            }

            if(APPS2_RANGE_FAULT_TIMER_EXPIRED)
            {
                VCUDataPtr->DigitalVal.APPS2_SEVERE_RANGE_FAULT = 1; // Set fault flag in vcu data structure
            }
        }
        else if (FP_sensor_2_sum > APPS2_MAX_VALUE) // APPS2 assumed shorted to 3.3V
        {
            if(APPS2_range_fault_timer_started == false)
            {
                if(xTimerStart(xTimers[3], pdMS_TO_TICKS(50)) != pdPASS) // start software timer for apps1 range fault
                {
                    for(;;);
                }
                else
                {
                    APPS2_range_fault_timer_started = true;
                }
            }

            if(APPS2_RANGE_FAULT_TIMER_EXPIRED)
            {
                VCUDataPtr->DigitalVal.APPS2_SEVERE_RANGE_FAULT = 1; // Set fault flag in vcu data structure
            }
        }
        else
        {
            // should be in normal range so no fault
            VCUDataPtr->DigitalVal.APPS2_SEVERE_RANGE_FAULT = 0;

            // reset fault timer
            xTimerStop(xTimers[3], portMAX_DELAY);
            APPS2_range_fault_timer_started = false;
        }

        // check for short to GND/5V on BSE
        if (BSE_sensor_sum < BSE_MIN_VALUE) // BSE is assumed shorted to GND
        {
            if(BSE_range_fault_timer_started == false)
            {
                if(xTimerStart(xTimers[4], pdMS_TO_TICKS(50)) != pdPASS) // start software timer for apps1 range fault
                {
                    for(;;);
                }
                else
                {
                    BSE_range_fault_timer_started = true;
                }
            }

            if(BSE_RANGE_FAULT_TIMER_EXPIRED)
            {
                VCUDataPtr->DigitalVal.BSE_SEVERE_RANGE_FAULT = 1; // Set fault flag in vcu data structure
            }
        }
        else if (BSE_sensor_sum > BSE_MAX_VALUE) // BSE is assumed shorted to 5V
        {
            if(BSE_range_fault_timer_started == false)
            {
                if(xTimerStart(xTimers[4], pdMS_TO_TICKS(50)) != pdPASS) // start software timer for apps1 range fault
                {
                    for(;;);
                }
                else
                {
                    BSE_range_fault_timer_started = true;
                }
            }

            if(BSE_RANGE_FAULT_TIMER_EXPIRED)
            {
                VCUDataPtr->DigitalVal.BSE_SEVERE_RANGE_FAULT = 1; // Set fault flag in vcu data structure
            }
        }
        else
        {
            // should be in normal range
            VCUDataPtr->DigitalVal.BSE_SEVERE_RANGE_FAULT = 0;

            // reset fault timer
            xTimerStop(xTimers[4], portMAX_DELAY);
            BSE_range_fault_timer_started = false;
        }

        // moving average signal conditioning.. worth it to graph this out and find a good filter time constant
        //        FP_sensor_1_avg = FP_sensor_1_sum/10;
        //        FP_sensor_2_avg = FP_sensor_2_sum/10;
        //        BSE_sensor_avg  = BSE_sensor_sum;

        //        FP_sensor_1_sum = 0;
        //        FP_sensor_2_sum = 0;
        //        BSE_sensor_sum  = 0;

        //        BSE_sensor_sum  = (unsigned int)FP_data[2].value;

        //        FP_sensor_1_percentage = (FP_sensor_1_avg-FP_sensor_1_min)/(FP_sensor_1_max-FP_sensor_1_min);
        //        FP_sensor_2_percentage = (FP_sensor_2_avg-FP_sensor_2_min)/(FP_sensor_2_max-FP_sensor_2_min);
        //        FP_sensor_diff = abs(FP_sensor_2_percentage - FP_sensor_1_percentage);

        //        ltoa(FP_sensor_1_avg,(char *)command);
        //        if (APPS_PRINT) {UARTSend(scilinREG, "0x");}
        //        if (APPS_PRINT) {UARTSend(scilinREG, command);}
        //
        //        ltoa(FP_sensor_2_avg,(char *)command);
        //        if (APPS_PRINT) {UARTSend(scilinREG, "   0x");}
        //        if (APPS_PRINT) {UARTSend(scilinREG, command);}
        //        if (APPS_PRINT) {UARTSend(scilinREG, "\r\n");}

        // brake light (flickers if pedal is around 2000 and is noisily jumping above and below!)
        // added hysteresis to remove the noisily jumping near the brake threshhold - jaypacamarra
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
        

        // What is all of this?? - jaypacamarra
//        NumberOfChars = ltoa(BSE_sensor_sum, (char *)command);
//        if (BSE_PRINT)
//        {
//            UARTSend(PC_UART, "*****BSE**** ");
//        }
//        if (BSE_PRINT)
//        {
//            sciSend(PC_UART, NumberOfChars, command);
//        }
//        if (BSE_PRINT)
//        {
//            UARTSend(PC_UART, "   ");
//        }
//
//        NumberOfChars = ltoa(FP_sensor_1_sum, (char *)command);
//        if (BSE_PRINT)
//        {
//            UARTSend(PC_UART, "*****APPS 1**** ");
//        }
//        if (BSE_PRINT)
//        {
//            sciSend(PC_UART, NumberOfChars, command);
//        }
//        if (BSE_PRINT)
//        {
//            UARTSend(PC_UART, "   ");
//        }
//
//        NumberOfChars = ltoa(FP_sensor_2_sum, (char *)command);
//        if (BSE_PRINT)
//        {
//            UARTSend(PC_UART, "*****APPS 2**** ");
//        }
//        if (BSE_PRINT)
//        {
//            sciSend(PC_UART, NumberOfChars, command);
//        }
//        if (BSE_PRINT)
//        {
//            UARTSend(PC_UART, "\r\n");
//        }

        // What does this do?? -jaypacamarra
        //        xStatus = xQueueSendToBack(xq, &FP_sensor_1_avg, 0);
        //        xStatus = xQueueSendToBack(xq, &FP_sensor_2_avg, 0);

        // Calculate FP_sensor_diff - jaypacamarra
        Percent_APPS1_pressed = ((float)FP_sensor_1_sum - (float)APPS1_MIN_VALUE) / ((float)APPS1_MAX_VALUE - (float)APPS1_MIN_VALUE); // APPS1 % pressed compared to MAX and MIN values
        Percent_APPS1_pressed = (FP_sensor_1_sum <= APPS1_MIN_VALUE) ? 0 : Percent_APPS1_pressed;                 // negative values are set to 0

        Percent_APPS2_pressed = ((float)FP_sensor_2_sum - (float)APPS2_MIN_VALUE) / ((float)APPS2_MAX_VALUE - (float)APPS2_MIN_VALUE); // APPS2 % pressed compared to MAX and MIN values
        Percent_APPS2_pressed = (FP_sensor_2_sum <= APPS2_MIN_VALUE) ? 0 : Percent_APPS2_pressed;                 // negative values are set to 0

        FP_sensor_diff = fabs(Percent_APPS2_pressed - Percent_APPS1_pressed); // Calculate absolute difference between APPS1 and APPS2 readings

        // 10% APPS redundancy check
        if (FP_sensor_diff > 0.10)
        {
            if(FP_diff_fault_timer_started == false)
            {
                if(xTimerStart(xTimers[5], pdMS_TO_TICKS(50)) != pdPASS) // start software timer for apps1 range fault
                {
                    for(;;);
                }
                else
                {
                    FP_diff_fault_timer_started = true;
                }
            }

            if(FP_DIFF_FAULT_TIMER_EXPIRED)
            {
                VCUDataPtr->DigitalVal.APPS_SEVERE_10DIFF_FAULT = 1; // Set fault flag in vcu data structure
            }
        }
        else // Added this else statement so we have a way to set APPS 10% fault to 0 - jaypacamarra
        {
            // No fault
            VCUDataPtr->DigitalVal.APPS_SEVERE_10DIFF_FAULT = 0;

            // reset the fault timer
            xTimerStop(xTimers[5], portMAX_DELAY);
            FP_diff_fault_timer_started = false;
        }

        // Check if brakes are pressed and accelerator pedal is pressed greater than or equal to 25% - jaypacamarra
        if (BSE_sensor_sum >= BRAKING_THRESHOLD &&
            FP_sensor_1_sum >= APPS1_MIN_VALUE + 0.25 * (APPS1_MAX_VALUE - APPS1_MIN_VALUE) &&
            FP_sensor_2_sum >= APPS2_MIN_VALUE + 0.25 * (APPS2_MAX_VALUE - APPS2_MIN_VALUE))
        {
            // Set fault
            VCUDataPtr->DigitalVal.BSE_APPS_MINOR_SIMULTANEOUS_FAULT = 1;
        }
        else
        {
            // APPS/Brake plausibility fault only clears if APPS returns to less than 5% pedal position
            // with or without brake operation (see EV.5.7.2) - jaypacamarra
            if (FP_sensor_1_sum < APPS1_MIN_VALUE + 0.05 * (APPS1_MAX_VALUE - APPS1_MIN_VALUE) &&
                FP_sensor_2_sum < APPS2_MIN_VALUE + 0.05 * (APPS2_MAX_VALUE - APPS2_MIN_VALUE))
            {
                // No fault
                VCUDataPtr->DigitalVal.BSE_APPS_MINOR_SIMULTANEOUS_FAULT = 0;
            }
        }

        // What is this?
        if (state == RUNNING && THROTTLE_AVAILABLE)
        {
            // send DAC to inverter
            unsigned int apps_avg = 0.5 * (FP_sensor_1_sum + FP_sensor_2_sum); // averaging the two foot pedal signals
            unsigned int throttle = 0.23640662 * apps_avg - 88.6524825;        // equation mapping the averaged signals to 0->500 for the DAC driver
            // ^ this equation may need to be modified for the curtis voltage lower limit and upper limit
            // i.e. map from 0.6V (60) to 4.5V (450) or something like that, instead of 0->500 (0V -> 5V)

            MCP48FV_Set_Value(throttle); // send throttle value to DAC driver

            // Print out DAC output
            NumberOfChars = ltoa(throttle, (char *)command);

            // printing debug:
            //            sciSend(PC_UART, NumberOfChars, command);
            //            UARTSend(PC_UART, "\r\n");
        }
        else
        {
            // send 0 to DAC
            MCP48FV_Set_Value(0);
            THROTTLE_AVAILABLE = false;
        }

        // for timing:
        gioSetBit(hetPORT1, 5, 0);
    }
}
