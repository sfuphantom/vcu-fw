#ifndef PHANTOM_QUEUE_H_
#define PHANTOM_QUEUE_H_

/*
 * phantom_queue.h
 *
 * Created on: July 15, 2022
 *      Author: Josh Guo
 * 
 * Light-weight wrapper for freertos queue API
 */

#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "os_queue.h"

QueueHandle_t Phantom_createQueue(UBaseType_t queue_length, UBaseType_t item_size);
QueueSetHandle_t Phantom_createQueueSet(UBaseType_t combined_queue_size);
void Phantom_addToQueueSet(QueueSetHandle_t queue_set_handle, QueueHandle_t queue_handle);

#define Phantom_createMailBox(item_size)                                        Phantom_createQueue(1, (item_size))
#define Phantom_overwrite                                                       xQueueOverwrite
#define Phantom_overwriteISR                                                    xQueueOverwriteFromISR

#define Phantom_queueSize(queue_handle)                                         (uxQueueMessagesWaiting(queue_handle) + uxQueueSpacesAvailable(queue_handle))
#define Phantom_queueSizeISR(queue_handle)                                      (uxQueueMessagesWaitingFromISR(queue_handle) + uxQueueSpacesAvailable(queue_handle))

#define Phantom_isQueueEmptyISR                                                 xQueueIsQueueEmptyFromISR
#define Phantom_isQueueFullISR                                                  xQueueIsQueueFullFromISR
#define Phantom_itemsInQueue                                                    uxQueueMessagesWaiting
#define Phantom_itemsInQueueISR                                                 uxQueueMessagesWaitingFromISR
#define Phantom_spaceInQueue                                                    uxQueueSpacesAvailable

#define Phantom_peek(queue_handle, buffer_ptr, wait_time_ms)                    xQueuePeek((queue_handle), (buffer_ptr), pdMS_TO_TICKS(wait_time_ms))
#define Phantom_peekISR                                                         xQueuePeekFromISR

#define Phantom_send(queue_handle, item_ptr, wait_time_ms)                      xQueueSendToBack((queue_handle), (item_ptr), pdMS_TO_TICKS(wait_time_ms))
#define Phantom_sendISR(queue_handle, item_ptr, WokenHigherPriorityTask)        xQueueSendFromISR((queue_handle), (item_ptr), WokenHigherPriorityTask)
#define Phantom_sendFront(queue_handle, item_ptr, wait_time_ms)                 xQueueSendToFront((queue_handle), (item_ptr), pdMS_TO_TICKS(wait_time_ms))
#define Phantom_sendFrontISR(queue_handle, item_ptr, WokenHigherPriorityTask)

#define Phantom_receive(queue_handle, buffer_ptr, wait_time_ms)                 xQueueReceive((queue_handle), (buffer_ptr), pdMS_TO_TICKS(wait_time_ms))
#define Phantom_receiveISR                                                      xQueueReceiveFromISR

#define Phantom_resetQueue                                                      xQueueReset

#define Phantom_selectFromQueueSet(qset_handle, wait_time_ms)                   xQueueSelectFromSet((qset_handle), pdMS_TO_TICKS(wait_time_ms))
#define Phantom_selectFromQueueSetISR                                           xQueueSelectFromSetFromISR

#endif
