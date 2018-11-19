#include <xc.h>
#include "MPU.h"
#include "Type_define.h"
#include "time.h"
//#include "decode_AX25.h"
#include "encode_AX25.h"
#include "EEPROM.h"
#include "I2C.h"
#include "pinDefine.h"
#include "OkError.h"

#define MELTING_TIME_MAX     3000  
#define MELTING_TIME_DEFAULT 3000 //[ms] 
#define MELTING_FINISH 0x08  //TBD

UINT invertState(UINT);
UINT invertStateWithTime(UINT,UBYTE,UBYTE);

void Init_MPU(void)
{
	//Initialize Port Status
	PORTA  = 0x00;
	PORTB  = 0x00;
	PORTC  = 0x00;
    PORTE  = 0x00;
	
	//AnalogorDigital Setting(All Digital)
	ANSEL  = 0x00;
	ANSELH = 0x00;
	
	//Port I/O Setting 
    //       0b76543210
	TRISA  = 0b10000000;
	TRISB  = 0b00010000;
    TRISC  = 0b01011100;
    TRISE  = 0b00000000;	

	//Port Initial Value Setting	
	PORTA  = 0x00;
	PORTB  = 0x00;
	PORTC  = 0x00;
    PORTE  = 0x00;
}

//Used to switch PIN to the opposite status(high/low)
//bit invertState(bit pinState){
UINT invertState(UINT pinState){
    if(pinState==high){
        return low;
    }else{
        return high;
    }
}

/*heater*/
void onOffHEATER(UBYTE onOff, UBYTE timeHigh, UBYTE timeLow){ //high->on
    if ( onOff == 0x00 ){        
            HEATER = low;  
    } else {                     
            HEATER = high;
    }

    if(timeHigh == 0x00 && timeLow == 0x00){ 
    }else {        
        UWORD wait_time;
        wait_time = calTime2Byte(timeHigh, timeLow);
        delay_ms(wait_time);
        HEATER =invertState(onOff);
    }
}

/*NTRX*/
void onOffNTRX(UBYTE onOff, UBYTE timeHigh, UBYTE timeLow){ //high->on
    if ( onOff == 0x00 ){        
            NTRX = low;  
    } else {                     
            NTRX = high;
    }

    if(timeHigh == 0x00 && timeLow == 0x00){ 
    }else {        
        UWORD wait_time;
        wait_time = calTime2Byte(timeHigh, timeLow);
        delay_ms(wait_time);
        NTRX =invertState(onOff);
    }
}

/*5R8G 5V Sub Power*/
void onOff5R8GSubPower(UBYTE onOff, UBYTE timeHigh, UBYTE timeLow){ //high->on
    if ( onOff == 0x00 ){        
            SW_5R8G = low;  
    } else {                     
            SW_5R8G = high;
    }

    if(timeHigh == 0x00 && timeLow == 0x00){ 
    }else {        
        UWORD wait_time;
        wait_time = calTime2Byte(timeHigh, timeLow);
        delay_ms(wait_time);
        SW_5R8G =invertState(onOff);
    }
}

/*antenna melting*/
void cutWire(UBYTE onOff, UBYTE timeHigh, UBYTE timeLow){ //high->on
    
/********************************************************/    
//melting program!! Be careful to write program to FM!!!!!
/********************************************************/
//    
//    if ( onOff == 0x00 ){        
////            WIRE_CUTTER = low;
//    } else {                     
////            WIRE_CUTTER = high;
//    }
//
//    if(timeHigh == 0x00 && timeLow == 0x00){
//    }else {        
//        UWORD wait_time = 0;
//        wait_time = calTime2Byte(timeHigh, timeLow);
//
//        if(wait_time>MELTING_TIME_MAX){
//            wait_time = MELTING_TIME_DEFAULT;
//        } else {}
//        delay_ms(wait_time);
////        WIRE_CUTTER =invertState(onOff);
//    }
}

/*antenna melting with meliing times*/
void cutWireWithMeltingtimes(UBYTE onOff, UBYTE timeHigh, UBYTE timeLow, UBYTE meltingTimes){
    UBYTE main_melting_status;
    UBYTE sub_melting_status;
    main_melting_status = ReadEEPROM(EEPROM_address,MeltingStatus_addressHigh, MeltingStatus_addressLow);
    sub_melting_status = ReadEEPROM(EEPROM_subaddress,MeltingStatus_addressHigh, MeltingStatus_addressLow);

    if(main_melting_status==0xff) main_melting_status = 0x00;
    if(sub_melting_status ==0xff) sub_melting_status  = 0x00;
    
    //bit operation
    //ex: 0b01101011 -> 0+1+1+0+1+0+1+1=5
    UBYTE main_melting_status_cal_result;
    UBYTE sub_melting_status_cal_result;
    main_melting_status_cal_result = bitCalResult(main_melting_status);
    sub_melting_status_cal_result = bitCalResult(sub_melting_status);
  
    //cal_result>TBD: melting already finish   / cal_result=<TBD: not yet
    if((main_melting_status_cal_result < MELTING_FINISH)&&(sub_melting_status_cal_result < MELTING_FINISH)){ 
        for(UBYTE i=0; i<meltingTimes; i++){    
            cutWire(onOff, timeHigh, timeLow);
            delay_s(WIRE_CUT_INTERVAL);
        }
    } else {
        //already melting finishs
    }
}

/*WDT*/
void onOffTXWDT(UBYTE onOff, UBYTE timeHigh, UBYTE timeLow){ //high->on
    if ( onOff == 0x00 ){        
            WDT_POWER = low;  
    } else {                     
            WDT_POWER = high;
    }

    if(timeHigh == 0x00 && timeLow == 0x00){ 
    }else {        
        UWORD wait_time;
        wait_time = calTime2Byte(timeHigh, timeLow);
        delay_ms(wait_time);
        WDT_POWER =invertState(onOff);
    }
}

/*FMPTT*/
void onOffFMPTT(UBYTE onOff, UBYTE timeHigh, UBYTE timeLow){ //high->on
    if ( onOff == 0x00 ){        
            FMPTT = low;  
    } else {                     
            FMPTT = high;
    }

    if(timeHigh == 0x00 && timeLow == 0x00){ 
    }else {        
        UWORD wait_time;
        wait_time = calTime2Byte(timeHigh, timeLow);
        delay_ms(wait_time);
        FMPTT =invertState(onOff);
    }
}

/*CWKEY*/
void onOffCWKEY(UBYTE onOff, UBYTE timeHigh, UBYTE timeLow){ //high->on
    if ( onOff == 0x00 ){        
            CWKEY = low;  
    } else {                     
            CWKEY = high;
    }

    if(timeHigh == 0x00 && timeLow == 0x00){ 
    }else {        
        UWORD wait_time;
        wait_time = calTime2Byte(timeHigh, timeLow);
        delay_ms(wait_time);
        CWKEY =invertState(onOff);
    }
}


//process command data if the command type is 'power supply'
void commandSwitchPowerSupply(UBYTE command, UBYTE onOff, UBYTE timeHigh, UBYTE timeLow, UBYTE melting_times){ //times are given in ms
    switch(command){
        case 'h':   //Heater
            onOffHEATER(onOff, timeHigh, timeLow);
            break;
        case 'n':   //NTRX
            onOffNTRX(onOff, timeHigh, timeLow);
            break;
        case '5': //5R8G 5V Sub Power
            onOff5R8GSubPower(onOff, timeHigh, timeLow);
            break;
        case 'a': //WIRE_CUTTER
////            cutWire(onOff, timeHigh, timeLow);
            break;
        case 't': //WIRE_CUTTER with times
////            cutWireWithMeltingtimes(onOff, timeHigh, timeLow, melting_times);
            break;
        case 'w': //WDT
            onOffTXWDT(onOff, timeHigh, timeLow);
            break;
        case 'p': //FMPTT
            onOffFMPTT(onOff, timeHigh, timeLow);
            break;
        case 'k': //CWKEY
            onOffCWKEY(onOff, timeHigh, timeLow);
            break;
        default:
            switchError(error_MPU_commandSwitchPowerSupply);
            break;
    }
}

void CheckNTRXsubpower(void){
    if(NTRX == 1){
        WriteOneByteToMainAndSubB0EEPROM(NTRX_subpower_status_addressHigh,NTRX_subpower_status_addressLow,NTRXsub_ON);
    }else{
        WriteOneByteToMainAndSubB0EEPROM(NTRX_subpower_status_addressHigh,NTRX_subpower_status_addressLow,NTRXsub_OFF);
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

