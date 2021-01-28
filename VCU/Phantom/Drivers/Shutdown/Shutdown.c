/*
 * Shutdown.c
 *
 *  Created on: Jan 27, 2021
 *      Author: Rafael Guevara
 */

#include "Shutdown.h"

void print_Shutdownvals(){

    printf("\nBMSval: ");
    printf(BMSval ? "true" : "false");

    printf("\nBSPDval: ");
    printf(BSPDval ? "true" : "false");


    printf("\nIMDval: ");
    printf(IMDval ? "true" : "false");

    printf("\nTSALval: ");
    printf(TSALval ? "true" : "false");

    printf("\nRESETval: ");
    printf(RESETval ? "true" : "false");

}

static void resetSignals(){

    BMS_FAULT = false;

    BSPD_FAULT = false;

    TSAL_WELDED = false;

    IMD_FAULT = false;

}

void gioNotification(gioPORT_t* port,uint32 bit){

    if(port == BMSPin && bit == BMSNumPin) BMS_FAULT = true;

    if(port == BSPDPin && bit == BSPDNumPin) BSPD_FAULT = true;

    if(port == IMDPin && bit == IMDNumPin) IMD_FAULT = true;

    // (Shutdown Board SHOULD be Triggered)   &&   (TSAL_HV == ON) ----> TSAL_WELDED
    if((BMS_FAULT || IMD_FAULT || BSPD_FAULT) && gioGetBit(TSALPin,TSALNumPin)) TSAL_WELDED = true;

}

//code will have to be merged with edgeNotification in IMD driver
void edgeNotification(hetBASE_t * hetREG,uint32 edge)
{
    if(hetREG == RESETPort && edge == RESETedge) resetSignals();

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

       gioEnableNotification(BMSPin,BMSNumPin);

       gioEnableNotification(BSPDPin,BSPDNumPin);

       gioEnableNotification(IMDPin,IMDNumPin);

       _enable_IRQ();

       printf("Shutdown Driver initialized\n");
}

void storeShutdownValues(){

    //getBit(blah blah)

    BMSval = gioGetBit(BMSPin,BMSNumPin);

    IMDval = gioGetBit(IMDPin,IMDNumPin);

    TSALval = gioGetBit(TSALPin,TSALNumPin);

    BSPDval = gioGetBit(BSPDPin,BSPDNumPin);

    RESETval = gioGetBit(RESETPort,RESETPin);

}
