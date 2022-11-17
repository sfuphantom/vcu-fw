/*
 *  Task Template made by Joshua Guo
 * 
 *  Created on:
 *      Author: Joshua Guo, 
 */
#include "phantom_task.h"
#include "phantom_queue.h"

#include "task_interrupt.h"
#include "task_statemachine.h"

#include "vcu_common.h"
#include "os_portmacro.h"
#include "stdarg.h"

#include "vcu_common.h"

#include "sys_common.h"
#include "sys_vim.h"
#include "gio.h"
#include "board_hardware.h"


/* Static members */
static const int NUM_INTERRUPTS = 4; // Not a macro to keep this scoped to the file 
static PipeTask_t actorTask;
static QueueHandle_t agentISR_Q;

static void vInterruptActorTask(void* arg);
static uint8_t sendFromISR(isr_action_t data);
static void receiveFromISR(isr_action_t* data_ptr);

/* Public API */
uint8_t receiveIntEvent(uint32_t delay)
{
    uint8_t event;

    Phantom_receive(actorTask.q, &event, delay);   

    return event;
}

void InterruptInit(void)
{
    /* Actor initialization */

    actorTask.task = (Task) {vInterruptActorTask, 0};

    actorTask.taskHandle = Phantom_createTask(&actorTask.task, "InterruptActorTask", 150, 5);

    actorTask.q = NULL; 

    /* Agent initialization */

    agentISR_Q = Phantom_createQueue(NUM_INTERRUPTS, sizeof(isr_action_t));

    vimInit();
}

/* Task thread */
static void vInterruptActorTask(void* arg)
{
    isr_action_t buffer;

    receiveFromISR(&buffer);

    buffer.isr_func(-1, actorTask.q); // TODO: and any other params?
}

/* Agent Interrupt Handler */
static uint8_t sendFromISR(isr_action_t data){

    BaseType_t WokenHigherPriorityTask = pdFALSE;

    Phantom_sendISR(agentISR_Q, &data, &WokenHigherPriorityTask);

    return WokenHigherPriorityTask == pdTRUE;
}

void gioNotification(gioPORT_t* port, uint32 bit)
{
    isr_action_t interruptAction = {ISR_ID_ERROR, NULL};

    if (port == READY_TO_DRIVE_PORT && bit == READY_TO_DRIVE_PIN) {
        interruptAction.id = RTDS_INTERRUPT;
        interruptAction.isr_func = NULL; // TODO: Write function handler
    }

    if (interruptAction.id != ISR_ID_ERROR) {


    }

    sendFromISR(interruptAction);
}

static void receiveFromISR(isr_action_t* data_ptr)
{
    Phantom_receive(agentISR_Q, data_ptr, portMAX_DELAY);   
}