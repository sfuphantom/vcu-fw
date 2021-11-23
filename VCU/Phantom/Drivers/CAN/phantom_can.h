/*
 * phantom_can.h
 *
 *  Created on: Nov 20, 2021
 *      Author: Rafael Guevara
 */

#ifndef PHANTOM_DRIVERS_CAN_PHANTOM_CAN_H_
#define PHANTOM_DRIVERS_CAN_PHANTOM_CAN_H_

#include "can.h"
#include "sys_vim.h"
#include "board_hardware.h"
#include "sys_core.h"


#include "Phantom_sci.h"
//#include "can_reg.h"

void phantom_can_init();

void sendVCUState(int,int,int,int);

void delayms(uint16_t ms);

#endif /* PHANTOM_DRIVERS_CAN_PHANTOM_CAN_H_ */
