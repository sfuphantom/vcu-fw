#include "phantom_queue.h"
#include "vcu_common.h"

#include "sys_common.h"
#include "sys_vim.h"
#include "gio.h"
#include "board_hardware.h"

#include "task_interrupt_actor.h"


void InterruptAgentInit()
{
    vimInit();
}

void gioNotification(gioPORT_t* port, uint32 bit)
{
    isr_action_t interruptAction = {ISR_ID_ERROR, NULL};

    if (port == READY_TO_DRIVE_PORT && bit == READY_TO_DRIVE_PIN) {
        interruptAction.id = RTDS_INTERRUPT;
        // interruptAction.isr_func = 
    }
    // else if ... TODO

    if (interruptAction.id != ISR_ID_ERROR) {
        QueueHandle_t actionQ = ThrottleActor_getActionHandleQueue();
    }
}

