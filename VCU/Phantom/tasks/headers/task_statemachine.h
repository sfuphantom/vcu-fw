/*
 * task_statemachine.h
 *
 *  Created on: Apr 16, 2020
 *      Author: gabriel
 */

#ifndef PHANTOM_TASKS_TASK_STATEMACHINE_H_
#define PHANTOM_TASKS_TASK_STATEMACHINE_H_


#define NOFAULT         0U
#define FAULT           1U

/*
 * Fault - Bit Number
 *      Can have 32 faults
 *
 *
 */
#define SDC_FAULT       1U
#define BSE_APPS_FAULT  2U
#define HV_LV_FAULT     3U
#define CAN_FAULT       4U

/*

/* SET/RESET */
#define SET         1U
#define ON          SET



/*
 *  SDC Fault types
 */
#define

void vStateMachineTask(void *);


#endif /* PHANTOM_TASKS_TASK_STATEMACHINE_H_ */
