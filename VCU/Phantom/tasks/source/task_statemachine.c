/*
 * task_throttle.c
 *
 *  Created on: Mar 31, 2020
 *      Author: gabriel
 */

#include <halcogen_vcu/include/FreeRTOS.h>
#include <halcogen_vcu/include/FreeRTOSConfig.h>
#include <halcogen_vcu/include/gio.h>
#include <halcogen_vcu/include/het.h>
#include <halcogen_vcu/include/os_queue.h>
#include <halcogen_vcu/include/os_semphr.h>
#include <halcogen_vcu/include/os_task.h>
#include <halcogen_vcu/include/os_timer.h>
#include "vcu_rev2.h"
#include "priorities.h"
#include "Phantom_sci.h"
#include "FreeRTOS.h"

uint32_t blue_duty = 100;
uint32_t blue_flag = 0;

extern State state;

/*********************************************************************************
 *                               SYSTEM STATE FLAGS
 *********************************************************************************/
extern uint8_t TSAL;// = 0;
extern uint8_t RTDS;// = 0;
extern long RTDS_RAW;// = 0;
extern uint8_t BMS;//  = 1;
extern uint8_t IMD;//  = 1;
extern uint8_t BSPD;// = 1;
extern uint8_t BSE_FAULT;// = 0;

/***********************************************************
 * @function                - vStateMachineTask
 *
 * @brief                   - This task evaluates the state of the vehicle and controls any change of state
 *
 * @param[in]               - pvParameters
 *
 * @return                  - None
 * @Note                    - None
 ***********************************************************/
void vStateMachineTask(void *pvParameters){
//    uint32 lrval;
//    char stbuf[64];
//    int nchars;

    TickType_t xLastWakeTime;          // will hold the timestamp at which the task was last unblocked
    const TickType_t xFrequency = 100; // task frequency in ms

    // Initialize the xLastWakeTime variable with the current time;
    xLastWakeTime = xTaskGetTickCount();

    while(true)
    {
        // Wait for the next cycle
        vTaskDelayUntil(&xLastWakeTime, xFrequency);

        // for timing:
        gioSetBit(hetPORT1, 9, 1);

        if (TASK_PRINT) {UARTSend(PC_UART, "STATE MACHINE UPDATE TASK\r\n");}

//        UARTSend(scilinREG, (char *)xLastWakeTime);

//        xStatus = xQueueReceive(xq, &lrval, 30);
//        nchars = ltoa(lrval, stbuf);
//        if (TASK_PRINT) {UARTSend(scilinREG, (char *)stbuf);}
//        if (TASK_PRINT) {UARTSend(scilinREG, "\r\n");}


        // MAKE SOME LED BLINK ON THE VCU! TECHNICALLY U HAVE 6 DIFFERENT ONES U CAN BLINK
        // MAKE EACH TASK BLINK A DIFFERENT LED

/*********************** STATE MACHINE EVALUATION ***********************************/


        if (state == TRACTIVE_OFF)
        {
//            pwmSetDuty(RGB_LED_PORT, BLUE_LED, 50U); // blue LED
            pwmSetDuty(RGB_LED_PORT, GREEN_LED, 100U); // green LED
            pwmSetDuty(RGB_LED_PORT, RED_LED, 100U); // red LED

            hetSIGNAL_t dutycycle_and_period;
            dutycycle_and_period.duty = blue_duty;
            dutycycle_and_period.period = 1000;
//            = {(unsigned int)1, (double)100}; // duty cycle in %, period in us

            pwmSetSignal(RGB_LED_PORT, BLUE_LED, dutycycle_and_period);

            if (blue_duty <= 0)
            {
                blue_flag = 1; // 1 means rising
            }
            else if (blue_duty >= 100)
            {
                blue_flag = 0; // 0 means falling
            }

            if (blue_flag == 1)
            {
                blue_duty+= 5;
            }
            else
            {
                blue_duty-= 5;
            }


            if (STATE_PRINT) {UARTSend(PC_UART, "********TRACTIVE_OFF********");}
            if (BMS == 1 && IMD == 1 && BSPD == 1 && TSAL == 1 && BSE_FAULT == 0)
            {
                // if BMS/IMD/BSPD = 1 then the shutdown circuit is closed
                // TSAL = 1 indicates that the AIRs have closed
                // tractive system should now be active
                state = TRACTIVE_ON;
            }
            else if (BSE_FAULT == 1)
            {
                state = FAULT;
            }
        }
        else if (state == TRACTIVE_ON)
        {
            pwmSetDuty(RGB_LED_PORT, GREEN_LED, 100U);
            pwmSetDuty(RGB_LED_PORT, RED_LED, 100U);
            pwmSetDuty(RGB_LED_PORT, BLUE_LED, 50U); // blue


            if (STATE_PRINT) {UARTSend(PC_UART, "********TRACTIVE_ON********");}

            if (RTDS == 1)
            {
                // ready to drive signal is switched
                state = RUNNING;
            }

            // Mechanism to switch back to tractive off from this state? or into error state?
        }
        else if (state == RUNNING)
        {
            pwmSetDuty(RGB_LED_PORT, BLUE_LED, 100U); // blue LED
            pwmSetDuty(RGB_LED_PORT, RED_LED, 100U); // red LED
            pwmSetDuty(RGB_LED_PORT, GREEN_LED, 50U); // green LED

            if (STATE_PRINT) {UARTSend(PC_UART, "********RUNNING********");}

            if (RTDS == 0)
            {
                // read to drive signal switched off
                state = TRACTIVE_ON;
            }
            if (BMS == 0 || IMD == 0 || BSPD == 0 || TSAL == 0)
            {
                // FAULT in shutdown circuit, or AIRs have opened from TSAL
                state = FAULT;
            }

        }
        else if (state == FAULT)
        {
            pwmSetDuty(RGB_LED_PORT, BLUE_LED, 100U); // blue LED
            pwmSetDuty(RGB_LED_PORT, RED_LED, 50U); // red LED
            pwmSetDuty(RGB_LED_PORT, GREEN_LED, 100U); // green LED

            if (STATE_PRINT) {UARTSend(PC_UART, "********FAULT********");}
            // uhhh turn on a fault LED here??
            // how will we reset out of this?

            if (BSE_FAULT == 0)
            {
                state = TRACTIVE_OFF;
            }
        }

        if (STATE_PRINT) {UARTSend(PC_UART, "\r\n");}

        // for timing:
        gioSetBit(hetPORT1, 9, 0);
    }
}
