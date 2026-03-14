/*
 *  task_pedal_readings.c
 * 
 *  Created on: July 15, 2022
 *      Author: Joshua Guo
 */

#include "vcu_common.h"
#include "can_handler.h"

/* C standard libs */
#include "stdlib.h"
#include "string.h"
#include "sci.h"
#include "stdio.h"
#include "math.h"
#include <stdint.h>

/* Halcogen drivers */
#include "sys_common.h"
#include "adc.h"
#include "gio.h"

/* Phantom modules */
#include "Phantom_sci.h"
#include "board_hardware.h"

/* Phantom tasks */
#include "task_pedal_readings.h"
#include "task_logger.h"
#include "task_event_handler.h"
#include "task_config.h"

//static data
static uint8 g_canRxData[8];
static float g_voltage;

static PipeTask_t rtos_handles;
typedef uint64_t segment_t;
#define SEGMENT_SIZE sizeof(segment_t)
// Init
TaskHandle_t CAN_Handler_Init(void)
{
    BaseType_t ret;

    /* Initialize CAN peripheral */
   canInit();
//
   // THIS IS THE PROBLEM FIX IT!!!!!!!!!!!!!!!
    ret = xTaskCreate(
        Task_CAN_Handler,
        "CAN_Handler",
        CAN_TASK_STACK_SIZE,
        NULL,
        CAN_TASK_PRIORITY,
        &rtos_handles.taskHandle
    );

    rtos_handles.q = xQueueCreate(16, sizeof(segment_t));

    return ret == pdPASS && rtos_handles.q ? rtos_handles.taskHandle : NULL;
}

//can rx task
void Task_CAN_Handler(void *pvParameters)
{
    (void)pvParameters;

    for (;;)
    {
        if (CAN_IsMessageAvailable(canREG1, canMESSAGE_BOX1))
        {
            Log("MESSAGE RECIEVED");
//            uint32 canId = canGetID(canREG1, CAN_RX_MESSAGE_BOX);
//
//            // only parse analog input
//            if (canId == 0x0A3) {
//                CAN_ReadMessage(
//                canREG1,
//                CAN_RX_MESSAGE_BOX,
//                g_canRxData
//            );

            // parsing logic
//            int16_t raw_voltage = (int16_t)(g_canRxData[0] | (g_canRxData[1] << 8)); // combine bytes
//            g_voltage = raw_voltage / 10.0f; // scaling
//            char buffer[32];
//		    sprintf(buffer, "CAN VOLTAGE 1 READING: %d", g_voltage);
//            Log(buffer);
//
//            int16_t rawLowVoltage = (int16_t)(g_canRxData[2] | (g_canRxData[3] << 8)); // combine bytes
//            g_voltage = rawLowVoltage / 100.0f; // scaling
//            sprintf(buffer, "CAN VOLTAGE 2 READING: %d", g_voltage);
//            Log(buffer);
        }



        /* Run at 100 Hz (10 ms) */
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

//halcogen can wrappers
uint32 CAN_IsMessageAvailable(canBASE_t *node, uint32 messageBox)
{
    return canIsRxMessageArrived(node, messageBox);
}

uint32 CAN_ReadMessage(canBASE_t *node, uint32 messageBox, uint8 *data)
{
    return canGetData(node, messageBox, data);
}
