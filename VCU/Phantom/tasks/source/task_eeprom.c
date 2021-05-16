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
#include "board_hardware.h" // Has definition of State possible values

// ++ This is the vcuState memory location in VCU Data structure
#define VCU_STATE 0x5B  // 0x5B = 91
// -- This is the vcuState memory location in VCU Data structure

extern TaskHandle_t eepromHandler; // Eeprom Task handler
extern SemaphoreHandle_t vcuKey;
extern data VCUData;
void *pVCUDataStructure = &VCUData;  // Need this void pointer to read from eeprom bank - Added by jjkhan
extern data* VCUDataPtr; // Need this pointer to update "state" with "vcuState" when last shutdown is not graceful
//extern State state; //  In state machine task can simply use VCUDataPtr->vcuState, instead of "state" ?
//++ Added by jjkhan for execution time measurement

#include <Phantom/support/execution_timer.h>

#define CPU_CLOCK_MHz (float) 160.0  // System clock is 180 MHz, RTI Clock is 80MHz
volatile unsigned long cycles_PMU_start; // CPU cycle count at start
volatile float time_PMU_code_uSecond; // the calculated time in uSecond.

//-- Added by jjkhan for execution time measurement

//++ For Messages from EEPROM Task

extern QueueHandle_t eepromMessages;
volatile uint8_t initializationOccured = 0;
char txBuffer[60];  // A buffer that will hold Messages from EEPROM Task, 60 Characters

//-- For Messages from EEPROM Task

//++ Status place holders
static volatile TI_FeeModuleStatusType fee_Status; // For Status of the FEE
static volatile TI_FeeJobResultType lastJob_Status; // For Status of the last job
static uint8_t jobCompletedFlag;   // Status indicator used for Synchronous EEPROM Jobs
static uint8_t jobScheduled;      // Status indicatro used for Asynchronous EEPROM Jobs
static State lastStoredState;  // used for storing last VCU state
// -- Status place holders

// ++ Need to define EEPROM_DEBUG_MESSAGES in this file to be able to send push EEPROM messages into Queue 'eepromMessages'
#define EEPROM_DEBUG_MESSAGES 1
// -- Need to define EEPROM_DEBUG_MESSAGES in this file to be able to send push EEPROM messages into Queue 'eepromMessages'

//++ Debug Messages
#if EEPROM_DEBUG_MESSAGES == 1
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
#endif
// -- Debug Messages

//++ Helper function Prototypes
inline void loadWithDefaultValues(void);
inline void loadFromEeprom(void);
inline void scheduleNewJob(void);
inline void updateEeprom(void);
inline void workOnLastScheduledJob(void);

inline void initializeVCU(void);
//-- Helper function Prototypes

void vEeprom(void *p){

       TickType_t mylastTickCount;
       mylastTickCount = xTaskGetTickCount();  // For an accurate Task Blocking Time

       while(1){

           vTaskDelayUntil(&mylastTickCount,EEPROM_TASK_PERIOD_MS);
#ifdef PMU_CYCLE
       // Start timer.
       cycles_PMU_start = timer_Start();
       gioSetBit(gioPORTA, 5 , 1);
       //gioToggleBit(gioPORTA, 5);
#endif
           if (TASK_PRINT) {UARTSend(PC_UART, "EEPROM TASK\r\n");}

           // Read current Status of the FEE
           fee_Status = eeprom_Status(EEP0);

           if(!initializationOccured){
               if(fee_Status== IDLE){
                     initializeVCU();
               }else if (fee_Status== UNINIT){
                   //sciSend(scilinREG, (uint32_t) sizeof(message7), &message7[0]);

               }else if(fee_Status == BUSY){
                   //sciSend(scilinREG, (uint32_t) sizeof(message9), &message9[0]);

               }else if(fee_Status == BUSY_INTERNAL){
                   //sciSend(scilinREG, (uint32_t) sizeof(message10), &message10[0]);
               }

           }else if(initializationOccured){

                if (fee_Status==UNINIT){
                    //sciSend(scilinREG, (uint32_t) sizeof(message7), &message7[0]);

                }else if(fee_Status==IDLE){
                    //sciSend(scilinREG, (uint32_t) sizeof(message8), &message8[0]);
                    updateEeprom();
                }else if(fee_Status==BUSY){
                    //sciSend(scilinREG, (uint32_t) sizeof(message9), &message9[0]);
                    workOnLastScheduledJob(); // This will finish the last scheduled Asynchronous job

                }else if(fee_Status==BUSY_INTERNAL){
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

inline void loadFromEeprom(void){

        /**************************************** Critical Section of this Task ****************************************/
    if(xSemaphoreTake(vcuKey,pdMS_TO_TICKS(10))==1){ // Protect vcuStructure & wait for 10 milliseconds, if key not available, skip
             if(fee_Status== IDLE){
                 jobCompletedFlag = eeprom_Read(EEP0, DATA_BLOCK_2, 0, (uint8_t *)pVCUDataStructure, UNKNOWN_BLOCK_LENGTH, SYNC);
                if(jobCompletedFlag==E_OK){

#if EEPROM_DEBUG_MESSAGES  == 1
                    sprintf(txBuffer, message2); // Write  message to txBuffer
                     xQueueSendToBack(eepromMessages,(void *)txBuffer, pdMS_TO_TICKS(0)); // Don't block if Queue is already full
#endif
                     initializationOccured = 1;
                     //state = VCUDataPtr->vcuState; // Get last saved state after loading VCUData from eeprom memory
                }else if(jobCompletedFlag == E_NOT_OK){
                    // Print VCU data structure load from eeprom failed.
                }
                xSemaphoreGive(vcuKey);
        /**************************************** Critical Section of this Task ****************************************/
             }else if (fee_Status== UNINIT){
                 //sciSend(scilinREG, (uint32_t) sizeof(message7), &message7[0]);

             }else if(fee_Status == BUSY){
                 //sciSend(scilinREG, (uint32_t) sizeof(message9), &message9[0]);

             }else if(fee_Status == BUSY_INTERNAL){
                 //sciSend(scilinREG, (uint32_t) sizeof(message10), &message10[0]);
             }

    }
}

inline void loadWithDefaultValues(void){

        /**************************************** Critical Section of this Task ****************************************/
     if(xSemaphoreTake(vcuKey,pdMS_TO_TICKS(10))==1){ // Protect vcuStructure & wait for 10 milliseconds
         initData(&VCUData);
         // Print VCU data structure initialized with default values.
#if EEPROM_DEBUG_MESSAGES  == 1
         sprintf(txBuffer, message4); // Write  message to txBuffer
         xQueueSendToBack(eepromMessages,(void *)txBuffer, pdMS_TO_TICKS(0)); // Don't block if Queue is already full
#endif
         /**************************************** Critical Section of this Task ****************************************/
         xSemaphoreGive(vcuKey);

         initializationOccured =1;
         /*  The following code will schedule a asynchronous write to update eeprom bank7 with our starting VCU state -
          *  needed to bypass BLOCK_INVALID error due to previous
         *   operation.
         *   The BLOCK_INVALID at startup can occur due to eeprom memory corruption or when the eeprom is not configured.
         */
         if(fee_Status==IDLE){
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

inline void scheduleNewJob(void){

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

inline void workOnLastScheduledJob(void){
    eepromNonBlockingMain(); // This will finish the last scheduled asynchronouse job
}

inline void initializeVCU(void){
    // Read the last saved POWER_FAILURE_FLAG, it is the last byte of the VCUData Structure,
    // VCUData Structure (at the time of integration) -> 73 Bytes
    // The last byte == POWER_FAILURE_FLAG,
    // Therefore, need to offset memory pointer by 72 bytes (in hex = 0x48)

    jobCompletedFlag = eeprom_Read(EEP0, DATA_BLOCK_2, VCU_STATE, &lastStoredState, 0x01, SYNC);
     if(jobCompletedFlag==E_OK){
         if(lastStoredState==SEVERE_FAULT || lastStoredState==MINOR_FAULT ){ // Last shutdown was due to a Fault
#if EEPROM_DEBUG_MESSAGES  == 1
                 sprintf(txBuffer, message24); // Write  message to txBuffer
                 xQueueSendToBack(eepromMessages,(void *)txBuffer, pdMS_TO_TICKS(0)); // Don't block if Queue is already full
#endif
                 loadFromEeprom();
         }else if(lastStoredState!=SEVERE_FAULT || lastStoredState!=MINOR_FAULT){  // Last shutdown was graceful
#if EEPROM_DEBUG_MESSAGES  == 1
                 sprintf(txBuffer, message25); // Write  message to txBuffer
                 xQueueSendToBack(eepromMessages,(void *)txBuffer, pdMS_TO_TICKS(0)); // Don't block if Queue is already full
#endif
                 loadWithDefaultValues();

         }else{
                 // Garbage data in Data Block 1
#if EEPROM_DEBUG_MESSAGES  == 1
                 sprintf(txBuffer, message5); // Write  message to txBuffer
                 xQueueSendToBack(eepromMessages,(void *)txBuffer, pdMS_TO_TICKS(0)); // Don't block if Queue is already full
#endif
                 lastJob_Status = eeprom_lastJobStatus(EEP0); // Read Status of last job

                 if((lastJob_Status== BLOCK_INVALID) || (lastJob_Status== BLOCK_INCONSISTENT)){
#if EEPROM_DEBUG_MESSAGES  == 1
                     sprintf(txBuffer, message19); // Write  message to txBuffer
                     xQueueSendToBack(eepromMessages,(void *)txBuffer, pdMS_TO_TICKS(0)); // Don't block if Queue is already full
#endif
                     // Data Block Invalidated or Inconsistent,
                     // either way, we lost the previous power cycle shutdown flag,
                     // re-initialized VCU data structure with default values.
                     loadWithDefaultValues();
                }
         }

     }else if(jobCompletedFlag==E_NOT_OK){

         // Print Data Block 1 read unsuccessful,
         // This can happen due to data block 1 being corrupted or not valid - i.e. Data block 1 was never initialized.
#if  EEPROM_DEBUG_MESSAGES  == 1
         sprintf(txBuffer, message6); // Write  message to txBuffer
         xQueueSendToBack(eepromMessages,(void *)txBuffer, pdMS_TO_TICKS(0)); // Don't block if Queue is already full
#endif
         lastJob_Status = eeprom_lastJobStatus(EEP0); // Read Status of last job
         if((lastJob_Status== BLOCK_INVALID) || (lastJob_Status== BLOCK_INCONSISTENT)){
#if  EEPROM_DEBUG_MESSAGES  == 1
             sprintf(txBuffer, message20); // Write  message to txBuffer
             xQueueSendToBack(eepromMessages,(void *)txBuffer, pdMS_TO_TICKS(0)); // Don't block if Queue is already full
#endif
              // Data Block Invalidated or Inconsistent,
              //either way, we lost the previous power cycle shutdown flag,
              // re-initialized VCU data structure with default values.
             loadWithDefaultValues();

           }else{
               // The Flash Memory is empty or the Data block is corrupted, no choice but to initialize with Default values
             loadWithDefaultValues();
           }
     }
}

inline void updateEeprom(void){

        lastJob_Status = eeprom_lastJobStatus(EEP0); // Get information on how last job went
        if(lastJob_Status==JOB_PENDING){
            workOnLastScheduledJob(); // This will finish the last scheduled Asynchronous job

        }else if(lastJob_Status==JOB_OK){
            // Inside this block -> VCU Structure has been initialized either from EEPROM or with Default values.
            // If last scheduled eeprom job completed, then schedule update .
            //sciSend(scilinREG, (uint32_t) sizeof(message18), &message18[0]);
          scheduleNewJob();

        }else if(lastJob_Status==JOB_FAILED){
            //sciSend(scilinREG, (uint32_t) sizeof(message14), &message14[0]);

        }else if(lastJob_Status==JOB_CANCELLED){
            //sciSend(scilinREG, (uint32_t) sizeof(message15), &message15[0]);

        }else if(lastJob_Status==BLOCK_INCONSISTENT){
            //sciSend(scilinREG, (uint32_t) sizeof(message16), &message16[0]);
        }
        else if(lastJob_Status==BLOCK_INVALID){
            //sciSend(scilinREG, (uint32_t) sizeof(message17), &message17[0]);

        }
}
