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
UINT Packetmaker(UBYTE *,UINT);
//void test_Packetmaker(UBYTE *, UBYTE *);

UBYTE eflag = 0;
UBYTE efcsflag = 0;
UBYTE estuff = 0;
UBYTE efcslo, efcshi;
UBYTE ePacket[52];
//UBYTE ePacket_50[52];
BIT ebitstatus = low;

/*--for debug--*/
// void test_Packetmaker(UBYTE *eDataField){
//     UBYTE num_ = Packetmaker(eDataField);
//     for(UBYTE i=0;i<num_;i++){
//         putch(ePacket[i]);
//     }
//     putcrlf();
// }

//void test_Packetmaker(UBYTE *eDataField){
//    UINT num_ = Packetmaker(eDataField);
//    for(UINT i=0;i<num_;i++){
//        putch(ePacket[i]);
//    }
//    putcrlf();
//}

UINT Packetmaker(UBYTE *eDataField,UINT num){
    for(UINT i=0;i<6;i++){
        ePacket[i] = ucall[i] << 1;
    }
    ePacket[6] = 0x60;  //SSID
    for(UBYTE i=0;i<6;i++){
        ePacket[i+7] = mycall[i] << 1;
    }
    ePacket[13] = 0xe1; //SSID.e1?
    ePacket[14] = 0x03; //Control.30?
    ePacket[15] = 0xf0; //PID
    UINT Datanum = num;
//    for(Datanum=0;eDataField[Datanum] != '\0';Datanum++);
//    Datanum -= 1;
//    while(*eDataField){
//        Datanum ++;
//        ++eDataField;
//    }
    for(UINT i=0;i<Datanum;i++){
        ePacket[16+i] = eDataField[i];
    }
    
    //  XXX : for debug
//    for(UBYTE i=0;i<16+Datanum;i++){
//        putch(ePacket[i]);
//    }
    return 16+Datanum;
}

void SendPacket(UBYTE *eDataField,UINT num){
//void SendPacket(void)
    UBYTE Packetnum;
    Packetnum = 0;
    Packetnum = Packetmaker(eDataField,num);
    ebitstatus = 1;
    efcslo = efcshi = 0xff;
    estuff = 0;
    //  FlagField
    eflag = 1;
    efcsflag = 0;
    for(UBYTE i=0;i<27;i++){
        SendByte(0x7e);
    }
    eflag = 0;
    //  eDataField
    for(UBYTE i=0;i<Packetnum;i++){
        SendByte(ePacket[i]);
    }
    
    
    //  FCSField
    efcsflag = 1;
    efcslo ^= 0xff;
    efcshi ^= 0xff;
    SendByte(efcslo);
    SendByte(efcshi);
    efcsflag = 0;
    
    //  FlagField
    eflag = 1;
    for(UBYTE i=0;i<6;i++){
        SendByte(0x7e);
    }
}

//void SendPacketWithDataSize(UBYTE *eDataField, UBYTE DataSize){
//    UBYTE Packetnum;
//    Packetnum = 0;
//    Packetnum = PacketmakerWithDataSize(eDataField, DataSize);
//    
//    //XXX : for debug
////    putChar('\r');
////    putChar('\n');
////    putChar(Packetnum);
////    putChar('\r');
////    putChar('\n');
//    
//    ebitstatus = 1;
//    efcslo = efcshi = 0xff;
//    estuff = 0;
//    //  FlagField
//    eflag = 1;
//    efcsflag = 0;
//    for(UBYTE i=0;i<27;i++){
//        SendByte(0x7e);
//    }
//    eflag = 0;
//    //  eDataField
//    for(UBYTE i=0;i<Packetnum;i++){
//        SendByte(ePacket_50[i]);
//    }    
//    
//    //  FCSField
//    efcsflag = 1;
//    efcslo ^= 0xff;
//    efcshi ^= 0xff;
//    SendByte(efcslo);
//    SendByte(efcshi);
//    efcsflag = 0;
//    
//    //  FlagField
//    eflag = 1;
//    for(UBYTE i=0;i<6;i++){
//        SendByte(0x7e);
//    }
//}

void SendByte(UBYTE byte){
    UBYTE bt;
    for(UBYTE i=0;i<8;i++){
        bt = (UBYTE)(byte & bit_H);
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
        byte = (UBYTE)(byte >> 1);
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
//    STATUS &= ~0x01;
//    efcshi = efcshi >> 1;
//    efcslo = efcslo >> 1;
    
    if(((STATUS & bit_H)^(tbyte)) == bit_H){
        efcshi ^= 0x84;
        efcslo ^= 0x08;
    }
}