/*
 * task_statemachine.h
 *
 *  Created on: Apr 16, 2020
 *      Author: gabriel
 *
 *
 *  Last Modified on: Dec 16, 2020 -> By jjkhan
 */

#ifndef PHANTOM_TASKS_TASK_STATEMACHINE_H_
#define PHANTOM_TASKS_TASK_STATEMACHINE_H_

/*
 *  Header files used by state machine task.
 */
#include "board_hardware.h"   // contains hardware defines for specific board used (i.e. VCU or launchpad)



#define NOFAULT         0U
#define FAULT           1U

/*
 *  Possible Fault Locations - Bit Number
 *
 */
#define SDC_FAULT              1U
#define BSE_APPS_FAULT         2U
#define HV_LV_FAULT            3U
#define CAN_FAULT              4U
#define IMD_SYSTEM_FAULT       5U

/*

/* SET/RESET */
#define SET         1U
#define ON          SET


void vStateMachineTask(void *);


extern State state; // Moved it from task_statemachine source file - jjkhan

/* Task Helper Functions. */

static int checkSDC(void);
static int checkIMD(void);
static int checkBSE_APPS(void);
static int CheckHVLVSensor(void);
static int checkCAN(void);
static int isRTDS(void);
static int isTSAL_ON(void);
static uint32_t faultLocation(void);
static int anyFaults(void);
static State getNewState(State currentState, uint32_t faultNumber, uint8_t* timer1_started, TickType_t* timer1, uint8_t* timer2_started, TickType_t* timer2, TickType_t timer_threshold);


#endif /* PHANTOM_TASKS_TASK_STATEMACHINE_H_ */
