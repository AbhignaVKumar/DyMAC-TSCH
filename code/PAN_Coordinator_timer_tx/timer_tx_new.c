#include "nrf_drv_spi.h"
#include "app_util_platform.h"
#include "nrf_gpio.h"
#include "nrf_delay.h"
#include "boards.h"
#include "app_error.h"
#include <string.h>
//#include "reg_cmd_tx.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
#include "stdio.h"
#include <stdbool.h>
#include <stdint.h>
#include "app_timer.h"
#include "nrf_drv_clock.h"
#include "nrf.h"
#include "nrf_gpiote.h"
#include "nrf_drv_gpiote.h"
#include "sdk_config_tx.h"

#include "reg_cmd_tx.h"
#include "read_write_func_tx.h"
#include "FSM_tx.h"
#include "timer_tx_new.h"
#include "Configuration.h"


int rssi;
int rssi_decimal;
long ASN =0;
int ASN_count=0;
uint32_t total_interrupt_count=0;
int pckt_write=0;
//int index=1;
uint32_t time_arr[10];
uint32_t get_time_tx(void);
uint32_t total_time=0; 
uint32_t cca_time=0; 
uint64_t Mega_time=0;                                                     // Holds the total time period
static uint32_t adf_freq,f;
int look_up_table[4]={0x337055C0,0x3371DC60,0x33736300,0x3374E9A0};   // Look up table for frequecy hopping
uint16_t cca_result=1;
uint64_t ctr_dn=0;
#define interrupt_from_adf_tx NRF_GPIO_PIN_MAP(0,26)                        // Interrupt pin either P0.26 pr P0.27 
#define interrupt_from_arduino NRF_GPIO_PIN_MAP(0,02)

uint32_t prev_time,curr_time;  
//added wrt 7242
int TotalLinks=4;
int CurTimeslot=0;

//Schedule slotframe and link IE according to macPIB attributes defined in page number 386 of the standard
//macSlotframeHandle is to be mentioned in both slotframe and Linktable PIB attributes, For now as only a single slotframe is being used the macSlotframeHandleis mentioned only in macSlotframe
//Both Slotframe anf Link PIB attributes are implemented as structure
struct slotframeTable
{
  uint8_t macSlotframeHandle;
  int macSlotframeSize;
  bool macSlotframeAdvertise;//Advertise the slotframe details in EB
} macSlotframeTable;

struct LinkTable
{
  int macLinkHandle;
  uint8_t state; //macRxType<<1|macTxType
  bool macSharedType;
  bool macLinkTimeKeeping;
  bool macPriorityType;
  bool macLinkType;
  bool macNodeAddressMode; //0 for long addres 1 for short address. In standard 
  double macNodeAddress;
  int macTimeslot;
  int macChannelOffset;
  bool macLinkAdvertise;
} macLinkTable[4];
//till here 

//TX variables                                                         // Flag for interrupt control
int tx_delay_tx[4] ={0,0,0,0};    //For network creation done on 13-04-2022
int prev_delay=0; 
int sfd_flag=0; 
int lss_tx[4]={0,0,0,0};     //For network creation done on 13-04-2022
//End of TX variables    
                                         // Timestamp variables
int flag_gpio_tx=0;                                                         // Flag for interrupt control
int number_of_channels=4;
static int rc_ready_Check=0;                                                // To capture RC_ready interrupt
int stop =0;   
//int state=0;
//RX variables
int previous_time[4]={0,0,0,0};
int current_time=0;
int flag[4]={1,1,1,1};
int offset=0;  
int rx_delay[4]={0,0,0,0};
int tx_delay_rx[4]={0,0,0,0};
int enable_offset[4]={0,0,0,0};                                                           // Stop Flag which indicates the timer has finished counting
int c[4]={0,0,0,0};
int read=0;
//extern uint8_t m_rx_buf[150];
int rx_fast[4]={-1,-1,-1,-1};
int tx_fast[4]={-1,-1,-1,-1};
//End of RX variables
//EB variables
int timeslot_period=10000;
int *P;//pointer to acces IE
//End of EB variables
//int i;n
APP_TIMER_DEF(time_out_operation);  

//Timer handler for ADF7242

 void timer_handler_1()                                                     // Timer Handler function
{
 // function needed to be done afert the timer expries
 //Write the code which is required	
  stop=1;
  printf("\n********************Interrupt Raised ******************\n");
}


static void create_timers()                                                // Create timer function
{   
    uint32_t err_code;

    // Create timers
    err_code = app_timer_create(&time_out_operation, APP_TIMER_MODE_SINGLE_SHOT,timer_handler_1);
    APP_ERROR_CHECK(err_code);
}



/* Function starting the internal LFCLK XTAL oscillator.
 */
static void lfclk_config(void)                                            // Low frequency clock configuration function
{
    nrf_drv_clock_init();
    nrf_drv_clock_lfclk_request(NULL);
}
static void lfclk_request(void)
{
    ret_code_t err_code = nrf_drv_clock_init();
    APP_ERROR_CHECK(err_code);
    nrf_drv_clock_lfclk_request(NULL);
}

static void channel_write(long asn)                                  // Frequency Hopping Method for adf 7030 written on 16-02-2023
{
    
      uint8_t x= (asn + macLinkTable[CurTimeslot].macChannelOffset) % 4;    //For network creation done on 13-04-2022                                            // To get the index value for accesing the lookup table
      adf_freq = look_up_table[x] ;   
      //adf_freq=0x3371DC60;              // Frequency change formula					
       //freq is in Hz...refer to pg. 430 of the 2020 std
       m_tx_buf[0] = 0x38;                                  // Command to write the channel frequency in the ADF Register (Lower byte first)                     
       m_tx_buf[1] = 0x20;                     //address of the freaquency register in 7030 0x200002EC,
       m_tx_buf[2] = 0x00;
       m_tx_buf[3] = 0x02;
       m_tx_buf[4] = 0xEC;
       m_tx_buf[5] = (adf_freq >> 24);
       m_tx_buf[6] = (adf_freq >> 16);
       m_tx_buf[7] = (adf_freq >> 8);
       m_tx_buf[8] =  adf_freq;
			
       nrf_drv_spi_transfer(&spi,m_tx_buf,9,m_rx_buf,8);
       nrf_delay_us(250); //check this 
       //       printf("The contents of m_rx_buf are");   
       //for(int i =0;i<=8;i++)
       //   printf("m_rx_buf[%d] = %x\t",i,m_rx_buf[i]);    
}
static void static_channel_write(int y)                                  // Frequency Hopping Method for adf 7030 written on 16-02-2023
{
    
      uint8_t x= y;    //For network creation done on 13-04-2022                                            // To get the index value for accesing the lookup table
      adf_freq = look_up_table[x] ;  
      // adf_freq=0x3371DC60;                  				
       //freq is in Hz...refer to pg. 430 of the 2020 std
       m_tx_buf[0] = 0x38;                                  // Command to write the channel frequency in the ADF Register (Lower byte first)                     
       m_tx_buf[1] = 0x20;                     //address of the frequency register in 7030 0x200002EC,
       m_tx_buf[2] = 0x00;
       m_tx_buf[3] = 0x02;
       m_tx_buf[4] = 0xEC;
       m_tx_buf[5] = (adf_freq >> 24);
       m_tx_buf[6] = (adf_freq >> 16);
       m_tx_buf[7] = (adf_freq >> 8);
       m_tx_buf[8] = adf_freq;
			
       nrf_drv_spi_transfer(&spi,m_tx_buf,9,m_rx_buf,8);
       nrf_delay_us(250); //check this   
              printf("The contents of m_rx_buf are");   
       for(int i =0;i<=8;i++)
          printf("m_rx_buf[%d] = %x\t",i,m_rx_buf[i]);  
}

static void debug_tx (void)
{
  nrf_gpio_pin_toggle(13);                                            // to debug using oscilloscope
}


void start_get_timer_tx(void)
{
  NRF_TIMER3->MODE = TIMER_MODE_MODE_Timer;                           // Set the clock in Timer Mode
  NRF_TIMER3->TASKS_CLEAR = 1;                                        // Clear timer to 0
  NRF_TIMER3->BITMODE = TIMER_BITMODE_BITMODE_32Bit;                  // Set counter to 32 bit resolution
  NRF_TIMER3->PRESCALER=4;                                            // Set prescaler for 1us resolution
  NRF_TIMER3->TASKS_START = 1;                                        // Start timer
}

uint32_t get_time_tx(void)
{
  NRF_TIMER3->TASKS_CAPTURE[2]=1;                                      // Capture time when called
  curr_time=NRF_TIMER3->CC[2];                                         // Updating curr_time variable
  printf(" %d \t",curr_time-prev_time);                                    // Printing time wrt prev_time
  uint32_t temp = prev_time;
  prev_time=curr_time;                                                 // Updating prev_time
  return (prev_time-temp);
}

void start_timer_tx(void)
{
  NRF_TIMER2->MODE = TIMER_MODE_MODE_Timer;                                                 // Set the clock in Timer Mode
  NRF_TIMER2->TASKS_CLEAR = 1;                                                              // clear timer to 0
  NRF_TIMER2->BITMODE = TIMER_BITMODE_BITMODE_32Bit;                                        // Set counter to 32 bit resolution
  NRF_TIMER2->PRESCALER=4;                                                                  // set prescaler for 1us resolution
  NRF_TIMER2->INTENSET = (TIMER_INTENSET_COMPARE0_Enabled << TIMER_INTENSET_COMPARE0_Pos);  // enabling interrupt for compare registers
  NVIC_EnableIRQ(TIMER2_IRQn);                                                              // enabling interrupt for timer
}


void display(uint32_t total)
{ 
  printf("****************************************\n");
  printf("********** %d **************************",total/9999);
}

void timeslot(void)
{   
        lfclk_request();                                 // Intializing the low frequency clock 
        app_timer_init();
        lfclk_config();  
        create_timers();                                 // Initializing the Timmer                            
        printf("Initializing counter lfclk \n");

    nrf_delay_us(5000);
    get_time_tx();
    read_MISC_FW_reg();
    config_all();
    set_GENERIC_PKT_FRAME_CFG5_reg();// to set the PHR value (length) 
    set_GENERIC_PKT_FRAME_CFG1_reg();//setting interrupt IRQ0 to full packet received interrupt , refer pg 72 of software manual
    write_address(0x20000394,0x06000000);//mapping pin no 3 to IRQ0 , refer pg no 67 of software manual
    write_address(0x20000304,0xC835300C);//changed tranmit power to 3 dBm
    config_change_150kbps();
    State();
    FSM(CFG_DEV);
    printf("After config_dev");
    State();   


    //nrf_delay_ms(1000);// this was commented
    //FSM(PHY_ON);
    for(ASN_count=0;ASN_count<100000;ASN_count+=1)
    {     
    
      app_timer_start(time_out_operation, APP_TIMER_TICKS_US(30000), NULL);
      ASN+=1; // Incrementing the Absolute Slot Number
            printf("#%ld#",ASN);
            get_time_tx();
            State();
      //if(ASN%number_of_channels==2)
      //  state=0;
      //else
      //    state=2;
      //else if(ASN%number_of_channels==0)
      //    state=2;
      //if(state==0)
      //{
      //  Enhanced_Beacon();
      //} 
      //else if(state==2)//rx state
      //{
      //  timeslot_rx();
      //}
      //else if (state==1)
      //{ 
      //    timeslot_tx();    
      //} 
      
      //else
      //{
      //  printf("State not defined \n");
      //} 
      //if(ASN%4==0)
        timeslot_rx();
      //else if(ASN%4==1)
      //  timeslot_tx();
      //else if(ASN%4==2)
      //  broadcast_EB();
       //get_time_tx(); 
   
      if (stop == 1)                                       // Checking if the Tx timeslot has exceeded the timeslot period
      {
          stop=0;
          continue;
      }

      else
      {
        // app_timer_stop(time_out_operation);
         // printf("\nTimer Stopped Explicitly \n");
         while (!stop)
           { 
             ctr_dn+=1;
             nrf_delay_us(5); 
           }
         printf("\n");
         stop=0;
      }

      get_time_tx();                         
      
    }
      
 // NVIC_SystemReset(); 
}

//void Enhanced_Beacon()
//{
  
//}

//void timeslot_rx()
//{

    
  
//}

//void timeslot_tx()
//{          
    
      
//}

void broadcast_EB()
{
  FSM(PHY_ON);
  static_channel_write(2); // check where to call this function, here or in void enhanced_beacon(as in 7242)
  write_EB(ASN);
  //write_Packet(ASN);
  FSM(PHY_TX);
  State();
  nrf_delay_us(13000);
  State();
  FSM(PHY_ON);

}

void receive_EB()
{
  FSM(PHY_ON); 
 static_channel_write(2);
  FSM(PHY_RX);
  State();
  nrf_delay_us(13000);
  read_EB();
  //read_Packet();
    FSM(PHY_ON);
}

void timeslot_rx()
{
  FSM(PHY_ON);
  channel_write(ASN);//added here,check where to call this function
   nrf_delay_us(250);
   FSM(PHY_RX);
   nrf_delay_us(12000);
   read_Packet();
   read_address(0x20000538);
   //for(int i=7;i<11;i++)
   //   printf("The rssi bits are 26 to 16 of m_rx_buff[%d]: %x \n",i, m_rx_buf[i]);
  //printf("The rssi bits of 26 to 16 are : %x   %x \n", m_rx_buf[7]&0x07,m_rx_buf[8]);
printf("the value of rssi before 2's complement in hexa is %x\n",(m_rx_buf[7]&0x07)<<8|m_rx_buf[8]);
  rssi = (0x000007FF-((m_rx_buf[7]&0x07)<<8|m_rx_buf[8]))+0x00000001;
  printf("the value of rssi after 2's complement in hexa is %x\n",rssi);
  printf("the value of rssi after 2's complement in decimal is %d\n",rssi);
  rssi_decimal = rssi*0.25;
  printf("The rssi value after multiplying 0.25 is -%d dBm\n",rssi_decimal);
  // printf("The reg including rssi bits of 31 to 0 are : %x  %x  %x  %x \n", m_rx_buf[7],m_rx_buf[8],m_rx_buf[9],m_rx_buf[10]);
   write_Ack(ASN);
   nrf_delay_us(250);
   get_time_tx();
   FSM(PHY_TX);
   nrf_delay_us(10000);//change in pan coordinator also
   FSM(PHY_ON);

}

void timeslot_tx()
{          
   FSM(PHY_ON);
   channel_write(ASN);//added here,check where to call this function
   write_Packet(ASN);//change phr/
   FSM(CCA);
   nrf_delay_us(1000);
   nrf_delay_us(10000);
   get_time_tx();
   FSM(PHY_RX);
   nrf_delay_us(12000);
   read_Ack();
   read_address(0x20000538);
   //for(int i=7;i<11;i++)
   //   printf("The rssi bits are 26 to 16 of m_rx_buff[%d]: %x \n",i, m_rx_buf[i]);
 printf("the value of rssi before 2's complement in hexa is %x\n",(m_rx_buf[7]&0x07)<<8|m_rx_buf[8]);
  rssi = (0x000007FF-((m_rx_buf[7]&0x07)<<8|m_rx_buf[8]))+0x00000001;
  printf("the value of rssi after 2's complement in hexa is %x\n",rssi);
  printf("the value of rssi after 2's complement in decimal is %d\n",rssi);
  rssi_decimal = rssi*0.25;
  printf("The rssi value after multiplying 0.25 is -%d dBm\n",rssi_decimal);
 //printf("The rssi bits of 26 to 16 are : %x   %x \n", m_rx_buf[7]&0x07,m_rx_buf[8]);
 //  printf("The reg including rssi bits of 31 to 0 are : %x  %x  %x  %x \n", m_rx_buf[7],m_rx_buf[8],m_rx_buf[9],m_rx_buf[10]);
   FSM(PHY_ON);     
}
void ADF7030InterruptHandler()
{
   clear_interrupts();
   get_time_tx();
   printf("******************Interrupt from ADF*******************");
   State();
   get_time_tx();
   nrf_delay_us(50);

}
void start_gpiote_tx(void)
{
  /*gpiotehttps://devzone.nordicsemi.com/f/nordic-q-a/46731/example-using-gpiote-and-gpio-modules-for-future-newbs-like-me*/
  ret_code_t err_code; //The result from the config functions
  //ret_code_t err_code_1;

  //Check if the GPIOTE is initialized
  if (nrfx_gpiote_is_init() == NULL) {
    //If its not initialized, init it now
    err_code = nrfx_gpiote_init(); //Init the GPIO interrupt and tasks
    APP_ERROR_CHECK(err_code); //Check the error code
    //(0x8 means the GPIOTE module is already enabled)
  }

  //Set the input configuration to interrupt on status change
  nrfx_gpiote_in_config_t inConfig = NRFX_GPIOTE_RAW_CONFIG_IN_SENSE_LOTOHI(true);
  inConfig.pull                    = NRF_GPIO_PIN_PULLDOWN;


  //When the interrupt occurs in pin "BUTTON_x", configured as "inConfig", the interrupt
  //will execute the function "ADFInterruptHandler"
  err_code = nrfx_gpiote_in_init(interrupt_from_adf_tx, &inConfig, ADF7030InterruptHandler);
  APP_ERROR_CHECK(err_code);
  nrfx_gpiote_in_event_enable(interrupt_from_adf_tx, true);

}
void config_change_100kbps()
{
  write_address(0x200002F0,0x0004821A);
  write_address(0x200002FC,0x640003E8);
  write_address(0x20000300,0x451A484D);
  write_address(0x2000031C,0x000504B8);
  write_address(0x20000320,0x25A80002);
  write_address(0x20000334,0x40607F02);
  write_address(0x20000340,0x0C010028);
  write_address(0x20000344,0x0200109C);
  write_address(0x20000378,0x06C00A43);
  write_address(0x20000530,0x50000393);
  write_address(0x20000534,0x10000AD5);
  write_address(0x20000628,0x0001001C);
  write_address(0x2000062C,0xE1C0E037);
  write_address(0x20000630,0xC0C0E037);
  write_address(0x20000634,0xE0C0E53C);
  write_address(0x20000638,0xEC140E44);
}
void config_change_150kbps()
{
  write_address(0x200002F0,0x0004F588);
  write_address(0x200002FC,0x640005DC);
  write_address(0x20000300,0x011A5850);
  write_address(0x2000031C,0x00050318);
  write_address(0x20000320,0x25280002);
  write_address(0x20000334,0x20607F02);
  write_address(0x20000340,0x06010028);
  write_address(0x20000344,0x0200109C);
  write_address(0x20000378,0x06C00F43);
  write_address(0x20000530,0xF0000000);
  write_address(0x20000534,0x10000000);
  write_address(0x20000628,0x0001001A);
  write_address(0x2000062C,0xE1C0E032);
  write_address(0x20000630,0xC0C0E032);
  write_address(0x20000634,0xE0C0E536);
  write_address(0x20000638,0xEC140E3E);
}
