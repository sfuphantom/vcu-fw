/*
 * vcu_data.c
 *
 *  Created on: Apr 18, 2020
 *      Author: soroush
 */
#include "vcu_data.h"

void initData(data* VCUDataPtr)
{
    /***********************************************************
     *              ANALOG VALUE INITIALIZATION
     ***********************************************************/

    VCUDataPtr->AnalogIn.APPS1_percentage.adc_value = 0;
    VCUDataPtr->AnalogIn.APPS1_percentage.value = 0.0;

    VCUDataPtr->AnalogIn.APPS2_percentage.adc_value = 0;
    VCUDataPtr->AnalogIn.APPS2_percentage.value = 0.0;

    VCUDataPtr->AnalogIn.BSE_percentage.adc_value = 0;
    VCUDataPtr->AnalogIn.BSE_percentage.value = 0.0;

    VCUDataPtr->AnalogIn.currentHV_A.adc_value = 0;
    VCUDataPtr->AnalogIn.currentHV_A.value = 0.0;

    VCUDataPtr->AnalogIn.voltageHV_V.adc_value = 0;
    VCUDataPtr->AnalogIn.voltageHV_V.value = 0.0;

    VCUDataPtr->AnalogIn.currentLV_A.adc_value = 0;
    VCUDataPtr->AnalogIn.currentLV_A.value = 0.0;

    VCUDataPtr->AnalogIn.voltageLV_V.adc_value = 0;
    VCUDataPtr->AnalogIn.voltageLV_V.value = 0.0;

    VCUDataPtr->AnalogOut.throttle_percentage.adc_value = 0;
    VCUDataPtr->AnalogOut.throttle_percentage.value = 0.0;

    /***********************************************************
     *              DIGITAL VALUE INITIALIZATION
     ***********************************************************/

    VCUDataPtr->DigitalVal.RTDS = 1;
    VCUDataPtr->DigitalVal.BMS_STATUS = 1;
    VCUDataPtr->DigitalVal.IMD_STATUS = 1;
    VCUDataPtr->DigitalVal.BSPD_STATUS = 1;

    VCUDataPtr->DigitalVal.TSAL_STATUS = 1;
    VCUDataPtr->DigitalVal.BSE_APPS_MINOR_SIMULTANEOUS_FAULT = 0;
    VCUDataPtr->DigitalVal.BSE_FAULT = 0;
    VCUDataPtr->DigitalVal.BSE_SEVERE_RANGE_FAULT = 0;
    VCUDataPtr->DigitalVal.APPS_SEVERE_10DIFF_FAULT = 0;
    VCUDataPtr->DigitalVal.APPS1_SEVERE_RANGE_FAULT = 0;
    VCUDataPtr->DigitalVal.APPS2_SEVERE_RANGE_FAULT = 0;
    VCUDataPtr->DigitalVal.APPS_FAULT = 0;

    VCUDataPtr->DigitalOut.BRAKE_LIGHT_ENABLE = 0;
}


