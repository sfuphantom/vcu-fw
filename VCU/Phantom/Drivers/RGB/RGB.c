/*
 * RGB.c
 *
 *  Created on: Mar 3, 2021
 *      Author: Rafael
 */

#include "RGB.h"


void RGBinit(){

    hetInit();

    resetRGB();

}

void resetRGB(){

    //Red pin
    pwmSetDuty(REDhetnum,REDhetbit,0);

    //Green pin
    pwmSetDuty(GREENhetnum,GREENhetbit,0);

    //Blue pin
    pwmSetDuty(BLUEhetnum,BLUEhetbit,0);

}


void setRGBcolor(int color){

    switch(color){

        case RED:

            //Red pin
            pwmSetDuty(REDhetnum,REDhetbit,75);

            //Green pin
            pwmSetDuty(GREENhetnum,GREENhetbit,0);

            //Blue pin
            pwmSetDuty(BLUEhetnum,BLUEhetbit,0);

            break;

        case GREEN:

            //Red pin
           pwmSetDuty(REDhetnum,REDhetbit,0);

           //Green pin
           pwmSetDuty(GREENhetnum,GREENhetbit,75);

           //Blue pin
           pwmSetDuty(BLUEhetnum,BLUEhetbit,0);

           break;

        case BLUE:
            //Red pin
           pwmSetDuty(REDhetnum,REDhetbit,0);

           //Green pin
           pwmSetDuty(GREENhetnum,GREENhetbit,0);

           //Blue pin
           pwmSetDuty(BLUEhetnum,BLUEhetbit,75);

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

