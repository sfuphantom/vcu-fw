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


extern TaskHandle_t eepromHandler; // Eeprom Task handler
extern SemaphoreHandle_t vcuKey;
extern data VCUData;
extern void *pVCUDataStructure;


//++ Added by jjkhan for execution time measurement

#include "../../execution_timer.h"

#define CPU_CLOCK_MHz (float) 80.0
volatile unsigned long cycles_PMU_start; // CPU cycle count at start
volatile float time_PMU_code_uSecond; // the calculated time in uSecond.

//-- Added by jjkhan for execution time measurement


void vEeprom(void *p){

       /*   Uncomment the message you want send for debugging purposes.
       uint8 message1[] ="Eeprom Data Block 1 Read successfull.\r\n";
       uint8 message2[] ="VCU data structure loaded from eeprom successfully.\r\n";
       uint8 message3[] ="VCU data structure load from eeprom failed.\r\n";
       uint8 message4[] ="VCU data structure initialized with default values.\r\n";
       uint8 message5[] ="Garbage Data in Data Block 1.\r\n";
       uint8 message6[] ="Eeprom Data Block 1 Read unsuccessfull.\r\n";


       uint8 message7[] ="EEPROM is uninitialized.\r\n";
       uint8 message8[] ="EEPROM is IDLE. Waiting for next job.\r\n";
       uint8 message9[] ="EEPROM is Busy, finishing up last job.\r\n";
       uint8 message10[] ="EEPROM is busy internal.\r\n";
       uint8 message11[] ="FEE Main function is called to finish last scheduled job.\r\n";
       uint8 message12[] ="VCU state update on eeprom scheduled succesfully.\r\n";
       uint8 message13[] ="Unable to schedule VCU state update on eeprom.\r\n";

       uint8 message14[] ="Last scheduled job failed.\r\n";
       uint8 message15[] ="Last job was canceled.\r\n";
       uint8 message16[] ="Last read operation failed, the data block is corrupted.\r\n";
       uint8 message17[] ="Last read operation failed, the data block is invalid.\r\n";


       uint8 message18[] = "Last scheduled job OK.\r\n";
       uint8 message19[] ="Data block has garbage, initializing VCU with default values.\r\n";
       uint8 message20[] ="Data block was invalidated/inconsistent, initializing VCU with default values & creating data block 1.\r\n";
       uint8 message21[] ="Data Block 1 created & updated successfully.\r\n";
       uint8 message22[] ="Unable to create data block 1.\r\n";
       uint8 message23[] ="Eeprom Task Entered.\r\n"; */

       //sciSend(scilinREG, (uint32_t) sizeof(message23), &message23[0]);
       //uint8 eepromVCUReceiveBuffer[72]; // Receive buffer for data read from eeprom


       //++ Added by jjkhan: For Storing RUN Time stats
#ifdef RUN_TIME_STATS_EEPROM
        /* Buffer to trace informations */
       static char cTraceBuffer[300];
       // -- Added by jjkhan For Storing RUN Time stats
#endif

       TickType_t mylastTickCount;
       mylastTickCount = xTaskGetTickCount();  // For an accurate Task Blocking Time

       volatile uint8_t initializationOccured;
       initializationOccured =0;
       uint8_t lastShutDownFlag;
       uint8_t jobCompletedFlag;  // used for Synchronous EEPROM Jobs
       uint8_t jobScheduled;      // used for Asynchronous EEPROM Jobs



       while(1){

           vTaskDelayUntil(&mylastTickCount,EEPROM_TASK_PERIOD_MS); // 3-10 millisecond blocking time for this task - ideally for eeprom, but we can adjust

           cycles_PMU_start = timer_Start();// Start timer

           if (TASK_PRINT) {UARTSend(PC_UART, "EEPROM TASK\r\n");}


           if(!initializationOccured){
               if(eeprom_Status(EEP0)== IDLE){
                     jobCompletedFlag = eeprom_Read(EEP0, DATA_BLOCK_2, 0x48, &lastShutDownFlag, 0x01, SYNC); // Read the last saved POWER_FAILURE_FLAG value from eeprom -> it is the last byte of the VCUData Structure

                     if(jobCompletedFlag==E_OK){
                         // Print Data block 1 read succesfull
                         //sciSend(scilinREG, (uint32_t) sizeof(message1), &message1[0]);
                             if(lastShutDownFlag == 0xFF){ // Last shutdown was due to a Fault
                                          /**************************************** Critical Section of this Task ****************************************/
                                     if(xSemaphoreTake(vcuKey,pdMS_TO_TICKS(100))==1){ // Protect vcuStructure & wait for 10 milliseconds, if key not available, skip
                                               if(eeprom_Status(EEP0)== IDLE){

                                                   jobCompletedFlag = eeprom_Read(EEP0, DATA_BLOCK_2, 0, (uint8_t *)pVCUDataStructure, UNKNOWN_BLOCK_LENGTH, SYNC);
                                                      if(jobCompletedFlag==E_OK){
                                                          // Print VCU data structure loaded from eeprom successfully.
                                                          //sciSend(scilinREG, (uint32_t) sizeof(message2), &message2[0]);
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
                             }else if(lastShutDownFlag == 0){  // Last shutdown was graceful
                                         /**************************************** Critical Section of this Task ****************************************/
                                      if(xSemaphoreTake(vcuKey,pdMS_TO_TICKS(100))==1){ // Protect vcuStructure & wait for 10 milliseconds
                                          initData(&VCUData);
                                          // Print VCU data structure initialized with default values.
                                          //sciSend(scilinREG, (uint32_t) sizeof(message4), &message4[0]);
                                          initializationOccured =1;
                                          /**************************************** Critical Section of this Task ****************************************/
                                          xSemaphoreGive(vcuKey);
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
                                     initializationOccured = 0;
                                     if((eeprom_lastJobStatus(EEP0)== BLOCK_INVALID) || (eeprom_lastJobStatus(EEP0)== BLOCK_INCONSISTENT)){
                                         //sciSend(scilinREG, (uint32_t) sizeof(message19), &message19[0]);

                                         // Data Block Invalidated or Inconsistent, either way, we lost the previous power cycle shutdown flag, re-initialized VCU data structure with default values.
                                                /**************************************** Critical Section of this Task ****************************************/
                                           if(xSemaphoreTake(vcuKey,pdMS_TO_TICKS(100))==1){ // Protect vcuStructure & wait for 10 milliseconds, if key not available, come back later.
                                               initData(&VCUData);
                                               // Print VCU data structure initialized with default values.
                                               //sciSend(scilinREG, (uint32_t) sizeof(message4), &message4[0]);
                                               initializationOccured =1;
                                               /**************************************** Critical Section of this Task ****************************************/
                                               xSemaphoreGive(vcuKey);
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
                                    }
                             }

                     }else if(jobCompletedFlag==E_NOT_OK){
                         // Print Data Block 1 read unsuccesfull -> This can happen due to data block 1 being corrupted or not valid - i.e. Data block 1 was never initialized.
                         //sciSend(scilinREG, (uint32_t) sizeof(message6), &message6[0]);
                         initializationOccured = 0;
                             if((eeprom_lastJobStatus(EEP0)== BLOCK_INVALID) || (eeprom_lastJobStatus(EEP0)== BLOCK_INCONSISTENT)){
                                   //sciSend(scilinREG, (uint32_t) sizeof(message20), &message20[0]);

                                   // Data Block Invalidated or Inconsistent, either way, we lost the previous power cycle shutdown flag, re-initialized VCU data structure with default values.
                                          /**************************************** Critical Section of this Task ****************************************/
                                     if(xSemaphoreTake(vcuKey,pdMS_TO_TICKS(100))==1){ // Protect vcuStructure & wait for 10 milliseconds, if key not available, come back later.
                                          initData(&VCUData);
                                         // Print VCU data structure initialized with default values.
                                         //sciSend(scilinREG, (uint32_t) sizeof(message4), &message4[0]);
                                         initializationOccured =1;
                                         /**************************************** Critical Section of this Task ****************************************/
                                          xSemaphoreGive(vcuKey);

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


           time_PMU_code_uSecond = timer_Stop(cycles_PMU_start, CPU_CLOCK_MHz);
           // ++ Added by jjkhan for task profiling
#ifdef RUN_TIME_STATS_EEPROM
           vTaskGetRunTimeStats(cTraceBuffer);
           printf(cTraceBuffer);
#endif
           // -- Added by jjkhan for task profiling
        }

}


