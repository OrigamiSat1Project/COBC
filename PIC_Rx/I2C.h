/*
 * File:   I2C.h
 * Author: Kosuke
 *
 * Created on 2017/04/18, 0:41
 */

#include <xc.h>


#ifndef I2C_H
#define	I2C_H

#include "typeDefine.h"

#define I2C_baud_rate_high 400000  //400kbps
#define I2C_baud_rate_low  100000  //100kbps
#define I2C_baud_rate_def  I2C_baud_rate_low

void InitI2CMaster(const UDWORD c);
void interruptI2C(void);
void I2CMasterWait(char);
SBYTE I2CMasterStart(unsigned char, unsigned char);
SBYTE I2CMasterRepeatedStart(unsigned char, unsigned char);
SBYTE I2CMasterStop(void);

/*write*/
int WriteToEEPROM(UBYTE addressEEPROM,UBYTE addressHigh,UBYTE addressLow,UBYTE *data);
int WriteToEEPROMWithDataSize(UBYTE, UBYTE ,UBYTE, UBYTE*, UBYTE);
void WriteToMainAndSubB0EEPROM(UBYTE addressHigh,UBYTE addressLow,UBYTE *data, UBYTE);
int WriteOneByteToEEPROM(UBYTE addressEEPROM,UBYTE addressHigh,UBYTE addressLow,UBYTE data);
void WriteOneByteToMainAndSubB0EEPROM(UBYTE addressHigh,UBYTE addressLow,UBYTE data);
void WriteCheckByteToEEPROMs(UBYTE B0Select,UBYTE addressHigh,UBYTE addressLow,UBYTE data);
void WriteLastCommandIdToEEPROM(UBYTE last_command_ID);

/*read*/
UBYTE ReadEEPROM(UBYTE, UBYTE, UBYTE);
UBYTE ReadEEPROMmainAndSub(UBYTE,UBYTE,UBYTE);
void ReadDataFromEEPROMWithDataSize(UBYTE, UBYTE, UBYTE , UBYTE*, UINT);

void I2CBufferClear(void);
void ChangeI2CBaudRate(UBYTE);

void commandSwitchI2C(UBYTE, UBYTE, UBYTE, UBYTE, UBYTE);
//void commandSwitchEEPROM(UBYTE, UBYTE, UBYTE, UBYTE, UBYTE, UBYTE*);
void commandSwitchEEPROM(UBYTE*);


#endif	/* I2C_H */
