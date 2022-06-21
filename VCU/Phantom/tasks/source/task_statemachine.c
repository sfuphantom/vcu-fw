/*
 *  Task Template made by Joshua Guo
 * 
 *  Created on:
 *      Author: gabriel, jjkhan, joshua guo 
 */
#include "phantom_task.h"
#include "phantom_timer.h"   // if you need to use timers

// #include your_task_header_file
// Any other .h files you need goes here...
#include "task_config.h"
#include "board_hardware.h"   // contains hardware defines for specific board used (i.e. VCU or launchpad)
#include "vcu_data.h"
#include "RGB_LED.h"

#include "task_statemachine.h"

static Task task;
static TaskHandle_t taskHandle; 

// Any other module-scope variables goes here... (make sure they have the 'static' keyword)
static TimerHandle_t HV_CurrentTimerHandle;
static TimerHandle_t HV_VoltageTimerHandle;
static bool HV_VOLTAGE_TIMER_EXPIRED;
static bool HV_CURRENT_TIMER_EXPIRED;

// Pre-define your static functions here...
static void vStateMachineTask(void* arg);
static bool isSevereFault(Fault faults, State currentState);
static bool isMinorFault(Fault faults);
static void HV_CurrentTimerCallback(TimerHandle_t timer);
static void HV_VoltageTimerCallback(TimerHandle_t timer);


// BTW this function should be the only thing in your header file (aside from include guards and other comments ofc)
void Task_StateMachineInit(void)
{
    task = (Task) {vStateMachineTask, STATE_MACHINE_TASK_PERIOD_MS};

    // Phantom_createTask should block infinitely if task creation failed
    taskHandle = Phantom_createTask(&task, "StateMachineTask", STATE_MACHINE_TASK_STACK_SIZE, STATE_MACHINE_TASK_PRIORITY);

    // create your timers here...
    HV_CurrentTimerHandle = Phantom_createTimer("HV_Current_OutOfRange_T", 10000, NO_RELOAD, NULL, HV_CurrentTimerCallback);
    HV_VoltageTimerHandle = Phantom_createTimer("HV_Voltage_OutOfRange_T", 10000, NO_RELOAD, NULL, HV_VoltageTimerCallback);

    // any other init code you want to put goes here...
    HV_VOLTAGE_TIMER_EXPIRED = false;
    HV_CURRENT_TIMER_EXPIRED = false;
}

static void vStateMachineTask(void* arg)
{
    // arg will always be NULL, so ignore it.

    // your task stuff goes in here...
    State newState = VCUData_getState();

    bool TSAL_signal = VCUData_getTSALSignal();
    bool RTDS_signal = VCUData_getRTDSignal();
    uint32 faults = VCUData_readFaults(ALL_FAULTS);

    switch(newState)
    {
        case TRACTIVE_OFF:
            RGB_drive(RGB_CYAN);

            if (RTDS_signal || faults) {
                newState = SEVERE_FAULT;
            } else if (TSAL_signal) {
                newState = TRACTIVE_ON; //No faults AND TSAL is on AND RTDS not set
            }
            break;

        case TRACTIVE_ON:
            RGB_drive(RGB_MAGENTA);

            if (faults) {
                newState = isSevereFault(faults, newState) ? SEVERE_FAULT : isMinorFault(faults) ? MINOR_FAULT : newState;
            } else if (!TSAL_signal) {
                newState = TRACTIVE_OFF;
            } else if (RTDS_signal) {
                newState = RUNNING;
            }
            break;

        case RUNNING:
            RGB_drive(RGB_GREEN);

            if (faults) {
                // Update state depending on severity of fault
                newState = isSevereFault(faults, newState) ? SEVERE_FAULT : isMinorFault(faults) ? MINOR_FAULT : newState;
            } else if(!TSAL_signal) {
                newState= SEVERE_FAULT;
            } else if(!RTDS_signal) {
                newState = TRACTIVE_ON;
            }
            break;

        case MINOR_FAULT:
            RGB_drive(RGB_YELLOW);
            if (faults && isSevereFault(faults, newState)) {
                newState = SEVERE_FAULT;
            } else if (RTDS_signal && TSAL_signal) {
                newState = RUNNING;
            } else if (!RTDS_signal && TSAL_signal){
                newState = TRACTIVE_ON;
            } else if (!RTDS_signal) {
                newState = TRACTIVE_OFF;
            }

            // Reset the HV timer_started flags as the HV fault was fixed before timeout
            if (newState != MINOR_FAULT) {
                Phantom_stopTimer(HV_CurrentTimerHandle, 5);
                Phantom_stopTimer(HV_VoltageTimerHandle, 5);
            }
            break;

        case SEVERE_FAULT:
            RGB_drive(RGB_RED);

            if (!(faults || RTDS_signal))
            {
                newState = TRACTIVE_OFF;
            }
            break;
    }

    VCUData_setState(newState);
}

static bool isSevereFault(Fault faults, State currentState)
{
    if (!faults) {
        return false;
    }

    // Shutdown Circuit Fault or IMD Fault -> its Severe so don't need to check other faults
    if (faults & (SHUTDOWN_CIRCUIT_FAULT_GROUP | IMD_FAULT_GROUP))
    {
        return true;
    }

    // Check BSE APPS Faults
    if (faults & (BSE_APPS_FAULT_GROUP & ~BSE_APPS_SIMULTANEOUS_MINOR_FAULT))
    {
        // if the fault isn't the Minor Fault, then set SEVERE_FAULT, don't need to check for minor
        return true;
    }

    // Check HV Range Faults and set flags iff this isnt the first time (i.e. timers already started.)
    // Check for SEVERE Faults Right now
    if ((faults & HV_VOLTAGE_OUT_OF_RANGE_MINOR_FAULT) && HV_VOLTAGE_TIMER_EXPIRED)
    {
        // Timer expired
        HV_VOLTAGE_TIMER_EXPIRED = false;
        return true;
    }

    if ((faults & HV_CURRENT_OUT_OF_RANGE_MINOR_FAULT) && HV_CURRENT_TIMER_EXPIRED)
    {                
        //timer Expired
        HV_CURRENT_TIMER_EXPIRED = false;
        return true;
    }

    if((faults & HV_APPS_PROPORTION_SEVERE_ERROR) && currentState==RUNNING)
    {
        return true;
    }

    if((faults & TSAL_FAULT_GROUP) && (currentState==RUNNING || currentState==TRACTIVE_ON)){
        return true;
    }

    // Check CAN Severe Faults
    // Either CAN Message indicates a severe fault (TYPE1 ERROR) or a minor fault (TYPE2 ERROR)
    if (faults & CAN_ERROR_TYPE1)
    { // Severe Fault message from CAN, so don't need to check other faults, change currentState and yield task
        return true;
    }

    return false;
}

static bool isMinorFault(Fault faults)
{
    if (!faults) {
        return false;
    }

    // Check for MINOR Faults
    if (faults & HV_VOLTAGE_OUT_OF_RANGE_MINOR_FAULT)
    {
        if(!Phantom_isTimerActive(HV_VoltageTimerHandle)){
            // Start Timer
            HV_VOLTAGE_TIMER_EXPIRED = false;
            Phantom_startTimer(HV_VoltageTimerHandle, 0);
        }
        return true;
    }
    if (faults & HV_CURRENT_OUT_OF_RANGE_MINOR_FAULT)
    {
        if(!Phantom_isTimerActive(HV_CurrentTimerHandle)){
            // Start Timer
            HV_CURRENT_TIMER_EXPIRED = false;
            Phantom_startTimer(HV_CurrentTimerHandle, 0);
        }
        return true;
    }

    if (faults & (LV_CURRENT_OUT_OF_RANGE_MINOR_FAULT | 
                  LV_VOLTAGE_OUT_OF_RANGE_MINOR_FAULT | 
                  BSE_APPS_SIMULTANEOUS_MINOR_FAULT | 
                  CAN_ERROR_TYPE2))
    {
        return true;
    }

    return false;
}

// Other helper functions and callbacks goes here...
static void HV_CurrentTimerCallback(TimerHandle_t timer)
{
    HV_CURRENT_TIMER_EXPIRED = true;
}

static void HV_VoltageTimerCallback(TimerHandle_t timer)
{
    HV_VOLTAGE_TIMER_EXPIRED = true;
}
