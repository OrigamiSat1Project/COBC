#include <xc.h>
#include "Type_define.h"
#include "adxl345.h"
#include "I2C.h"
#include "EEPROM.h"
#include "Time.h"

const UBYTE ADXL345_ADDR            = 0x1D;
const UBYTE ADXL345_DEVID           = 0x00;
const UBYTE ADXL345_DEVID_VALUE     = 0xE5;
const UBYTE ADXL345_BW_RATE         = 0x2C;
const UBYTE ADXL345_DATA_FORMAT     = 0x31;
const UBYTE ADXL345_FIFO_CTL        = 0x38;
const UBYTE ADXL345_POWER_CTL       = 0x2D;
const UBYTE ADXL345_DATA_START      = 0x32;
const UBYTE ADXL345_DATA_STOP       = 0x37;
const UBYTE ADXL345_INT_SOURCE      = 0x30;
const UBYTE ADXL345_INT_ENABLE      = 0x2E;
//
////
int readADXLAddr(char address)
{
    int ans;
    ans = I2CMasterStart(ADXL345_ADDR,0);
    if(ans == 0){
        I2CMasterWrite(address);
        I2CMasterRepeatedStart(ADXL345_ADDR,1);
        ans = I2CMasterRead(1);
    }else ans = -1;
    I2CMasterStop();
    return ans;
}

int writeADXLAddr(char address, char val)
{
    int ans;
    ans = I2CMasterStart(ADXL345_ADDR,0);
    if(ans == 0){
        I2CMasterWrite(address);
        I2CMasterWrite(val);
    }else ans = -1;
    I2CMasterStop();
    return ans;
}

int initADXL()
{
    int ans;
    __delay_us(2000);
    ans = readADXLAddr(ADXL345_DEVID);
    if(ans == ADXL345_DEVID_VALUE){
        writeADXLAddr(ADXL345_POWER_CTL,0x08);      // autosleep=off,mode=measure
        writeADXLAddr(ADXL345_INT_ENABLE,0x80);     // interupe enable
        writeADXLAddr(ADXL345_BW_RATE,0x0B);        // rate = 200Hz
        writeADXLAddr(ADXL345_DATA_FORMAT,0x0B);    // proto=I2C,full resolution mode,range=16g
        writeADXLAddr(ADXL345_FIFO_CTL,0x00);       // FIFO=bypass
        __delay_us(2000);
    }else ans = -1;
    return ans;
}

int readADXL(UBYTE *data, int offset)
{
    int ans , i , ack;
    //UBYTE who[8] = {};

    while(ans != 0x80){
        ans = readADXLAddr(ADXL345_INT_SOURCE);
        ans = ans & 0x80;
    }
    
    ans = I2CMasterStart(ADXL345_ADDR,0);
    if(ans == 0){
        I2CMasterWrite(ADXL345_DATA_START);
        I2CMasterRepeatedStart(ADXL345_ADDR,1);
        ack = 0;
        for(i=0;i<6;i++){
            if(i==5) ack = 1;
            data[offset+i] = I2CMasterRead(ack);
        }
    }else ans = -1;
    I2CMasterStop();
    return ans;
}