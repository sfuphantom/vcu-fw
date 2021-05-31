/*
 * phantom_freertos.c
 *
 *  Created on: Nov 14, 2020
 *      Author: gabriel
 */


#include "phantom_freertos.h"
#include "gio.h"
#include "het.h"

unsigned int BSE_sensor_sum  = 0;    // needs to be stored in VCU data structure and referenced from there

bool INTERRUPT_AVAILABLE = false;
bool THROTTLE_AVAILABLE = false; // used to only enable throttle after the buzzer has gone for 2 seconds

/* array to hold handles to the created timers*/
TimerHandle_t xTimers[NUMBER_OF_TIMERS];

xQueueHandle VCUDataQueue;

extern data* VCUDataPtr;

void phantom_freeRTOSInit(void)
{
    phantom_freeRTOStimerInit();
    phantom_freeRTOStaskInit();
}

void phantom_freeRTOStimerInit(void)
{
/*********************************************************************************
 *                          freeRTOS SOFTWARE TIMER SETUP
 *********************************************************************************/

    xTimers[DEBOUNCE_TIMER] = xTimerCreate
            ( /* Just a text name, not used by the RTOS
             kernel. */
             "Debounce_Timer",
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

    xTimers[BUZZER_TIMER] = xTimerCreate
            ( /* Just a text name, not used by the RTOS
             kernel. */
             "Buzzer_Timer",
             /* The timer period in ticks, must be
             greater than 0. */
             pdMS_TO_TICKS(2000),
             /* The timers will auto-reload themselves
             when they expire. */
             pdFALSE,
             /* The ID is used to store a count of the
             number of times the timer has expired, which
             is initialised to 0. */
             ( void * ) 0,
             /* Callback function for when the timer expires*/
             Timer_2s
           );


    // with more timers being added it's more worth it to do a for loop for initializing each one here at the start

    if( xTimers[BUZZER_TIMER] == NULL )
    {
         /* The timer was not created. */
        UARTSend(PC_UART, "The timer was not created.\r\n");
    }
    else
    {
         /* Start the timer.  No block time is specified, and
         even if one was it would be ignored because the RTOS
         scheduler has not yet been started. */
//         if( xTimerStart( xTimers[BUZZER_TIMER], 0 ) != pdPASS )
//         {
//             /* The timer could not be set into the Active
//             state. */
//             UARTSend(PC_UART, "The timer could not be set into the active state.\r\n");
//         }
    }

    if( xTimers[DEBOUNCE_TIMER] == NULL )
    {
         /* The timer was not created. */
        UARTSend(PC_UART, "The timer was not created.\r\n");
    }
    else
    {
         /* Start the timer.  No block time is specified, and
         even if one was it would be ignored because the RTOS
         scheduler has not yet been started. */
         if( xTimerStart( xTimers[DEBOUNCE_TIMER], 0 ) != pdPASS )
         {
             /* The timer could not be set into the Active
             state. */
             UARTSend(PC_UART, "The timer could not be set into the active state.\r\n");
         }
    }
}

void phantom_freeRTOStaskInit(void)
{
    // create a freeRTOS queue to pass data between tasks
     // this will be useful when passing the VCU data structure in between different tasks

     VCUDataQueue = xQueueCreate(5, sizeof(long)); // what does this 5 mean?

     // can I already shove the VCU data structure into here? or do i need to do that within a task

     // need to do an "if queue != NULL"


     // freeRTOS API to create a task, takes in a task name, stack size, something, priority, something else
     if (xTaskCreate(vStateMachineTask, (const char*)"StateMachineTask",  240, NULL,  (STATE_MACHINE_TASK_PRIORITY), NULL) != pdTRUE)
     {
         // if xTaskCreate returns something != pdTRUE, then the task failed, wait in this infinite loop..
         // probably need a better error handler
         sciSend(PC_UART,23,(unsigned char*)"StateMachineTask Creation Failed.\r\n");
         while(1);
     }


     if (xTaskCreate(vThrottleTask, (const char*)"ThrottleTask",  240, NULL,  (THROTTLE_TASK_PRIORITY), NULL) != pdTRUE)
     {
         // if xTaskCreate returns something != pdTRUE, then the task failed, wait in this infinite loop..
         // probably need a better error handler
         sciSend(PC_UART,23,(unsigned char*)"ThrottleTask Creation Failed.\r\n");
         while(1);
     }


     if (xTaskCreate(vSensorReadTask, (const char*)"SensorReadTask",  240, NULL,  (SENSOR_READ_TASK_PRIORITY), NULL) != pdTRUE)
     {
         // if xTaskCreate returns something != pdTRUE, then the task failed, wait in this infinite loop..
         // probably need a better error handler
         sciSend(PC_UART,23,(unsigned char*)"SensorReadTask Creation Failed.\r\n");
         while(1);
     }


     if (xTaskCreate(vDataLoggingTask, (const char*)"DataLoggingTask",  240, NULL,  (DATA_LOGGING_TASK_PRIORITY), NULL) != pdTRUE)
     {
         // if xTaskCreate returns something != pdTRUE, then the task failed, wait in this infinite loop..
         // probably need a better error handler
         sciSend(PC_UART,23,(unsigned char*)"DataLoggingTask Creation Failed.\r\n");
         while(1);
     }

     if (xTaskCreate(vWatchdogTask, (const char*)"WatchdogTask",  240, NULL,  WATCHDOG_TASK_PRIORITY, NULL) != pdTRUE)
     {
         // if xTaskCreate returns something != pdTRUE, then the task failed, wait in this infinite loop..
         // probably need a better error handler
         sciSend(PC_UART,23,(unsigned char*)"WatchdogTask Creation Failed.\r\n");
         while(1);
     }


     // all tasks have been created successfully
     UARTSend(PC_UART, "Tasks created\r\n"); // We want to replace scilinREG with something like "PC_UART". and the BMS one to be "BMS_UART"
     // will need our own hardware defines file to do this for all the ports and pins we use..
     // will need to be different based on the launchpad or VCU being used. This can be changed via build configurations
     // so one build has all the right files/linker included, right debugger, right MCU
}

/* Timer callback when it expires */
 void Timer_300ms(TimerHandle_t xTimers)
 {
     INTERRUPT_AVAILABLE = true;
 }

 /* Timer callback when it expires for the ready to drive sound */
 void Timer_2s(TimerHandle_t xTimers)
 {
     pwmSetDuty(BUZZER_PORT,READY_TO_DRIVE_BUZZER,0);
//     pwmSetDuty(BUZZER_PORT, READY_TO_DRIVE_BUZZER, 0U);
     THROTTLE_AVAILABLE = true;
 }

 /*********************************************************************************
  *               READY TO DRIVE SIGNAL INTERRUPT (SHOULD PROBABLY FIND A BETTER PLACE TO PUT THIS)
  *********************************************************************************/
 void gioNotification(gioPORT_t *port, uint32 bit)
 {
 /*  enter user code between the USER CODE BEGIN and USER CODE END. */
 /* USER CODE BEGIN (19) */


     /*********************************** SHUTDOWN SIGNALS ***************************************/

     if(port == SHUTDOWN_CIRCUIT_PORT && bit == BMS_FAULT_PIN){

        #ifdef SENSOR_PRINT
        UARTSend(PC_UART, "BMS Fault Triggered!\r\n");
        #endif

         VCUDataPtr->DigitalVal.BMS_FAULT = true;

     }

     if(port == SHUTDOWN_CIRCUIT_PORT && bit == IMD_FAULT_PIN){

        #ifdef SENSOR_PRINT
        UARTSend(PC_UART, "IMD Fault Triggered!\r\n");
        #endif

         VCUDataPtr->DigitalVal.IMD_FAULT = true;
     }


 //    UARTSend(PC_UART, "---------Interrupt Request-------\r\n");
     if (port == READY_TO_DRIVE_PORT && bit == READY_TO_DRIVE_PIN /* && INTERRUPT_AVAILABLE */){ //active on rising and falling edge

        BaseType_t* xHigherPriorityTaskWoken = pdFALSE;

        UARTSend(PC_UART, "---------Interrupt Active\r\n");
        if (/* BSE_sensor_sum > BRAKING_THRESHOLD && */  gioGetBit(READY_TO_DRIVE_PORT, READY_TO_DRIVE_PIN)){

            //            gioSetBit(gioPORTA, 6, 1); //enable brake light

            //CHANGE TO STATE RUNNING
            VCUDataPtr->DigitalVal.RTDS = 1;

            // ready to drive buzzer ON
            pwmSetDuty(BUZZER_PORT,READY_TO_DRIVE_BUZZER,50);

            // reset the 2 second timer to let the buzzer ring for 2 seconds and allow throttle to motor (within BUZZER_TIMER)


//            if (xTimerStartFromISR(xTimers[BUZZER_TIMER], xHigherPriorityTaskWoken) != pdPASS){
//
//                // timer reset failed
//                UARTSend(PC_UART, "---------Timer reset failed-------\r\n");
//            }
//            xTimerStartFromISR(xTimers[BUZZER_TIMER], 500);

            if (xTimerResetFromISR(xTimers[BUZZER_TIMER], xHigherPriorityTaskWoken) != pdPASS){

                // timer reset failed
                UARTSend(PC_UART, "---------Buzzer Timer reset failed-------\r\n");
            }

        }else{

            //block throttle to motor
            THROTTLE_AVAILABLE = false;
        }

        //BUTTON DEBOUNCING
        INTERRUPT_AVAILABLE = false;
        if (xTimerResetFromISR(xTimers[DEBOUNCE_TIMER], xHigherPriorityTaskWoken) != pdPASS) // after 300ms the timer will allow the interrupt to toggle the signal again
        {
            // timer reset failed
            UARTSend(PC_UART, "---------Debounce Timer reset failed-------\r\n");
        }

     } //RTDS interrupt handling

 }//gioNotification
