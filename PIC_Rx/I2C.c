#include <xc.h>
#include "typeDefine.h"
#include "pinDefine.h"
#include "time.h"
#include "MPU.h"
#include "EEPROM.h"
#include "I2C.h"
#include "OkError.h"
#include "UART.h"
#include "timer.h"

//UBYTE EEPROMData[32];

#define MELTING_FINISH_FLAG 0b01111110

/*******************************************************************************
*setting
******************************************************************************/
void InitI2CMaster(const UDWORD c){//Init Master Synchronous Serial Port(MSSP)
  SSPCON = 0b00101000;          //MSSP Control Register: Synchronous Serial Port Enabled;I2C Master mode, clock = FOSC/(4 * (SSPADD + 1))
  SSPCON2 = 0;                  //MSSP Control Register 2:
  SSPADD = (_XTAL_FREQ/(4*c))-1; //MSSP Address Register: the lower seven bits of SSPADD act as the baud rate generator reload value
  SSPSTAT = 0;                  //MSSP Status Register
  PIE1bits.SSPIE  = 1 ;               // enable MSSP interrupt
  PIE2bits.BCLIE  = 1 ;               // enable bus collision interrupt
  PIR1bits.SSPIF  = 0 ;               // clear MSSP interrupt flag
  PIR2bits.BCLIF  = 0 ;               // clear bus collision flag
}



void I2CMasterWait(char mask){
  while ((SSPSTAT & mask) || (SSPCON2 & 0x1F));
  //SSPSTAT : 0x05 -> transmit is not in progress & buffer empty
  //          0x04 -> transmit is not in progress
  //SSPCON2 : ack,receive,start,restart,stop is idle
}


int I2CMasterStart(UBYTE slave_address,UBYTE rw){
//  I2CMasterWait();
//  SEN = 1;                      //SEN Start Condition Enable; bit 0 of SSPCON2
     CollisionCheck = 0 ;
     I2CMasterWait(0x5) ;
     SSPCON2bits.SEN = 1 ;
     I2CMasterWait(0x5) ;
     if (CollisionCheck == 1) return -1 ;
     AckCheck = 1 ;
     SSPBUF = (char)((slave_address<<1)+rw);
     while (AckCheck);
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
     CollisionCheck = 0 ;
     I2CMasterWait(0x5) ;
     SSPCON2bits.PEN = 1 ;
     if (CollisionCheck == 1) return -1 ;
     else                     return  0 ;
}

int I2CMasterWrite(UBYTE dataByte){
//  I2CMasterWait();
//  SSPBUF = dataByte;                   //Serial Receive/Transmit Buffer Register
     CollisionCheck = 0 ;
     I2CMasterWait(0x5) ;
     if (CollisionCheck == 1) return -1;
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
/*
 *  Write To EEPROM
 *	arg      :   addressEEPROM, addressHigh, addressLow, *data
 *	return   :   EEPROM_address, high_address, low_address -> *data
 *	TODO     :   debug --->finish
 *	FIXME    :   not yet
 *	XXX      :   not yet
 */

void WriteToMainAndSubB0EEPROM(UBYTE addressHigh,UBYTE addressLow,UBYTE *data,UBYTE datasize){
    WriteToEEPROMWithDataSize(MAIN_EEPROM_ADDRESS,addressHigh,addressLow,data,datasize);
    WriteToEEPROMWithDataSize(SUB_EEPROM_ADDRESS,addressHigh,addressLow,data,datasize);
}

/*
 *  Write To EEPROM
 *	arg      :   addressEEPROM, addressHigh, addressLow, *data, dataSize
 *	return   :   EEPROM_address, high_address, low_address, dataSize -> *data
 *	TODO     :   debug --->finish
 *	FIXME    :   not yet
 *	XXX      :   not yet
 */
int WriteToEEPROMWithDataSize(UBYTE addressEEPROM,UBYTE addressHigh,UBYTE addressLow,UBYTE *data, UBYTE DataSize){
    int ans;

    ans = I2CMasterStart(addressEEPROM,0);
    if (ans == 0){
        I2CMasterWrite(addressHigh);
        I2CMasterWrite(addressLow);
        for(UBYTE i = 0 ; i < DataSize ; i ++){
            I2CMasterWrite(data[i]);
        }
    }else ans = -1;
    I2CMasterStop();
    __delay_ms(5);
    return ans;
}

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
    WriteOneByteToEEPROM(MAIN_EEPROM_ADDRESS,addressHigh,addressLow,data);
    WriteOneByteToEEPROM(SUB_EEPROM_ADDRESS,addressHigh,addressLow,data);
}

void WriteCheckByteToEEPROMs(UBYTE B0Select,UBYTE addressHigh,UBYTE addressLow,UBYTE data){
    UBYTE mainAddress;
    UBYTE subAddress;
    mainAddress = MAIN_EEPROM_ADDRESS | B0Select;
    subAddress = SUB_EEPROM_ADDRESS | B0Select;
    WriteOneByteToEEPROM(mainAddress,addressHigh,addressLow,data);
    WriteOneByteToEEPROM(subAddress,addressHigh,addressLow,data);
}

void WriteLastCommandIdToEEPROM(UBYTE last_command_ID){
    WriteCheckByteToEEPROMs(B0select_for_RXCOBCLastCommand, HighAddress_for_RXCOBCLastCommandID, LowAddress_for_RXCOBCLastCommandID, last_command_ID);
}

/*******************************************************************************
*Method for EEPROM Read
******************************************************************************/
/*
 *  Read Data From EEPROM (the size od read data is only 1byte)
 *	arg      :   EEPROM_address, high_address, low_address
 *	return   :   EEPROM_address, high_address, low_address -> ReadData
 *	TODO     :   debug  ---> finish
 *	FIXME    :   not yet
 *	XXX      :   not yet
 */
UBYTE ReadEEPROM(UBYTE EEPROM_address,UBYTE high_address,UBYTE low_address){
    UBYTE dat;
    int ans = -1;
    ans = I2CMasterStart(EEPROM_address,0);         //Start condition
    if(ans == 0){
        I2CMasterWrite(high_address);    //Adress High Byte
        I2CMasterWrite(low_address);    //Adress Low Byte
        I2CMasterRepeatedStart(EEPROM_address,1);         //Restart condition
        dat = (UBYTE)I2CMasterRead(1); //Read + Acknowledge
    }
    I2CMasterStop();
    __delay_ms(5);
    if(ans == -1) return 0xFF;
    return dat;
}

UBYTE ReadEEPROMmainAndSub(UBYTE B0select,UBYTE high_address,UBYTE low_address){
    UBYTE mainaddress = MAIN_EEPROM_ADDRESS | B0select;
    UBYTE subaddress = SUB_EEPROM_ADDRESS | B0select;
    UBYTE ReadData;
    ReadData = ReadEEPROM(mainaddress ,high_address,low_address);
    if (ReadData==0xFF){
        ReadData = ReadEEPROM(subaddress, high_address,low_address);
    }
    return ReadData;
}



/*
 *  Read Data From EEPROM With DataSize
 *	arg      :   EEPROM_address, high_address, low_address, *ReadData, EEPROMDataLength
 *	return   :   EEPROM_address, high_address, low_address, EEPROMDataLength -> *ReadData
 *	TODO     :   debug ---> finish
 *	FIXME    :   not yet
 *	XXX      :   not yet
 */
void ReadDataFromEEPROMWithDataSize(UBYTE EEPROM_address,UBYTE high_address,UBYTE low_address,UBYTE *ReadData, UINT EEPROMDataLength){
    int ans = -1;
    ans = I2CMasterStart(EEPROM_address, 0);                       //Start condition
    if ( ans == 0 ){
        I2CMasterWrite(high_address);           //Adress High Byte
        I2CMasterWrite(low_address);            //Adress Low Byte
        I2CMasterRepeatedStart(EEPROM_address,1);               //Restart condition
        for(UBYTE i = 0; i < EEPROMDataLength - 1; i++){
            ReadData[i] = I2CMasterRead(0);     //Read + Acknowledge
        }
        ReadData[EEPROMDataLength - 1] = I2CMasterRead(1);
    }
    I2CMasterStop();                        //Stop condition
    __delay_ms(5);
}

/*******************************************************************************
*Method for various function
******************************************************************************/

//TODO:check
void I2CBufferClear(void){
   SSPBUF = 0;   //Serial Receive/Transmit Buffer Register
}

/*
 *  Change I2C Baud Rate
 *	arg      :   I2C_baud_rate_type (0x00/0x01)
 *	return   :   0x00->high-speed mode (400 kHz) / 0x01->standard speed mode (100 kHz)
 *	TODO     :   debug ---> finish
 *	FIXME    :   not yet
 *	XXX      :   not yet
 *  default 100000bps / datasheet p81-p82
 */
void ChangeI2CBaudRate( UBYTE I2C_baud_rate_type ){
    switch (I2C_baud_rate_type){
        case 0x00:     //high-speed mode (400 kHz)
            SMP = 0;  //Slew Rate Control bit
            SSPADD = (_XTAL_FREQ/(4*I2C_baud_rate_high))-1; //MSSP Address Register: the lower seven bits of SSPADD act as the baud rate generator reload value
            break;
        case 0x01:     //standard speed mode (100 kHz)
            SMP = 1;  //Slew Rate Control bit
            SSPADD = (_XTAL_FREQ/(4*I2C_baud_rate_low))-1; //MSSP Address Register: the lower seven bits of SSPADD act as the baud rate generator reload value
            break;
    }
}

/*******************************************************************************
*process command data if the command type is 'I2C'
******************************************************************************/
void commandSwitchI2C(UBYTE command, UBYTE slaveAdress, UBYTE dataHigh, UBYTE dataLow, UBYTE data){
    switch(command){
        case 'w': //I2C write
            I2CMasterWrite(slaveAdress);//TODO: check if method 'I2C write' is correct
            I2CMasterWrite(dataHigh);
            I2CMasterWrite(dataLow);
            break;
        case 'r': //I2C read
            dataHigh = I2CMasterRead(slaveAdress);//TODO: check if method 'I2C read' is correct, especially do the pointers work as intended
            dataLow = I2CMasterRead(slaveAdress); //TODO: what should happen with read in data?
            //TODO: write data to EEPROM
            //TODO: send Address where it is written to TXCOBC
            break;
        case 't': //I2C test
            //TODO: write method for I2C test (OBC and TXCOBC)
            //TODO: write test data to EEPROM
            //TODO: read EEPROM
            //TODO: send EEPROM address to TXCOBC
            break;
        case 'c': //I2C buffer clear
            //TODO: write method for I2C buffer clear---finish?
            //TODO: clear: SSPBUF---finish?
            I2CBufferClear();
            break;
        case 'b': //change I2C baud rate
            //TODO: write method for change I2C baud rate---finish
            ChangeI2CBaudRate( slaveAdress );
            break;
        default:
            switchError(error_I2C_commandSwitchI2C);
            break;
    }
}

/*******************************************************************************
*process command data if the command type is 'EEPROM'
******************************************************************************/
void commandSwitchEEPROM(UBYTE command, UBYTE slaveAdress, UBYTE dataHigh, UBYTE dataLow, UBYTE data1, UBYTE *data2){
    UBYTE data_length;
    UBYTE *read_data;
    switch(command){
        case 'w': //write data to EEPROM
            WriteToEEPROMWithDataSize(slaveAdress, dataHigh, dataLow, data2, data1);  //data1 is the data to send
            break;
        case 'r': //read data from EEPROM
            data_length = data1;
            ReadDataFromEEPROMWithDataSize(slaveAdress, dataHigh, dataLow, read_data, data1);
            //TODO: send data to TXCOBC or/and OBC by I2C or UART
            break;
        case 'm': //write melting status to EEPROM --> stop melting
            /*---write melting status to EEPROM---*/
            WriteOneByteToMainAndSubB0EEPROM(MeltingStatus_addressHigh, MeltingStatus_addressLow, MELTING_FINISH_FLAG);
            /*---change melting status PIN : low -> high ---*/
            MRLTING_FLAG_FOR_OBC = HIGH;
            break;
        default:
            switchError(error_I2C_commandSwitchEEPROM);
            break;
    }
}
