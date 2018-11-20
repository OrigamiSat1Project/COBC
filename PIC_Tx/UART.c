#include <xc.h>
#include "UART.h"
#include "Type_define.h"
#include "encode_AX25.h"
#include "CRC16.h"
#include "time.h"
#include "I2C.h"
#include "OkError.h"

void Init_SERIAL(void){

    SPBRG  = 10;                   // boudrate is  14400 bps at BRGH = 0
    BRGH   = 0;                   	// Fast baudrate
	SYNC   = 0;						// Asynchronous
	SPEN   = 1;						// Enable serial port pins
	CREN   = 1;						// Enable reception
	SREN   = 0;						// No effect
    INTCON  = 0b11000000;
    PIR1    = 0b00100000;                     // RX frag Reset
    PIE1    = 0b00101000;
    PIE2    = 0b00000000;
	TX9    = 0;						// 8-bit transmission
	RX9    = 0;						// 8-bit reception
	TXEN   = 0;						// Reset transmitter
	TXEN   = 1;						// Enable the transmitter

}

UBYTE getChar(void){              
    /**/
	if(FERR || OERR) // If over run error, then reset the receiver
	{
        CREN = 0;
        NOP();
        CREN = 1;
    }
    
    UINT break_counter = 0;

    
	while(!RCIF){
        break_counter ++;
        if(break_counter >= 2000){
            NOP();
            break_counter = 0;
            break;
        }
    }
    RCIF = 0;
    return RCREG;
}

void putChar(UBYTE byte){
    while(!TXIF);  
	TXREG = byte;
}

void putString(UBYTE *x)
{
    while(*x != '\0'){
        putChar(*x);
        x++;
    }
}
/*
 *	change Interrupt Permission
 *	arg      :   GIE_status, PEIE_status
 *               GIE: Global Interrupt Enable bit / PEIE: Peripheral Interrupt Enable bit
 *	return   :   GIE_status : 1 -> Enables all unmasked interrupts, 0 -> Disables all interrupts
 *               PEIE_status : 1 -> Enables all unmasked peripheral interrupts, 0 -> Disables all peripheral interrupts
 *	TODO     :   debug--->finish
 *	FIXME    :   not yet
 *	XXX      :   not yet
 */
void changeInterruptPermission(UBYTE GIE_status, UBYTE PEIE_status){
    if (GIE_status == 0x01){
        INTCONbits.GIE  = 1;
    } else {
        INTCONbits.GIE  = 0;
    }
    
    if (PEIE_status == 0x01){
        INTCONbits.PEIE  = 1;
    } else {
        INTCONbits.PEIE  = 0;
    }
}

/*---for debug---*/
void put_error(void){
   putChar('E');
   putChar('R');
   putChar('R');
   putChar('O');
   putChar('R');
   putChar('\r');
   putChar('\n');
}

void put_ok(void){
   putChar('O');
   putChar('K');
   putChar('\r');
   putChar('\n');
   
}

void put_lf(void){
   putChar('\r');
   putChar('\n');
}

void changeBaudRate(UBYTE type_select,UBYTE SPBRG_number,UBYTE BRGH_number){
    if(type_select=='h'){ //115200bps
        SPBRG  = 4;                   
        BRGH   = 1;   
    } else if(type_select=='l'){ //14400bps
        SPBRG  = 10;                   
        BRGH   = 0;  
    } else if(type_select=='a'){
        SPBRG  = SPBRG_number;                   
        BRGH   = BRGH_number;            
    } else {
        //TODO:add error
    }
}

void UARTbufferClear(void){
    RCREG = 0;   //USART Receive Register
}

//process command data if the command type is UART
void commandSwitchUART(UBYTE command, UBYTE data1, UBYTE data2, UBYTE data3, UBYTE data4, UBYTE data5){
    UBYTE ReadData[];
    switch(command){    
        case 'c': //UART buffer clear
            UARTbufferClear();
            break;
        case 'b': //change UART baud rate
            changeBaudRate(data1,data2,data3);
            break;
        case 'i': //interrupt permission
            changeInterruptPermission(data1,data2);
            break;
        default:
            updateErrorStatus(error_UART_commandSwitchUART);
            break;
    }
}
