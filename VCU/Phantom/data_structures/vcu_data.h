/*
 * vcu_data.h
 *
 *  Created on: Apr 18, 2020
 *      Author: soroush, Josh Guo
 */

#ifndef PHANTOM_DATA_STRUCTURES_VCU_DATA_H_
#define PHANTOM_DATA_STRUCTURES_VCU_DATA_H_

#include "hal_stdtypes.h"

typedef enum {
    BMS_FAULT = (1U << 0),                      // if 0, then BMS is healthy, if 1 then BMS fault
    IMD_FAULT = (1U << 1),                      // if 0, then IMD is healthy, if 1 then IMD fault
    BSPD_FAULT = (1U << 2),                     // if 0, then BSPD is healthy, if 1 then BSPD fault
    TSAL_FAULT = (1U << 3),                     // if 0, then TSAL is on and AIRs are closed, if 1 then TSAL is off and AIRs should be open
    BSE_FAULT = (1U << 4),                      // if 0, then BSE is healthy, if 1 then BSE fault

    BSE_APPS_SIMULTANEOUS_FAULT = (1U << 5),    // MINOR
    BSE_RANGE_FAULT = (1U << 6),                // SEVERE
    APPS_10DIFF_FAULT = (1U << 7),              // SEVERE
    APPS1_RANGE_FAULT = (1U << 8),              // SEVERE
    APPS2_RANGE_FAULT = (1U << 9),              // SEVERE

    ALL_FAULTS = (~0U)   // All 1s to let all faults through the mask
} FaultMask;

typedef enum {
    TRACTIVE_OFF, 
    TRACTIVE_ON, 
    RUNNING, 
    MINOR_FAULT,
    SEVERE_FAULT
} State;

// function prototypes
void VCUData_init(void);

uint32 VCUData_readFaults(uint32 mask);
bool VCUData_turnOnFaults(uint32 mask);
bool VCUData_turnOffFaults(uint32 mask);
bool VCUData_setFaults(uint32 mask);


bool VCUData_getRTDSignal(void);
bool VCUData_getBrakeLightSignal(void);
bool VCUData_getThrottleAvailableSignal(void);
State VCUData_getState(void);

bool VCUData_setRTDSignal(bool newSignal);
bool VCUData_setBrakeLightSignal(bool newSignal);
bool VCUData_setThrottleAvailableSignal(bool newSignal);
bool VCUData_setState(State newState);


float VCUData_getCurrentHV_A(void);
float VCUData_getVoltageHV_V(void);
float VCUData_getCurrentLV_A(void);
float VCUData_getVoltageLV_V(void);
float VCUData_getBSEPercentage(void);
float VCUData_getAPPS1Percentage(void);
float VCUData_getAPPS2Percentage(void);
float VCUData_getThrottlePercentage(void);

bool VCUData_setCurrentHV_A(float newValue);
bool VCUData_setVoltageHV_V(float newValue);
bool VCUData_setCurrentLV_A(float newValue);
bool VCUData_setVoltageLV_V(float newValue);
bool VCUData_setBSEPercentage(float newValue);
bool VCUData_setAPPS1Percentage(float newValue);
bool VCUData_setAPPS2Percentage(float newValue);
bool VCUData_setThrottlePercentage(float newValue);

// Any module that needs direct access
// to the VCU data structure needs to 
// #define VCUDATA_PRIVLEDGED_ACCESS in their file
#ifdef VCUDATA_PRIVLEDGED_ACCESS

typedef struct {
    float currentHV_A;
    float voltageHV_V;
    float currentLV_A;
    float voltageLV_V;
    float BSE_percentage;
    float APPS1_percentage;
    float APPS2_percentage;
    
    float throttle_percentage;

    bool RTD_signal;
    bool throttle_available;
    uint32 fault_flags;

    bool brake_light_signal;

    State VCU_state;
} VCUData;

VCUData* VCUData_getPointer(void);

#endif

#endif /* PHANTOM_DATA_STRUCTURES_VCU_DATA_H_ */
