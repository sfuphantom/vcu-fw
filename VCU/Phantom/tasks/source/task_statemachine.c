/*
 * task_throttle.c
 *
 *  Created on: Mar 31, 2020
 *      Author: gabriel
 *
 *
 *
 *
 *  Last Modified on: Dec 16, 2020  ->  jjkhan
 *
 */

#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "gio.h"
#include "het.h"
#include "os_queue.h"
#include "os_semphr.h"
#include "os_task.h"
#include "os_timer.h"
#include "priorities.h"
#include "Phantom_sci.h"
#include "FreeRTOS.h"
#include "os_projdefs.h"

// Needed the swiSwitchMode defined in here, will moved swiSwitchMode to board_hardware.h
#include "eeprom_driver.h"
#include "vcu_data.h" // data structure to hold VCU data
#include "task_statemachine.h"

uint32_t blue_duty = 100;
uint32_t blue_flag = 0;


/*
 *  task_eeprom.c initializes the VCUData structure based on last stored VCU state in eeprom.
 *      Task can't should not execute its body until initialization has occurred.
 */
// ++ Added by jjkhan
extern volatile uint8_t initializationOccured;
extern SemaphoreHandle_t vcuKey;
extern data* VCUDataPtr;
// -- Added by jjkhan



/*********************************************************************************
 *                               SYSTEM STATE FLAGS
 *********************************************************************************/
extern data* VCUDataPtr;


/* ++ Added by jjkhan */



int checkSDC(void){
        if( VCUDataPtr->DigitalVal.IMD_FAULT
                || VCUDataPtr->DigitalVal.BSPD_FAULT
                || VCUDataPtr->DigitalVal.BMS_GPIO_FAULT
                      ){
            return FAULT;
        }
            return NOFAULT;

}

int checkIMD(void){
    if(VCUDataPtr->DigitalVal.IMD_LOW_ISO_FAULT
            || VCUDataPtr->DigitalVal.IMD_SHORT_CIRCUIT_FAULT
            || VCUDataPtr->DigitalVal.IMD_DEVICE_ERR_FAULT
            || VCUDataPtr->DigitalVal.IMD_BAD_INFO_FAULT
            || VCUDataPtr->DigitalVal.IMD_UNDEF_ERR
            || VCUDataPtr->DigitalVal.IMD_GARBAGE_DATA_FAULT
            ){
        return FAULT;
    }
    return NOFAULT;
}

int checkBSE_APPS(void){
        if(VCUDataPtr->DigitalVal.BSE_SEVERE_RANGE_FAULT
                           || VCUDataPtr->DigitalVal.APPS1_SEVERE_RANGE_FAULT
                           || VCUDataPtr->DigitalVal.APPS2_SEVERE_RANGE_FAULT
                           || VCUDataPtr->DigitalVal.APPS_SEVERE_10DIFF_FAULT
                           || VCUDataPtr->DigitalVal.BSE_APPS_MINOR_SIMULTANEOUS_FAULT){
            return FAULT;
        }
            return NOFAULT;
}

int CheckHVLVSensor(void){
        if(VCUDataPtr->DigitalVal.HV_CURRENT_OUT_OF_RANGE
                || VCUDataPtr->DigitalVal.HV_VOLTAGE_OUT_OF_RANGE_FAULT
                || VCUDataPtr->DigitalVal.LV_CURRENT_OUT_OF_RANGE
                || VCUDataPtr->DigitalVal.LV_VOLTAGE_OUT_OF_RANGE
                || VCUDataPtr->DigitalVal.APPS_PROPORTION_ERROR){
             return FAULT;
        }
            return NOFAULT;
}

int checkCAN(void){
    if(VCUDataPtr->DigitalVal.CAN_ERROR_TYPE1 || VCUDataPtr->DigitalVal.CAN_ERROR_TYPE2){
        return FAULT;
    }
    return NOFAULT;
}

int isRTDS(void){
    if(VCUDataPtr->DigitalVal.RTDS){
        return SET;
    }
    return !SET;
}

int isTSAL_ON(void){
    if(VCUDataPtr->DigitalVal.TSAL_ON){
        return ON;
    }
    return !ON;
}

/***********************************************************
 * @function                - faultLocation
 *
 * @brief                   - This function is a helper function for vStateMachineTask, its used to find the faulty subsystems.
 *
 * @param[in]               - void
 *
 * @return                  - 32-bit value where each bit corresponds to if there is a fault in that subsystem.
 * @Note                    -  A '1' indicates fault(s), a '0' indicates no fault
 *
 *                             Bit 0 =  Shutdown Circuit has fault.
 *                             Bit 1 =  BSE or APPS has fault
 *                             Bit 2 =  HV or LV fault sensed.
 *                             Bit 3 =  Fault sent over CAN bus
 *                             Bit 4 =  IMD has a fault.
 *
 ***********************************************************/


uint16_t faultLocation(void){

    uint16_t systemFaultIndicator = 0;

    // Check if fault in Shutdown Circuit
    if(checkSDC()){
        systemFaultIndicator |= 1 << SDC_FAULT;
    }
    // Check if fault in APPS
    if(checkBSE_APPS()){
        systemFaultIndicator |= 1 << BSE_APPS_FAULT;
    }
    // Check if fault in HV or LV
    if(CheckHVLVSensor()){
        systemFaultIndicator |= 1 << HV_LV_FAULT;
    }
    // Check if fault from CAN
    if(checkCAN()){
        systemFaultIndicator |= 1 << CAN_FAULT;
    }

    // Check if fault from IMD
    if(checkIMD()){
        systemFaultIndicator |= 1 << IMD_SYSTEM_FAULT;
    }
    return systemFaultIndicator;

}


/***********************************************************
 * @function                - anyERRORS
 *
 * @brief                   - This function is a helper function for vStateMachineTask, its used to check if there are any faults, regardless if severe or minor
 *
 * @param[in]               - void
 *
 * @return                  - 1 or 0
 * @Note                    -  A '1' indicates fault(s), a '0' indicates no fault
 *
 ***********************************************************/

int anyFaults(void){
    if(checkSDC() || checkCAN() || checkIMD() || checkBSE_APPS()  || CheckHVLVSensor()){
        return FAULT;
    }
    return NOFAULT;
}


/***********************************************************
 * @function                - vStateMachineTask
 *
 * @brief                   - This task evaluates the state of the vehicle and controls any change of state
 *
 * @param[in]               - pvParameters
 *
 * @return                  - None
 * @Note                    - None
 ***********************************************************/

void vStateMachineTask(void *pvParameters){
//    uint32 lrval;
//    char stbuf[64];
//    int nchars;

    TickType_t xLastWakeTime;          // will hold the timestamp at which the task was last unblocked
    //const TickType_t xFrequency = 100; // task frequency in ms

    // Initialize the xLastWakeTime variable with the current time;
    xLastWakeTime = xTaskGetTickCount();


    /* ++ Added to simulate Timer for HV Current and Voltage Fault, will implement proper timer later - jjkhan */


    // ++ For HV Current
    uint8_t timer1_started = 0;
    //TimerHandle_t timer1_value = xTimers[2]; // HV Current Out of Range Timer
    // -- For HV Current


    // ++ For HV Voltage
    uint8_t timer2_started = 0;
    //TimerHandle_t timer2_value = xTimers[3]; // HV Voltage Out of Range Timer
    // -- For HV Voltage

    uint16_t faultNumber;
    State temp_state;

    /* -- Added to simulate Timer for HV Current and Voltage Fault, will implement proper timer later - jjkhan */

    while(true)
    {
        if(initializationOccured){
        // for timing:
        gioSetBit(hetPORT1, 9, 1);

        if (TASK_PRINT) {UARTSend(PC_UART, "STATE MACHINE UPDATE TASK\r\n");}

/*********************** STATE MACHINE EVALUATION ***********************************/


        if (state == TRACTIVE_OFF){
            /* ++ New Code: Added by jjkhan */


           if (STATE_PRINT) {UARTSend(PC_UART, "********TRACTIVE_OFF********");}

           if(isRTDS() || anyFaults()){
               state = SEVERE_FAULT;  // RTDS should be off when state is TRACTIVE_OFF && you all errors should be taken care before you proceed.
           }else if(isTSAL_ON() && !anyFaults() && !isRTDS()){
               state = TRACTIVE_ON; //No faults AND TSAL is on AND RTDS not set
           }

           /* -- New Code: Added by jjkhan */
        }else if (state == TRACTIVE_ON){


            temp_state = state;
            /* ++ New Code - Added by jjkhan */
            if(!isTSAL_ON()){
                state = SEVERE_FAULT;// TSAL light is OFF - shouldn't happen, but if it does, handle it - Redundant

            }else if(anyFaults()){

                  faultNumber = faultLocation();

                  // Shutdown Circuit Fault - its Severe so don't need to check other faults
                  if((faultNumber & (1U<<SDC_FAULT))){
                     //UARTSend(PC_UART,"SDC FAULT DETECTED.\r\n");
                     if (checkSDC()){
                         temp_state= SEVERE_FAULT;
                     }
                  }else if((faultNumber & (1U << IMD_SYSTEM_FAULT))){

                      // IMD Fault Checks - its Severe so don't need to check other faults
                     //UARTSend(PC_UART, "IMD FAULT DETECTED. \r\n");
                     if(checkIMD()){
                         temp_state=SEVERE_FAULT;
                     }
                  }
                  // Check BSE APPS Faults
                  if((faultNumber & (1U<<BSE_APPS_FAULT))){ // Either BSE or APPS Fault
                      //UARTSend(PC_UART, "BSE_APPS FAULT DETECTED. \r\n");
                      if(!(VCUDataPtr->DigitalVal.BSE_APPS_MINOR_SIMULTANEOUS_FAULT)){ // if the fault isn't the Minor Fault, then set SEVERE_FAULT and yield, don't need to check for minor
                          temp_state= SEVERE_FAULT;
                      }
                  }

                  if((faultNumber & (1U << HV_LV_FAULT))){
                      //UARTSend(PC_UART, "HV  FAULT DETECTED. \r\n");
                      // Check for SEVERE Faults Right now
                      if(timer1_started || timer2_started){
                          //UARTSend(PC_UART, "HV Timer Already started. \r\n");
                          if(VCUDataPtr->DigitalVal.HV_VOLTAGE_OUT_OF_RANGE_FAULT){
                              if(HV_VOLTAGE_TIMER_EXPIRED){
                                  temp_state = SEVERE_FAULT;
                                  timer2_started = 0; // Reset Timer
                                  HV_VOLTAGE_TIMER_EXPIRED=false;
                              }
                          }else if(VCUDataPtr->DigitalVal.HV_CURRENT_OUT_OF_RANGE){
                              if(HV_CURRENT_TIMER_EXPIRED){
                                  temp_state = SEVERE_FAULT;
                                  timer1_started = 0; // Reset Timer
                                  HV_CURRENT_TIMER_EXPIRED = false;

                              }
                          }

                      }
                  }

                  // Check CAN Faults
                  if((faultNumber & (1U<<CAN_FAULT))){ // Either CAN Message indicates a severe fault (TYPE1 ERROR) or a minor fault (TYPE2 ERROR)
                      //UARTSend(PC_UART, "CAN FAULT DETECTED. \r\n");

                      if(VCUDataPtr->DigitalVal.CAN_ERROR_TYPE1){ // Severe Fault message from CAN, so don't need to check other faults, change state and yield task
                          temp_state= SEVERE_FAULT;
                      }
                  }

                  if((temp_state!=SEVERE_FAULT)){  // Check for MINOR Faults

                      if((faultNumber & (1U << HV_LV_FAULT))){
                          //UARTSend(PC_UART, "Inside Minor Faults \r\n");

                          if(VCUDataPtr->DigitalVal.HV_VOLTAGE_OUT_OF_RANGE_FAULT){
                              HV_VOLTAGE_TIMER_EXPIRED = false;
                              if(xTimerStart(xTimers[3], 0)!=pdPASS){
                                  for(;;);
                              }else{
                                  timer1_started = 1;
                                  temp_state= MINOR_FAULT;
                              }

                          }else if(VCUDataPtr->DigitalVal.HV_CURRENT_OUT_OF_RANGE){
                              //UARTSend(PC_UART, "Inside HV Current Fault. \r\n");
                              HV_CURRENT_TIMER_EXPIRED = false;
                              if(xTimerStart(xTimers[2],0)!=pdPASS){
                                  for(;;);
                              }else{
                                  UARTSend(PC_UART, "Timer Started. \r\n");
                                  timer2_started=1;
                                  temp_state=MINOR_FAULT;
                              }
                          }else if(VCUDataPtr->DigitalVal.LV_CURRENT_OUT_OF_RANGE){
                              temp_state = MINOR_FAULT;

                          }else if(VCUDataPtr->DigitalVal.LV_VOLTAGE_OUT_OF_RANGE){
                              temp_state = MINOR_FAULT;
                          }else if(VCUDataPtr->DigitalVal.BSE_APPS_MINOR_SIMULTANEOUS_FAULT){

                              if(isRTDS()){
                                      temp_state=TRACTIVE_ON;
                              }else{
                                  temp_state = MINOR_FAULT;
                              }
                          }
                      }else if ((faultNumber & (1U<<CAN_FAULT))){

                          if(VCUDataPtr->DigitalVal.CAN_ERROR_TYPE2){
                              temp_state = MINOR_FAULT;
                          }
                      }
                  }
                  state = temp_state;



            }else if(isRTDS()){
                state = RUNNING;
            }

           /* -- New Code - Added by jjkhan */


        }else if (state == RUNNING){



            /* ++ New Code - Added by jjkhan */

            // Find fault in the system
            temp_state = state;

            if(anyFaults()){
                       faultNumber = faultLocation();

                         //UARTSend(PC_UART,"Got Fault number. \r\n");
                         if((faultNumber & (1U<<SDC_FAULT))){
                            //UARTSend(PC_UART,"SDC FAULT DETECTED.\r\n");
                            if (checkSDC()){
                                temp_state= SEVERE_FAULT;
                            }
                         }else if((faultNumber & (1U << IMD_SYSTEM_FAULT))){
                            //UARTSend(PC_UART, "IMD FAULT DETECTED. \r\n");
                            if(checkIMD()){
                                temp_state=SEVERE_FAULT;
                            }
                         }

                         if((faultNumber & (1U<<BSE_APPS_FAULT))){ // Either BSE or APPS Fault
                             //UARTSend(PC_UART, "BSE_APPS FAULT DETECTED. \r\n");
                             if(!(VCUDataPtr->DigitalVal.BSE_APPS_MINOR_SIMULTANEOUS_FAULT)){ // if the fault isn't the Minor Fault, then set SEVERE_FAULT and yield, don't need to check for minor
                                 temp_state= SEVERE_FAULT;
                             }
                         }
                         if((faultNumber & (1U<<CAN_FAULT))){ // Either CAN Message indicates a severe fault (TYPE1 ERROR) or a minor fault (TYPE2 ERROR)
                             //UARTSend(PC_UART, "CAN FAULT DETECTED. \r\n");

                             if(VCUDataPtr->DigitalVal.CAN_ERROR_TYPE1){ // Severe Fault message from CAN, so don't need to check other faults, change state and yield task
                                 temp_state= SEVERE_FAULT;
                             }
                         }
                         if((faultNumber & (1U << HV_LV_FAULT))){

                             // Check for SEVERE Faults Right now
                             if(timer1_started || timer2_started){
                                 if(VCUDataPtr->DigitalVal.HV_VOLTAGE_OUT_OF_RANGE_FAULT){
                                     if(HV_VOLTAGE_TIMER_EXPIRED){
                                         temp_state = SEVERE_FAULT;
                                         timer2_started = 0; // Reset Timer
                                         HV_VOLTAGE_TIMER_EXPIRED=false;
                                     }
                                 }else if(VCUDataPtr->DigitalVal.HV_CURRENT_OUT_OF_RANGE){
                                     if(HV_CURRENT_TIMER_EXPIRED){
                                         temp_state = SEVERE_FAULT;
                                         timer1_started = 0; // Reset Timer
                                         HV_CURRENT_TIMER_EXPIRED = false;

                                     }
                                 }

                             }
                         }

                         if((temp_state!=SEVERE_FAULT)){  // Check for MINOR Faults

                             if((faultNumber & (1U << HV_LV_FAULT))){

                                 if(VCUDataPtr->DigitalVal.HV_VOLTAGE_OUT_OF_RANGE_FAULT){
                                     HV_VOLTAGE_TIMER_EXPIRED = false;
                                     if(xTimerStart(xTimers[3], 0)!=pdPASS){
                                         for(;;);
                                     }else{
                                         timer1_started = 1;
                                         temp_state= MINOR_FAULT;
                                     }

                                 }else if(VCUDataPtr->DigitalVal.HV_CURRENT_OUT_OF_RANGE){
                                     HV_CURRENT_TIMER_EXPIRED = false;
                                     if(xTimerStart(xTimers[2],0)!=pdPASS){
                                         for(;;);
                                     }else{
                                         timer2_started=1;
                                         temp_state=MINOR_FAULT;
                                     }
                                 }else if(VCUDataPtr->DigitalVal.LV_CURRENT_OUT_OF_RANGE){
                                     temp_state = MINOR_FAULT;

                                 }else if(VCUDataPtr->DigitalVal.LV_VOLTAGE_OUT_OF_RANGE){
                                     temp_state = MINOR_FAULT;
                                 }else if(VCUDataPtr->DigitalVal.BSE_APPS_MINOR_SIMULTANEOUS_FAULT){
                                         temp_state = MINOR_FAULT;
                                 }
                             }else if ((faultNumber & (1U<<CAN_FAULT))){

                                 if(VCUDataPtr->DigitalVal.CAN_ERROR_TYPE2){
                                     temp_state = MINOR_FAULT;
                                 }
                             }
                        }

            }

            if(faultNumber==0){
                if(isRTDS()){
                    state = temp_state;
                }else{
                    state = TRACTIVE_ON;
                }
            }else{
                state = temp_state;
            }

            /* -- New Code - Added by jjkhan */
        }else if (state == MINOR_FAULT){

            if (STATE_PRINT) {UARTSend(PC_UART, "********MINOR_FAULT********");}


            /* ++ New Code - Added by jjkhan */
            temp_state = state;
            // Check if faults have been cleared -> Could run the same fault checking scenario above.
            if(anyFaults()){
                faultNumber = faultLocation();

                // In MINOR FAULT state, only need to worry about MINOR FAULTS.

                if((faultNumber & (1U << HV_LV_FAULT))){

                    //UARTSend(PC_UART, "Inside Minor Fault state. \r\n");
                     // Check for SEVERE Faults Right now
                     if(timer1_started || timer2_started){
                         //UARTSend(PC_UART, "Inside Minor Timer started. \r\n");

                         if(VCUDataPtr->DigitalVal.HV_VOLTAGE_OUT_OF_RANGE_FAULT){
                             if(HV_VOLTAGE_TIMER_EXPIRED){
                                 temp_state = SEVERE_FAULT;
                                 timer2_started = 0; // Reset Timer
                                 HV_VOLTAGE_TIMER_EXPIRED=false;
                             }else{
                                 temp_state = MINOR_FAULT;
                             }
                         }else if(VCUDataPtr->DigitalVal.HV_CURRENT_OUT_OF_RANGE){

                             if(HV_CURRENT_TIMER_EXPIRED){
                                 //UARTSend(PC_UART, "Timeout. \r\n");
                                 temp_state = SEVERE_FAULT;
                                 timer1_started = 0; // Reset Timer
                                 HV_CURRENT_TIMER_EXPIRED = false;

                             }else{
                                 temp_state = MINOR_FAULT;
                             }

                         }

                     }else if(VCUDataPtr->DigitalVal.LV_CURRENT_OUT_OF_RANGE){
                         temp_state = MINOR_FAULT;

                     }else if(VCUDataPtr->DigitalVal.LV_VOLTAGE_OUT_OF_RANGE){
                         temp_state = MINOR_FAULT;
                     }else if(VCUDataPtr->DigitalVal.BSE_APPS_MINOR_SIMULTANEOUS_FAULT){
                             temp_state = MINOR_FAULT;
                     }
                 }else if ((faultNumber & (1U<<CAN_FAULT))){

                     if(VCUDataPtr->DigitalVal.CAN_ERROR_TYPE2){
                         temp_state = MINOR_FAULT;
                     }
                 }
           }else{
               faultNumber = 0;
           }

           // Checked for all Faults above and now if no MINOR_FAULTS were found, then we can move back to TRACTIVE_ON if RTDS is on and TSAL is ON, else we go to TRACTIVE_OFF; SEVERE_FAULTS wouldn't reach this far because taskYIELDs whenever you find a SEVERE_FAULT

            if(!anyFaults()){

                    if(isRTDS() && isTSAL_ON()){  // RTDS =1 and HV present
                        state = RUNNING;

                    }else if(!isRTDS() && isTSAL_ON()){ // RTDS = 0 but HV present
                        state = TRACTIVE_ON;
                    }else{  // RTDS = 0 and HV not present
                        state = TRACTIVE_OFF;
                    }
                    // Reset the HV timer_started flags as the fault was fixed before timeout
                    if(timer1_started || timer2_started){
                        timer1_started = 0;
                        timer2_started = 0;
                    }

            }else{
                state = temp_state;
            }

            /* -- New Code - Added by jjkhan */
        }else if(state==SEVERE_FAULT){

            /* ++ New Code - Added by jjkhan */
            if (STATE_PRINT) {UARTSend(PC_UART, "********SEVERE_FAULT********");}

            if(anyFaults()){
                state = SEVERE_FAULT;
            }else{
                state = TRACTIVE_OFF;
            }

            /* -- New Code - Added by jjkhan */
        }

            if (STATE_PRINT) {UARTSend(PC_UART, "\r\n");}

            // for timing:
            //gioSetBit(hetPORT1, 9, 0);
        }else{
            vTaskDelayUntil(&xLastWakeTime, STATE_MACHINE_TASK_PERIOD_MS); // A delay of 0.1 seconds -  based on line 66 statement
        }
    }
}
