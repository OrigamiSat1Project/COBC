#include <xc.h>
#include "typeDefine.h"
#include "pinDefine.h"
#include "time.h"
#include "EEPROM.h"
#include "I2C.h"
#include "MPU.h"
#include "UART.h"
#include "FMCW.h"
#include "ADC.h"
#include "SatMode.h"
#include "OkError.h"

UBYTE ReserveBeforeSatMode = SATMODE_SAVING;//spare BeforeSatMode (when can't read BeforeSatMode from EEPROM)

UWORD MeasureBatVoltageAndChangeSatMode(){
          //------battery voltage measure-------------
//        debug : error handling is not determined
//            for(UBYTE i=0; i<4; i++)putChar(0x11);
//            put_lf(); 
            UBYTE bat_voltage[2];
            UWORD Voltage;//Voltage is 10 bit           
            UBYTE error_status = 0;            
            
            //if Voltage is 0x0000 or very large,read one more time. Then it is still 0x0000 or very large,CHange SafeMode.
            ReadBatVoltageWithPointer(bat_voltage);
            WriteToMainAndSubB0EEPROM(BatteryVoltage_addressHigh,BatteryVoltage_addressLow,bat_voltage);
            Voltage = (UWORD)bat_voltage[0] << 8 | (UWORD)bat_voltage[1];
            if(Voltage == 0x0000 || (bat_voltage[0] & 0xFC) != 0){
//                putChar(0xA0);
//                putChar(bat_voltage[0]);
//                putChar(bat_voltage[1]);
//                put_lf();
                ReadBatVoltageWithPointer(bat_voltage);
                WriteToMainAndSubB0EEPROM(BatteryVoltage_addressHigh,BatteryVoltage_addressLow,bat_voltage);
                Voltage = (UWORD)bat_voltage[0] << 8 | (UWORD)bat_voltage[1];
                if(Voltage == 0x0000 || (bat_voltage[0] & 0xFC) != 0){ //ADC read error
//                    putChar(0xB0);
//                    putChar(bat_voltage[0]);
//                    putChar(bat_voltage[1]);
//                    put_lf();
                    UBYTE SWchangeSavingMode = ReadEEPROM(MAIN_EEPROM_ADDRESS,SW_Change_SavingMode_ADC_addresshigh,SW_Change_SavingMode_ADC_addresslow);  
                    UBYTE bitcount = BitCount(SWchangeSavingMode);  
                    error_status = error_status | 0x0003;//0b 00000000 00000011
                    if(bitcount >= 2 && bitcount <= 4){
//                        putChar(0xC0);
//                        put_lf();
                        SwitchToSavingMode();                    
                        return error_status;
                    }else if(bitcount >= 5 && bitcount <= 7){
//                        putChar(0xD0);
//                        put_lf();
                        return error_status;
                    }else{
                        SWchangeSavingMode = ReadEEPROM(SUB_EEPROM_ADDRESS,SW_Change_SavingMode_ADC_addresshigh,SW_Change_SavingMode_ADC_addresslow);
                        bitcount = BitCount(SWchangeSavingMode);                    
                        if(bitcount >= 2 && bitcount <= 4){
//                            putChar(0xE0);
//                            put_lf();
                            SwitchToSavingMode();
                            return error_status;
                        }else if(bitcount >= 5 && bitcount <= 7){
//                            putChar(0xF0);
//                            put_lf();
                            return error_status;
                        }else{
//                            putChar(0xA1);
//                            put_lf();
                            error_status = 0xAAAA; //0b 10101010 10101010;
                            return error_status;
                        }
                    }
                }
            }
            
//            for(UBYTE i=0; i<4; i++)putChar(0x22);
//            put_lf();
//            putChar(bat_voltage[0]);
//            putChar(bat_voltage[1]);
//            put_lf();
            
            
//            putChar(0xC0);
//            putChar(0xC0);
//            putChar(0xC0);
//            putChar(0xC0);
//            putChar(0xC0);
//            putChar(0xC0);
//            putChar(0xC0);
//            putChar(0xC0);
//            putChar(0xC0);
//            put_lf();
            //if BatVol_nominal_saving_high is very large,read one more time. Then it is still very large,thereshold BatVol is Initial Value.
            UWORD BatVol_nominal_saving_high = (UWORD)ReadEEPROM(MAIN_EEPROM_ADDRESS, BatVol_nominal_saving_datahigh_addressHigh, BatVol_nominal_saving_datahigh_addressLow);
            UWORD BatVol_nominal_saving_low = (UWORD)ReadEEPROM(MAIN_EEPROM_ADDRESS, BatVol_nominal_saving_datalow_addressHigh, BatVol_nominal_saving_datalow_addressLow);    
            UWORD BatVol_nominal_saving = (BatVol_nominal_saving_high << 8) | BatVol_nominal_saving_low;
//            putChar((UBYTE)BatVol_nominal_saving_high);
//            putChar((UBYTE)BatVol_nominal_saving_low);
            if(((BatVol_nominal_saving_high & 0b11111100) != 0) || (BatVol_nominal_saving <= TheresholdBatVol_lower_limit)){
//                putChar(0xC1);
//                putChar(0xC1);
//                putChar(0xC1);
//                putChar(0xC1);
//                putChar(0xC1);
//                putChar(0xC1);
//                putChar(0xC1);
//                putChar(0xC1);
//                putChar(0xC1);
//                putChar(0xC1);
//                putChar(0xC1);
//                putChar(0xC1);
//                putChar(0xC1);
//                putChar(0xC1);
//                putChar(0xC1);
//                put_lf();
                BatVol_nominal_saving_high = (UWORD)ReadEEPROM(SUB_EEPROM_ADDRESS, BatVol_nominal_saving_datahigh_addressHigh, BatVol_nominal_saving_datahigh_addressLow);
                BatVol_nominal_saving_low = (UWORD)ReadEEPROM(SUB_EEPROM_ADDRESS, BatVol_nominal_saving_datalow_addressHigh, BatVol_nominal_saving_datalow_addressLow);
                BatVol_nominal_saving = (BatVol_nominal_saving_high << 8) | BatVol_nominal_saving_low;
//                putChar((UBYTE)BatVol_nominal_saving_high);
//                putChar((UBYTE)BatVol_nominal_saving_low);
                if(((BatVol_nominal_saving_high & 0b11111100) != 0) || (BatVol_nominal_saving <= TheresholdBatVol_lower_limit)){
//                    putChar(0xC2);
//                    putChar(0xC2);
//                    putChar(0xC2);
//                    putChar(0xC2);
//                    putChar(0xC2);
//                    putChar(0xC2);
//                    putChar(0xC2);
//                    putChar(0xC2);
//                    putChar(0xC2);
//                    putChar(0xC2);
//                    putChar(0xC2);
//                    put_lf();
                    BatVol_nominal_saving_high = Init_TheresholdBatVol_nominal_saving_high;
                    BatVol_nominal_saving_low = Init_TheresholdBatVol_nominal_saving_low;
                    BatVol_nominal_saving = (BatVol_nominal_saving_high << 8) | BatVol_nominal_saving_low;
//                    putChar((UBYTE)BatVol_nominal_saving_high);
//                    putChar((UBYTE)BatVol_nominal_saving_low);
                    error_status = error_status | 0x000C; //0b 00000000 00001100
                }              
            }    
            __delay_ms(100);///DON'T Delete
//            putChar((UBYTE)BatVol_nominal_saving_high);
//            putChar((UBYTE)BatVol_nominal_saving_low);
//            put_lf();
            
//            putChar(0xD0);
            UWORD BatVol_saving_survival_high = (UWORD)ReadEEPROM(MAIN_EEPROM_ADDRESS, BatVol_saving_survival_datahigh_addressHigh, BatVol_saving_survival_datahigh_addressLow);
            UWORD BatVol_saving_survival_low = (UWORD)ReadEEPROM(MAIN_EEPROM_ADDRESS, BatVol_saving_survival_datalow_addressHigh, BatVol_saving_survival_datalow_addressLow);
            UWORD BatVol_saving_survival = (BatVol_saving_survival_high << 8) | BatVol_saving_survival_low;
            if(((BatVol_saving_survival_high & 0b11111100) != 0) || (BatVol_saving_survival <= TheresholdBatVol_lower_limit)){
//                putChar(0xD1);
                BatVol_saving_survival_high = (UWORD)ReadEEPROM(SUB_EEPROM_ADDRESS, BatVol_saving_survival_datahigh_addressHigh, BatVol_saving_survival_datahigh_addressLow);
                BatVol_saving_survival_low = (UWORD)ReadEEPROM(SUB_EEPROM_ADDRESS, BatVol_saving_survival_datalow_addressHigh, BatVol_saving_survival_datalow_addressLow);
                BatVol_saving_survival = (BatVol_saving_survival_high << 8) | BatVol_saving_survival_low;
                if(((BatVol_saving_survival_high & 0b11111100) != 0) || (BatVol_saving_survival <= TheresholdBatVol_lower_limit)){
//                    putChar(0xD2);
                    BatVol_saving_survival_high = Init_TheresholdBatVol_saving_survival_high;
                    BatVol_saving_survival_low = Init_TheresholdBatVol_saving_survival_low;
                    BatVol_saving_survival = (BatVol_saving_survival_high << 8) | BatVol_saving_survival_low;
                    error_status = error_status | 0x0030; //0b 00000000 00110000;
                }              
            }
            __delay_ms(100);///DON'T Delete
//            putChar((UBYTE)BatVol_saving_survival_high);
//            putChar((UBYTE)BatVol_saving_survival_low);
//            put_lf();
            
//            putChar(0xE0);
//            putChar(0xE0);
//            putChar(0xE0);
//            putChar(0xE0);
//            putChar(0xE0);
//            putChar(0xE0);
//            putChar(0xE0);
//            putChar(0xE0);
//            putChar(0xE0);
//            putChar(0xE0);
//            put_lf();
            UWORD BatVol_nominal_revival_high = (UWORD)ReadEEPROM(MAIN_EEPROM_ADDRESS, BatVol_nominal_revival_datahigh_addressHigh, BatVol_nominal_revival_datahigh_addressLow);
            UWORD BatVol_nominal_revival_low = (UWORD)ReadEEPROM(MAIN_EEPROM_ADDRESS, BatVol_nominal_revival_datalow_addressHigh, BatVol_nominal_revival_datalow_addressLow);
            UWORD BatVol_nominal_revival = (BatVol_nominal_revival_high << 8) | BatVol_nominal_revival_low;
//            putChar((UBYTE)BatVol_nominal_revival_high);
//            putChar((UBYTE)BatVol_nominal_revival_low);
            if(((BatVol_nominal_revival_high & 0b11111100) != 0) || (BatVol_nominal_revival <= TheresholdBatVol_lower_limit)){
//                putChar(0xE1);
                BatVol_nominal_revival_high = (UWORD)ReadEEPROM(SUB_EEPROM_ADDRESS, BatVol_nominal_revival_datahigh_addressHigh, BatVol_nominal_revival_datahigh_addressLow);
                BatVol_nominal_revival_low = (UWORD)ReadEEPROM(SUB_EEPROM_ADDRESS, BatVol_nominal_revival_datalow_addressHigh, BatVol_nominal_revival_datalow_addressLow);
                BatVol_nominal_revival = (BatVol_nominal_revival_high << 8) | BatVol_nominal_revival_low;
//                putChar((UBYTE)BatVol_nominal_revival_high);
//            putChar((UBYTE)BatVol_nominal_revival_low);
                if(((BatVol_nominal_revival_high & 0b11111100) != 0) || (BatVol_nominal_revival <= TheresholdBatVol_lower_limit)){
//                    putChar(0xE2);
                    BatVol_nominal_revival_high = Init_TheresholdBatVol_nominal_revival_high;
                    BatVol_nominal_revival_low = Init_TheresholdBatVol_nominal_revival_low;
                    BatVol_nominal_revival = (BatVol_nominal_revival_high << 8) | BatVol_nominal_revival_low;
//                    putChar((UBYTE)BatVol_nominal_revival_high);
//            putChar((UBYTE)BatVol_nominal_revival_low);
                    error_status = error_status | 0x00C0; // 0b 00000000 11000000;
                }              
            }
            __delay_ms(100);///DON'T Delete
//            putChar((UBYTE)BatVol_nominal_revival_high);
//            putChar((UBYTE)BatVol_nominal_revival_low);
//            put_lf();
            
//            putChar(0xF0);
            UWORD BatVol_saving_revival_high = (UWORD)ReadEEPROM(MAIN_EEPROM_ADDRESS, BatVol_saving_revival_datahigh_addressHigh, BatVol_saving_revival_datahigh_addressLow);
            UWORD BatVol_saving_revival_low = (UWORD)ReadEEPROM(MAIN_EEPROM_ADDRESS, BatVol_saving_revival_datalow_addressHigh, BatVol_saving_revival_datalow_addressLow);
            UWORD BatVol_saving_revival = (BatVol_saving_revival_high << 8) | BatVol_saving_revival_low;
            if(((BatVol_saving_revival_high & 0b11111100) != 0) || (BatVol_saving_revival <= TheresholdBatVol_lower_limit)){
//                putChar(0xF1);
                BatVol_saving_revival_high = (UWORD)ReadEEPROM(SUB_EEPROM_ADDRESS, BatVol_saving_revival_datahigh_addressHigh, BatVol_saving_revival_datahigh_addressLow);
                BatVol_saving_revival_low = (UWORD)ReadEEPROM(SUB_EEPROM_ADDRESS, BatVol_saving_revival_datalow_addressHigh, BatVol_saving_revival_datalow_addressLow);
                BatVol_saving_revival = (BatVol_saving_revival_high << 8) | BatVol_saving_revival_low;
                if(((BatVol_saving_revival_high & 0b11111100) != 0) || (BatVol_saving_revival <= TheresholdBatVol_lower_limit)){
//                    putChar(0xF2);
                    BatVol_saving_revival_high = Init_TheresholdBatVol_saving_revival_high;
                    BatVol_saving_revival_low = Init_TheresholdBatVol_saving_revival_low;
                    BatVol_saving_revival = (BatVol_saving_revival_high << 8) | BatVol_saving_revival_low;
                    error_status = error_status | 0x0300; //0b 00000011 00000000;
                }              
            }
            __delay_ms(100);///DON'T Delete
//            putChar((UBYTE)BatVol_saving_revival_high);
//            putChar((UBYTE)BatVol_saving_revival_low);
//            put_lf();
            
            UBYTE BeforeSatMode = ReadEEPROM(MAIN_EEPROM_ADDRESS,SatelliteMode_addressHigh,SatelliteMode_addressLow);
            UBYTE ChoicedSatMode = BeforeSatMode;//For change BeforeSatMode/ReserveBeforeSatMode when EEPROM broken;
            BeforeSatMode = BeforeSatMode & 0xF0;
            ReserveBeforeSatMode = ReserveBeforeSatMode & 0xF0;           

            switch(BeforeSatMode){
                case SATMODE_NOMINAL:
                case SATMODE_SAVING:
                case SATMODE_SURVIVAL: 
                    ChoicedSatMode =  BeforeSatMode;
                    break;
                default:
                    BeforeSatMode = ReadEEPROM(SUB_EEPROM_ADDRESS,SatelliteMode_addressHigh,SatelliteMode_addressLow);
                    BeforeSatMode = BeforeSatMode & 0xF0;
                    switch(BeforeSatMode){
                        case SATMODE_NOMINAL:
                        case SATMODE_SAVING:
                        case SATMODE_SURVIVAL: 
                            ChoicedSatMode =  BeforeSatMode;
                            break;
                        default:
                            ChoicedSatMode = ReserveBeforeSatMode;
                            error_status = error_status | 0x0C00; //0b 00001100 00000000;
                            break;                       
                    }
                    break;
            }
//            for(UBYTE i=0; i<4; i++)putChar(0x33);
//            put_lf();
            switch(ChoicedSatMode){
                case SATMODE_NOMINAL:
                    putChar(0xAA);
                    if(Voltage >= BatVol_nominal_saving) {
                        putChar(0x11);
                        //write SatMode nominal(SEP -> ON, RBF -> ON)                        
                        switch(OBC_STATUS){
                            case OBC_ALIVE:                               
                                break;
                            case OBC_DIED:
//                                killEPS();
//                                onEPS();                                                                          
//                                setPLL();
                                break;
                            default:    
                                break;
                        }
                        WriteOneByteToMainAndSubB0EEPROM(SatelliteMode_addressHigh, SatelliteMode_addressLow, SATMODE_NOMINAL_SEPON_RBFON);
                        ReserveBeforeSatMode = SATMODE_NOMINAL_SEPON_RBFON;
                    }else if(Voltage <= BatVol_saving_survival){
                        putChar(0x22);
                        //write SatMode survival(SEP -> OFF, RBF -> ON)                      
                        killEPS();
                        WriteOneByteToMainAndSubB0EEPROM(SatelliteMode_addressHigh, SatelliteMode_addressLow, SATMODE_SURVIVAL_SEPOFF_RBFON);
                        ReserveBeforeSatMode = SATMODE_SURVIVAL_SEPOFF_RBFON;
                    }else{
                        putChar(0x33);
                        //Write SatMode saving(SEP -> OFF, RBF -> ON)                        
                        killEPS();                   
                        onNtrxPowerSupplyCIB(0,0);
                        WriteOneByteToMainAndSubB0EEPROM(SatelliteMode_addressHigh, SatelliteMode_addressLow, SATMODE_SAVING_SEPOFF_RBFON);
                        ReserveBeforeSatMode = SATMODE_SAVING_SEPOFF_RBFON;
                    }
                    break;
                case SATMODE_SAVING:
                    putChar(0xBB);
                    if(Voltage >= BatVol_nominal_revival){
                        putChar(0x11);
                        //write SatMode nominal(SEP -> ON, RBF -> ON)                                               
                        offNtrxPowerSupplyCIB();
                        onEPS();
                        setPLL();
                        WriteOneByteToMainAndSubB0EEPROM(SatelliteMode_addressHigh, SatelliteMode_addressLow, SATMODE_NOMINAL_SEPON_RBFON);
                        ReserveBeforeSatMode = SATMODE_NOMINAL_SEPON_RBFON;
                    }else if (Voltage <= BatVol_saving_survival){
                        putChar(0x22);
                        //write SatMode survival(SEP -> OFF, RBF -> ON)                       
                        offNtrxPowerSupplyCIB();
                        WriteOneByteToMainAndSubB0EEPROM(SatelliteMode_addressHigh, SatelliteMode_addressLow, SATMODE_SURVIVAL_SEPOFF_RBFON);
                        ReserveBeforeSatMode = SATMODE_SURVIVAL_SEPOFF_RBFON;
                    }else{
                        putChar(0x33);
                        //Write SatMode saving(SEP -> OFF, RBF -> ON)
                        WriteOneByteToMainAndSubB0EEPROM(SatelliteMode_addressHigh, SatelliteMode_addressLow, SATMODE_SAVING_SEPOFF_RBFON);
                        ReserveBeforeSatMode = SATMODE_SAVING_SEPOFF_RBFON;
                    }
                    break;
                case SATMODE_SURVIVAL:
                    putChar(0xCC);
                    if(Voltage >= BatVol_nominal_revival){
                        putChar(0x11);
                        //write SatMode nominal(SEP -> ON, RBF -> ON)                       
                        onEPS();
                        setPLL();
                        WriteOneByteToMainAndSubB0EEPROM(SatelliteMode_addressHigh, SatelliteMode_addressLow, SATMODE_NOMINAL_SEPON_RBFON);
                        ReserveBeforeSatMode = SATMODE_NOMINAL_SEPON_RBFON;
                    }else if (Voltage <= BatVol_saving_revival){
                        putChar(0x22);
                        //write SatMode survival(SEP -> OFF, RBF -> ON)
                        WriteOneByteToMainAndSubB0EEPROM(SatelliteMode_addressHigh, SatelliteMode_addressLow, SATMODE_SURVIVAL_SEPOFF_RBFON);
                        ReserveBeforeSatMode = SATMODE_SURVIVAL_SEPOFF_RBFON;
                    }else{
                        putChar(0x33);
                        //Write SatMode saving(SEP -> OFF, RBF -> ON)                      
                        onNtrxPowerSupplyCIB(0,0);
                        WriteOneByteToMainAndSubB0EEPROM(SatelliteMode_addressHigh, SatelliteMode_addressLow, SATMODE_SAVING_SEPOFF_RBFON);
                        ReserveBeforeSatMode = SATMODE_SAVING_SEPOFF_RBFON;
                    }                  
                    break;
                default:
                    putChar(0xDD);       
                    error_status = error_status | 0x3000;// 0b 00110000 00000000;
                    break;
        }
//            put_lf();
//            for(UBYTE i=0; i<4; i++) putChar(0x44);
//            putChar(ChoicedSatMode);
//            putChar((UBYTE)(error_status>>8));
//            putChar((UBYTE)error_status);
//            put_lf();
            
//            putChar((UBYTE)BatVol_nominal_saving_high);
//            putChar((UBYTE)BatVol_nominal_saving_low);
//            put_lf();
//            putChar((UBYTE)BatVol_saving_survival_high);
//            putChar((UBYTE)BatVol_saving_survival_low);
//            put_lf();
//            putChar((UBYTE)BatVol_nominal_revival_high);
//            putChar((UBYTE)BatVol_nominal_revival_low);
//            put_lf();
//            putChar((UBYTE)BatVol_saving_revival_high);
//            putChar((UBYTE)BatVol_saving_revival_low);
//            put_lf();
            
            return error_status;
}

UBYTE BitCount(UBYTE Checker){
    UBYTE bitcounter = 0;
    for(UBYTE i=0 ; i < 8 ; i++){
        if(Checker & 0x01 == 1){
            bitcounter += 1;
        }
        Checker = Checker >> 1;
    }
    return bitcounter;
}

void SwitchToSavingMode(void){
    killEPS();
    onNtrxPowerSupplyCIB(0,0);
    WriteOneByteToMainAndSubB0EEPROM(SatelliteMode_addressHigh, SatelliteMode_addressLow, SATMODE_SAVING_SEPOFF_RBFON);
    ReserveBeforeSatMode = SATMODE_SAVING_SEPOFF_RBFON;
}

/*******************************************************************************
*command swtich feature 
******************************************************************************/
/*
 *	change satellite mode
 *	arg      :   command, timeHigh, timeLow
 *	return   :   0x07 -> Nominal mode (ON: CIB, EPS, OBC, Tx(CW), Rx)
 *               0x3C -> Power saving mode (ON: CIB, Tx(CW), Rx / OFF: EPS, OBC)
 *               0xF0 -> Survival mode (ON: CIB / OFF: EPS, OBC, Tx(CW), Rx)             
 */
void commandSwitchSatMode(UBYTE command, UBYTE timeHigh, UBYTE timeLow){ //times are given in ms
    switch(command){    
        case 0x07: 
            /*---------------------------*/
            /* method for nominal mode (ON: CIB, EPS, OBC, Tx(CW), Rx)
             * 1.SEP_SW & RBF_SW = LOW -> EPS switch ON
             * 2.then Rx/Tx/OBC switch  ON
            /*---------------------------*/
            offNtrxPowerSupplyCIB();
            switchPowerEPS(0x01, timeHigh, timeLow);
            setPLL();
            WriteOneByteToMainAndSubB0EEPROM(SatelliteMode_addressHigh, SatelliteMode_addressLow, SATMODE_NOMINAL_SEPON_RBFON);
            ReserveBeforeSatMode = SATMODE_NOMINAL_SEPON_RBFON;
            break;
        case 0x3C: 
            /*----------------------------*/
            /*method for power saving mode (ON: CIB, Tx(CW), Rx / OFF: EPS, OBC)
             * 1.first kill EPS (this also kills Rx/Tx/OBC)
             * 2.send command to TXCOBC to turn back on RX and TX  (Radio Unit)
             *      task target:t(TX COBC)
             *      CommandType:p(power)
             *      Parameter1:0x01(on) / 2:timeHigh / 3:timeLow
             * 3.RXCOBC reset PLL data
             * 4.after setting time, revive EPS (this also revive OBC)
            /*----------------------------*/
            killEPS();
            onNtrxPowerSupplyCIB(timeHigh,timeLow);
            WriteOneByteToMainAndSubB0EEPROM(SatelliteMode_addressHigh, SatelliteMode_addressLow, SATMODE_SAVING_SEPOFF_RBFON);
            ReserveBeforeSatMode = SATMODE_SAVING_SEPOFF_RBFON;
            break;
        case 0xF0: 
            /*---------------------------*/
            /* method for survival mode (ON: CIB / OFF: EPS, OBC, Tx(CW), Rx)
             * only enter if time in survival mode is specified 
             * set automatical revival time
             * 1.SEP_SW & RBF_SW = HIGH -> EPS switch OFF
             * 2.then Rx/Tx/OBC switch  OFF
             * 3.if time has run out switch to power saving mode
            /*---------------------------*/
            switchPowerEPS(0x00, timeHigh, timeLow);
            WriteOneByteToMainAndSubB0EEPROM(SatelliteMode_addressHigh, SatelliteMode_addressLow, SATMODE_SURVIVAL_SEPOFF_RBFON);
            ReserveBeforeSatMode = SATMODE_SAVING_SEPOFF_RBFON;
            break;
        default:
            switchError(error_MPU_commandSwitchSatMode);
            break;
    }
}

void testInitSatMode(){
    WriteOneByteToMainAndSubB0EEPROM(SatelliteMode_addressHigh,SatelliteMode_addressLow,SATMODE_NOMINAL_SEPON_RBFON);
    WriteOneByteToMainAndSubB0EEPROM(BatVol_nominal_saving_datahigh_addressHigh, BatVol_nominal_saving_datahigh_addressLow,Init_TheresholdBatVol_nominal_saving_high);
    WriteOneByteToMainAndSubB0EEPROM(BatVol_nominal_saving_datalow_addressHigh, BatVol_nominal_saving_datalow_addressLow,Init_TheresholdBatVol_nominal_saving_low);
    WriteOneByteToMainAndSubB0EEPROM(BatVol_saving_survival_datahigh_addressHigh, BatVol_saving_survival_datahigh_addressLow,Init_TheresholdBatVol_saving_survival_high);
    WriteOneByteToMainAndSubB0EEPROM(BatVol_saving_survival_datalow_addressHigh,BatVol_saving_survival_datalow_addressLow ,Init_TheresholdBatVol_saving_survival_low);
    WriteOneByteToMainAndSubB0EEPROM(BatVol_nominal_revival_datahigh_addressHigh,BatVol_nominal_revival_datahigh_addressLow,Init_TheresholdBatVol_nominal_revival_high);
    WriteOneByteToMainAndSubB0EEPROM(BatVol_nominal_revival_datalow_addressHigh,BatVol_nominal_revival_datalow_addressLow,Init_TheresholdBatVol_nominal_revival_low );
    WriteOneByteToMainAndSubB0EEPROM(BatVol_saving_revival_datahigh_addressHigh,BatVol_saving_revival_datahigh_addressLow,Init_TheresholdBatVol_saving_revival_high);
    WriteOneByteToMainAndSubB0EEPROM(BatVol_saving_revival_datalow_addressHigh,BatVol_saving_revival_datalow_addressLow,Init_TheresholdBatVol_saving_revival_low);    
    WriteOneByteToMainAndSubB0EEPROM(SW_Change_SavingMode_ADC_addresshigh,SW_Change_SavingMode_ADC_addresslow,0b11100000);
}