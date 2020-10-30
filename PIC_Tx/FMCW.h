/* 
 * File:   FMCW.h
 * Author: Kosuke
 *
 * Created on 2017/04/18, 17:45
 */

#ifndef FMCW_H
#define	FMCW_H

#include <xc.h>
#include <stdio.h>
#include "Type_define.h"

//***Read Me!!!
//***comentout methods are defined in FMCW.c

// #define SHORT_DELAYTIMES_FOR_MORSE     15000  //80wpm
// #define MIDDLE_DELAYTIMES_FOR_MORSE    45000
// #define LONG_DELAYTIMES_FOR_MORSE      105000
//#define SHORT_DELAYTIMES_FOR_MORSE     30000  //40wpm
// #define MIDDLE_DELAYTIMES_FOR_MORSE    90000
// #define LONG_DELAYTIMES_FOR_MORSE      210000

#define SHORT_DELAYTIMES_FOR_MORSE     65800  //20wpm
#define MIDDLE_DELAYTIMES_FOR_MORSE    197400
#define LONG_DELAYTIMES_FOR_MORSE      460600
//#define ADD_BLANK_FOR_MORSE            LONG_DELAYTIMES_FOR_MORSE-MIDDLE_DELAYTIMES_FOR_MORSE
#define ADD_BLANK_FOR_MORSE            LONG_DELAYTIMES_FOR_MORSE


#define FM_DATA_SIZE    36
#define FM_FREE_DATA_MAX_SIZE  12

/*Downlink Command*/
void downlinkReceivedCommand(UBYTE, UBYTE, UBYTE, UBYTE);

/*Function : FM and CW downkink */
void _NOP(void);

/*FM*/
void downlinkFMSignal(UBYTE,UBYTE, UBYTE, UBYTE, UBYTE, UBYTE);

/*Function : HK downlink*/
void HKDownlink(void);
void HKDownlinkFR0(void);
void HKDownlinkFR1(void);
void HKDownlinkFR2(void);

/*Function : CW downlink*/

void commandSwitchCWDownlink(UBYTE, UBYTE, UBYTE, UBYTE, UBYTE, UBYTE, UBYTE);
long changeCharMorse (char);
char changeBinaryToChar(UBYTE);
void DevideDataAndChangeBinaryToChar (UBYTE, UBYTE*);
void sendMorse(char*,size_t);
void ReadOneByteDataFromEEPROMandSendMorse(UBYTE, UBYTE, UBYTE);
void ReadDatasFromEEPROMWithDataSizeAndSendMorse(UBYTE, UBYTE, UBYTE, UBYTE*, UINT);
void ReadDatasFromEEPROMWithDataSizeAndSendMorseWithDownlinkTimes(UBYTE, UBYTE, UBYTE, UBYTE *, UINT, UBYTE);
void GetDatasizeAndReadDatasFromEEPROMWithDataSizeAndSendMorseWithDownlinkTimes(UBYTE, UBYTE, UBYTE, UBYTE *, UINT, UBYTE, UBYTE);

void Init_HK(void);//for debug

#endif	/* FMCW_H */

