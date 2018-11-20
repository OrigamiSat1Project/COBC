

#ifndef TIMER_H
#define	TIMER_H

#include "typeDefine.h"
#define one_second 38
#define one_minute 60
#define one_hour 60
#define one_day 24
#define one_week 7

#define NTRX_PLL_INTERVAL        1
#define EPS_RSET_INTERVAL_LONG   10
#define EPS_RSET_INTERVAL_SHORT  2
#define INITIAL_OPE_INTERVAL     23
#define COMMAND_COUNTER_INTERVAL 50      
#define EPS_MEASURE_INTERVAL     1        


void initTimer(void);
void interrupt TimerCheck(void);

int CollisionCheck;
int AckCheck;

void set_NTRX_pll_setting_counter(UINT, UINT, UINT, UINT);
UINT get_NTRX_pll_setting_counter_sec(void);
UINT get_NTRX_pll_setting_counter_min(void);
UINT get_NTRX_pll_setting_counter_hour(void);
UINT get_NTRX_pll_setting_counter_day(void);

void set_eps_reset_counter(UINT, UINT);  //FIXME:for debug
UINT get_eps_reset_counter_sec(void);   //FIXME:for debug
UINT get_eps_reset_counter_min(void);   //FIXME:for debug

void set_receive_command_counter(UINT, UINT);
UINT get_receive_command_counter_sec(void);  //FIXME:for debug
UINT get_receive_command_counter_min(void);

void set_init_ope_counter(UINT, UINT);
UINT get_init_ope_counter_sec(void);   //FIXME:for debug
UINT get_init_ope_counter_min(void);

void set_bat_meas_counter(UINT, UINT);
UINT get_bat_meas_counter_sec(void);   //FIXME:for debug
UINT get_bat_meas_counter_min(void);

UINT get_timer_counter(UBYTE unit);

void reset_timer(void);

//void interrupt TimerReset(void);

#endif	/* TIME_H */
