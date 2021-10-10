/*
 * main.c
 *
 *  Created on: Mar 17, 2020
 *      Author: gabriel
 */

/* USER CODE BEGIN (0) */

/* Include Files */

#include <Phantom/support/execution_timer.h>  // Code Execution Timer Module written Junaid Khan
#include "adc.h"
#include "gio.h"
#include "het.h"
#include "reg_het.h"
#include "sys_common.h"
#include "system.h"          // is this required?
#include "stdlib.h"          // stdlib.h has ltoa() which we use for our simple SCI printing routine.
#include <stdio.h>
#include "reg_het.h"
#include "mibspi.h"
#include "sys_core.h"


#include "MCP48FV_DAC_SPI.h" // DAC library written by Ataur Rehman
#include "LV_monitor.h"      // INA226 Current Sense Amplifier Library written by David Cao
#include "IMD.h"             // Bender IR155 IMD Library written by Sumreen Rattan
#include "RTD_Buzzer.h"      // Ready to Drive buzzer wrapper written by Gabriel Soares
#include "RGB_LED.h"         // RGB LED wrapper
#include "Phantom_sci.h"     // UART wrapper written by Mahmoud Kamaleldin
#include "hv_driver.h"       // HV_VS library written by Yash Vijay

#include "phantom_freertos.h" // contains functions for freertos startup, timer setup, and task creation
#include "vcu_data.h"         // holds VCU data structure
#include "board_hardware.h"   // contains hardware defines for specific board used (i.e. VCU or launchpad)

#include "execution_timer.h"

State state = TRACTIVE_OFF;   // needs to be stored in VCU data structure and referenced from there
data VCUData;
data* VCUDataPtr = &VCUData;


// ++ Added by jjkhan:
     // Uncomment the following directive if the current sensor module is connected.
    // VCU has it on-board, launchpad doesn't -> will get stuck in lv_monitorInit()
//#define LV_MONITOR
// -- Added by jjkhan

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

void main(void)
{
/* USER CODE BEGIN (3) */



#ifdef PMU_CYCLE
       // Initialize code execution timer
       timer_Init();
#endif
    /* Halcogen Initialization */

    _enable_IRQ();              // Enable interrupts
    sciInit();                  // Initialize UART (SCI) halcogen driver
    gioInit();                  // Initialize GPIO halcogen driver
    adcInit();                  // Initialize ADC halcogen driver
    hetInit();                  // Initialize HET (PWM) halcogen driver
    eepromBlocking_Init();      // Initialization EEPROM Memory - added by jjkhan
    mibspiInit();


#ifdef PMU_CYCLE
        // Set Port GIO_PORTA_5 as output pin - using it to confirm PMU timer value is in range of I/O toggle
       gioSetDirection(gioPORTA, 32);
#endif
    /* Phantom Library Initialization */
    //initData(VCUDataPtr);       // Initialize VCU Data Structure
    RTD_Buzzer_Init();          // Initialize Ready to Drive buzzer
    RGB_LED_Init();             // Initialize RGB LEDs to start off
    MCP48FV_Init();             // Initialize DAC Library

#if LV_MONITOR
    lv_monitorInit();           // Initialize LV Monitor Library
#endif

#if HV_VS
    /* Slave Data */
    adcSlaveDataSetup();
    masterDataTranser();          // Transfer slave data to master
#endif

    initializeIMD();            // Initialize IMD Library

    /* freeRTOS Initialization */
    phantom_freeRTOSInit();     // Initialize freeRTOS timers, queues, and tasks
    vTaskStartScheduler();      // start freeRTOS task scheduler
   
    // infinite loop to prevent code from ending. The scheduler will now pre-emptively switch between tasks.

    while(1);

/* USER CODE END */
}
/* USER CODE BEGIN (4) */
/* USER CODE END */
