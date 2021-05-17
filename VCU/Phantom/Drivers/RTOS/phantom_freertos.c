/*
 * phantom_freertos.c
 *
 *  Created on: Nov 14, 2020
 *      Author: gabriel
 */


#include "phantom_freertos.h"
#include "gio.h"

unsigned int BSE_sensor_sum  = 0;    // needs to be stored in VCU data structure and referenced from there
bool INTERRUPT_AVAILABLE;
bool THROTTLE_AVAILABLE; // used to only enable throttle after the buzzer has gone for 2 seconds

// ++ Added by jjkhan: State machine Timer callback flags
volatile bool HV_CURRENT_TIMER_EXPIRED;
volatile bool HV_VOLTAGE_TIMER_EXPIRED;
// -- Added by jjkhan: State machine Timer callback flags


/* array to hold handles to the created timers*/
TimerHandle_t xTimers[NUMBER_OF_TIMERS];
xQueueHandle VCUDataQueue;

// ++ Added by jjkhan
TaskHandle_t testingEepromHandler = NULL; // Eeprom Test Task Task Handler
TaskHandle_t stateMachineHandler = NULL;  // State machine Task Handler
TaskHandle_t testingStateMachineTask = NULL;  // State Machine Test Task Handler.
TaskHandle_t eepromHandler = NULL;  // Eeprom Task Task Handler
SemaphoreHandle_t vcuKey;        // Mutex to protect VCU data structure
SemaphoreHandle_t powerfailureFlagKey;  // still using this? - jjkhan
xQueueHandle eepromMessages, stateMachineMessages;
// -- Added by jjkhan

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

    xTimers[1] = xTimerCreate
            ( /* Just a text name, not used by the RTOS
             kernel. */
             "RTDS_Timer",
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

    // ++ Added by jjkhan - State Machint Task Timers for HV Current and Voltage Flags

        xTimers[2] = xTimerCreate
                ( /* Just a text name, not used by the RTOS
                 kernel. */
                 "HV_Current_OutOfRange_T",
                 /* The timer period in ticks, must be
                 greater than 0. */
                 pdMS_TO_TICKS(10000),
                 /* The timers will auto-reload themselves
                 when they expire. */
                 pdFALSE,
                 /* The ID is used to store a count of the
                 number of times the timer has expired, which
                 is initialised to 0. */
                 ( void * ) 0,
                 /* Callback function for when the timer expires*/
                 Timer_HV_CurrentRange
               );

        xTimers[3] = xTimerCreate
                ( /* Just a text name, not used by the RTOS
                 kernel. */
                 "HV_Voltage_OutOfRange_T",
                 /* The timer period in ticks, must be
                 greater than 0. */
                 pdMS_TO_TICKS(10000),
                 /* The timers will auto-reload themselves
                 when they expire. */
                 pdFALSE,
                 /* The ID is used to store a count of the
                 number of times the timer has expired, which
                 is initialised to 0. */
                 ( void * ) 0,
                 /* Callback function for when the timer expires*/
                 Timer_HV_VoltageRange
               );

    // -- Added by jjkhan - State Machint Task Timers for HV Current and Voltage Flags

    // with more timers being added it's more worth it to do a for loop for initializing each one here at the start

    if( xTimers[0] == NULL )
    {
         /* The timer was not created. */
        UARTSend(PC_UART, "The timer was not created.\r\n");
    }
    else
    {
         /* Start the timer.  No block time is specified, and
         even if one was it would be ignored because the RTOS
         scheduler has not yet been started. */
         if( xTimerStart( xTimers[0], 0 ) != pdPASS )
         {
             /* The timer could not be set into the Active
             state. */
             UARTSend(PC_UART, "The timer could not be set into the active state.\r\n");
         }
    }

    if( xTimers[1] == NULL )
    {
         /* The timer was not created. */
        UARTSend(PC_UART, "The timer was not created.\r\n");
    }
    else
    {
         /* Start the timer.  No block time is specified, and
         even if one was it would be ignored because the RTOS
         scheduler has not yet been started. */
         if( xTimerStart( xTimers[1], 0 ) != pdPASS )
         {
             /* The timer could not be set into the Active
             state. */
             UARTSend(PC_UART, "The timer could not be set into the active state.\r\n");
         }
    }


    // ++ Added by jjkhan: State machine Timer Declarations
        if( xTimers[2] == NULL )
        {
             /* The timer was not created. */
            UARTSend(PC_UART, "HV Current Fault timer was not created.\r\n");
        }


        if( xTimers[3] == NULL )
        {
             /* The timer was not created. */
            UARTSend(PC_UART, "HV Voltage Fault timer was not created.\r\n");
        }
     // -- Added by jjkhan: State machine Timer Declarations

}

void phantom_freeRTOStaskInit(void)
{
    // create a freeRTOS queue to pass data between tasks
     // this will be useful when passing the VCU data structure in between different tasks

     VCUDataQueue = xQueueCreate(5, sizeof(long)); // what does this 5 mean?
     eepromMessages = xQueueCreate(10, sizeof(char)*60U);
     stateMachineMessages = xQueueCreate(10, sizeof(char)*60U);

     // can I already shove the VCU data structure into here? or do i need to do that within a task

     // need to do an "if queue != NULL"

     // ++ Added by jjkhan

     vcuKey = xSemaphoreCreateMutex(); // vcuKey to protect VCUData

     // -- Added by jjkhan


     // freeRTOS API to create a task, takes in a task name, stack size, something, priority, something else
     if (xTaskCreate(vStateMachineTask, (const char*)"StateMachineTask",  240, NULL,  (STATE_MACHINE_TASK_PRIORITY), NULL) != pdTRUE)
     {
         // if xTaskCreate returns something != pdTRUE, then the task failed, wait in this infinite loop..
         // probably need a better error handler
         sciSend(PC_UART,23,(unsigned char*)"StateMachineTask Creation Failed.\r\n");
         while(1);
     }


     if (xTaskCreate(vThrottleTask, (const char*)"ThrottleTask",  150, NULL,  (THROTTLE_TASK_PRIORITY), NULL) != pdTRUE)
     {
         // if xTaskCreate returns something != pdTRUE, then the task failed, wait in this infinite loop..
         // probably need a better error handler
         sciSend(PC_UART,23,(unsigned char*)"ThrottleTask Creation Failed.\r\n");
         while(1);
     }


     if (xTaskCreate(vSensorReadTask, (const char*)"SensorReadTask",  150, NULL,  (SENSOR_READ_TASK_PRIORITY), NULL) != pdTRUE)
     {
         // if xTaskCreate returns something != pdTRUE, then the task failed, wait in this infinite loop..
         // probably need a better error handler
         sciSend(PC_UART,23,(unsigned char*)"SensorReadTask Creation Failed.\r\n");
         while(1);
     }


     if (xTaskCreate(vDataLoggingTask, (const char*)"DataLoggingTask",  150, NULL,  (DATA_LOGGING_TASK_PRIORITY), NULL) != pdTRUE)
     {
         // if xTaskCreate returns something != pdTRUE, then the task failed, wait in this infinite loop..
         // probably need a better error handler
         sciSend(PC_UART,23,(unsigned char*)"DataLoggingTask Creation Failed.\r\n");
         while(1);
     }

     if (xTaskCreate(vWatchdogTask, (const char*)"WatchdogTask",  150, NULL,  WATCHDOG_TASK_PRIORITY, NULL) != pdTRUE)
     {
         // if xTaskCreate returns something != pdTRUE, then the task failed, wait in this infinite loop..
         // probably need a better error handler
         sciSend(PC_UART,23,(unsigned char*)"WatchdogTask Creation Failed.\r\n");
         while(1);
     }

     /* ++ Added by jjkhan */

        if (xTaskCreate(vEeprom, (const char*)"EepromTask",  250, NULL,  EEPROM_TASK_PRIORITY, &eepromHandler) != pdTRUE)
        {
                uint8 message[]="EEPROM task Creation Failed.\r\n";
                sciSend(PC_UART,(uint32)sizeof(message),&message[0]);
                while(1);
        }

// TEST_EEPROM is defined in phantom_freertos.h, change it to 0 to skip the following task creation
#if TEST_EEPROM == 1
        if (xTaskCreate(testEeprom, (const char*)"testEeprom",  150, NULL,  tskIDLE_PRIORITY+1, &testingEepromHandler) != pdTRUE)
        {
                uint8 message[]="Test task Creation Failed.\r\n";
                sciSend(PC_UART,(uint32)sizeof(message),&message[0]);
                while(1);
        }
#endif
      /* -- Added by jjkhan */

#if TEST_STATEMACHINE == 1
      // ++ Added by jjkhan - State machine Test Task
            if(xTaskCreate(stateMachineTaskTest, (const char*)"State Machine Task Test", 240, NULL, tskIDLE_PRIORITY, &testingStateMachineTask)!= pdTRUE){
                sciSend(PC_UART,23,(unsigned char*)"TestTask Creation Failed.\r\n");
                while(1);
            }
      // -- Added by jjkhan - State machine Test Task
#endif

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
//     pwmStop(BUZZER_PORT, READY_TO_DRIVE_BUZZER);
     THROTTLE_AVAILABLE = true;
 }


 // ++ Added by jjkhan - Timer Callbacks for HV Current and Voltage Fault Timers

  void Timer_HV_CurrentRange(TimerHandle_t xTimers){
      HV_CURRENT_TIMER_EXPIRED = true; //  i.e. Timer value = threshold
  }

  void Timer_HV_VoltageRange(TimerHandle_t xTimers){
      HV_VOLTAGE_TIMER_EXPIRED = true; //  i.e. Timer value = threshold
  }

  // -- Added by jjkhan - Timer Callbacks for HV Current and Voltage Fault Timers


 /*********************************************************************************
  *               READY TO DRIVE SIGNAL INTERRUPT (SHOULD PROBABLY FIND A BETTER PLACE TO PUT THIS)
  *********************************************************************************/
 void gioNotification(gioPORT_t *port, uint32 bit)
 {
 /*  enter user code between the USER CODE BEGIN and USER CODE END. */
 /* USER CODE BEGIN (19) */
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
 //                pwmStart(BUZZER_PORT, READY_TO_DRIVE_BUZZER);

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
