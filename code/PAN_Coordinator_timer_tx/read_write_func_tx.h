#ifndef _READ_WRITE_FUNC__TX_H_      // protective header file // format: #ifndef _<FILE_NAME_In_CAPS>_H_
#define _READ_WRITE_FUNC_TX_H_

#include <stdint.h>

 extern uint8_t m_tx_buf[150];
 extern uint8_t m_rx_buf[150];
 extern int delay_write;
 extern int delay_read;
 extern int pkt_rcvd_Check,pkt_sent_Check;

 #define SPI_INSTANCE  0 /**< SPI instance index. */
 extern const nrf_drv_spi_t spi ; /**< SPI instance. */
 void spi_event_handler(nrf_drv_spi_evt_t const * p_event,
                       void *                    p_context);
 extern volatile bool spi_xfer_done;
 extern const uint8_t m_length;
 void spi_init(void);

// void ADF_write(uint16_t address, uint8_t Data);
// void ADF_read(uint16_t address);

// void set_buffer_cfg(uint8_t bcfg);
// void set_pkt_cfg(uint8_t pcfg);

// void set_delaycfg0(uint8_t dcfg0);
// void set_delaycfg2(uint8_t dcfg2);
// void set_cca_threshold(void);
// void set_rssi_avg_time(void);
// void set_cca_type(void);
 
// uint16_t read_rssi_readback(void);

// void set_rc_cfg(void);
// void set_dataRate(void);
// void set_TX_mode_reg(void);
// void set_TX_freqDev(void);


// void channelwrite(uint8_t channel);
// void read_channel_reg(void);

// void packetRead(void);
// int ackRead(long);

// void packetWrite(long asn);
// void packetSent(void);
// void acksSent(int , int , long);
// void packetRead_1(void);
//void Eb_write(long);
// int* EBread(void);

// void read_interrupts_src0(void);
// void read_interrupts_src1(void);
// void interrupts_mask(void);
// void interrupts_mask_rc_ready(void);
// void interrupts_mask_rx_pkt_rcvd(void);
// void interrupts_mask_cca_complete(void);
// void clear_all_interrupts(void);
// void interrupts_mask_sfd_rcvd(void);
// void interrupts_mask_sfd_TX_rcvd(void);


void read_MISC_FW_reg(void);
void set_GENERIC_PKT_FRAME_CFG5_reg(void);
void set_GENERIC_PKT_FRAME_CFG1_reg(void);
void read_address(long);
void write_address(long, long);
void write_Packet(long);
void read_Packet(void);
void write_Ack(long);
void read_Ack(void);
void write_EB(long);
void read_EB(void);
void clear_interrupts(void);

 #endif


