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

<<<<<<< HEAD
//#include <Phantom/tasks/headers/task_data_logging.h>
//#include <Phantom/tasks/headers/task_sensor_read.h>
//#include <Phantom/tasks/headers/task_statemachine.h>
//#include <Phantom/tasks/headers/task_throttle.h>
//#include <Phantom/tasks/headers/task_watchdog.h>

#include "task_data_logging.h"
#include "task_sensor_read.h"
#include "task_statemachine.h"
#include "task_throttle.h"
#include "task_watchdog.h"
=======
#include "MCP48FV_DAC_SPI.h" // DAC library written by Ataur Rehman
#include "LV_monitor.h"      // INA226 Current Sense Amplifier Library written by David Cao
#include "IMD.h"             // Bender IR155 IMD Library written by Sumreen Rattan
#include "RTD_Buzzer.h"      // Ready to Drive buzzer wrapper written by Gabriel Soares
#include "RGB_LED.h"         // RGB LED wrapper
#include "Phantom_sci.h"     // UART wrapper written by Mahmoud Kamaleldin

#include "phantom_freertos.h" // contains functions for freertos startup, timer setup, and task creation
#include "vcu_data.h"         // holds VCU data structure
#include "board_hardware.h"   // contains hardware defines for specific board used (i.e. VCU or launchpad)
>>>>>>> 0afe16f869cdcfbfaa2f256f02ee721cccb7ee98


<<<<<<< HEAD
//#include <Phantom/tasks/headers/priorities.h>

#include "vcu_data.h" // holds VCU data structure

//#include <Phantom/data_structures/vcu_data.h>

#include "board_hardware.h"   // contains hardware defines for specific board used (i.e. VCU or launchpad)
=======
State state = TRACTIVE_OFF;   // needs to be stored in VCU data structure and referenced from there

data VCUData;
data* VCUDataPtr = &VCUData;
>>>>>>> 0afe16f869cdcfbfaa2f256f02ee721cccb7ee98

//#include <Phantom/hardware/launchpad_hw/board_hardware.h>



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

    /* Halcogen Initialization */

<<<<<<< HEAD
    // LV monitor library
    lv_monitorInit();

    //Shutdown Driver
    ShutdownInit();
/*********************************************************************************
 *                          freeRTOS SOFTWARE TIMER SETUP
 *********************************************************************************/
    xTimers[0] = xTimerCreate
            ( /* Just a text name, not used by the RTOS
             kernel. */
             "RTDS_Timer",
             /* The timer period in ticks, must be
             greater than 0. */
             pdMS_TO_TICKS(10),
             /* The timers will auto-reload themselves
             when they expire. */
             pdFALSE,
             /* The ID is used to store a count of the
             number of times the timer has expired, which
             is initialised to 0. */
             ( void * ) 0,
             /* Callback function for when the timer expires*/
             Timer_300ms
           );
=======
    _enable_IRQ();              // Enable interrupts
    sciInit();                  // Initialize UART (SCI) halcogen driver
    gioInit();                  // Initialize GPIO halcogen driver
    adcInit();                  // Initialize ADC halcogen driver
    hetInit();                  // Initialize HET (PWM) halcogen driver
>>>>>>> 0afe16f869cdcfbfaa2f256f02ee721cccb7ee98

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

    // infinite loop to prevent code from ending. The scheduler will now pre-emptively switch between tasks.
    while(1);
/* USER CODE END */
    return 0;
}
/* USER CODE BEGIN (4) */
<<<<<<< HEAD

/*********************************************************************************
 *                          freeRTOS TASK IMPLEMENTATIONS
 *********************************************************************************/
void gioNotification(gioPORT_t *port, uint32 bit)
{
/*  enter user code between the USER CODE BEGIN and USER CODE END. */
/* USER CODE BEGIN (19) */

    /*********************************** SHUTDOWN SIGNALS ***************************************/
    if(port == SHUTDOWN_CIRCUIT_PORT && bit == BMS_FAULT_PIN) VCUDataPtr->DigitalVal.BMS_FAULT = true;

    if(port == SHUTDOWN_CIRCUIT_PORT && bit == BSPD_FAULT_PIN) VCUDataPtr->DigitalVal.BSPD_FAULT = true;

    if(port == SHUTDOWN_CIRCUIT_PORT && bit == IMD_FAULT_PIN) VCUDataPtr->DigitalVal.IMD_FAULT = true;

    //      (Shutdown Board SHOULD be Triggered)  &&  (TSAL_HV == ON) ----> TSAL_WELDED
    if((VCUDataPtr->DigitalVal.BMS_FAULT || VCUDataPtr->DigitalVal.IMD_FAULT || VCUDataPtr->DigitalVal.BSPD_FAULT) && gioGetBit(TSAL_PORT,TSAL_ACTIVE_PIN))

        VCUDataPtr->DigitalVal.TSAL_WELDED = true;




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
                pwmStart(BUZZER_PORT, READY_TO_DRIVE_BUZZER);

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

/* Timer callback when it expires */
 void Timer_300ms(TimerHandle_t xTimers)
 {
     INTERRUPT_AVAILABLE = true;
 }

 /* Timer callback when it expires for the ready to drive sound */
 void Timer_2s(TimerHandle_t xTimers)
 {
     pwmStop(BUZZER_PORT, READY_TO_DRIVE_BUZZER);
     THROTTLE_AVAILABLE = true;
 }

void vApplicationMallocFailedHook( void )
{
    // Error handling if malloc fails to allocate memory properly
    // Write some code here
}

/* configSUPPORT_STATIC_ALLOCATION is set to 1, so the application must provide an
implementation of vApplicationGetIdleTaskMemory() to provide the memory that is
used by the Idle task. */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer,
                                    StackType_t **ppxIdleTaskStackBuffer,
                                    uint32_t *pulIdleTaskStackSize )
{
/* If the buffers to be provided to the Idle task are declared inside this
function then they must be declared static – otherwise they will be allocated on
the stack and so not exists after this function exits. */
static StaticTask_t xIdleTaskTCB;
static StackType_t uxIdleTaskStack[ configMINIMAL_STACK_SIZE ];

    /* Pass out a pointer to the StaticTask_t structure in which the Idle task’s
    state will be stored. */
    *ppxIdleTaskTCBBuffer = &xIdleTaskTCB;

    /* Pass out the array that will be used as the Idle task’s stack. */
    *ppxIdleTaskStackBuffer = uxIdleTaskStack;

    /* Pass out the size of the array pointed to by *ppxIdleTaskStackBuffer.
    Note that, as the array is necessarily of type StackType_t,
    configMINIMAL_STACK_SIZE is specified in words, not bytes. */
    *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}
/*———————————————————–*/

/* configSUPPORT_STATIC_ALLOCATION and configUSE_TIMERS are both set to 1, so the
application must provide an implementation of vApplicationGetTimerTaskMemory()
to provide the memory that is used by the Timer service task. */
void vApplicationGetTimerTaskMemory( StaticTask_t **ppxTimerTaskTCBBuffer,
                                     StackType_t **ppxTimerTaskStackBuffer,
                                     uint32_t *pulTimerTaskStackSize )
{
/* If the buffers to be provided to the Timer task are declared inside this
function then they must be declared static – otherwise they will be allocated on
the stack and so not exists after this function exits. */
static StaticTask_t xTimerTaskTCB;
static StackType_t uxTimerTaskStack[ configTIMER_TASK_STACK_DEPTH ];

    /* Pass out a pointer to the StaticTask_t structure in which the Timer
    task’s state will be stored. */
    *ppxTimerTaskTCBBuffer = &xTimerTaskTCB;

    /* Pass out the array that will be used as the Timer task’s stack. */
    *ppxTimerTaskStackBuffer = uxTimerTaskStack;

    /* Pass out the size of the array pointed to by *ppxTimerTaskStackBuffer.
    Note that, as the array is necessarily of type StackType_t,
    configTIMER_TASK_STACK_DEPTH is specified in words, not bytes. */
    *pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
}

=======
>>>>>>> 0afe16f869cdcfbfaa2f256f02ee721cccb7ee98
/* USER CODE END */
