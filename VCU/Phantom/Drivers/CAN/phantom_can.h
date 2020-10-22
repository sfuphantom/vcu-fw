/*
 * phantom_can.h
 *
 *  Created on: Feb 25, 2020
 *      Author: XingLu Wang
 */

#ifndef INCLUDE_PHANTOM_CAN_H_
#define INCLUDE_PHANTOM_CAN_H_

#include "stdint.h"

void CANSend();
void sendVoltageArray();
void sendFloat(float val);
void delayms(int ms);

struct Queue* queue;
struct Queue* createQueue(unsigned capacity);
int isFull(struct Queue* queue);
int isEmpty(struct Queue* queue);
void enqueue(struct Queue* queue, int item);
int dequeue(struct Queue* queue);
int front(struct Queue* queue);
int rear(struct Queue* queue);


#endif /* INCLUDE_PHANTOM_CAN_H_ */
