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

/*
 *
 *  New faults: Documentation: https://docs.google.com/document/d/1R-fJ0VAyK6HpsrrwhxrWxydJqgsh1hNZr3dGRHAYEkk/edit - jjkhan
 *
 */

typedef struct digitalValues
{


    uint8_t TSAL_ON;
    uint8_t RTDS;


    // Shutdown Circuit Faults
    uint8_t BMS_GPIO_FAULT; // Changed BMS_FAULT to BMS_GPIO_FAULT - jjkhan
    uint8_t IMD_FAULT; // IMD Fault Added back - jjkhan
    uint8_t BSPD_FAULT;
    //uint8_t BSE_FAULT;  //  -> No more BSE, there is BSE_SEVERE_RANGE_FAULT
    //uint8_t APPS_FAULT;//  -> No more APPS, there is APPS1 and APPS2 check  - jjkhan


    /* ++ New Faults*/

    /* APPS/BSE Sensor - Faults */

    uint8_t BSE_SEVERE_RANGE_FAULT;
    uint8_t APPS1_SEVERE_RANGE_FAULT;
    uint8_t APPS2_SEVERE_RANGE_FAULT;
    uint8_t APPS_SEVERE_10DIFF_FAULT;
    uint8_t BSE_APPS_MINOR_SIMULTANEOUS_FAULT;

    /* HV Current Sensor & Voltage Sensor- Faults */

    uint8_t HV_CURRENT_OUT_OF_RANGE;
    uint8_t APPS_PROPORTION_ERROR;
    uint8_t HV_VOLTAGE_OUT_OF_RANGE_FAULT;

    /* CAN Error Messages. */

    uint8_t CAN_ERROR_TYPE1; // Severe Error reported by CAN
    uint8_t CAN_ERROR_TYPE2;  // Minor Errot Reported by CAN

    /* LV Current Sensor & Voltage Sensor- Faults */
    uint8_t LV_CURRENT_OUT_OF_RANGE;
    uint8_t LV_VOLTAGE_OUT_OF_RANGE;

    /* IMD Faults. */
    uint8_t IMD_LOW_ISO_FAULT;
    uint8_t IMD_SHORT_CIRCUIT_FAULT;
    uint8_t IMD_DEVICE_ERR_FAULT;
    uint8_t IMD_BAD_INFO_FAULT;
    uint8_t IMD_UNDEF_ERR;
    uint8_t IMD_GARBAGE_DATA_FAULT;

    /* TSAL Faults.*/
    uint8_t TSAL_WELDED_AIRS_FAULT;

    /* -- New Faults  */



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
