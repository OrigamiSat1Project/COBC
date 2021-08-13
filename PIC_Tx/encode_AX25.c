#include "UART.h"
#include "Type_define.h"
#include "time.h"
#include "encode_AX25.h"
#include "pinDefine.h"

#define bit_H 0x01
#define bit_L 0x00
#define ucall "JQ1YCZ"   //call sign of Tokyo Tech
#define mycall "JS1YAX"  //call sign of OrigamiSat-1
#define low  0
#define high 1

//void SendPacket(void);
void SendByte(UBYTE);
void flipout(void);
void fcsbit(UBYTE);
/*--for debug--*/
//void test_Packetmaker(UBYTE *);
UINT Packetmaker(UBYTE *,UINT);
//void test_Packetmaker(UBYTE *, UBYTE *);

UINT eflag = 0;
UINT efcsflag = 0;
UINT estuff = 0;
UBYTE efcslo, efcshi;
UBYTE ePacket[52];
BIT ebitstatus = low;

UINT Packetmaker(UBYTE *eDataField,UINT num){
    for(UINT i=0;i<6;i++){
        ePacket[i] = ucall[i] << 1;
    }
    ePacket[6] = 0x60;  //SSID
    for(UINT i=0;i<6;i++){
        ePacket[i+7] = mycall[i] << 1;
    }
    ePacket[13] = 0xe1; //SSID.e1?
    ePacket[14] = 0x03; //Control.30?
    ePacket[15] = 0xf0; //PID
    UINT Datanum = num;
    for(UINT i=0;i<Datanum;i++){
        ePacket[16+i] = eDataField[i];
    }
    return 16+Datanum;
}

void SendPacket(UBYTE *eDataField,UINT num){
    UINT Packetnum;
    Packetnum = 0;
    Packetnum = Packetmaker(eDataField,num);
    ebitstatus = 1;
    efcslo = efcshi = 0xff;
    estuff = 0;
    //  FlagField
    eflag = 1;
    efcsflag = 0;
    for(UINT i=0;i<27;i++){
        SendByte(0x7e);
    }
    eflag = 0;
    //  eDataField
    for(UINT i=0;i<Packetnum;i++){
        SendByte(ePacket[i]);
    }


    //  FCSField
    efcsflag = 1;
//    efcslo ^= 0xff;
//    efcshi ^= 0xff;
    efcslo = efcslo ^ 0xff;
    efcshi = efcshi ^ 0xff;
    SendByte(efcslo);
    SendByte(efcshi);
    efcsflag = 0;

    //  FlagField
    eflag = 1;
    for(UINT i=0;i<6;i++){
        SendByte(0x7e);
    }
}

void SendByte(UBYTE byte){
    UBYTE bt;
    putHex(byte);
    for(UINT i=0;i<8;i++){
        bt = byte & bit_H;
        //  eDataField -- FCSCalculate
        if(efcsflag == 0 && eflag == 0){
            fcsbit(bt);
        }
        //  eDataField, FCSField -- bitestuffing
        if(bt == bit_L){
            flipout();
        }else{
            estuff ++;

            if(eflag == 0 && estuff == 5){
                __delay_us(espan);
                flipout();
            }
        }
        __delay_us(espan);
        byte = byte >> 1;
    }
}
//NRZI
void flipout(void){
    estuff = 0;
    if(ebitstatus == low){
        txd = high;
        ebitstatus = high;
    }else{
        txd = low;
        ebitstatus = low;
    }
}


//  FCSCalculation
void fcsbit(UBYTE tbyte){
    //FIXME:fix assembry
    #asm
        BCF 03,0
        RRF _efcshi,F
        RRF _efcslo,F
    #endasm

    if(((STATUS & bit_H)^(tbyte)) == bit_H){
        efcshi ^= 0x84;
        efcslo ^= 0x08;
    }
}
