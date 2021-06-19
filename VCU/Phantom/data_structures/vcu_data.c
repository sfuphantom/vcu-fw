/*
 * vcu_data.c
 *
 *  Created on: Apr 18, 2020
 *      Author: soroush
 */
#include "vcu_data.h"


/*******************************************************************
* NAME :            initData
*
* DESCRIPTION :     Initializes the VCU State Machine
*
* INPUTS : Pointer to baseAddress to data structure of VCU data.
*
* RETURN : None
*
*
* NOTES :
*
*/
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

    VCUDataPtr->DigitalOut.BRAKE_LIGHT_ENABLE = 0;

    VCUDataPtr->DigitalVal.RTDS = 1;
    VCUDataPtr->DigitalVal.BMS_FAULT = 1;
    VCUDataPtr->DigitalVal.IMD_FAULT = 1;
    VCUDataPtr->DigitalVal.BSPD_FAULT = 1;

    VCUDataPtr->DigitalVal.TSAL_FAULT = 1;
    VCUDataPtr->DigitalVal.BSE_FAULT = 0;
    VCUDataPtr->DigitalVal.APPS_FAULT = 0;



    VCUDataPtr->DigitalVal.APPS_PROPORTION_ERROR = 0; // -rafguevara14.....
    VCUDataPtr->DigitalVal.HVCURRENT_OUT_OF_RANGE = 0;

    VCUDataPtr->DigitalVal.RESET_STATUS = 0;
    VCUDataPtr->DigitalVal.IMD_FAULT = 0;
    VCUDataPtr->DigitalVal.BSPD_FAULT = 0;
    VCUDataPtr->DigitalVal.BMS_FAULT = 0;
    VCUDataPtr->DigitalVal.TSAL_WELDED = 0;         //  -.....rafguevara14









    VCUDataPtr->DigitalOut.BRAKE_LIGHT_ENABLE = 0;
    // ++ Added by jjkhan
    VCUDataPtr->vcuState = TRACTIVE_OFF;
    // -- Added by jjkhan
}

