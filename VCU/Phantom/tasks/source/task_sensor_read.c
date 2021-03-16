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

#include "Phantom_sci.h"
#include "LV_monitor.h"
#include "IMD.h"
#include "vcu_data.h"
#include "FreeRTOS.h"
#include "Current_transducer.h"

#define TASK_PRINT  0

// change to better data type (move to VCU data structure)
int lv_current = 0;
//float sensor_current = 0.0;     not sure what this is....

/*********************************************************************************
 *                               SYSTEM STATE FLAGS
 *********************************************************************************/

extern data* VCUDataPtr;

extern uint8_t RTDS;// = 0;
long RTDS_RAW;      // = 0;
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
    const TickType_t xFrequency = 100; // task frequency in ms

    // Initialize the xLastWakeTime variable with the current time;
    xLastWakeTime = xTaskGetTickCount();

    while(true)
    {
        // Wait for the next cycle
        vTaskDelayUntil(&xLastWakeTime, xFrequency);

        // for timing:
        gioSetBit(hetPORT1, 25, 1);

        RTDS_RAW = gioGetBit(READY_TO_DRIVE_PORT, READY_TO_DRIVE_PIN);

        if ( gioGetBit(gioPORTA, 2) == 1)
        {
            VCUDataPtr->DigitalVal.RTDS = 0;
//            UARTSend(PC_UART, "RTDS RAW IS READ AS 1, RESETTING RTDS SIGNAL\r\n");
        }
        else
        {
//            UARTSend(PC_UART, "RTDS RAW IS READ AS 0, RESETTING RTDS SIGNAL\r\n");
        }

        if (TASK_PRINT) {UARTSend(PC_UART, "SENSOR READING TASK\r\n");}
//        UARTSend(scilinREG, xTaskGetTickCount());

<<<<<<< HEAD
        //HVcurrent data merge with yash branch first

        //get and store current values into analogInputs struct
//        VCUDataPtr->AnalogIn.currentHV_A.value = getHVsensorCurrent();
//
//        //OUT OF RANGE ERROR
//        VCUDataPtr->DigitalVal.HVCURRENT_OUT_OF_RANGE = isHVcurrent_inRange();
        
        //add yashs HV Bus Voltage driver
  
            //and update HVflags

            //update HV flags accordingly

        //merge with develop first
//        updateIMDData();
//
//        IMDData_t dataIMD = getIMDData();
//
//        //determine state of all flags (add to vcu data structure in some format...?)
//        VCUDataPtr->DigitalVal.IMD_LOW_ISO_FAULT = (dataIMD.IMDState == Normal_25 || dataIMD.IMDState == Isolation_Failure); //double check this line later....
//
//        VCUDataPtr->DigitalVal.IMD_SHORT_CIRCUIT_FAULT = (dataIMD.IMDState == Short_Circuit);
//
//        VCUDataPtr->DigitalVal.IMD_DEVICE_ERR_FAULT =  (dataIMD.IMDState == Device_Error);
//
//        VCUDataPtr->DigitalVal.IMD_BAD_INFO_FAULT =  (dataIMD.IMDState == Bad_Info);
//
//        VCUDataPtr->DigitalVal.IMD_UNDEF_ERR =  (dataIMD.IMDState == Undefined_fault);
//
//        VCUDataPtr->DigitalVal.IMD_GARBAGE_DATA_FAULT =  (dataIMD.IMDState == Unknown);

        // TSAL and Shutdown GPIO states
//        storeShutdownValues();

        // CAN status from BMS (call Xinglu driver) (this may need an interrupt for when data arrives, and maybe stored in a buffer? maybe not.. we should try both)
=======
        // IMD data (maybe this needs to be a separate interrupt?)
        updateIMDData();
        serialSendData();

        // Shutdown GPIOs (will probably start with these non-interrupt and see if we need to later..)

        // TSAL state

        // CAN status from BMS (this may need an interrupt for when data arrives, and maybe stored in a buffer? maybe not.. we should try both)
>>>>>>> 0afe16f869cdcfbfaa2f256f02ee721cccb7ee98

        // read LV voltage, current
        VCUDataPtr->AnalogIn.currentLV_A.adc_value = LV_reading(LV_current_register);

<<<<<<< HEAD
        VCUDataPtr->AnalogIn.voltageLV_V.adc_value = LV_reading(LV_bus_voltage_register);
=======
        // this needs to be updated to not block the whole system if i2c not available
//        lv_current = LV_reading(LV_current_register);

        // make sure state machine signal flags are updated

        // check for all errors here and update VCU data structure or state machine flags accordingly
>>>>>>> 0afe16f869cdcfbfaa2f256f02ee721cccb7ee98


        // for timing:
        gioSetBit(hetPORT1, 25, 0);
    }
}
