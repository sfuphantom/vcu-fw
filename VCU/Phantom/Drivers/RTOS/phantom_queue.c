/*
 * phantom_queue.h
 *
 * Created on: July 15, 2022
 *      Author: Josh Guo
 * 
 * Light-weight wrapper for freertos queue API
 */

#include "phantom_queue.h"

QueueHandle_t Phantom_createQueue(UBaseType_t queue_length, UBaseType_t item_size)
{
    QueueHandle_t qHandle = xQueueCreate(queue_length, item_size);
    return qHandle;
}

QueueSetHandle_t Phantom_createQueueSet(UBaseType_t combined_queue_size)
{
    QueueSetHandle_t qSetHandle = xQueueCreateSet(combined_queue_size);
    return qSetHandle;
}

void Phantom_addToQueueSet(QueueSetHandle_t queue_set_handle, QueueHandle_t queue_handle)
{
    BaseType_t result = xQueueAddToSet(queue_handle, queue_set_handle);
}
