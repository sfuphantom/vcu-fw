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
#include "os_queue.h"
#include <string.h>

extern QueueHandle_t stateMachineMessages;
extern volatile uint8_t initializationOccured;
extern data *VCUDataPtr;

static inline void activateTractiveSystem(void);
static inline void setReadyToDrive(void);
static inline void deactiveTractiveSystem(void);
static inline void resetReadyToDrive(void);
static inline void triggerFault(uint8_t location, char *typeOfFault);
static inline void clearFault(uint8_t location, char *typeOfFault);
static inline void clearAllFaults(void);

extern int checkSDC(void);
extern int checkIMD(void);
extern int checkBSE_APPS(void);
extern int CheckHVLVSensor(void);
extern int checkCAN(void);
// Send messages from task_statemachine.c and receive them in this task -> if needed.
void stateMachineTaskTest(void* parameters){
    // Create a variable to be able to set a breakpoint to read rxBuffer Message each time in Memory Browser
       uint8_t forBreakPoint;

       // A buffer that will hold messages received from eepromTask - Don't need to print it.
       char rxBuffer[60];

       // Start Sequence Flag
       TickType_t mylastTickCount;
       mylastTickCount = xTaskGetTickCount();

       while(1){

           if(initializationOccured){

               unsigned long numberOfMessages = uxQueueMessagesWaiting(stateMachineMessages);
               if(numberOfMessages){

                   if (xQueueReceive(stateMachineMessages,rxBuffer,pdMS_TO_TICKS(0))==pdPASS){ // Don't block if no message in the Queue
                       forBreakPoint =1;
                   }
               }

               // Clear all faults first
               while(checkCAN() || checkBSE_APPS() || checkCAN() || checkIMD()
                        || checkSDC()){
                   // Faults exist
                   if (xSemaphoreTake(vcuKey, pdMS_TO_TICKS(2)))
                   {
                       resetReadyToDrive();  // Clear RTD
                       deactiveTractiveSystem();  // clear TSAL
                       clearAllFaults();  // Clear all faults
                       xSemaphoreGive(vcuKey);
                   }
               }

               // Block Task for 100ms : expected State == TRACTIVE OFF
               vTaskDelayUntil(&mylastTickCount,pdMS_TO_TICKS(100));

           //++  Test Start up sequence
               while(!VCUDataPtr->DigitalVal.TSAL_ON){
                   if (xSemaphoreTake(vcuKey, pdMS_TO_TICKS(2)))
                  {
                      // Active tractive system
                      activateTractiveSystem();
                      xSemaphoreGive(vcuKey);
                  }
               }

               // Block Task for 100ms : expected State == TRACTIVE ON
               vTaskDelayUntil(&mylastTickCount,pdMS_TO_TICKS(100));

               while(!VCUDataPtr->DigitalVal.RTDS){
                   if (xSemaphoreTake(vcuKey, pdMS_TO_TICKS(2)))
                 {
                     // Set Ready to drive
                     setReadyToDrive();
                     xSemaphoreGive(vcuKey);
                 }
               }

               // Block Task for 100ms : expected State == RUNNING
               vTaskDelayUntil(&mylastTickCount,pdMS_TO_TICKS(100));
           //--  Test Start up sequence

           //++ Test HV timed Fault
              while(!VCUDataPtr->DigitalVal.HV_VOLTAGE_OUT_OF_RANGE_FAULT){
                  if (xSemaphoreTake(vcuKey, pdMS_TO_TICKS(2)))
                {
                    // Set Ready to drive
                    triggerFault(HV_LV_FAULT, "severe");
                    xSemaphoreGive(vcuKey);
                }
              }
              // Block Task for 100ms : expected State -> MINOR_FAULT -> start timer -> timer expired -> SEVERE
              // Increase timer period to ensure the state changes to minor before going to severe.
                 vTaskDelayUntil(&mylastTickCount,pdMS_TO_TICKS(100));
           //++ Test HV timed Fault

               // You can add more scenarios below to check different state transitions
               // I've only tested the important ones: HV timers and start-up sequence.

               // Block for 500ms
               vTaskDelayUntil(&mylastTickCount,pdMS_TO_TICKS(500));

           }else{

               // Block for 500ms
               vTaskDelayUntil(&mylastTickCount,pdMS_TO_TICKS(100));
           }
       }
}

// Activate Tractive System
static inline void activateTractiveSystem(void)
{

    VCUDataPtr->DigitalVal.TSAL_ON = 1;
}

// Set "Ready to Drive"
static inline void setReadyToDrive(void)
{

    VCUDataPtr->DigitalVal.RTDS = 1;

}

//  Deactive Tractive System Light
static inline void deactiveTractiveSystem(void)
{
    VCUDataPtr->DigitalVal.TSAL_ON = 0;
}

// Reset "Ready to Drive"
static inline void resetReadyToDrive(void)
{

    VCUDataPtr->DigitalVal.RTDS = 0;

}

// Introduce a fault
/*
 *  @Locations:
 *      SDC_FAULT              0U
 *      BSE_APPS_FAULT         1U
 *      HV_LV_FAULT            2U
 *      CAN_FAULT              3U
 *      IMD_SYSTEM_FAULT       4U
 *  @Type:
 *      MINOR   FAULT                  "minor"
 *      SEVERE SEVERE                 "severe"
 */

static inline void triggerFault(uint8_t location, char *typeOfFault)
{

    if ((location & SDC_FAULT) == SDC_FAULT)
    {

        if (strcmp(typeOfFault, "minor"))
        {
            // There are no minor SDC faults

        }
        else if (strcmp(typeOfFault, "severe"))
        {
            // There are multiple, just picked one randomly
            VCUDataPtr->DigitalVal.BSPD_FAULT = 1;
        }
    }
    else if ((location & BSE_APPS_FAULT) == BSE_APPS_FAULT)
    {
        if (strcmp(typeOfFault, "minor"))
        {
            // There are multiple, just picked one randomly

            VCUDataPtr->DigitalVal.BSE_APPS_MINOR_SIMULTANEOUS_FAULT = 1;

        }
        else if (strcmp(typeOfFault, "severe"))
        {
            // There are multiple, just picked one randomly

            VCUDataPtr->DigitalVal.APPS_SEVERE_10DIFF_FAULT = 1;

        }

    }
    else if ((location & HV_LV_FAULT) == HV_LV_FAULT)
    {

        if (strcmp(typeOfFault, "minor"))
        {
            // Pick a LV Fault

            VCUDataPtr->DigitalVal.LV_CURRENT_OUT_OF_RANGE = 1;

        }
        else if (strcmp(typeOfFault, "severe"))
        {
            // Call one of the HV Faults

            VCUDataPtr->DigitalVal.HV_VOLTAGE_OUT_OF_RANGE_FAULT = 1;

        }

    }
    else if ((location & CAN_FAULT) == CAN_FAULT)
    {
        if (strcmp(typeOfFault, "minor"))
        {

            VCUDataPtr->DigitalVal.CAN_ERROR_TYPE2 = 1;

        }
        else if (strcmp(typeOfFault, "severe"))
        {

            VCUDataPtr->DigitalVal.CAN_ERROR_TYPE1 = 1;

        }
    }
    else if ((location & IMD_SYSTEM_FAULT) == IMD_SYSTEM_FAULT)
    {
        if (strcmp(typeOfFault, "minor"))
        {
            // There are no minor faults defined at the time of testing
        }
        else if (strcmp(typeOfFault, "severe"))
        {
            // Just picked any of the possible IMD faults

            VCUDataPtr->DigitalVal.IMD_SHORT_CIRCUIT_FAULT = 1;

        }
    }

}

// Clear a fault
/*
 *  @Locations=integer:
 *      SDC_FAULT              0U
 *      BSE_APPS_FAULT         1U
 *      HV_LV_FAULT            2U
 *      CAN_FAULT              3U
 *      IMD_SYSTEM_FAULT       4U
 *  @Type=string:
 *      MINOR   FAULT                  "minor"
 *      SEVERE SEVERE                 "severe"
 */

static inline void clearFault(uint8_t location, char *typeOfFault)
{
    if ((location & SDC_FAULT) == SDC_FAULT)
    {

        if (strcmp(typeOfFault, "minor"))
        {
            // There are no minor SDC faults

        }
        else if (strcmp(typeOfFault, "severe"))
        {
            // There are multiple, just picked one randomly

            VCUDataPtr->DigitalVal.BSPD_FAULT = 0;

        }
    }
    else if ((location & BSE_APPS_FAULT) == BSE_APPS_FAULT)
    {
        if (strcmp(typeOfFault, "minor"))
        {
            // There are multiple, just picked one randomly

            VCUDataPtr->DigitalVal.BSE_APPS_MINOR_SIMULTANEOUS_FAULT = 0;

        }
        else if (strcmp(typeOfFault, "severe"))
        {
            // There are multiple, just picked one randomly

            VCUDataPtr->DigitalVal.APPS_SEVERE_10DIFF_FAULT = 0;

        }

    }
    else if ((location & HV_LV_FAULT) == HV_LV_FAULT)
    {

        if (strcmp(typeOfFault, "minor"))
        {
            // Pick a LV Fault

            VCUDataPtr->DigitalVal.LV_CURRENT_OUT_OF_RANGE = 0;

        }
        else if (strcmp(typeOfFault, "severe"))
        {
            // Call one of the HV Faults

            VCUDataPtr->DigitalVal.HV_VOLTAGE_OUT_OF_RANGE_FAULT = 0;

        }

    }
    else if ((location & CAN_FAULT) == CAN_FAULT)
    {
        if (strcmp(typeOfFault, "minor"))
        {

            VCUDataPtr->DigitalVal.CAN_ERROR_TYPE2 = 0;

        }
        else if (strcmp(typeOfFault, "severe"))
        {

            VCUDataPtr->DigitalVal.CAN_ERROR_TYPE1 = 0;

        }
    }
    else if ((location & IMD_SYSTEM_FAULT) == IMD_SYSTEM_FAULT)
    {
        if (strcmp(typeOfFault, "minor"))
        {
            // There are no minor faults defined at the time of testing
        }
        else if (strcmp(typeOfFault, "severe"))
        {
            // Just picked any of the possible IMD faults

            VCUDataPtr->DigitalVal.IMD_SHORT_CIRCUIT_FAULT = 0;

        }
    }
}

// Clear all faults
static inline void clearAllFaults(void)
{

    VCUDataPtr->DigitalVal.BMS_GPIO_FAULT = 0;
    VCUDataPtr->DigitalVal.IMD_FAULT = 0;
    VCUDataPtr->DigitalVal.BSPD_FAULT = 0;

    /* APPS/BSE Sensor - Faults */

    VCUDataPtr->DigitalVal.BSE_SEVERE_RANGE_FAULT = 0;
    VCUDataPtr->DigitalVal.APPS1_SEVERE_RANGE_FAULT = 0;
    VCUDataPtr->DigitalVal.APPS2_SEVERE_RANGE_FAULT = 0;
    VCUDataPtr->DigitalVal.APPS_SEVERE_10DIFF_FAULT = 0;
    VCUDataPtr->DigitalVal.BSE_APPS_MINOR_SIMULTANEOUS_FAULT = 0;

    /* HV Current Sensor & Voltage Sensor- Faults */

    VCUDataPtr->DigitalVal.HV_CURRENT_OUT_OF_RANGE = 0;
    VCUDataPtr->DigitalVal.APPS_PROPORTION_ERROR = 0;
    VCUDataPtr->DigitalVal.HV_VOLTAGE_OUT_OF_RANGE_FAULT = 0;

    /* CAN Error Messages. */

    VCUDataPtr->DigitalVal.CAN_ERROR_TYPE1 = 0; // Severe Error reported by CAN
    VCUDataPtr->DigitalVal.CAN_ERROR_TYPE2 = 0;  // Minor Errot Reported by CAN

    /* LV Current Sensor & Voltage Sensor- Faults */
    VCUDataPtr->DigitalVal.LV_CURRENT_OUT_OF_RANGE = 0;
    VCUDataPtr->DigitalVal.LV_VOLTAGE_OUT_OF_RANGE = 0;

    /* IMD Faults. */
    VCUDataPtr->DigitalVal.IMD_LOW_ISO_FAULT = 0;
    VCUDataPtr->DigitalVal.IMD_SHORT_CIRCUIT_FAULT = 0;
    VCUDataPtr->DigitalVal.IMD_DEVICE_ERR_FAULT = 0;
    VCUDataPtr->DigitalVal.IMD_BAD_INFO_FAULT = 0;
    VCUDataPtr->DigitalVal.IMD_UNDEF_ERR = 0;
    VCUDataPtr->DigitalVal.IMD_GARBAGE_DATA_FAULT = 0;

    /* TSAL Faults.*/
    VCUDataPtr->DigitalVal.TSAL_WELDED_AIRS_FAULT = 0;

    VCUDataPtr->DigitalOut.BRAKE_LIGHT_ENABLE = 0;
}
