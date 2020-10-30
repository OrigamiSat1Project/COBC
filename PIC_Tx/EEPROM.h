/*
 * File:   EEPROM.h
 * Author: Kosuke
 *
 * Created on 2017/04/18, 2:17
 */

#include <xc.h>
#include "Type_define.h"

#ifndef EEPROM_H
#define	EEPROM_H

/*******************************************************************************
*EEPROM address
******************************************************************************/
#define EEPROM_address 0x50
#define EEPROM_address_B1 0x54
#define EEPROM_subaddress 0x52
#define EEPROM_subMaddress_B1 0x56

/*******************************************************************************
*OK and ERROR
******************************************************************************/

#define B0select_for_Ok       0x00
#define HighAddress_for_Ok    TXCOBC_CommandErrorStatus_addressHigh
#define LowAddress_for_Ok     TXCOBC_CommandErrorStatus_addressLow

#define B0select_for_Error     0x00
#define HighAddress_for_Error  TXCOBC_CommandErrorStatus_addressHigh
#define LowAddress_for_Error   TXCOBC_CommandErrorStatus_addressLow

/*******************************************************************************
*Last Command ID
******************************************************************************/
#define B0select_for_TXCOBCLastCommandID            0x00
#define HighAddress_for_TXCOBCLastCommandID         0x81
#define LowAddress_for_TXCOBCLastCommandID          0x85
#define HighAddress_for_TXCOBCLastCommandStatus     TXCOBC_CommandErrorStatus_addressHigh
#define LowAddress_for_TXCOBCLastCommandStatus      TXCOBC_CommandErrorStatus_addressLow
#define HighAddress_for_RXCOBCLastCommandID         0x81//
#define LowAddress_for_RXCOBCLastCommandID          0x83//

#define OffSet_for_CommandID 1

/*******************************************************************************
*ADC value
******************************************************************************/
//ch1 (Battery Temperature))
#define adcValue_CH1_addressHigh 0x81
#define adcValue_CH1_addressLow  0x81
//ch2 (5VBUS voltage(EPS 5V))
#define adcValue_CH2_addressHigh 0x81
#define adcValue_CH2_addressLow  0x8B
//ch3 (3V3BUS voltage(EPS 3.3V))
#define adcValue_CH3_addressHigh 0x81
#define adcValue_CH3_addressLow  0x89
//ch4
#define adcValue_CH4_addressHigh 0x80//
#define adcValue_CH4_addressLow  0x80//

/*******************************************************************************
*address for satellite mode
******************************************************************************/
//TODO:change address
#define satelliteMode_addressHigh       0x81//include SEP(3,2bit) and RBF(1,0bit)
#define satelliteMode_addressLow        0x80//include SEP(3,2bit) and RBF(1,0bit)
#define satelliteMode_DataSize          1

/*******************************************************************************
*CW downlink
******************************************************************************/

#define RXCOBC_CommandErrorStatus_addressHigh 0x81//
#define RXCOBC_CommandErrorStatus_addressLow 0x84//
#define TXCOBC_CommandErrorStatus_addressHigh 0x81//
#define TXCOBC_CommandErrorStatus_addressLow 0x86//

//HK
#define BatteryVoltageOBC_addressHigh  0x81
#define BatteryVoltageOBC_addressLow 0x8D
#define BatteryVoltageCIB_addressHigh  0x81
#define BatteryVoltageCIB_addressLow 0x87
#define MeltingStatus_addressHigh  0x85
#define MeltingStatus_addressLow  0x00
#define LatestExcutionCommandID_addressHigh   0x81//
#define LatestExcutionCommandID_addressLow   0x8F//
#define OBC_CommandErrorStatus_addressHigh 0x81//
#define OBC_CommandErrorStatus_addressLow 0x90//
#define BatteryCurrent_addressHigh 0x81//
#define BatteryCurrent_addressLow  0x91//
#define EpsSwitchStatus_addressHigh   0x81//
#define EpsSwitchStatus_addressLow   0x93//
#define TxTemperature_addressHigh 0x81//
#define TxTemperature_addressLow 0x95//
#define RxTemperature_addressHigh 0x81//
#define RxTemperature_addressLow 0x96//

#define FreeData1_slaveaddress_addressHigh 0x80//
#define FreeData1_slaveaddress_addressLow 0x83//
#define FreeData1Highaddress_addressHigh 0x80//
#define FreeData1Highaddress_addressLow 0x84//
#define FreeData1Lowaddress_addressHigh 0x80//
#define FreeData1Lowaddress_addressLow 0x85//

#define FreeData2_slaveaddress_addressHigh 0x80//
#define FreeData2_slaveaddress_addressLow 0x86//
#define FreeData2Highaddress_addressHigh 0x80//
#define FreeData2Highaddress_addressLow 0x87//
#define FreeData2Lowaddress_addressHigh 0x80//
#define FreeData2Lowaddress_addressLow 0x88//

#define NTRX_subpower_status_addressHigh 0x80//
#define NTRX_subpower_status_addressLow 0x82//

/*******************************************************************************
*MPU.c
******************************************************************************/
/*-----------in/out status-----------------*/
#define TRISA_addressHigh 0x80
#define TRISA_addressLow  0x89
#define TRISB_addressHigh 0x80
#define TRISB_addressLow  0x8A
#define TRISC_addressHigh 0x80
#define TRISC_addressLow  0x8B
#define TRISE_addressHigh 0x80
#define TRISE_addressLow  0x8C

/*-----------high/low status-----------------*/
#define PORTA_addressHigh 0x80
#define PORTA_addressLow  0x8D
#define PORTB_addressHigh 0x80
#define PORTB_addressLow  0x8E
#define PORTC_addressHigh 0x80
#define PORTC_addressLow  0x8F
#define PORTE_addressHigh 0x80
#define PORTE_addressLow  0x90


#define SatMode_error_status1_addresshigh 0x81//
#define SatMode_error_status1_addresslow 0x01

#endif	/* EEPROM_H */
