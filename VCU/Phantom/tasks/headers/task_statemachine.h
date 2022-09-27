/*
 * task_statemachine.h
 *
 *  Created on: Apr 16, 2020
 *      Author: gabriel, jjkhan, josh
 *
 *
 *  Last Modified on: Dec 30, 2020 -> By jjkhan
 */

#ifndef PHANTOM_TASKS_TASK_STATEMACHINE_H_
#define PHANTOM_TASKS_TASK_STATEMACHINE_H_

#include "vcu_common.h"

State StateMachine_getState();

/* Task */
void Task_StateMachineInit(void);

#endif /* PHANTOM_TASKS_TASK_STATEMACHINE_H_ */
