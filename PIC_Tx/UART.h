/* 
 * File:   UART.h
 * Author: Curie
 *
 * Created on 2017/01/21, 18:51
 */


#ifndef UART_H
#define	UART_H

#include <xc.h> // include processor files - each processor file is guarded.  
#include "Type_define.h"

#define NOT_RECEIVE 0x00
#define UNCORRECT_RECEIVE 0x01
#define CORRECT_RECEIVE 0x02

UBYTE ReceiveFlag = NOT_RECEIVE;

void Init_SERIAL(void);
void putHex(UBYTE data);
UBYTE getChar(void);
void putChar(UBYTE);                  // Writes a character to the serial port
void putString(UBYTE *);

/*Method*/
void changeInterruptPermission(UBYTE, UBYTE);
void put_error(void);
void put_ok(void);
void put_lf(void);
void changeBaudRate(UBYTE,UBYTE,UBYTE);
void UARTbufferClear(void);

/*Switch*/
void commandSwitchUART(UBYTE command, UBYTE data1, UBYTE data2, UBYTE data3, UBYTE data4, UBYTE data5);

#ifdef	__cplusplus
}
#endif

#endif	/* UART_H */

