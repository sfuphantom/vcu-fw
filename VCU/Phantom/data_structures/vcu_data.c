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

    // 1 = has a fault  ||      0 = has no fault
    VCUDataPtr->DigitalVal.RTDS = 0;
    VCUDataPtr->DigitalVal.BMS_GPIO_FAULT = 0;
    VCUDataPtr->DigitalVal.IMD_FAULT = 0;
    VCUDataPtr->DigitalVal.BSPD_FAULT = 0;
    VCUDataPtr->DigitalVal.TSAL_ON = 0;

    /* ++ New Faults*/

    /* APPS/BSE Sensor - Faults */

    VCUDataPtr->DigitalVal.BSE_SEVERE_RANGE_FAULT=0;
    VCUDataPtr->DigitalVal.APPS1_SEVERE_RANGE_FAULT=0;
    VCUDataPtr->DigitalVal.APPS2_SEVERE_RANGE_FAULT=0;
    VCUDataPtr->DigitalVal.APPS_SEVERE_10DIFF_FAULT=0;
    VCUDataPtr->DigitalVal.BSE_APPS_MINOR_SIMULTANEOUS_FAULT=0;

    /* HV Current Sensor & Voltage Sensor- Faults */

    VCUDataPtr->DigitalVal.HV_CURRENT_OUT_OF_RANGE=0;
    VCUDataPtr->DigitalVal.APPS_PROPORTION_ERROR=0;
    VCUDataPtr->DigitalVal.HV_VOLTAGE_OUT_OF_RANGE_FAULT=0;

    /* CAN Error Messages. */

    VCUDataPtr->DigitalVal.CAN_ERROR_TYPE1=0; // Severe Error reported by CAN
    VCUDataPtr->DigitalVal.CAN_ERROR_TYPE2=0;  // Minor Errot Reported by CAN

    /* LV Current Sensor & Voltage Sensor- Faults */
    VCUDataPtr->DigitalVal.LV_CURRENT_OUT_OF_RANGE=0;
    VCUDataPtr->DigitalVal.LV_VOLTAGE_OUT_OF_RANGE=0;

    /* IMD Faults. */
    VCUDataPtr->DigitalVal.IMD_LOW_ISO_FAULT=0;
    VCUDataPtr->DigitalVal.IMD_SHORT_CIRCUIT_FAULT=0;
    VCUDataPtr->DigitalVal.IMD_DEVICE_ERR_FAULT=0;
    VCUDataPtr->DigitalVal.IMD_BAD_INFO_FAULT=0;
    VCUDataPtr->DigitalVal.IMD_UNDEF_ERR=0;
    VCUDataPtr->DigitalVal.IMD_GARBAGE_DATA_FAULT=0;

    /* TSAL Faults.*/
    VCUDataPtr->DigitalVal.TSAL_WELDED_AIRS_FAULT=0;

    /* -- New Faults  */

    VCUDataPtr->DigitalOut.BRAKE_LIGHT_ENABLE = 0;

    // ++ Added by Jay Pacamarra
    VCUDataPtr->DigitalVal.BSE_APPS_MINOR_SIMULTANEOUS_FAULT = 0;
    VCUDataPtr->DigitalVal.BSE_SEVERE_RANGE_FAULT = 0;
    VCUDataPtr->DigitalVal.APPS_SEVERE_10DIFF_FAULT = 0;
    VCUDataPtr->DigitalVal.APPS1_SEVERE_RANGE_FAULT = 0;
    VCUDataPtr->DigitalVal.APPS2_SEVERE_RANGE_FAULT = 0;
    // ++ Added by Jay Pacamarra

    // ++ Added by jjkhan
    VCUDataPtr->vcuState = TRACTIVE_OFF;
}

