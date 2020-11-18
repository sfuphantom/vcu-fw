/*
 * task_throttle.c
 *
 *  Created on: Mar 31, 2020
 *      Author: gabriel
 */

#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "gio.h"
#include "het.h"
#include "os_queue.h"
#include "os_semphr.h"
#include "os_task.h"
#include "os_timer.h"

#include "board_hardware.h"   // contains hardware defines for specific board used (i.e. VCU or launchpad)

#include "priorities.h"
#include "Phantom_sci.h"
#include "FreeRTOS.h"

#include "vcu_data.h" // data structure to hold VCU data
#include "task_statemachine.h"

uint32_t blue_duty = 100;
uint32_t blue_flag = 0;

extern State state;

/*********************************************************************************
 *                               SYSTEM STATE FLAGS
 *********************************************************************************/
//extern uint8_t TSAL;// = 0;
//extern uint8_t RTDS;// = 0;
//extern long RTDS_RAW;// = 0;
//extern uint8_t BMS;//  = 1;
//extern uint8_t IMD;//  = 1;
//extern uint8_t BSPD;// = 1;
//extern uint8_t BSE_FAULT;// = 0;
extern data* VCUDataPtr;


/* ++ Added by jjkhan */

/***********************************************************
 * @function                - anyERRORS
 *
 * @brief                   - This task is a helper function for vStateMachineTask, its used to check if there are any faults, regardless if severe or minor
 *
 * @param[in]               - void
 *
 * @return                  - 1 or 0
 * @Note                    -  A '1' indicates there fault(s), a '0' indicates no fault
 *
 ***********************************************************/

static int anyFaults(void){

    if(VCUDataPtr->DigitalVal.BMS_GPIO_FAULT
                   || VCUDataPtr->DigitalVal.BSPD_FAULT
                   || VCUDataPtr->DigitalVal.BSE_SEVERE_RANGE_FAULT
                   || VCUDataPtr->DigitalVal.APPS1_SEVERE_RANGE_FAULT
                   || VCUDataPtr->DigitalVal.APPS2_SEVERE_RANGE_FAULT
                   || VCUDataPtr->DigitalVal.APPS_SEVERE_10DIFF_FAULT
                   || VCUDataPtr->DigitalVal.BSE_APPS_MINOR_SIMULTANEOUS_FAULT
                   || VCUDataPtr->DigitalVal.HV_CURRENT_OUT_OF_RANGE
                   || VCUDataPtr->DigitalVal.APPS_PROPORTION_ERROR
                   || VCUDataPtr->DigitalVal.HV_VOLTAGE_OUT_OF_RANGE_FAULT
                   || VCUDataPtr->DigitalVal.LV_CURRENT_OUT_OF_RANGE
                   || VCUDataPtr->DigitalVal.LV_VOLTAGE_OUT_OF_RANGE
                   || VCUDataPtr->DigitalVal.CAN_ERROR_TYPE1
                   || VCUDataPtr->DigitalVal.CAN_ERROR_TYPE2
                   || VCUDataPtr->DigitalVal.IMD_LOW_ISO_FAULT
                   || VCUDataPtr->DigitalVal.IMD_SHORT_CIRCUIT_FAULT
                   || VCUDataPtr->DigitalVal.IMD_DEVICE_ERR_FAULT
                   || VCUDataPtr->DigitalVal.IMD_BAD_INFO_FAULT
                   || VCUDataPtr->DigitalVal.IMD_UNDEF_ERR
                   || VCUDataPtr->DigitalVal.IMD_GARBAGE_DATA_FAULT
                   || VCUDataPtr->DigitalVal.TSAL_WELDED_AIRS_FAULT){
        return FAULT;
    }
    return NOFAULT;

}

static int checkSDC(void){
        if( VCUDataPtr->DigitalVal.BSPD_FAULT
                      || VCUDataPtr->DigitalVal.BMS_GPIO_FAULT
                      || VCUDataPtr->DigitalVal.IMD_LOW_ISO_FAULT
                      || VCUDataPtr->DigitalVal.IMD_SHORT_CIRCUIT_FAULT
                      || VCUDataPtr->DigitalVal.IMD_DEVICE_ERR_FAULT
                      || VCUDataPtr->DigitalVal.IMD_BAD_INFO_FAULT
                      || VCUDataPtr->DigitalVal.IMD_UNDEF_ERR
                      || VCUDataPtr->DigitalVal.IMD_GARBAGE_DATA_FAULT){

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


static uint32_t faultLocation(void){

    int systemFaultIndicator = 0;
    if(checkSDC()){
        systemFaultIndicator |= 1 << SDC_FAULT;
    }

    if(checkBSE_APPS()){
        systemFaultIndicator |= 1 << BSE_APPS_FAULT;
    }
    if(CheckHVLVSensor()){
        systemFaultIndicator |= 1 << HV_LV_FAULT;
    }
    if(checkCAN()){
        systemFaultIndicator |= 1 << CAN_FAULT;
    }
    return systemFaultIndicator;



}
static void stateLEDs(uint32_t blueLEDDutyCylce, uint32_t greenLEDDutyCycle, uint32_t redLEDDutyCycle){
     /* Write the PWM code here. */


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
    volatile uint8_t timer1_started = 0;
    volatile uint8_t timer1_value;
    volatile uint8_t timer2_started = 0;
    volatile uint8_t timer2_value;
    volatile uint8_t set_threshold;
    /* -- Added to simulate Timer for HV Current and Voltage Fault, will implement proper timer later - jjkhan */

    while(true)
    {
        // Wait for the next cycle
        vTaskDelayUntil(&xLastWakeTime, xFrequency);

        // for timing:
        gioSetBit(hetPORT1, 9, 1);

        if (TASK_PRINT) {UARTSend(PC_UART, "STATE MACHINE UPDATE TASK\r\n");}

//        UARTSend(scilinREG, (char *)xLastWakeTime);

//        xStatus = xQueueReceive(xq, &lrval, 30);
//        nchars = ltoa(lrval, stbuf);
//        if (TASK_PRINT) {UARTSend(scilinREG, (char *)stbuf);}
//        if (TASK_PRINT) {UARTSend(scilinREG, "\r\n");}


        // MAKE SOME LED BLINK ON THE VCU! TECHNICALLY U HAVE 6 DIFFERENT ONES U CAN BLINK
        // MAKE EACH TASK BLINK A DIFFERENT LED

/*********************** STATE MACHINE EVALUATION ***********************************/


        if (state == TRACTIVE_OFF)
        {
            /* ++ New Code: Added by jjkhan */

           if (STATE_PRINT) {UARTSend(PC_UART, "********TRACTIVE_OFF********");}

           // Note: taskYIELD() gives up CPU time to a Ready Task of equal priority.
           if(isRTDS() || anyFaults()){ state = SEVERE_FAULT; taskYIELD(); }  // RTDS should be off when state is TRACTIVE_OFF && you all errors should be taken care before you proceed.
           if(!isTSAL_ON()){ state = TRACTIVE_OFF; taskYIELD(); }  // This will prevent from executing all the steps below - suspend task here?
           state = TRACTIVE_ON; // No faults and TSAL is on,

           /* -- New Code: Added by jjkhan */

            /* ++  Old code: Commented out by jjkhan
             *
//          pwmSetDuty(RGB_LED_PORT, BLUE_LED, 50U); // blue LED
            pwmSetDuty(RGB_LED_PORT, GREEN_LED, 100U); // green LED
            pwmSetDuty(RGB_LED_PORT, RED_LED, 100U); // red LED

            hetSIGNAL_t dutycycle_and_period;
            dutycycle_and_period.duty = blue_duty;
            dutycycle_and_period.period = 1000;
//            = {(unsigned int)1, (double)100}; // duty cycle in %, period in us

            pwmSetSignal(RGB_LED_PORT, BLUE_LED, dutycycle_and_period);

            if (blue_duty <= 0)
            {
                blue_flag = 1; // 1 means rising
            }
            else if (blue_duty >= 100)
            {
                blue_flag = 0; // 0 means falling
            }

            if (blue_flag == 1)
            {
                blue_duty+= 5;
            }
            else
            {
                blue_duty-= 5;
            }

            */


        }else if (state == TRACTIVE_ON){
                        /* ++  OLD CODE: Commented out by jjkhan
                        pwmSetDuty(RGB_LED_PORT, GREEN_LED, 100U);
                        pwmSetDuty(RGB_LED_PORT, RED_LED, 100U);
                        pwmSetDuty(RGB_LED_PORT, BLUE_LED, 50U); // blue
                        --  OLD CODE: Commented out by jjkhan  */

            /* ++ New Code - Added by jjkhan */
            if(!isTSAL_ON()){ state = SEVERE_FAULT; taskYIELD();} // TSAL light is OFF - shouldn't happen, but if it does, handle it.
            if(anyFaults()){

                uint32_t faultNumber = faultLocation();

                /* ++ Will move all of this inside function faultLocation, after unit-testing -> Each state can call this function, parameters - "State faultLocation(currentState,timer1_started, timer1, timer2_started, timer2)" */
                // Check SDC Faults
                if(faultNumber && (1<<SDC_FAULT)){ // Shutdown Circuit Fault - its Severe so don't need to check other faults
                    state = SEVERE_FAULT;
                    taskYIELD();
                }
                // Check BSE APPS Fauls
                if(faultNumber && (1<<BSE_APPS_FAULT)){ // Eithr BSE or APPS Fault
                    if(!VCUDataPtr->DigitalVal.BSE_APPS_MINOR_SIMULTANEOUS_FAULT){ // if the fault isn't the Minor Fault, then set SEVERE_FAULT and yield, don't need to check for minor
                        state = SEVERE_FAULT;
                        taskYIELD();
                    }else{
                        state = MINOR_FAULT; // Minor Fault, so we should still look at the
                    }

                }

                // Check HV and LV Sensing
                if(faultNumber && (1<<HV_LV_FAULT) ){ // Either LV or HV Sensing fault

                    /*
                     * HV Current out of Safe Range:
                     *      1. Need a timer to go on and this timer will be stopped when this fault is handled.
                     *      2. If Timer was started last time and fault is still HV Current out of safe range -> SEVERE_FAULT
                     *      3. If Fault is HV Current out of safe range, then start timer and and state = MINOR_FAULT
                     */
                    if(VCUDataPtr->DigitalVal.HV_CURRENT_OUT_OF_RANGE && (timer1_started)){
                        if(timer1_value>set_threshold){ // timer1_value>set_threshold, switch to SEVERE_FAULT
                            state = SEVERE_FAULT;
                            timer1_started = 0;  //  should reset timer here?
                            taskYIELD(); // This is a severe Fault, don't need to check the others, yieldTask,
                        }else{
                            state = MINOR_FAULT;
                        }
                    }else if(VCUDataPtr->DigitalVal.HV_CURRENT_OUT_OF_RANGE){ // First incident of HV current being out of range
                         timer1_started = 1; // Start timer
                         state = MINOR_FAULT;
                    }

                    /* HV Voltage out of Safe Range:
                     *      1. Need a timer to go on and this timer will be stopped when this fault is handled.
                     *      2. If Timer was started last time and fault is still HV Voltage out of safe range -> SEVERE_FAULT
                     *      3. If Fault is HV Voltage out of safe range, then start timer and and state = MINOR_FAULT
                     */

                    if(VCUDataPtr->DigitalVal.HV_VOLTAGE_OUT_OF_RANGE_FAULT && (timer2_started)){
                        if(timer2_value>set_threshold){ // if timer2_value>set_threshold, switch to SEVERE_FAULT
                            state = SEVERE_FAULT;
                            timer2_started = 0; //should reset timer here?
                            taskYIELD(); // This is a severe Fault, don't need to check the others, yieldTask
                        }else{
                            state = MINOR_FAULT;
                        }
                    }else if(VCUDataPtr->DigitalVal.HV_VOLTAGE_OUT_OF_RANGE_FAULT){
                         timer2_started = 1; // Start timer
                         state = MINOR_FAULT;
                    }


                    /*
                     * LV Voltage or Current Out of Range
                     */

                    if(VCUDataPtr->DigitalVal.LV_CURRENT_OUT_OF_RANGE || VCUDataPtr->DigitalVal.LV_VOLTAGE_OUT_OF_RANGE){
                        state = MINOR_FAULT;   // Give a warning
                    }

                    /*
                     * Pedal position not corresponding to the amount of current drawn from battery
                     */

                    if(VCUDataPtr->DigitalVal.BSE_APPS_MINOR_SIMULTANEOUS_FAULT){

                        if(isRTDS() && (state!=MINOR_FAULT)){ // Check if RTDS is set and previous fault checks didn't give MINOR_FAULT
                            state = TRACTIVE_ON;
                        }else{
                            state = MINOR_FAULT; // previous fault checks have a MINOR_FAULT, can't ignore it
                        }
                    }

                }
                 // Check CAN Faults
                if(faultNumber && (1<<CAN_FAULT)){ // Either CAN Message indicates a severe fault (TYPE1 ERROR) or a minor fault (TYPE2 ERROR)

                    if(VCUDataPtr->DigitalVal.CAN_ERROR_TYPE1){ // Severe Fault message from CAN, so don't need to check other faults, change state and yield task
                        state = SEVERE_FAULT;
                        taskYIELD();
                    }else{
                        state = MINOR_FAULT; // Minor fault, we should still look at the
                    }

                }
           }
           /* -- Will move all of this inside function faultLocation, after unit-testing. "State faultLocation(currentState,timer1_started, timer1, timer2_started, timer2)"  */

           // Checked for all Faults above and now if no MINOR_FAULTS were found, then we can move to RUNNING; SEVERE_FAULTS wouldn't reach this far because taskYIELDs whenever you find a SEVERE_FAULT
           if(isRTDS() && (state!=MINOR_FAULT) ){
               state = RUNNING;
           }

           /* -- New Code - Added by jjkhan */


        }else if (state == RUNNING){
                                   /* ++  OLD CODE: Commented out by jjkhan
                                   pwmSetDuty(RGB_LED_PORT, GREEN_LED, 100U);
                                   pwmSetDuty(RGB_LED_PORT, RED_LED, 100U);
                                   pwmSetDuty(RGB_LED_PORT, BLUE_LED, 50U); // blue
                                   --  OLD CODE: Commented out by jjkhan  */

            if (STATE_PRINT) {UARTSend(PC_UART, "********RUNNING********");}

            while(isRTDS() && !anyFaults()){ state = RUNNING; taskYIELD();}  // Ready To Drive is Set and there are no Faults, state doesn't change, yieldTask to skip all steps below

            // Find fault in the system
            if(anyFaults()){

                       uint32_t faultNumber = faultLocation();

                       /* ++ Will move all of this inside function faultLocation, after unit-testing -> Each state can call this function, parameters - "state faultLocation(currentState,timer1_started, timer1, timer2_started, timer2)" */
                       // Check SDC Faults
                       if(faultNumber && (1<<SDC_FAULT)){ // Shutdown Circuit Fault - its Severe so don't need to check other faults
                           state = SEVERE_FAULT;
                           taskYIELD();
                       }
                       // Check BSE APPS Fauls
                       if(faultNumber && (1<<BSE_APPS_FAULT)){ // Eithr BSE or APPS Fault
                           if(!VCUDataPtr->DigitalVal.BSE_APPS_MINOR_SIMULTANEOUS_FAULT){ // if the fault isn't this Minor Fault, then set SEVERE_FAULT and yield, don't need to check for other faults below because SEVERE_FAULT Detected
                               state = SEVERE_FAULT;
                               taskYIELD();
                           }else{
                               state = MINOR_FAULT; // Minor Fault, so we should still look at the
                           }

                       }

                       // Check HV and LV Sensing
                       if(faultNumber && (1<<HV_LV_FAULT) ){ // Either LV or HV Sensing fault

                           /*
                            * HV Current out of Safe Range: In RUNNING State, if HV current out of safe range, state changes to SEVERE_FAULT, no need for timers
                            *
                            */
                           if(VCUDataPtr->DigitalVal.HV_CURRENT_OUT_OF_RANGE && (timer1_started)){
                                   state = SEVERE_FAULT;
                                   taskYIELD(); // This is a severe Fault, don't need to check the others, yieldTask,

                           }

                           /* HV Voltage out of Safe Range:
                            *      1. Need a timer to go on and this timer will be stopped when this fault is handled.
                            *      2. If Timer was started last time and fault is still HV Voltage out of safe range -> SEVERE_FAULT
                            *      3. If Fault is HV Voltage out of safe range, then start timer and and state = MINOR_FAULT
                            */

                           if(VCUDataPtr->DigitalVal.HV_VOLTAGE_OUT_OF_RANGE_FAULT && (timer2_started)){
                               if(timer2_value>set_threshold){ // if timer2_value>set_threshold, switch to SEVERE_FAULT
                                   state = SEVERE_FAULT;
                                   timer2_started = 0; //should reset timer here?
                                   taskYIELD(); // This is a severe Fault, don't need to check the others, yieldTask
                               }else{
                                   state = MINOR_FAULT;
                               }
                           }else if(VCUDataPtr->DigitalVal.HV_VOLTAGE_OUT_OF_RANGE_FAULT){
                                timer2_started = 1; // Start timer
                                state = MINOR_FAULT;
                           }


                           /*
                            * LV Voltage or Current Out of Range
                            */

                           if(VCUDataPtr->DigitalVal.LV_CURRENT_OUT_OF_RANGE || VCUDataPtr->DigitalVal.LV_VOLTAGE_OUT_OF_RANGE){
                               state = MINOR_FAULT;   // Give a warning
                           }

                           /*
                            * Pedal position not corresponding to the amount of current drawn from battery
                            */

                           if(VCUDataPtr->DigitalVal.BSE_APPS_MINOR_SIMULTANEOUS_FAULT){
                                   state = MINOR_FAULT; // previous fault checks have a MINOR_FAULT, can't ignore it
                           }

                       }
                        // Check CAN Faults
                       if(faultNumber && (1<<CAN_FAULT)){ // Either CAN Message indicates a severe fault (TYPE1 ERROR) or a minor fault (TYPE2 ERROR)

                           if(VCUDataPtr->DigitalVal.CAN_ERROR_TYPE1){ // Severe Fault message from CAN, so don't need to check other faults, change state and yield task
                               state = SEVERE_FAULT;
                               taskYIELD();
                           }else{
                               state = MINOR_FAULT; // Minor fault, we should still look at the
                           }

                       }
              }
                /* -- Will move all of this inside function faultLocation, after unit-testing. "state faultLocation(currentState,timer1_started, timer1, timer2_started, timer2)"  */

            // Checked for all Faults above and now its either: 1) there were MINOR_FAULTS detected or  2) RTDS was flipped to off; SEVERE_FAULTS wouldn't reach this far because taskYIELDs whenever you find a SEVERE_FAULT

            if(isRTDS() && state==MINOR_FAULT){
                state = MINOR_FAULT;
            }else if(!isRTDS()){
                state=TRACTIVE_ON; // Ready To Drive Is Not Set and there were no MINOR_FAULTS, go back to TRACTIVE_ON
            }

        }else if (state == MINOR_FAULT){


                                              /* ++  OLD CODE: Commented out by jjkhan
                                              pwmSetDuty(RGB_LED_PORT, GREEN_LED, 100U);
                                              pwmSetDuty(RGB_LED_PORT, RED_LED, 100U);
                                              pwmSetDuty(RGB_LED_PORT, BLUE_LED, 50U); // blue
                                              --  OLD CODE: Commented out by jjkhan  */

            if (STATE_PRINT) {UARTSend(PC_UART, "********FAULT********");}

            // Check if faults have been cleared -> Could run the same fault checking scenario above.
            if(anyFaults()){

                /*
                 *  Initially change the state, now as we go through the process of checking every source of Fault, at the end if we find no fault (i.e. no MINOR_FAULT - because SEVERE_FAULT will yeild this task), then we can check if:
                                1) TSAL is on (should be- but just to be sure) AND 2) RTDS is still set -> if 1) and 2) are true then we go back to RUNNING, else go to TRACTIVE_ON if TSAL__ON and RTDS = 0, else TRACTIVE_OFF.
                 */
                state = TRACTIVE_ON;
                uint32_t faultNumber = faultLocation();

                /* ++ Will move all of this inside function faultLocation, after unit-testing -> Each state can call this function, parameters - "State faultLocation(currentState,timer1_started, timer1, timer2_started, timer2)" */
                // Check SDC Faults
                if(faultNumber && (1<<SDC_FAULT)){ // Shutdown Circuit Fault - its Severe so don't need to check other faults
                    state = SEVERE_FAULT;
                    taskYIELD();
                }
                // Check BSE APPS Fauls
                if(faultNumber && (1<<BSE_APPS_FAULT)){ // Eithr BSE or APPS Fault
                    if(!VCUDataPtr->DigitalVal.BSE_APPS_MINOR_SIMULTANEOUS_FAULT){ // if the fault isn't the Minor Fault, then set SEVERE_FAULT and yield, don't need to check for minor
                        state = SEVERE_FAULT;
                        taskYIELD();
                    }else if(VCUDataPtr->DigitalVal.BSE_APPS_MINOR_SIMULTANEOUS_FAULT){
                        state = MINOR_FAULT; // Minor Fault, so we should still look at the
                    }
                }

                // Check HV and LV Sensing
                if(faultNumber && (1<<HV_LV_FAULT) ){ // Either LV or HV Sensing fault

                    /*
                     * HV Current out of Safe Range:
                     *      1. Need a timer to go on and this timer will be stopped when this fault is handled.
                     *      2. If Timer was started last time and fault is still HV Current out of safe range -> SEVERE_FAULT
                     *      3. If Fault is HV Current out of safe range, then start timer and and state = MINOR_FAULT
                     */
                    if(VCUDataPtr->DigitalVal.HV_CURRENT_OUT_OF_RANGE && (timer1_started)){
                        if(timer1_value>set_threshold){ // timer1_value>set_threshold, switch to SEVERE_FAULT
                            state = SEVERE_FAULT;
                            timer1_started = 0;  //  should reset timer here?
                            taskYIELD(); // This is a severe Fault, don't need to check the others, yieldTask,
                        }else{
                            state = MINOR_FAULT;
                        }
                    }else if(VCUDataPtr->DigitalVal.HV_CURRENT_OUT_OF_RANGE){ // First incident of HV current being out of range
                         timer1_started = 1; // Start timer
                         state = MINOR_FAULT;
                    }

                    /* HV Voltage out of Safe Range:
                     *      1. Need a timer to go on and this timer will be stopped when this fault is handled.
                     *      2. If Timer was started last time and fault is still HV Voltage out of safe range -> SEVERE_FAULT
                     *      3. If Fault is HV Voltage out of safe range, then start timer and and state = MINOR_FAULT
                     */

                    if(VCUDataPtr->DigitalVal.HV_VOLTAGE_OUT_OF_RANGE_FAULT && (timer2_started)){
                        if(timer2_value>set_threshold){ // if timer2_value>set_threshold, switch to SEVERE_FAULT
                            state = SEVERE_FAULT;
                            timer2_started = 0; //should reset timer here?
                            taskYIELD(); // This is a severe Fault, don't need to check the others, yieldTask
                        }else{
                            state = MINOR_FAULT;
                        }
                    }else if(VCUDataPtr->DigitalVal.HV_VOLTAGE_OUT_OF_RANGE_FAULT){
                         timer2_started = 1; // Start timer
                         state = MINOR_FAULT;
                    }


                    /*
                     * LV Voltage or Current Out of Range
                     */

                    if(VCUDataPtr->DigitalVal.LV_CURRENT_OUT_OF_RANGE || VCUDataPtr->DigitalVal.LV_VOLTAGE_OUT_OF_RANGE){
                        state = MINOR_FAULT;   // Give a warning
                    }

                    /*
                     * Pedal position not corresponding to the amount of current drawn from battery
                     */

                     if(VCUDataPtr->DigitalVal.BSE_APPS_MINOR_SIMULTANEOUS_FAULT){
                         state = MINOR_FAULT; // previous fault checks have a MINOR_FAULT, can't ignore it
                     }


                }
                 // Check CAN Faults
                if(faultNumber && (1<<CAN_FAULT)){ // Either CAN Message indicates a severe fault (TYPE1 ERROR) or a minor fault (TYPE2 ERROR)

                    if(VCUDataPtr->DigitalVal.CAN_ERROR_TYPE1){ // Severe Fault message from CAN, so don't need to check other faults, change state and yield task
                        state = SEVERE_FAULT;
                        taskYIELD();
                    }else{
                        state = MINOR_FAULT; // Minor fault, we should still look at the
                    }

                }
           }
           /* -- Will move all of this inside function faultLocation, after unit-testing. "State faultLocation(currentState,timer1_started, timer1, timer2_started, timer2)"  */

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

        }else if(state==SEVERE_FAULT){

            if(anyFaults()){
                taskYIELD(); // Severe Faults haven't been cleared.
            }else{
                state = TRACTIVE_OFF; // All faults cleared. Move to starting state
            }
        }

        if (STATE_PRINT) {UARTSend(PC_UART, "\r\n");}

        // for timing:
        gioSetBit(hetPORT1, 9, 0);
    }
}
