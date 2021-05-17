/*
 * stateMachineTaskTest.c
 *
 *  Created on: May 15, 2021
 *      Author: junaidkhan
 */

#include "stateMachineTaskTest.h"
#include "phantom_freertos.h"
#include "vcu_data.h"
#include "board_hardware.h"
#include "os_queue.h"

extern QueueHandle_t stateMachineMessages;
extern volatile uint8_t initializationOccured;

// Send messages from task_statemachine.c and receive them in this task -> if needed.
void stateMachineTaskTest(void* parameters){
    // Create a variable to be able to set a breakpoint to read rxBuffer Message each time in Memory Browser
       uint8_t forBreakPoint=0;

       // A buffer that will hold messages received from eepromTask - Don't need to print it.
       char rxBuffer[60];

       TickType_t mylastTickCount;
       mylastTickCount = xTaskGetTickCount();

       while(1){

           if(initializationOccured){

               unsigned long numberOfMessages = uxQueueMessagesWaiting(stateMachineMessages);
               if(numberOfMessages){

                   if (xQueueReceive(stateMachineMessages,rxBuffer,pdMS_TO_TICKS(0))==pdPASS){ // Don't block if no message in the Queue
                       forBreakPoint =1;
                   }
               }
               // Block for 500ms
               vTaskDelayUntil(&mylastTickCount,pdMS_TO_TICKS(100));

           }else{

               // Block for 500ms
               vTaskDelayUntil(&mylastTickCount,pdMS_TO_TICKS(100));
           }
       }
}



