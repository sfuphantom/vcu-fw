/*
 * RGB.c
 *
 *  Created on: Mar 3, 2021
 *      Author: Rafael Guevara
 */

#include "RGB.h"

void RGBinit(){

    hetInit();

    resetRGB();
}

void resetRGB(){

   setRGBvalue(0,0,0);
}

void setRGBcolor(int color){

    switch(color){

        case RED:

            setRGBvalue(75,0,0);

            break;

        case GREEN:

           setRGBvalue(0,75,0);

           break;

        case BLUE:

           setRGBvalue(0,0,75);

           break;

        default:

            resetRGB();

            break;

    }//case statement

}

void setRGBvalue(int REDpwm,int GREENpwm,int BLUEpwm){

    //error handling
    if(REDpwm  > 100 || REDpwm < 0) return;
    if(GREENpwm  > 100 || GREENpwm < 0) return;
    if(BLUEpwm  > 100 || BLUEpwm < 0) return;

    //Red pin
    pwmSetDuty(REDhetnum,REDhetbit,REDpwm);

    //Green pin
    pwmSetDuty(GREENhetnum,GREENhetbit,GREENpwm);

    //Blue pin
    pwmSetDuty(BLUEhetnum,BLUEhetbit,BLUEpwm);

}

