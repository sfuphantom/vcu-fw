/*
 * task_throttle.c
 *
 *  Created on: Mar 31, 2020
 *      Author: gabriel
 */

#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "gio.h"
#include "het.h"
#include "os_queue.h"
#include "os_semphr.h"
#include "os_task.h"
#include "os_timer.h"

#include "board_hardware.h"   // contains hardware defines for specific board used (i.e. VCU or launchpad)

#include "priorities.h"
#include "Phantom_sci.h"
#include "FreeRTOS.h"
#include "os_projdefs.h"

// Needed the swiSwitchMode defined in here, will moved swiSwitchMode to board_hardware.h
#include "eeprom_driver.h"
#include "vcu_data.h" // data structure to hold VCU data

uint32_t blue_duty = 100;
uint32_t blue_flag = 0;

extern State state;

/* ++ Added by jjkhan - All declarations should be in header file. - move the following declarations to task_statemachine.h */
/*extern TaskHandle_t eepromHandler;
extern uint8_t powerFailureFlag;
extern SemaphoreHandle_t powerfailureFlagKey; */

extern SemaphoreHandle_t vcuKey;

/*  Added by jjkhan */
/*********************************************************************************
 *                               SYSTEM STATE FLAGS
 *********************************************************************************/
//extern uint8_t TSAL;// = 0;
//extern uint8_t RTDS;// = 0;
//extern long RTDS_RAW;// = 0;
//extern uint8_t BMS;//  = 1;
//extern uint8_t IMD;//  = 1;
//extern uint8_t BSPD;// = 1;
//extern uint8_t BSE_FAULT;// = 0;

extern data* VCUDataPtr;
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
    //const TickType_t xFrequency = 100; // task frequency in ms

    // Initialize the xLastWakeTime variable with the current time;
    xLastWakeTime = xTaskGetTickCount();

    /*
     * ++ Added by jjkhan for Notification result.
     *
     * Notification value is 32-bits wide. We're using Bit 0 to send notification to task_eeprom to update Data Block 1.
     * Data block 1 will store a flag used for deciding load VCUData with default or eeprom.
     *
     * */

    /* -- Added by jjkhan for Notification result. */

    while(true)
    {
        // Wait for the next cycle -> By jjkhan: Call at the end, this will block the task and give CPU access to the next high priority task.
        //vTaskDelayUntil(&xLastWakeTime, xFrequency);

        /* Added by jjkhan */
                vTaskDelayUntil(&xLastWakeTime, STATE_MACHINE_TASK_PERIOD_MS); // A delay of 0.1 seconds -  based on line 66 statement
        /* Added by jjkhan */
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
            /*
//            pwmSetDuty(RGB_LED_PORT, BLUE_LED, 50U); // blue LED
            pwmSetDuty(RGB_LED_PORT, GREEN_LED, 100U); // green LED
            pwmSetDuty(RGB_LED_PORT, RED_LED, 100U); // red LED

            hetSIGNAL_t dutycycle_and_period;
            dutycycle_and_period.duty = blue_duty;
            dutycycle_and_period.period = 1000;
//            = {(unsigned int)1, (double)100}; // duty cycle in %, period in us

            pwmSetSignal(RGB_LED_PORT, BLUE_LED, dutycycle_and_period);
            */

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


            if (STATE_PRINT) {UARTSend(PC_UART, "********TRACTIVE_OFF********\r\n");}

            // A '1' indicates Healthy, i.e. no fault. A '0' indicates there is a fault in the particular sub-system. I didn't set this up, will change it later. -Added by jjkhan
            if (VCUDataPtr->DigitalVal.BMS_FAULT == 1 && VCUDataPtr->DigitalVal.IMD_FAULT == 1
                    && VCUDataPtr->DigitalVal.BSPD_FAULT == 1 && VCUDataPtr->DigitalVal.TSAL_FAULT == 1 && VCUDataPtr->DigitalVal.BSE_FAULT == 0)  // Initially, check if all sub-systems are healthy and no BSE fault. '0' for BSE means healthy. Again, I didn't set this up, will change it to make it more intuitive. - jjkhan
            {
                // if BMS/IMD/BSPD = 1 then the shutdown circuit is closed
                // TSAL = 1 indicates that the AIRs have closed
                // tractive system should now be active
                state = TRACTIVE_ON;
                //++ Added by jjkhan
                if(xSemaphoreTake(vcuKey, pdMS_TO_TICKS(200))){ // Wait for 10 milliseconds if the key not available, come back later
                    VCUDataPtr->DigitalVal.POWER_FAILURE_FLAG = 0;  // Update powerFailureFlag
                    xSemaphoreGive(vcuKey);
                }
                //-- Added by jjkhan

            }
            else if (VCUDataPtr->DigitalVal.BSE_FAULT == 1)
            {
                state = FAULT;

            }
            //++ Added by jjkhan
            //xTaskNotify(&eepromHandler, (1<<0), eSetBits/*eSetValueWithoutOverwrite*/);  // Notify eeprom task to keep updating Data block 1

            //-- Added by jjkhan
        }
        else if (state == TRACTIVE_ON)
        {
            /*
            pwmSetDuty(RGB_LED_PORT, GREEN_LED, 100U);
            pwmSetDuty(RGB_LED_PORT, RED_LED, 100U);
            pwmSetDuty(RGB_LED_PORT, BLUE_LED, 50U); // blue
            */

            if (STATE_PRINT) {UARTSend(PC_UART, "********TRACTIVE_ON********\r\n");}

            if (VCUDataPtr->DigitalVal.RTDS == 1)
            {
                // ready to drive signal is switched
                state = RUNNING;

                //++ Added by jjkhan
                if(xSemaphoreTake(vcuKey, pdMS_TO_TICKS(200))){ // Wait for 10 milliseconds if the key not available, come back later
                    VCUDataPtr->DigitalVal.POWER_FAILURE_FLAG = 0;  // Update powerFailureFlag
                    xSemaphoreGive(vcuKey);
                }
                //-- Added by jjkhan
            }

            // Mechanism to switch back to tractive off from this state? or into error state?
        }
        else if (state == RUNNING)
        {
            /*
            pwmSetDuty(RGB_LED_PORT, BLUE_LED, 100U); // blue LED
            pwmSetDuty(RGB_LED_PORT, RED_LED, 100U); // red LED
            pwmSetDuty(RGB_LED_PORT, GREEN_LED, 50U); // green LED
            */

            if (STATE_PRINT) {UARTSend(PC_UART, "********RUNNING********\r\n");}

            if (VCUDataPtr->DigitalVal.RTDS == 0)
            {
                // read to drive signal switched off -> Update state_machine to  TRACTIVE_ON state first
                state = TRACTIVE_ON;
            }
            if (VCUDataPtr->DigitalVal.BMS_FAULT == 0 || VCUDataPtr->DigitalVal.IMD_FAULT == 0 || VCUDataPtr->DigitalVal.BSPD_FAULT == 0 || VCUDataPtr->DigitalVal.TSAL_FAULT == 0)
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

            if (STATE_PRINT) {UARTSend(PC_UART, "********FAULT********\r\n");}
            // uhhh turn on a fault LED here??
            // how will we reset out of this?

            //++ Added by jjkhan
               if(xSemaphoreTake(vcuKey, pdMS_TO_TICKS(200))){ // Wait for 10 milliseconds if the key not available, come back later
                   VCUDataPtr->DigitalVal.POWER_FAILURE_FLAG = 0xFF;  // Update powerFailureFlag
                   xSemaphoreGive(vcuKey);
               }
            //-- Added by jjkhan

            if (VCUDataPtr->DigitalVal.BSE_FAULT == 0)
            {
                state = TRACTIVE_OFF;

            }
        }

        if (STATE_PRINT) {UARTSend(PC_UART, "\r\n");}

        // for timing:
        gioSetBit(hetPORT1, 9, 0);



    }
}
