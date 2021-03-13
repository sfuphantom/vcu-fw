/*
 * Shutdown.c
 *
 *  Created on: Jan 27, 2021
 *      Author: Rafael Guevara
 */

//may need to add extern keyword for accessing vcu data structure?

#include "Shutdown.h"

static void resetSignals(){

    VCUDataPtr->DigitalVal.BMS_FAULT = false;

    VCUDataPtr->DigitalVal.BSPD_FAULT = false;

    VCUDataPtr->DigitalVal.TSAL_WELDED = false;

    VCUDataPtr->DigitalVal.IMD_FAULT = false;

}

void ShutdownInit(){

       gioInit();

       hetInit();

       vimInit();

       //set hetPORT1 as inputs
       gioSetDirection(hetPORT1, 0x00000000);

       resetSignals();

       //Enable Interrupts
       edgeEnableNotification(RESETPort,RESETedge);

       gioEnableNotification(SHUTDOWN_CIRCUIT_PORT,BMS_FAULT_PIN);

       gioEnableNotification(SHUTDOWN_CIRCUIT_PORT,BSPD_FAULT_PIN);

       gioEnableNotification(SHUTDOWN_CIRCUIT_PORT,IMD_FAULT_PIN);

       _enable_IRQ();

       printf("Shutdown Driver initialized\n");
}

void print_Shutdownvals(){

    printf("\nBMSval: ");
    printf(VCUDataPtr->DigitalVal.BMS_STATUS ? "true" : "false");

    printf("\nBSPDval: ");
    printf(VCUDataPtr->DigitalVal.BSPD_STATUS ? "true" : "false");


    printf("\nIMDval: ");
    printf(VCUDataPtr->DigitalVal.IMD_STATUS ? "true" : "false");

    printf("\nTSALval: ");
    printf(VCUDataPtr->DigitalVal.TSAL_STATUS ? "true" : "false");

    printf("\nRESETval: ");
    printf(VCUDataPtr->DigitalVal.RESET_STATUS ? "true" : "false");

}

void storeShutdownValues(){

    //getBit(blah blah)

    VCUDataPtr->DigitalVal.BMS_STATUS = gioGetBit(SHUTDOWN_CIRCUIT_PORT,BMS_FAULT_PIN);

    VCUDataPtr->DigitalVal.IMD_STATUS = gioGetBit(SHUTDOWN_CIRCUIT_PORT,IMD_FAULT_PIN);

    VCUDataPtr->DigitalVal.TSAL_STATUS = gioGetBit(TSAL_PORT,TSAL_ACTIVE_PIN);

    VCUDataPtr->DigitalVal.BSPD_STATUS = gioGetBit(SHUTDOWN_CIRCUIT_PORT,BSPD_FAULT_PIN);

    VCUDataPtr->DigitalVal.RESET_STATUS = gioGetBit(RESETPort,RESETPin);

}

void gioNotification(gioPORT_t* port,uint32 bit){

    if(port == SHUTDOWN_CIRCUIT_PORT && bit == BMS_FAULT_PIN) VCUDataPtr->DigitalVal.BMS_FAULT = true;

    if(port == SHUTDOWN_CIRCUIT_PORT && bit == BSPD_FAULT_PIN) VCUDataPtr->DigitalVal.BSPD_FAULT = true;

    if(port == SHUTDOWN_CIRCUIT_PORT && bit == IMD_FAULT_PIN) VCUDataPtr->DigitalVal.IMD_FAULT = true;

    // (Shutdown Board SHOULD be Triggered)   &&   (TSAL_HV == ON) ----> TSAL_WELDED
    if((BMS_FAULT || IMD_FAULT || BSPD_FAULT) && gioGetBit(TSAL_PORT,TSAL_ACTIVE_PIN)) VCUDataPtr->DigitalVal.TSAL_WELDED = true;

}

//code will have to be merged with edgeNotification in IMD driver
void edgeNotification(hetBASE_t * hetREG,uint32 edge)
{
    if(hetREG == RESETPort && edge == RESETedge) resetSignals();

}


