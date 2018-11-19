#include <xc.h>
#include <__null.h>
#include "MPU.h"
#include "pinDefine.h"
#include "time.h"
#include "typeDefine.h"
#include "UART.h"
#include "FMCW.h"
#include "EEPROM.h"
#include "I2C.h"
#include "OkError.h"
#include "WDT.h"
#include "SatMode.h"

/*******************************************************************************
*Initialize MPU 
******************************************************************************/
void InitMPU(void)
{
	//Initialize Port Status
	PORTA  = 0x00;
	PORTB  = 0x00;
	PORTC  = 0x00;
    PORTD  = 0x00;
    PORTE  = 0x00;
	
	//Port I/O Setting 
    //       0b76543210
	TRISA  = 0b10000000;
	TRISB  = 0b00100000;
    TRISC  = 0b10011000;
    TRISD  = 0b10000000;
    TRISE  = 0b00000000;	

	//Port Initial Value Setting	
	PORTA  = 0x00;
	PORTB  = 0x00; //WDT on
	PORTC  = 0x00;
    PORTD  = 0x00;
    PORTE  = 0x00;
}

//Used to switch PIN to the opposite status(high/low)
//bit invertState(bit pinState){
UBYTE invertState(UBYTE pinState){
    if(pinState==HIGH){
        return LOW;
    }else{
        return HIGH;
    }
}

/*******************************************************************************
*Swticch Power 5.8/OBC/WDT
******************************************************************************/
/*
 *	witch Power Spply for 1pin
 *	arg      :   POWER_PIN, onOff, timeHigh, timeLow
 *	return   :   target_select :  5->5R8G O->OBC W->WDT 
 *               0x00->off 0x01->on
 *               timeHigh == 0x00 && timeLow == 0x00 -> not chage until next Uplink
 *               timeHigh != 0x00 && timeLow == 0x00 -> wait time is short ( 1byte:1~255[ms])  -> invert state
 *               timeHigh != 0x00 && timeLow != 0x00 -> wait time is long ( 2byte:266~65535[ms)  -> invert state
 *	TODO     :   need debug
 *	FIXME    :   not yet
 *	XXX      :   add function to change OnOff time
 */
void onOff5R8G(UBYTE onOff, UBYTE timeHigh, UBYTE timeLow){ //high->on
    if ( onOff == 0x00 ){        
            POWER_5R8G = LOW;  
    } else {                     
            POWER_5R8G = HIGH;
    }

    if(timeHigh == 0x00 && timeLow == 0x00){ 
    }else {        
        UWORD wait_time;
        wait_time = (UWORD)(timeHigh << 8 | timeLow);
        delay_ms(wait_time);
        POWER_5R8G =invertState(onOff);
    }
}

void onOffOBC(UBYTE onOff, UBYTE timeHigh, UBYTE timeLow){ //high->on
    if ( onOff == 0x00 ){        
            POWER_OBC = LOW;  
    } else {                     
            POWER_OBC = HIGH;
    }

    if(timeHigh == 0x00 && timeLow == 0x00) return;    
    UWORD wait_time;
    wait_time = (UWORD)(timeHigh << 8 | timeLow);
    delay_ms(wait_time);
    POWER_OBC =invertState(onOff);
}

void onOffWDT(UBYTE onOff, UBYTE timeHigh, UBYTE timeLow){ //high->off
    if ( onOff == 0x00 ){        
            POWER_WDT = HIGH;  
    } else {                     
            POWER_WDT = LOW;
    }

    if(timeHigh == 0x00 && timeLow == 0x00){ 
    }else {        
        UWORD wait_time;
        wait_time = (UWORD)(timeHigh << 8 | timeLow);
        delay_ms(wait_time);
        POWER_WDT =invertState(onOff);
    }
}

void switchPowerSpply1pin(UBYTE target_select, UBYTE onOff, UBYTE timeHigh, UBYTE timeLow){  
    switch(target_select){
        case '5':
            onOff5R8G(onOff, timeHigh, timeLow);
            break;
        case 'O':
            onOffOBC(onOff, timeHigh, timeLow);
            break;
        case 'W':
            onOffWDT(onOff, timeHigh, timeLow);
            break;
        default:
            switchError(error_MPU_switchPowerSpply1pin);
            break;
    }  
}

/*******************************************************************************
*Swticch Power EPS 
******************************************************************************/
void killEPS(void){
    sendPulseWDT();
    SEP_SW = HIGH;     //EPS off -> 5VBUS off
    RBF_SW = LOW;
    __delay_ms(500);  
    sendPulseWDT(); 
}

void onEPS(void){
    sendPulseWDT();
    SEP_SW = LOW;      //EPS on  -> 800ms INTERVAL ->5VBUS on
    RBF_SW = LOW;
    __delay_ms(2000);
    sendPulseWDT();  
}

void resetEPS(void){
    sendPulseWDT();
    SEP_SW = HIGH;     //EPS off -> 5VBUS off
    RBF_SW = LOW;
    __delay_ms(500);  
    SEP_SW = LOW;      //EPS on  -> 800ms INTERVAL ->5VBUS on
    RBF_SW = LOW;
    __delay_ms(2000);
    sendPulseWDT();
}

/*
 *	switch Power EPS
 *	arg      :   POWER_PIN_1, POWER_PIN_2, onOff, timeHigh, timeLow
 *	return   :   0x00 -> SEP_SW&RBF_SW : HIGH -> EPS : OFF, 0x01 -> SEP_SW&RBF_SW : LOW -> EPS : ON
 *               timeHigh == 0x00 && timeLow == 0x00 -> not chage until next Uplink
 *               timeHigh != 0x00 && timeLow == 0x00 -> wait time is short ( 1byte:1~255[ms]) -> invert state
 *               timeHigh != 0x00 && timeLow != 0x00 -> wait time is long ( 2byte:266~65535[ms) -> invert state
 *	TODO     :   need debug
 *	FIXME    :   not yet
 *	XXX      :   add function to change OnOff time
 */
void switchPowerEPS(UBYTE onOff, UBYTE timeHigh, UBYTE timeLow){  
    if ( onOff == 0x00 ){        //EPS off
            SEP_SW = HIGH;
            RBF_SW = LOW;
            __delay_ms(500);
    } else {                     //EPS on
            SEP_SW = LOW;
            RBF_SW = LOW;
            __delay_ms(2000);
    }
    
    if(timeHigh == 0x00 && timeLow == 0x00){   

    }else {        
        UWORD wait_time;
        wait_time = (UWORD)(timeHigh << 8 | timeLow);
        delay_ms(wait_time);
        SEP_SW =invertState(onOff);
    }

}

/*
 *	reive EPS
 *	arg      :   timeHigh, timeLow
 *	return   :   timeHigh == 0x00 && timeLow == 0x00 -> not chage until next Uplink
 *               timeHigh != 0x00 && timeLow == 0x00 -> wait time is short ( 1byte:1~255[ms]) -> revive EPS
 *               timeHigh != 0x00 && timeLow != 0x00 -> wait time is long ( 2byte:266~65535[ms) -> revive EPS           
 *	TODO     :   need debug
 *	FIXME    :   not yet
 *	XXX      :   not yet
 */
void reviveEPS(UBYTE timeHigh, UBYTE timeLow){
    if(timeHigh == 0x00 && timeLow == 0x00){     
    }else if(timeLow == 0x00){                    
        __delay_ms(timeHigh);                     
        onEPS();
    }else {                                      
        UWORD wait_time;
        wait_time = (UWORD)(timeHigh << 8 | timeLow);
        __delay_ms(wait_time);
        onEPS();
    }
}

/*******************************************************************************
*functions for commandSwitchIntProcess
******************************************************************************/
void changeInOut(UINT pin_select_command, UBYTE inOut){
    UBYTE inOutStatus_addressHigh;
    UBYTE inOutStatus_addressLow;
    switch (pin_select_command){
        case 0x0a:
            TRISA = inOut;
            inOutStatus_addressHigh = TRISA_addressHigh;
            inOutStatus_addressLow  = TRISA_addressLow;
            break;
        case 0x0b:
            TRISB = inOut;
            inOutStatus_addressHigh = TRISB_addressHigh;
            inOutStatus_addressLow  = TRISB_addressLow;
            break;
        case 0x0c:
            TRISC = inOut;
            inOutStatus_addressHigh = TRISC_addressHigh;
            inOutStatus_addressLow  = TRISC_addressLow;
            break;
        case 0x0d:
            TRISD = inOut;
            inOutStatus_addressHigh = TRISD_addressHigh;
            inOutStatus_addressLow  = TRISD_addressLow;
            break;
        case 0x0e:
            TRISE = inOut;
            inOutStatus_addressHigh = TRISE_addressHigh;
            inOutStatus_addressLow  = TRISE_addressLow;
            break;
        default:
            switchError(error_MPU_changeInOut);
            break;
    }
    WriteOneByteToMainAndSubB0EEPROM(inOutStatus_addressHigh, inOutStatus_addressLow, inOut);
}

void changeHighLow(UINT pin_select_command, UBYTE highLow){
    UBYTE highLowStatus_addressHigh;
    UBYTE highLowStatus_addressLow;
    switch (pin_select_command){
        case 0x0a:
            PORTA = highLow;
            highLowStatus_addressHigh = PORTA_addressHigh;
            highLowStatus_addressLow  = PORTA_addressLow;
            break;
        case 0x0b:
            PORTB = highLow;
            highLowStatus_addressHigh = PORTB_addressHigh;
            highLowStatus_addressLow  = PORTB_addressLow;
            break;
        case 0x0c:
            PORTC = highLow;
            highLowStatus_addressHigh = PORTC_addressHigh;
            highLowStatus_addressLow  = PORTC_addressLow;
            break;
        case 0x0d:
            PORTD = highLow;
            highLowStatus_addressHigh = PORTD_addressHigh;
            highLowStatus_addressLow  = PORTD_addressLow;
            break;
        case 0x0e:
            PORTE = highLow;
            highLowStatus_addressHigh = PORTE_addressHigh;
            highLowStatus_addressLow  = PORTE_addressLow;
            break;
        default:
            switchError(error_MPU_changeHighLow);
            break;
    }
    WriteOneByteToMainAndSubB0EEPROM(highLowStatus_addressHigh, highLowStatus_addressLow, highLow);
}

//process command data if the command type is 'power supply'
void commandSwitchPowerSupply(UBYTE command, UBYTE onOff, UBYTE timeHigh, UBYTE timeLow){ //times are given in ms
    switch(command){    
        case '5': //5R8G
            switchPowerSpply1pin('5', onOff, timeHigh, timeLow);
            break;
        case 'e': //EPS
            switchPowerEPS(onOff, timeHigh, timeLow);
            break;
        case 'o': //OBC
            switchPowerSpply1pin('O', onOff, timeHigh, timeLow);
            break;
        case 'w': //WDT
            switchPowerSpply1pin('W', onOff, timeHigh, timeLow);
            break;
        default:
            switchError(error_MPU_commandSwitchSatMode);
            break;
    }
}


//process command data if the command type is 'internal processing'
void commandSwitchIntProcess(UBYTE command, UBYTE data1, UBYTE data2){ 
    switch(command){    
        case 'i': //change in/out
            changeInOut(data1, data2);
            break;
        case 'h': //change high/low
            changeHighLow(data1, data2);     
            break;
        default:
            switchError(error_MPU_commandSwitchIntProcess);
            break;
    }
}

//turn ON NTRX(CIB power supply)
void onNtrxPowerSupplyCIB(UBYTE timeHigh,UBYTE timeLow){
    UBYTE send_command[8];
    send_command[0] = 't';
    send_command[1] = 'p';
    send_command[2] = 'n';
    send_command[3] = 0x01;
    send_command[4] = timeHigh;
    send_command[5] = timeLow;
    send_command[6] = 0x00;
    send_command[7] = 0x00;
    put_lf();  
    sendCommandByPointer(send_command);
    __delay_ms(2000);//wait EPS ON
    setPLL();
//    reviveEPS(timeHigh, timeLow);           
}

//turn off NTRX(CIB power supply)
void offNtrxPowerSupplyCIB(void){
    UBYTE send_command[8];
    send_command[0] = 't';
    send_command[1] = 'p';
    send_command[2] = 'n';
    send_command[3] = 0x00;
    send_command[4] = 0x00;
    send_command[5] = 0x00;
    send_command[6] = 0x00;
    send_command[7] = 0x00;
    put_lf();  
    sendCommandByPointer(send_command);
    __delay_ms(500);
}

//Read NTRX SubPower status and count HIGH bit and return count
UBYTE ReadNtrxSubPowerStatus(void){
    UBYTE status = ReadEEPROM(MAIN_EEPROM_ADDRESS,NTRX_subpower_status_addressHigh,NTRX_subpower_status_addressLow);
    UBYTE count = BitCount(status);
    if(count > 2 && count < 4){//subPower OFF
        return 0;
    }else if(count > 5 && count < 7){//subPower ON
        return 1;
    }else{
        status = ReadEEPROM(SUB_EEPROM_ADDRESS,NTRX_subpower_status_addressHigh,NTRX_subpower_status_addressLow);
        count = BitCount(status);
        if(count > 2 && count < 4){//subPower OFF
            return 0;
        }else if(count > 5 && count < 7){//subPower ON
            return 1;
        }else{
            return 15;//0x0F
        }
    }
}