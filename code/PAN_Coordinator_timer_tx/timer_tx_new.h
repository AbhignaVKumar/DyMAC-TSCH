#ifndef _TIMER_TX_NEW_H_      // protective header file // format: #ifndef _<FILE_NAME_In_CAPS>_H_
#define _TIMER_TX_NEW_H_

#include <stdint.h>

extern uint32_t prev_time,curr_time;

static void debug_tx (void);
void start_get_timer_tx(void);
uint32_t get_time_tx(void);
void start_timer_tx(void);

void TIMER2_IRQHandler(void);
void ADFInterruptHandler_tx(void);
void start_gpiote_tx(void);

static void func_idle_to_phyrdy(void);
static void func_phyrdy_to_pktsent(void);
static void func_phyrdy_ack_rcvd(void);
//static void func_txtorx_ack_rcvd(void);
static void func_phyrdy_to_ccacomp(void);

void Phy_to_Cca(void);//added on 29-11-21s

void timeslot(void);
void Enhanced_Beacon(void);
void timeslot_tx(void);
void timeslot_rx(void);//added on 16-10-21
void broadcast_EB(void);
void receive_EB(void);
void ADF7030InterruptHandler(void);
void config_change_100kbps(void);
void config_change_150kbps(void);

#endif // _TIMER_TX_NEW_H_



