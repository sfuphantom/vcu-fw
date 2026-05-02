#ifndef CAN_HANDLER_H
#define CAN_HANDLER_H

#include <stdint.h>
#include "can.h"
#include "FreeRTOS.h"


//configuration
#define CAN_TASK_STACK_SIZE    512
#define CAN_TASK_PRIORITY      2

#define CAN_RX_MESSAGE_BOX     1

//public api
TaskHandle_t CAN_Handler_Init(void);
void Task_CAN_Handler(void *pvParameters);

/* HALCoGen helper wrappers */
uint32 CAN_IsMessageAvailable(canBASE_t *node, uint32 messageBox);
uint32 CAN_ReadMessage(canBASE_t *node, uint32 messageBox, uint8 *data);

#endif
