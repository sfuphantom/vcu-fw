/*
 * state_machine.c
 *
 *  Created on: Feb 12, 2023
 *      Author: rafgu
 */




#include "state_machine.h"

#include "task_event_handler.h"
#include "task_logger.h"
#include "ansi_colors.h"

static void UpdateStateMachine(void* data);

static SystemTasks_t system_tasks;

void StateMachineInit(SystemTasks_t tasks)
{
	system_tasks.throttleAgent = tasks.throttleAgent;
	system_tasks.throttleActor = tasks.throttleActor;
	system_tasks.EventHandler = tasks.EventHandler;
	system_tasks.Logger = tasks.Logger;
}

void NotifyStateMachine(eCarEvents event)
{
	HandleToFront(UpdateStateMachine, event, FROM_SCHEDULER);
}

void NotifyStateMachineFromISR(eCarEvents event)
{
	HandleToFront(UpdateStateMachine, event, FROM_ISR);
}

void NotifyStateMachineFromTimer(TimerHandle_t timer)
{
	TimerID_t event = (TimerID_t)pvTimerGetTimerID(timer);

	static char buffer[64];
	snprintf(buffer, 64, "(%s:%d): Expired after %dms", pcTimerGetTimerName(timer), event, xTimerGetPeriod(timer));
	LogColor(YEL, buffer);

	HandleToFront(UpdateStateMachine, event, FROM_SCHEDULER);
}

static State TractiveOff(eCarEvents event)
{
	if (event == EVENT_TRACTIVE_ON)
	{
		LogColor(BLU, "Moving from TractiveOff to TractiveOn");
		return TRACTIVE_ON;
	}
	else
	{
		Log("Event is irrelevant. Staying at TractiveOff");
	}


	return TRACTIVE_OFF;
}

static State TractiveOn(eCarEvents event)
{
	if (event == EVENT_READY_TO_DRIVE)
	{
		LogColor(GRN, "Moving from TractiveOn to Running");

		LogColor(GRN, "Resuming throttle actor.");
		vTaskResume(system_tasks.throttleActor);

		return RUNNING;
	}
	else
	{
		Log("Event is irrelevant. Staying at TractiveOn");
	}

	return TRACTIVE_ON;
}

static State Running(eCarEvents event)
{
	bool agentFault = any(4, EVENT_APPS1_RANGE_FAULT, EVENT_APPS2_RANGE_FAULT, EVENT_BSE_RANGE_FAULT, EVENT_FP_DIFF_FAULT);
	if (agentFault)
	{
		LogColor(RED, "Moving from Running to SevereFault");

		LogColor(RED, "Suspending throttle actor.");
		vTaskSuspend(system_tasks.throttleActor);	

		return SEVERE_FAULT;
	}
	else
	{
		Log("Event is irrelevant. Staying at Running");
	}

	return RUNNING;
}

static State SevereFault(eCarEvents event)
{
	if (event == EVENT_RESET_CAR)
	{
		LogColor(CYN, "Moving from SevereFault to TractiveOff");
		return TRACTIVE_OFF;
	}
	else
	{
		Log("Event is irrelevant. Staying at SevereFault");
	}

	return SEVERE_FAULT;
}


/* Event Handler */
static void UpdateStateMachine(void* data)
{
	/* static cached values */
	static State state = TRACTIVE_OFF;

	eCarEvents event = *(uint16_t*) data;

 	char buffer[32];
	sprintf(buffer, "Event occurred: %d", event);
	Log(buffer);

	switch(state)
	{
		case TRACTIVE_OFF:
			state = TractiveOff(event);
			break;

		case TRACTIVE_ON:
			state = TractiveOn(event);
			break;

		case RUNNING:
			state = Running(event);
			break;

		case SEVERE_FAULT:
			state = SevereFault(event);
			break;
	}
}

