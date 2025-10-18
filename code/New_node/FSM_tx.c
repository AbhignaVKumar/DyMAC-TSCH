#include "nrf_drv_spi.h"
#include "app_util_platform.h"
#include "nrf_gpio.h"
#include "nrf_delay.h"
#include "boards.h"
#include "app_error.h"
#include <string.h>
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#include "reg_cmd_tx.h"
#include "read_write_func_tx.h"
#include "FSM_tx.h"

uint8_t current_state=0;
uint8_t next_state=0; // index of current state
uint8_t status=0; // input
int radio_state=0;
 

void invoke_state(uint8_t current_state1)
{
       m_tx_buf[0]=(current_state)|(0x80);    //invoke state radio command
       m_tx_buf[1]=0xff;
       //m_tx_buf[2]=0xff;
       nrf_drv_spi_transfer(&spi,m_tx_buf,2,m_rx_buf,2);
       nrf_delay_us(250);    
       //for(i=0;i<4;i++)
       // printf("m_rx_buff[%d]: %x \n",i, m_rx_buf[i]); // to check the status byte
}

void FSM_2(uint8_t input)
{
  char flag=0;
  switch(input)
  {
  case 0x00: if(current_state==PHY_OFF||current_state==PHY_ON)
          {
            next_state = PHY_SLEEP;
          }
          else
          {
            flag=1;
          }
  case 0x01: if(current_state==PHY_ON)
          {
            next_state = PHY_OFF;
          }
          else
          {
            flag=1;
          }
          break;
  case 0x02: if(current_state==PHY_OFF||current_state==PHY_RX||current_state==PHY_TX||current_state==CCA)
          {
            next_state = PHY_ON;
          }
          else
          {
            flag=1;
          }
          break;
  case 0x03: if(current_state==PHY_ON||current_state==PHY_TX||current_state==PHY_RX)
          {
            next_state = PHY_RX;
          }
          else
          {
            flag=1;
          }
          break;
  case 0x04: if(current_state==PHY_ON||current_state==PHY_TX||current_state==PHY_RX||current_state==CCA)
          {
            next_state = PHY_TX;
          }
          else
          {
            flag=1;
          }
          break;
  case 0x05: if(current_state==PHY_OFF)
          {
            next_state = CFG_DEV;
          }
          else
          {
            flag=1;
          }
          break;
  case 0x06: if(current_state==PHY_ON)
          {
            next_state = CCA;
          }
          else
          {
            flag=1;
          }
          break;
  default:flag=1;
          break;
  }
current_state=next_state;
if(flag==0)
  invoke_state(current_state);
else 
  printf("INVALID STATE");
}


void FSM(uint8_t input) //With status byte from ADF7030
{
  
  State();
  char flag=0;
  if(status==0)
    flag=1;
  if(input != current_state)
  {
  switch(input)
  {
  case 0x00: if(current_state==PHY_OFF||current_state==PHY_ON)
          {
            next_state = PHY_SLEEP;
          }
          else
          {
            flag=1;
          }
  case 0x01: if(current_state==PHY_ON)
          {
            next_state = PHY_OFF;
          }
          else
          {
            flag=1;
          }
          break;
  case 0x02: if(current_state==PHY_OFF||current_state==PHY_RX||current_state==PHY_TX||current_state==CCA)
          {
            next_state = PHY_ON;
          }
          else
          {
            flag=1;
          }
          break;
  case 0x03: if(current_state==PHY_ON||current_state==PHY_TX||current_state==PHY_RX)
          {
            next_state = PHY_RX;
          }
          else
          {
            flag=1;
          }
          break;
  case 0x04: if(current_state==PHY_ON||current_state==PHY_TX||current_state==PHY_RX||current_state==CCA)
          {
            next_state = PHY_TX;
          }
          else
          {
            flag=1;
          }
          break;
  case 0x05: if(current_state==PHY_OFF)
          {
            next_state = CFG_DEV;
          }
          else
          {
            flag=1;
          }
          break;
  case 0x06: if(current_state==PHY_ON)
          {
            next_state = CCA;
          }
          else
          {
            flag=1;
          }
          break;
  case 0x09: if(current_state==PHY_ON)
          {
            next_state = DO_CAL;
          }
          else
          {
            flag=1;
          }
          break;
  case 0x10: if(current_state==PHY_ON)
          {
            next_state = GPCLK;
          }
          else
          {
            flag=1;
          }
          break;
  case 0x0A: if(current_state==PHY_ON)
          {
            next_state = MON;
          }
          else
          {
            flag=1;
          }
          break;
  case 0x0C: if(current_state==PHY_ON)
          {
            next_state = LFRC_CAL;
          }
          else
          {
            flag=1;
          }
          break;
  default:flag=1;
          break;
  }
current_state=next_state;
if(flag==0)
  invoke_state(current_state);
else 
  printf("INVALID STATE TRANSITION");
}
else
printf("Already in the commanded state");
}



//void init_state(void)
//{
//printf("Starting intiate\n");
//invokeIdle();
//}

//Checking the State of the radio
void State()
{
  read_MISC_FW_reg();
 // state=0;
  radio_state=m_rx_buf[9]&(0x3F);
  status=m_rx_buf[10]&(0x03);
  switch(radio_state)
  {
    case PHY_SLEEP :printf("Radio is in SLEEP and ");
                     current_state=PHY_SLEEP;
                     break;
    case PHY_OFF : printf("Radio is in PHY_OFF and ");
                   current_state=PHY_OFF;
                   break;
    case PHY_ON : printf("Radio is in PHY_ON and ");
                  current_state=PHY_ON;
                  break;
    case PHY_RX : printf("Radio is in PHY_RX and ");
                  current_state=PHY_RX;
                  break;
    case PHY_TX : printf("Radio is in PHY_TX and ");
                  current_state=PHY_TX;
                  break;
    case CFG_DEV : printf("Radio is in Configuring and ");
                   current_state=CFG_DEV;
                   break;
    case CCA : printf("Radio is in CCA and ");
               current_state=CCA;
               break;
    case DO_CAL : printf("Radio is in Caliberating and ");
                  current_state=DO_CAL;
                  break;
    case MON : printf("Radio is in Monitoring and ");
               current_state=MON;
               break;
    case GPCLK : printf("Radio is in GPCLK state and ");
               current_state=GPCLK;
               break;
    case LFRC_CAL : printf("Radio is in LFRC_CAL state  and ");
               current_state=LFRC_CAL;
               break;
    default : printf(" *************Error State undefined ************* ");
              status=3;
              break;
  }
 // state=0;
  switch(status)
  {
    case 0 : printf("trasitioning state\n");
             break;
    case 1 : printf("Executing state\n");
             break;
    case 2 : printf("idle state\n");
           //  state=1;
             break;
    default: printf("\n");
             break;
  }
  printf("\n");
}














