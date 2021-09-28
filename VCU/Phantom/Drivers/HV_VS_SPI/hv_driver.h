/* hv_driver.h
 * Author: Yash Bhavnani
 * Date  : 27 Feb, 2021
 */

#ifndef __ADC_H__
#define __ADC_H__

/*============================================================================*/
/* Function Prototypes           */
/*============================================================================*/
static uint16 bitExtracted(uint16 number, uint8 k, uint8 p);
static void decToBinary(int n);;
static int binTwosComplementToSignedDecimal(uint8 binary[],uint8 significantBits);
static float Interpolate_input (int adc_input);
void UARTprintf(const char *_format, ...);
void adcVoltageRamp();
void adcSlaveDataSetup();
void masterDataTranser();
float getADCdata(uint16 input);
void unitTesting();


#endif
