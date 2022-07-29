#ifndef _PHANTOM_VCU_COMMON_DEFINES_H_
#define _PHANTOM_VCU_COMMON_DEFINES_H_

/**
 * @brief Machine states for the VCU.
 *          TRACTIVE_OFF = MAGENTA,
 *          TRACTIVE_ON  = CYAN,
 *          RUNNING      = RUNNING,
 *          MINOR_FAULT  = YELLOW,
 *          SEVERE_FAULT = RED
 */
typedef enum _state {
    TRACTIVE_OFF, 
    TRACTIVE_ON, 
    RUNNING, 
    MINOR_FAULT,
    SEVERE_FAULT
} State;

typedef enum _isr_id {
    RTDS_INTERRUPT,
    ISR_ID_ERROR
} isr_id_t;

typedef struct _isr_action {
    isr_id_t id;
    BaseType_t (*isr_func) (int state, ...);
} isr_action_t;

#endif