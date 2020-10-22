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

/*
 *   Added by jjkhan:
 *                  XXX_FAULT = 1 -> System is not healthy/fault detected.
 *                  XXX_FAULT = 0 -> System is healthy/No fault.
 *
 *                  TSAL_ON = 1 -> Tractive System Active Light is on  (AIRs closed)
 *                  TSAL_ON = 0 -> Tractive System Active Light is off.(AIRs open)
 *
 *                  RTDS = 1 -> Ready To Drive is Set
 *                  RTDS = 0 -> Ready To Driver is not Set
 */

typedef struct digitalValues
{
    uint8_t TSAL_ON;
    uint8_t RTDS;
    uint8_t BMS_FAULT;
    uint8_t IMD_FAULT;
    uint8_t BSPD_FAULT;
    uint8_t BSE_FAULT;
    uint8_t APPS_FAULT;

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
