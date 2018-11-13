
#include "UART.h"
#include "I2C.h"
#include "Type_define.h"
#include "EEPROM.h"
#include "OkError.h"

#define _XTAL_FREQ 10000000
//UBYTE EEPROMData[16];
//UINT EEPROMDataLength;


/*******************************************************************************
*setting
******************************************************************************/
void InitI2CMaster(const UDWORD c){
  SSPCON = 0b00101000;
  SSPCON2 = 0;
  SSPADD = (_XTAL_FREQ/(4*c))-1;
  SSPSTAT = 0;
}

void I2CMasterWait(){
  while ((SSPSTAT & 0x04) || (SSPCON2 & 0x1F));
}

void I2CMasterStart(){
  I2CMasterWait();
  SEN = 1;
}

void I2CMasterRepeatedStart(){
  I2CMasterWait();
  RSEN = 1;
}

void I2CMasterStop(){
  I2CMasterWait();
  PEN = 1;
}

void I2CMasterWrite(UBYTE d){
  I2CMasterWait();
  SSPBUF = d;
}

UBYTE I2CMasterRead(UBYTE a){
  UBYTE temp;
  
  I2CMasterWait();
  RCEN = 1;
  I2CMasterWait();
  temp = SSPBUF;
  
  I2CMasterWait();
  ACKDT = (a)?0:1;
  ACKEN = 1;
  return temp;
}

/*******************************************************************************
*Method for EEPROM Write 
******************************************************************************/
//need debug
void writeDataI2C(UBYTE slave_address, UBYTE address_size, UBYTE *address, UBYTE data_size, UBYTE *data){
    UBYTE Address;
    Address= (UBYTE)(slave_address << 1);

    I2CMasterStart();                  //Start condition
    I2CMasterWrite(Address);           //7 bit address + Write
    
    for(UBYTE i=0; i<address_size; i++){
        I2CMasterWrite(address[i]);    //address
    } 
    
    for(UBYTE i=0; i<data_size; i++){
        I2CMasterWrite(data[i]);       //Data
    }
    
    I2CMasterStop();                  //Stop condition
    __delay_ms(200);    
}

//debug finish
void WriteToEEPROM(UBYTE addressEEPROM,UBYTE addressHigh,UBYTE addressLow,UBYTE *data){
    UBYTE address;
    address= (UBYTE)(addressEEPROM << 1);
    //UINT Datasize = sizeof(data);
    /**/
    I2CMasterStart();               //Start condition
    I2CMasterWrite(address);        //7 bit address + Write
    I2CMasterWrite(addressHigh);    //Adress High Byte
    I2CMasterWrite(addressLow);     //Adress Low Byte
    while(*data){
        I2CMasterWrite(*data);      //Data
        ++data;
    }
    I2CMasterStop();                //Stop condition
    __delay_ms(200);
}
//debug finish
void WriteOneByteToEEPROM(UBYTE addressEEPROM,UBYTE addressHigh,UBYTE addressLow,UBYTE data){
    UBYTE address;
    address= (UBYTE)(addressEEPROM << 1);
    //UINT Datasize = sizeof(data);
    I2CMasterStart();               //Start condition
    I2CMasterWrite(address);        //7 bit address + Write
    I2CMasterWrite(addressHigh);    //Adress High Byte
    I2CMasterWrite(addressLow);     //Adress Low Byte
    I2CMasterWrite(data);      //Data
    I2CMasterStop();                //Stop condition
    __delay_ms(200);
}
//debug finish
void WriteOneByteToMainAndSubB0EEPROM(UBYTE addressHigh,UBYTE addressLow,UBYTE data){
    WriteOneByteToEEPROM(EEPROM_address,addressHigh,addressLow,data);
    WriteOneByteToEEPROM(EEPROM_subaddress,addressHigh,addressLow,data);
}
//debug finish
void WriteCheckByteToEEPROMs(UBYTE B0Select,UBYTE addressHigh,UBYTE addressLow,UBYTE data){
    UBYTE mainAddress;
    UBYTE subAddress;
    mainAddress = (UBYTE)(EEPROM_address | B0Select);
    subAddress = (UBYTE)(EEPROM_subaddress | B0Select);
    WriteOneByteToEEPROM(mainAddress,addressHigh,addressLow,data);
    WriteOneByteToEEPROM(subAddress,addressHigh,addressLow,data);
}

void WriteCheckByteToEEPROMsWithDataSize(UBYTE B0Select,UBYTE addressHigh,UBYTE addressLow,UBYTE *data, UBYTE data_size){
    for(UBYTE i=0; i<data_size; i++){
        WriteCheckByteToEEPROMs(B0Select, addressHigh, addressLow, data[i]);
    }
}

void WriteLastCommandIdToEEPROM(UBYTE last_command_ID){
    WriteCheckByteToEEPROMs(B0select_for_TXCOBCLastCommandID, HighAddress_for_TXCOBCLastCommandID, LowAddress_for_TXCOBCLastCommandID, last_command_ID);
}


/*******************************************************************************
*Method for EEPROM Read
******************************************************************************/
//need debug
void readDataI2C(UBYTE Address7Bytes, UBYTE address_size, UBYTE *address, UBYTE data_size, UBYTE *data){
    UBYTE Address = (UBYTE)(Address7Bytes << 1);
    UBYTE ReadAddress = (UBYTE)(Address | 0x01);
    I2CMasterStart();                       //Start condition
    I2CMasterWrite(Address);                //7 bit address + Write
    
    for(UBYTE i=0; i<address_size; i++){
        I2CMasterWrite(address[i]);         //Adress
    }
    
    I2CMasterRepeatedStart();               //Restart condition
    I2CMasterWrite(ReadAddress);            //7 bit address + Read
    
    for(UBYTE i = 0; i < data_size - 1; i++){
        data[i] = I2CMasterRead(1);         //Read + Acknowledge
    }
    
    data[data_size - 1] = I2CMasterRead(0);
    I2CMasterStop();                        //Stop condition   
}
//debug finish
void ReadDataFromEEPROM(UBYTE Address7Bytes,UBYTE high_address,UBYTE low_address,UBYTE *ReadData, UINT EEPROMDataLength){

    UBYTE Address = (UBYTE)(Address7Bytes << 1);
    UBYTE ReadAddress = (UBYTE)(Address | 0x01);
    I2CMasterStart();                       //Start condition
    I2CMasterWrite(Address);                //7 bit address + Write
    I2CMasterWrite(high_address);           //Adress High Byte
    I2CMasterWrite(low_address);            //Adress Low Byte
    I2CMasterRepeatedStart();               //Restart condition
    
    I2CMasterWrite(ReadAddress);            //7 bit address + Read
    for(UINT i = 0; i < EEPROMDataLength - 1; i++){
        ReadData[i] = I2CMasterRead(1);     //Read + Acknowledge
    }
    ReadData[EEPROMDataLength - 1] = I2CMasterRead(0);
    I2CMasterStop();                        //Stop condition

    //for debugging
    /*
    for(UINT j = 0; j < *EEPROMDataLength; j++){
    //putch(ReadData[0]);
    //for(UINT j = 0; j < 5; j++){
        putch(ReadData[j]);
    }
    putcrlf();*/
    __delay_ms(200);
}

//debug finish
/*
 *  Read Data From EEPROM (the size od read data is only 1byte)
 *	arg      :   EEPROM_address, high_address, low_address
 *	return   :   EEPROM_address, high_address, low_address -> ReadData
 *	TODO     :   debug  ---> finish
 *	FIXME    :   not yet
 *	XXX      :   not yet
 */
UBYTE ReadEEPROM(UBYTE Address7Bytes, UBYTE high_address, UBYTE low_address){
    UBYTE Address = (UBYTE)(Address7Bytes << 1);
    UBYTE ReadAddress = (UBYTE)(Address | 0x01);
    UBYTE ReadData;
   
    I2CMasterStart();         //Start condition
    I2CMasterWrite(Address);     //7 bit address + Write
    I2CMasterWrite(high_address);    //Adress High Byte
    I2CMasterWrite(low_address);    //Adress Low Byte
    I2CMasterRepeatedStart();         //Restart condition
    
    I2CMasterWrite(ReadAddress);     //7 bit address + Read
    
    ReadData = I2CMasterRead(0); //Read + Acknowledge
    
    I2CMasterStop();          //Stop condition
    return ReadData; 
}

//TODO:need debug  ‚à‚µ‚©‚µ‚½‚ç‚¢‚ç‚È‚¢‚©‚àH
void ReadDataAndDataSizeFromEEPROM(UBYTE Address7Bytes,UBYTE high_address,UBYTE low_address,UBYTE *ReadData, UINT *EEPROMDataLength){
    UBYTE Address = (UBYTE)(Address7Bytes << 1);
    UBYTE ReadAddress = (UBYTE)(Address | 0x01);
    I2CMasterStart();                       //Start condition
    I2CMasterWrite(Address);                //7 bit address + Write
    I2CMasterWrite(high_address);           //Adress High Byte
    I2CMasterWrite(low_address);            //Adress Low Byte
    I2CMasterRepeatedStart();               //Restart condition
    
    I2CMasterWrite(ReadAddress);            //7 bit address + Read
    for (*EEPROMDataLength = 0; ReadData[*EEPROMDataLength]!= I2Cnull; *EEPROMDataLength++);
    for(UINT i = 0; i < *EEPROMDataLength; i++){
        ReadData[i] = I2CMasterRead(1);     //Read + Acknowledge
    }
    ReadData[*EEPROMDataLength] = I2CMasterRead(0);
    I2CMasterStop();          //Stop condition
    
    //for denbugging
    /*
    for(UINT j = 0; j < *EEPROMDataLength; j++){
    //putch(ReadData[0]);
    //for(UINT j = 0; j < 5; j++){
        putch(ReadData[j]);
    }
    putcrlf();*/
    __delay_ms(200);
}

/*******************************************************************************
*Function
******************************************************************************/
//need debug
void I2CBufferClear(void){
   SSPBUF = 0;   //Serial Receive/Transmit Buffer Register
}
// debug finish
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
//need debug
//•Ï‚¦‚Ä‚à‚¢‚¢‚Å‚·
void testEEPROM(UBYTE slaveAdress, UBYTE test_type){
    UBYTE read_data[TEST_DATA_SIZE] = {0};
    UBYTE error_status = 0x00;
    UBYTE write_data = 'w';
    switch (test_type){
        case 'r':
            ReadDataFromEEPROM(slaveAdress, TestEEPROMread_addressHigh, TestEEPROMread_addressLow,read_data, TEST_DATA_SIZE);
            if(read_data[0]!='T'){
                error_status += 0x21;
            }
            if(read_data[1]!='E'){
                error_status += 0x21;
            }
            if(read_data[2]!='S'){
                error_status += 0x21;
            }
            if(read_data[3]!='T'){
                error_status += 0x21;
            }
            WriteOneByteToMainAndSubB0EEPROM(TXCOBC_CommandErrorStatus_addressHigh,TXCOBC_CommandErrorStatus_addressLow, error_status);
            break;
        case 'w':
            WriteOneByteToMainAndSubB0EEPROM(TestEEPROMwrite_addressHigh,TestEEPROMwrite_addressHigh, write_data);
            read_data[0] = ReadEEPROM(EEPROM_address, TestEEPROMwrite_addressHigh,TestEEPROMwrite_addressHigh);
            read_data[1] = ReadEEPROM(EEPROM_subaddress, TestEEPROMwrite_addressHigh,TestEEPROMwrite_addressHigh);
            if(read_data[0]!='w'){
                error_status += 0x23;
            }
            if(read_data[1]!='w'){
                error_status += 0x25;
            } 
            WriteOneByteToMainAndSubB0EEPROM(TXCOBC_CommandErrorStatus_addressHigh,TXCOBC_CommandErrorStatus_addressLow, error_status);
            break;
    }
}

//process command data if the command type is 'I2C'
void commandSwitchI2C(UBYTE command, UBYTE slaveAdress_baudRate, UBYTE addressSize, UBYTE *address, UBYTE dataSize, UBYTE *data){ 
    switch(command){    
        case 'w': //I2C write  //TODO:need debug
            writeDataI2C(slaveAdress_baudRate, addressSize,  *address,  dataSize,  *data);
            break;
        case 'r': //I2C read  //TODO:need debug
            readDataI2C(slaveAdress_baudRate, addressSize, *address, dataSize, *data);
            //TODO:down link data FM or CW
            break;
        case 't': //I2C test
            //TODO: write method for I2C test
            //TODO: write test data to EEPROM
            //TODO: read EEPRON
            //TODO: send EEPROM address to TXCOBC
            break;
        case 'c': //I2C buffer clear
            I2CBufferClear();
            break;
        case 'b': //change I2C baud rate
            //TODO: write method for change I2C baud rate
            ChangeI2CBaudRate(slaveAdress_baudRate);
            break;
        case 's': //set as a slave ic
            //TODO: write method for set as a slave ic
            break;
        case 'i': //measure IMU
            //TODO: write method for measure IMU
            break;
        default:
            switchError(error_I2C_commandSwitchI2C);
            break;
    }
}

void commandSwitchEEPROM(UBYTE command, UBYTE B0B1select_slaveAdress, UBYTE dataHigh_type, UBYTE dataLow, UBYTE data_size, UBYTE data1, UBYTE data2, UBYTE data3,UBYTE data4, UBYTE data5){ 
    UBYTE eeprom_data[5];
    switch(command){    
        case 'w': //write data to EEPROM  //TODO:need debug
            eeprom_data[0] = data1;
            eeprom_data[1] = data2;
            eeprom_data[2] = data3;
            eeprom_data[3] = data4;
            eeprom_data[4] = data5;
            WriteCheckByteToEEPROMsWithDataSize(B0B1select_slaveAdress, dataHigh_type, dataLow, eeprom_data, data_size);
            break;            
        case 't': //EEPROM test   //TODO:need debug
            testEEPROM(B0B1select_slaveAdress, dataHigh_type);
            break;
        default:
            // switchError(error_I2C_commandSwitchEEPROM);
            break;
    }
}

/*******************************************************************************
*for debug
******************************************************************************/
void debugForCommandSwitchI2C(void){
    putChar('S');
    put_ok();
    
    UBYTE command = 0xcc;
    UBYTE slaveAdress_baudRate = 0xcc;
    UBYTE addressSize = 0xcc;
    UBYTE address[2] = {0x00};
    UBYTE dataSize = 0xcc;
    UBYTE data[3] = {0x00};
    
    /*--change baud rate---*/
    command = 'b';
    slaveAdress_baudRate = 0x00; // 0x00 / 0x01
    commandSwitchI2C(command, slaveAdress_baudRate, addressSize, address, dataSize, data);
    
    /*---write I2C---*/
    command = 'w';
    slaveAdress_baudRate = EEPROM_address;
    addressSize = 2;
    address[0] = whigh_address;
    address[1] = wlow_address;  
    dataSize = 3;
    data[0] = '1';
    data[1] = '2';
    data[2] = '3';
    commandSwitchI2C(command, slaveAdress_baudRate, addressSize, address, dataSize, data);

    putChar('W');
    put_ok();
    
    /*--read I2C---*/
    command = 'r';
    UBYTE read_data[3] = {0x00};
    commandSwitchI2C(command, slaveAdress_baudRate, addressSize, address, dataSize, read_data);
    
    for(UBYTE i=0; i<3; i++){
        putChar(read_data[i]);
    }   
    putChar('R');
    put_ok();
}

void debugForCommandSwitchEEPROM(void){
    
    putChar('S');
    put_ok();    
    
    /*---write data to EEPROM---*/
    UBYTE command = 'w';
    UBYTE B0B1select_slaveAdress = 0x00;
    UBYTE dataHigh_type = whigh_address;
    UBYTE dataLow = wlow_address;
    UBYTE data_size = 5;
    UBYTE data1 = 'T';
    UBYTE data2 = 'E';
    UBYTE data3 = 'S';
    UBYTE data4 = 'T';
    UBYTE data5 = '!';    
    
    commandSwitchEEPROM(command, B0B1select_slaveAdress, dataHigh_type, dataLow, data_size, data1, data2, data3, data4, data5);
    
    putChar('W');
    put_ok();
    
    /*---read data from EEPROM---*/
    UBYTE ReadData_main[5];
    UBYTE ReadData_sub[5];
    //B0
    ReadDataFromEEPROM(EEPROM_address, dataHigh_type, dataLow, ReadData_main, data_size);
    ReadDataFromEEPROM(EEPROM_subaddress, dataHigh_type, dataLow, ReadData_sub, data_size);
    //B1
    ReadDataFromEEPROM(EEPROM_address_B1, dataHigh_type, dataLow, ReadData_main, data_size);
    ReadDataFromEEPROM(EEPROM_subMaddress_B1, dataHigh_type, dataLow, ReadData_sub, data_size);
    
    for(UBYTE i=0; i<5; i++){
        putChar(ReadData_main[i]);
        putChar(ReadData_sub[i]);
    }
    
    putChar('R');
    put_ok();

    /*---EEPROM test(read)---*/
    UBYTE command = 't';
    UBYTE B0B1select_slaveAdress = EEPROM_address; //EEPROM_address / EEPROM_subaddress / EEPROM_address_B1 / EEPROM_subMaddress_B1
    UBYTE dataHigh_type = 'r';
    
    commandSwitchEEPROM(command, B0B1select_slaveAdress, dataHigh_type, dataLow, data_size, data1, data2, data3, data4, data5);
    putChar(ReadEEPROM(EEPROM_address, TXCOBC_CommandErrorStatus_addressHigh, TXCOBC_CommandErrorStatus_addressLow));
    putChar(ReadEEPROM(EEPROM_subaddress, TXCOBC_CommandErrorStatus_addressHigh, TXCOBC_CommandErrorStatus_addressLow));

    putChar('T');
    put_ok();
    
    /*---EEPROM test(write)---*/
    UBYTE command = 't';
    UBYTE B0B1select_slaveAdress = EEPROM_address; //EEPROM_address / EEPROM_subaddress / EEPROM_address_B1 / EEPROM_subMaddress_B1
    UBYTE dataHigh_type = 'w';
    
    commandSwitchEEPROM(command, B0B1select_slaveAdress, dataHigh_type, dataLow, data_size, data1, data2, data3, data4, data5);
    putChar(ReadEEPROM(EEPROM_address, TXCOBC_CommandErrorStatus_addressHigh, TXCOBC_CommandErrorStatus_addressLow));
    putChar(ReadEEPROM(EEPROM_subaddress, TXCOBC_CommandErrorStatus_addressHigh, TXCOBC_CommandErrorStatus_addressLow));

    putChar('T');
    put_ok();
    
}
