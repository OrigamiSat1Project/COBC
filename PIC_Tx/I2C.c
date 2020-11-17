#include	<stdio.h>
#include <xc.h>
#include "UART.h"
#include "I2C.h"
#include "Type_define.h"
#include "EEPROM.h"
#include "OkError.h"
#include "WDT.h"
#include "Type_define.h"

#define _XTAL_FREQ 10000000



/*******************************************************************************
*setting
******************************************************************************/
void InitI2CMaster(const UDWORD c){//Init Master Synchronous Serial Port(MSSP)
  SSPCON = 0b00101000;          //MSSP Control Register: Synchronous Serial Port Enabled;I2C Master mode, clock = FOSC/(4 * (SSPADD + 1))
  SSPCON2 = 0;                  //MSSP Control Register 2:
  SSPADD = (_XTAL_FREQ/(4*c))-1; //MSSP Address Register: the lower seven bits of SSPADD act as the baud rate generator reload value
  SSPSTAT = 0x00;                  //MSSP Status Register
//  SSPSTAT = 0x80;                  //MSSP Status Register
  PIE1bits.SSPIE  = 1 ;               // enable MSSP interrupt
  PIE2bits.BCLIE  = 1 ;               // enable bus collision interrupt
  PIR1bits.SSPIF  = 0 ;               // clear MSSP interrupt flag
  PIR2bits.BCLIF  = 0 ;               // clear bus collision flag
}

void interruptI2C(void)
{
    if (PIR1bits.SSPIF == 1) {
        if (AckCheck == 1) {
            putChar('A');
            putChar('C');
            putChar('K');
            putChar('\r');
            putChar('\n');
            AckCheck = 0;
        }
          
        putChar('S');
        putChar('S');
        putChar('P');
        putChar('\r');
        putChar('\n');
        PIR1bits.SSPIF = 0;
     }
    if (PIR2bits.BCLIF == 1) {
        putChar('B');
        putChar('C');
        putChar('L');
        putChar('\r');
        putChar('\n');
        CollisionCheck = 1;
        PIR2bits.BCLIF = 0;
//          putChar('c');
//          putChar('\r');
//          putChar('\n');
    }
}

void I2CMasterWait(char mask){
    put_lf();
    putHex(SSPSTAT);
    putHex(SSPCON);
    putHex(SSPCON2);
    while ((SSPSTAT & mask) || (SSPCON2 & 0x1F));
//  putChar('Z');
//  while (SSPSTATbits.R_nW);
//  putChar('A');
//  while (SSPSTATbits.BF);
//  putChar('B');
//  while (SSPCON2bits.GCEN);
//  putChar('C');
//  while (SSPCON2bits.ACKSTAT);
//  putChar('D');
//  while (SSPCON2bits.ACKDT);
//  putChar('E');
//  while (SSPCON2bits.ACKEN);
//  putChar('F');
//  while (SSPCON2bits.SEN);
//  putChar('G');
  //SSPSTAT : 0x05 -> transmit is not in progress & buffer empty
  //          0x04 -> transmit is not in progress
  //SSPCON2 : ack,receive,start,restart,stop is idle
}


SBYTE I2CMasterStart(UBYTE slave_address,UBYTE rw){
//  I2CMasterWait();
//  SEN = 1;                      //SEN Start Condition Enable; bit 0 of SSPCON2
    putChar('S');
    putChar('T');
    putChar('A');
    putChar('R');
    putChar('T');
    put_lf();
    CollisionCheck = 0 ;
    I2CMasterWait(0x5) ;
//     sendPulseWDT();
    SSPCON2bits.SEN = 1 ;
//     I2CMasterWait(0x5) ;
//     sendPulseWDT();
    if (CollisionCheck == 1) return -1 ;
//     sendPulseWDT();
    AckCheck = 0 ;
    SSPBUF = (char)((slave_address<<1)+rw);
//     sendPulseWDT();
    while (AckCheck);
//     sendPulseWDT();
//     if (SSPCONbits.WCOL == 1){
//        CollisionCheck = 1;
//        putChar('W');
//        WCOL = 0;
//     }
    if (CollisionCheck == 1) return -1 ;
    return SSPCON2bits.ACKSTAT;
}

int I2CMasterRepeatedStart(UBYTE slave_address,UBYTE rw){
//  I2CMasterWait();
//  RSEN = 1;                     //Repeated Start Condition Enabled bit (Master mode only); bit 1 of SSPCON2
     CollisionCheck = 0 ;
     I2CMasterWait(0x5) ;
     SSPCON2bits.RSEN = 1 ;
     I2CMasterWait(0x5) ;
//     if (1) return -1 ;
     if (CollisionCheck == 1) return -1 ;
     AckCheck = 1;
     SSPBUF = (char)((slave_address<<1)+rw);
     while (AckCheck);
     if (CollisionCheck == 1) return -1;
     return SSPCON2bits.ACKSTAT;
}

int I2CMasterStop(void){
//  I2CMasterWait();
//  PEN = 1;                      //Stop Condition Enable bit (Master mode only); bit 2 of SSPCON2
    putChar('S');
    putChar('T');
    putChar('O');
    putChar('P');
    put_lf();
    CollisionCheck = 0 ;
    I2CMasterWait(0x5) ;
    SSPCON2bits.PEN = 1 ;
    if (CollisionCheck == 1) return -1 ;
    else                     return  0 ;
}

int I2CMasterWrite(UBYTE dataByte){
//  I2CMasterWait();
//  SSPBUF = dataByte;                   //Serial Receive/Transmit Buffer Register
    putChar('W');
    putChar('R');
    putChar('I');
    putChar('T');
    putChar('E');
    put_lf();
    CollisionCheck = 0 ;
    I2CMasterWait(0x5) ;
    if (CollisionCheck == 1) return -1;
//     if (1) return -1;
    AckCheck = 1;
    SSPBUF = dataByte;
    while (AckCheck);
    if (CollisionCheck == 1) return -1;
    return SSPCON2bits.ACKSTAT;
}


int I2CMasterRead(UBYTE address){
     int data_from_slave ;

     CollisionCheck = 0 ;
     I2CMasterWait(0x5) ;
     SSPCON2bits.RCEN = 1;      //  enable receive from slave
     I2CMasterWait(0x4) ;
     if (CollisionCheck == 1) return -1 ;
//     if (1) return -1 ;
     data_from_slave = SSPBUF;
     I2CMasterWait(0x5) ;
     if (CollisionCheck == 1) return -1 ;
     SSPCON2bits.ACKDT = address;
     SSPCON2bits.ACKEN = 1;
     return data_from_slave;
}


/*******************************************************************************
*Method for EEPROM Write
******************************************************************************/
int WriteToEEPROM(UBYTE addressEEPROM,UBYTE addressHigh,UBYTE addressLow,UBYTE *data){
    int ans;

    ans = I2CMasterStart(addressEEPROM,0);
    if (ans == 0){
        I2CMasterWrite(addressHigh);
        I2CMasterWrite(addressLow);
        while(*data){
            I2CMasterWrite(*data);
            ++data;
        }
//        I2CMasterStop();
    } else ans = -1;
//    I2CMasterStop();
    __delay_ms(5);
    return ans;
}

int WriteToEEPROMWithDataSize(UBYTE addressEEPROM,UBYTE addressHigh,UBYTE addressLow,UBYTE *data, UBYTE DataSize){
    int ans;

    ans = I2CMasterStart(addressEEPROM,0);
    if (ans == 0){
        I2CMasterWrite(addressHigh);
        I2CMasterWrite(addressLow);
        for(UBYTE i = 0 ; i < DataSize ; i ++){
            I2CMasterWrite(data[i]);
        }        
//        I2CMasterStop();
    } else ans = -1;
//    I2CMasterStop();
    __delay_ms(5);
    return ans;
}

/**/
int WriteOneByteToEEPROM(UBYTE addressEEPROM,UBYTE addressHigh,UBYTE addressLow,UBYTE data){
    int ans = -1;
    ans = I2CMasterStart(addressEEPROM,0);               //Start condition
    if(ans == 0){
        I2CMasterWrite(addressHigh);              //Adress High Byte
        I2CMasterWrite(addressLow);           //Adress Low Byte
        I2CMasterWrite(data);             //Data
    }else ans = -1;
    I2CMasterStop();
    __delay_ms(5);
    return ans;
}

void WriteOneByteToMainAndSubB0EEPROM(UBYTE addressHigh,UBYTE addressLow,UBYTE data){
    WriteOneByteToEEPROM(EEPROM_address,addressHigh,addressLow,data);
    WriteOneByteToEEPROM(EEPROM_subaddress,addressHigh,addressLow,data);
}

void WriteCheckByteToEEPROMs(UBYTE B0Select,UBYTE addressHigh,UBYTE addressLow,UBYTE data){
    UBYTE mainAddress;
    UBYTE subAddress;
    mainAddress = (UBYTE)(EEPROM_address | B0Select);
    subAddress = (UBYTE)(EEPROM_subaddress | B0Select);
    WriteOneByteToEEPROM(mainAddress,addressHigh,addressLow,data);
    WriteOneByteToEEPROM(subAddress,addressHigh,addressLow,data);
}

void WriteLastCommandIdToEEPROM(UBYTE last_command_ID){
    WriteCheckByteToEEPROMs(B0select_for_TXCOBCLastCommandID, HighAddress_for_TXCOBCLastCommandID, LowAddress_for_TXCOBCLastCommandID, last_command_ID);
}

void WriteLastCommandStatusToEEPROM(UBYTE command_status){
    WriteCheckByteToEEPROMs(B0select_for_TXCOBCLastCommandID, HighAddress_for_TXCOBCLastCommandStatus, LowAddress_for_TXCOBCLastCommandStatus, command_status);
}



/*******************************************************************************
*Method for EEPROM Read
******************************************************************************/
int ReadDataFromEEPROM(UBYTE Address7Bytes,UBYTE high_address,UBYTE low_address,UBYTE *ReadData, UBYTE EEPROMDataLength){
    int ans = -1;
    ans = I2CMasterStart(Address7Bytes, 0);                       //Start condition
    if ( ans == 0 ){
        I2CMasterWrite(high_address);           //Adress High Byte
        I2CMasterWrite(low_address);            //Adress Low Byte
        I2CMasterRepeatedStart(Address7Bytes,1);               //Restart condition
        for(UBYTE i = 0; i < EEPROMDataLength - 1; i++){
            ReadData[i] = I2CMasterRead(0);     //Read + Acknowledge
        }
        ReadData[EEPROMDataLength - 1] = I2CMasterRead(1);
//        I2CMasterStop();                        //Stop condition
    }else ans = -1;
//    I2CMasterStop();                        //Stop condition
    __delay_ms(5);
    return ans;
}

/*
 *  Read Data From EEPROM (the size od read data is only 1byte)
 *	arg      :   EEPROM_address, high_address, low_address
 *	return   :   EEPROM_address, high_address, low_address -> ReadData
 *	TODO     :   debug  ---> finish
 *	FIXME    :   not yet
 *	XXX      :   not yet
 */
UBYTE ReadEEPROM(UBYTE address,UBYTE high_address,UBYTE low_address){
    UBYTE dat;
    int ans = -1;
    ans = I2CMasterStart(address,0);         //Start condition
    if(ans == 0){
        I2CMasterWrite(high_address);    //Adress High Byte
        I2CMasterWrite(low_address);    //Adress Low Byte
        I2CMasterRepeatedStart(address,1);         //Restart condition
        dat = (UBYTE)I2CMasterRead(1); //Read + Acknowledge
//        I2CMasterStop();
    };
//    I2CMasterStop();
    __delay_ms(5);
    if(ans == -1) return 0xFF;
    return dat;
}

//process command data if the command type is 'I2C'
void commandSwitchI2C(UBYTE command, UBYTE slaveAdress, UBYTE *dataHigh, UBYTE *dataLow){
    switch(command){
        case 'w': //I2C write
//            I2CMasterWrite(slaveAdress);//TODO: check if method 'I2C write' is correct
//            I2CMasterWrite(dataHigh);
//            I2CMasterWrite(dataLow);
            break;
        case 'r': //I2C read
//            dataHigh = I2CMasterRead(slaveAdress);//TODO: check if method 'I2C read' is correct, especially do the pointers work as intended
//            dataLow = I2CMasterRead(slaveAdress); //TODO: what should happen with read in data?
            //TODO: write data to EEPROM
            //TODO: send Address where it is written to TXCOBC
            break;
        case 'c': //I2C buffer clear
            //TODO: write method for I2C buffer clear
            //TODO: clear: SSPBUF
            break;
        case 'b': //change I2C baud rate
            //TODO: write method for change I2C baud rate
            break;
        case 's': //set as a slave ic
            //TODO: write method for set as a slave ic
            break;
        case 'i': //measure IMU
            //TODO: write method for measure IMU
            break;
        default:
//            updateErrorStatus(error_I2C_commandSwitchI2C);
            break;
    }
}
