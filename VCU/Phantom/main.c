/*
 * main.c
 *
 *  Created on: Mar 17, 2020
 *      Author: gabriel, Joshua Guo
 */

/* USER CODE BEGIN (0) */

#include "sci.h"
#include "gio.h"
#include "adc.h"
#include "het.h"

#include "RGB_LED.h"            // RGB LED wrapper written by Joshua Guo
// #include "RTD_Buzzer.h"         // Ready to Drive buzzer wrapper written by Gabriel Soares
// #include "MCP48FV_DAC_SPI.h"    // DAC library written by Ataur Rehman
// #include "LV_monitor.h"         // COMMENT OUT IF USING LAUNCHPAD
// #include "IMD.h"                // Bender IR155 IMD Library written by Sumreen Rattan
// #include "eeprom_driver.h"      // EEPROM driver written by Junaid Khan

#include "board_hardware.h"     // contains hardware defines for specific board used (i.e. VCU or launchpad)
#include "vcu_data.h"           // VCU Data structure interface written by Joshua Guo

#include "phantom_task.h"       // os_task wrapper written by Joshua Guo
// #include "Phantom_sci.h"     // UART wrapper written by Mahmoud Kamaleldin

// #include "execution_timer.h"

#include "task_test.h"
// #include "task_throttle.h"
// #include "task_sensor_read.h"
// #include "task_statemachine.h"
// #include "task_watchdog.h"
// #include "task_data_logging.h"
// #include "task_eeprom.h"

/* USER CODE END */

/* USER CODE BEGIN (4) */
void halcogenInit()
{
    /* Halcogen Initialization */
    _enable_IRQ();              // Enable interrupts        // which file is this function from? -josh
    sciInit();                  // Initialize UART (SCI) halcogen driver
    gioInit();                  // Initialize GPIO halcogen driver
    adcInit();                  // Initialize ADC halcogen driver
    hetInit();                  // Initialize HET (PWM) halcogen driver
    // eepromBlocking_Init();      // Initialization EEPROM Memory - added by jjkhan

#ifdef PMU_CYCLE
    // Initialize code execution timer
    timer_Init();
    // Set Port GIO_PORTA_5 as output pin - using it to confirm PMU timer value is in range of I/O toggle
    gioSetDirection(gioPORTA, 32);
#endif
}

void phantomDriversInit()
{
    /* Phantom Library Initialization */
    VCUData_init();             // Initialize VCU Data Structure
    // RTD_Buzzer_Init();          // Initialize Ready to Drive buzzer
    RGB_LED_Init();             // Initialize RGB LEDs to start off
    // MCP48FV_Init();             // Initialize DAC Library
#ifdef LV_MONITOR
    lv_monitorInit();           // Initialize LV Monitor Library
#endif
    // initializeIMD();            // Initialize IMD Library
}

void phantomTasksInit()
{
    // initalizations of tasks
    testTaskInit();
}
/* USER CODE END */

void main(void)
{
/* USER CODE BEGIN (3) */
    
    halcogenInit();
    phantomDriversInit();
    phantomTasksInit();

    // Phantom_startTaskScheduler is Blocking
    Phantom_startTaskScheduler();
   
/* USER CODE END */
}
