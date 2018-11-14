#include <xc.h>
#include "Type_define.h"
#include "I2C.h"
#include "EEPROM.h"
#include "adxl345.h"
#include "ITG3701.h"
#include "ICM20601.h"
#include "time.h"
#include "IMU.h"


void readIMUsequence_ICM(UBYTE ee_p, UBYTE EEPROMH, UBYTE EEPROML, int measuring_time){
    
    UBYTE IMUdata[16] = {};
//    UBYTE delay_time = ee_p && 0x3F;
    UBYTE selEEP = (ee_p && 0xC0) >> 6;
    
    sampling_counter = 0;
    sampling_counterL = 0;
    sampling_counterH = 0;
    while(sampling_counter <= measuring_time){
        if(sampling_counterL == 0xFF){
            sampling_counterL = 0x00;
            sampling_counterH += 0x01;
        }else sampling_counterL += 0x01;
        
        sampling_counter = sampling_counterH << 8 + sampling_counterL;

        IMUdata[0] = sampling_counterH;
        IMUdata[1] = sampling_counterL;
        readICM(IMUdata,2);
        __delay_us(20);
        
        
//            writeEEPROM(selEEP, EEPROMH, EEPROML, IMUdatas, 128);
            __delay_us(3000);
            if(EEPROML == 0x80){
                EEPROMH += 0x01;
                EEPROML = 0x00;
            }else{
                EEPROML = 0x80;
            }
            if(EEPROMH >= 0xF0){
                selEEP += 1;
                EEPROML = 0;
                EEPROMH = 0;
                if(selEEP == 4){
                    break;  //EEPROM full
                }
            }
        }
        

    return;
}
