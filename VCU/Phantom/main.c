/*
 * main.c
 *
 *  Created on: Mar 17, 2020
 *  Author: gabriel, Joshua Guo
 */

/* USER CODE BEGIN (0) */

// uncomment to switch to simulation mode
// #define VCU_SIM_MODE

#include "rti.h"
#include "sci.h"
#include "gio.h"
#include "adc.h"
#include "het.h"

#include "RGB_LED.h"            // RGB LED wrapper written by Joshua Guo
#include "RTD_Buzzer.h"         // Ready to Drive buzzer wrapper written by Gabriel Soares
#include "MCP48FV_DAC_SPI.h"    // DAC library written by Ataur Rehman
#include "eeprom_driver.h"      // EEPROM driver written by Junaid Khan

#include "board_hardware.h"     // contains hardware defines for specific board used (i.e. VCU or launchpad)
#include "vcu_data.h"           // VCU Data structure interface written by Joshua Guo (DEPRECATED)

#include "phantom_queue.h"      // os_queue wrapper written by Joshua Guo
#include "phantom_task.h"       // os_task wrapper written by Joshua Guo
#include "Phantom_sci.h"     // UART wrapper written by Mahmoud Kamaleldin

#include "execution_timer.h"

#include "task_test.h"
#include "task_interrupt.h"
#include "task_receive.h"
#include "task_throttle_actor.h"
#include "task_statemachine.h"
#include "task_watchdog.h"
#include "task_eeprom.h"

/* USER CODE END */

/* USER CODE BEGIN (4) */
void halcogenInit()
{
    /* Halcogen Initialization */
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
    RTD_Buzzer_Init();          // Initialize Ready to Drive buzzer
    RGB_init();             // Initialize RGB LEDs to start off
}

void phantomTasksInit()
{
//    Task_testInit();
    ReceiveTaskInit();
    ThrottleInit();
    InterruptInit();

}
volatile unsigned long ulHighFrequencyTimerTicks;
void rtiNotification(uint32 notification)
{
    ulHighFrequencyTimerTicks++;
    // should probably add a check to see if the right timer 
    // was called but VCU only has one active hardware timer atm
}
/* USER CODE END */



void main(void)
{
/* USER CODE BEGIN (3) */
    rtiInit();
    rtiEnableNotification(rtiNOTIFICATION_COMPARE0);
    _enable_IRQ();

    halcogenInit();
    phantomDriversInit();

    phantomTasksInit();

    // Phantom_startTaskScheduler is Blocking
    Phantom_startTaskScheduler();
   
/* USER CODE END */
}
