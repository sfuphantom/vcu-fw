/*
 * vcu_data.c
 *
 *  Created on: Apr 18, 2020
 *      Author: soroush, Josh Guo
 */
#include "FreeRTOS.h"
#include "os_semphr.h"

#define VCUDATA_PRIVLEDGED_ACCESS
#include "vcu_data.h"

#define MUTEX_POLLING_TIME_MS   10
static SemaphoreHandle_t VCU_Key = NULL;

static VCUData data;
/*
typedef struct data
{
    Note: -> DO NOT change the place of vcuState structure in this file because:
         *  In eepromTask, we're reading vcuState from FEE bank using an OFFSET of 72 bytes.
         *  i.e. 73rd byte is the value corresponding to vcuState, based on the setup on
         *  the placement order of "data" structure below.

         *  Where did this 72 bytes number come from? Also, what is it offsetting from?

    Note: Now that the data structure has changed, how would the FEE bank know where vcu state is?

} data;
*/

/*
* NOTE : This function must be called before any operation on the VCU
*        data structure. Otherwise, undefined behaviour will happen.
*/
void VCUData_init(void)
{
    VCU_Key = xSemaphoreCreateMutex();

    data = (VCUData) {
        // Analog Readings
        (analogData) {0.0, 0.0},    // currentHV_A
        (analogData) {0.0, 0.0},    // voltageHV_V
        (analogData) {0.0, 0.0},    // currentLV_A
        (analogData) {0.0, 0.0},    // voltageLV_V
        (analogData) {0.0, 0.0},    // BSE_percentage
        (analogData) {0.0, 0.0},    // APPS1_percentage
        (analogData) {0.0, 0.0},    // APPS2_percentage
        // Analog Output
        (analogData) {0.0, 0.0},    // throttle_percentage
        // Digital Readings
        1U,                         // RTD_signal
        0U,                         // fault_flags
        // Digital Output
        0U,                         // brake_light_signal
        // Machine State
        TRACTIVE_OFF                // VCU_state
    };
}

/* FAULT FUNCTIONS */
uint32 VCUData_readFaults(uint32 mask)
{
    return data.fault_flags & mask;
}

bool VCUData_turnOnFaults(uint32 mask)
{
    if (xSemaphoreTake(VCU_Key, pdMS_TO_TICKS(MUTEX_POLLING_TIME_MS))) {

        data.fault_flags |= mask;

        return xSemaphoreGive(VCU_Key);
    } else {
        return false;
    }
}

bool VCUData_turnOffFaults(uint32 mask)
{
    if (xSemaphoreTake(VCU_Key, pdMS_TO_TICKS(MUTEX_POLLING_TIME_MS))) {

        data.fault_flags &= ~mask;

        return xSemaphoreGive(VCU_Key);
    } else {
        return false;
    }
}

bool VCUData_setFaults(uint32 newFaultBitSet)
{
    if (xSemaphoreTake(VCU_Key, pdMS_TO_TICKS(MUTEX_POLLING_TIME_MS))) {

        data.fault_flags = newFaultBitSet;

        return xSemaphoreGive(VCU_Key);
    } else {
        return false;
    }
}

/* SIGNAL FUNCTIONS */
uint8 VCUData_getRTDSignal(void)
{
    return data.RTD_signal;
}

bool VCUData_setRTDSignal(uint8 newSignal)
{
    if (xSemaphoreTake(VCU_Key, pdMS_TO_TICKS(MUTEX_POLLING_TIME_MS))) {

        data.RTD_signal = newSignal;

        return xSemaphoreGive(VCU_Key);
    } else {
        return false;
    }
}

uint8 VCUData_getBrakeLightSignal(void)
{
    return data.brake_light_signal;
}

bool VCUData_setBrakeLightSignal(uint8 newSignal)
{
    if (xSemaphoreTake(VCU_Key, pdMS_TO_TICKS(MUTEX_POLLING_TIME_MS))) {

        data.brake_light_signal = newSignal;

        return xSemaphoreGive(VCU_Key);
    } else {
        return false;
    }
}


/* STATE FUNCTIONS */
State VCUData_getState(void)
{
    return data.VCU_state;
}

bool VCUData_setState(State newState)
{
    if (xSemaphoreTake(VCU_Key, pdMS_TO_TICKS(MUTEX_POLLING_TIME_MS))) {

        data.VCU_state = newState;

        return xSemaphoreGive(VCU_Key);
    } else {
        return false;
    }
}


/* ANALOG DATA FUNCTIONS */
analogData VCUData_getCurrentHV_A(void)
{
    return data.currentHV_A;
}

bool VCUData_setCurrentHV_A(analogData newData)
{
    if (xSemaphoreTake(VCU_Key, pdMS_TO_TICKS(MUTEX_POLLING_TIME_MS))) {

        data.currentHV_A = newData;

        return xSemaphoreGive(VCU_Key);
    } else {
        return false;
    }
}

analogData VCUData_getVoltageHV_V(void)
{
    return data.voltageHV_V;
}

bool VCUData_setVoltageHV_V(analogData newData)
{
    if (xSemaphoreTake(VCU_Key, pdMS_TO_TICKS(MUTEX_POLLING_TIME_MS))) {

        data.voltageHV_V = newData;

        return xSemaphoreGive(VCU_Key);
    } else {
        return false;
    }
}

analogData VCUData_getCurrentLV_A(void)
{
    return data.currentLV_A;
}

bool VCUData_setCurrentLV_A(analogData newData)
{
    if (xSemaphoreTake(VCU_Key, pdMS_TO_TICKS(MUTEX_POLLING_TIME_MS))) {

        data.currentLV_A = newData;

        return xSemaphoreGive(VCU_Key);
    } else {
        return false;
    }
}

analogData VCUData_getVoltageLV_V(void)
{
    return data.voltageLV_V;
}

bool VCUData_setVoltageLV_V(analogData newData)
{
    if (xSemaphoreTake(VCU_Key, pdMS_TO_TICKS(MUTEX_POLLING_TIME_MS))) {

        data.voltageLV_V = newData;

        return xSemaphoreGive(VCU_Key);
    } else {
        return false;
    }
}

analogData VCUData_getBSEPercentage(void)
{
    return data.BSE_percentage;
}

bool VCUData_setBSEPercentage(analogData newData)
{
    if (xSemaphoreTake(VCU_Key, pdMS_TO_TICKS(MUTEX_POLLING_TIME_MS))) {

        data.BSE_percentage = newData;

        return xSemaphoreGive(VCU_Key);
    } else {
        return false;
    }
}

analogData VCUData_getAPPS1Percentage(void)
{
    return data.APPS1_percentage;
}

bool VCUData_setAPPS1Percentage(analogData newData)
{
    if (xSemaphoreTake(VCU_Key, pdMS_TO_TICKS(MUTEX_POLLING_TIME_MS))) {

        data.APPS1_percentage = newData;

        return xSemaphoreGive(VCU_Key);
    } else {
        return false;
    }
}

analogData VCUData_getAPPS2Percentage(void)
{
    return data.APPS2_percentage;
}

bool VCUData_setAPPS2Percentage(analogData newData)
{
    if (xSemaphoreTake(VCU_Key, pdMS_TO_TICKS(MUTEX_POLLING_TIME_MS))) {

        data.APPS2_percentage = newData;

        return xSemaphoreGive(VCU_Key);
    } else {
        return false;
    }
}

analogData VCUData_getThrottlePercentage(void)
{
    return data.throttle_percentage;
}

bool VCUData_setThrottlePercentage(analogData newData)
{
    if (xSemaphoreTake(VCU_Key, pdMS_TO_TICKS(MUTEX_POLLING_TIME_MS))) {

        data.throttle_percentage = newData;

        return xSemaphoreGive(VCU_Key);
    } else {
        return false;
    }
}

// FOR PRIVLEDGED ACCESS ONLY (like EEPROM)
VCUData* VCUData_getPointer(void)
{
    return &data;
}
