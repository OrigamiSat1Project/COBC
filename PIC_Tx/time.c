/*** ?��?��?��Ԑ�?��?�� ***/

#include <htc.h>
#include "Type_define.h"
#include "time.h"
/**/

static UINT timer_counter_only_getChar = 0;

//wait [ms] function
void delay_ms(UWORD msec)
{
	while(msec)
	{
		__delay_ms(1);
		msec--;
	}	
}

//wait [us] function
void delay_us(UWORD usec)
{
	while(usec)
	{
		__delay_us(1);
		usec--;
	}	
}

//wait [s] function
void delay_s(UWORD sec)
{
	while(sec)
	{
		__delay_s(1);
		sec--;
	}	
}

UWORD calTime2Byte(UBYTE timeHigh, UBYTE timeLow){
    UWORD timeBin = 0;     
    timeBin = (UWORD)((timeHigh<<8) | timeLow);
    return timeBin;
}


