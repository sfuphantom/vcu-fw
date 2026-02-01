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
#include "can.h"

#include "RGB_LED.h"            // RGB LED wrapper written by Joshua Guo
#include "RTD_Buzzer.h"         // Ready to Drive buzzer wrapper written by Gabriel Soares
#include "MCP48FV_DAC_SPI.h"    // DAC library written by Ataur Rehman
#include "eeprom_driver.h"      // EEPROM driver written by Junaid Khan

#include "board_hardware.h"     // contains hardware defines for specific board used (i.e. VCU or launchpad)
#include "vcu_data.h"           // VCU Data structure interface written by Joshua Guo (DEPRECATED)
#include "vcu_common.h"    

#include "Phantom_sci.h"     // UART wrapper written by Mahmoud Kamaleldin
#include "state_machine.h"

#include "execution_timer.h"


#include "task_throttle.h"
#include "task_pedal_readings.h"
#include "task_event_handler.h"
#include "task_logger.h"

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

    // Add RTD switch GPIO configuration
    gioSetDirection(READY_TO_DRIVE_PORT, 0); //GPIO pin should be configured as an input, 0 being input
    //gioSetPullup(READY_TO_DRIVE_PORT, READY_TO_DRIVE_PIN, gioPULLUP_ENABLE); //  Enables the internal pull-up resistor for a GPIO pin, might not need
    gioEnableNotification(READY_TO_DRIVE_PORT, READY_TO_DRIVE_PIN);
    
    // Enable interrupts
    _enable_IRQ();
}

void phantomDriversInit()
{
    /* Phantom Library Initialization */
    VCUData_init();             // Initialize VCU Data Structure
    RTD_Buzzer_Init();          // Initialize Ready to Drive buzzer
    RGB_init();             // Initialize RGB LEDs to start off

//    rtiInit();
//    rtiEnableNotification(rtiNOTIFICATION_COMPARE1);
//    _enable_IRQ();
//    rtiStartCounter(rtiNOTIFICATION_COUNTER1);
//    uint32 x= rtiGetPeriod(rtiNOTIFICATION_COMPARE1);
}

void phantomTasksInit()
{
    // Parameters for xTaskCreate:
    // - pxTaskCode  = Function the task will execute (Task_CANMonitor).
    // - pcName      = Name of the task ("CANMonitor").
    // - useStackDepth = Stack size (512 bytes).
    // - pvParameters = No parameters passed to Task_CANMonitor() (NULL).
    // - uxPriority  = Task priority is 2 (higher value = higher priority).
    // - pxCreatedTask = Task handle (NULL means handle is not used).
    xTaskCreate(Task_CANMonitor, "CANMonitor", 512, NULL, 2, NULL);
    SystemTasks_t t = {
        .EventHandler=EventHandlerInit(),
        .Logger=LoggerInit(),
        .Throttle=ThrottleInit(),
        .PedalReadings=PedalReadingsInit()
    };

    if (!all(4, t.EventHandler, t.Logger, t.Throttle, t.PedalReadings))
    {
        while(1) UARTprintln("Some tasks not initialized: %d, %d, %d, %d", t.EventHandler, t.Logger, t.Throttle, t.PedalReadings);
    }

    /* Phantom Library Initialization */
    VCUData_init();             // Initialize VCU Data Structure
    RTD_Buzzer_Init();          // Initialize Ready to Drive buzzer
    RGB_init();                 // Initialize RGB LEDs
    
    // Initialize RTD switch state
    uint8_t initialRTDState = gioGetBit(READY_TO_DRIVE_PORT, READY_TO_DRIVE_PIN);
    sendEventToStateMachine(EV_RTD_SWITCH_CHANGED, initialRTDState);

    StateMachineInit(t);
}

//Retrieve CAN data
void Task_CANMonitor(void *pvParameters)
{
    while (1) {
        receiveDAQMessage(); // Call the function from can.c
        vTaskDelay(pdMS_TO_TICKS(100)); // Check CAN messages every 100ms
    }
}


volatile unsigned long ulHighFrequencyTimerTicks;

// TODO: Never actually triggered
void rtiNotification(uint32 notification)
{
    ulHighFrequencyTimerTicks++;
    // should probably add a check to see if the right timer
    // was called but VCU only has one active hardware timer atm
}
/* USER CODE END */

/* USER CODE BEGIN (4) */
// RTD Switch Interrupt Handler
#pragma INTERRUPT(rtdSwitchISR, IRQ)
void rtdSwitchISR(void)
{
    // Clear interrupt flag
    gioSetBit(READY_TO_DRIVE_PORT, READY_TO_DRIVE_PIN, 0);
    
    // Read current switch state
    uint8_t rtdState = gioGetBit(READY_TO_DRIVE_PORT, READY_TO_DRIVE_PIN);
    
    // Notify state machine
    sendEventToStateMachine(EV_RTD_SWITCH_CHANGED, rtdState);
}
/* USER CODE END */

void main(void)
{
/* USER CODE BEGIN (3) */

    halcogenInit();

    phantomDriversInit();

    UARTInit(PC_UART, 460800); // something up above overwrites the configuration set here. Make sure this goes last!

    phantomTasksInit();

    UARTprintln("Starting scheduler!");

    vTaskStartScheduler();

    while(1) UARTprintf("Scheduler exited!");
   
/* USER CODE END */
}
