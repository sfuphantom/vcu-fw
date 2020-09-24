/*
 * Current_transducer.h
 *
 *  Created on: Sep 3, 2020
 *      Author: yashv
 */

#ifndef PHANTOM_DRIVERS_CURRENT_TRANSDUCER_CURRENT_TRANSDUCER_H_
#define PHANTOM_DRIVERS_CURRENT_TRANSDUCER_CURRENT_TRANSDUCER_H_

#define MAX_POS_CURRENT 200
#define MIN_NEGATIVE_CURRENT -200
#define I_PN 200    // primary nominal current
#define Uref 2.5

typedef int bool;
#define true 1
#define false 0

// global variables for Ip(lower limit) and Ip(upper limit)
float IpLow;
float IpHigh;
bool flag ;

flag = false;
float voltage_to_current(const float voltage);

#endif /* PHANTOM_DRIVERS_CURRENT_TRANSDUCER_CURRENT_TRANSDUCER_H_ */
