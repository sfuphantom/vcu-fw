/*
 * main.c
 *
 *  Created on: Mar 17, 2020
 *      Author: gabriel
 */

/* USER CODE BEGIN (0) */
/* USER CODE END */

/* Include Files */

#include "adc.h"
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "gio.h"
#include "het.h"
#include "os_queue.h"
#include "os_semphr.h"
#include "os_task.h"
#include "os_timer.h"
#include "reg_het.h"
#include "sys_common.h"
#include "system.h" // is this required?
#include "MCP48FV_DAC_SPI.h" // DAC library written by Ataur Rehman
#include "LV_monitor.h"      // INA226 Current Sense Amplifier Library written by David Cao
#include "IMD.h"             // Bender IR155 IMD Library written by Sumreen Rattan
#include "FreeRTOS.h"
#include "Phantom_sci.h"
#include "stdlib.h" // stdlib.h has ltoa() which we use for our simple SCI printing routine.
#include <stdio.h>
#include "reg_het.h"

//#include "task_data_logging.h"
//#include "task_sensor_read.h"
//#include "task_statemachine.h"
//#include "task_throttle.h"
//#include "task_watchdog.h"
#include "phantom_freertos.h"  // contains functions for freertos startup, timer setup, and task creation

#include "vcu_data.h" // holds VCU data structure

#include "board_hardware.h"   // contains hardware defines for specific board used (i.e. VCU or launchpad)

/* USER CODE END */

/** @fn void main(void)
*   @brief Application main function
*   @note This function is empty by default.
*
*   This function is called after startup.
*   The user can use this function to implement the application.
*/

/* USER CODE BEGIN (2) */


bool INTERRUPT_AVAILABLE;
bool THROTTLE_AVAILABLE;

/* array to hold handles to the created timers*/
TimerHandle_t xTimers[NUMBER_OF_TIMERS];

/*********************************************************************************
 *                          STATE ENUMERATION
 *********************************************************************************/
//typedef enum {TRACTIVE_OFF, TRACTIVE_ON, RUNNING, FAULT} State;
State state = TRACTIVE_OFF;

/*********************************************************************************
 *                          INITIALIZE DATA STRUCTURE...
 *                          or can this be done and outputted in the init function.. hm
 *********************************************************************************/

data VCUData;

data* VCUDataPtr = &VCUData;


uint8 i;
char command[8]; // used for ADC printing.. this is an array of 8 chars, each char is 8 bits
long xStatus;

/*********************************************************************************
 *                               SYSTEM STATE FLAGS
 *********************************************************************************/
//uint8_t TSAL = 0;
//uint8_t RTDS = 0;
long RTDS_RAW = 0;
//uint8_t BMS  = 1;
//uint8_t IMD = 1;
//uint8_t BSPD = 1;
//uint8_t BSE_FAULT = 0;

// ^^^^^^^^^^^^^^^ these should all be inside the data structure now

/*********************************************************************************
                 ADC FOOT PEDAL AND APPS STUFF (SHOULD GENERALIZE THIS)
 *********************************************************************************/
adcData_t FP_data[3];
adcData_t *FP_data_ptr = &FP_data[0];
unsigned int FP_sensor_1_sum = 0;
unsigned int FP_sensor_1_avg;
unsigned int FP_sensor_2_sum = 0;
unsigned int FP_sensor_2_avg;

unsigned int BSE_sensor_sum  = 0;
unsigned int BSE_sensor_avg  = 0;
unsigned int NumberOfChars;

uint16 FP_sensor_1_min = 0;
uint16 FP_sensor_2_min = 0;

uint16 FP_sensor_1_max = 4095; // 12-bit ADC
uint16 FP_sensor_2_max = 4095; // 12-bit ADC
uint16 FP_sensor_1_percentage;
uint16 FP_sensor_2_percentage;
uint16 FP_sensor_diff;


// change to better data type
//int lv_current = 0;


/* USER CODE END */

int main(void)
{

/* USER CODE BEGIN (3) */
/*********************************************************************************
 *                          HALCOGEN PERIPHERAL INITIALIZATION
 *********************************************************************************/
    _enable_IRQ();
    sciInit();
    gioInit();
    adcInit();
    hetInit();


//    pwmStop(BUZZER_PORT, READY_TO_DRIVE_BUZZER); // stop the ready to drive buzzer PWM from starting automatically
//
//    // turn off RGB LEDs
//    pwmStart(RGB_LED_PORT, BLUE_LED); // blue
//    pwmStart(RGB_LED_PORT, GREEN_LED); // green
//    pwmStart(RGB_LED_PORT, RED_LED); // red
    // maybe this can be changed in halcogen?

    // initialize HET pins ALL to output.. may need to change this later
//    gioSetDirection(hetPORT1, 0xFFFFFFFF);


    initData(VCUDataPtr); // maybe i return the data structure here?

    //using MCP48FV Library
    MCP48FV_Init();
//    MCP48FV_Set_Value(400);//500 =5.00V, 250= 2.5V


//    lv_monitorInit();   // LV monitor library


    initializeIMD();     // IMD Library

    phantom_freeRTOSInit();

    // start freeRTOS task scheduler
    vTaskStartScheduler();

    // infinite loop to prevent code from ending. The scheduler will now pre-emptively switch between tasks.
    while(1);
/* USER CODE END */

    return 0;
}
/* USER CODE BEGIN (4) */

/*********************************************************************************
 *                          READY TO DRIVE SIGNAL INTERRUPT
 *********************************************************************************/
void gioNotification(gioPORT_t *port, uint32 bit)
{
/*  enter user code between the USER CODE BEGIN and USER CODE END. */
/* USER CODE BEGIN (19) */
//    UARTSend(PC_UART, "---------Interrupt Request-------\r\n");
    if (port == gioPORTA && bit == 2 && INTERRUPT_AVAILABLE)
    {
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        // RTDS switch
        UARTSend(PC_UART, "---------Interrupt Active\r\n");
        if (VCUDataPtr->DigitalVal.RTDS == 0 && gioGetBit(gioPORTA, 2) == 0)
        {
            if (BSE_sensor_sum < 2000)
            {
                gioSetBit(gioPORTA, 6, 1);
                VCUDataPtr->DigitalVal.RTDS = 1; // CHANGE STATE TO RUNNING
                UARTSend(PC_UART, "---------RTDS set to 1 in interrupt\r\n");

                // ready to drive buzzer, need to start a 2 second timer here
//                pwmStart(BUZZER_PORT, READY_TO_DRIVE_BUZZER);

                // reset the 2 second timer to let the buzzer ring for 2 seconds
                if (xTimerResetFromISR(xTimers[1], xHigherPriorityTaskWoken) != pdPASS)// after 2s the timer will allow the interrupt to toggle the signal again
                {
                    // timer reset failed
                    UARTSend(PC_UART, "---------Timer reset failed-------\r\n");
                }
            }
        }
//        else
//        {
//            UARTSend(PC_UART, "---------RTDS set to 0 in interrupt\r\n");
//            RTDS = 0;
//        }

        INTERRUPT_AVAILABLE = false;
        if (xTimerResetFromISR(xTimers[0], xHigherPriorityTaskWoken) != pdPASS)// after 300ms the timer will allow the interrupt to toggle the signal again
        {
            // timer reset failed
            UARTSend(PC_UART, "---------Timer reset failed-------\r\n");
        }
    }
}

/* USER CODE END */
