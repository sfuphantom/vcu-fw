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
    while (qHandle == NULL);
    return qHandle;
}

QueueSetHandle_t Phantom_createQueueSet(UBaseType_t combined_queue_size)
{
    QueueSetHandle_t qSetHandle = xQueueCreateSet(combined_queue_size);
    while (qSetHandle == NULL);
    return qSetHandle;
}

BaseType_t Phantom_send(QueueHandle_t queue_handle, void* item_ptr, TickType_t wait_time_ms)
{
    UBaseType_t queue_size = uxQueueMessagesWaiting(queue_handle) + uxQueueSpacesAvailable(queue_handle);
    return (queue_size > 1) ? xQueueSendToBack(queue_handle, item_ptr, pdMS_TO_TICKS(wait_time_ms)) : xQueueOverwrite(queue_handle, item_ptr);
}

BaseType_t Phantom_sendFront(QueueHandle_t queue_handle, void* item_ptr, TickType_t wait_time_ms)
{
    UBaseType_t queue_size = uxQueueMessagesWaiting(queue_handle) + uxQueueSpacesAvailable(queue_handle);
    return (queue_size > 1) ? xQueueSendToFront(queue_handle, item_ptr, pdMS_TO_TICKS(wait_time_ms)) : xQueueOverwrite(queue_handle, item_ptr);
}

BaseType_t Phantom_sendISR(QueueHandle_t queue_handle, void* item_ptr, BaseType_t* WokenHigherPriorityTask)
{
    UBaseType_t queue_size = uxQueueMessagesWaitingFromISR(queue_handle) + uxQueueSpacesAvailable(queue_handle);
    return (queue_size > 1) ? xQueueSendFromISR(queue_handle, item_ptr, WokenHigherPriorityTask) : xQueueOverwriteFromISR(queue_handle, item_ptr, WokenHigherPriorityTask);
}

BaseType_t Phantom_sendFrontISR(QueueHandle_t queue_handle, void* item_ptr, BaseType_t* WokenHigherPriorityTask)
{
    UBaseType_t queue_size = uxQueueMessagesWaitingFromISR(queue_handle) + uxQueueSpacesAvailable(queue_handle);
    return (queue_size > 1) ? xQueueSendToFrontFromISR(queue_handle, item_ptr, WokenHigherPriorityTask) : xQueueOverwriteFromISR(queue_handle, item_ptr, WokenHigherPriorityTask);
}

void Phantom_addToQueueSet(QueueSetHandle_t queue_set_handle, QueueHandle_t queue_handle)
{
    BaseType_t result = xQueueAddToSet(queue_handle, queue_set_handle);
    while (result == pdFAIL); // maybe also add an error message saying "queue is already in another set"
}
