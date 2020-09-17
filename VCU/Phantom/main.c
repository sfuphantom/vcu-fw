/*
 * main.c
 *
 *  Created on: Mar 17, 2020
 *      Author: gabriel
 */

/* USER CODE BEGIN (0) */
/* USER CODE END */

/* Include Files */

#include "adc.h"
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "gio.h"
#include "het.h"
#include "os_queue.h"
#include "os_semphr.h"
#include "os_task.h"
#include "os_timer.h"
#include "reg_het.h"
#include "sys_common.h"
#include "system.h" // is this required?
#include "MCP48FV_DAC_SPI.h" // DAC library written by Ataur Rehman
#include "LV_monitor.h"      // INA226 Current Sense Amplifier Library written by David Cao
#include "FreeRTOS.h"
#include "Phantom_sci.h"
#include "stdlib.h" // stdlib.h has ltoa() which we use for our simple SCI printing routine.
#include <stdio.h>
#include "reg_het.h"

#include "task_data_logging.h"
#include "task_sensor_read.h"
#include "task_statemachine.h"
#include "task_throttle.h"
#include "task_watchdog.h"
#include "task_eeprom.h"

#include "priorities.h" // holds the task priorities

#include "vcu_data.h" // holds VCU data structure

#include "board_hardware.h"   // contains hardware defines for specific board used (i.e. VCU or launchpad)


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
#define NUMBER_OF_TIMERS   2

/* array to hold handles to the created timers*/
TimerHandle_t xTimers[NUMBER_OF_TIMERS];

/* This timer is used to debounce the interrupts for the RTDS and SDC signals */
bool INTERRUPT_AVAILABLE = true;
bool THROTTLE_AVAILABLE = false; // used to only enable throttle after the buzzer has gone for 2 seconds

void Timer_300ms(TimerHandle_t xTimers);
void Timer_2s(TimerHandle_t xTimers);

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
    //initData(VCUDataPtr); // maybe i return the data structure here?

/*********************************************************************************
 *                          PHANTOM LIBRARY INITIALIZATION
 *********************************************************************************/
    //using MCP48FV Library
    MCP48FV_Init();
//    MCP48FV_Set_Value(400);//500 =5.00V, 250= 2.5V

    // LV monitor library
    //lv_monitorInit();

    // Eeprom Initialization
    eepromBlocking_Init();
    vcuKey = xSemaphoreCreateMutex();
/*********************************************************************************
 *                          freeRTOS SOFTWARE TIMER SETUP
 *********************************************************************************/

    xTimers[0] = xTimerCreate
            ( /* Just a text name, not used by the RTOS
             kernel. */
             "RTDS_Timer",
             /* The timer period in ticks, must be
             greater than 0. */
             pdMS_TO_TICKS(300),
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
/*********************************************************************************
 *                          freeRTOS TASK & QUEUE CREATION
 *********************************************************************************/

    // create a freeRTOS queue to pass data between tasks
    // this will be useful when passing the VCU data structure in between different tasks

    //VCUDataQueue = xQueueCreate(5, sizeof(VCUData)); // what does this 5 mean? - Answer: 5 block queue, with each block having the size of an VCU Data Structure.

    // can I already shove the VCU data structure into here? or do i need to do that within a task

    // need to do an "if queue != NULL"

    // freeRTOS API to create a task, takes in a task name, stack size, something, priority, something else
    if (xTaskCreate(vStateMachineTask, (const char*)"StateMachineTask",  150, NULL,  STATE_MACHINE_TASK_PRIORITY, stateMachineHandler) != pdTRUE)
    {
        // if xTaskCreate returns something != pdTRUE, then the task failed, wait in this infinite loop..
        // probably need a better error handler
        sciSend(PC_UART,23,(unsigned char*)"StateMachineTask Creation Failed.\r\n");
        while(1);
    }

/*
    if (xTaskCreate(vThrottleTask, (const char*)"ThrottleTask",  150, NULL,  (THROTTLE_TASK_PRIORITY), NULL) != pdTRUE)
    {
        // if xTaskCreate returns something != pdTRUE, then the task failed, wait in this infinite loop..
        // probably need a better error handler
        sciSend(PC_UART,23,(unsigned char*)"ThrottleTask Creation Failed.\r\n");
        while(1);
    }
*/
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


    if (xTaskCreate(vWatchdogTask, (const char*)"WatchdogTask",  150, NULL, WATCHDOG_TASK_PRIORITY, NULL) != pdTRUE)
    {
        // if xTaskCreate returns something != pdTRUE, then the task failed, wait in this infinite loop..
        // probably need a better error handler
        sciSend(PC_UART,23,(unsigned char*)"WatchdogTask Creation Failed.\r\n");
        while(1);
    }


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

    // all tasks have been created successfully
    UARTSend(PC_UART, "Tasks created\r\n"); // We want to replace scilinREG with something like "PC_UART". and the BMS one to be "BMS_UART"
    // will need our own hardware defines file to do this for all the ports and pins we use..
    // will need to be different based on the launchpad or VCU being used. This can be changed via build configurations
    // so one build has all the right files/linker included, right debugger, right MCU

    // start freeRTOS task scheduler
    vTaskStartScheduler();

    // infinite loop to prevent code from ending. The scheduler will now pre-emptively switch between tasks.
    while(1);
/* USER CODE END */

}
/* USER CODE BEGIN (4) */

/*********************************************************************************
 *                          freeRTOS TASK IMPLEMENTATIONS
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
                pwmStart(BUZZER_PORT, READY_TO_DRIVE_BUZZER);

                // reset the 2 second timer to let the buzzer ring for 2 seconds
                if (xTimerResetFromISR(xTimers[1], &xHigherPriorityTaskWoken) != pdPASS)// after 2s the timer will allow the interrupt to toggle the signal again
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
        if (xTimerResetFromISR(xTimers[0], &xHigherPriorityTaskWoken) != pdPASS)// after 300ms the timer will allow the interrupt to toggle the signal again
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
function then they must be declared static � otherwise they will be allocated on
the stack and so not exists after this function exits. */
static StaticTask_t xIdleTaskTCB;
static StackType_t uxIdleTaskStack[ configMINIMAL_STACK_SIZE ];

    /* Pass out a pointer to the StaticTask_t structure in which the Idle task�s
    state will be stored. */
    *ppxIdleTaskTCBBuffer = &xIdleTaskTCB;

    /* Pass out the array that will be used as the Idle task�s stack. */
    *ppxIdleTaskStackBuffer = uxIdleTaskStack;

    /* Pass out the size of the array pointed to by *ppxIdleTaskStackBuffer.
    Note that, as the array is necessarily of type StackType_t,
    configMINIMAL_STACK_SIZE is specified in words, not bytes. */
    *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}
/*��������������������*/

/* configSUPPORT_STATIC_ALLOCATION and configUSE_TIMERS are both set to 1, so the
application must provide an implementation of vApplicationGetTimerTaskMemory()
to provide the memory that is used by the Timer service task. */
void vApplicationGetTimerTaskMemory( StaticTask_t **ppxTimerTaskTCBBuffer,
                                     StackType_t **ppxTimerTaskStackBuffer,
                                     uint32_t *pulTimerTaskStackSize )
{
/* If the buffers to be provided to the Timer task are declared inside this
function then they must be declared static � otherwise they will be allocated on
the stack and so not exists after this function exits. */
static StaticTask_t xTimerTaskTCB;
static StackType_t uxTimerTaskStack[ configTIMER_TASK_STACK_DEPTH ];

    /* Pass out a pointer to the StaticTask_t structure in which the Timer
    task�s state will be stored. */
    *ppxTimerTaskTCBBuffer = &xTimerTaskTCB;

    /* Pass out the array that will be used as the Timer task�s stack. */
    *ppxTimerTaskStackBuffer = uxTimerTaskStack;

    /* Pass out the size of the array pointed to by *ppxTimerTaskStackBuffer.
    Note that, as the array is necessarily of type StackType_t,
    configTIMER_TASK_STACK_DEPTH is specified in words, not bytes. */
    *pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
}

/* USER CODE END */
