/*
 * task_eeprom.c
 *
 *  Created on: Aug 9, 2020
 *      Author: junaidkhan
 */
#include "task_eeprom.h"
#include "priorities.h"
#include "os_portmacro.h"
#include "os_task.h"
#include "gio.h"
#include "os_queue.h"


extern TaskHandle_t eepromHandler; // Eeprom Task handler
extern SemaphoreHandle_t vcuKey;
extern data VCUData;

void *pVCUDataStructure = &VCUData;  // Need this void pointer to read from eeprom bank - Added by jjkhan

//++ Added by jjkhan for execution time measurement

#include <Phantom/support/execution_timer.h>

#define CPU_CLOCK_MHz (float) 160.0  // System clock is 180 MHz, RTI Clock is 80MHz
volatile unsigned long cycles_PMU_start; // CPU cycle count at start
volatile float time_PMU_code_uSecond; // the calculated time in uSecond.

//-- Added by jjkhan for execution time measurement

//++ Messages from EEPROM Task

extern QueueHandle_t eepromMessages;
volatile uint8_t initializationOccured = 0;

//-- Messages from EEPROM Task

void vEeprom(void *p){

       /*   Uncomment the message you want send for debugging purposes. */
       //const char message1[] ="Eeprom Data Block 1 Read successfull.\r\n";
       const char message2[] ="VCU data structure loaded from eeprom successfully.\r\n";
       //const char message3[] ="VCU data structure load from eeprom failed.\r\n";
       const char message4[] ="VCU data structure initialized with default values.\r\n";
       const char message5[] ="Garbage Data in Data Block 1.\r\n";
       const char message6[] ="Eeprom Data Block 1 Read unsuccessfull.\r\n";


       //const char message7[] ="EEPROM is uninitialized.\r\n";
       //const char message8[] ="EEPROM is IDLE. Waiting for next job.\r\n";
       //const char message9[] ="EEPROM is Busy, finishing up last job.\r\n";
       //const char message10[] ="EEPROM is busy internal.\r\n";
       //const char message11[] ="FEE Main function is called to finish last scheduled job.\r\n";
       const char message12[] ="VCU state update on eeprom scheduled succesfully.\r\n";
       const char message13[] ="Unable to schedule VCU state update on eeprom.\r\n";

       //const char message14[] ="Last scheduled job failed.\r\n";
       //const char message15[] ="Last job was canceled.\r\n";
       //const char message16[] ="Last read operation failed, the data block is corrupted.\r\n";
       //const char message17[] ="Last read operation failed, the data block is invalid.\r\n";


       //const char message18[] = "Last scheduled job OK.\r\n";
       const char message19[] ="Data block has garbage, initializing VCU with default values.\r\n";
       const char message20[] ="Initializing VCU with default values & creating data block 1.\r\n";
       //const char message21[] ="Data Block 1 created & updated successfully.\r\n";
       //const char message22[] ="Unable to create data block 1.\r\n";
       //const char message23[] ="Eeprom Task Entered.\r\n";

       const char message24[] ="Last shutdown not graceful.\r\n";
       const char message25[] ="Last shutdown was graceful.\r\n";

       //sciSend(scilinREG, (uint32_t) sizeof(message23), &message23[0]);
       //uint8 eepromVCUReceiveBuffer[72]; // Receive buffer for data read from eeprom


       TickType_t mylastTickCount;
       mylastTickCount = xTaskGetTickCount();  // For an accurate Task Blocking Time

       //volatile uint8_t initializationOccured;
       //initializationOccured =0;
       uint8_t lastShutDownFlag;
       uint8_t jobCompletedFlag;  // used for Synchronous EEPROM Jobs
       uint8_t jobScheduled;      // used for Asynchronous EEPROM Jobs

       char txBuffer[60];  // A buffer that will Messages from EEPROM Task, 100 Characters


       while(1){

           vTaskDelayUntil(&mylastTickCount,EEPROM_TASK_PERIOD_MS); // 3-10 millisecond blocking time for this task - ideally for eeprom, but we can adjust

#ifdef PMU_CYCLE
       // Start timer.
       cycles_PMU_start = timer_Start();
       gioSetBit(gioPORTA, 5 , 1);
       //gioToggleBit(gioPORTA, 5);
#endif
           if (TASK_PRINT) {UARTSend(PC_UART, "EEPROM TASK\r\n");}


           if(!initializationOccured){
               if(eeprom_Status(EEP0)== IDLE){
                     jobCompletedFlag = eeprom_Read(EEP0, DATA_BLOCK_2, 0x48, &lastShutDownFlag, 0x01, SYNC); // Read the last saved POWER_FAILURE_FLAG value from eeprom -> it is the last byte of the VCUData Structure

                     if(jobCompletedFlag==E_OK){
                         // Print Data block 1 read successful
                         //sciSend(scilinREG, (uint32_t) sizeof(message1), &message1[0]);
                             if(lastShutDownFlag){ // Last shutdown was due to a Fault

                                     sprintf(txBuffer, message24); // Write  message to txBuffer
                                     xQueueSendToBack(eepromMessages,(void *)txBuffer, pdMS_TO_TICKS(0)); // Don't block if Queue is already full

                                          /**************************************** Critical Section of this Task ****************************************/
                                     if(xSemaphoreTake(vcuKey,pdMS_TO_TICKS(10))==1){ // Protect vcuStructure & wait for 10 milliseconds, if key not available, skip
                                               if(eeprom_Status(EEP0)== IDLE){

                                                   jobCompletedFlag = eeprom_Read(EEP0, DATA_BLOCK_2, 0, (uint8_t *)pVCUDataStructure, UNKNOWN_BLOCK_LENGTH, SYNC);
                                                      if(jobCompletedFlag==E_OK){
                                                          // Print VCU data structure loaded from eeprom successfully.
                                                          //sciSend(scilinREG, (uint32_t) sizeof(message2), &message2[0]);
                                                          sprintf(txBuffer, message2); // Write  message to txBuffer
                                                           xQueueSendToBack(eepromMessages,(void *)txBuffer, pdMS_TO_TICKS(0)); // Don't block if Queue is already full
                                                           initializationOccured = 1;
                                                      }else if(jobCompletedFlag == E_NOT_OK){
                                                          // Print VCU data structure load from eeprom failed.
                                                          //sciSend(scilinREG, (uint32_t) sizeof(message3), &message3[0]);
                                                      }
                                                  /**************************************** Critical Section of this Task ****************************************/
                                               }else if (eeprom_Status(EEP0)== UNINIT){
                                                   //sciSend(scilinREG, (uint32_t) sizeof(message7), &message7[0]);

                                               }else if(eeprom_Status(EEP0) == BUSY){
                                                   //sciSend(scilinREG, (uint32_t) sizeof(message9), &message9[0]);

                                               }else if(eeprom_Status(EEP0) == BUSY_INTERNAL){
                                                   //sciSend(scilinREG, (uint32_t) sizeof(message10), &message10[0]);
                                               }
                                          xSemaphoreGive(vcuKey);

                                     }
                             }else if(!lastShutDownFlag){  // Last shutdown was graceful
                                         sprintf(txBuffer, message25); // Write  message to txBuffer
                                         xQueueSendToBack(eepromMessages,(void *)txBuffer, pdMS_TO_TICKS(0)); // Don't block if Queue is already full

                                         /**************************************** Critical Section of this Task ****************************************/
                                      if(xSemaphoreTake(vcuKey,pdMS_TO_TICKS(10))==1){ // Protect vcuStructure & wait for 10 milliseconds
                                          initData(&VCUData);
                                          // Print VCU data structure initialized with default values.
                                          //sciSend(scilinREG, (uint32_t) sizeof(message4), &message4[0]);
                                          sprintf(txBuffer, message4); // Write  message to txBuffer
                                          xQueueSendToBack(eepromMessages,(void *)txBuffer, pdMS_TO_TICKS(0)); // Don't block if Queue is already full
                                          /**************************************** Critical Section of this Task ****************************************/
                                          xSemaphoreGive(vcuKey);

                                          initializationOccured =1;
                                          /* The following code will schedule a asynchronous write to update eeprom bank7 with our starting VCU state - needed to bypass BLOCK_INVALID error due to previous
                                          *   operation. The BLOCK_INVALID at startup can occur due to eeprom memory corruption or when the eeprom is not configured.
                                          */
                                          if(eeprom_Status(EEP0)==IDLE){
                                              swiSwitchToMode(SYSTEM_MODE);
                                              jobScheduled = eeprom_Write(EEP0, DATA_BLOCK_2, (uint8_t *)pVCUDataStructure, ASYNC);
                                              swiSwitchToMode(USER_MODE);
                                              if(jobScheduled==E_OK){
                                                   // Print "VCU state update on eeprom scheduled succesfully."
                                                  //sciSend(scilinREG, (uint32_t) sizeof(message12), &message12[0]);

                                              }else if(jobScheduled==E_NOT_OK){
                                                  // Print "Unable to schedule VCU state update on eeprom"
                                                  //sciSend(scilinREG, (uint32_t) sizeof(message13), &message13[0]);

                                              }
                                          }
                                      }

                             }else{
                                      // Garbage data in Data Block 1
                                     //sciSend(scilinREG, (uint32_t) sizeof(message5), &message5[0]);
                                     sprintf(txBuffer, message5); // Write  message to txBuffer
                                     xQueueSendToBack(eepromMessages,(void *)txBuffer, pdMS_TO_TICKS(0)); // Don't block if Queue is already full
                                     initializationOccured = 0;
                                     if((eeprom_lastJobStatus(EEP0)== BLOCK_INVALID) || (eeprom_lastJobStatus(EEP0)== BLOCK_INCONSISTENT)){
                                         //sciSend(scilinREG, (uint32_t) sizeof(message19), &message19[0]);
                                         sprintf(txBuffer, message19); // Write  message to txBuffer
                                         xQueueSendToBack(eepromMessages,(void *)txBuffer, pdMS_TO_TICKS(0)); // Don't block if Queue is already full

                                         // Data Block Invalidated or Inconsistent, either way, we lost the previous power cycle shutdown flag, re-initialized VCU data structure with default values.
                                                /**************************************** Critical Section of this Task ****************************************/
                                           if(xSemaphoreTake(vcuKey,pdMS_TO_TICKS(10))==1){ // Protect vcuStructure & wait for 10 milliseconds, if key not available, come back later.
                                               initData(&VCUData);
                                               // Print VCU data structure initialized with default values.
                                               sprintf(txBuffer, message4); // Write  message to txBuffer
                                               xQueueSendToBack(eepromMessages,(void *)txBuffer, pdMS_TO_TICKS(0)); // Don't block if Queue is already full
                                               //sciSend(scilinREG, (uint32_t) sizeof(message4), &message4[0]);
                                               /**************************************** Critical Section of this Task ****************************************/
                                               xSemaphoreGive(vcuKey);
                                               initializationOccured =1;
                                               /* The following code will schedule a asynchronous write to update eeprom bank7 with our starting VCU state - needed to bypass BLOCK_INVALID error due to previous
                                                *   operation. The BLOCK_INVALID at startup can occur due to eeprom memory corruption or when the eeprom is not configured.
                                               */
                                               if(eeprom_Status(EEP0)==IDLE){
                                                 swiSwitchToMode(SYSTEM_MODE);
                                                 jobScheduled = eeprom_Write(EEP0, DATA_BLOCK_2, (uint8_t *)pVCUDataStructure, ASYNC);
                                                 swiSwitchToMode(USER_MODE);
                                                 if(jobScheduled==E_OK){
                                                      // Print "VCU state update on eeprom scheduled succesfully."
                                                     //sciSend(scilinREG, (uint32_t) sizeof(message12), &message12[0]);
                                                     sprintf(txBuffer, message12); // Write  message to txBuffer
                                                     xQueueSendToBack(eepromMessages,(void *)txBuffer, pdMS_TO_TICKS(0)); // Don't block if Queue is already full

                                                 }else if(jobScheduled==E_NOT_OK){
                                                     // Print "Unable to schedule VCU state update on eeprom"
                                                     //sciSend(scilinREG, (uint32_t) sizeof(message13), &message13[0]);
                                                     sprintf(txBuffer, message13); // Write  message to txBuffer
                                                     xQueueSendToBack(eepromMessages,(void *)txBuffer, pdMS_TO_TICKS(0)); // Don't block if Queue is already full

                                                 }
                                             }
                                         }
                                    }
                             }

                     }else if(jobCompletedFlag==E_NOT_OK){
                         // Print Data Block 1 read unsuccesfull -> This can happen due to data block 1 being corrupted or not valid - i.e. Data block 1 was never initialized.
                         //sciSend(scilinREG, (uint32_t) sizeof(message6), &message6[0]);
                         sprintf(txBuffer, message6); // Write  message to txBuffer
                         xQueueSendToBack(eepromMessages,(void *)txBuffer, pdMS_TO_TICKS(0)); // Don't block if Queue is already full

                         //initializationOccured = 0;
                             if((eeprom_lastJobStatus(EEP0)== BLOCK_INVALID) || (eeprom_lastJobStatus(EEP0)== BLOCK_INCONSISTENT)){
                                   //sciSend(scilinREG, (uint32_t) sizeof(message20), &message20[0]);
                                 sprintf(txBuffer, message20); // Write  message to txBuffer
                                 xQueueSendToBack(eepromMessages,(void *)txBuffer, pdMS_TO_TICKS(0)); // Don't block if Queue is already full

                                   // Data Block Invalidated or Inconsistent, either way, we lost the previous power cycle shutdown flag, re-initialized VCU data structure with default values.
                                          /**************************************** Critical Section of this Task ****************************************/
                                     if(xSemaphoreTake(vcuKey,pdMS_TO_TICKS(10))==1){ // Protect vcuStructure & wait for 10 milliseconds, if key not available, come back later.
                                          initData(&VCUData);
                                         // Print VCU data structure initialized with default values.
                                         //sciSend(scilinREG, (uint32_t) sizeof(message4), &message4[0]);
                                          sprintf(txBuffer, message4); // Write  message to txBuffer
                                          xQueueSendToBack(eepromMessages,(void *)txBuffer, pdMS_TO_TICKS(0)); // Don't block if Queue is already full
                                         /**************************************** Critical Section of this Task ****************************************/
                                          xSemaphoreGive(vcuKey);
                                          initializationOccured =1;

                                          /* The following code will schedule a asynchronous write to update eeprom bank7 with our starting VCU state - needed to bypass BLOCK_INVALID error due to previous
                                           *   operation. The BLOCK_INVALID at startup can occur due to eeprom memory corruption or when the eeprom is not configured.
                                           */
                                          if(eeprom_Status(EEP0)==IDLE){

                                            swiSwitchToMode(SYSTEM_MODE);
                                            jobScheduled = eeprom_Write(EEP0, DATA_BLOCK_2, (uint8_t *)pVCUDataStructure, ASYNC);
                                            swiSwitchToMode(USER_MODE);
                                            if(jobScheduled==E_OK){
                                                 // Print "VCU state update on eeprom scheduled succesfully."
                                                //sciSend(scilinREG, (uint32_t) sizeof(message12), &message12[0]);
                                                sprintf(txBuffer, message12); // Write  message to txBuffer
                                                xQueueSendToBack(eepromMessages,(void *)txBuffer, pdMS_TO_TICKS(0)); // Don't block if Queue is already full

                                            }else if(jobScheduled==E_NOT_OK){
                                                // Print "Unable to schedule VCU state update on eeprom"
                                                //sciSend(scilinREG, (uint32_t) sizeof(message13), &message13[0]);
                                                sprintf(txBuffer, message13); // Write  message to txBuffer
                                                xQueueSendToBack(eepromMessages,(void *)txBuffer, pdMS_TO_TICKS(0)); // Don't block if Queue is already full

                                            }
                                          }
                                     }
                               }else{

                                   // The Flash Memory is empty or the Data block is corrupted, no choice but to

                                   if(xSemaphoreTake(vcuKey,pdMS_TO_TICKS(10))==1){ // Protect vcuStructure & wait for 10 milliseconds, if key not available, come back later.
                                         initData(&VCUData);
                                        // Print VCU data structure initialized with default values.
                                        //sciSend(scilinREG, (uint32_t) sizeof(message4), &message4[0]);
                                         sprintf(txBuffer, message4); // Write  message to txBuffer
                                         xQueueSendToBack(eepromMessages,(void *)txBuffer, pdMS_TO_TICKS(0)); // Don't block if Queue is already full
                                        /**************************************** Critical Section of this Task ****************************************/
                                         xSemaphoreGive(vcuKey);
                                         initializationOccured =1;
                                         /* The following code will schedule a asynchronous write to update eeprom bank7 with our starting VCU state - needed to bypass BLOCK_INVALID error due to previous
                                          *   operation. The BLOCK_INVALID at startup can occur due to eeprom memory corruption or when the eeprom is not configured.
                                          */
                                         if(eeprom_Status(EEP0)==IDLE){

                                           swiSwitchToMode(SYSTEM_MODE);
                                           jobScheduled = eeprom_Write(EEP0, DATA_BLOCK_2, (uint8_t *)pVCUDataStructure, ASYNC);
                                           swiSwitchToMode(USER_MODE);
                                           if(jobScheduled==E_OK){
                                                // Print "VCU state update on eeprom scheduled succesfully."
                                               //sciSend(scilinREG, (uint32_t) sizeof(message12), &message12[0]);
                                               sprintf(txBuffer, message12); // Write  message to txBuffer
                                               xQueueSendToBack(eepromMessages,(void *)txBuffer, pdMS_TO_TICKS(0)); // Don't block if Queue is already full

                                           }else if(jobScheduled==E_NOT_OK){
                                               // Print "Unable to schedule VCU state update on eeprom"
                                               //sciSend(scilinREG, (uint32_t) sizeof(message13), &message13[0]);
                                               sprintf(txBuffer, message13); // Write  message to txBuffer
                                               xQueueSendToBack(eepromMessages,(void *)txBuffer, pdMS_TO_TICKS(0)); // Don't block if Queue is already full

                                           }
                                         }
                                  }

                               }
                     }
               }else if (eeprom_Status(EEP0)== UNINIT){
                   //sciSend(scilinREG, (uint32_t) sizeof(message7), &message7[0]);

               }else if(eeprom_Status(EEP0) == BUSY){
                   //sciSend(scilinREG, (uint32_t) sizeof(message9), &message9[0]);

               }else if(eeprom_Status(EEP0) == BUSY_INTERNAL){
                   //sciSend(scilinREG, (uint32_t) sizeof(message10), &message10[0]);
               }

           }else if(initializationOccured){
                    if (eeprom_Status(EEP0)==UNINIT){
                        //sciSend(scilinREG, (uint32_t) sizeof(message7), &message7[0]);

                    }else if(eeprom_Status(EEP0)==IDLE){
                        //sciSend(scilinREG, (uint32_t) sizeof(message8), &message8[0]);

                        if(eeprom_lastJobStatus(EEP0)==JOB_PENDING){
                                        // Call the TI_FeeMainfunction to execute last scheduled job because there is a job pending.
                                       //sciSend(scilinREG, (uint32_t) sizeof(message11), &message11[0]);
                                       eepromNonBlockingMain(); // This will finish the last scheduled asynchronouse job

                        }else if(eeprom_lastJobStatus(EEP0)==JOB_OK){
                                        // Inside this block -> VCU Structure has been initialized either from EEPROM or with Default values.
                                       // If last scheduled eeprom job completed, then schedule update .
                                    //sciSend(scilinREG, (uint32_t) sizeof(message18), &message18[0]);

                                    swiSwitchToMode(SYSTEM_MODE);
                                    jobScheduled = eeprom_Write(EEP0, DATA_BLOCK_2, (uint8_t *)pVCUDataStructure, ASYNC);
                                    swiSwitchToMode(USER_MODE);
                                    if(jobScheduled==E_OK){
                                         // Print "VCU state update on eeprom scheduled succesfully."
                                        //sciSend(scilinREG, (uint32_t) sizeof(message12), &message12[0]);
                                    }else if(jobScheduled==E_NOT_OK){
                                        // Print "Unable to schedule VCU state update on eeprom"
                                        //sciSend(scilinREG, (uint32_t) sizeof(message13), &message13[0]);
                                    }


                        }else if(eeprom_lastJobStatus(EEP0)==JOB_FAILED){
                            //sciSend(scilinREG, (uint32_t) sizeof(message14), &message14[0]);

                        }else if((eeprom_lastJobStatus(EEP0))==JOB_CANCELLED){
                            //sciSend(scilinREG, (uint32_t) sizeof(message15), &message15[0]);

                        }else if((eeprom_lastJobStatus(EEP0))==BLOCK_INCONSISTENT){
                            //sciSend(scilinREG, (uint32_t) sizeof(message16), &message16[0]);
                        }
                        else if((eeprom_lastJobStatus(EEP0))==BLOCK_INVALID){
                            //sciSend(scilinREG, (uint32_t) sizeof(message17), &message17[0]);
                        }

                    }else if(eeprom_Status(EEP0)==BUSY){
                        //sciSend(scilinREG, (uint32_t) sizeof(message9), &message9[0]);
                        eepromNonBlockingMain(); // This will finish the last scheduled asynchronouse job

                    }else if(eeprom_Status(EEP0)==BUSY_INTERNAL){
                        //sciSend(scilinREG, (uint32_t) sizeof(message10), &message10[0]);

                    }
           }
#ifdef PMU_CYCLE
       // Start timer.
       time_PMU_code_uSecond = timer_Stop(cycles_PMU_start, CPU_CLOCK_MHz);
       gioSetBit(gioPORTA, 5 , 0);
       //gioToggleBit(gioPORTA, 5);
#endif
        }

}


