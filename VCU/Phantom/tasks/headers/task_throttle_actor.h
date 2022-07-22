/*
 * task_throttle_actor.h
 *
 *  Created on: July 15, 2022
 *      Author: josh
 */

#ifndef PHANTOM_TASKS_TASK_THROTTLE_ACTOR_H_
#define PHANTOM_TASKS_TASK_THROTTLE_ACTOR_H_

void Task_throttleActorInit(void);
void Task_throttleActorSetReadQueueSet(void* queue_set_handle);
void Task_throttleActorSetSendQueue(void* queue_handle);

#endif
