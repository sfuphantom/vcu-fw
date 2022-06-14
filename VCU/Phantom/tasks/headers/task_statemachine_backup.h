// /*
//  * task_statemachine.h
//  *
//  *  Created on: Apr 16, 2020
//  *      Author: gabriel
//  *
//  *
//  *  Last Modified on: Dec 30, 2020 -> By jjkhan
//  */

// #ifndef PHANTOM_TASKS_TASK_STATEMACHINE_H_
// #define PHANTOM_TASKS_TASK_STATEMACHINE_H_

// /*
//  *  Header files used by state machine task.
//  */
// #include "board_hardware.h"   // contains hardware defines for specific board used (i.e. VCU or launchpad)



// #define NOFAULT         0U
// #define FAULT           1U

// /*
//  *  Possible Fault Locations - Bit Number
//  *
//  */
// #define SDC_FAULT              0U
// #define BSE_APPS_FAULT         1U
// #define HV_LV_FAULT            2U
// #define CAN_FAULT              3U
// #define IMD_SYSTEM_FAULT       4U
// #define TSAL_FAULTS            5U



// /* SET RESET */
// #define SET         1U
// #define ON          SET

// extern State state; // Moved it from task_statemachine source file - jjkhan

// // Defined in main.c, used in the task for HV Current and Voltage Out of range sensors.
// extern TimerHandle_t xTimers[];
// extern volatile bool HV_VOLTAGE_TIMER_EXPIRED;
// extern volatile bool HV_CURRENT_TIMER_EXPIRED;

// /* Task Helper Functions. */

// int checkSDC(void);
// int checkIMD(void);
// int checkBSE_APPS(void);
// int CheckHVLVSensor(void);
// int checkCAN(void);
// int checkTSAL_FAULTS(void);
// uint16_t faultLocation(void);

// /* Task */
// void vStateMachineTask(void *);





// #endif /* PHANTOM_TASKS_TASK_STATEMACHINE_H_ */
