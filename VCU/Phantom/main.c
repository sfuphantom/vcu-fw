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
#include "gio.h"
#include "het.h"
#include "reg_het.h"
#include "sys_common.h"
#include "system.h"          // is this required?
#include "stdlib.h"          // stdlib.h has ltoa() which we use for our simple SCI printing routine.
#include <stdio.h>
#include "reg_het.h"

#include "MCP48FV_DAC_SPI.h" // DAC library written by Ataur Rehman
#include "LV_monitor.h"      // INA226 Current Sense Amplifier Library written by David Cao
#include "IMD.h"             // Bender IR155 IMD Library written by Sumreen Rattan
#include "Phantom_sci.h"

#include "phantom_freertos.h" // contains functions for freertos startup, timer setup, and task creation
#include "vcu_data.h"         // holds VCU data structure
#include "board_hardware.h"   // contains hardware defines for specific board used (i.e. VCU or launchpad)


State state = TRACTIVE_OFF;          // needs to be stored in VCU data structure and referenced from there

data VCUData;
data* VCUDataPtr = &VCUData;

/* USER CODE END */

/** @fn void main(void)
*   @brief Application main function
*   @note This function is empty by default.
*
*   This function is called after startup.
*   The user can use this function to implement the application.
*/

/* USER CODE BEGIN (2) */
/* USER CODE END */

int main(void)
{
/* USER CODE BEGIN (3) */
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

    MCP48FV_Init();             // DAC Library

//    lv_monitorInit();         // LV monitor library

    initializeIMD();            // IMD Library

    phantom_freeRTOSInit();     // initialize freeRTOS timers, queues, and tasks

    vTaskStartScheduler();      // start freeRTOS task scheduler

    // infinite loop to prevent code from ending. The scheduler will now pre-emptively switch between tasks.
    while(1);
/* USER CODE END */

    return 0;
}
/* USER CODE BEGIN (4) */
/* USER CODE END */
