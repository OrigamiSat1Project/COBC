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

void updateErrorStatus(UBYTE error_srarus){
    WriteOneByteToMainAndSubB0EEPROM(TXCOBC_CommandErrorStatus_addressHigh, TXCOBC_CommandErrorStatus_addressLow, error_srarus);
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