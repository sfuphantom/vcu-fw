/*
 * state_machine.h
 *
 *  Created on: Feb 12, 2023
 *      Author: rafgu
 */

#ifndef PHANTOM_DRIVERS_STATE_MACHINE_STATE_MACHINE_H_
#define PHANTOM_DRIVERS_STATE_MACHINE_STATE_MACHINE_H_

#include "vcu_common.h"
#include "phantom_timer.h"


void StateMachineInit(SystemTasks_t tasks);

void NotifyStateMachine(eCarEvents event);
void NotifyStateMachineFromISR(eCarEvents event);
void NotifyStateMachineFromTimer(TimerHandle_t timer);


#endif /* PHANTOM_DRIVERS_STATE_MACHINE_STATE_MACHINE_H_ */
