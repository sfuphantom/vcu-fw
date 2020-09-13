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

    /* Note: -> DO NOT change the place of POWER_FAILURE_FLAG & "digitalValues" structure in this file because:
     *
     *  In eepromTask, we're reading POWER_FAILURE_FLAG from FEE bank using an OFFSET of 72 bytes.
     *      i.e. 73rd byte is the value corrresponding to POWER_FAILURE_FLAG, based on the setup on the placement order of "data" structure below.
     *
     *
     * Comment by - jjkhan
     *  */


    uint8_t RTDS;               // if 1, ready to drive is set, if 0 then not ready to drive
    uint8_t BMS_FAULT;         // if 1, then BMS is healthy, if 0 then BMS fault
    uint8_t IMD_FAULT;         // if 1, then IMD is healthy, if 0 then IMD fault
    uint8_t BSPD_FAULT;        // if 1, then BSPD is healthy, if 0 then BSPD fault

    uint8_t TSAL_FAULT;        // if 1, then TSAL is on and AIRs are closed, if 0 then TSAL is off and AIRs should be open
    uint8_t BSE_FAULT;          // if 0, then BSE is healthy, if 1 then BSE fault
    uint8_t APPS_FAULT;         // if 0, then APPS is healthy, if 1 then APPS fault

    /* ++Added by jjkhan */
    uint8_t POWER_FAILURE_FLAG;
    /* --Added by jjkhan */

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
    digitalOutputs DigitalOut;

    /*  Comments by jjkhan -> the addition of powerFailureFlag to the end allows me to easily offset
     *  Data_block2 (assigned to VCU data struture) by 72 bytes, as I will update the byte periodically in the state machine task and
     *  write that value in eeprom memory -> need it for initialization of vcu-data structure after power-on.
     */
    digitalValues DigitalVal;




} data;



// function prototypes

void initData(data* VCUDataPtr);

#endif /* PHANTOM_DATA_STRUCTURES_VCU_DATA_H_ */
