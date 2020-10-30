/* 
 * File:   time.h
 * Author: Reo
 *
 * Created on 2017/01/21, 18:49
 */


#ifndef TIME_H
#define	TIME_H

#include "typeDefine.h"

// Clock frequency
//TODO:change 20M[Hz] and 4M[Hz] ?��K?��؂Ȓl
//TODO:MPU?��?��change Xtal //?��Ƃ�?��?��?��Ƃ�?��̃G?��?��?��[?��?��?��P
#define _XTAL_FREQ_HIGH      20000000   //20M[Hz]
#define _XTAL_FREQ_MIDDLE    10000000   //10M[Hz]
#define _XTAL_FREQ_LOW        4000000   // 4M[Hz]         
#define _XTAL_FREQ           _XTAL_FREQ_MIDDLE 

#define __delay_us(x) _delay((unsigned long)((x)*(_XTAL_FREQ/4000000.0)))
#define __delay_ms(x) _delay((unsigned long)((x)*(_XTAL_FREQ/4000.0)))
#define __delay_s(x) _delay((unsigned long)((x)*(_XTAL_FREQ/4.0)))

#define HALF_INTERVAL   400   // half the time interval of one bit //decided by measurement

//functions to wait [ms] or [us]
void delay_s(UWORD);
void delay_ms(UWORD);
void delay_us(UWORD);


#endif	/* TIME_H */

