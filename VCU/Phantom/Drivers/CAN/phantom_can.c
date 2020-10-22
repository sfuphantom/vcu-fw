/*
 * phantom_can.c
 *
 *  Created on: Feb 25, 2020
 *      Author: XingLu Wang
 */
#include "phantom_can.h"
#include "can.h"
#include "stdlib.h"
//#include "sys_main.h"
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

#define D_COUNT 8
uint32 cnt = 0, error = 0, tx_done = 0;
uint8 rx_data1[8] = { 0 };
uint8 *rx_ptr1 = &rx_data1[0];
uint8 rx_data2[8] = {0};
uint8 *rx_ptr2 = &rx_data2[0];
uint8 vcuint = 0;
int timeout;

volatile uint32_t transmitCounter = 0;
volatile uint32_t receiveVCUCounter = 0;

// A structure to represent a queue
struct Queue {
    int front, rear, size;
    unsigned capacity;
    int* array;
};

// function to create a queue
// of given capacity.
// It initializes size of queue as 0
struct Queue* createQueue(unsigned capacity)
{
    struct Queue* queue = (struct Queue*)malloc(
        sizeof(struct Queue));
    queue->capacity = capacity;
    queue->front = queue->size = 0;

    // This is important, see the enqueue
    queue->rear = capacity - 1;
    queue->array = (int*)malloc(
        queue->capacity * sizeof(int));
    return queue;
}




void CANSend()
{
    uint8 tx_data[D_COUNT] = { 'V', 'C', 'U', 'R', 'E', 'A', 'D', 'Y' };
    uint8 *tx_ptr = &tx_data[0];

    for (cnt = 0; cnt < D_COUNT; cnt++)
    {
        canTransmit(canREG1, canMESSAGE_BOX1, tx_ptr); /* transmitting 8 different chunks 1 by 1 */
//        while (timeout != 1U)
//        {
//            timeout = GetTimeout(); /* ... wait until transmit request is through */
//        }

        timeout = 0U;
        tx_done = 0;
        tx_ptr += 8; /* next chunk ...*/
    }
}

void sendVoltageArray() // need to package it so that it sends 4 values in 1 message (10 msgs total)
{
    float voltArr[40] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 5,
                          1, 2, 3, 4, 5, 6, 7, 8, 9, 5,
                          1, 2, 3, 4, 5, 6, 7, 8, 9, 5,
                          1, 2, 3, 4, 5, 6, 7, 8, 9, 5};
    int i;
    for (i = 0; i < 40; i++)
    {
        sendFloat(voltArr[i]);
        delayms(100);
//        for (clk = 0; clk < 5000000; clk++); // Min 5000 or else breaks it\
        // VCU code skips over the time delay
    }
}

void delayms(int ms) {
      volatile unsigned int delayval;
      delayval = ms * 8400;   // 8400 are about 1ms
      while(delayval--);
}

void sendFloat(float val)
{
    // val is MAX 10 for now
    int num = val * 6553.6;

    uint16 tx_data2[D_COUNT] = { 'A', 'B', 'C', 'D' }; //just default values
    tx_data2[3] = num;

    uint16 *tx_ptr2 = &tx_data2[0]; //rename better

    int j = 0;
    for (j = 0; j < D_COUNT; j++)
    {
        canTransmit(canREG1, canMESSAGE_BOX8, tx_ptr2); // technically it wants 8 bit but it works
        tx_ptr2 += 16; /* next chunk ...*/
    }
}

void canMessageNotification(canBASE_t *node, uint32 messageBox)
{


    //this is specifically to receive uint16 (4 blocks)
    //now they are saved as 4 separate integers (converted from hex)
//     if(node==canREG1)
//     {
//      while(!canIsRxMessageArrived(canREG1, canMESSAGE_BOX7));
//      canGetData(canREG1, canMESSAGE_BOX7, rx_ptr1); /* copy to RAM */
//      enqueue(queue, rx_data1[0]);
//      enqueue(queue, rx_data1[1]);
//      enqueue(queue, rx_data1[2]);
//      enqueue(queue, rx_data1[3]);
//      rx_ptr1=&rx_data1[0];
//     }

    /* node 1 - transfer request */
   // if I comment this out, all the extra following things stop
   // if I comment out and d_count is > 8, then only first 8 sends
    if(node==canREG1)
    {
      tx_done=1; /* confirm transfer request */
    }


    /* MB 2 - receive complete */
//     if(node==canREG1)
//     {
//      while(!canIsRxMessageArrived(canREG1, canMESSAGE_BOX2));
//      canGetData(canREG1, canMESSAGE_BOX2, rx_ptr2); /* copy to RAM */
//      rx_ptr2 +=8;
//     }

    /* MB 3 - receive complete */
//    if(node==canREG1)
//    {
//     while(!canIsRxMessageArrived(canREG1, canMESSAGE_BOX3));
//     canGetData(canREG1, canMESSAGE_BOX3, rx_ptr1); /* copy to RAM */
//     rx_ptr1 +=8;
//    }

    /* Note: since only message box 1 is used on both nodes we dont check it here.*/
}


// Queue is full when size becomes
// equal to the capacity
int isFull(struct Queue* queue)
{
    return (queue->size == queue->capacity);
}

// Queue is empty when size is 0
int isEmpty(struct Queue* queue)
{
    return (queue->size == 0);
}

// Function to add an item to the queue.
// It changes rear and size
void enqueue(struct Queue* queue, int item)
{
    if (isFull(queue))
        return;
    queue->rear = (queue->rear + 1)
                  % queue->capacity;
    queue->array[queue->rear] = item;
    queue->size = queue->size + 1;
    printf("%d enqueued to queue\n", item);
}

// Function to remove an item from queue.
// It changes front and size
int dequeue(struct Queue* queue)
{
    if (isEmpty(queue))
        return INT_MIN;
    int item = queue->array[queue->front];
    queue->front = (queue->front + 1)
                   % queue->capacity;
    queue->size = queue->size - 1;
    return item;
}

// Function to get front of queue
int front(struct Queue* queue)
{
    if (isEmpty(queue))
        return INT_MIN;
    return queue->array[queue->front];
}

// Function to get rear of queue
int rear(struct Queue* queue)
{
    if (isEmpty(queue))
        return INT_MIN;
    return queue->array[queue->rear];
}
