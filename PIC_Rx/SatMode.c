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

UBYTE MeasureBatVoltageAndChangeSatMode(){
          //------battery voltage measure-------------
//        debug : error handling is not determined
                 
            UBYTE bat_voltage[2];
            UWORD Voltage;//Voltage is 10 bit           
            UBYTE error_status = 0;            
            
            //if Voltage is 0x0000 or very large,read one more time. Then it is still 0x0000 or very large,CHange SafeMode.
            ReadBatVoltageWithPointer(bat_voltage);
            WriteToMainAndSubB0EEPROM(BatteryVoltage_addressHigh,BatteryVoltage_addressLow,bat_voltage);
            Voltage = (UWORD)bat_voltage[0] << 8 | (UWORD)bat_voltage[1];
            if(Voltage == 0x0000 | (Voltage & 0xFC) != 0){
                ReadBatVoltageWithPointer(bat_voltage);
                WriteToMainAndSubB0EEPROM(BatteryVoltage_addressHigh,BatteryVoltage_addressLow,bat_voltage);
                Voltage = (UWORD)bat_voltage[0] << 8 | (UWORD)bat_voltage[1];
                if(Voltage == 0x0000 | (Voltage & 0xFC) != 0){ //ADC read error  
                    UBYTE SWchangeSavingMode = ReadEEPROM(MAIN_EEPROM_ADDRESS,SW_Change_SavingMode_ADC_addresshigh,SW_Change_SavingMode_ADC_addresslow);  
                    UBYTE bitcount = BitCount(SWchangeSavingMode);  
                    error_status = error_status | 0b11000000;
                    if(bitcount >= 2 && bitcount <= 4){
                        SwitchToSavingMode();                    
                        return error_status;
                    }else if(bitcount >= 5 && bitcount <= 7){
                        return error_status;
                    }else{
                        SWchangeSavingMode = ReadEEPROM(SUB_EEPROM_ADDRESS,SW_Change_SavingMode_ADC_addresshigh,SW_Change_SavingMode_ADC_addresslow);
                        bitcount = BitCount(SWchangeSavingMode);                    
                        if(bitcount >= 2 && bitcount <= 4){
                            SwitchToSavingMode();
                            return error_status;
                        }else if(bitcount >= 5 && bitcount <= 7){
                            return error_status;
                        }else{
                            error_status =  0b10101010;
                            return error_status;
                        }
                    }
                }
            }
            
      
            //if BatVol_nominal_saving_high is very large,read one more time. Then it is still very large,thereshold BatVol is Initial Value.
            UWORD BatVol_nominal_saving_high = (UWORD)ReadEEPROM(MAIN_EEPROM_ADDRESS, BatVol_nominal_saving_datahigh_addressHigh, BatVol_nominal_saving_datahigh_addressLow);
            UWORD BatVol_nominal_saving_low = (UWORD)ReadEEPROM(MAIN_EEPROM_ADDRESS, BatVol_nominal_saving_datalow_addressHigh, BatVol_nominal_saving_datalow_addressLow);    
            if((BatVol_nominal_saving_high & 0b11111100) != 0){
                BatVol_nominal_saving_high = (UWORD)ReadEEPROM(SUB_EEPROM_ADDRESS, BatVol_nominal_saving_datahigh_addressHigh, BatVol_nominal_saving_datahigh_addressLow);
                BatVol_nominal_saving_low = (UWORD)ReadEEPROM(SUB_EEPROM_ADDRESS, BatVol_nominal_saving_datalow_addressHigh, BatVol_nominal_saving_datalow_addressLow);
                if((BatVol_nominal_saving_high & 0b11111100) != 0){
                    BatVol_nominal_saving_high = Init_TheresholdBatVol_nominal_saving_high;
                    BatVol_nominal_saving_low = Init_TheresholdBatVol_nominal_saving_low;
                    error_status = error_status | 0b00000011;
                }              
            }
            
            UWORD BatVol_saving_survival_high = (UWORD)ReadEEPROM(MAIN_EEPROM_ADDRESS, BatVol_saving_survival_datahigh_addressHigh, BatVol_saving_survival_datahigh_addressLow);
            UWORD BatVol_saving_survival_low = (UWORD)ReadEEPROM(MAIN_EEPROM_ADDRESS, BatVol_saving_survival_datalow_addressHigh, BatVol_saving_survival_datalow_addressLow);
            if((BatVol_saving_survival_high & 0b11111100) != 0){
                BatVol_saving_survival_high = (UWORD)ReadEEPROM(SUB_EEPROM_ADDRESS, BatVol_saving_survival_datahigh_addressHigh, BatVol_saving_survival_datahigh_addressLow);
                BatVol_saving_survival_low = (UWORD)ReadEEPROM(SUB_EEPROM_ADDRESS, BatVol_saving_survival_datalow_addressHigh, BatVol_saving_survival_datalow_addressLow);
                if((BatVol_saving_survival_high & 0b11111100) != 0){
                    BatVol_saving_survival_high = Init_TheresholdBatVol_saving_survival_high;
                    BatVol_saving_survival_low = Init_TheresholdBatVol_saving_survival_low;
                    error_status = error_status | 0b00000011;
                }              
            }

            UWORD BatVol_nominal_revival_high = (UWORD)ReadEEPROM(MAIN_EEPROM_ADDRESS, BatVol_nominal_revival_datahigh_addressHigh, BatVol_nominal_revival_datahigh_addressLow);
            UWORD BatVol_nominal_revival_low = (UWORD)ReadEEPROM(MAIN_EEPROM_ADDRESS, BatVol_nominal_revival_datalow_addressHigh, BatVol_nominal_revival_datalow_addressLow);
            if((BatVol_nominal_revival_high & 0b11111100) != 0){
                BatVol_nominal_revival_high = (UWORD)ReadEEPROM(SUB_EEPROM_ADDRESS, BatVol_nominal_revival_datahigh_addressHigh, BatVol_nominal_revival_datahigh_addressLow);
                BatVol_nominal_revival_low = (UWORD)ReadEEPROM(SUB_EEPROM_ADDRESS, BatVol_nominal_revival_datalow_addressHigh, BatVol_nominal_revival_datalow_addressLow);
                if((BatVol_nominal_revival_high & 0b11111100) != 0){
                    BatVol_nominal_revival_high = Init_TheresholdBatVol_nominal_revival_high;
                    BatVol_nominal_revival_low = Init_TheresholdBatVol_nominal_revival_low;
                    error_status = error_status | 0b00000011;
                }              
            }
            
            UWORD BatVol_saving_revival_high = (UWORD)ReadEEPROM(MAIN_EEPROM_ADDRESS, BatVol_saving_revival_datahigh_addressHigh, BatVol_saving_revival_datahigh_addressLow);
            UWORD BatVol_saving_revival_low = (UWORD)ReadEEPROM(MAIN_EEPROM_ADDRESS, BatVol_saving_revival_datalow_addressHigh, BatVol_saving_revival_datalow_addressLow);
            if((BatVol_saving_revival_high & 0b11111100) != 0){
                BatVol_saving_revival_high = (UWORD)ReadEEPROM(SUB_EEPROM_ADDRESS, BatVol_saving_revival_datahigh_addressHigh, BatVol_saving_revival_datahigh_addressLow);
                BatVol_saving_revival_low = (UWORD)ReadEEPROM(SUB_EEPROM_ADDRESS, BatVol_saving_revival_datalow_addressHigh, BatVol_saving_revival_datalow_addressLow);
                if((BatVol_saving_revival_high & 0b11111100) != 0){
                    BatVol_saving_revival_high = Init_TheresholdBatVol_saving_revival_high;
                    BatVol_saving_revival_low = Init_TheresholdBatVol_saving_revival_low;
                    error_status = error_status | 0b00000011;
                }              
            }           
            
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
                            error_status = error_status | 0b00001100;
                            break;                       
                    }
                    break;
            }

            switch(ChoicedSatMode){
                case SATMODE_NOMINAL:
                    if(Voltage >= (BatVol_nominal_saving_high << 8 | BatVol_nominal_saving_low)) {
                        //write SatMode nominal(SEP -> ON, RBF -> ON)                        
                        switch(OBC_STATUS){
                            case OBC_ALIVE:                               
                                break;
                            case OBC_DIED:
                                killEPS();
                                onEPS();                                                                          
                                setPLL();
                                break;
                            default:    
                                break;
                        }
                        WriteOneByteToMainAndSubB0EEPROM(SatelliteMode_addressHigh, SatelliteMode_addressLow, SATMODE_NOMINAL_SEPON_RBFON);
                        ReserveBeforeSatMode = SATMODE_NOMINAL_SEPON_RBFON;
                    }else if(Voltage <= ((UWORD)BatVol_saving_survival_high<<8 | (UWORD)BatVol_saving_survival_low)){
                        //write SatMode survival(SEP -> OFF, RBF -> ON)                      
                        killEPS();
                        WriteOneByteToMainAndSubB0EEPROM(SatelliteMode_addressHigh, SatelliteMode_addressLow, SATMODE_SURVIVAL_SEPOFF_RBFON);
                        ReserveBeforeSatMode = SATMODE_SURVIVAL_SEPOFF_RBFON;
                    }else{
                        //Write SatMode saving(SEP -> OFF, RBF -> ON)                        
                        killEPS();                   
                        onNtrxPowerSupplyCIB(0,0);
                        WriteOneByteToMainAndSubB0EEPROM(SatelliteMode_addressHigh, SatelliteMode_addressLow, SATMODE_SAVING_SEPOFF_RBFON);
                        ReserveBeforeSatMode = SATMODE_SAVING_SEPOFF_RBFON;
                    }
                    break;
                case SATMODE_SAVING:
                    if(Voltage >= ((UWORD)BatVol_nominal_revival_high <<8 |(UWORD)BatVol_nominal_revival_low)){
                        //write SatMode nominal(SEP -> ON, RBF -> ON)                                               
                        offNtrxPowerSupplyCIB();
                        onEPS();
                        setPLL();
                        WriteOneByteToMainAndSubB0EEPROM(SatelliteMode_addressHigh, SatelliteMode_addressLow, SATMODE_NOMINAL_SEPON_RBFON);
                        ReserveBeforeSatMode = SATMODE_NOMINAL_SEPON_RBFON;
                    }else if (Voltage <= ((UWORD)BatVol_saving_survival_high << 8 | (UWORD)BatVol_saving_survival_low)){
                        //write SatMode survival(SEP -> OFF, RBF -> ON)                       
                        offNtrxPowerSupplyCIB();
                        WriteOneByteToMainAndSubB0EEPROM(SatelliteMode_addressHigh, SatelliteMode_addressLow, SATMODE_SURVIVAL_SEPOFF_RBFON);
                        ReserveBeforeSatMode = SATMODE_SURVIVAL_SEPOFF_RBFON;
                    }else{
                        //Write SatMode saving(SEP -> OFF, RBF -> ON)
                        WriteOneByteToMainAndSubB0EEPROM(SatelliteMode_addressHigh, SatelliteMode_addressLow, SATMODE_SAVING_SEPOFF_RBFON);
                        ReserveBeforeSatMode = SATMODE_SAVING_SEPOFF_RBFON;
                    }
                    break;
                case SATMODE_SURVIVAL:
                    if(Voltage >= ((UWORD)BatVol_nominal_revival_high <<8 | (UWORD)BatVol_nominal_revival_low)){
                        //write SatMode nominal(SEP -> ON, RBF -> ON)                       
                        onEPS();
                        setPLL();
                        WriteOneByteToMainAndSubB0EEPROM(SatelliteMode_addressHigh, SatelliteMode_addressLow, SATMODE_NOMINAL_SEPON_RBFON);
                        ReserveBeforeSatMode = SATMODE_NOMINAL_SEPON_RBFON;
                    }else if (Voltage <= ((UWORD)BatVol_saving_revival_high << 8 | (UWORD)BatVol_saving_revival_low)){
                        //write SatMode survival(SEP -> OFF, RBF -> ON)
                        WriteOneByteToMainAndSubB0EEPROM(SatelliteMode_addressHigh, SatelliteMode_addressLow, SATMODE_SURVIVAL_SEPOFF_RBFON);
                        ReserveBeforeSatMode = SATMODE_SURVIVAL_SEPOFF_RBFON;
                    }else{
                        //Write SatMode saving(SEP -> OFF, RBF -> ON)                      
                        onNtrxPowerSupplyCIB(0,0);
                        WriteOneByteToMainAndSubB0EEPROM(SatelliteMode_addressHigh, SatelliteMode_addressLow, SATMODE_SAVING_SEPOFF_RBFON);
                        ReserveBeforeSatMode = SATMODE_SAVING_SEPOFF_RBFON;
                    }                  
                    break;
                default:       
                    error_status = error_status | 0b00110000;
                    break;
        }
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
    __delay_ms(500);
    onNtrxPowerSupplyCIB(0,0);
    __delay_ms(2000);//wait EPS ON
    setPLL();
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