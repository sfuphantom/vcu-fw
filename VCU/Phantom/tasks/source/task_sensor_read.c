/*
 * task_throttle.c
 *
 *  Created on: Mar 31, 2020
 *      Author: gabriel
 */

#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "gio.h"
#include "os_queue.h"
#include "os_semphr.h"
#include "os_task.h"
#include "os_timer.h"

#include "board_hardware.h"   // contains hardware defines for specific board used (i.e. VCU or launchpad)
//#include <Phantom/hardware/vcu_hw/board_hardware.h>

#include "Phantom_sci.h"
#include "LV_monitor.h"
#include "IMD.h"
#include "vcu_data.h"
#include "FreeRTOS.h"
//#include "Current_transducer.h" //merge with yash branch first
#include "Shutdown.h"
#include "priorities.h"

unsigned int temp_sum = 0;



//#define TASK_PRINT

// change to better data type (move to VCU data structure)
int lv_current = 0;
//float sensor_current = 0.0;     not sure what this is....

/*********************************************************************************
 *                               SYSTEM STATE FLAGS
 *********************************************************************************/

extern data* VCUDataPtr;

extern uint8_t RTDS;// = 0;
long RTDS_RAW;      // = 0;


/*
 *  task_eeprom.c initializes the VCUData structure based on last stored VCU state in eeprom.
 *      Task can't should not execute its body until initialization has occured.
 */
// ++ Added by jjkhan
extern volatile uint8_t initializationOccured;
// ++ Added by jjkhan

/***********************************************************
 * @function                - vSensorReadTask
 *
 * @brief                   - This task will read all the sensors in the vehicle (except for the APPS which requires more critical response)
 *
 * @param[in]               - pvParameters
 *
 * @return                  - None
 * @Note                    - None
 ***********************************************************/
void vSensorReadTask(void *pvParameters){

    // any initialization
    TickType_t xLastWakeTime;          // will hold the timestamp at which the task was last unblocked
    //const TickType_t xFrequency = 100; // task frequency in ms

    // Initialize the xLastWakeTime variable with the current time;
    xLastWakeTime = xTaskGetTickCount();
    while(true)
    {
       if(initializationOccured){
            // for timing:
            gioSetBit(hetPORT1, 25, 1);
            //
            if (TASK_PRINT) { UARTSend(PC_UART, "SENSOR READING TASK\r\n"); }


            // TSAL and Shutdown GPIO states, check welded airs
            storeShutdownValues();

            //RTDS values, turn off RTDS if TSAL is off
            if(VCUDataPtr->DigitalVal.TSAL_STATUS == 0){

                VCUDataPtr->DigitalVal.RTDS = 0;
            }

            //RTDS = RTDS if no severe faults else 0
            VCUDataPtr->DigitalVal.RTDS = ( !isSevereFault() && VCUDataPtr->DigitalVal.RTDS );

            //throttle still allowed w/ minor fault...


            //HVcurrent data merge with yash branch first

            //get and store current values into analogInputs struct
            //        VCUDataPtr->AnalogIn.currentHV_A.value = getHVsensorCurrent();

            //        //OUT OF RANGE ERROR
            //        VCUDataPtr->DigitalVal.HVCURRENT_OUT_OF_RANGE = isHVcurrent_inRange();

            //add yashs HV Bus Voltage driver

            //and update HVflags

            //update HV flags accordingly

            // CAN status from BMS (call Xinglu driver) (this may need an interrupt for when data arrives, and maybe stored in a buffer? maybe not.. we should try both)

            //        // read LV voltage, current (confirm Range w/ test board team) (need timeout for i2c first)
            //        VCUDataPtr->AnalogIn.currentLV_A.adc_value = LV_reading(LV_current_register);
            //
            //        VCUDataPtr->AnalogIn.voltageLV_V.adc_value = LV_reading(LV_bus_voltage_register);


            // IMD data (maybe this needs to be a separate interrupt?)
            updateIMDData();

            IMDData_t dataIMD = getIMDData();

//            setIMDStates(dataIMD);


            //        pwmSetDuty(BUZZER_PORT,READY_TO_DRIVE_BUZZER,50);
            //        pwmStart(BUZZER_PORT, READY_TO_DRIVE_BUZZER);

            #ifdef SENSOR_PRINT

            //        UARTSend(PC_UART, "SENSOR READING TASK\r\n");

            //        serialSendData();

            //        UARTSend(PC_UART, "\r\n\n\n");
            UARTSend(PC_UART, VCUDataPtr->DigitalVal.TSAL_WELDED ? "AIRS ARE WELDED\r\n" : "");

            //        print_Shutdownvals();

            #endif

       }else{
            vTaskDelayUntil(&xLastWakeTime, SENSOR_READ_TASK_PERIOD_MS);
       }

    }//while loop
}

void setIMDStates(IMDData_t dataIMD){


    //determine state OF IMD flags
//    VCUDataPtr->DigitalVal.IMD_LOW_ISO_FAULT = (dataIMD.IMDState == Normal_25 || dataIMD.IMDState == Isolation_Failure); //double check this line later....
//
//    VCUDataPtr->DigitalVal.IMD_SHORT_CIRCUIT_FAULT = (dataIMD.IMDState == Short_Circuit);
//
//    VCUDataPtr->DigitalVal.IMD_DEVICE_ERR_FAULT =  (dataIMD.IMDState == Device_Error);
//
//    VCUDataPtr->DigitalVal.IMD_BAD_INFO_FAULT =  (dataIMD.IMDState == Bad_Info);
//
//    VCUDataPtr->DigitalVal.IMD_UNDEF_ERR =  (dataIMD.IMDState == Undefined_fault);
//
//    VCUDataPtr->DigitalVal.IMD_GARBAGE_DATA_FAULT =  (dataIMD.IMDState == Unknown);

}


