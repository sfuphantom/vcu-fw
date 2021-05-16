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

#include "stdlib.h" // stdlib.h has ltoa() which we use for our simple SCI printing routine.


#include "board_hardware.h"   // contains hardware defines for specific board used (i.e. VCU or launchpad)

#include "MCP48FV_DAC_SPI.h"
#include "Phantom_sci.h"
#include "gio.h"

#include "vcu_data.h"

#include "priorities.h"

State state;

/*********************************************************************************
                 ADC FOOT PEDAL AND APPS STUFF (SHOULD GENERALIZE THIS)
 *********************************************************************************/
adcData_t FP_data[3];
adcData_t *FP_data_ptr = FP_data;
unsigned int FP_sensor_1_sum;// = 0;
unsigned int FP_sensor_1_avg;
unsigned int FP_sensor_2_sum;// = 0;
unsigned int FP_sensor_2_avg;

extern unsigned int BSE_sensor_sum;//  = 0;
unsigned int BSE_sensor_avg;//  = 0;
unsigned int NumberOfChars;

uint16 FP_sensor_1_min;// = 0;
uint16 FP_sensor_2_min;// = 0;

uint16 FP_sensor_1_max;// = 4095; // 12-bit ADC
uint16 FP_sensor_2_max;// = 4095; // 12-bit ADC
uint16 FP_sensor_1_percentage;
uint16 FP_sensor_2_percentage;
uint16 FP_sensor_diff;

char command[8]; // used for ADC printing.. this is an array of 8 chars, each char is 8 bits

extern data* VCUDataPtr;

extern bool THROTTLE_AVAILABLE;

/*
 *  task_eeprom.c initializes the VCUData structure based on last stored VCU state in eeprom.
 *      Task can't should not execute its body until initialization has occured.
 */
// ++ Added by jjkhan
extern volatile uint8_t initializationOccured;
// ++ Added by jjkhan

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
void vThrottleTask(void *pvParameters){


    TickType_t xLastWakeTime;          // will hold the timestamp at which the task was last unblocked
    //const TickType_t xFrequency = 10 ; // task frequency in ms

    // Initialize the xLastWakeTime variable with the current time;
    xLastWakeTime = xTaskGetTickCount();

    while(true)
    {
        if(initializationOccured){

            // Added by jjkhan: moved "state" inside VCU Data structure;
            state = VCUDataPtr->vcuState; // Get current VCU state

            // Wait for the next cycle
            vTaskDelayUntil(&xLastWakeTime, THROTTLE_TASK_PERIOD_MS);

            // for timing:
            gioSetBit(hetPORT1, 5, 1);

            // read APPS signals
            if (TASK_PRINT) {UARTSend(PC_UART, "THROTTLE CONTROL\r\n");}
    //        UARTSend(scilinREG, xTaskGetTickCount());

            // how was this i from 0 to 10 selected?
    //        for(i=0; i<10; i++)
    //        {
    //            adcStartConversion(adcREG1, adcGROUP1);
    //            while(!adcIsConversionComplete(adcREG1, adcGROUP1));
    //            adcGetData(adcREG1, 1U, FP_data_ptr);
    //            FP_sensor_1_sum += (unsigned int)FP_data[0].value;
    //            FP_sensor_2_sum += (unsigned int)FP_data[1].value;
    //            BSE_sensor_sum  += (unsigned int)FP_data[2].value;
    //        }

            adcStartConversion(adcREG1, adcGROUP1);
            while(!adcIsConversionComplete(adcREG1, adcGROUP1));
            adcGetData(adcREG1, 1U, FP_data_ptr);
            BSE_sensor_sum = (unsigned int)FP_data[0].value; // ADC pin 19?
            FP_sensor_1_sum = (unsigned int)FP_data[1].value; // ADC pin 20?
            FP_sensor_2_sum = (unsigned int)FP_data[2].value; // ADC pin 21?


            // check for short to GND/5V on sensor 1
            // thresholds

            // check for short to GND/3V3 on sensor 2
            // thresholds

            // check for short to GND/5V on BSE
            if (BSE_sensor_sum < BSE_MIN_VALUE)
            {
                // if it's less than 0.5V, then assume shorted to GND as this is not normal range
                VCUDataPtr->DigitalVal.BSE_SEVERE_RANGE_FAULT = 1;

            }
            else if (BSE_sensor_sum > BSE_MAX_VALUE) // change from magic number to a #define BSE_MAX_VALUE
            {
                // if it's greater than 4.5V, then assume shorted to 5V as this is not normal range
                VCUDataPtr->DigitalVal.BSE_SEVERE_RANGE_FAULT = 1;
            }
            else
            {
                // should be in normal range
                VCUDataPtr->DigitalVal.BSE_SEVERE_RANGE_FAULT = 0;
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
            if (BSE_sensor_sum < BRAKING_THRESHOLD)
            {
                //gioSetBit(BRAKE_LIGHT_PORT, BRAKE_LIGHT_PIN, 1);
            }
            else
            {
                //gioSetBit(BRAKE_LIGHT_PORT, BRAKE_LIGHT_PIN, 0);
            }

            NumberOfChars = ltoa(BSE_sensor_sum,(char *)command);
            if (BSE_PRINT) {UARTSend(PC_UART, "*****BSE**** ");}
            if (BSE_PRINT) {sciSend(PC_UART, NumberOfChars, command);}
            if (BSE_PRINT) {UARTSend(PC_UART, "   ");}

            NumberOfChars = ltoa(FP_sensor_1_sum,(char *)command);
            if (BSE_PRINT) {UARTSend(PC_UART, "*****APPS 1**** ");}
            if (BSE_PRINT) {sciSend(PC_UART, NumberOfChars, command);}
            if (BSE_PRINT) {UARTSend(PC_UART, "   ");}

            NumberOfChars = ltoa(FP_sensor_2_sum,(char *)command);
            if (BSE_PRINT) {UARTSend(PC_UART, "*****APPS 2**** ");}
            if (BSE_PRINT) {sciSend(PC_UART, NumberOfChars, command);}
            if (BSE_PRINT) {UARTSend(PC_UART, "\r\n");}

    //        xStatus = xQueueSendToBack(xq, &FP_sensor_1_avg, 0);
    //        xStatus = xQueueSendToBack(xq, &FP_sensor_2_avg, 0);


            // 10% APPS redundancy check
            if(FP_sensor_diff > 0.10)
            {
                UARTSend(PC_UART, "SENSOR DIFFERENCE FAULT\r\n");
            }

            // need to do APPS plausibility check with BSE

            if (state == RUNNING && THROTTLE_AVAILABLE)
            {
                // send DAC to inverter
                unsigned int apps_avg = 0.5*(FP_sensor_1_sum + FP_sensor_2_sum); // averaging the two foot pedal signals
                unsigned int throttle = 0.23640662*apps_avg - 88.6524825; // equation mapping the averaged signals to 0->500 for the DAC driver
                // ^ this equation may need to be modified for the curtis voltage lower limit and upper limit
                // i.e. map from 0.6V (60) to 4.5V (450) or something like that, instead of 0->500 (0V -> 5V)


                MCP48FV_Set_Value(throttle); // send throttle value to DAC driver

                // Print out DAC output
                NumberOfChars = ltoa(throttle,(char *)command);

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
        }else{
            vTaskDelayUntil(&xLastWakeTime, THROTTLE_TASK_PERIOD_MS);
        }
    }


}
