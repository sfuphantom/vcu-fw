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
#include "vcu_data.h"
#include "FreeRTOS.h"
#include "Current_transducer.h"

#define TASK_PRINT  0

// change to better data type (move to VCU data structure)
int lv_current = 0;
float sensor_current = 0.0;     

/*********************************************************************************
 *                               SYSTEM STATE FLAGS
 *********************************************************************************/

extern data* VCUDataPtr;

extern uint8_t RTDS;// = 0;
extern long RTDS_RAW;// = 0;
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

    int nchars;
    char stbuf[64];

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

        //HVcurrent data -rafguevara14
        //get and store voltage and current values into analogInputs struct
        VCUDataPtr->analogInputs.currentHV_A.value = getHVsensorCurrent();

        //OUT OF RANGE ERROR
        VCUDataPtr->digitalValues.HVCURRENT_OUT_OF_RANGE = isHVcurrent_inRange();
        
        //add yashs HV Bus Voltage driver
  
            //and update HVflags

            //update HV flags accordingly

        // IMD data (maybe this needs to be a separate interrupt?)
        updateIMDData();

        IMDData_t dataIMD = getIMDData();

        //determine state of all flags -rafguevara14
        VCUDataPtr->digitalValues.IMD_LOW_ISO_FAULT = (dataIMD.IMDState == Normal_25 || dataIMD.IMDState == Isolation_Failure);

        VCUDataPtr->digitalValues.IMD_SHORT_CIRCUIT_FAULT = (dataIMD.IMDState == Short_Circuit);

        VCUDataPtr->digitalValues.IMD_DEVICE_ERR_FAULT =  (dataIMD.IMDState == Device_Error);

        VCUDataPtr->digitalValues.IMD_BAD_INFO_FAULT =  (dataIMD.IMDState == Bad_Info);

        VCUDataPtr->digitalValues.IMD_UNDEF_ERR =  (dataIMD.IMDState == Undefined_fault);

        VCUDataPtr->digitalValues.IMD_GARBAGE_DATA_FAULT =  (dataIMD.IMDState == Unknown);

   

        // TSAL state (shutdown driver func call)

        // CAN status from BMS (call Xinglu driver) (this may need an interrupt for when data arrives, and maybe stored in a buffer? maybe not.. we should try both)

        // read LV voltage, current
        lv_current = LV_reading(LV_current_register);

        //read lv voltage
        
  

        // for timing:
        gioSetBit(hetPORT1, 25, 0);
    }
}
