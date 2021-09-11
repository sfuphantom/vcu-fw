// /*
//  * testEeprom.c
//  *
//  *  Created on: Mar 7, 2021
//  *      Author: junaidkhan
//  */
// #include "phantom_freertos.h"
// #include "testEeprom.h"
// #include "os_queue.h"
// #include "vcu_data.h"

// extern QueueHandle_t eepromMessages;
// extern volatile uint8_t initializationOccured;
// void testEeprom(void *p){

//     // Create a variable to be able to set a breakpoint to read rxBuffer Message each time in Memory Browser
//     uint8_t forBreakPoint =0;

//     // A buffer that will hold messages received from eepromTask - Don't need to print it.
//     char rxBuffer[60];

//     TickType_t mylastTickCount;
//     mylastTickCount = xTaskGetTickCount();

//     while(1){

//         if(initializationOccured){

//             unsigned long numberOfMessages = uxQueueMessagesWaiting(eepromMessages);
//             if(numberOfMessages){

//                 if (xQueueReceive(eepromMessages,rxBuffer,pdMS_TO_TICKS(0))==pdPASS){ // Don't block if no message in the Queue
//                     forBreakPoint =1;
//                 }
//             }
//             // Block for 500ms
//             vTaskDelayUntil(&mylastTickCount,pdMS_TO_TICKS(100));

//         }else{

//             // Block for 500ms
//             vTaskDelayUntil(&mylastTickCount,pdMS_TO_TICKS(100));
//         }
//     }
// }

// /* This will test if the eeprom bank is being updated.
// void testEeprom(void *p){
//     TickType_t mylastTickCount;
//     mylastTickCount = xTaskGetTickCount();
//     while(1){
//         if(xSemaphoreTake(vcuKey, pdMS_TO_TICKS(10))){
//             VCUDataPtr->DigitalVal.TSAL_FAULT ^= (1<<0);       // Toggle Bit-0
//             xSemaphoreGive(vcuKey);
//         }
//         vTaskDelayUntil(&mylastTickCount,pdMS_TO_TICKS(500));
//     }
// }
// */
