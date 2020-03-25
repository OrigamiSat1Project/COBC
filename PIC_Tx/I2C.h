/*
 * File:   I2C.h
 * Author: Kosuke
 *
 * Created on 2017/04/18, 0:41
 */

#include <xc.h>
#include "Type_define.h"

#ifndef I2C_H
#define	I2C_H

//#define I2Cbps 400000 //400kbps
#define I2Cbps 100000 //100kbps
#define I2Cnull 0xFF

/*******************************************************************************
*setting
******************************************************************************/
void InitI2CMaster(const UDWORD c);
void interruptI2C(void);
void I2CMasterWait(char);
SBYTE I2CMasterStart(unsigned char, unsigned char);
int I2CMasterRepeatedStart(unsigned char, unsigned char);
int I2CMasterStop(void);

int AckCheck = 0;
int CollisionCheck = 1;


/*******************************************************************************
*Method for EEPROM Write
******************************************************************************/
int WriteToEEPROM(UBYTE,UBYTE,UBYTE,UBYTE *);
int WriteToEEPROMWithDataSize(UBYTE, UBYTE ,UBYTE, UBYTE*, UBYTE);
int WriteOneByteToEEPROM(UBYTE,UBYTE,UBYTE,UBYTE);
void WriteOneByteToMainAndSubB0EEPROM(UBYTE addressHigh,UBYTE addressLow,UBYTE data);
void WriteLastCommandIdToEEPROM(UBYTE);
void WriteLastCommandStatusToEEPROM(UBYTE);
void WriteCheckByteToEEPROMs(UBYTE,UBYTE,UBYTE,UBYTE);

/*******************************************************************************
*Method for EEPROM Read
******************************************************************************/
int ReadDataFromEEPROM(UBYTE ,UBYTE ,UBYTE,UBYTE *,UBYTE);
UBYTE ReadEEPROM(UBYTE, UBYTE, UBYTE);
void ReadDataAndDataSizeFromEEPROM(UBYTE ,UBYTE ,UBYTE,UBYTE *,UBYTE *);
void commandSwitchI2C(UBYTE , UBYTE , UBYTE *, UBYTE *);


#endif	/* I2C_H */
