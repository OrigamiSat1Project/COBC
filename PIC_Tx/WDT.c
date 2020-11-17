#include <xc.h>
#include "WDT.h"
#include "Type_define.h"
#include "pinDefine.h"
#include "time.h"
#include "UART.h"

void Init_WDT(void){
    WDT_POWER = LOW;
    sendPulseWDT();
}


void WDTwait(void){
    __delay_ms(13000);
}


//TODO enter sendPulseWDT function at necessary points in main function, considering pulse max distance of 10sec
//to keep WDT from resetting the PIC 
void sendPulseWDT(void){
    if(WDT_POWER== LOW){
//        putChar('W');
        WDT_PULSE = LOW; //no delay necessary
        __delay_us(800);
        WDT_PULSE = HIGH;
    }
    putChar('W');
    putChar('D');
    putChar('T');
    put_lf();
}