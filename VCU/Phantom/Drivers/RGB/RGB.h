/*
 * RGB.h
 *
 *  Created on: Mar 3, 2021
 *      Author: Rafael Guevara
 */

//Halcogen Setup
//1.Enable HET Drivers
//2.Enable pwm channel outputting to specified pin
//3.Enable Output of pin in HET window

#ifndef SOURCE_RGB_H_
#define SOURCE_RGB_H_

#include "gio.h"
#include "het.h"

//pins (used on launchpad)
#define REDhetbit pwm0 //N2HET2_4 pwm0
#define GREENhetbit pwm1 //N2HET1_29 pwm1
#define BLUEhetbit pwm0 //N2HET1_27 pwm0

#define REDhetnum hetRAM2 //N2HET2_4 pwm0
#define GREENhetnum hetRAM1 //N2HET1_29 pwm1
#define BLUEhetnum hetRAM1 //N2HET1_27 pwm0

//may add more colors...
#define RED 0
#define GREEN 1
#define BLUE 2


void RGBinit();

//turn off RGB LED
void resetRGB();


void setRGBcolor(int);

//directly set pwm of RGBpins
//must be between 0-100
void setRGBvalue(int,int,int);


#endif /* SOURCE_RGB_H_ */
