/* 
 * File:   EEPROM.h
 * Author: Kosuke
 *
 * Created on 2017/04/18, 2:17
 */

#include <xc.h>


#ifndef EEPROM_H
#define	EEPROM_H

#include "typeDefine.h"

/*******************************************************************************
*EEPROM address
******************************************************************************/
#define MAIN_EEPROM_ADDRESS     0x50//
#define SUB_EEPROM_ADDRESS      0x52//
#define MAIN_EEPROM_ADDRESS_B1  0x54//
#define SUB_EEPROM_ADDRESS_B1   0x56//

#define B0select_EEPROM 0
#define B1select_EEPROM 1

/*******************************************************************************
*MPU.c
******************************************************************************/
/*-----------in/out status-----------------*/
#define TRISA_addressHigh 0x81//
#define TRISA_addressLow  0x15//
#define TRISB_addressHigh 0x81//
#define TRISB_addressLow  0x16//
#define TRISC_addressHigh 0x81//
#define TRISC_addressLow  0x17//
#define TRISD_addressHigh 0x81//
#define TRISD_addressLow  0x18//
#define TRISE_addressHigh 0x81//
#define TRISE_addressLow  0x19//

/*-----------high/low status-----------------*/
#define PORTA_addressHigh 0x81//
#define PORTA_addressLow  0x1A//
#define PORTB_addressHigh 0x81//
#define PORTB_addressLow  0x1B//
#define PORTC_addressHigh 0x81//
#define PORTC_addressLow  0x1C//
#define PORTD_addressHigh 0x81//
#define PORTD_addressLow  0x1D//
#define PORTE_addressHigh 0x81//
#define PORTE_addressLow  0x1E//


/*******************************************************************************
*OK and ERROR
******************************************************************************/
#define B0select_for_commandID     0x00
#define HighAddress_for_commandID  HighAddress_for_RXCOBCLastCommandID//
#define LowAddress_for_commandID   LowAddress_for_RXCOBCLastCommandID

#define B0select_for_Ok       0x00
#define HighAddress_for_Ok    HighAddress_for_RXCOBCLastCommandStatus//
#define LowAddress_for_Ok     LowAddress_for_RXCOBCLastCommandStatus

#define B0select_for_Error     0x00
#define HighAddress_for_Error  HighAddress_for_RXCOBCLastCommandStatus//
#define LowAddress_for_Error   LowAddress_for_RXCOBCLastCommandStatus

#define InitialOpe_error_status_addressHigh  0x81
#define InitialOpe_error_status_addressLow   0x0C


#define SatMode_error_status1_addresshigh 0x81//
#define SatMode_error_status1_addresslow 0x01
#define SatMode_error_status2_addresshigh 0x81//
#define SatMode_error_status2_addresslow 0x02
#define SW_Change_SavingMode_ADC_addresshigh 0x81//
#define SW_Change_SavingMode_ADC_addresslow 0x09

/*******************************************************************************
*Last Command ID
******************************************************************************/
#define B0select_for_RXCOBCLastCommand     0x00
#define HighAddress_for_RXCOBCLastCommandID  0x81//
#define LowAddress_for_RXCOBCLastCommandID   0x83
#define HighAddress_for_RXCOBCLastCommandStatus  0x81//
#define LowAddress_for_RXCOBCLastCommandStatus   0x84

//for uplink receive command (target OBC/TXCOBC/RXCOBC)
#define HighAddress_for_LastCommandID  0x81
#define LowAddress_for_LastCommandID   0x1F


/*******************************************************************************
*DATA
******************************************************************************/
/*---initial operation---*/
#define MeltingStatus_addressHigh      0x85
#define MeltingStatus_addressLow       0x00
#define MeltingCounter_addressHigh     0x81
#define MeltingCounter_addressLow      0x0B
#define BatteryVoltage_addressHigh     0x81//
#define BatteryVoltage_addressLow      0x87
#define SatelliteMode_addressHigh       0x81//include SEP(3,2bit) and RBF(1,0bit)//
#define SatelliteMode_addressLow        0x80//include SEP(3,2bit) and RBF(1,0bit)

#define BatVol_nominal_saving_datahigh_addressHigh  0x81//
#define BatVol_nominal_saving_datahigh_addressLow 0x0D
#define BatVol_nominal_saving_datalow_addressHigh  0x81//
#define BatVol_nominal_saving_datalow_addressLow 0x0E
#define BatVol_saving_survival_datahigh_addressHigh  0x81//
#define BatVol_saving_survival_datahigh_addressLow 0x0F
#define BatVol_saving_survival_datalow_addressHigh  0x81//
#define BatVol_saving_survival_datalow_addressLow 0x10

#define BatVol_nominal_revival_datahigh_addressHigh 0x81//
#define BatVol_nominal_revival_datahigh_addressLow 0x11
#define BatVol_nominal_revival_datalow_addressHigh 0x81//
#define BatVol_nominal_revival_datalow_addressLow 0x12
#define BatVol_saving_revival_datahigh_addressHigh 0x81//
#define BatVol_saving_revival_datahigh_addressLow 0x13
#define BatVol_saving_revival_datalow_addressHigh 0x81//
#define BatVol_saving_revival_datalow_addressLow 0x14

#define NTRX_subpower_status_addressHigh 0x80//
#define NTRX_subpower_status_addressLow 0x82

#endif	/* EEPROM_H */

