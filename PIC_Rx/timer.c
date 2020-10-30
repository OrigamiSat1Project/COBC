#include <xc.h>
#include "timer.h"
#include "UART.h"
#include "typeDefine.h"
#include "pinDefine.h"
#include "EPS.h"
#include "time.h"
#include "FMCW.h"
#include "ADC.h"
#include "I2C.h"
#include "MPU.h"
#include "EEPROM.h"
#include "OkError.h"
#include "WDT.h"

void initTimer(void){
    INTCON = 0b11100000;    //GIE = 1, PEIE = 1, TMR0IE = 1
    OPTION_REG = 0b01000111;    //prescaler is assigned, TMR0 rate 1:256
    TMR0 = 0x00;    //Initializing Timer0 Module Register
}

void interruptI2C(void)
{
    if (PIR1bits.SSPIF == 1) {
        if (AckCheck == 1) {
            putChar('A');
            putChar('C');
            putChar('K');
            putChar('\r');
            putChar('\n');
            AckCheck = 0;
        }

        putChar('S');
        putChar('S');
        putChar('P');
        putChar('\r');
        putChar('\n');
        PIR1bits.SSPIF = 0;
    }
    if (PIR2bits.BCLIF == 1) {
       putChar('B');
       putChar('C');
       putChar('L');
       putChar('\r');
       putChar('\n');
       CollisionCheck = 1;
       PIR2bits.BCLIF = 0;
    }
}

UBYTE EPS_reset_time = EPS_RSET_INTERVAL_SHORT;
UWORD time = 0;

static UINT timer_counter   = 0;
static UINT second_counter  = 0;
static UINT minute_counter  = 0;
static UINT hour_counter    = 0;
static UINT day_counter     = 0;
static UINT week_counter    = 0;

static UINT NTRX_pll_setting_counter_sec  = 0;
static UINT NTRX_pll_setting_counter_min  = 0;
static UINT NTRX_pll_setting_counter_hour = 0;
static UINT NTRX_pll_setting_counter_day  = 0;
static UINT receive_command_counter_sec = 0;
static UINT receive_command_counter_min = 0;
static UINT bat_meas_counter_sec        = 0;
static UINT bat_meas_counter_min        = 0;
//static UINT eps_rest_counter_sec        = 0;
static UINT init_ope_counter_sec        = 0;
static UINT init_ope_counter_min        = 0;
UBYTE WDT_flag = 0x00;

//for debug
static UINT eps_reset_counter_sec = 0;
static UINT eps_reset_counter_min = 0;

//for debug function
void interrupt TimerCheck(void){
    interruptI2C();

    if(INTCONbits.TMR0IF){
        INTCONbits.TMR0IF = 0;
        TMR0 = 0x00;
        timer_counter ++;
    }

    if(timer_counter >= one_second){
        timer_counter = 0;
        second_counter += 1;

        NTRX_pll_setting_counter_sec ++;
        eps_reset_counter_sec ++; //for debug
        init_ope_counter_sec ++;
        bat_meas_counter_sec ++;
        receive_command_counter_sec ++;
        WDT_flag = 0x01;
    }
    if(second_counter >= one_minute){
        second_counter = 0;
        minute_counter ++;
    }
    if(NTRX_pll_setting_counter_sec >= one_minute){
        NTRX_pll_setting_counter_sec = 0;
        NTRX_pll_setting_counter_min ++;
    }
    //for debug
    if(eps_reset_counter_sec >= one_minute){
        eps_reset_counter_sec = 0;
        eps_reset_counter_min ++;
    }
    if(receive_command_counter_sec >= one_minute){
        receive_command_counter_sec = 0;
        receive_command_counter_min ++;
    }
    if(init_ope_counter_sec >= one_minute){
        init_ope_counter_sec = 0;
        init_ope_counter_min ++;
    }
    if(bat_meas_counter_sec >= one_minute){
        bat_meas_counter_sec = 0;
        bat_meas_counter_min ++;
    }
    if(minute_counter >= one_hour){
        minute_counter = 0;
        hour_counter ++;
    }
    if(NTRX_pll_setting_counter_min >= one_hour){
        NTRX_pll_setting_counter_min = 0;
        NTRX_pll_setting_counter_hour ++;
    }

    if(hour_counter >= one_day){
        hour_counter = 0;
        day_counter ++;
    }
    if(NTRX_pll_setting_counter_hour >= one_day){
        NTRX_pll_setting_counter_hour = 0;
        NTRX_pll_setting_counter_day ++;
    }

    if(day_counter >= one_week){
        day_counter = 0;
        week_counter ++;
    }
    if(week_counter >= 2){
        week_counter = 0;
    }
}

void set_NTRX_pll_setting_counter(UINT time_sec, UINT time_min, UINT time_hour, UINT time_day){
    NTRX_pll_setting_counter_sec = time_sec;
    NTRX_pll_setting_counter_min = time_min;
    NTRX_pll_setting_counter_min = time_hour;
    NTRX_pll_setting_counter_min = time_day;
}
UINT get_NTRX_pll_setting_counter_sec(void){
    return NTRX_pll_setting_counter_sec;
}
UINT get_NTRX_pll_setting_counter_min(void){
    return NTRX_pll_setting_counter_min;
}
UINT get_NTRX_pll_setting_counter_hour(void){
    return NTRX_pll_setting_counter_hour;
}
UINT get_NTRX_pll_setting_counter_day(void){
    return NTRX_pll_setting_counter_day;
}


//for debug
void set_eps_reset_counter(UINT time_sec, UINT time_min){
    eps_reset_counter_sec = time_sec;
    eps_reset_counter_min = time_min;
}

//FIXME:for debug
UINT get_eps_reset_counter_sec(void){
    return eps_reset_counter_sec;
}

//for debug
UINT get_eps_reset_counter_min(void){
    return eps_reset_counter_min;
}


void set_receive_command_counter(UINT time_sec, UINT time_min){
    receive_command_counter_sec = time_sec;
    receive_command_counter_min = time_min;
}

//FIXME:for debug
UINT get_receive_command_counter_sec(void){
    return receive_command_counter_sec;
}

UINT get_receive_command_counter_min(void){
    return receive_command_counter_min;
}

void set_init_ope_counter(UINT time_sec, UINT time_min){
    init_ope_counter_sec = time_sec;
    init_ope_counter_min = time_min;
}

//for debug
UINT get_init_ope_counter_sec(void){
    return init_ope_counter_sec;
}

UINT get_init_ope_counter_min(void){
    return init_ope_counter_min;
}

void set_bat_meas_counter(UINT time_sec, UINT time_min){
    bat_meas_counter_sec = time_sec;
    bat_meas_counter_min = time_min;
}

//for debug
UINT get_bat_meas_counter_sec(void){
    return bat_meas_counter_sec;
}

UINT get_bat_meas_counter_min(void){
    return bat_meas_counter_min;
}

UINT get_timer_counter(UBYTE unit){
    switch(unit){
        case 's':
            return second_counter;
        case 'm':
            return minute_counter;
        case 'h':
            return hour_counter;
        case 'd':
            return day_counter;
        case 'w':
            return week_counter;
        default:
            return 0;
    }
}

void reset_timer(void){
    timer_counter   = 0;
    second_counter  = 0;
    minute_counter  = 0;
    hour_counter    = 0;
    day_counter     = 0;
    week_counter    = 0;
}