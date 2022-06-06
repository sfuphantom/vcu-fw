// /*
//  * task_throttle.c
//  *
//  *  Created on: Mar 31, 2020
//  *      Author: gabriel
//  *
//  *  Last Modified on: Dec 16, 2020 by jjkhan
//  *
//  */

// #include "FreeRTOS.h"
// #include "FreeRTOSConfig.h"
// #include "gio.h"
// #include "het.h"
// #include "os_queue.h"
// #include "os_semphr.h"
// #include "os_task.h"
// #include "os_timer.h"
// #include "task_config.h"
// #include "Phantom_sci.h"
// #include "os_projdefs.h"
// #include "vcu_data.h"
// #include "task_statemachine_backup.h"
// #include <stdbool.h>
// #include "RGB_LED.h" // needed to drive LEDs for each machine state

// /*
//  *  task_eeprom.c initializes the VCUData structure based on last stored VCU state in eeprom.
//  *      Task can't should not execute its body until initialization has occurred.
//  */

// // ++ Added by jjkhan
// extern volatile uint8_t initializationOccured;
// extern SemaphoreHandle_t vcuKey;
// extern data *VCUDataPtr;
// // -- Added by jjkhan

// static uint16_t faultNumber;
// static State currentState;
// static State newState;
// volatile static bool TSAL_ON;
// volatile static bool RTDS_SET;
// volatile static bool FAULTS;
// volatile static bool HV_CurrentTimerStarted;
// volatile static bool HV_VoltageTimerStarted;
// inline bool isTSAL_ON();
// inline bool isRTDS();
// inline bool anyFaults(void);
// inline bool isSevereFault(void);
// inline bool isMinorFault(void);
// inline State stateUpdate(void);
// /***********************************************************
//  * @function                - vStateMachineTask
//  *
//  * @brief                   - This task evaluates the state of the vehicle and controls any change of state
//  *
//  * @param[in]               - pvParameters
//  *
//  * @return                  - None
//  * @Note                    - None
//  ***********************************************************/

// void vStateMachineTask(void *pvParameters)
// {

//     TickType_t xLastWakeTime; // will hold the timestamp at which the task was last unblocked

//     // Initialize the xLastWakeTime variable with the current time;
//     xLastWakeTime = xTaskGetTickCount();

//     /* ++ Added Timer flags for HV Current and Voltage Fault */

//     // ++ For HV Current
//     HV_CurrentTimerStarted = false;
//     // -- For HV Current

//     // ++ For HV Voltage
//     HV_VoltageTimerStarted = false;
//     // -- For HV Voltage

//     /* -- Added Timer flags for HV Current and Voltage Fault */

//     while (true)
//     {
//         if (initializationOccured)
//         {
//             if (TASK_PRINT)
//             {
//                 UARTSend(PC_UART, "STATE MACHINE UPDATE TASK\r\n");
//             }

//             /*********************** STATE MACHINE EVALUATION ***********************************/

//             currentState = VCUDataPtr->vcuState; // Get current State
//             faultNumber = 0;                     // clear faultNumber

//             TSAL_ON = isTSAL_ON();
//             RTDS_SET = isRTDS();
//             FAULTS = anyFaults();

//             if (currentState == TRACTIVE_OFF)
//             {
//                 RGB_LED_drive(RGB_CYAN);

//                 if (STATE_PRINT)
//                 {
//                     UARTSend(PC_UART, "********TRACTIVE_OFF********");
//                 }

//                 if (RTDS_SET || FAULTS)
//                 {
//                     newState = SEVERE_FAULT;
//                 }
//                 else if (TSAL_ON && !FAULTS && !RTDS_SET)
//                 {
//                     newState = TRACTIVE_ON; //No faults AND TSAL is on AND RTDS not set
//                 }

//             }
//             else if (currentState == TRACTIVE_ON)
//             {
//                  RGB_LED_drive(RGB_MAGENTA);

//                  if (STATE_PRINT)
//                  {
//                      UARTSend(PC_UART, "********TRACTIVE_ON********");
//                  }
//                 if (TSAL_ON && !RTDS_SET && !FAULTS)
//                 {
//                     newState = TRACTIVE_ON;

//                 }
//                 else if (TSAL_ON && RTDS_SET && FAULTS)
//                 {
//                     // Update state depending on severity of fault
//                     newState = stateUpdate();
//                 }
//                 else if (TSAL_ON && RTDS_SET && !FAULTS)
//                 {
//                     newState = RUNNING;

//                 }else if (TSAL_ON && !RTDS_SET && FAULTS){

//                     newState = stateUpdate();
                    
//                 }else if (!TSAL_ON && !RTDS_SET && !FAULTS){
//                     newState = TRACTIVE_OFF;

//                 }else{
//                     newState = SEVERE_FAULT;
//                 }

//             }
//             else if (currentState == RUNNING)
//             {

//                 RGB_LED_drive(RGB_GREEN);

//                 if (FAULTS)
//                 {
//                     // Update state depending on severity of fault
//                     newState = stateUpdate();
//                 }
//                 else
//                 {
//                     if (RTDS_SET && TSAL_ON)
//                     {
//                         newState = RUNNING;
//                     }
//                     else if(!RTDS_SET && TSAL_ON)
//                     {
//                         newState = TRACTIVE_ON;
//                     }else if(!TSAL_ON && RTDS_SET)
//                     {
//                         newState= SEVERE_FAULT;
//                     }
//                 }

//             }
//             else if (currentState == MINOR_FAULT)
//             {
//                 if (STATE_PRINT)
//                 {
//                     UARTSend(PC_UART, "********MINOR_FAULT********");
//                 }
                
//                 RGB_LED_drive(RGB_YELLOW);

//                 if (FAULTS)
//                 {
//                     // Update state depending on severity of fault
//                     faultNumber = faultLocation();
//                     if(isSevereFault()){
//                         newState = SEVERE_FAULT;
//                     }

//                 }else{
//                     // no FAULTS were found; i.e. faults were fixed
//                     if (RTDS_SET && TSAL_ON)
//                     {
//                         newState = RUNNING; // RTDS =1 and HV present
//                     }
//                     else if (!RTDS_SET && TSAL_ON)
//                     {
//                         newState = TRACTIVE_ON; // RTDS = 0 but HV present
//                     }
//                     else
//                     {
//                         newState = TRACTIVE_OFF; // RTDS = 0 and HV not present
//                     }
//                     // Reset the HV timer_started flags as the HV fault was fixed before timeout
//                     if (HV_CurrentTimerStarted || HV_VoltageTimerStarted)
//                     {
//                         xTimerStop(xTimers[2], pdMS_TO_TICKS(5));
//                         xTimerStop(xTimers[3], pdMS_TO_TICKS(5));
//                         HV_CurrentTimerStarted = 0;
//                         HV_VoltageTimerStarted = 0;
//                     }
//                 }
//             }
//             else if (currentState == SEVERE_FAULT)
//             {

//                 if (STATE_PRINT)
//                 {
//                     UARTSend(PC_UART, "********SEVERE_FAULT********");
//                 }

//                 RGB_LED_drive(RGB_RED);

//                 if (FAULTS)
//                 {
//                     newState = SEVERE_FAULT;
//                 }
//                 else
//                 {
//                     if(!RTDS_SET){
//                         newState = TRACTIVE_OFF;
//                     }else{
//                         newState = SEVERE_FAULT;
//                     }
//                 }
//             }

//             // Update VCUData with newState

//             if(xSemaphoreTake(vcuKey, pdMS_TO_TICKS(2))){

//                 // vcu Key taken
//                 VCUDataPtr->vcuState = newState; // update newState
//                 xSemaphoreGive(vcuKey); // Return vcu key
//             }

//             if (STATE_PRINT)
//             {
//                 UARTSend(PC_UART, "\r\n");
//             }

//             // Block Task
//             vTaskDelayUntil(&xLastWakeTime, STATE_MACHINE_TASK_PERIOD_MS);
//         }
//         else
//         {
//             vTaskDelayUntil(&xLastWakeTime, STATE_MACHINE_TASK_PERIOD_MS);
//         }
//     }
// }

// /* Helper Function definitions */

// int checkSDC(void)
// {
//     if (VCUDataPtr->DigitalVal.IMD_FAULT || VCUDataPtr->DigitalVal.BSPD_FAULT
//             || VCUDataPtr->DigitalVal.BMS_GPIO_FAULT)
//     {
//         return FAULT;
//     }
//     return NOFAULT;

// }

// int checkIMD(void)
// {
//     if (VCUDataPtr->DigitalVal.IMD_LOW_ISO_FAULT
//             || VCUDataPtr->DigitalVal.IMD_SHORT_CIRCUIT_FAULT
//             || VCUDataPtr->DigitalVal.IMD_DEVICE_ERR_FAULT
//             || VCUDataPtr->DigitalVal.IMD_BAD_INFO_FAULT
//             || VCUDataPtr->DigitalVal.IMD_UNDEF_ERR
//             || VCUDataPtr->DigitalVal.IMD_GARBAGE_DATA_FAULT)
//     {
//         return FAULT;
//     }
//     return NOFAULT;
// }

// int checkBSE_APPS(void)
// {
//     if (VCUDataPtr->DigitalVal.BSE_SEVERE_RANGE_FAULT
//             || VCUDataPtr->DigitalVal.APPS1_SEVERE_RANGE_FAULT
//             || VCUDataPtr->DigitalVal.APPS2_SEVERE_RANGE_FAULT
//             || VCUDataPtr->DigitalVal.APPS_SEVERE_10DIFF_FAULT
//             || VCUDataPtr->DigitalVal.BSE_APPS_MINOR_SIMULTANEOUS_FAULT)
//     {
//         return FAULT;
//     }
//     return NOFAULT;
// }

// int CheckHVLVSensor(void)
// {
//     if (VCUDataPtr->DigitalVal.HV_CURRENT_OUT_OF_RANGE
//             || VCUDataPtr->DigitalVal.HV_VOLTAGE_OUT_OF_RANGE_FAULT
//             || VCUDataPtr->DigitalVal.LV_CURRENT_OUT_OF_RANGE
//             || VCUDataPtr->DigitalVal.LV_VOLTAGE_OUT_OF_RANGE
//             || VCUDataPtr->DigitalVal.APPS_PROPORTION_ERROR)
//     {
//         return FAULT;
//     }
//     return NOFAULT;
// }

// int checkCAN(void)
// {
//     if (VCUDataPtr->DigitalVal.CAN_ERROR_TYPE1
//             || VCUDataPtr->DigitalVal.CAN_ERROR_TYPE2)
//     {
//         return FAULT;
//     }
//     return NOFAULT;
// }

// int checkTSAL_FAULTS(){
//     if(VCUDataPtr->DigitalVal.TSAL_WELDED_AIRS_FAULT){
//         return FAULT;
//     }
//     return NOFAULT;
// }

// inline bool isRTDS(void)
// {
//     if (VCUDataPtr->DigitalVal.RTDS)
//     {
//         return SET;
//     }
//     return !SET;
// }

// inline bool isTSAL_ON(void)
// {
//     if (VCUDataPtr->DigitalVal.TSAL_ON)
//     {
//         return ON;
//     }
//     return !ON;
// }



// /***********************************************************
//  * @function                - faultLocation
//  *
//  * @brief                   - This function is a helper function for vStateMachineTask, its used to find the faulty subsystems.
//  *
//  * @param[in]               - void
//  *
//  * @return                  - 32-bit value where each bit corresponds to if there is a fault in that subsystem.
//  * @Note                    -  A '1' indicates fault(s), a '0' indicates no fault
//  *
//  *                             Bit 0 =  Shutdown Circuit has fault.
//  *                             Bit 1 =  BSE or APPS has fault
//  *                             Bit 2 =  HV or LV fault sensed.
//  *                             Bit 3 =  Fault sent over CAN bus
//  *                             Bit 4 =  IMD has a fault.
//  *
//  ***********************************************************/

// uint16_t faultLocation(void)
// {

//     uint16_t systemFaultIndicator = 0;

//     // Check if fault in Shutdown Circuit
//     if (checkSDC())
//     {
//         systemFaultIndicator |= 1U << SDC_FAULT;
//     }
//     // Check if fault in APPS
//     if (checkBSE_APPS())
//     {
//         systemFaultIndicator |= 1U << BSE_APPS_FAULT;
//     }
//     // Check if fault in HV or LV
//     if (CheckHVLVSensor())
//     {
//         systemFaultIndicator |= 1U << HV_LV_FAULT;
//     }
//     // Check if fault from CAN
//     if (checkCAN())
//     {
//         systemFaultIndicator |= 1U << CAN_FAULT;
//     }

//     // Check if fault from IMD
//     if (checkIMD())
//     {
//         systemFaultIndicator |= 1U << IMD_SYSTEM_FAULT;
//     }

//     if(checkTSAL_FAULTS()){
//         systemFaultIndicator |= 1U << TSAL_FAULTS;
//     }
//     return systemFaultIndicator;

// }

// /***********************************************************
//  * @function                - anyERRORS
//  *
//  * @brief                   - This function is a helper function for vStateMachineTask, its used to check if there are any faults, regardless if severe or minor
//  *
//  * @param[in]               - void
//  *
//  * @return                  - 1 or 0
//  * @Note                    -  A '1' indicates fault(s), a '0' indicates no fault
//  *
//  ***********************************************************/

// inline bool anyFaults(void)
// {
//     if (checkSDC() || checkCAN() || checkIMD() || checkBSE_APPS()
//             || CheckHVLVSensor() || checkTSAL_FAULTS())
//     {
//         return FAULT;
//     }
//     return NOFAULT;
// }


// inline bool isSevereFault(void){

//     bool isSevere = false;

//     // Shutdown Circuit Fault or IMD Fault -> its Severe so don't need to check other faults
//     if (faultNumber & (1U << SDC_FAULT) || faultNumber & (1U << IMD_SYSTEM_FAULT))
//     {
//         //temp_state = SEVERE_FAULT;
//         isSevere = true;
//         return isSevere;
//     }

//     // Check BSE APPS Faults
//     if (faultNumber & (1U << BSE_APPS_FAULT))
//     {
//       // Either BSE or APPS Fault
//         if (!(VCUDataPtr->DigitalVal.BSE_APPS_MINOR_SIMULTANEOUS_FAULT))
//         {
//             // if the fault isn't the Minor Fault, then set SEVERE_FAULT, don't need to check for minor
//             //temp_state = SEVERE_FAULT;
//             isSevere = true;
//             return isSevere;
//         }
//     }
//     // Check HV Range Faults and set flags iff this isnt the first time (i.e. timers already started.)
//     if (faultNumber & (1U << HV_LV_FAULT))
//     {
//         // Check for SEVERE Faults Right now
//         if (HV_CurrentTimerStarted || HV_VoltageTimerStarted)
//         {
//             if (VCUDataPtr->DigitalVal.HV_VOLTAGE_OUT_OF_RANGE_FAULT)
//             {
//                 if (HV_VOLTAGE_TIMER_EXPIRED)
//                 {
//                     // Timer expired
//                     isSevere = true;
//                     HV_VoltageTimerStarted = false; // Reset Timer
//                     HV_VOLTAGE_TIMER_EXPIRED = false;
//                     return isSevere;
//                 }else{
//                     // timer not expired yet
//                     isSevere = false;
//                 }
//             }

//             if (VCUDataPtr->DigitalVal.HV_CURRENT_OUT_OF_RANGE)
//             {
//                 if (HV_CURRENT_TIMER_EXPIRED)
//                 {
//                     //timer Expired
//                     isSevere = true;
//                     HV_CurrentTimerStarted = false; // Reset Timer
//                     HV_CURRENT_TIMER_EXPIRED = false;
//                     return isSevere;

//                 }else{
//                     // timer not expired yet
//                     isSevere = false;
//                 }
//             }

//         }
//         if(VCUDataPtr->DigitalVal.APPS_PROPORTION_ERROR
//                             && currentState==RUNNING)
//         {
//             isSevere = true;
//             return isSevere;
//         }

//     }

//     if(faultNumber & (1U << TSAL_FAULTS)){
//         if(currentState==RUNNING || currentState==TRACTIVE_ON ){
//             isSevere = true;
//             return isSevere;
//         }
//     }

//     // Check CAN Severe Faults
//     if (faultNumber & (1U << CAN_FAULT))
//     {
//         // Either CAN Message indicates a severe fault (TYPE1 ERROR) or a minor fault (TYPE2 ERROR)
//         if (VCUDataPtr->DigitalVal.CAN_ERROR_TYPE1)
//         { // Severe Fault message from CAN, so don't need to check other faults, change currentState and yield task
//             isSevere = true;
//             return isSevere;
//         }
//     }

//     return isSevere;

// }

// inline bool isMinorFault(void){

//     bool isMinor = false;
//     // Check for MINOR Faults

//     if ((faultNumber & (1U << HV_LV_FAULT)))
//     {

//         if (VCUDataPtr->DigitalVal.HV_VOLTAGE_OUT_OF_RANGE_FAULT)
//         {
//             if(!HV_VoltageTimerStarted){
//                 // Start Timer
//                 HV_VOLTAGE_TIMER_EXPIRED = false;
//                 if (xTimerStart(xTimers[3], 0) != pdPASS)
//                 {
//                     for (;;)
//                         ;
//                 }
//                 else
//                 {
//                     HV_CurrentTimerStarted = true;
//                     //temp_state = MINOR_FAULT;
//                     isMinor = true;
//                     return isMinor;
//                 }
//             }else{

//                 // Timer already started
//                 // Need to decide what to do here.
//                 isMinor = false;

//             }
//         }
//         if (VCUDataPtr->DigitalVal.HV_CURRENT_OUT_OF_RANGE)
//         {
//             if(!HV_CurrentTimerStarted){
//                 // Start Timer
//                 HV_CURRENT_TIMER_EXPIRED = false;
//                 if (xTimerStart(xTimers[2],0) != pdPASS)
//                 {
//                     for (;;)
//                         ;
//                 }
//                 else
//                 {
//                     HV_VoltageTimerStarted = true;
//                     //temp_state = MINOR_FAULT;
//                     isMinor = true;
//                     return isMinor;
//                 }
//             }else{

//                 // Timer already started
//                 // Need to decide what to do here.
//                 isMinor = false;
//             }
//         }
//         if (VCUDataPtr->DigitalVal.LV_CURRENT_OUT_OF_RANGE || VCUDataPtr->DigitalVal.LV_VOLTAGE_OUT_OF_RANGE )
//         {
//             isMinor = true;
//             return isMinor;
//         }
//     }

//     if(faultNumber & (1U << BSE_APPS_FAULT)){
//         if (VCUDataPtr->DigitalVal.BSE_APPS_MINOR_SIMULTANEOUS_FAULT)
//             {
//                     isMinor = true;
//                     return isMinor;
//             }
//     }


//     if ((faultNumber & (1U << CAN_FAULT)))
//     {
//         if (VCUDataPtr->DigitalVal.CAN_ERROR_TYPE2)
//         {
//             isMinor = true;
//             return isMinor;
//         }
//     }

//     return isMinor;
// }

// inline State stateUpdate(void){
//     faultNumber = faultLocation();
//     if(isSevereFault()){
//         newState = SEVERE_FAULT;
//     }else if(isMinorFault()){
//         newState = MINOR_FAULT;
//     }
//     return newState;
// }
