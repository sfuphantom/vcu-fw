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

// (int)((ceil(log10(2^32))+1)*sizeof(char)) -stack overflow
static char float_str[11];
extern volatile unsigned long ulHighFrequencyTimerTicks;

static Task task;
static TaskHandle_t taskHandle;

static void vReceiveTask(void* arg);

void ReceiveTaskInit(void)
{
    task = (Task) {vReceiveTask, 0};

    // blocks indefinitely if task creation failed
    taskHandle = Phantom_createTask(&task, "VCU_CLI", 512, 0);
}

static void vReceiveTask(void* arg)
{


}
