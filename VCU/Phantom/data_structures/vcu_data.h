/*
 * vcu_data.h
 *
 *  Created on: Apr 18, 2020
 *      Author: soroush, Josh Guo
 */

#ifndef PHANTOM_DATA_STRUCTURES_VCU_DATA_H_
#define PHANTOM_DATA_STRUCTURES_VCU_DATA_H_
typedef struct {
    float adc_value;
    float value; // in the unit of measurement
} analogData;

typedef enum {
    currentHV_A,
    voltageHV_V,
    currentLV_A,
    voltageLV_V,
    BSE_percentage,
    APPS1_percentage,
    APPS2_percentage,

    // output data
    throttle_percentage,

    // leave this as the last item in enum
    _ANALOG_DATA_LENGTH
} AnalogValueIndex;

typedef enum {
    BMS_FAULT_MASK = 1U,      // if 0, then BMS is healthy, if 1 then BMS fault
    IMD_FAULT_MASK = 2U,      // if 0, then IMD is healthy, if 1 then IMD fault
    BSPD_FAULT_MASK = 4U,      // if 0, then BSPD is healthy, if 1 then BSPD fault
    TSAL_FAULT_MASK = 8U,      // if 0, then TSAL is on and AIRs are closed, if 1 then TSAL is off and AIRs should be open
    BSE_FAULT_MASK = 16U,     // if 0, then BSE is healthy, if 1 then BSE fault
    APPS_FAULT_MASK = 32U,     // if 0, then APPS is healthy, if 1 then APPS fault
    ALL_FAULTS_MASK = (~0U)   // All 1s to let all faults through the mask
} FaultMasks;

typedef enum {
    TRACTIVE_OFF, 
    TRACTIVE_ON, 
    RUNNING, 
    MINOR_FAULT,
    SEVERE_FAULT
} State;


// function prototypes

void VCUData_init(void);

analogData VCUData_getAnalogData(AnalogValueIndex keyword);
bool VCUData_setAnalogData(AnalogValueIndex keyword, analogData newData);

bool VCUData_getRTDSignal(void);
bool VCUData_setRTDSignal(bool newSignal);

bool VCUData_getBrakeLightSignal(void);
bool VCUData_setBrakeLightSignal(bool newSignal);

uint8 VCUData_readFaults(uint8 mask);
bool VCUData_turnOnFaults(uint8 mask);
bool VCUData_turnOffFaults(uint8 mask);
bool VCUData_setFaults(uint8 newFaultBitSet);

State VCUData_getState(void);
bool VCUData_setState(State newState);

#endif /* PHANTOM_DATA_STRUCTURES_VCU_DATA_H_ */
