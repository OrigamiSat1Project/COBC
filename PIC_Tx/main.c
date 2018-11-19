#include <stdio.h>
#include <stdlib.h>
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
UBYTE RXDATA[commandSize];
UBYTE STOCKDATA[3][commandSize];

//UBYTE ReceiveFlag = NOT_RECEIVE;

void interrupt InterReceiver(void){
    interruptI2C();
    if(RCIF==1){
        UBYTE tmp;
//        putChar('U');
        tmp = getChar();
        if(tmp != 'g' && tmp != 't') return;

        for(UINT j=0; j<3; j++){
            if(STOCKDATA[j][0] != 'g' && STOCKDATA[j][0] != 't'){
                STOCKDATA[j][0] = tmp;
                for(UBYTE i=1 ;i< commandSize; i++) STOCKDATA[j][i] = getChar();
                put_lf();
//                for(UBYTE i=0 ;i< commandSize; i++) putChar(STOCKDATA[j][i]);
                put_lf();
                return;
            }
        }

        STOCKDATA[0][0] = tmp;
        for(UBYTE i=1 ;i< commandSize; i++) STOCKDATA[0][i] = getChar();
        put_lf();
//        for(UBYTE i=0 ;i< commandSize; i++) putChar(RXDATA[i]);
        put_lf();
        return;
    }
}

void main(void) {

    __delay_ms(1000);
    Init_MPU();
    InitI2CMaster(I2Cbps);
    Init_SERIAL();
    Init_WDT();
    sendPulseWDT();
    delay_s(TURN_ON_WAIT_TIME);   //wait for PLL satting by RXCOBC and start CW downlink
    putChar('S');
//    putChar(0xF0);
//    put_lf();

    HK_test_setting();
//    delay_s(TURN_ON_WAIT_TIME);   //wait for PLL satting by RXCOBC
//    delay_s(CW_START_WAIT_TIME);  //wait for 200sec --> start CW downlink
//
    while(1){
//        put_lf();
//        putChar(0xE1);
//        put_lf();
        sendPulseWDT();

        measureAllChanelADC();
        if(read5VBusAndSwitchNtrxPower() != 0){
            if(read5VBusAndSwitchNtrxPower() != 0){
                onOffNTRX(0x01,0,0);//subPower ON
            }
        }

        CheckNTRXsubpower();

//        putChar(0xE2);
        HKDownlink();
//        putChar(0xE3);
        put_lf();

        //======================================================================
        //UART receive process

        for(UINT j=0; j<3; j++){
            if(STOCKDATA[j][0] == 'g' || STOCKDATA[j][0] == 't'){
                for(UINT i=0; i<commandSize; i++) RXDATA[i] = STOCKDATA[j][i];
                for(UINT i=0; i<commandSize; i++) STOCKDATA[j][i] = 0;;
//                if(j == 0) putChar(0xe0);
//                if(j == 1) putChar(0xe1);
//                if(j == 2) putChar(0xe2);
                ReceiveFlag = CORRECT_RECEIVE;
                break;
            }
        }

        if(ReceiveFlag == CORRECT_RECEIVE){
            put_lf();
//            putChar(0xbb);
//            for(UBYTE i=0; i<10 ; i++){
//                putChar(RXDATA[i]);
//            }
            put_lf();
            UBYTE command_ID = 0x00;
            UBYTE command_status = 0x00;
            UBYTE ID_add_high = 0x00;
            UBYTE ID_add_low = 0x00;

            //Calculate Address for CRCcheck byte
            ID_add_high   = RXDATA[3];
            ID_add_low    = RXDATA[4] + OffSet_for_CommandID;
//            putChar(0xc4);
//            putChar(ID_add_high);
//            putChar(ID_add_low);

            //Read Command ID byte from EEPROM
            if(RXDATA[0] == 'g'){
                command_ID = ReadEEPROM(RXDATA[2], ID_add_high, ID_add_low);
//                putChar(0xc5);
//                putChar(command_ID);
                WriteLastCommandIdToEEPROM(command_ID);
                WriteLastCommandStatusToEEPROM(UNEXECUTED);
                if(crc16(0,RXDATA,8) != CRC_check(RXDATA, 8)){
                    //Write status to EEPROM
                    WriteLastCommandStatusToEEPROM(error_main_crcCheck);
                    ReceiveFlag = UNCORRECT_RECEIVE;
//                    putChar(0xc6);
//                    put_lf();
                    continue;
                }
            }
            if(RXDATA[0] == 't'){
                if(crc16(0,RXDATA,8) != CRC_check(RXDATA, 8)){
                    ReceiveFlag = UNCORRECT_RECEIVE;
                    continue;
                }
            }
            /*---Define if command target is 't' or 'g' and read in task target ---*/
            /*------------------------------------------------------------------*/
            UBYTE FMdata[8];
            switch(RXDATA[1]){
                /*---Command from RXCOBC---*/
                /*------------------------------------------------------------------*/
//                case 0x75:  //'u'
//                    putChar(0xa4);
//                    downlinkReceivedCommand(RXDATA[2],RXDATA[3],RXDATA[4],RXDATA[5]);
//                    break;
//                /*---Command from OBC---*/
//                /*------------------------------------------------------------------*/
                case 0x63: /*'c':CW Downlink*/
//                    putChar(0xa5);
//                    commandSwitchCWDownlink(RXDATA[2], RXDATA[3], RXDATA[4], RXDATA[5], RXDATA[6], RXDATA[7], RXDATA[8]);
                    break;
                case 0x66:  /*'f':FM Downlink*/
//                    putChar(0xa6);
                    for(UBYTE i=0; i<7; i++){
                        FMdata[i] = RXDATA[i+3];
                    }    
                    commandSwitchFMDownlink(RXDATA[2], FMdata);
                    break;
                case 'p':/*'p':power*/
//                    putChar(0xa7);
                    commandSwitchPowerSupply(RXDATA[2],RXDATA[3],RXDATA[4],RXDATA[5],RXDATA[6]);
                    break;
                case 0x68: /*'h':update HK data (DC-DC voltage) (HK = house keeping)*/
//                    measureDcDcTemperature();
                    break;
                case 0x72: /*'r':send command to RXCOBC*/
//                    sendCommand(RXDATA[2], RXDATA[3], RXDATA[4], RXDATA[5], RXDATA[6], RXDATA[7], 0x00, 0x00);
                    break;
                    //for debug //putChar only
                case 0x80:
                    put_ok();
                    break;
                default:
//                    putChar(0xc8);
//                    switchError(error_main_commandfromOBCorRXCOBC);
                    put_error();
                    break;
            }
//            putChar(0xc9);
            WriteLastCommandStatusToEEPROM(command_status);
            ReceiveFlag = NOT_RECEIVE;
            put_lf();
        }
        /*---write CRC result 6bit 1 ---*/
//        switchOk(error_main_crcCheck);

    //======================================================================
    }
}
