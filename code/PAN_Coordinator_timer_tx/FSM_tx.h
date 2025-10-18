#ifndef _FSM_TX_H_      // protective header file // format: #ifndef _<FILE_NAME_In_CAPS>_H_
#define _FSM_TX_H_

#include <stdint.h>



void invoke_state(uint8_t);
void FSM(uint8_t);
void init_state(void);
void check_state(void);
extern uint8_t current_state,next_state; // index of current state
void State(void);

#endif
