/*
 * main.c
 *
 *  Created on: Mar 17, 2020
 *  Author: gabriel, Joshua Guo
 */

/* USER CODE BEGIN (0) */
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
#include "vcu_data.h"           // VCU Data structure interface written by Joshua Guo

#include "phantom_queue.h"      // os_queue wrapper written by Joshua Guo
#include "phantom_task.h"       // os_task wrapper written by Joshua Guo
#include "Phantom_sci.h"     // UART wrapper written by Mahmoud Kamaleldin

#include "execution_timer.h"

#include "task_test.h"
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
     RTD_Buzzer_Init();          // Initialize Ready to Drive buzzer
    RGB_init();             // Initialize RGB LEDs to start off
     MCP48FV_Init();             // Initialize DAC Library
}

void phantomQueueInit()
{
    QueueHandle_t statusQ = Phantom_createQueue(10, sizeof(int)),
                  stateThrottleQ = Phantom_createQueue(1, sizeof(int)),
                  stateInterruptQ = Phantom_createQueue(1, sizeof(int)),
                  pedalQ = Phantom_createQueue(1, sizeof(int) * 3),
                  interruptQ = Phantom_createQueue(10, sizeof(char)),
                  printQ = Phantom_createQueue(32, sizeof(char) * 8);
    
    QueueSetHandle_t throttleQSet = Phantom_createQueueSet(sizeof(int) * 4),
                     interruptQSet = Phantom_createQueueSet(sizeof(int) + (10 * sizeof(char)));
    
    // add queues to receive sets
    Phantom_addToQueueSet(throttleQSet, pedalQ);
    Phantom_addToQueueSet(throttleQSet, stateThrottleQ);
    Phantom_addToQueueSet(interruptQSet, interruptQ);
    Phantom_addToQueueSet(interruptQSet, stateInterruptQ);

    // set tasks' references to their corresponding queues
    Task_StateMachineSetSendQueue1(stateInterruptQ);
    Task_StateMachineSetSendQueue2(stateThrottleQ);
    Task_StateMachineSetReadQueue(statusQ);
    Task_throttleActorSetSendQueue(statusQ);
    Task_throttleActorSetReadQueueSet(throttleQSet);
}

void phantomTasksInit()
{
    // initalizations of tasks
//    Task_testInit();
    ReceiveTaskInit();
    Task_throttleActorInit();
}
volatile unsigned long ulHighFrequencyTimerTicks;
void rtiNotification(uint32 notification)
{
    ulHighFrequencyTimerTicks++;
    // should probably add a check to see if the right timer was called but test board only has one active hardware timer atm


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

    phantomQueueInit();
    phantomTasksInit();

    // Phantom_startTaskScheduler is Blocking
    Phantom_startTaskScheduler();
   
/* USER CODE END */
}
