/*
 * Shutdown.h
 *
 *  Created on: Jan 27, 2021
 *      Author: Rafael Guevara
 */

//3 interrupts (BMS,BSPD,IMD) called back gioNotification() to assert appropriate flag
//1 interrupt (RESET) called back to edgeNotification() to reset faults <-----will have to merge code with IMD Driver's callback function later...
//GPIO interrupts set at falling edge; edge interrupt set at rising edge; all set to high priority

//Halcogen Setup
//1.Enable GIO Drivers
//2.Enable Interrupt for appropriate GIO bit
//3.Select VIM Channel for either Low/High Priority (same priority as GIO bit)
//4.Initialize VIM,GIO, and enableIRQ in the code

#ifndef SHUTDOWN_H
#define SHUTDOWN_H

#include <stdio.h>
#include "sys_common.h"
#include "gio.h"
#include "system.h"
#include "het.h"
#include <Phantom/hardware/launchpad_hw/board_hardware.h>

//***Pins configured with launchpad***//

////triggered on...falling edge
//#define BMSPin gioPORTA
//#define BMSNumPin 0
//
////triggered on...falling edge
//#define IMDPin gioPORTA
//#define IMDNumPin 1
//
////triggered on...falling edge
//#define BSPDPin gioPORTB
//#define BSPDNumPin 2  //(USER Button) Enable gioB_2 in special PinMuxing
//
//#define TSALPin gioPORTB
//#define TSALNumPin 3
//
////triggered on...rising edge
//#define RESETPort hetREG1
//#define RESETPin 20
//#define RESETedge 0


/*Function Headers */

static void resetSignals();

void ShutdownInit();

void print_Shutdownvals();

void storeShutdownValues();

//void gioNotification(gioPORT_t* port,uint32 bit);

//void edgeNotification(hetBASE_t * hetREG,uint32 edge);



#endif
