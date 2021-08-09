/*
 * vcu_data.c
 *
 *  Created on: Apr 18, 2020
 *      Author: soroush
 */
#include "hal_stdtypes.h"
#include "FreeRTOS.h"
#include "os_semphr.h"

#include "vcu_data.h"

static analogData analogDataArray[_ANALOG_DATA_LENGTH];

/* DIGITAL DATA */
static bool RTD_signal;
static bool brake_light_signal; // originally named BRAKE_LIGHT_ENABLED

static uint8 fault_flags; // fault_flags can hold 8 different faults

static State VCU_state;

#define MUTEX_POLLING_TIME_MS   10
static SemaphoreHandle_t VCU_Key = NULL;

/*
typedef struct data
{
    Note: -> DO NOT change the place of vcuState structure in this file because:
         *  In eepromTask, we're reading vcuState from FEE bank using an OFFSET of 72 bytes.
         *  i.e. 73rd byte is the value corresponding to vcuState, based on the setup on
         *  the placement order of "data" structure below.

         *  Where did this 72 bytes number come from? Also, what is it offsetting from?

    Note: Wow, this method needs to change. Reading from a raw memory position, while it may work
        if and only if the position is correct, is a terribly dangerous and unstable way to read
        the data you need. This method also completely restricts where and what the data structure
        can be and can hold without the headache of actually having to find where in physical memory it is.

} data;
*/

/*******************************************************************
* NOTES : This function must be called before any operation on the VCU
*         data structure. Otherwise, undefined behaviour will happen.
*/
void VCUData_init(void)
{
    VCU_Key = xSemaphoreCreateMutex();

    for (int i = 0; i < _ANALOG_DATA_LENGTH; i++) {
        analogDataArray[i] = (analogData) {0.0, 0.0};
    }

    signal_flags = RTD_SIGNAL_MASK;
    fault_flags = 0U;
    VCU_state = TRACTIVE_OFF;
}


/* ANALOG DATA FUNCTIONS */
analogData VCU_getAnalogData(AnalogValueIndex keyword)
{
    return analogDataArray[keyword];
}

bool VCU_setAnalogData(AnalogValueIndex keyword, analogData newData)
{
    if (xSemaphoreTake(VCU_Key, pdMS_TO_TICKS(MUTEX_POLLING_TIME_MS))) {

        analogDataArray[keyword] = newData;

        return xSemaphoreGive(VCU_Key);
    } else {
        return false;
    }
}


/* SIGNAL FUNCTIONS */
bool VCUData_getRTDSignal(void)
{
    return RTD_signal;
}

bool VCUData_setRTDSignal(bool newSignal)
{
    if (xSemaphoreTake(VCU_Key, pdMS_TO_TICKS(MUTEX_POLLING_TIME_MS))) {

        RTD_signal = newSignal;

        return xSemaphoreGive(VCU_Key);
    } else {
        return false;
    }
}

bool VCUData_getBrakeLightSignal(void)
{
    return brake_light_signal;
}

bool VCUData_setBrakeLightSignal(bool newSignal)
{
    if (xSemaphoreTake(VCU_Key, pdMS_TO_TICKS(MUTEX_POLLING_TIME_MS))) {

        brake_light_signal = newSignal;

        return xSemaphoreGive(VCU_Key);
    } else {
        return false;
    }
}


/* FAULT FUNCTIONS */
uint8 VCUData_readFaults(uint8 mask)
{
    return fault_flags & mask;
}

bool VCUData_turnOnFaults(uint8 mask)
{
    if (xSemaphoreTake(VCU_Key, pdMS_TO_TICKS(MUTEX_POLLING_TIME_MS))) {

        fault_flags |= mask;

        return xSemaphoreGive(VCU_Key);
    } else {
        return false;
    }
}

bool VCUData_turnOffFaults(uint8 mask)
{
    if (xSemaphoreTake(VCU_Key, pdMS_TO_TICKS(MUTEX_POLLING_TIME_MS))) {

        fault_flags &= ~mask;

        return xSemaphoreGive(VCU_Key);
    } else {
        return false;
    }
}

bool VCUData_setFaults(uint8 newFaultBitSet)
{
    if (xSemaphoreTake(VCU_Key, pdMS_TO_TICKS(MUTEX_POLLING_TIME_MS))) {

        fault_flags = newFaultBitSet;

        return xSemaphoreGive(VCU_Key);
    } else {
        return false;
    }
}


/* STATE FUNCTIONS */
State VCUData_getState(void)
{
    return VCU_state;
}

bool VCUData_setState(State newState)
{
    if (xSemaphoreTake(VCU_Key, pdMS_TO_TICKS(MUTEX_POLLING_TIME_MS))) {

        VCU_state = newState;

        return xSemaphoreGive(VCU_Key);
    } else {
        return false;
    }
}