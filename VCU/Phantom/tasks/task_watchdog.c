/*
 * task_watchdog.c
 *
 *  Created on: Mar 31, 2020
 *      Author: gabriel
 */

#include "vcu_rev2.h"
#include "Phantom_sci.h"

#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "os_task.h"
#include "os_queue.h"
#include "os_semphr.h"
#include "os_timer.h"
/***********************************************************
 * @function                - vWatchdogTask
 *
 * @brief                   - This task will monitor all threads and pet the watchdog if everything is fine. Else it will let the watchdog reset the MCU
 *
 * @param[in]               - pvParameters
 *
 * @return                  - None
 * @Note                    - None
 ***********************************************************/

void vWatchdogTask(void *pvParameters){

    // any initialization
    TickType_t xLastWakeTime;          // will hold the timestamp at which the task was last unblocked
    const TickType_t xFrequency = 300; // task frequency in ms
    // watchdog timeout is 1.6 seconds

    // Initialize the xLastWakeTime variable with the current time;
    xLastWakeTime = xTaskGetTickCount();

    while(true)
    {
        // Wait for the next cycle
        vTaskDelayUntil(&xLastWakeTime, xFrequency);

        if (TASK_PRINT) {UARTSend(PC_UART, "------------->WATCHDOG TASK\r\n");}
//            UARTSend(scilinREG, xTaskGetTickCount());

        gioToggleBit(WATCHDOG_PORT, WATCHDOG_PIN);
    }

}
