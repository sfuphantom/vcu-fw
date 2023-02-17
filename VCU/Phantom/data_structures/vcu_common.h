#ifndef _PHANTOM_VCU_COMMON_DEFINES_H_
#define _PHANTOM_VCU_COMMON_DEFINES_H_

#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "os_queue.h"
#include "os_task.h"

#include "stdarg.h"
#include "hal_stdtypes.h"


bool any(uint8_t num, ...);
bool all(uint8_t num, ...);

typedef struct SystemTasks_t{
	TaskHandle_t throttleAgent;
	TaskHandle_t throttleActor;
	TaskHandle_t Logger;
	TaskHandle_t EventHandler;
}SystemTasks_t;

typedef enum eCarEvents{
    BEGIN_OF_EVENTS=0,

    EVENT_APPS1_RANGE_FAULT, 
    EVENT_APPS2_RANGE_FAULT, 
    EVENT_BSE_RANGE_FAULT, 
    EVENT_FP_DIFF_FAULT, 
    EVENT_RESET_CAR,
    EVENT_READY_TO_DRIVE,
    EVENT_TRACTIVE_ON,
    EVENT_TRACTIVE_OFF,
    EVENT_BRAKE_PLAUSIBILITY_CLEARED,
    EVENT_BRAKE_PLAUSIBILITY_FAULT,
    EVENT_UNRESPONSIVE_APPS,

    END_OF_EVENTS
} eCarEvents;
typedef enum eSource{
	FROM_ISR,
	FROM_SCHEDULER
} eSource;

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
    void (*isr_func) (int, void*);
} isr_action_t;

typedef struct _pedal_reading {
    uint16_t bse;
    uint16_t fp2;
    uint16_t fp1;
} pedal_reading_t;

typedef struct {
    TaskFunction_t functionPtr;
    uint32 frequencyMs;
} Task;

typedef struct {
    Task task;
    TaskHandle_t taskHandle;
    QueueHandle_t q;
}PipeTask_t; // common struct for agent/actor tasks

typedef enum {

    // Shutdown Circuit Faults
    IMD_SEVERE_FAULT = (1U),
    BSPD_SEVERE_FAULT = (1U << 1),
    BMS_GPIO_SEVERE_FAULT = (1U << 2),                          // Changed BMS_FAULT to BMS_GPIO_FAULT - jjkhan
    
    // APPS/BSE Faults
    BSE_RANGE_SEVERE_FAULT = (1U << 3),
    APPS1_RANGE_SEVERE_FAULT = (1U << 4),
    APPS2_RANGE_SEVERE_FAULT = (1U << 5),
    APPS_10DIFF_SEVERE_FAULT = (1U << 6),
    BSE_APPS_SIMULTANEOUS_MINOR_FAULT = (1U << 7),
    
    // HV Sensor Faults
    HV_CURRENT_OUT_OF_RANGE_MINOR_FAULT = (1U << 8),            // SEVERE if persistent
    HV_VOLTAGE_OUT_OF_RANGE_MINOR_FAULT = (1U << 9),            // SEVERE if persistent
    HV_APPS_PROPORTION_SEVERE_ERROR = (1U << 10),                // Stays in TRACTIVE_OFF if already in TRACTIVE_OFF state

    // LV Sensor Faults
    LV_CURRENT_OUT_OF_RANGE_MINOR_FAULT = (1U << 11),
    LV_VOLTAGE_OUT_OF_RANGE_MINOR_FAULT = (1U << 12),

    // CAN Faults
    CAN_ERROR_TYPE1 = (1U << 13),                                // Severe Error reported by CAN
    CAN_ERROR_TYPE2 = (1U << 14),                                // Minor Errot Reported by CAN

    // TSAL Faults
    TSAL_WELDED_AIRS_SEVERE_FAULT = (1U << 15),

    // IMD Faults
    IMD_LOW_ISO_SEVERE_FAULT = (1U << 16),
    IMD_SHORT_CIRCUIT_SEVERE_FAULT = (1U << 17),
    IMD_DEVICE_ERR_SEVERE_FAULT = (1U << 18),
    IMD_BAD_INFO_SEVERE_FAULT = (1U << 19),
    IMD_UNDEF_SEVERE_FAULT = (1U << 20),
    IMD_GARBAGE_DATA_SEVERE_FAULT = (1U << 21),

    ALL_FAULTS = (~0U)                              // All 1s to let all faults through the mask
} Fault;

typedef enum {
    SHUTDOWN_CIRCUIT_FAULT_GROUP = IMD_SEVERE_FAULT || BSPD_SEVERE_FAULT || BMS_GPIO_SEVERE_FAULT,
    BSE_APPS_FAULT_GROUP = BSE_RANGE_SEVERE_FAULT || APPS1_RANGE_SEVERE_FAULT || APPS2_RANGE_SEVERE_FAULT || APPS_10DIFF_SEVERE_FAULT || BSE_APPS_SIMULTANEOUS_MINOR_FAULT,
    HV_FAULT_GROUP = HV_CURRENT_OUT_OF_RANGE_MINOR_FAULT || HV_VOLTAGE_OUT_OF_RANGE_MINOR_FAULT || HV_APPS_PROPORTION_SEVERE_ERROR,
    LV_FAULT_GROUP = LV_CURRENT_OUT_OF_RANGE_MINOR_FAULT || LV_VOLTAGE_OUT_OF_RANGE_MINOR_FAULT,
    CAN_FAULT_GROUP = CAN_ERROR_TYPE1 || CAN_ERROR_TYPE2,
    TSAL_FAULT_GROUP = TSAL_WELDED_AIRS_SEVERE_FAULT,
    IMD_FAULT_GROUP = IMD_LOW_ISO_SEVERE_FAULT || IMD_SHORT_CIRCUIT_SEVERE_FAULT || IMD_DEVICE_ERR_SEVERE_FAULT || IMD_BAD_INFO_SEVERE_FAULT || IMD_UNDEF_SEVERE_FAULT || IMD_GARBAGE_DATA_SEVERE_FAULT
} FaultGroup;
#endif
