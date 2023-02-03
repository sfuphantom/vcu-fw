#ifndef PHANTOM_TASKS_INTERRUPT_ACTOR_H_
#define PHANTOM_TASKS_INTERRUPT_ACTOR_H_

#include <stdint.h>

void InterruptInit(void);
uint8_t receiveIntEvent(uint32_t delay);

#endif
