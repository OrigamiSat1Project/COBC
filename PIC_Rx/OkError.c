/*
 * File:   OkError
 * Author: Madoka
 * Revision history: v1.0
 */

#include "typeDefine.h"
#include "UART.h"
#include "EEPROM.h"
#include "FMCW.h"
#include "I2C.h"
#include "OkError.h"
#include <xc.h>
#include <stdio.h>

void updateErrorStatus(UBYTE error_status){
    WriteOneByteToMainAndSubB0EEPROM(HighAddress_for_RXCOBCLastCommandStatus, LowAddress_for_RXCOBCLastCommandStatus, error_status);
}
