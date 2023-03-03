/*
 * state_machine.c
 *
 *  Created on: Feb 12, 2023
 *      Author: rafgu
 */

#include "state_machine.h"

/* Phantom modules */
#include "ansi_colors.h"

/* Phantom tasks */
#include "task_event_handler.h"
#include "task_throttle.h"
#include "task_logger.h"

static void UpdateStateMachine(void* data);
static State VariousStates(State state, eCarEvents event);
static State TractiveOff(eCarEvents event);
static State TractiveOn(eCarEvents event);
static State Running(eCarEvents event);
static State SevereFault(eCarEvents event);


static SystemTasks_t system_tasks;


/* Public API */

void StateMachineInit(SystemTasks_t tasks)
{
	system_tasks.PedalReadings = tasks.PedalReadings;
	system_tasks.Throttle = tasks.Throttle;
	system_tasks.EventHandler = tasks.EventHandler;
	system_tasks.Logger = tasks.Logger;
}

void NotifyStateMachine(eCarEvents event)
{
	HandleEvent(UpdateStateMachine, event);
}

void NotifyStateMachineFromISR(eCarEvents event)
{
 	char buffer[32];
	sprintf(buffer, "Event occurred: %d", event);
	LogFromISR(UWHT, buffer);

	HandleEventFromISR(UpdateStateMachine, event);
}

void NotifyStateMachineFromTimer(TimerHandle_t timer)
{
	TimerID_t event = (TimerID_t)pvTimerGetTimerID(timer);

	static char buffer[64];
	snprintf(buffer, 64, "(%s:%d): Expired after %dms", pcTimerGetTimerName(timer), event, xTimerGetPeriod(timer));
	LogColor(YEL, buffer);

	HandleEvent(UpdateStateMachine, event);
}


/* Internal implementation */

/* Event callback */
static void UpdateStateMachine(void* data)
{
    #ifdef VCU_SIM_MODE
    LogColor(YEL, "VCU is in Simulation Mode. Events do not affect the state machine.");
	return;
    #endif

	/* static cached values */
	static State state = TRACTIVE_OFF;

	eCarEvents event = *(uint16_t*) data;

	/* Events that have an effect on multiple states */
	State new_state = VariousStates(state, event);

	// no state change occurred; let's try normal state specific events
	if (new_state == state)
	{
		switch(state)
		{
			case TRACTIVE_OFF:

				new_state = TractiveOff(event);

				break;

			case TRACTIVE_ON:

				new_state = TractiveOn(event);

				break;

			case RUNNING:

				new_state = Running(event);

				break;

			case SEVERE_FAULT:
			
				new_state = SevereFault(event);

				break;
			
			default:
				break;
		}

	}

	// update state
	state = new_state;
}


/* State handlers */

static State VariousStates(State state, eCarEvents event)
{
	bool faults = any(6, 
		event == EVENT_APPS1_RANGE_FAULT,
		event == EVENT_APPS2_RANGE_FAULT, 
		event == EVENT_BRAKE_PLAUSIBILITY_FAULT,
		event == EVENT_BSE_RANGE_FAULT,
		event == EVENT_FP_DIFF_FAULT,
		event == EVENT_UNRESPONSIVE_APPS
	);

	if (faults)
	{
		LogColor(RED, "Suspending throttle task.");
		SuspendThrottle(system_tasks.Throttle);
		
		LogColor(RED, "Moving to SevereFault state");
		return SEVERE_FAULT;
	}

	return state;
}

static State TractiveOff(eCarEvents event)
{
	if (event == EVENT_TRACTIVE_ON)
	{
		LogColor(BLU, "Moving from TractiveOff to TractiveOn");
		return TRACTIVE_ON;
	}

	return TRACTIVE_OFF;
}

static State TractiveOn(eCarEvents event)
{
	if (event == EVENT_READY_TO_DRIVE)
	{
		LogColor(GRN, "Moving from TractiveOn to Running");

		LogColor(GRN, "Resuming throttle task.");
		vTaskResume(system_tasks.Throttle);

		return RUNNING;
	}

	return TRACTIVE_ON;
}

static State Running(eCarEvents event)
{
	if (false)
	{
		// ...
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

	return SEVERE_FAULT;
}
