/*
 * stateMachineTaskTest.c
 *
 *  Created on: May 15, 2021
 *      Author: junaidkhan
 */

#include "stateMachineTaskTest.h"
#include "phantom_freertos.h"
#include "vcu_data.h"
#include "board_hardware.h"

void stateMachineTaskTest(void* parameters){

    TickType_t LastTickCount;
    LastTickCount = xTaskGetTickCount();
    bool tractive_off_tested = false;
    bool faultCorrected = false;


    while(1){

        if(state == TRACTIVE_OFF){

            //UARTSend(PC_UART, "TRACTIVE_OFF.\r\n");


            if(xSemaphoreTake(vcuKey, pdMS_TO_TICKS(20)) && !tractive_off_tested){
                /*
                    VCUDataPtr->DigitalVal.RTDS = 1; // Set RTD
                    vTaskDelayUntil(&LastTickCount, pdMS_TO_TICKS(1000));  // Create  delay to check the response of state machine task on serial monitor

                    VCUDataPtr->DigitalVal.RTDS = 0; // Clear RTD
                    VCUDataPtr->DigitalVal.BSE_SEVERE_RANGE_FAULT = 1;  // Introduce any fault you like
                    vTaskDelayUntil(&LastTickCount, pdMS_TO_TICKS(1000));

                    VCUDataPtr->DigitalVal.RTDS = 0; // Clear RTD
                    VCUDataPtr->DigitalVal.BSE_SEVERE_RANGE_FAULT = 0;  // clear the fault.
                    vTaskDelayUntil(&LastTickCount, pdMS_TO_TICKS(1000));
                */

                VCUDataPtr->DigitalVal.TSAL_ON = 1; // Set TSAL on, the state should change to TRACTIVE_ON and stay there.

                xSemaphoreGive(vcuKey);
                tractive_off_tested = true;
            }


        }else if(state == TRACTIVE_ON){
            //UARTSend(PC_UART, "TRACTIVE_ON.\r\n");

            if(xSemaphoreTake(vcuKey, pdMS_TO_TICKS(20))&& tractive_off_tested){

                /*
                    VCUDataPtr->DigitalVal.TSAL_ON=0; // Turn off TSAL and see how the system responds.
                    vTaskDelayUntil(&LastTickCount, pdMS_TO_TICKS(1000));  // Create  delay to check the response of state machine task on serial monitor - correct response.
                */

                // Introduce a MINOR Error in the system - state should change MINOR_ERROR

                //VCUDataPtr->DigitalVal.HV_VOLTAGE_OUT_OF_RANGE_FAULT=1;
                //vTaskDelayUntil(&LastTickCount, pdMS_TO_TICKS(1000));  // Create  delay to check the response of state machine task on serial monitor -

                 // Correct the MINOR Error
                 //VCUDataPtr->DigitalVal.BSE_APPS_MINOR_SIMULTANEOUS_FAULT=0; // State should change back to RUNNING
                 //vTaskDelayUntil(&LastTickCount, pdMS_TO_TICKS(1000));  // Create  delay to check the response of state machine task on serial monitor -


                //VCUDataPtr->DigitalVal.BSE_SEVERE_RANGE_FAULT=1;
                //vTaskDelayUntil(&LastTickCount, pdMS_TO_TICKS(1000));  // Create  delay to check the response of state machine task on serial monitor -
                //VCUDataPtr->DigitalVal.RTDS = 1;

                VCUDataPtr->DigitalVal.RTDS=1;

                xSemaphoreGive(vcuKey);
            }

        }else if(state == RUNNING){
            //UARTSend(PC_UART, "Current state is RUNNING. \r\n");
            if(xSemaphoreTake(vcuKey, pdMS_TO_TICKS(20)) && !faultCorrected){
                //UARTSend(PC_UART, "Key Accessed.\r\n");
                VCUDataPtr->DigitalVal.HV_VOLTAGE_OUT_OF_RANGE_FAULT = 1;
                xSemaphoreGive(vcuKey);
            }


        }else if(state == MINOR_FAULT){
            //UARTSend(PC_UART, "Current state is MINOR_FAULT. \r\n");
            if(xSemaphoreTake(vcuKey, pdMS_TO_TICKS(20))&& !faultCorrected){
                           //UARTSend(PC_UART, "Key Accessed.\r\n");
                           VCUDataPtr->DigitalVal.HV_VOLTAGE_OUT_OF_RANGE_FAULT = 0;
                           xSemaphoreGive(vcuKey);
                           faultCorrected = true;
             }

        }else if(state == SEVERE_FAULT){

            //UARTSend(PC_UART, "Current state is SEVERE_FAULT. \r\n");
        }else{
            //UARTSend(PC_UART, "Current state unknown. \r\n");
        }

        vTaskDelayUntil(&LastTickCount, pdMS_TO_TICKS(500));


    }
}

//-- For Testing State machine Task - Added by jjkhan



