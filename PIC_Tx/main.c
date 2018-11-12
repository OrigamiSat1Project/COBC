//#include <stdio.h>
//#include <stdlib.h>
#include <xc.h>
#include <PIC16F886.h>
#include "UART.h"
#include "MPU.h"
#include "Type_define.h"
#include "time.h"
#include "encode_AX25.h"
#include "I2C.h"
#include "EEPROM.h"
#include "FMCW.h"
#include "WDT.h"
#include "CW.h"
#include "pinDefine.h"
#include "CRC16.h"
#include "ADC.h"
#include "OkError.h"

void interrupt InterReceiver(void);


// PIC16F887 Configuration Bit Settings

// 'C' source line config statements

/* PIC16F887 Configuration Bit Settings */
#pragma config FOSC     = HS            // Oscillator Selection bits (HS oscillator: High-speed crystal/resonator on RA6/OSC2/CLKOUT and RA7/OSC1/CLKIN)
#pragma config WDTE     = OFF           // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE    = ON            // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE    = ON            // RE3/MCLR pin function select bit (RE3/MCLR pin function is MCLR)
#pragma config CP       = OFF           // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD      = OFF           // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN    = OFF            // Brown Out Reset Selection bits (BOR enabled)
#pragma config IESO     = OFF            // Internal External Switchover bit (Internal/External Switchover mode is enabled)
#pragma config FCMEN    = OFF            // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is enabled)
#pragma config LVP      = OFF           // Low Voltage Programming Enable bit (RB3 pin has digital I/O, HV on MCLR must be used for programming)
#pragma config BOR4V    = BOR40V        // Brown-out Reset Selection bit (Brown-out Reset set to 4.0V)
#pragma config WRT      = OFF           // Flash Program Memory Self Write Enable bits (Write protection off)

#define commandSize 10


//void interrupt InterReceiver(void){
//    
////    UBYTE commandSize;
////    commandSize = 10;
//
////    UBYTE RXDATA[10];//array size = commandSize
//    UBYTE RXDATA[commandSize];
////    volatile static int intr_counter;
//
//    if (RCIF == 1) {
//        /*---for debug---*/
////         for (UBYTE i = 0; i < commandSize; i++){
////             RXDATA[i] = getChar();
////         }
//        putChar('I');
//        putChar('I');
//        
//        UBYTE get_char_state = 0;
//        while(get_char_state < commandSize){
//            RXDATA[0] = getChar();
//            putChar('X');
////            putChar('e');
////            putChar('t');
////            putChar(RXDATA[0]);
//            get_char_state++;
//            if(RXDATA[0] != 't' && RXDATA[0] != 'g'){
//                get_char_state = 0;
//            } else {    
//                for (UBYTE i = 1; i < commandSize; i++){
//                    RXDATA[i] = getChar();
//                    get_char_state++;
//                }
//            }
//        }
//                
//        //TODO:need?
//        /*---Send command using UARTto RXCOBC---*/
//        for (UBYTE i = 0; i < commandSize; i++){
//            putChar(RXDATA[i]);
//            NOP();
//        }
//        
//        /*---CRC check for command from RXCOBC or OBC---*/ 
//       //TODO add case RXDATA[0]!=t or g
//        UWORD crcResult, crcValue;
//        UBYTE crcResultHigh,crcResultLow,crcValueHigh,crcValueLow;
//        crcResult = crc16(0,RXDATA,8);
//        crcValue =  CRC_check(RXDATA,8);
//        crcResultHigh = crcResult>>8;
//        crcResultLow = crcResult & 0x00FF;
//        crcValueHigh = crcValue>>8;
//        crcValueLow = crcValue & 0x00FF;
//        
//        /*----------------------------------------------*/
//        //FIXME:[start]debug for test to CRCcheck
////        putChar(0xcc);
////        putChar(0xcc);
////        putChar(crcResultHigh);
////        putChar(crcResultLow);
////        putChar(crcValueHigh);
////        putChar(crcValueLow);
//        //FIXME:[finish]debug for test to CRCcheck
//        /*----------------------------------------------*/
//        
//        /*---read command ID---*/
//        UBYTE commandID;
//        commandID = ReadEEPROM(EEPROM_address, HighAddress_for_commandID, LowAddress_for_commandID);
//        //TODO:read datas from sub EEPROM
//        
//        /*---read CRC check from EEPROM---*/
//        UBYTE CRC_check_result;
////        CRC_check_result = 0b1000000; //for debug
////        WriteOneByteToMainAndSubB0EEPROM(crcResult_addressHigh, crcResult_addressLow,CRC_check_result); //for debug
//        CRC_check_result = ReadEEPROM(EEPROM_address, crcResult_addressHigh, crcResult_addressLow);
//        
//        if(crcResult != crcValue){  //crc error
//            
//            /*---write CRC error result (6bit 0) ---*/
//            CRC_check_result &= 0b10111111;
//            WriteOneByteToMainAndSubB0EEPROM(crcResult_addressHigh, crcResult_addressLow,CRC_check_result);
//            
//            putChar(0xa1);
//            putChar(CRC_check_result);
//            putErrorNoDownlink(error_main_crcCheck);    
//            
//        } else {  //crc  OK
//            
//            /*---write CRC ok result (6bit 1) ---*/
//            CRC_check_result |= 0b01000000;
//            WriteOneByteToMainAndSubB0EEPROM(crcResult_addressHigh, crcResult_addressLow,CRC_check_result);
//            
//            putChar(0xa2);
//            putChar(CRC_check_result);
//            
//            /*---Define if command target is 't' or 'g' and read in task target ---*/
//            /*------------------------------------------------------------------*/
//            if (RXDATA[0]!='t' && RXDATA[0]!='g' ){
//                //TODO:add error messege
//                putChar(0xa3);
//            } else {
//                switch(RXDATA[1]){
//                    /*---Command from RXCOBC---*/
//                    /*------------------------------------------------------------------*/
//                    case 0x75:  //'u'
//                        putChar('R'); 
//                        putChar(0xa4);
//                        downlinkReceivedCommand(RXDATA[2],RXDATA[3],RXDATA[4],RXDATA[5]);
//                        break;
//
//                    /*---Command from OBC---*/
//                    /*------------------------------------------------------------------*/
//                    case 0x63: /*'c':CW Downlink*/
//                        putChar(0xa5);
//                        commandSwitchCWDownlink(RXDATA[2], RXDATA[3], RXDATA[4], RXDATA[5], RXDATA[6], RXDATA[7], RXDATA[8]);
//                        WriteLastCommandIdToEEPROM(commandID);
//                        break;
//                    case 0x66:  /*'f':FM Downlink*/
//                        putChar(0xa6);
//                        downlinkFMSignal(RXDATA[2],RXDATA[3],RXDATA[4],RXDATA[5],RXDATA[6]);
//                        WriteLastCommandIdToEEPROM(commandID);
//                        break;
//                    case 'p':/*'p':power*/
//                        putChar(0xa7); 
//                        commandSwitchPowerSupply(RXDATA[2],RXDATA[3],RXDATA[4],RXDATA[5],RXDATA[6]);
//                        WriteLastCommandIdToEEPROM(commandID);
//                        break;
//                    case 0x68: /*'h':update HK data (DC-DC voltage) (HK = house keeping)*/
//                        measureDcDcTemperature();
//                        WriteLastCommandIdToEEPROM(commandID);
//                        break;
//                    case 0x72: /*'r':send command to RXCOBC*/
//                        sendCommand(RXDATA[2], RXDATA[3], RXDATA[4], RXDATA[5], RXDATA[6], RXDATA[7], 0x00, 0x00);
//                        break;
//                    default:
//                        switchError(error_main_commandfromOBCorRXCOBC);   
//                        break;                             
//                }
//            }
//            /*---write CRC result 6bit 1 ---*/
//            CRC_check_result |= 0b01000000;
//            WriteOneByteToMainAndSubB0EEPROM(crcResult_addressHigh, crcResult_addressLow,CRC_check_result);
//            switchOk(error_main_crcCheck);   
//        }
//        RCIF = 0;
//    }
//}

void main(void) {
    
    __delay_ms(1000);
    Init_MPU();
    InitI2CMaster(I2Cbps);
    Init_SERIAL();     
//    Init_WDT();    
    sendPulseWDT();
    delay_s(TURN_ON_WAIT_TIME);   //wait for PLL satting by RXCOBC and start CW downlink
    putChar('S');


//    delay_s(TURN_ON_WAIT_TIME);   //wait for PLL satting by RXCOBC
//    delay_s(CW_START_WAIT_TIME);  //wait for 200sec --> start CW downlink
    

    HK_test_setting();
    
    while(1){
        
        putChar('m');

        //TODO send pulse to WDT
        sendPulseWDT();
        __delay_ms(5000);
         
        measureDcDcTemperature();
        if(OBC_STATUS == low){          
            measureChannel2();//read 5V Bus
        }else{     
        }         
        //TODO debug send HK 
        HKDownlink();
    }
}
