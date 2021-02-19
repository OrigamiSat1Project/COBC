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

#define MELTING_FINISH        4
#define MELTING_FINISH_FLAG 0b01111110
#define commandSize 10
UBYTE RXDATA[commandSize];
UBYTE STOCKDATA[3][commandSize];

//UBYTE ReceiveFlag = NOT_RECEIVE;

void interrupt InterReceiver(void){
    interruptI2C();
    if(RCIF==1){
        UBYTE tmp;
        tmp = getChar();
        if(tmp != 'g' && tmp != 't') return;
        
        ReceiveFlag = CORRECT_RECEIVE;
                
        for(UINT j=0; j<3; j++){
            if(STOCKDATA[j][0] != 'g' && STOCKDATA[j][0] != 't'){
                STOCKDATA[j][0] = tmp;
                for(UBYTE i=1 ;i< commandSize; i++) STOCKDATA[j][i] = getChar();
                return;
            }
        }

        STOCKDATA[0][0] = tmp;
        for(UBYTE i=1 ;i< commandSize; i++) STOCKDATA[0][i] = getChar();
        return;
    }
}

void main(void) {

    __delay_ms(1000);
    Init_MPU();
    InitI2CMaster(I2Cbps);
    Init_SERIAL();
    Init_WDT();
    Init_HK();
    sendPulseWDT();
    delay_s(TURN_ON_WAIT_TIME);   

    UBYTE melting_status[2] = {0x00};
    melting_status[0] = checkMeltingStatus(EEPROM_address);
    melting_status[1] = checkMeltingStatus(EEPROM_subaddress);
//    if((melting_status[0] < MELTING_FINISH)&&(melting_status[1] < MELTING_FINISH)) {  //before melting                                                                           //before melting
//        /*---200s ( 50s * 4times)---*/
//        for(UBYTE i=0; i<4; i++){
//            /*---wait 50s---*/
//            sendPulseWDT();
//            for(UBYTE j=0; j<10; j++){
//                delay_s(5);
//                sendPulseWDT();
//            }
//        }
//    }                  
     
    while(1){
        sendPulseWDT();

        measureAllChanelADC();
        if(read5VBusAndSwitchNtrxPower() != 0){
            if(read5VBusAndSwitchNtrxPower() != 0){
                onOffNTRX(0x01,0,0);//subPower ON
            }
        }

        CheckNTRXsubpower();
        
        HKDownlink();


        //======================================================================
        //UART receive process

        for(UINT j=0; j<3; j++){
            if(STOCKDATA[j][0] == 'g' || STOCKDATA[j][0] == 't'){
                for(UINT i=0; i<commandSize; i++) RXDATA[i] = STOCKDATA[j][i];
                for(UINT i=0; i<commandSize; i++) STOCKDATA[j][i] = 0;;
                ReceiveFlag = CORRECT_RECEIVE;
                break;
            }
        }

        if(ReceiveFlag == CORRECT_RECEIVE){
            UBYTE command_ID = 0x00;
            UBYTE ID_add_high = 0x00;
            UBYTE ID_add_low = 0x00;

            //Calculate Address for CRCcheck byte
            ID_add_high   = RXDATA[3];
            ID_add_low    = RXDATA[4] + OffSet_for_CommandID;

            //Read Command ID byte from EEPROM
            if(RXDATA[0] == 'g' || RXDATA[0] == 't'){
                command_ID = ReadEEPROM(RXDATA[2], ID_add_high, ID_add_low);
                WriteLastCommandIdToEEPROM(command_ID);
                if(crc16(0,RXDATA,8) != CRC_check(RXDATA, 8)){
                    //Write status to EEPROM
                    ReceiveFlag = UNCORRECT_RECEIVE;
                    continue;
                }
            }

            /*---Define if command target is 't' or 'g' and read in task target ---*/
            /*------------------------------------------------------------------*/
            switch(RXDATA[1]){
                /*---Command from RXCOBC---*/
                /*------------------------------------------------------------------*/
                case 0x75:  //'u'
                    downlinkReceivedCommand(RXDATA[2],RXDATA[3],RXDATA[4],RXDATA[5]);
                    break;
//                /*---Command from OBC---*/
//                /*------------------------------------------------------------------*/
                case 0x63: /*'c':CW Downlink*/
                    commandSwitchCWDownlink(RXDATA[2], RXDATA[3], RXDATA[4], RXDATA[5], RXDATA[6], RXDATA[7], RXDATA[8]);
                    break;
                case 0x66:  /*'f':FM Downlink*/
//                    commandSwitchFMDownlink(RXDATA[2], FMdata);
                    downlinkFMSignal(RXDATA[2],RXDATA[3], RXDATA[4], RXDATA[5], RXDATA[6], RXDATA[7]);
                    break;
                case 'p':/*'p':power*/
                    commandSwitchPowerSupply(RXDATA[2],RXDATA[3],RXDATA[4],RXDATA[5],RXDATA[6]);
                    break;
                default:
                    break;
            }
            ReceiveFlag = NOT_RECEIVE;
        }

    //======================================================================
    }
}
