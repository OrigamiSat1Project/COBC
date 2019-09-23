#include <xc.h>
#include "WDT.h"
#include "typeDefine.h"
#include "pinDefine.h"
#include "UART.h"
#include "time.h"
 
//EXTERNAL Watchdog Timer is used!!
// for reset of PIC the MCLR pin is used

void InitWDT(void){
    POWER_WDT = LOW;
    sendPulseWDT();
}

void WDTwait(void){
    __delay_ms(13000);
}

//TODO enter sendPulseWDT function at necessary points in main function, considering pulse max distance of 10sec
//to keep WDT from resetting the PIC 
void sendPulseWDT(void){
    if(POWER_WDT != LOW) return;
    putChar('w');
//    PULSE_WDT = LOW; 
//    __delay_us(800);  //do not change!!
//    PULSE_WDT = HIGH;
}
