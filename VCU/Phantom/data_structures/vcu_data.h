/*
 * vcu_data.h
 *
 *  Created on: Apr 18, 2020
 *      Author: soroush
 */

#ifndef PHANTOM_DATA_STRUCTURES_VCU_DATA_H_
#define PHANTOM_DATA_STRUCTURES_VCU_DATA_H_


#include "sys_common.h"

typedef struct analogData
{
    float adc_value;
    float value; // in the unit of measurement
} analogData;

typedef struct analogInputs
{
    analogData currentHV_A;
    analogData voltageHV_V;
    analogData currentLV_A;
    analogData voltageLV_V;
    analogData BSE_percentage;
    analogData APPS1_percentage;
    analogData APPS2_percentage;
} analogInputs;

typedef struct analogOutputs
{
    analogData throttle_percentage;
} analogOutputs;

typedef struct digitalValues
{
    uint8_t RTDS;               // if 1, ready to drive is set, if 0 then not ready to drive
    uint8_t BMS_STATUS;         // if 1, then BMS is healthy, if 0 then BMS fault
    uint8_t IMD_STATUS;         // if 1, then IMD is healthy, if 0 then IMD fault
    uint8_t BSPD_STATUS;        // if 1, then BSPD is healthy, if 0 then BSPD fault

    uint8_t TSAL_STATUS;                        // if 1, then TSAL is on and AIRs are closed, if 0 then TSAL is off and AIRs should be open
    uint8_t BSE_FAULT;                          // if 0, then BSE is healthy, if 1 then BSE fault
    uint8_t BSE_SEVERE_RANGE_FAULT;             // if 0, then BSE is healthy, if 1 then BSE fault - jaypacamarra
    uint8_t BSE_APPS_MINOR_SIMULTANEOUS_FAULT;  // if 0, then brake&accelerator NOT pressed together, if 1 then brake&accelerator pressed together - jaypacamarra
    uint8_t APPS_SEVERE_10DIFF_FAULT;           // if 0, then APPS is healthy, if 1 then APPS1 and APPS2 disagree by more than 10% - jaypacamarra
    uint8_t APPS1_SEVERE_RANGE_FAULT;           // if 0, then APPS is healthy, if 1 then APPS fault - jaypacamarra
    uint8_t APPS2_SEVERE_RANGE_FAULT;           // if 0, then APPS is healthy, if 1 then APPS fault - jaypacamarra
    uint8_t APPS_FAULT;                         // if 0, then APPS is healthy, if 1 then APPS fault

    // many other faults..
} digitalValues;

typedef struct digitalOutputs
{
    uint8_t BRAKE_LIGHT_ENABLE; // if 1, then BSE is pressed and brake light should turn on, if 0 then brake light is off
    // somehow show the PWM for the buzzer here
} digitalOutputs;


typedef struct data
{
    analogInputs AnalogIn;
    analogOutputs AnalogOut;
    digitalValues DigitalVal;
    digitalOutputs DigitalOut;
} data;



// function prototypes

void initData(data* VCUDataPtr);

#endif /* PHANTOM_DATA_STRUCTURES_VCU_DATA_H_ */
