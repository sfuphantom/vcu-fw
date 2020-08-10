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
*       AMS  = Alternator Management System : Constantly monitors your vehicle’s operating conditions and battery state of charge to ensure optimized charge.
*       AIRs = Accumulator Isolation Relays
*       - The Accumulator Indicator Light(AIL) will light whenever the voltage of the battery pack exceeds 60V.
*       - The Tractive System Active Light (TSAL) is a red/green indicator light that indicates the presence of high-voltage in the Tractive System.
*               The Tractive System is the part of the Vehicle that carries the current to power your motor(s).
*               This includes the Accumulator, the Inverter(s) and the motor(s).
*                   The Accumulator consists of the cells which store the actual energy,
*                       the AMS which is measuring and monitoring the cells' voltages and temperatures,
*                           the AIRs which are normally open relays to isolate the cells when the Tractive system is turned off,
*                               and eventually an IMD that is needed to ensure isolation of the Tractive system from the Low Voltage System and a DC/DC converter to power the Low Voltage System from the Accumulator.
*                                   The Tractive System Accumulator Container is the housing, that contains all these parts.
*       - The brake system plausibility device (BSPD) is a critical safety board which ensures shutdown of the tractive system when there is high load on both the accelerator pedal position sensors and brake sensors.
*             Specifically, as defined in the FSAE2020 V2.1 rules,it is required to send the signal to the shutdown circuit (SDC) whenever the following two conditions are achieved for at least 500ms:
*                   1) 5kW of power delivered to the inverter at nominal battery voltage
*                   2) The brake sensor receives a ‘hard braking’ event
*           Read documentation: https://docs.google.com/document/d/1zKc7dew-7V57hvTovyfzPF5JPPPnbJh_N0uKOBGgJLc/edit
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

    VCUDataPtr->DigitalVal.RTDS = 1;
    VCUDataPtr->DigitalVal.BMS_FAULT = 1;
    VCUDataPtr->DigitalVal.IMD_FAULT = 1;
    VCUDataPtr->DigitalVal.BSPD_FAULT = 1;

    VCUDataPtr->DigitalVal.TSAL_FAULT = 1;
    VCUDataPtr->DigitalVal.BSE_FAULT = 0;
    VCUDataPtr->DigitalVal.APPS_FAULT = 0;

    VCUDataPtr->DigitalOut.BRAKE_LIGHT_ENABLE = 0;
}


