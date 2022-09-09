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
    void (*isr_func) (int, void*);
} isr_action_t;

typedef struct _pedal_reading {
    uint16_t bse;
    uint16_t fp2;
    uint16_t fp1;
} pedal_reading_t;


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
