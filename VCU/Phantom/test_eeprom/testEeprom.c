/*
 * testEeprom.c
 *
 *  Created on: Mar 7, 2021
 *      Author: junaidkhan
 */
#include "phantom_freertos.h"
#include "testEeprom.h"
//++ For testing Purposes - simulating vehicle state change and testing eeprom task.


/*
 *
 * Note: Some of the fault conventions are not intuitive, i.e. some places '1' means healthy/no-fault and some places '1' means 'flag set' - Will fix this later.
 *
 *   The following Task is introducing a software fault by toggling one of the FAULT flags available
 *      I'm toggling the TSAL line, which is a flag set when the AIRs are opened due to a fault, resulting in de-activation of tractive system;
 *          For TSAL_FAULT, '1' means TSAL healthy, i.e. no fault, if '0' -> there is a fault, state = tractive system off
 *
 *
 *   The State machine Flow for now:
 *      At power-on --> In TRACTIVE_SYSTEM_OFF:
 *                          If BMS, IMD, BSPD, TSAL, BSE are healthy --> VCU state changes to TRACTIVE_SYSTEM_ON, (note: car still not moving)
 *      In TRACTIVE_SYSTEM_ON state:
 *                      If Ready To Drive Set (RTDS) is set --> VCU state changes to RUNNING, car is now moving..
 *      In RUNNING state:
 *                      If the BMS, IMD, BSPD, TSAL, BSE are healthy and RTDS is set --> Continue in RUNNING state
 *                      If the BMS, IMD, BSPD, TSAL, BSE are healthy and RTDS no set --> VCU state changes to TRACTIVE_SYSTEM_ON, car not moving, i.e. slowing down to halt.
 *                      If RTDS is set and BMS or IMD or BSPD or TSAL signal are fault -->  VCU state changes to FAULT state -> Car still moving or do we slow down?
 *      In FAULT state:
 *                      If BSE is healthy, then you will be stuck in FAULT state -> we need to implement how we get out of FAULT state.
 *                      If BSE is unhealthy, the VCU state changes to TRACTIVE_SYSTEM_OFF - i.e. car is shutdown.
 *
 *
 *      Based on the state machine set up right now, the following task will introduce a fault in one of the sub-system after power-on, which will place the
 *          VCU in FAULT state. Once in FAULT state, we need to introduct BSE fault, which will cause the shutdown, moving the VCU to TRACTIVE_SYSTEM_OFF state, which is the default state
 *              of VCU at power on.
 *
 *      The testEEprom is using the TSAL_FAULT for entering the FAULT state, but you could use any from BMS, IMD, BSPD, TSAL. It toggles the
 *          fault line every second. This lets you visualize that eeprom bank7 is getting updated periodically as VCU data structure value(s) are getting updated. -> Go to Memory window and open 0xF020 0000
 *      There is no implementation to get out FAULT state unless the BSE fault line is set, i.e. BSE signals a fault -> needs implementation.
 *      For now, after 10-seconds I introduce BSE fault, which will move VCU out of FAULT state and into TRACTIVE_SYSTEM_OFF -> basically a reset.
 *      Once you've set BSE fault, you can check the VCU data structure contents and then reset the board. On power-on, you should note that based on the value stored in
 *               POWER_FAILURE_FLAG of VCU data strcuture, the VCU data structure will either get initialized with values from eeprom memory or the default values.
 *                  If POWER_FAILURE_FLAG = 0xFF --> last shutdown not graceful--> load VCU data structure with data in eeprom memory.
 *                  If POWER_FAILURE_FLAG = 0x00 --> last shutdown graceful  --> Load VCU data structure with default values.
 *
 *
 *      Power on-> Observe how eeprom bank is getting updated periodically as VCU data structure values get updated.
 *              Once in FAULT state, the terminal will show FAULT state -> Reset the board.
*               On Power-on:
*                   Check eeprom last saved VCU data structure values - especially the last 2-data bytes (stores a flag which indicates if last saved VCUData was in a faulty state or not). Place VCUData in watch-window
*                       Next, Set break-point at the start of '(initializationOccured) if-block' -> this block will be entered after VCUData intialized at power-up by either default values or by last-updated eeprom bank values.
*                           Compare values of the VCUData in watch window with the eeprom block values.
*                               If last 2bytes of eeprom data = 0xFF, last saved VCU state was faulty, load VCUData with eeprom values.
*                               If last 2byte of eeprom data = 0x00, last saved VCU state was healthy, load VCUData with default values.
*
*
 *
 */
void testEeprom(void *p){
    TickType_t mylastTickCount;
    mylastTickCount = xTaskGetTickCount();
    while(1){
        if(xSemaphoreTake(vcuKey, pdMS_TO_TICKS(10))){
            VCUDataPtr->DigitalVal.TSAL_FAULT ^= (1<<0);       // Toggle Bit-0
            xSemaphoreGive(vcuKey);
        }
        vTaskDelayUntil(&mylastTickCount,pdMS_TO_TICKS(500));  // Every 500ms you toggle the shutdown signal - to check if eeprom is updated or not.

        if(VCUDataPtr->DigitalVal.TSAL_FAULT==0){  // TSAL_FAULT
            if(xSemaphoreTake(vcuKey, pdMS_TO_TICKS(100))){
                VCUDataPtr->DigitalVal.BSE_FAULT = 1;  // Send BSE fault
                xSemaphoreGive(vcuKey);
            }
        }
        vTaskDelayUntil(&mylastTickCount,pdMS_TO_TICKS(10000));  // Every 500ms you toggle the shutdown signal - to check if eeprom is updated or not.
    }
}

//-- For testing Purposes - simulating vehicle state change and testing Eeprom task.



