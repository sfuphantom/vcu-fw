/*
 * task_data_logging.c
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

//#include "board_hardware.h"   // contains hardware defines for specific board used (i.e. VCU or launchpad)

#include <Phantom/hardware/vcu_hw/board_hardware.h>
//#include <Phantom/hardware/launchpad_hw/board_hardware.h>

#include "Phantom_sci.h"


//#include "task_data_logging.h"


//#define TASK_PRINT 1


/***********************************************************
 * @function                - vDataLoggingTask
 *
 * @brief                   - This task will send any important data over CAN to the dashboard for logging onto the SD card
 *
 * @param[in]               - pvParameters
 *
 * @return                  - None
 * @Note                    - None
 ***********************************************************/

void vDataLoggingTask(void *pvParameters){

    // any initialization
    TickType_t xLastWakeTime;          // will hold the timestamp at which the task was last unblocked
    const TickType_t xFrequency = 500; // task frequency in ms

    // Initialize the xLastWakeTime variable with the current time;
    xLastWakeTime = xTaskGetTickCount();

    while(true)
    {
        // Wait for the next cycle
        vTaskDelayUntil(&xLastWakeTime, xFrequency);

        // for timing:
        gioSetBit(TASK_TIMING_PORT, 4, 1);

//        MCP48FV_Set_Value(300);

//        gioToggleBit(gioPORTA, 7);
        if (TASK_PRINT) {UARTSend(PC_UART, "------------->DATA LOGGING TO DASHBOARD\r\n");}
//            UARTSend(scilinREG, xTaskGetTickCount());
            //----> do we need to send battery voltage to dashboard?


            // log HV voltage, current TSAL state, shutdown circuit states to CAN
            // send to dashboard
            // this may or may not depend on state

        // for timing:
        gioSetBit(TASK_TIMING_PORT, 4, 0);
    }

}
