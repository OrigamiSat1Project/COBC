/*
 * File:   main.c
 * Author: Hohmann Britta
 * Revision history: v1.0
 * 
 * Program Description: This Program Allows
 * implements ADC for PIN RA3 of PIC16F886
 * function is checked using the onboard LED
 * 
 * Created on 2018/05/08, 16:54
 */

/* 
* ch1 AN2  BAT temperature
* ch2 AN3  5VBUS voltage(EPS 5V)
* ch3 AN4  3V3BUS voltage(EPS 3.3V) 
* ch4 AN10 5V voltage
* 
*/

/*******************************************************************************
*Includes and defines
******************************************************************************/
//#include "PIC16F886_Internal.h"
#include "Type_define.h"
#include "UART.h"
#include "time.h"
#include "ADC.h"
#include "EEPROM.h"
#include "I2C.h"
#include "OkError.h"
#include "MPU.h"
#include <xc.h>
#include <stdio.h>

/*******************************************************************************
*method
******************************************************************************/
void initADC(void);
UWORD adc_read();
UWORD adcValue[Channel_Size];
UBYTE adcValue_dataHigh_addressHigh[Channel_Size];
UBYTE adcValue_dataHigh_addressLow[Channel_Size];
UBYTE adcValue_dataLow_addressHigh[Channel_Size];
UBYTE adcValue_dataLow_addressLow[Channel_Size];
/*******************************************************************************
* Function: void initMain()
*
* Returns: Nothing
*
* Description: Contains initializations for main
*
* Usage: initMain()
******************************************************************************/
void initADC(){
    ANSEL = 0x00;
    ANSELH = 0x00;
    
    //----------------------
    // Run at 8 MHz //if this value is changed, ADC clock frequency must be adjusted accordingly
    //Clock determined by FOSC in configuration bits
    SCS = 0;
    //Frequency select bits
//    IRCF0 = 1;
//    IRCF1 = 1;
//    IRCF2 = 1;
    //-----------------------
    
    /*ADCON0: A/N CONTROL REGISTER 0*/
    // Set ADC conversion clock source, conversion time is 3.2us (Fosc/32)
    ADCON0bits.ADCS1 = 1;
    ADCON0bits.ADCS0 = 0;

    /*ADCON1: A/N CONTROL REGISTER 1*/
    // Set result formatting to right justified
    ADCON1bits.ADFM = 1;

    // Set ADC reference Voltage
    ADCON1bits.VCFG1 = 0;   //Vref- = Vss
    ADCON1bits.VCFG0 = 0;   //Vref+ = Vdd

    /*Set Port Configuration*/
    //TRIS:1->input / ANSEL:1->analog                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                      
    // Set PIN A2 as analog input
    TRISAbits.TRISA2 = 1;
    ANSELbits.ANS2 = 1;
    // Set PIN A3 as analog input
    TRISAbits.TRISA3 = 1;
    ANSELbits.ANS3 = 1;
    // Set PIN A5 as analog input
    TRISAbits.TRISA5 = 1;
    ANSELbits.ANS4 = 1;
    // Set PIN B1 as analog input
    TRISBbits.TRISB1 = 1;
    ANSELHbits.ANS10 = 1;

    //Set interrupt control 
    // PIE1bits.ADIE = 0;  //disable ADC interrupt
    // PIR1bits.ADIF = 0;  //ADC has not completed or has not been started    
    
    // Zero ADRESL and ADRESH
    ADRESL = 0;
    ADRESH = 0;

}

UWORD adc_read(){
    //Turn ADC on
    ADCON0bits.ADON = 1;
    //Sample Channel
    __delay_us(100);
    ADCON0bits.GO = 1;
    while (ADCON0bits.GO_nDONE);
    //give out ADC result
    UWORD value;
    value = (UWORD)((ADRESH<<8) | ADRESL);
    return(value);              
}

/*******************************************************************************
* Function: Main
*
* Returns: Nothing
*
 * Description: Program entry point
******************************************************************************/
void measureAllChanelADC(){
    initADC();          
    measureDcDcTemperature();
    measureChannel2();
    measureChannel4();
}

/**
 * measure 1Chanel (DC-DC temperature)
 * 1. select chanel
 * 2. read ADC data (**the size of data is 2byte)
 * 3. write to main and sub EEPROM 
 * arg     : slaveaddress, high_address, low_address
 * return  : ---
 * TODO    : debug 
 * 
*/
void measureDcDcTemperature() {    
    initADC();          
    ADCON0bits.CHS = 0b0010;
    adcValue[0] = adc_read();  
    //data high
    WriteOneByteToEEPROM(EEPROM_address, adcValue_CH1_addressHigh, adcValue_CH1_addressLow, (UBYTE)(adcValue[0] >> 8));     
    WriteOneByteToEEPROM(EEPROM_subaddress, adcValue_CH1_addressHigh, adcValue_CH1_addressLow, (UBYTE)(adcValue[0] >> 8));     
    //data low
    WriteOneByteToEEPROM(EEPROM_address, adcValue_CH1_addressHigh, adcValue_CH1_addressLow + 0x01, (UBYTE)(adcValue[0] & 0xff));   
    WriteOneByteToEEPROM(EEPROM_subaddress, adcValue_CH1_addressHigh , adcValue_CH1_addressLow + 0x01, (UBYTE)(adcValue[0] & 0xff));  
}

/**
 * measure 1 channle and downlinl the data 
 * 1. read ADC data (**the size of data is 2byte)
 * 2. write to EEPROM
 * 3. downlink the data 
 * return  : ---
 * TODO    : debug 
 * 
*/
void measureChannel2(){
    initADC();
    ADCON0bits.CHS = 0b0011;
    adcValue[1] = adc_read();
    //data High
    WriteOneByteToEEPROM(EEPROM_address, adcValue_CH2_addressHigh, adcValue_CH2_addressLow, (UBYTE)(adcValue[1] >> 8));
    WriteOneByteToEEPROM(EEPROM_subaddress, adcValue_CH2_addressHigh, adcValue_CH2_addressLow, (UBYTE)(adcValue[1] >> 8));
    //data low
    WriteOneByteToEEPROM(EEPROM_address, adcValue_CH2_addressHigh, adcValue_CH2_addressLow+0x01, (UBYTE)(adcValue[1] & 0xff));   
    WriteOneByteToEEPROM(EEPROM_subaddress, adcValue_CH2_addressHigh, adcValue_CH2_addressLow+0x01, (UBYTE)(adcValue[1] & 0xff));
}
void measureChannel3(){
    initADC();
    ADCON0bits.CHS = 0b0100;
    adcValue[2] = adc_read();
    //data High
    WriteOneByteToEEPROM(EEPROM_address, adcValue_CH3_addressHigh, adcValue_CH3_addressLow, (UBYTE)(adcValue[2] >> 8));
    WriteOneByteToEEPROM(EEPROM_subaddress, adcValue_CH3_addressHigh, adcValue_CH3_addressLow, (UBYTE)(adcValue[2] >> 8));
    //data low
    WriteOneByteToEEPROM(EEPROM_address, adcValue_CH3_addressHigh, adcValue_CH3_addressLow+0x01, (UBYTE)(adcValue[2] & 0xff));   
    WriteOneByteToEEPROM(EEPROM_subaddress, adcValue_CH3_addressHigh, adcValue_CH3_addressLow+0x01, (UBYTE)(adcValue[2] & 0xff));
}
void measureChannel4(){
    initADC();
    ADCON0bits.CHS = 0b1010;
    adcValue[3] = adc_read();
    //data High
    WriteOneByteToEEPROM(EEPROM_address, adcValue_CH4_addressHigh, adcValue_CH4_addressLow, (UBYTE)(adcValue[3] >> 8));
    WriteOneByteToEEPROM(EEPROM_subaddress, adcValue_CH4_addressHigh, adcValue_CH4_addressLow, (UBYTE)(adcValue[3] >> 8));
    //data low
    WriteOneByteToEEPROM(EEPROM_address, adcValue_CH4_addressHigh, adcValue_CH4_addressLow+0x01, (UBYTE)(adcValue[3] & 0xff));   
    WriteOneByteToEEPROM(EEPROM_subaddress, adcValue_CH4_addressHigh, adcValue_CH4_addressLow+0x01, (UBYTE)(adcValue[3] & 0xff));
}

//process command data if the command type is 'HKdata'
void commandSwitchHKdata(UBYTE type_sellect){ 
   switch(type_sellect){    
        case 'd': //measure DC-DC temperature
            measureDcDcTemperature();
            updateErrorStatus(ok_ADC_downlinkReceivedCommand_DcDc);
            break;
        case '5': //5VBUS 
            measureChannel2();
            updateErrorStatus(ok_ADC_downlinkReceivedCommand_5VBus);
            break;
        case 'C': //5V CIB
            measureChannel4();
            updateErrorStatus(ok_ADC_downlinkReceivedCommand_5VCIB);
            break;
        case 'u': //update all HK data
            measureAllChanelADC();
            updateErrorStatus(ok_ADC_downlinkReceivedCommand_All);
            break;
        default:
            updateErrorStatus(error_ADC_commandSwitchHKdata);
            break;
   }
}

UBYTE read5VBusAndSwitchNtrxPower(void){
    UBYTE SatMode;
    UBYTE error_status = 0;   
    measureChannel2();//read 5V Bus
    UBYTE adcValue_high = ReadEEPROM(EEPROM_address, adcValue_CH2_addressHigh, adcValue_CH2_addressLow);
    UBYTE adcValue_low = ReadEEPROM(EEPROM_address, adcValue_CH2_addressHigh, adcValue_CH2_addressLow +1);
    UWORD adcValue = (UWORD)adcValue_high << 8 | (UWORD)adcValue_low;
        if(adcValue <= ADC_Value_4V){
            SatMode = ReadEEPROM(EEPROM_address,satelliteMode_addressHigh,satelliteMode_addressLow);
            SatMode = SatMode & 0xF0;
            switch(SatMode){
                case SatMode_Nominal:
                    error_status = 1;
                    break;
                case SatMode_Saving:
                case SatMode_Survival:
                    break;
                default:
                    SatMode = ReadEEPROM(EEPROM_subaddress,satelliteMode_addressHigh,satelliteMode_addressLow);
                    SatMode = SatMode & 0xF0;
                    switch(SatMode){
                        case SatMode_Nominal:
                            error_status = 1;
                            break;
                        case SatMode_Saving:
                        case SatMode_Survival:
                            break;
                        default:
                            onOffNTRX(1,0,0);//subPower ON
                            break;
                    }
                    break;
            }          
        }else{
            onOffNTRX(0,0,0);//subPower off
        }
    return error_status;
}