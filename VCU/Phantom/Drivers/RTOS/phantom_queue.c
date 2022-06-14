
#include "FreeRTOS.h"
#include "os_queue.h"

#include "phantom_queue.h"

static QueueHandle_t queues[NUM_OF_QS];

inline QueueHandle_t Phantom_getQHandle(PhantomQueueIndex index)
{
    return queues[index];
}

QueueHandle_t Phantom_createQ(PhantomQueueIndex index, UBaseType_t qLength, size_t itemSize)
{
    QueueHandle_t newQ = xQueueCreate(qLength, itemSize);
    queues[index] = newQ;

    return newQ;
}


