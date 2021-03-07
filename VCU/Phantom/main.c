/*
 * main.c
 *
 *  Created on: Mar 17, 2020
 *      Author: gabriel
 */

/* USER CODE BEGIN (0) */

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
#include "RTD_Buzzer.h"      // Ready to Drive buzzer wrapper written by Gabriel Soares
#include "RGB_LED.h"         // RGB LED wrapper
#include "Phantom_sci.h"     // UART wrapper written by Mahmoud Kamaleldin

#include "phantom_freertos.h" // contains functions for freertos startup, timer setup, and task creation
#include "vcu_data.h"         // holds VCU data structure
#include "board_hardware.h"   // contains hardware defines for specific board used (i.e. VCU or launchpad)

#include "task_eeprom.h"


State state = TRACTIVE_OFF;   // needs to be stored in VCU data structure and referenced from there

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
/* ++ Added by jjkhan */

data* VCUDataPtr = &VCUData;         // Pointer to VCU Data structure
TaskHandle_t eepromHandler = NULL;   //Eeprom task handler
TaskHandle_t stateMachineHandler = NULL;
SemaphoreHandle_t vcuKey;        // Mutex to protect VCU data structure
SemaphoreHandle_t powerfailureFlagKey;

void *pVCUDataStructure = &VCUData;  // Need this void pointer to read from eeprom bank

//++ For testing Purposes - simulating vehicle state change and testing eeprom task.
TaskHandle_t testingEepromHandler = NULL;

/*
 *
 * Note: Some of the fault conventions are not intuitive, i.e. some places '1' means healthy/no-fault and some places '1' means 'flag set' - Will fix this later.
 *
 *   The following Task is introducing a software fault by toggling one of the FAULT flags available
 *      I'm toggling the TSAL line, which is a flag set when the AIRs are opened due to a fault, resulting in deactivation of tractive system;
 *          For TSAL_FAULT, '1' means TSAL healthy, i.e. no fault, if '0' -> there is a fault, state = tractive system off
 *
 *
 *   The State machine Flow for now:
 *      At power-on --> In TRACTIVE_SYSTEM_OFF:
 *                          If BMS, IMD, BSPD, TSAL, BSE are healthy --> VCU state changes to TRACTIVE_SYSTEM_ON, (note: car still not moving)
 *      In TRACTIVE_SYSTEM_ON state:
 *                      If Ready To Drive Set (RTDS) is set --> VCU state changes to RUNNING, car is now moving..
 *      In RUNNING state:
 *                      If the BMS, IMD, BSPD, TSAL, BSE are healthy and RTDS is set --> Continue in RUNNING state
 *                      If the BMS, IMD, BSPD, TSAL, BSE are healthy and RTDS no set --> VCU state changes to TRACTIVE_SYSTEM_ON, car not moving, i.e. slowing down to halt.
 *                      If RTDS is set and BMS or IMD or BSPD or TSAL signal are fault -->  VCU state changes to FAULT state -> Car still moving or do we slow down?
 *      In FAULT state:
 *                      If BSE is healthy, then you will be stuck in FAULT state -> we need to implement how we get out of FAULT state.
 *                      If BSE is unhealthy, the VCU state changes to TRACTIVE_SYSTEM_OFF - i.e. car is shutdown.
 *
 *
 *      Based on the state machine set up right now, the following task will introduce a fault in one of the sub-system after power-on, which will place the
 *          VCU in FAULT state. Once in FAULT state, we need to introduct BSE fault, which will cause the shutdown, moving the VCU to TRACTIVE_SYSTEM_OFF state, which is the default state
 *              of VCU at power on.
 *
 *      The testEEprom is using the TSAL_FAULT for entering the FAULT state, but you could use any from BMS, IMD, BSPD, TSAL. It toggles the
 *          fault line every second. This lets you visualize that eeprom bank7 is getting updated periodically as VCU data structure value(s) are getting updated. -> Go to Memory window and open 0xF020 0000
 *      There is no implementation to get out FAULT state unless the BSE fault line is set, i.e. BSE signals a fault -> needs implementation.
 *      For now, after 10-seconds I introduce BSE fault, which will move VCU out of FAULT state and into TRACTIVE_SYSTEM_OFF -> basically a reset.
 *      Once you've set BSE fault, you can check the VCU data structure contents and then reset the board. On power-on, you should note that based on the value stored in
 *               POWER_FAILURE_FLAG of VCU data strcuture, the VCU data structure will either get initialized with values from eeprom memory or the default values.
 *                  If POWER_FAILURE_FLAG = 0xFF --> last shutdown not graceful--> load VCU data structure with data in eeprom memory.
 *                  If POWER_FAILURE_FLAG = 0x00 --> last shutdown graceful  --> Load VCU data structure with default values.
 *
 *
 *      Power on-> Observe how eeprom bank is getting updated periodically as VCU data structure values get updated.
 *              Once in FAULT state, the terminal will show FAULT state -> Reset the board.
*               On Power-on:
*                   Check eeprom last saved VCU data structure values - especially the last 2-data bytes (stores a flag which indicates if last saved VCUData was in a faulty state or not). Place VCUData in watch-window
*                       Next, Set break-point at the start of '(initializationOccured) if-block' -> this block will be entered after VCUData intialized at power-up by either default values or by last-updated eeprom bank values.
*                           Compare values of the VCUData in watch window with the eeprom block values.
*                               If last 2bytes of eeprom data = 0xFF, last saved VCU state was faulty, load VCUData with eeprom values.
*                               If last 2byte of eeprom data = 0x00, last saved VCU state was healthy, load VCUData with default values.
*
*
 *
 */
void testEeprom(void *p){
    TickType_t mylastTickCount;
    mylastTickCount = xTaskGetTickCount();
    while(1){
        if(xSemaphoreTake(vcuKey, pdMS_TO_TICKS(100))){
            VCUDataPtr->DigitalVal.TSAL_FAULT ^= (1<<0);       // Toggle Bit-0
            xSemaphoreGive(vcuKey);
        }
        vTaskDelayUntil(&mylastTickCount,pdMS_TO_TICKS(1000));  // Every 1s you toggle the shutdown signal - to check if eeprom is updated or not.

        if(VCUDataPtr->DigitalVal.TSAL_FAULT==0){  // TSAL_FAULT
            if(xSemaphoreTake(vcuKey, pdMS_TO_TICKS(100))){
                VCUDataPtr->DigitalVal.BSE_FAULT = 1;  // Send BSE fault
                xSemaphoreGive(vcuKey);
            }
        }
        vTaskDelayUntil(&mylastTickCount,pdMS_TO_TICKS(10000));  // Every 1s you toggle the shutdown signal - to check if eeprom is updated or not.
    }
}

//-- For testing Purposes - simulating vehicle state change and testing eeprom task.

/* -- Added by jjkhan */

/* USER CODE END */

void main(void)
{
/* USER CODE BEGIN (3) */

    /* Halcogen Initialization */

    _enable_IRQ();              // Enable interrupts
    sciInit();                  // Initialize UART (SCI) halcogen driver
    gioInit();                  // Initialize GPIO halcogen driver
    adcInit();                  // Initialize ADC halcogen driver
    hetInit();                  // Initialize HET (PWM) halcogen driver
    // Eeprom Initialization
    eepromBlocking_Init();
    vcuKey = xSemaphoreCreateMutex();


    /* Phantom Library Initialization */

    initData(VCUDataPtr);       // Initialize VCU Data Structure
    RTD_Buzzer_Init();          // Initialize Ready to Drive buzzer
    RGB_LED_Init();             // Initialize RGB LEDs to start off
    MCP48FV_Init();             // Initialize DAC Library
    lv_monitorInit();           // Initialize LV Monitor Library
    initializeIMD();            // Initialize IMD Library

    /* freeRTOS Initialization */

    phantom_freeRTOSInit();     // Initialize freeRTOS timers, queues, and tasks

    vTaskStartScheduler();      // start freeRTOS task scheduler
   
/*********************************************************************************
 *                          freeRTOS TASK & QUEUE CREATION
 *********************************************************************************/
    /* ++ Added by jjkhan */

    if (xTaskCreate(vEeprom, (const char*)"EepromTask",  150, NULL,  EEPROM_TASK_PRIORITY, &eepromHandler) != pdTRUE)
    {
            uint8 message[]="EEPROM task Creation Failed.\r\n";
            sciSend(PC_UART,(uint32)sizeof(message),&message[0]);
            while(1);
    }


    if (xTaskCreate(testEeprom, (const char*)"testEeprom",  150, NULL,  tskIDLE_PRIORITY, &testingEepromHandler) != pdTRUE)
    {
            uint8 message[]="Test task Creation Failed.\r\n";
            sciSend(PC_UART,(uint32)sizeof(message),&message[0]);
            while(1);
    }
    /* -- Added by jjkhan */

    // infinite loop to prevent code from ending. The scheduler will now pre-emptively switch between tasks.
    while(1);
/* USER CODE END */
    return 0;
}
/* USER CODE BEGIN (4) */
/* USER CODE END */
