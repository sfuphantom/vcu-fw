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

#include "vcu_data.h" // data structure to hold VCU data
#include "task_statemachine.h"

uint32_t blue_duty = 100;
uint32_t blue_flag = 0;


/*********************************************************************************
 *                               SYSTEM STATE FLAGS
 *********************************************************************************/
extern data* VCUDataPtr;


/* ++ Added by jjkhan */



static int checkSDC(void){
        if( VCUDataPtr->DigitalVal.IMD_FAULT
                || VCUDataPtr->DigitalVal.BSPD_FAULT
                || VCUDataPtr->DigitalVal.BMS_GPIO_FAULT
                      ){
            return FAULT;
        }
            return NOFAULT;

}

static int checkIMD(void){
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

static int checkBSE_APPS(void){
        if(VCUDataPtr->DigitalVal.BSE_SEVERE_RANGE_FAULT
                           || VCUDataPtr->DigitalVal.APPS1_SEVERE_RANGE_FAULT
                           || VCUDataPtr->DigitalVal.APPS2_SEVERE_RANGE_FAULT
                           || VCUDataPtr->DigitalVal.APPS_SEVERE_10DIFF_FAULT
                           || VCUDataPtr->DigitalVal.BSE_APPS_MINOR_SIMULTANEOUS_FAULT){
            return FAULT;
        }
            return NOFAULT;
}

static int CheckHVLVSensor(void){
        if(VCUDataPtr->DigitalVal.HV_CURRENT_OUT_OF_RANGE
                || VCUDataPtr->DigitalVal.HV_VOLTAGE_OUT_OF_RANGE_FAULT
                || VCUDataPtr->DigitalVal.LV_CURRENT_OUT_OF_RANGE
                || VCUDataPtr->DigitalVal.LV_VOLTAGE_OUT_OF_RANGE
                || VCUDataPtr->DigitalVal.APPS_PROPORTION_ERROR){
             return FAULT;
        }
            return NOFAULT;
}

static int checkCAN(void){
    if(VCUDataPtr->DigitalVal.CAN_ERROR_TYPE1 || VCUDataPtr->DigitalVal.CAN_ERROR_TYPE2){
        return FAULT;
    }
    return NOFAULT;
}

static int isRTDS(void){
    if(VCUDataPtr->DigitalVal.RTDS){
        return SET;
    }
    return !SET;
}

static int isTSAL_ON(void){
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


static uint32_t faultLocation(void){

    int systemFaultIndicator = 0;

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

static int anyFaults(void){
    if(checkSDC() || checkCAN() || checkIMD() || checkBSE_APPS()  || CheckHVLVSensor()){
        return FAULT;
    }
    return NOFAULT;
}

static State getNewState(State currentState, uint32_t faultNumber, uint8_t* timer1_started, TimerHandle_t* timer1, uint8_t* timer2_started, TimerHandle_t* timer2){

    static State state;

    /*
     * The following Faults are common to TRACTIVE_ON, RUNNING and MINOR_FAULT States + they have same state change -> i.e. change state to SEVERE_FAULT.
     *
     * Therefore, placing it at the start of this function, this way if the fault is severe, we can skip checking the rest of the systems, saving time.
     *
     */

    // // Shutdown Circuit Fault or IMD Fault Checks - its Severe so don't need to check other faults
    if((faultNumber && (1<<SDC_FAULT)) || (faultNumber && (1 << IMD_SYSTEM_FAULT))){
        return SEVERE_FAULT;
    }


    // Check BSE APPS Faults
    if(faultNumber && (1<<BSE_APPS_FAULT)){ // Either BSE or APPS Fault
        if(!VCUDataPtr->DigitalVal.BSE_APPS_MINOR_SIMULTANEOUS_FAULT){ // if the fault isn't the Minor Fault, then set SEVERE_FAULT and yield, don't need to check for minor
            return SEVERE_FAULT;
        }else{
            state = MINOR_FAULT; // Minor Fault, so we should still look at the
        }
    }

    // Check CAN Faults
    if(faultNumber && (1<<CAN_FAULT)){ // Either CAN Message indicates a severe fault (TYPE1 ERROR) or a minor fault (TYPE2 ERROR)

        if(VCUDataPtr->DigitalVal.CAN_ERROR_TYPE1){ // Severe Fault message from CAN, so don't need to check other faults, change state and yield task
            return SEVERE_FAULT;
        }else{
            state = MINOR_FAULT; // Minor fault, we should still look at the
        }
    }


    // Check HV and LV
    if(faultNumber && (1 << HV_LV_FAULT)){

        /* HV Voltage out of Safe Range:
        *      1. Need a timer to go on and this timer will be stopped when this fault is handled.
        *      2. If Timer was started last time and fault is still HV Voltage out of safe range -> SEVERE_FAULT
        *      3. If Fault is HV Voltage out of safe range, then start timer and and state = MINOR_FAULT
        */

       if(VCUDataPtr->DigitalVal.HV_VOLTAGE_OUT_OF_RANGE_FAULT && (*timer2_started)){
           if(HV_VOLTAGE_TIMER_EXPIRED){ // if timer2_value>set_threshold, switch to SEVERE_FAULT
               return SEVERE_FAULT;
           }else{
               state = MINOR_FAULT;
           }
       }else if(VCUDataPtr->DigitalVal.HV_VOLTAGE_OUT_OF_RANGE_FAULT){
            if(xTimerStart(*timer2, pdMS_TO_TICKS(10))){
                *timer2_started = 1; // Start timer
                state = MINOR_FAULT;
            }

       }

       /*
        * LV Voltage or Current Out of Range
        */

       if(VCUDataPtr->DigitalVal.LV_CURRENT_OUT_OF_RANGE || VCUDataPtr->DigitalVal.LV_VOLTAGE_OUT_OF_RANGE){
           state = MINOR_FAULT;   // Give a warning
       }


       // Pedal position not corresponding to the amount of current drawn from battery

        if(VCUDataPtr->DigitalVal.BSE_APPS_MINOR_SIMULTANEOUS_FAULT){

              if((currentState == TRACTIVE_ON) && isRTDS() && (state!=MINOR_FAULT)  ){ // Check if RTDS is set and previous fault checks didn't give MINOR_FAULT
                          state = TRACTIVE_ON;
              }else{
                  state = MINOR_FAULT; // previous fault checks have a MINOR_FAULT, can't ignore it
              }
         }


        // Check HV


        /*
         * For CurrentState = TRACTIVE_ON or MINOR_FAULT - start a timer if this is first time the HV current is sensed to be outside the safe range, else check if the timer value is greater than predefined threshold
         *
         *  HV Current out of Safe Range:
         *      1. Need a timer to go on and this timer will be stopped when this fault is handled.
         *      2. If Timer was started last time and fault is still HV Current out of safe range -> SEVERE_FAULT
         *      3. If Fault is HV Current out of safe range, then start timer and and state = MINOR_FAULT
         */

        if(VCUDataPtr->DigitalVal.HV_CURRENT_OUT_OF_RANGE){

            if(currentState==RUNNING){
                return SEVERE_FAULT;  // In RUNNING State, if HV current out of safe range, state changes to SEVERE_FAULT, no need for timers
            }else{

                if(*timer1_started && HV_CURRENT_TIMER_EXPIRED){    // check if this first time HV current sensed out of safe range, if true then check if timer1>timer_threshold
                    return SEVERE_FAULT;

                }else{ // First time HV current out of safe range, start timer

                    if(xTimerStart(*timer1, pdMS_TO_TICKS(10))){
                        *timer1_started = 1; // Start timer
                        state = MINOR_FAULT;
                    }

                }
            }

        }

    }

    return state;  // At this point you will return MINOR_FAULT

}

/* -- Added by jjkhan */


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
    const TickType_t xFrequency = 100; // task frequency in ms

    // Initialize the xLastWakeTime variable with the current time;
    xLastWakeTime = xTaskGetTickCount();



    /* ++ Added to simulate Timer for HV Current and Voltage Fault, will implement proper timer later - jjkhan */


    // ++ For HV Current
    uint8_t timer1_started = 0;
    TimerHandle_t timer1_value = xTimers[2]; // HV Current Out of Range Timer
    // -- For HV Current


    // ++ For HV Voltage
    uint8_t timer2_started = 0;
    TimerHandle_t timer2_value = xTimers[3]; // HV Voltage Out of Range Timer
    // -- For HV Voltage


    /* -- Added to simulate Timer for HV Current and Voltage Fault, will implement proper timer later - jjkhan */

    while(true)
    {


        // for timing:
        gioSetBit(hetPORT1, 9, 1);

        if (TASK_PRINT) {UARTSend(PC_UART, "STATE MACHINE UPDATE TASK\r\n");}

/*********************** STATE MACHINE EVALUATION ***********************************/


        if (state == TRACTIVE_OFF){
            /* ++ New Code: Added by jjkhan */


           if (STATE_PRINT) {UARTSend(PC_UART, "********TRACTIVE_OFF********");}

           if(isRTDS() || anyFaults()){ state = SEVERE_FAULT;}  // RTDS should be off when state is TRACTIVE_OFF && you all errors should be taken care before you proceed.
           if(isTSAL_ON() && !anyFaults() && !isRTDS()){ state = TRACTIVE_ON; } //No faults AND TSAL is on AND RTDS not set


           /* -- New Code: Added by jjkhan */
        }else if (state == TRACTIVE_ON){

            if (STATE_PRINT) {UARTSend(PC_UART, "********TRACTIVE_ON********");}

            /* ++ New Code - Added by jjkhan */
            if(!isTSAL_ON()){ state = SEVERE_FAULT;} // TSAL light is OFF - shouldn't happen, but if it does, handle it.
            if(anyFaults()){
               uint32_t faultNumber = faultLocation();
               state = getNewState(state,faultNumber,&timer1_started, &timer1_value, &timer2_started, &timer2_value);
            }
           // Checked for all Faults above and now if no MINOR_FAULTS were found, then we can move to RUNNING; SEVERE_FAULTS wouldn't reach this far because taskYIELDs whenever you find a SEVERE_FAULT
           if(isRTDS() && (state!=MINOR_FAULT) ){
               state = RUNNING;
           }

           /* -- New Code - Added by jjkhan */


        }else if (state == RUNNING){

            if (STATE_PRINT) {UARTSend(PC_UART, "********RUNNING********");}


            /* ++ New Code - Added by jjkhan */
            if(isRTDS() && !anyFaults()){ state = RUNNING;}  // Ready To Drive is Set and there are no Faults, state doesn't change, yieldTask to skip all steps below

            // Find fault in the system
            if(anyFaults()){
                       uint32_t faultNumber = faultLocation();
                       state = getNewState(state,faultNumber,&timer1_started, &timer1_value, &timer2_started, &timer2_value);
              }
            // Checked for all Faults above and now its either: 1) there were MINOR_FAULTS detected or  2) RTDS was flipped to off; SEVERE_FAULTS wouldn't reach this far because taskYIELDs whenever you find a SEVERE_FAULT

            if(isRTDS() && state==MINOR_FAULT){
                state = MINOR_FAULT;
            }else if(!isRTDS()){
                state=TRACTIVE_ON; // Ready To Drive Is Not Set and there were no MINOR_FAULTS, go back to TRACTIVE_ON
            }


            /* -- New Code - Added by jjkhan */
        }else if (state == MINOR_FAULT){

            if (STATE_PRINT) {UARTSend(PC_UART, "********MINOR_FAULT********");}


            /* ++ New Code - Added by jjkhan */

            // Check if faults have been cleared -> Could run the same fault checking scenario above.
            if(anyFaults()){
                uint32_t faultNumber = faultLocation();
                state = getNewState(state,faultNumber,&timer1_started, &timer1_value, &timer2_started, &timer2_value);
            }

           // Checked for all Faults above and now if no MINOR_FAULTS were found, then we can move back to TRACTIVE_ON if RTDS is on and TSAL is ON, else we go to TRACTIVE_OFF; SEVERE_FAULTS wouldn't reach this far because taskYIELDs whenever you find a SEVERE_FAULT

            if(state!=MINOR_FAULT){
                if(isRTDS() && isTSAL_ON()){  // RTDS =1 and HV present
                    state = RUNNING;
                }else if(!isRTDS() && isTSAL_ON()){ // RTDS = 0 but HV present
                    state = TRACTIVE_ON;
                }else{  // RTDS = 0 and HV not present
                    state = TRACTIVE_OFF;
                }

            }else{ // We could check if RTDS is still ON here despite being in MINOR_FAULT??
                state = MINOR_FAULT;
            }

            /* -- New Code - Added by jjkhan */
        }else if(state==SEVERE_FAULT){

            /* ++ New Code - Added by jjkhan */
            if (STATE_PRINT) {UARTSend(PC_UART, "********SEVERE_FAULT********");}

            if(!isRTDS() && !anyFaults()){ // Move to Tractive off iff no faults and RTD signal not set.
                state = TRACTIVE_OFF; // All faults cleared. Move to starting state
            }

            /* -- New Code - Added by jjkhan */
        }

        if (STATE_PRINT) {UARTSend(PC_UART, "\r\n");}

        // for timing:
        gioSetBit(hetPORT1, 9, 0);

        // Wait for the next cycle
         vTaskDelayUntil(&xLastWakeTime, xFrequency);

    }
}
