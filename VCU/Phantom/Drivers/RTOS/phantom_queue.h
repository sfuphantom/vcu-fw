#ifndef _PHANTOM_QUEUE_H_
#define _PHANTOM_QUEUE_H_

/*
    Wrapper queue functions for use in the VCU system
*/

typedef enum {
    StateDirectorQ,
    ThrottleQ,
    CanQ,
    DigitalQ,
    TransmitQ,
    EepromQ,
    NUM_OF_QS
} PhantomQueueIndex;

typedef void* QueueHandle_t;

QueueHandle_t Phantom_getQHandle(PhantomQueueIndex index);
QueueHandle_t Phantom_createQ(PhantomQueueIndex index, unsigned long qLength, size_t itemSize);

#endif