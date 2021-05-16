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


#include "MCP48FV_DAC_SPI.h" // DAC library written by Ataur Rehman
#include "LV_monitor.h"      // INA226 Current Sense Amplifier Library written by David Cao
#include "IMD.h"             // Bender IR155 IMD Library written by Sumreen Rattan
#include "RTD_Buzzer.h"      // Ready to Drive buzzer wrapper written by Gabriel Soares
#include "RGB_LED.h"         // RGB LED wrapper
#include "Phantom_sci.h"     // UART wrapper written by Mahmoud Kamaleldin

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

/*********************************************************************************
 *                          TASK HEADER DECLARATIONS
 *********************************************************************************/
//static void vStateMachineTask(void *);  // This task will evaluate the state machine and decide whether or not to change states
//static void vSensorReadTask(void *);    // This task will read all the sensors in the vehicle (except for the APPS which requires more critical response)
//static void vThrottleTask(void *);      // This task reads the APPS, performs signal plausibility, and controls the inverter through a DAC
//static void vDataLoggingTask(void *);   // This task will send any important data over CAN to the dashboard for logging onto the SD card
//static void vWatchdogTask(void *);      // This task will monitor all the threads and make sure they are all running, if not (code hangs/freezes or task doesn't get run)
                                        // it will fail to pet the watchdog and the watchdog timer will reset the MCU

// task handle creation??? shouldn't they need to be passed into the xTaskCreate function?

/*********************************************************************************
 *                          SOFTWARE TIMER INITIALIZATION
 *********************************************************************************/
#define NUMBER_OF_TIMERS   4

/* array to hold handles to the created timers*/
TimerHandle_t xTimers[NUMBER_OF_TIMERS];

/* This timer is used to debounce the interrupts for the RTDS and SDC signals */
bool INTERRUPT_AVAILABLE = true;
bool THROTTLE_AVAILABLE = false; // used to only enable throttle after the buzzer has gone for 2 seconds

void Timer_300ms(TimerHandle_t xTimers);
void Timer_2s(TimerHandle_t xTimers);



// ++ Added by jjkhan

volatile bool HV_CURRENT_TIMER_EXPIRED = false;
volatile bool HV_VOLTAGE_TIMER_EXPIRED = false;


void Timer_HV_CurrentRange(TimerHandle_t xTimers);
void Timer_HV_VoltageRange(TimerHandle_t xTimers);
// -- Added by jjkhan

/*********************************************************************************
 *                          STATE ENUMERATION
 *********************************************************************************/
//typedef enum {TRACTIVE_OFF, TRACTIVE_ON, RUNNING, FAULT} State;
State state = TRACTIVE_OFF;

/*********************************************************************************
 *                          QUEUE HANDLE CREATION
 *********************************************************************************/
xQueueHandle VCUDataQueue;
/*********************************************************************************
 *                          GLOBAL VARIABLE DECLARATIONS
 *********************************************************************************/

/*********************************************************************************
 *                          INITIALIZE DATA STRUCTURE...
 *                          or can this be done and outputted in the init function.. hm
 *********************************************************************************/

data VCUData;
data* VCUDataPtr = &VCUData;

// ++ New Code  - Added by jjkhan
SemaphoreHandle_t vcuKey;        // Mutex to protect VCU data structure
SemaphoreHandle_t powerfailureFlagKey;

// -- New Code - Added by jjkhan

//++ For Testing State machine Task - Added by jjkhan

TaskHandle_t testingStateMachineTask = NULL;  // Task Handler for state machine Task.


void stateMachineTaskTest(void* parameters){

    TickType_t LastTickCount;
    LastTickCount = xTaskGetTickCount();
    bool tractive_off_tested = false;
    bool faultCorrected = false;


    while(1){

        if(state == TRACTIVE_OFF){

            //UARTSend(PC_UART, "TRACTIVE_OFF.\r\n");


            if(xSemaphoreTake(vcuKey, pdMS_TO_TICKS(20)) && !tractive_off_tested){
                /*
                    VCUDataPtr->DigitalVal.RTDS = 1; // Set RTD
                    vTaskDelayUntil(&LastTickCount, pdMS_TO_TICKS(1000));  // Create  delay to check the response of state machine task on serial monitor

                    VCUDataPtr->DigitalVal.RTDS = 0; // Clear RTD
                    VCUDataPtr->DigitalVal.BSE_SEVERE_RANGE_FAULT = 1;  // Introduce any fault you like
                    vTaskDelayUntil(&LastTickCount, pdMS_TO_TICKS(1000));

                    VCUDataPtr->DigitalVal.RTDS = 0; // Clear RTD
                    VCUDataPtr->DigitalVal.BSE_SEVERE_RANGE_FAULT = 0;  // clear the fault.
                    vTaskDelayUntil(&LastTickCount, pdMS_TO_TICKS(1000));
                */

                VCUDataPtr->DigitalVal.TSAL_ON = 1; // Set TSAL on, the state should change to TRACTIVE_ON and stay there.

                xSemaphoreGive(vcuKey);
                tractive_off_tested = true;
            }


        }else if(state == TRACTIVE_ON){
            //UARTSend(PC_UART, "TRACTIVE_ON.\r\n");

            if(xSemaphoreTake(vcuKey, pdMS_TO_TICKS(20))&& tractive_off_tested){

                /*
                    VCUDataPtr->DigitalVal.TSAL_ON=0; // Turn off TSAL and see how the system responds.
                    vTaskDelayUntil(&LastTickCount, pdMS_TO_TICKS(1000));  // Create  delay to check the response of state machine task on serial monitor - correct response.
                */

                // Introduce a MINOR Error in the system - state should change MINOR_ERROR

                //VCUDataPtr->DigitalVal.HV_VOLTAGE_OUT_OF_RANGE_FAULT=1;
                //vTaskDelayUntil(&LastTickCount, pdMS_TO_TICKS(1000));  // Create  delay to check the response of state machine task on serial monitor -

                 // Correct the MINOR Error
                 //VCUDataPtr->DigitalVal.BSE_APPS_MINOR_SIMULTANEOUS_FAULT=0; // State should change back to RUNNING
                 //vTaskDelayUntil(&LastTickCount, pdMS_TO_TICKS(1000));  // Create  delay to check the response of state machine task on serial monitor -


                //VCUDataPtr->DigitalVal.BSE_SEVERE_RANGE_FAULT=1;
                //vTaskDelayUntil(&LastTickCount, pdMS_TO_TICKS(1000));  // Create  delay to check the response of state machine task on serial monitor -
                //VCUDataPtr->DigitalVal.RTDS = 1;

                VCUDataPtr->DigitalVal.RTDS=1;

                xSemaphoreGive(vcuKey);
            }

        }else if(state == RUNNING){
            //UARTSend(PC_UART, "Current state is RUNNING. \r\n");
            if(xSemaphoreTake(vcuKey, pdMS_TO_TICKS(20)) && !faultCorrected){
                //UARTSend(PC_UART, "Key Accessed.\r\n");
                VCUDataPtr->DigitalVal.HV_VOLTAGE_OUT_OF_RANGE_FAULT = 1;
                xSemaphoreGive(vcuKey);
            }


        }else if(state == MINOR_FAULT){
            //UARTSend(PC_UART, "Current state is MINOR_FAULT. \r\n");
            if(xSemaphoreTake(vcuKey, pdMS_TO_TICKS(20))&& !faultCorrected){
                           //UARTSend(PC_UART, "Key Accessed.\r\n");
                           VCUDataPtr->DigitalVal.HV_VOLTAGE_OUT_OF_RANGE_FAULT = 0;
                           xSemaphoreGive(vcuKey);
                           faultCorrected = true;
             }

        }else if(state == SEVERE_FAULT){

            //UARTSend(PC_UART, "Current state is SEVERE_FAULT. \r\n");
        }else{
            //UARTSend(PC_UART, "Current state unknown. \r\n");
        }

        vTaskDelayUntil(&LastTickCount, pdMS_TO_TICKS(500));


    }
}

//-- For Testing State machine Task - Added by jjkhan



uint8 i;
char command[8]; // used for ADC printing.. this is an array of 8 chars, each char is 8 bits
long xStatus;

/*********************************************************************************
 *                               SYSTEM STATE FLAGS
 *********************************************************************************/
//uint8_t TSAL = 0;
//uint8_t RTDS = 0;
long RTDS_RAW = 0;
//uint8_t BMS  = 1;
//uint8_t IMD = 1;
//uint8_t BSPD = 1;
//uint8_t BSE_FAULT = 0;

// ^^^^^^^^^^^^^^^ these should all be inside the data structure now

/*********************************************************************************
                 ADC FOOT PEDAL AND APPS STUFF (SHOULD GENERALIZE THIS)
 *********************************************************************************/
adcData_t FP_data[3];
adcData_t *FP_data_ptr = &FP_data[0];
unsigned int FP_sensor_1_sum = 0;
unsigned int FP_sensor_1_avg;
unsigned int FP_sensor_2_sum = 0;
unsigned int FP_sensor_2_avg;

unsigned int BSE_sensor_sum  = 0;
unsigned int BSE_sensor_avg  = 0;
unsigned int NumberOfChars;

uint16 FP_sensor_1_min = 0;
uint16 FP_sensor_2_min = 0;

uint16 FP_sensor_1_max = 4095; // 12-bit ADC
uint16 FP_sensor_2_max = 4095; // 12-bit ADC
uint16 FP_sensor_1_percentage;
uint16 FP_sensor_2_percentage;
uint16 FP_sensor_diff;



// change to better data type
//int lv_current = 0;


/* USER CODE END */

void main(void)
{
/* USER CODE BEGIN (3) */
/*********************************************************************************
 *                          HALCOGEN PERIPHERAL INITIALIZATION
 *********************************************************************************/
    _enable_IRQ();
    sciInit();
    gioInit();
    adcInit();
    hetInit();
    pwmStop(BUZZER_PORT, READY_TO_DRIVE_BUZZER); // stop the ready to drive buzzer PWM from starting automatically

    // turn off RGB LEDs
    pwmStart(RGB_LED_PORT, BLUE_LED); // blue
    pwmStart(RGB_LED_PORT, GREEN_LED); // green
    pwmStart(RGB_LED_PORT, RED_LED); // red
    // maybe this can be changed in halcogen?

    // initialize HET pins ALL to output.. may need to change this later
    gioSetDirection(hetPORT1, 0xFFFFFFFF);

/*********************************************************************************
 *                          VCU DATA STRUCTURE INITIALIZATION
 *********************************************************************************/
    initData(VCUDataPtr); // maybe i return the data structure here?

    // ++ New Code - Added by jjkhan
    vcuKey = xSemaphoreCreateMutex(); // VCU Key Intialization
    // -- New Code - Added by jjkhan
/*********************************************************************************
 *                          PHANTOM LIBRARY INITIALIZATION
 *********************************************************************************/
    //using MCP48FV Library
    MCP48FV_Init();
//    MCP48FV_Set_Value(400);//500 =5.00V, 250= 2.5V

    // LV monitor library
    //lv_monitorInit();  - commented out by jjkhan

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

    // ++ New Code by jjkhan - Timers for HV Current and Voltage Flags

    xTimers[2] = xTimerCreate
            ( /* Just a text name, not used by the RTOS
             kernel. */
             "HV_Current_OutOfRange_T",
             /* The timer period in ticks, must be
             greater than 0. */
             pdMS_TO_TICKS(100),
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
             pdMS_TO_TICKS(100),
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

    // -- New code by jjkhan - Timeres for HV Current and Voltage Flags


    // with more timers being added it's more worth it to do a for loop for initializing each one here at the start

    if( xTimers[0] == NULL )
    {
         /* The timer was not created. */
        UARTSend(PC_UART, "The timer was not created.\r\n");
    }
    /*else
    {

          Start the timer.  No block time is specified, and
         even if one was it would be ignored because the RTOS
         scheduler has not yet been started.
         if( xTimerStart( xTimers[0], 0 ) != pdPASS )
         {
             /* The timer could not be set into the Active
             state.
             UARTSend(PC_UART, "The timer could not be set into the active state.\r\n");
         }
    }
    */
    if( xTimers[1] == NULL )
    {
         /* The timer was not created. */
        UARTSend(PC_UART, "The timer was not created.\r\n");
    }
    /*else
    {
          Start the timer.  No block time is specified, and
         even if one was it would be ignored because the RTOS
         scheduler has not yet been started.
         if( xTimerStart( xTimers[1], 0 ) != pdPASS )
         {
             /* The timer could not be set into the Active
             state.
             UARTSend(PC_UART, "The timer could not be set into the active state.\r\n");
         }
    }*/



    // ++ New Code added by jjkhan - Check if Timer Created
    if( xTimers[2] == NULL )
    {
         /* The timer was not created. */
        UARTSend(PC_UART, "HV Current Fault timer was not created.\r\n");
    }/*else{
           if( xTimerStart( xTimers[2], 0 ) != pdPASS )
        {
            //The timer could not be set into the Active state.
            UARTSend(PC_UART, "The timer could not be set into the active state.\r\n");
        }
    }*/


    if( xTimers[3] == NULL )
    {
         /* The timer was not created. */
        UARTSend(PC_UART, "HV Voltage Fault timer was not created.\r\n");
    }
    /*else{
            if( xTimerStart( xTimers[3], 0 ) != pdPASS )
        {
            //The timer could not be set into the Active state.
            UARTSend(PC_UART, "The timer could not be set into the active state.\r\n");
        }
    }*/

    // -- New Code added by jjkhan - Check if Timer Created
/*********************************************************************************
 *                          freeRTOS TASK & QUEUE CREATION
 *********************************************************************************/

    // create a freeRTOS queue to pass data between tasks
    // this will be useful when passing the VCU data structure in between different tasks

    VCUDataQueue = xQueueCreate(5, sizeof(long)); // what does this 5 mean?

    // can I already shove the VCU data structure into here? or do i need to do that within a task

    // need to do an "if queue != NULL"

    // freeRTOS API to create a task, takes in a task name, stack size, something, priority, something else
    if (xTaskCreate(vStateMachineTask, (const char*)"StateMachineTask",  240, NULL,  (STATE_MACHINE_TASK_PRIORITY+1), NULL) != pdTRUE)
    {
        // if xTaskCreate returns something != pdTRUE, then the task failed, wait in this infinite loop..
        // probably need a better error handler
        sciSend(PC_UART,23,(unsigned char*)"StateMachineTask Creation Failed.\r\n");
        while(1);
    }

/*  ++ Commented out by jjkhan - nothing happening in the task  - memory wasted.
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

    //-- Commented out by jjkhan - nothing happening in the task  - memory wasted.

    */


    // ++ Added by jjkhan - State machine Test Task
    if(xTaskCreate(stateMachineTaskTest, (const char*)"State Machine Task Test", 240, NULL, tskIDLE_PRIORITY, &testingStateMachineTask)!= pdTRUE){
        sciSend(PC_UART,23,(unsigned char*)"TestTask Creation Failed.\r\n");
        while(1);
    }
    // -- Added by jjkhan - State machine Test Task

    // all tasks have been created successfully
    UARTSend(PC_UART, "Tasks created\r\n"); // We want to replace scilinREG with something like "PC_UART". and the BMS one to be "BMS_UART"
    // will need our own hardware defines file to do this for all the ports and pins we use..
    // will need to be different based on the launchpad or VCU being used. This can be changed via build configurations
    // so one build has all the right files/linker included, right debugger, right MCU

    // start freeRTOS task scheduler
    vTaskStartScheduler();

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
    initializeIMD();            // Initialize IMD Library

    /* freeRTOS Initialization */
    phantom_freeRTOSInit();     // Initialize freeRTOS timers, queues, and tasks
    vTaskStartScheduler();      // start freeRTOS task scheduler
   
    // infinite loop to prevent code from ending. The scheduler will now pre-emptively switch between tasks.
    while(1);
/* USER CODE END */

}
/* USER CODE BEGIN (4) */
 // ++ Added by jjkhan - Timer Callbacks for HV Current and Voltage Fault Timers

 void Timer_HV_CurrentRange(TimerHandle_t xTimers){
     HV_CURRENT_TIMER_EXPIRED = true; //  i.e. Timer value = threshold


 }

 void Timer_HV_VoltageRange(TimerHandle_t xTimers){
     HV_VOLTAGE_TIMER_EXPIRED = true; //  i.e. Timer value = threshold
 }

 // -- Added by jjkhan - Timer Callbacks for HV Current and Voltage Fault Timers
/* USER CODE END */
