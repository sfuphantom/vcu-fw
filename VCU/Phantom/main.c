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

// ++ Added by jjkhan - this has the function call to switch between user and system mode - i.e. raising privileges- need it for task profiling.
        // Will move the swiSwitchToMode ( uint32 mode ) to a central location once task profiling works.
#include "eeprom_driver.h"

#define RUN_TIME_STATS                      0        // For compiling Timer Configuration and getting timer value in vTaskScheduler for task profiling with vTaskGetRunTimeStats
#define RUN_TIME_STATS_EEPROM               0         // Using FreeRTOS Run Time Stats inside eeprom task
#define RUN_TIME_STATS_THROTTLE             0         // Using FreeRTOS Run Time Stats inside throttle task
#define RUN_TIME_STATS_SENSOR_READ          0         // Using FreeRTOS Run Time Stats inside sensor read task
#define RUN_TIME_STATS_STATE_MACHINE        0        // Using FreeRTOS Run Time Stats inside state machine task
#define RUN_TIME_STATS_DATA_LOGGING         0        // Using FreeRTOS Run Time Stats inside Data logging task

// -- Added by jjkhan

State state = TRACTIVE_OFF;   // needs to be stored in VCU data structure and referenced from there

data VCUData;
data* VCUDataPtr = &VCUData;

void *pVCUDataStructure = &VCUData;  // Need this void pointer to read from eeprom bank - Added by jjkhan

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

//++ Added by jjkhan for Code execution Time counter using the PMU module
//#define PMU_CYCLE
#include "execution_timer.h"
/*
#ifdef PMU_CYCLE

#include "execution_timer.h"
#define CPU_CLOCK_MHz (float) 160.0

volatile unsigned long cycles_PMU_start; // CPU cycle count at start
volatile float time_PMU_code_uSecond; // the calculated time in uSecond.

#endif
*/

//-- Added by jjkhan


/* USER CODE END */

void main(void)
{
/* USER CODE BEGIN (3) */

#ifdef PMU_CYCLE
       // Initialize code t screimer.
       timer_Init();
       gioSetDirection(gioPORTA, 32);
#endif

    /* Halcogen Initialization */

    _enable_IRQ();              // Enable interrupts
    sciInit();                  // Initialize UART (SCI) halcogen driver
    gioInit();                  // Initialize GPIO halcogen driver
    adcInit();                  // Initialize ADC halcogen driver
    hetInit();                  // Initialize HET (PWM) halcogen driver
    eepromBlocking_Init();      // Initialization EEPROM Memory - added by jjkhan

#ifdef PMU_CYCLE
       gioSetDirection(gioPORTA, 32); // Set Port GIO_PORTA_5 as output pin
#endif
#ifdef PMU_CYCLE_1
       // Start timer.
       gioSetDirection(gioPORTA, 32);
       cycles_PMU_start = timer_Start();
       gioToggleBit(gioPORTA, 5);

#endif
    /* Phantom Library Initialization */

    initData(VCUDataPtr);       // Initialize VCU Data Structure
    RTD_Buzzer_Init();          // Initialize Ready to Drive buzzer
    RGB_LED_Init();             // Initialize RGB LEDs to start off
    MCP48FV_Init();             // Initialize DAC Library

#if LV_MONITOR
    lv_monitorInit();           // Initialize LV Monitor Library
#endif
    initializeIMD();            // Initialize IMD Library

    /* freeRTOS Initialization */

    phantom_freeRTOSInit();     // Initialize freeRTOS timers, queues, and tasks

#ifdef PMU_CYCLE_1
    //gioToggleBit(gioPORTA, 5);
    //gioSetBit(gioPORTA, 5, 0);
    time_PMU_code_uSecond = timer_Stop(cycles_PMU_start, CPU_CLOCK_MHz);
    gioToggleBit(gioPORTA, 5);
#endif
    vTaskStartScheduler();      // start freeRTOS task scheduler
   
    // infinite loop to prevent code from ending. The scheduler will now pre-emptively switch between tasks.
    while(1);
/* USER CODE END */
}
/* USER CODE BEGIN (4) */

/* ++ Added by jjkhan - will move it once task profiling works. */

/*
#ifdef RUN_TIME_STATS
// Configure a Timer that will be used by FreeRTOS to create runtime stats
// This function will be called by the vTaskStartScheduler() -> Application layer provides the definition.
void initializeProfiler(){
    /* Enable PMU Cycle Counter for Profiling
    swiSwitchToMode(SYSTEM_MODE);
    _pmuInit_();
    _pmuEnableCountersGlobal_();
    _pmuResetCycleCounter_();
    _pmuStartCounters_(pmuCYCLE_COUNTER);
    swiSwitchToMode(USER_MODE);
}

// This function called by vTaskGetRunTimeStats() to create an ASCII table of all Task stat -> Application layer provides the definition.

uint32_t getProfilerTimerCount(){
    uint32_t cycleCount;
    swiSwitchToMode(SYSTEM_MODE);
    cycleCount = _pmuGetCycleCount_();
    swiSwitchToMode(USER_MODE);
    return cycleCount;
}
#endif
*/
/* -- Added by jjkhan - will move it once task profiling works. */
/* USER CODE END */
