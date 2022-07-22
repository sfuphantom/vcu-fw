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

/* Task */
void Task_StateMachineInit(void);
void Task_StateMachineSetSendQueue1(QueueHandle_t queueHandle);
void Task_StateMachineSetSendQueue2(QueueHandle_t queueHandle);
void Task_StateMachineSetReadQueue(QueueHandle_t queueHandle);

#endif /* PHANTOM_TASKS_TASK_STATEMACHINE_H_ */
