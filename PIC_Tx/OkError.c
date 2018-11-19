/*
 * File:   OkError
 * Author: Madoka
 * Revision history: v1.0
 */

#include "Type_define.h"
#include "UART.h"
#include "time.h"
#include "EEPROM.h"
#include "FMCW.h"
#include "I2C.h"
#include "OkError.h"
#include <xc.h>
#include <stdio.h>

void switchOk(UBYTE action_select){
    UBYTE mainAddress;
    UBYTE subAddress;
    UBYTE Ok_data[2];
    mainAddress = EEPROM_address | B0select_for_Ok;
    subAddress = EEPROM_subaddress | B0select_for_Ok;
    /*Ok_data[0]->coomandID*/
    Ok_data[0] = ReadEEPROM(mainAddress, HighAddress_for_TXCOBCLastCommandID, LowAddress_for_TXCOBCLastCommandID);
    Ok_data[1] = action_select;
    WriteToEEPROMWithDataSize(EEPROM_address, HighAddress_for_Ok, LowAddress_for_Ok, Ok_data,2);
    WriteToEEPROMWithDataSize(EEPROM_subaddress, HighAddress_for_Ok, LowAddress_for_Ok, Ok_data,2);
}

void switchError(UBYTE action_select){
    UBYTE mainAddress;
    UBYTE subAddress;
    UBYTE error_data[2];
    mainAddress = EEPROM_address | B0select_for_Error;
    subAddress = EEPROM_subaddress | B0select_for_Error;
    /*error_data[0]->coomandID*/
    error_data[0] = ReadEEPROM(mainAddress, HighAddress_for_TXCOBCLastCommandID, LowAddress_for_TXCOBCLastCommandID);
    error_data[1] = action_select;
    WriteToEEPROMWithDataSize(EEPROM_address, HighAddress_for_Error, LowAddress_for_Error, error_data,2);
    WriteToEEPROMWithDataSize(EEPROM_subaddress, HighAddress_for_Error, LowAddress_for_Error, error_data,2);
}

void putErrorNoDownlink(UBYTE action_select){
    UBYTE mainAddress;
    UBYTE subAddress;
    UBYTE error_data[2];
    mainAddress = EEPROM_address | B0select_for_Error;
    subAddress = EEPROM_subaddress | B0select_for_Error;
    /*error_data[0]->coomandID*/
    error_data[0] = ReadEEPROM(mainAddress, HighAddress_for_TXCOBCLastCommandID, LowAddress_for_TXCOBCLastCommandID);
    error_data[1] = action_select;
    WriteToEEPROMWithDataSize(EEPROM_address, HighAddress_for_Error, LowAddress_for_Error, error_data,2);
    WriteToEEPROMWithDataSize(EEPROM_subaddress, HighAddress_for_Error, LowAddress_for_Error, error_data,2);
}

//ex: 0b01101011 -> 0+1+1+0+1+0+1+1=5
UBYTE bitCalResult(UBYTE bit_input){
    UBYTE bit_cal_result = 0;
    for(UBYTE cal_counter=0; cal_counter<8; cal_counter++){
        if((bit_input & 1)==1){
            bit_cal_result += 1;
        } else {
            bit_cal_result += 0;
        }
        bit_input >>= 1;
    }
    return bit_cal_result;
}

UBYTE checkMeltingStatus(UBYTE e_address){
    /*---read melting status---*/
    UBYTE temp;
    temp = ReadEEPROM(e_address, MeltingStatus_addressHigh, MeltingStatus_addressLow);
        
    /*---0xff -> I2C error -> melting status is reset---*/
    if(temp==0xff){
        temp = 0x00;
    }
    
    /*---bit operation---*/
    //ex: 0b01101011 -> 0+1+1+0+1+0+1+1=5
    temp = bitCalResult(temp);
    return temp;
}