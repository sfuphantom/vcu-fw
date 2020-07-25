/*
 * eeprom_driver.h
 *
 *  Created on: Jul 20, 2020
 *      Author: Junaid & Andrei
 */

#ifndef PHANTOM_DRIVERS_EEPROM_EEPROM_DRIVER_H_
#define PHANTOM_DRIVERS_EEPROM_EEPROM_DRIVER_H_


/*
 *  Include files
 */

#include "ti_fee.h"
#include "sys_common.h"
#include "hal_stdtypes.h"


/*
 *  EEPROMs Definition
 */

#define EEP0    0U
#define EEP1    1U

/*
 * Data Block Definitions ; Add more blocks to ti_fee_cfg.c, followed by adding their define below.
 */


#define DATA_BLOCK_1    1U
#define DATA_BLOCK_2    2U
#define DATA_BLOCK_3    3U
#define DATA_BLOCK_4    4U



/*
 *  Asynchronous and Synchronous Macros
 */
#define ASYNC    0U
#define SYNC     1U

/*
 *  Format Keys
 *
 */
#define FORMAT_CONFIGURED_SECTORS_ONLY    0xA5A5A5A5U
#define FORMAT_EEPROM_BANK7               0x5A5A5A5AU

/*
 *  Use following Macro Data Block Length is unknown, the FEE API internally get the block length from Data Block header
 */

#define UNKNOWN_BLOCK_LENGTH    0xFFFFU

/*
 *  Supported APIs
 */

void eeprom_Init();
void eepromBlockingMain();
uint8_t eeprom_Write(uint16_t eepromNumber, uint16_t dataBlock, uint8_t *pDataBuffer, uint8_t sync_or_async);
uint8_t eeprom_Read(uint16_t eepromNumber, uint16_t dataBlock, uint16_t startingAddress, uint8_t *pRecieveBuffer, uint16_t dataBlockLength, uint8_t sync_or_async);
uint8_t eeprom_Erase(uint16_t dataBlock);
uint8_t eeprom_Format(uint16_t eepromNumber, uint32_t formatCode);
uint8_t eeprom_InvalidateBlock(uint16_t eepromNumber, uint32_t dataBlock);
TI_FeeModuleStatusType eeprom_Status(uint16_t eepromNumber);



#endif /* PHANTOM_DRIVERS_EEPROM_EEPROM_DRIVER_H_ */
