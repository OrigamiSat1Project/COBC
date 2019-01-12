
#include <xc.h>
#include <stdio.h>
#include <stdlib.h>
#include "FMCW.h"
#include "EEPROM.h"
#include "pinDefine.h"
#include "I2C.h"
#include "CRC16.h"
#include "encode_AX25.h"
#include "MPU.h"
#include "Type_define.h"
#include "time.h"
#include "OkError.h"
#include "WDT.h"
#include "UART.h"
#include "ADC.h"

/*******************************************************************************
*Marco, Enum / Global data / Method
******************************************************************************/
#define EEPROM_COMMAND_DATA_SIZE 32
#define MAX_DOWNLINK_DATA_SIZE 32
#define PACKET_COUNTER_SIZE 3

#define CWHIGH 1
#define CWLOW 0

UBYTE downlink_data[35];



/*******************************************************************************
*Downlink Command
******************************************************************************/
void downlinkReceivedCommand(UBYTE B0Select, UBYTE addressHigh, UBYTE addressLow, UBYTE downlinlkTimes){
    UBYTE commandData[EEPROM_COMMAND_DATA_SIZE];

    UBYTE mainAddress;
    UBYTE subAddress;
    mainAddress = (UBYTE)(EEPROM_address | B0Select);
    subAddress = (UBYTE)(EEPROM_subaddress | B0Select);
    ReadDataFromEEPROM(mainAddress,addressHigh,addressLow, commandData,EEPROM_COMMAND_DATA_SIZE);

    /*---CRC check for command from Grand Station---*/
    /*------------------------------------------------------------------*/
    if(crc16(0,commandData,29) == CRC_check(commandData,29)){
        //5bit 0--->1
        commandData[31] |= 0b00100000;
    }else{
        ReadDataFromEEPROM(subAddress,addressHigh,addressLow, commandData,EEPROM_COMMAND_DATA_SIZE);
        if(crc16(0,commandData,29) == CRC_check(commandData,29)){
            //5bit 0--->0 / 4bit 0--->1
            commandData[31] &= 0b11011111;
            commandData[31] |= 0b00010000;
        }else{
            //5,4bit 0--->0
            commandData[31] &= 0b11001111;
        }
    }

//    WriteCheckByteToEEPROMs(B0Select,addressHigh,addressLow, commandData[31]);
    __delay_us(200);

    for(UBYTE sendCounter = 0; sendCounter < downlinlkTimes; sendCounter++){
        FMPTT = 1;
        SendPacket(commandData,EEPROM_COMMAND_DATA_SIZE);
        FMPTT = 0;
        __delay_ms(300);
    }
    for(int i = 0 ; i < EEPROM_COMMAND_DATA_SIZE ; i ++){
        putChar(commandData[i]);
    }
    
    putChar('\r');
    putChar('\n');

    /*-------------------------------------------------*/
    if(commandData[0]=='T'){                //command target = PIC_TX
        //Task target
        if(commandData[2] == 't'){          //task target =  PIC_TX
        // Command type
            UBYTE commandData5_19[15] = {0};

            switch(commandData[3]){         //Process command type
                case 'm':/*get satellite mode*/
//                    downlinkFMSignal(EEPROM_address, satelliteMode_addressHigh, satelliteMode_addressLow, commandData[4], satelliteMode_DataSize);
                case 'C':/*downlink CW Signal*/
                    commandSwitchCWDownlink(commandData[4],commandData[5],commandData[6],commandData[7],commandData[8], commandData[9], commandData[10]);
                    break;
                case 'f':/*downlink FM Signal*/
                    downlinkFMSignal(commandData[4],commandData[5],commandData[6],commandData[7], commandData[8],commandData[9]);
                    break;
                case 'p': /*power supply*/
                    commandSwitchPowerSupply(commandData[4], commandData[5], commandData[6], commandData[7], commandData[8]);
                    break;
                case 'i':/*I2C*/
                    commandSwitchI2C(commandData[4], commandData[5], commandData[6], commandData[7]);
                    break;
                case 'u':/*UART*/
                    commandSwitchUART(commandData[4], commandData[5], commandData[6], commandData[7], commandData[8], commandData[9]);
                    break;
                case 'w':/*WDT (watch dog timer)*/
                    WDTwait();
                    break;
                case 'h':/*update HK data (BAT_POS V) (HK = house keeping)*/
                   commandSwitchHKdata(commandData[4]);
                    break;
                case 't':/*internal processing*/
                    commandSwitchIntProcess(commandData[4], commandData[5], commandData[6]);
                    break;
                default:
//                    updateErrorStatus(error_FMCW_downlinkReceivedCommand);
                    break;
            }
        }

    }
}

/*******************************************************************************
*Function : FM and CW downkink
******************************************************************************/
/*
 *  Do not process anything (standby)
 *  Wait 5 processing
 */
void _NOP(void) {
    for(UBYTE i=0; i<5; i++){
        NOP();
    }
}

/*******************************************************************************
*FM
******************************************************************************/
void downlinkFMSignal(UBYTE Address7bit, UBYTE addressHigh, UBYTE addressLow, UBYTE downlinkTimes, UBYTE DataLengthHigh, UBYTE DataLengthLow){
    UINT DataLength = ((UINT)DataLengthHigh << 8) + (UINT)DataLengthLow;
    UINT address = ((UINT)addressHigh << 8) + (UINT)addressLow;
    UBYTE flag = 0;
    UBYTE packet_counter = 1;
    
    CWKEY = low;
    while(!flag){
        for(UBYTE i = 0 ; i < 3 ; i++){
            downlink_data[i] = packet_counter;
        }
        for(UBYTE sendCounter = 0; sendCounter < downlinkTimes; sendCounter++){
            sendPulseWDT();
            if(DataLength <= MAX_DOWNLINK_DATA_SIZE){
                ReadDataFromEEPROM(Address7bit,addressHigh,addressLow, &downlink_data[3],DataLength);
                FMPTT = high;
                SendPacket(downlink_data,DataLength + PACKET_COUNTER_SIZE);
                FMPTT = low;
                flag = 1;
            }else{
                ReadDataFromEEPROM(Address7bit,addressHigh,addressLow, &downlink_data[3],MAX_DOWNLINK_DATA_SIZE);
                FMPTT = high;
                SendPacket(downlink_data,MAX_DOWNLINK_DATA_SIZE + PACKET_COUNTER_SIZE);
                FMPTT = low;
            }
            __delay_ms(500);
        }
        
        if(DataLength <= MAX_DOWNLINK_DATA_SIZE){
            ReadDataFromEEPROM(Address7bit,addressHigh,addressLow, &downlink_data[3],DataLength);
            for(int i = 0 ; i < DataLength + PACKET_COUNTER_SIZE ; i ++){
                putChar(downlink_data[i]);
            }
            putChar('\r');
            putChar('\n');
            flag = 1;
        }else{
            ReadDataFromEEPROM(Address7bit,addressHigh,addressLow, &downlink_data[3],MAX_DOWNLINK_DATA_SIZE);
            for(int i = 0 ; i < MAX_DOWNLINK_DATA_SIZE + PACKET_COUNTER_SIZE ; i++){
                putChar(downlink_data[i]);
            }
            putChar('\r');
            putChar('\n');
        }
        
        if(flag) break;
        packet_counter += 1;
        address += 0x0020;
        addressHigh = (UBYTE)(address >> 8);
        addressLow = (UBYTE)address;
        DataLength -= MAX_DOWNLINK_DATA_SIZE;
        DataLengthHigh = (UBYTE)(DataLength >> 8);
        DataLengthLow = (UBYTE)DataLength;
    }
    return;
}

/*******************************************************************************
*CW swtich
******************************************************************************/
//TODO:debug not yet
void commandSwitchCWDownlink(UBYTE type_select, UBYTE Address7bit, UBYTE high_address_forData, UBYTE low_address_forData, UBYTE downlink_times, UBYTE EEPROMDataLength_or_high_address_forDataSize, UBYTE low_address_forDataSize){
    UBYTE read_data_forCW[];
    switch(type_select){
        case 0xaa:  //the size of data is specified by the command
            ReadDatasFromEEPROMWithDataSizeAndSendMorseWithDownlinkTimes(Address7bit, high_address_forData, low_address_forData, read_data_forCW, EEPROMDataLength_or_high_address_forDataSize, downlink_times);
//            updateErrorStatus(ok_FMCW_commandSwitchCWDownlink_aa);
            break;
        case 0xbb:  //the size of data is written in EEPROM
            GetDatasizeAndReadDatasFromEEPROMWithDataSizeAndSendMorseWithDownlinkTimes(Address7bit, high_address_forData, low_address_forData, read_data_forCW, EEPROMDataLength_or_high_address_forDataSize, low_address_forDataSize, downlink_times);
//            updateErrorStatus(ok_FMCW_commandSwitchCWDownlink_bb);
            break;
        default:
//            updateErrorStatus(error_FMCW_commandSwitchCWDownlink);
            break;

    }
}


/*******************************************************************************
/**Function : CW downlink
 * tu = 3ton
 * [ton] CWKEY = 1; -> SHORT_DELAYTIMES_FOR_MORSE -> CWKEY = 0; -> SHORT_DELAYTIMES_FOR_MORSE
 * [tu ] CWKEY = 1; -> MIDDLE_DELAYTIMES_FOR_MORSE -> CWKEY = 0; -> SHORT_DELAYTIMES_FOR_MORSE
 * [Interval between ton-tu / ton-ton / tu-tu] SHORT_DELAYTIMES_FOR_MORSE (=1ton)
 * [Interval between moji]                     MIDDLE_DELAYTIMES_FOR_MORSE (=3ton)
 * [Interval between tango]                    LONG_DELAYTIMES_FOR_MORSE (=7ton)
******************************************************************************/

/*
 *  translate char to morse signal
 *  arg     : char
 *  return  : bin (morse)
 *            ton -> 1 / tu ->111 / delay -> 0
 *  TODO    : need debug
 */
//long changeCharMorse (char _c){
//    switch(_c){
//        case '0': return 0b1110111011101110111;
//        case '1': return 0b11101110111011101;
//        case '2': return 0b111011101110101;
//        case '3': return 0b1110111010101;
//        case '4': return 0b11101010101;
//        case '5': return 0b101010101;
//        case '6': return 0b10101010111;
//        case '7': return 0b1010101110111;
//        case '8': return 0b101011101110111;
//        case '9': return 0b10111011101110111;
//        case 'a':
//        case 'A': return 0b11101;
//        case 'b':
//        case 'B': return 0b101010111;
//        case 'c':
//        case 'C': return 0b10111010111;
//        case 'd':
//        case 'D': return 0b1010111;
//        case 'e':
//        case 'E': return 0b1;
//        case 'f':
//        case 'F': return 0b101110101;
//        case 'g':
//        case 'G': return 0b101110111;
//        case 'h':
//        case 'H': return 0b1010101;
//        case 'i':
//        case 'I': return 0b101;
//        case 'j':
//        case 'J': return 0b1110111011101;
//        case 'k':
//        case 'K': return 0b111010111;
//        case 'l':
//        case 'L': return 0b101011101;
//        case 'm':
//        case 'M': return 0b1110111;
//        case 'n':
//        case 'N': return 0b10111;
//        case 'o':
//        case 'O': return 0b11101110111;
//        case 'p':
//        case 'P': return 0b10111011101;
//        case 'q':
//        case 'Q': return 0b1110101110111;
//        case 'r':
//        case 'R': return 0b1011101;
//        case 's':
//        case 'S': return 0b10101;
//        case 't':
//        case 'T': return 0b111;
//        case 'u':
//        case 'U': return 0b1110101;
//        case 'v':
//        case 'V': return 0b111010101;
//        case 'w':
//        case 'W': return 0b111011101;
//        case 'x':
//        case 'X': return 0b11101010111;
//        case 'y':
//        case 'Y': return 0b1110111010111;
//        case 'z':
//        case 'Z': return 0b10101110111;
//        default : return 0;
//    }
//}

const static long char2hex[] = {
    0b1110111011101110111,
    0b11101110111011101,
    0b111011101110101,
    0b1110111010101,
    0b11101010101,
    0b101010101,
    0b10101010111,
    0b1010101110111,
    0b101011101110111,
    0b10111011101110111,
    0b11101,
    0b101010111,
    0b10111010111,
    0b1010111,
    0b1,
    0b101110101,
    0b101110111,
    0b1010101,
    0b101,
    0b1110111011101,
    0b111010111,
    0b101011101,
    0b1110111,
    0b10111,
    0b11101110111,
    0b10111011101,
    0b1110101110111,
    0b1011101,
    0b10101,
    0b111,
    0b1110101,
    0b111010101,
    0b111011101,
    0b11101010111,
    0b1110111010111,
    0b10101110111
};

long changeCharMorse (char value){
    if(value >= 0x41 && value <= 0x5a) value -= 55;
    else if(value >= 0x61 && value <= 0x7a) value -= 87;
    else if(value >= 0x30 && value <= 0x39) value -= 48;
    else return 0;
    return char2hex[value];
}

/*
 *  translate binary to char
 *  arg     : UBYTE
 *  return  : char
 *  TODO    : need debug
 */
//char changeBinaryToChar(UBYTE _binary){
//    switch(_binary){
//        case 0x00 : return '0';
//        case 0x01 : return '1';
//        case 0x02 : return '2';
//        case 0x03 : return '3';
//        case 0x04 : return '4';
//        case 0x05 : return '5';
//        case 0x06 : return '6';
//        case 0x07 : return '7';
//        case 0x08 : return '8';
//        case 0x09 : return '9';
//        case 0x0A : return 'A';
//        case 0x0B : return 'B';
//        case 0x0C : return 'C';
//        case 0x0D : return 'D';
//        case 0x0E : return 'E';
//        case 0x0F : return 'F';
//        default   : return 'X'; //TODO:change 'X' to error messege
//    }
//}

const static UBYTE ascii[] = {
    '0',
    '1',
    '2',
    '3',
    '4',
    '5',
    '6',
    '7',
    '8',
    '9',
    'A',
    'B',
    'C',
    'D',
    'E',
    'F'
};

char changeBinaryToChar(UBYTE _binary){
    if(_binary > 0x0f) return 'X';
    return ascii[_binary];
}

/*
 *  DevideDataAndChangeBinaryToChar
 *  1. devide data (high/low)
 *  2. translate binary to char
 *  arg     : binary
 *  return  : char
 *  TODO    : need debug
 */
void DevideDataAndChangeBinaryToChar (UBYTE binary_data, UBYTE *char_data_highLow){
    UBYTE binary_data_high;
    UBYTE binary_data_low;
    binary_data_high = binary_data >> 4;    //7654bit
    binary_data_low  = binary_data & 0x0F;  //3210bit
    char_data_highLow[0] = (changeBinaryToChar (binary_data_high));
    char_data_highLow[1] = (changeBinaryToChar (binary_data_low));
}

void sendMorse(char *HK_Data,size_t data_size){
    for (UINT i = 0;i<data_size;i++){
        long mo = changeCharMorse(HK_Data[i]);
        for (int n=0;n<19;n++){
            if(mo==0){
                break;
            } else if((mo&1)==1){
                CWKEY = CWHIGH;
                __delay_us(SHORT_DELAYTIMES_FOR_MORSE);
            } else {
                CWKEY = CWLOW;
                __delay_us(SHORT_DELAYTIMES_FOR_MORSE);
            }
            mo=mo>>1;
        }
        CWKEY = CWLOW;
        __delay_us(MIDDLE_DELAYTIMES_FOR_MORSE);
    }
}



/*
 *  ReadOneByteDataFromEEPROMandSendMorse
 *  1. read only 1byte data from EEPROM
 *  2. send a data by morse signal
 *  arg     : UBYTE
 *  return  : N/A
 *  TODO    : need debug
 */
void ReadOneByteDataFromEEPROMandSendMorse(UBYTE Address7Bytes, UBYTE high_address, UBYTE low_address){
    UBYTE read_data;
    UBYTE char_data_highLow[2];
    read_data = ReadEEPROM(Address7Bytes, high_address, low_address);
    DevideDataAndChangeBinaryToChar (read_data, char_data_highLow);
    sendMorse(char_data_highLow,sizeof(char_data_highLow)/sizeof(char_data_highLow[0]));
    sendPulseWDT();
}

/*
 *  ReadDatasFromEEPROMWithDataSizeAndSendMorse
 *  1. read one or more data from EEPROM
 *  2. send data by morse signal
 *  arg     : UBYTE
 *  return  : N/A
 *  TODO    : need debug
 */
void ReadDatasFromEEPROMWithDataSizeAndSendMorse(UBYTE Address7Bytes, UBYTE high_address, UBYTE low_address, UBYTE *ReadData, UINT EEPROMDataLength){
    UBYTE char_data_highLow[2];
    UBYTE send_data[];
    ReadDataFromEEPROM(Address7Bytes, high_address, low_address, ReadData, EEPROMDataLength);
    for(UBYTE i=0; i<EEPROMDataLength; i++){
        DevideDataAndChangeBinaryToChar (ReadData[i], char_data_highLow);
        sendMorse(char_data_highLow,sizeof(char_data_highLow)/sizeof(char_data_highLow[0]));
        sendPulseWDT();
    }
}

/*
 *  ReadDatasFromEEPROMWithDataSizeAndSendMorseWithDownlinkTimes
 *  1. read one or more data from EEPROM
 *  2. send data by morse signal
 *  3. repeat step2 DownlinkTimes
 *  arg     : UBYTE
 *  return  : N/A
 *  TODO    : need debug
 */
void ReadDatasFromEEPROMWithDataSizeAndSendMorseWithDownlinkTimes(UBYTE Address7Bytes, UBYTE high_address, UBYTE low_address, UBYTE *ReadData, UINT EEPROMDataLength, UBYTE downlink_times){
    UBYTE char_data_highLow[2];
    UBYTE send_data[];
    ReadDataFromEEPROM(Address7Bytes, high_address, low_address, ReadData, EEPROMDataLength);

    for(UBYTE downlink_counter =0; downlink_counter<downlink_times; downlink_counter++){
        for(UBYTE i=0; i<EEPROMDataLength; i++){
            DevideDataAndChangeBinaryToChar (ReadData[i], char_data_highLow);
            sendMorse(char_data_highLow,sizeof(char_data_highLow)/sizeof(char_data_highLow[0]));
            delay_us(ADD_BLANK_FOR_MORSE);
            sendPulseWDT();
        }
    }
}

void GetDatasizeAndReadDatasFromEEPROMWithDataSizeAndSendMorseWithDownlinkTimes(UBYTE Address7Bytes, UBYTE high_address, UBYTE low_address, UBYTE *ReadData, UINT high_address_forDataSize, UBYTE low_address_forDataSize, UBYTE downlink_times){
    UBYTE char_data_highLow[2];
    UBYTE send_data[];
    UBYTE EEPROMDataLength;
    EEPROMDataLength = ReadEEPROM(Address7Bytes,high_address_forDataSize,low_address_forDataSize);
    ReadDataFromEEPROM(Address7Bytes, high_address, low_address, ReadData, EEPROMDataLength);

    for(UBYTE downlink_counter =0; downlink_counter<downlink_times; downlink_counter++){
        for(UBYTE i=0; i<EEPROMDataLength; i++){
            DevideDataAndChangeBinaryToChar (ReadData[i], char_data_highLow);
            sendMorse(char_data_highLow,sizeof(char_data_highLow)/sizeof(char_data_highLow[0]));
            delay_us(ADD_BLANK_FOR_MORSE);
            sendPulseWDT();
        }
    }
}

/*******************************************************************************
/**Main : HK downlink
 * ---
 * FR0  :  satellite MYCALL('JS1YAX') + satellite name('origami')
 * FR1  :  HK Data
 * FR2  :  Free Data which can be selected by Command from GSC
 * ---
 * interval between frames is 10 seconds (normalmode)
******************************************************************************/
void HKDownlink(void){
    FMPTT=0;
    if(ReceiveFlag == CORRECT_RECEIVE) return;
    HKDownlinkFR0();
    __delay_s(1);
    if(ReceiveFlag == CORRECT_RECEIVE) return;
    HKDownlinkFR1();
    __delay_s(1);
    if(ReceiveFlag == CORRECT_RECEIVE) return;
    HKDownlinkFR2();
    __delay_s(1);
}

/*******************************************************************************
Frame
******************************************************************************/
void HKDownlinkFR0(void){
    sendPulseWDT();
    UBYTE MYCALL[6] = {'J', 'S', '1', 'Y','A','X'};
    sendMorse(MYCALL,sizeof(MYCALL)/sizeof(MYCALL[0]));
    sendPulseWDT();
    if(ReceiveFlag == CORRECT_RECEIVE) return;
    __delay_us(LONG_DELAYTIMES_FOR_MORSE);
    sendPulseWDT();
    UBYTE SatName[7] = {'o', 'r', 'i', 'g', 'a','m','i'};
    sendMorse(SatName,sizeof(SatName)/sizeof(SatName[0]));
    sendPulseWDT();
    if(ReceiveFlag == CORRECT_RECEIVE) return;
}

void HKDownlinkFR1(void){
    UBYTE DATA[2];//for ReadDatasFromEEPROMWithDataSizeAndSendMorse()
    //Sattellite Mode
    ReadOneByteDataFromEEPROMandSendMorse(EEPROM_address,satelliteMode_addressHigh,satelliteMode_addressLow);
    sendPulseWDT();
    if(ReceiveFlag == CORRECT_RECEIVE) return;
    //SatMode error status
   __delay_us(LONG_DELAYTIMES_FOR_MORSE);
   ReadOneByteDataFromEEPROMandSendMorse(EEPROM_address,SatMode_error_status1_addresshigh,SatMode_error_status1_addresslow);
   sendPulseWDT();
   if(ReceiveFlag == CORRECT_RECEIVE) return;
//    //battery Temperature
    __delay_us(LONG_DELAYTIMES_FOR_MORSE);
    ReadDatasFromEEPROMWithDataSizeAndSendMorse(EEPROM_address,adcValue_CH1_addressHigh,adcValue_CH1_addressLow,DATA,2);
    sendPulseWDT();
    if(ReceiveFlag == CORRECT_RECEIVE) return;
   //latest execution command ID(RX)
   __delay_us(LONG_DELAYTIMES_FOR_MORSE);
   ReadOneByteDataFromEEPROMandSendMorse(EEPROM_address,HighAddress_for_RXCOBCLastCommandID,LowAddress_for_RXCOBCLastCommandID);
   sendPulseWDT();
   if(ReceiveFlag == CORRECT_RECEIVE) return;
//    //latest execution command ID(TX)
   __delay_us(LONG_DELAYTIMES_FOR_MORSE);
   ReadOneByteDataFromEEPROMandSendMorse(EEPROM_address,HighAddress_for_TXCOBCLastCommandID,LowAddress_for_TXCOBCLastCommandID);
   sendPulseWDT();
   if(ReceiveFlag == CORRECT_RECEIVE) return;
//    //battery Voltage (CIB)
    __delay_us(LONG_DELAYTIMES_FOR_MORSE);
    ReadDatasFromEEPROMWithDataSizeAndSendMorse(EEPROM_address,BatteryVoltageCIB_addressHigh,BatteryVoltageCIB_addressLow,DATA,2);
    sendPulseWDT();
    if(ReceiveFlag == CORRECT_RECEIVE) return;
//    //5VBus Voltage
    __delay_us(LONG_DELAYTIMES_FOR_MORSE);
    ReadDatasFromEEPROMWithDataSizeAndSendMorse(EEPROM_address,adcValue_CH2_addressHigh,adcValue_CH2_addressLow,DATA,2);
    sendPulseWDT();
    if(ReceiveFlag == CORRECT_RECEIVE) return;

   //3V3Bus Voltage
   __delay_us(LONG_DELAYTIMES_FOR_MORSE);
   ReadDatasFromEEPROMWithDataSizeAndSendMorse(EEPROM_address,adcValue_CH3_addressHigh,adcValue_CH3_addressLow,DATA,2);
   sendPulseWDT();
   if(ReceiveFlag == CORRECT_RECEIVE) return;
//    //battery Voltage (OBC)
   __delay_us(LONG_DELAYTIMES_FOR_MORSE);
   ReadOneByteDataFromEEPROMandSendMorse(EEPROM_address,BatteryVoltageOBC_addressHigh,BatteryVoltageOBC_addressLow);
   sendPulseWDT();
   if(ReceiveFlag == CORRECT_RECEIVE) return;
//    //latest execution command ID (OBC)
   __delay_us(LONG_DELAYTIMES_FOR_MORSE);
   ReadOneByteDataFromEEPROMandSendMorse(EEPROM_address,LatestExcutionCommandID_addressHigh,LatestExcutionCommandID_addressLow);
   sendPulseWDT();
   if(ReceiveFlag == CORRECT_RECEIVE) return;
//    //command error status(OBC)
   __delay_us(LONG_DELAYTIMES_FOR_MORSE);
   ReadOneByteDataFromEEPROMandSendMorse(EEPROM_address,OBC_CommandErrorStatus_addressHigh,OBC_CommandErrorStatus_addressLow);
   sendPulseWDT();
   if(ReceiveFlag == CORRECT_RECEIVE) return;
//    //Battery Current
   __delay_us(LONG_DELAYTIMES_FOR_MORSE);
   ReadDatasFromEEPROMWithDataSizeAndSendMorse(EEPROM_address,BatteryCurrent_addressHigh,BatteryCurrent_addressLow,DATA,2);
   sendPulseWDT();
   if(ReceiveFlag == CORRECT_RECEIVE) return;
//    //EPS switch status
   __delay_us(LONG_DELAYTIMES_FOR_MORSE);
   ReadDatasFromEEPROMWithDataSizeAndSendMorse(EEPROM_address,EpsSwitchStatus_addressHigh,EpsSwitchStatus_addressLow,DATA,2);
   sendPulseWDT();
   if(ReceiveFlag == CORRECT_RECEIVE) return;
//    //TX temperature
   __delay_us(LONG_DELAYTIMES_FOR_MORSE);
   ReadOneByteDataFromEEPROMandSendMorse(EEPROM_address,TxTemperature_addressHigh,TxTemperature_addressLow);
   sendPulseWDT();
   if(ReceiveFlag == CORRECT_RECEIVE) return;
//    //RX temperature
   __delay_us(LONG_DELAYTIMES_FOR_MORSE);
   ReadOneByteDataFromEEPROMandSendMorse(EEPROM_address,RxTemperature_addressHigh,RxTemperature_addressLow);
   sendPulseWDT();
   if(ReceiveFlag == CORRECT_RECEIVE) return;
}

void HKDownlinkFR2(void){
    sendPulseWDT();
    UBYTE ReadData1_slaveaddress = ReadEEPROM(EEPROM_address,FreeData1_slaveaddress_addressHigh,FreeData1_slaveaddress_addressLow);
    UBYTE ReadData1_addressHigh = ReadEEPROM(EEPROM_address, FreeData1Highaddress_addressHigh, FreeData1Highaddress_addressLow);
    UBYTE ReadData1_addressLow = ReadEEPROM(EEPROM_address, FreeData1Lowaddress_addressHigh, FreeData1Lowaddress_addressLow);
    ReadOneByteDataFromEEPROMandSendMorse(ReadData1_slaveaddress,ReadData1_addressHigh,ReadData1_addressLow);
    sendPulseWDT();
    if(ReceiveFlag == CORRECT_RECEIVE) return;
    __delay_us(LONG_DELAYTIMES_FOR_MORSE);
    UBYTE ReadData2_slaveaddress = ReadEEPROM(EEPROM_address,FreeData2_slaveaddress_addressHigh,FreeData2_slaveaddress_addressLow);
    UBYTE ReadData2_addressHigh = ReadEEPROM(EEPROM_address, FreeData2Highaddress_addressHigh, FreeData2Highaddress_addressLow);
    UBYTE ReadData2_addressLow = ReadEEPROM(EEPROM_address, FreeData2Lowaddress_addressHigh, FreeData2Lowaddress_addressLow);
    ReadOneByteDataFromEEPROMandSendMorse(ReadData2_slaveaddress,ReadData2_addressHigh,ReadData2_addressLow);
    sendPulseWDT();
    if(ReceiveFlag == CORRECT_RECEIVE) return;
}


void Init_HK(void){
    sendPulseWDT();
    
    //FR2
    WriteOneByteToEEPROM(EEPROM_address,FreeData1_slaveaddress_addressHigh,FreeData1_slaveaddress_addressLow,EEPROM_address);
    WriteOneByteToEEPROM(EEPROM_address,FreeData1Highaddress_addressHigh,FreeData1Highaddress_addressLow,MeltingStatus_addressHigh);
    WriteOneByteToEEPROM(EEPROM_address,FreeData1Lowaddress_addressHigh,FreeData1Lowaddress_addressLow,MeltingStatus_addressLow);
    
    WriteOneByteToEEPROM(EEPROM_address,FreeData2_slaveaddress_addressHigh,FreeData2_slaveaddress_addressLow,EEPROM_address);
    WriteOneByteToEEPROM(EEPROM_address,FreeData2Highaddress_addressHigh,FreeData2Highaddress_addressLow,NTRX_subpower_status_addressHigh);
    WriteOneByteToEEPROM(EEPROM_address,FreeData2Lowaddress_addressHigh,FreeData2Lowaddress_addressLow,NTRX_subpower_status_addressLow);
}
