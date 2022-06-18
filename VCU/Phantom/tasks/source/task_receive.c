/*
 * task_receive.c
 *
 *  Created on: Jun 16, 2022
 *      Author: Rafael Guveara
 */


#include "phantom_task.h"
#include "Phantom_sci.h"
#include "FreeRTOS.h"
#include "os_task.h"
#include "board_hardware.h"
#include "stdlib.h"
#include "vcu_data.h"
enum{

    ECHO_THROTTLE=1,
    ECHO_APPS1,
    ECHO_APPS2,
    ECHO_BSE,
    STAT_RUN,  // vTaskGetRunTimeStats
    STAT_START,  // xTaskGetTickCount
    TASK_LIST, // vTaskList
};



// (int)((ceil(log10(2^32))+1)*sizeof(char)) -stack overflow
static char float_str[11];
static char ptrTaskList[500];
extern volatile unsigned long ulHighFrequencyTimerTicks;

static Task task;
static TaskHandle_t taskHandle;

// Pre-define your static functions here...
static void vReceiveTask(void* arg);

// BTW this function should be the only thing in your header file (aside from include guards and other comments ofc)
void ReceiveTaskInit(void)
{
    task = (Task) {vReceiveTask, 0};

    // Phantom_createTask should block infinitely if task creation failed
    taskHandle = Phantom_createTask(&task, "VCU_CLI", 512, 0);

    // any other init code you want to put goes here...
}

void UARTprintf(char msg[]){
    UARTSend(PC_UART, "\n");
    UARTSend(PC_UART, msg);
    UARTSend(PC_UART, "\r");
}

static void vReceiveTask(void* arg)
{
    char cli_msg = '0';
    sciReceive(PC_UART, 1, &cli_msg);
    uint8_t cmd = atoi(&cli_msg);
    char num_tasks;

    switch(cmd){

        case TASK_LIST:

            vTaskGetRunTimeStats(ptrTaskList);
            UARTprintf(ptrTaskList);
             vTaskList(ptrTaskList);
             UARTprintf(ptrTaskList);

            break;

        case ECHO_THROTTLE:
            ltoa((uint32_t)VCUData_getThrottlePercentage(), float_str);
            UARTSend(PC_UART, float_str);
            UARTSend(PC_UART, "\r");
            break;
        case ECHO_APPS1:
            ltoa((uint32_t)VCUData_getAPPS1Percentage(), float_str);
            UARTSend(PC_UART, float_str);
            UARTSend(PC_UART,"%\r");
            break;

        case ECHO_APPS2:

            ltoa((uint32_t)VCUData_getAPPS2Percentage(), float_str);
            UARTSend(PC_UART,float_str);
            UARTSend(PC_UART ,"%,\r");
            break;
        case ECHO_BSE:

            ltoa((uint32_t)VCUData_getBSEPercentage(), float_str);
            UARTSend(PC_UART,float_str);
            UARTSend(PC_UART, "%\r");
            break;

        case STAT_START:
            ltoa((uint32_t)xTaskGetTickCount(), float_str);
            UARTSend(PC_UART, float_str);
            UARTSend(PC_UART, "\r");
        default:
//            ltoa(cmd, &cli_msg);
//            UARTprintf(&cli_msg);
            break;
    }


}

// Other helper functions and callbacks goes here...
// static void yourTimerCallbackFunc(TimerHandle_t timer) {}
