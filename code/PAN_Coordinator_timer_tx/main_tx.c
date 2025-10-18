#include "nrf_drv_spi.h"
#include "nrf.h"
#include "app_util_platform.h"
#include "nrf_delay.h"
#include "boards.h"
#include "app_error.h"
#include "nrfx_timer.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
#include "nrf_drv_clock.h"
#include "nrf_gpio.h"

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#define led NRF_GPIO_PIN_MAP(0,14) // to indicate wether the tx has started based in the input
#define inp NRF_GPIO_PIN_MAP(0,11) // input pin to get the trigger after 1000 counts from the arduino based on the pps given by GPS

/*My libraries*/
#include "reg_cmd_tx.h"
#include "sdk_config_tx.h"
#include "read_write_func_tx.h"
#include "timer_tx_new.h"
#include "FSM_tx.h"
uint16_t counter=0;

int main(void)
{
   
   
    spi_init();             //read_write_func.c : assisgning spi interafce lines to pin numbers
    nrf_gpio_cfg_output(led);
    nrf_gpio_cfg_input(inp,NRF_GPIO_PIN_PULLDOWN);
    nrf_gpio_pin_set(led);
    start_gpiote_tx();
    start_timer_tx();
    start_get_timer_tx(); //initialize the timer 3 for measuring the time and also start the timer to measure the time
    while(1){
    //if(nrf_gpio_pin_read(inp))
    //{   
        nrf_gpio_pin_clear(led); 
        timeslot();         
    //}
    nrf_gpio_pin_set(led);
   nrf_delay_us(5000000);
   NVIC_SystemReset(); 
    }
    bsp_board_init(BSP_INIT_LEDS);
    APP_ERROR_CHECK(NRF_LOG_INIT(NULL));
    NRF_LOG_DEFAULT_BACKENDS_INIT();
}