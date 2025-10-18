#include "nrf_drv_spi.h"
#include "app_util_platform.h"
#include "nrf_gpio.h"
#include "nrf_delay.h"
#include "boards.h"
#include "app_error.h"
#include <string.h>
#include "reg_cmd_tx.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
#include "FSM_tx.h"
#include "read_write_func_tx.h"

 uint8_t m_tx_buf[150];   //TX buffer
 uint8_t m_rx_buf[150];  //RX buffer
 int length = 0;
 int pad_data = 0;
 #define SPI_INSTANCE  0 /**< SPI instance index. */
 const nrf_drv_spi_t spi = NRF_DRV_SPI_INSTANCE(SPI_INSTANCE);  /**< SPI instance. */
 volatile bool spi_xfer_done;
 const uint8_t m_length = sizeof(m_tx_buf);
//IEs
//uint64_t rcvd_asn=0;
//uint64_t rasn[5];


 //IE variables
int i_EB=28;//initialize i to index of hte1+1 index for IE
bool type;//type of IE 0/1
int len_IE;//Length of IE
int IE_ID;//ID of the IE
int j=0;  // to access each nested IE
int k; // to access index of IE array

 int channel_id=0;    //channel id is extracted to find which link descriptor we need to access 
 int num_of_channels=0;// to changes asn%n -->n=numbeer of channel
 int timeslot_numb[4]={0,0,0,0};
 int timeslot_len=0;
 int channel_offset[4]={0,0,0,0};
 int Link_option[4]={0,0,0,0};
 int IE[11]={0,0,0,0,0,0,0,0,0,0,0}; //array to return IEs to called function, increase the index if more IE are needed to be returned
//End of IE variables
//TX variables
//End of IE declaration
 //int channel;
 int i;
 //extern int pkt_sent_Check;
 //extern int pkt_rcvd_Check;

 //RX Variables
//int random=3;       //for network creation changing initialization of random 13-04-2022
//int loss_rx=0;
//int curr_asn_rx=0;
//int prev_asn_rx=0;

// int time_tx=0;
//uint64_t asn_tx=0;
//uint64_t prev_asn_tx=0;
//int loss_tx=0;
//int previous_time_ack[4] = {0,0,0,0};
//long ASN_in_ack=0;
//End of Tx variables
void spi_event_handler(nrf_drv_spi_evt_t const * p_event,
                       void *                    p_context)
{
    spi_xfer_done = true;
    NRF_LOG_INFO("Transfer completed.");
    if (m_rx_buf[0] != 0)
    {
        NRF_LOG_INFO(" Received:");
        NRF_LOG_HEXDUMP_INFO(m_rx_buf, 150);
    }
}

void spi_init(void)
{

    nrf_drv_spi_config_t spi_config = NRF_DRV_SPI_DEFAULT_CONFIG;
    spi_config.ss_pin   = SPI_SS_PIN;//pin 29
    spi_config.miso_pin = SPI_MISO_PIN;// pin 28
    spi_config.mosi_pin = SPI_MOSI_PIN;// pin 4
    spi_config.sck_pin  = SPI_SCK_PIN;// pin 3
    APP_ERROR_CHECK(nrf_drv_spi_init(&spi, &spi_config, spi_event_handler, NULL));
    memset(m_rx_buf, 0, m_length);
    spi_xfer_done = false;

}

//void Eb_write(long asn)
//{
//	m_tx_buf[0] = SPI_PKT_WR; // SPI_PKT_WR command to write to TX_BUFFER
//  //Note the PHR field is written in Decimal
//	m_tx_buf[1] = 72; // PHR --> PHY header field ( Total number of bytes written to TX_BUFFER )
//	m_tx_buf[2] = 0x22; // FCF --> Frame control field [15:8] MSB 
//	m_tx_buf[3] = 0x00; // FCF --> Frame control field [7:0] LSB
//	m_tx_buf[4] = 0x1C; // SEQ NUM -->Sequence number
//	m_tx_buf[5] = 0x22; // Dest PAN Identifier --> [15:8] MSB
//	m_tx_buf[6] = 0x22; // Dest PAN Identifier --> [7:0] MSB
//	m_tx_buf[7] = 0x00; // Dest Address --> [63:56]
//	m_tx_buf[8] = 0x00; // Dest Address --> [55:48]
//	m_tx_buf[9] = 0x00; // Dest Address --> [47:40]
//	m_tx_buf[10] = 0x00; // Dest Address --> [39:32]
//	m_tx_buf[11] = 0x00; // Dest Address --> [31:24]
//	m_tx_buf[12] = 0x00; // Dest Address --> [23:16]
//	m_tx_buf[13] = 0x22; // Dest Address --> [15:8]
//	m_tx_buf[14] = 0x22; // Dest Address --> [7:0]
//	m_tx_buf[15] = 0x11; // Source PAN Identifier --> [15:8] MSB
//	m_tx_buf[16] = 0x11; // Source PAN Identifier --> [15:8] MSB
//	m_tx_buf[17] = 0x00; // Source Address --> [63:56]
//	m_tx_buf[18] = 0x00; // Source Address --> [55:48]
//	m_tx_buf[19] = 0x00; // Source Address --> [47:40]
//	m_tx_buf[20] = 0x00; // Source Address --> [39:32]
//	m_tx_buf[21] = 0x00; // Source Address --> [31:24]
//	m_tx_buf[22] = 0x00; // Source Address --> [23:16]
//	m_tx_buf[23] = 0x11; // Source Address --> [15:8]
//	m_tx_buf[24] = 0x11; // Source Address --> [7:0]	
//        //Header Termination IE HTE1 Length=0, Element ID=7E, Type=0
//        m_tx_buf[25] = 0x1F; 
//        m_tx_buf[26] = 0x80;
//        //For formats of IE  refer page 198 of the standard
//        //TSCH Sync IEs  refer Pg no. 201
//        m_tx_buf[27] = 0x06;//Length of IE in octects -->(5 bytes asn, 1 byte Join metric)
//        m_tx_buf[28] = 0x1A;//0x1A= 0001 1010 ---> Type=0,Sub Id=0x1A 
//        m_tx_buf[29] = asn;  //ASN 1st byte 
//        m_tx_buf[30] = asn>>8; //ASN 2nd byte
//        m_tx_buf[31] = asn>>16; //ASN 3rd byte
//        m_tx_buf[32] = asn>>24; //ASN 4th byte
//        m_tx_buf[33] = asn>>32; //ASN 5th byte
//        m_tx_buf[34] = 0x01;  // Join metric
//        //End pf TSCH Sync IEs
//        //Channel hopping IE page 233
//        m_tx_buf[35] = 0x0A; // Length of IE
//        m_tx_buf[36] = 0xC8; // Type and Sub ID 
//        m_tx_buf[37] = 0x0A; // Hopping sequence ID
//        m_tx_buf[38] = 0x03; // channel sequence ID this should be the channel value that the node should operate and this valure should be set in real time by mac
//        m_tx_buf[39] = 0x04; // Number of channels
//        m_tx_buf[40] = 0x0F; //PHY Configuration
//        //MAC extended bit map not present
//        m_tx_buf[41] = 0x04;// Hopping sequence length
//        //Refer pg 235 of the standard
//        m_tx_buf[42] = 0x17; //channel hopping number1 in hex
//        m_tx_buf[43] = 0x0F; //channel hopping number2 in hex
//        m_tx_buf[44] = 0x14; //channel hopping number3 in hex
//        m_tx_buf[45] = 0x0B; //channel hopping number4 
//        m_tx_buf[46] = 0x00; // current hopping index 
//        //End of Channel Hopping IEs 
//        //TSCH Timeslot IE Refer Pg no 203 
//        m_tx_buf[47] = 0x02;// Length of IE
//        m_tx_buf[48] = 0x1C;//Type and Sub ID
//        m_tx_buf[49] = 0x00;//Timeslot ID It follows default timming of the hardware hence we are mentioning only timeslot length
//        m_tx_buf[50] = 0x0A;//Timeslot Length
//        //End of TSCH Timeslot IE
//        //TSCH Slot Frame and Link IE Refer Pg no. 201
//        m_tx_buf[51] = 0x10;//Legth of IE
//        m_tx_buf[52] = 0x1B;//Type and Sub ID  
//        m_tx_buf[53] = 0x01;// Number of slotframe
//        //Start of slot frame descriptor 1
//        m_tx_buf[54] = 0x01;// Slotframe handler
//        m_tx_buf[55] = 0x04;// Slot frame size
//        m_tx_buf[56] = 0x04; //Number of Links
//        // Link1 Information 
//        m_tx_buf[57] = 0x00; // Timeslot Number
//        m_tx_buf[58] = 0x00; // Channel Offset
//        m_tx_buf[59] = 0x10; // Link options is priority
//        // Link2 Information 
//        m_tx_buf[60] = 0x01; // Timeslot Number
//        m_tx_buf[61] = 0x01; // Channel Offset
//        m_tx_buf[62] = 0x02; // Link options Rx link
//        // Link3 Information 
//        m_tx_buf[63] = 0x02; // Timeslot Number
//        m_tx_buf[64] = 0x02; // Channel Offset
//        m_tx_buf[65] = 0x02; // Link options Rx link
//        // Link4 Information 
//        m_tx_buf[66] = 0x03; // Timeslot Number
//        m_tx_buf[67] = 0x03; // Channel Offset
//        m_tx_buf[68] = 0x02; // Link options RX link 
//        //End of Slot frame descriptor 1
//        //End of TSCH slotframe IE
//        //Payload TerminationIE Length=0, Element ID=0xF, Type=1
//        m_tx_buf[69] = 0x00;
//        m_tx_buf[70] = 0x7C;

////Note if you change the frame payload length, update the PHR Field
//	nrf_drv_spi_transfer(&spi,m_tx_buf,71,m_rx_buf,71);
//        nrf_delay_us(1500);//got till 56th char for 1200
//       // printf("EBwrite :\n");
//        ASN=ASN+1;
//}
//void packetRead_1(void)
//{       
//	m_tx_buf[0] = SPI_PKT_RD;
//	m_tx_buf[1] = SPI_NOP;
//        m_tx_buf[2] = SPI_NOP;
//        nrf_drv_spi_transfer(&spi,m_tx_buf,3,m_rx_buf,74);
//        nrf_delay_us(1000);
        
//        for (i =25;i<=35;i++)
//        printf("%c ",m_rx_buf[i]);
//        printf(" %d *", m_rx_buf[i]);
//        prev_asn_rx=curr_asn_rx;
//        curr_asn_rx=m_rx_buf[i];
//        if(prev_asn_rx==curr_asn_rx)
//              loss_rx++;
//        else
//            loss_rx=0;
//        if(loss_rx==10)    //Lost 10 packets?
//        {
//              loss_rx=0;
//              curr_asn_rx=0;
//              prev_asn_rx=0;
//              //NVIC_SystemReset();             // Reseting after loosing 10 packets
//        } 
        
//}

//int* EBread(void)
//{       
//	m_tx_buf[0] = SPI_PKT_RD;
//	m_tx_buf[1] = SPI_NOP;
//        m_tx_buf[2] = SPI_NOP;
        
//        nrf_drv_spi_transfer(&spi,m_tx_buf,3,m_rx_buf,72);
//        nrf_delay_us(1000);
//        printf("EBRead :\n");
//        printf("#\n");

//        i_EB=28;
//        do
//        { 
//          i_EB++;
//          type=m_rx_buf[i_EB]&0x80;
//          if(type)//type 1
//          {
//            IE_ID=(m_rx_buf[i_EB]&0x78)>>3;
//            len_IE=m_rx_buf[i_EB-1]|((m_rx_buf[i_EB]&0x07)<<8); //length of type 1 IE is 10 bits
//          }
//           else // type 0 
//           {
//           IE_ID=m_rx_buf[i_EB];
//           len_IE=m_rx_buf[i_EB-1];  // length of type 0 IE is 8 bits
//           }
//           if(IE_ID==0x7C)
//            break;
//           printf("IE ID: %d\n",IE_ID);
//           printf("IE Length: %d\n",len_IE);
//           j=i_EB+1;
//           printf("i=%d \t j=%d\n",i_EB,j);
//          switch(IE_ID)
//          {  
//              case 0x1A:{//time sync IE
//                        printf("Time sync IE");
//                        for (k=0;k<5;k++,j++) //changing to print asn 
//                        {
//                            printf("#%d ",m_rx_buf[j]);// index is j 
//                            IE[k]=m_rx_buf[j];
//                        }
//                        break;
//                        }
//              case 0x09:{//channel Hopping IE
//                        printf("Channel hopping IE");
//                        //j=i+1--> 1st byte of Channel Hopping IE ie hopping sequence IE
//                        channel_id=m_rx_buf[++j];//channel sequence ID this should be the channel value that the node should operate and this valure should be set in real time by mac
//                        IE[k++]=channel_id;
//                        num_of_channels=m_rx_buf[++j];//Number of channel
//                        IE[k++]=num_of_channels;
//                        break;
//                        }
//              case 0x1C:  {//TSCH timeslot IE
//                        printf("TSCH timeslot IE");
//                        timeslot_len=m_rx_buf[++j];
//                        IE[k++]=timeslot_len;
//                        break;
//                        }
//              case 0x1B:{//TSCH slotframe IE
//                        printf("TSCH slotframe IE");
//                       // for(k=0;k<m_rx_buf[j];k++)
//                       // { 
//                          //slot_frame_handler=m_rx_buf[++j];
//                          //slot_frame_size=m_rx_buf[++j];
//                          //number_of_links=m_rx_buf[++j];
//                          j=j+(channel_id*3)+4;   //+1 to go to next index in array +3 to skip over slotfram_handler
//                          timeslot_numb=m_rx_buf[j];
//                          IE[k++]=timeslot_numb;
//                          channel_offset=m_rx_buf[++j];
//                          IE[k++]=channel_offset;
//                          Link_option=m_rx_buf[++j];
//                          IE[k++]=Link_option;
                            
//                        //}
//                        break;
//                        }
//              default : break;
//          }
//         i_EB=i_EB+len_IE+1;
//        }
//        while(i_EB<=72);
//       // printf("channel_id : %d\nnum_of_channel : %d\ntimeslot_numb : %d\nchannel_offset : %d\nLink_option : %d\nTimeslot_length : %d\n", channel_id, num_of_channels, timeslot_numb, channel_offset, Link_option,timeslot_len);
//      return IE;
//}



//void packetWrite(long asn)
//{
//	m_tx_buf[0] = 0x10; // SPI_PKT_WR command to write to TX_BUFFER
//  //Note the PHR field is written in Decimal
//	m_tx_buf[1] = 72; // PHR --> PHY header field ( Total number of bytes written to TX_BUFFER )
//	m_tx_buf[2] = 0xDC; // FCF --> Frame control field [15:8] MSB
//	m_tx_buf[3] = 0x51; // FCF --> Frame control field [7:0] LSB
//	m_tx_buf[4] = 0x1C; // SEQ NUM -->Sequence number
//	m_tx_buf[5] = 0x22; // Dest PAN Identifier --> [15:8] MSB
//	m_tx_buf[6] = 0x22; // Dest PAN Identifier --> [7:0] MSB
//	m_tx_buf[7] = 0x00; // Dest Address --> [63:56]
//	m_tx_buf[8] = 0x00; // Dest Address --> [55:48]
//	m_tx_buf[9] = 0x00; // Dest Address --> [47:40]
//	m_tx_buf[10] = 0x00; // Dest Address --> [39:32]
//	m_tx_buf[11] = 0x00; // Dest Address --> [31:24]
//	m_tx_buf[12] = 0x00; // Dest Address --> [23:16]
//	m_tx_buf[13] = 0x22; // Dest Address --> [15:8]
//	m_tx_buf[14] = 0x22; // Dest Address --> [7:0]
//	m_tx_buf[15] = 0x11; // Source PAN Identifier --> [15:8] MSB
//	m_tx_buf[16] = 0x11; // Source PAN Identifier --> [15:8] MSB
//	m_tx_buf[17] = 0x00; // Source Address --> [63:56]
//	m_tx_buf[18] = 0x00; // Source Address --> [55:48]
//	m_tx_buf[19] = 0x00; // Source Address --> [47:40]
//	m_tx_buf[20] = 0x00; // Source Address --> [39:32]
//	m_tx_buf[21] = 0x00; // Source Address --> [31:24]
//	m_tx_buf[22] = 0x00; // Source Address --> [23:16]
//	m_tx_buf[23] = 0x11; // Source Address --> [15:8]
//	m_tx_buf[24] = 0x11; // Source Address --> [7:0]	
//	m_tx_buf[25] = 'H'; // Frame Payload
//	m_tx_buf[26] = 'E'; // Frame Payload
//	m_tx_buf[27] = 'l'; // Frame Payload
//	m_tx_buf[28] = 'l'; // Frame Payload
//	m_tx_buf[29] = 'o'; // Frame Payload
//	m_tx_buf[30] = 'W'; // Frame Payload
//	m_tx_buf[31] = 'o'; // Frame Payload
//	m_tx_buf[32] = 'r'; // Frame Payload
//	m_tx_buf[33] = 'l'; // Frame Payload
//	m_tx_buf[34] = 'd'; // Frame Payload
//	m_tx_buf[35] = asn;
//         m_tx_buf[36] = 0xC8; // Type and Sub ID 
//        m_tx_buf[37] = 0x0A; // Hopping sequence ID
//        m_tx_buf[38] = 0x03; // channel sequence ID this should be the channel value that the node should operate and this valure should be set in real time by mac
//        m_tx_buf[39] = 0x04; // Number of channels
//        m_tx_buf[40] = 0x0F; //PHY Configuration
//        //MAC extended bit map not present
//        m_tx_buf[41] = 0x04;// Hopping sequence length
//        //Refer pg 235 of the standard
//        m_tx_buf[42] = 0x17; //channel hopping number1 in hex
//        m_tx_buf[43] = 0x0F; //channel hopping number2 in hex
//        m_tx_buf[44] = 0x14; //channel hopping number3 in hex
//        m_tx_buf[45] = 0x0B; //channel hopping number4 
//        m_tx_buf[46] = 0x00; // current hopping index 
//        //End of Channel Hopping IEs 
//        //TSCH Timeslot IE Refer Pg no 203 
//        m_tx_buf[47] = 0x02;// Length of IE
//        m_tx_buf[48] = 0x1C;//Type and Sub ID
//        m_tx_buf[49] = 0x00;//Timeslot ID It follows default timming of the hardware hence we are mentioning only timeslot length
//        m_tx_buf[50] = 0x14;//Timeslot Length
//        //End of TSCH Timeslot IE
//        //TSCH Slot Frame and Link IE Refer Pg no. 201
//        m_tx_buf[51] = 0x10;//Legth of IE
//        m_tx_buf[52] = 0x1B;//Type and Sub ID  
//        m_tx_buf[53] = 0x01;// Number of slotframe
//        //Start of slot frame descriptor 1
//        m_tx_buf[54] = 0x01;// Slotframe handler
//        m_tx_buf[55] = 0x04;// Slot frame size
//        m_tx_buf[56] = 0x04; //Number of Links
//        // Link1 Information 
//        m_tx_buf[57] = 0x00; // Timeslot Number
//        m_tx_buf[58] = 0x00; // Channel Offset
//        m_tx_buf[59] = 0x10; // Link options is priority
//        // Link2 Information 
//        m_tx_buf[60] = 0x01; // Timeslot Number
//        m_tx_buf[61] = 0x01; // Channel Offset
//        m_tx_buf[62] = 0x02; // Link options Rx link
//        // Link3 Information 
//        m_tx_buf[63] = 0x02; // Timeslot Number
//        m_tx_buf[64] = 0x02; // Channel Offset
//        m_tx_buf[65] = 0x02; // Link options Rx link
//        // Link4 Information 
//        m_tx_buf[66] = 0x03; // Timeslot Number
//        m_tx_buf[67] = 0x03; // Channel Offset
//        m_tx_buf[68] = 0x02; // Link options RX link 
//        //End of Slot frame descriptor 1
//        //End of TSCH slotframe IE
//        //Payload TerminationIE Length=0, Element ID=0xF, Type=1
//        m_tx_buf[69] = 0x00;
//        m_tx_buf[70] = 0x7C;

////Note if you change the frame payload length, update the PHR Field 
	
//	for (i =25;i<=34;i++)
//         printf("%c ",m_tx_buf[i]);
//        printf("%d *",m_tx_buf[i]); //commented on 2-12-21 to check for cca
        
//	nrf_drv_spi_transfer(&spi,m_tx_buf,71,m_rx_buf,11);
//        nrf_delay_us(1000);//got till 56th char for 1200
//        ASN=ASN+1;
//}
//int ackRead(long ASN_rcvd)
//{
//	m_tx_buf[0] = SPI_PKT_RD;
//	m_tx_buf[1] = SPI_NOP;
//        m_tx_buf[2] = SPI_NOP;
//        m_tx_buf[3] = SPI_NOP;
//       //nrf_delay_us(2000);//ack delay added
//	nrf_drv_spi_transfer(&spi,m_tx_buf,4,m_rx_buf,74);
//	//nrf_delay_us(1000);// nrf_delay_us(10000);       //this delay affects the accuracy of packets rcvd - need to optimize 10000 us delay
//        nrf_delay_us(1000);//added on 28-10-21
       
 
//       for (i =32;i<=34;i++)
//            printf("%c ",m_rx_buf[i]); //commented 24 sept
//       // for(;i<=39;i++)
//        // printf("#%d#",m_rx_buf[i]);
         
//        ASN_in_ack=(m_rx_buf[i])+(m_rx_buf[i+1])*256+(m_rx_buf[i+2])*256*256+((m_rx_buf[i+3])*256*256*256)+((m_rx_buf[i+4])*256*256*256*256);
//        printf("#%ld#",ASN_in_ack);
//        prev_asn_tx = asn_tx;
//        asn_tx=m_rx_buf[i];
//       // asn_tx=(m_rx_buf[i]|(m_rx_buf[++i]<<8)|(m_rx_buf[++i]<<16)|(m_rx_buf[++i]<<24)|(m_rx_buf[++i]<<32));
  
//        if (prev_asn_tx==asn_tx)
//          loss_tx+=1;
        
//        else
//          loss_tx=0;
//        if(loss_tx==10)
//        {
//           prev_asn_tx=0;
//           asn_tx=0;
//           loss_tx=0;
//           //NVIC_SystemReset();
//        }
//        i=39;
//        time_tx=(m_rx_buf[++i]|(m_rx_buf[++i]<<8)|(m_rx_buf[++i]<<16)|(m_rx_buf[++i]<<24));
//        printf("   |%d  ",time_tx);
//        printf("   |%d  ",m_rx_buf[++i]);
//        if((m_rx_buf[i]==1)&&(time_tx<=3000)) //1 indicates Tx is faster than Rx
//        { previous_time_ack[ASN_rcvd%3] = time_tx;      //For network creation done on 13-04-2022
//          return time_tx;
//        }
//         else if((m_rx_buf[i]==2)&&(time_tx<=3000)) // indicates Tx is slower than Rx and offset value is negative
//             return 0;
//         else
//              return previous_time_ack[ASN_rcvd%3];      //For network creation done on 13-04-2022  //indiactes Tx is fater than Rx and offset value is negative hence maintain the previous delay
       

//}

//void  acksSent(int time_rx, int en, long asn)
//{       random=asn;
//	m_tx_buf[0] = SPI_PKT_WR; // SPI_PKT_WR command to write to TX_BUFFER
//  //Note the PHR field is written in Decimal
//	m_tx_buf[1] = 72; // PHR --> PHY header field ( Total number of bytes written to TX_BUFFER )
//	m_tx_buf[2] = 0x2E; // FCF --> Frame control field [15:8] MSB
//	m_tx_buf[3] = 0x3A; // FCF --> Frame control field [7:0] LSB
//	m_tx_buf[4] = 0x1C; // SEQ NUM -->Sequence number
//	m_tx_buf[5] = 0x22; // Dest PAN Identifier --> [15:8] MSB
//	m_tx_buf[6] = 0x22; // Dest PAN Identifier --> [7:0] MSB
//	m_tx_buf[7] = 0x00; // Dest Address --> [63:56]
//        m_tx_buf[8] = 0x00; // Dest Address --> [55:48]
//	m_tx_buf[9] = 0x00; // Dest Address --> [47:40]
//	m_tx_buf[10] = 0x00; // Dest Address --> [39:32]
//	m_tx_buf[11] = 0x00; // Dest Address --> [31:24]
//	m_tx_buf[12] = 0x00; // Dest Address --> [23:16]
//	m_tx_buf[13] = 0x22; // Dest Address --> [15:8]
//	m_tx_buf[14] = 0x22; // Dest Address --> [7:0]
//	m_tx_buf[15] = 0x11; // Source PAN Identifier --> [15:8] MSB
//	m_tx_buf[16] = 0x11; // Source PAN Identifier --> [15:8] MSB
//	m_tx_buf[17] = 0x00; // Source Address --> [63:56]
//	m_tx_buf[18] = 0x00; // Source Address --> [55:48]
//	m_tx_buf[19] = 0x00; // Source Address --> [47:40]
//	m_tx_buf[20] = 0x00; // Source Address --> [39:32]
//	m_tx_buf[21] = 0x00; // Source Address --> [31:24]
//	m_tx_buf[22] = 0x00; // Source Address --> [23:16]
//	m_tx_buf[23] = 0x11; // Source Address --> [15:8]
//	m_tx_buf[24] = 0x11; // Source Address --> [7:0]
//        m_tx_buf[25] = 0x6D; // auxframe
//	m_tx_buf[26] = 0x00;//IDX; // Frame Payload
//	m_tx_buf[27] = 0x02; // IE Header
//	m_tx_buf[28] = 0x0F; // IE Header
//	m_tx_buf[29] = 0x00;//ts1; // Payload IE
//	m_tx_buf[30] = 0x00;//ts2; // Payload IE
//        m_tx_buf[31] = 'A';
//        m_tx_buf[32] = 'C';
//        m_tx_buf[33] = 'K';
//        m_tx_buf[34] = random;                   //m_tx_buf[34] = random;
//        m_tx_buf[35] = random>>8;
//        m_tx_buf[36] = random>>16;
//        m_tx_buf[37] = random>>24;
//        m_tx_buf[38] = random>>32;
//        m_tx_buf[39] = time_rx;
//        m_tx_buf[40] = time_rx >>8;
//        m_tx_buf[41] = time_rx >>16;
//        m_tx_buf[42] = time_rx>>24;
//        m_tx_buf[43] = en;
//        m_tx_buf[44] = 0x14; //channel hopping number3 in hex
//        m_tx_buf[45] = 0x0B; //channel hopping number4 
//        m_tx_buf[46] = 0x00; // current hopping index 
//        //End of Channel Hopping IEs 
//        //TSCH Timeslot IE Refer Pg no 203 
//        m_tx_buf[47] = 0x02;// Length of IE
//        m_tx_buf[48] = 0x1C;//Type and Sub ID
//        m_tx_buf[49] = 0x00;//Timeslot ID It follows default timming of the hardware hence we are mentioning only timeslot length
//        m_tx_buf[50] = 0x14;//Timeslot Length
//        //End of TSCH Timeslot IE
//        //TSCH Slot Frame and Link IE Refer Pg no. 201
//        m_tx_buf[51] = 0x10;//Legth of IE
//        m_tx_buf[52] = 0x1B;//Type and Sub ID  
//        m_tx_buf[53] = 0x01;// Number of slotframe
//        //Start of slot frame descriptor 1
//        m_tx_buf[54] = 0x01;// Slotframe handler
//        m_tx_buf[55] = 0x04;// Slot frame size
//        m_tx_buf[56] = 0x04; //Number of Links
//        // Link1 Information 
//        m_tx_buf[57] = 0x00; // Timeslot Number
//        m_tx_buf[58] = 0x00; // Channel Offset
//        m_tx_buf[59] = 0x10; // Link options is priority
//        // Link2 Information 
//        m_tx_buf[60] = 0x01; // Timeslot Number
//        m_tx_buf[61] = 0x01; // Channel Offset
//        m_tx_buf[62] = 0x02; // Link options Rx link
//        // Link3 Information 
//        m_tx_buf[63] = 0x02; // Timeslot Number
//        m_tx_buf[64] = 0x02; // Channel Offset
//        m_tx_buf[65] = 0x02; // Link options Rx link
//        // Link4 Information 
//        m_tx_buf[66] = 0x03; // Timeslot Number
//        m_tx_buf[67] = 0x03; // Channel Offset
//        m_tx_buf[68] = 0x02; // Link options RX link 
//        //End of Slot frame descriptor 1
//        //End of TSCH slotframe IE
//        //Payload TerminationIE Length=0, Element ID=0xF, Type=1
//        m_tx_buf[69] = 0x00;
//        m_tx_buf[70] = 0x7C;

////Note if you change the frame payload length, update the PHR Field 
	
	
//	nrf_drv_spi_transfer(&spi,m_tx_buf,71,m_rx_buf,11);
//        nrf_delay_us(1000);//got till 56th char for 1200


//}



//void clear_all_interrupts(void)
//{
      
//}

//FUNCTIONS RELATED TO ADF7030

//Reading MISC_FW register Address of the MISC_FW register 0x400042B4 Pg no 79 of ADF siftware manual
void read_MISC_FW_reg()
{      // mode 1 memory access mode in read mode  (pg no 47 of ADF software manual)
       m_tx_buf[0]=0x78; // CNM=0, RNW=1, BNR=1, ANP=1, LNS=1, MHPTR=000 (dont care) -->01111000
       m_tx_buf[1]=0x40;  // 1st Byte (MSB)
       m_tx_buf[2]=0x00;  //2nd Byte
       m_tx_buf[3]=0x42;  //3rd Byte
       m_tx_buf[4]=0xB4;  //4th Byte (LSB)
       nrf_drv_spi_transfer(&spi,m_tx_buf,5,m_rx_buf,11);
       //Although we are reading 11 bytes 1st 7 bytes will be status bytes next 4 bytes is the data from ADF to host
       nrf_delay_us(100);
       //for(i=7;i<11;i++)
       // printf("m_rx_buff[%d]: %x \n",i, m_rx_buf[i]);
}
//GENERIC PACKET FRAME CONFIGURATION 1 REGISTER Address: 0x20000500, Name: GENERIC_PKT_FRAME_CFG1
//TRX_IRQ1_TYPE=00000000
//TRX_IRQ0_TYPE=00000000
//PREAMBLE_UNIT=1
//PAYLOAD_SIZE-0000000
void set_GENERIC_PKT_FRAME_CFG1_reg()
{
      read_address(0x20000500);
     //Memory acccess mode 1 in write format
      m_tx_buf[0]=0x38;  //Memory access command CNM=0, RNW=0, BNR=1, ANP=1, LNS=1, MHPTR=000 --> 0011 1000
      m_tx_buf[1]=0x20; //1st byte ofAddress of the register
      m_tx_buf[2]=0x00; //2nd byte ofAddress of the register
      m_tx_buf[3]=0x05; //3rd byte ofAddress of the register
      m_tx_buf[4]=0x00; //4th byte ofAddress of the register
      m_tx_buf[5]=0x00;   //1st byte
      m_tx_buf[6]=0x80;   //2nd byte
      m_tx_buf[7]=m_rx_buf[9];   //#rd byte
      m_tx_buf[8]=m_rx_buf[10];   //4th byte
      nrf_drv_spi_transfer(&spi,m_tx_buf,9,m_rx_buf,2);
      nrf_delay_us(100);
      printf("Configuring GENERIC PACKET FRAME CONF 1 register\n");
      for(i=0;i<4;i++)
      printf("m_rx_buff[%d]: %x \n",i, m_rx_buf[i]);
}
//GENERIC PACKET FRAME CONFIGURATION 5 REGISTER Address: 0x20000510, Name: GENERIC_PKT_FRAME_CFG5
//TX_PHR=0x0288     REFER pg no. 5 of application note, Refer pg no. 68 of IEEE 802.15.4g std
    //Mode switch =0
    //FCS Type=1
    //Data whitening=0
    //Frame Length=40 bytes
void set_GENERIC_PKT_FRAME_CFG5_reg()
{
     //Memory acccess mode 1 in write format
      read_address(0x200000510);
      int TX_PHR=(MODE_SWITCH<<15)|0x0000|(FCS_TYPE<<12)|(DATA_WHITENING<<11)|((length + 2)& 0x07FF);//0X0149 FOR EB of length 72
      printf("\nTX_PHR %x", TX_PHR);
      m_tx_buf[0]=0x38;  //Memory access command CNM=0, RNW=0, BNR=1, ANP=1, LNS=1, MHPTR=000 --> 0011 1000
      m_tx_buf[1]=0x20; //1st byte ofAddress of the register
      m_tx_buf[2]=0x00; //2nd byte ofAddress of the register
      m_tx_buf[3]=0x05; //3rd byte ofAddress of the register
      m_tx_buf[4]=0x10; //4th byte ofAddress of the register
      m_tx_buf[5]=m_rx_buf[7];   //1st byte
      m_tx_buf[6]=m_rx_buf[8];   //2nd byte
      m_tx_buf[7]=TX_PHR>>8;   //#rd byte
      m_tx_buf[8]=TX_PHR;   //4th byte
      nrf_drv_spi_transfer(&spi,m_tx_buf,9,m_rx_buf,2);
      nrf_delay_us(100);
      //printf("Configuring GENERIC PACKET FRAME CONF 5 register\n");
      //for(i=0;i<4;i++)
      //printf("m_rx_buff[%d]: %x \n",i, m_rx_buf[i]);
}
//RF CHANNEL FREQUENCY REGISTER Address: 0x200002EC, Name: PROFILE_CH_FREQ
// Refer pg no. 62 of IEEE 802.15.4g std(80 page standard)

//void set_PROFILE_CH_FREQ_reg()
//{
//  //read_address(0x200002EC);
//      m_tx_buf[0]=0x38;  //Memory access command CNM=0, RNW=0, BNR=1, ANP=1, LNS=1, MHPTR=000 --> 0011 1000
//      m_tx_buf[1]=0x20; //1st byte ofAddress of the register
//      m_tx_buf[2]=0x00; //2nd byte ofAddress of the register
//      m_tx_buf[3]=0x05; //3rd byte ofAddress of the register
//      m_tx_buf[4]=0x10; //4th byte ofAddress of the register
//      m_tx_buf[5]=m_rx_buf[7];   //1st byte
//      m_tx_buf[6]=m_rx_buf[8];   //2nd byte
//      m_tx_buf[7]=TX_PHR>>8;   //#rd byte
//      m_tx_buf[8]=TX_PHR;   //4th byte
//      nrf_drv_spi_transfer(&spi,m_tx_buf,9,m_rx_buf,2);
//      nrf_delay_us(100);


//}      //Dont want this function,frequency written to this address in set_channel



//here this function reads a word data to a specified address
void read_address(long addx)
{
     // mode 1 memory access mode in read mode  (pg no 47 of ADF software manual)
       m_tx_buf[0]=0x78; // CNM=0, RNW=1, BNR=1, ANP=1, LNS=1, MHPTR=000 (dont care) -->01111000
       m_tx_buf[1]=addx>>24;  // 1st Byte (MSB)
       m_tx_buf[2]=addx>>16;  //2nd Byte
       m_tx_buf[3]=addx>>8;  //3rd Byte
       m_tx_buf[4]=addx;  //4th Byte (LSB)
       nrf_drv_spi_transfer(&spi,m_tx_buf,5,m_rx_buf,11);
       //Although we are reading 11 bytes 1st 7 bytes will be status bytes next 4 bytes is the data from ADF to host
       nrf_delay_us(250);
       //for(i=7;i<11;i++)
       // printf("m_rx_buff[%d]: %x \n",i, m_rx_buf[i]);
}
//here this function writes a word data to a specified address
void write_address(long addx, long datax)
{
  //Memory acccess mode 1 in write format
      m_tx_buf[0]=0x38;  //Memory access command CNM=0, RNW=0, BNR=1, ANP=1, LNS=1, MHPTR=000 --> 0011 1000
      m_tx_buf[1]=addx>>24; //1st byte ofAddress of the register
      m_tx_buf[2]=addx>>16; //2nd byte ofAddress of the register
      m_tx_buf[3]=addx>>8; //3rd byte ofAddress of the register
      m_tx_buf[4]=addx; //4th byte ofAddress of the register
      m_tx_buf[5]=datax>>24;   //1st byte
      m_tx_buf[6]=datax>>16;   //2nd byte
      m_tx_buf[7]=datax>>8;   //#rd byte
      m_tx_buf[8]=datax;   //4th byte
      nrf_drv_spi_transfer(&spi,m_tx_buf,9,m_rx_buf,2);
      nrf_delay_us(100);
}

//writing Data to Packet Memory at address 0x20000AF0
//PTR_TX_BASE=02BC Tx_payload_buffer=20000000+(2BC*4)=20000AF0
void write_Packet(long asn)
{
        length=74;
        check_length_and_padding();
        set_GENERIC_PKT_FRAME_CFG5_reg();
//Memory acccess mode 1 in write format
      m_tx_buf[0]=0x38;  //Memory access command CNM=0, RNW=0, BNR=1, ANP=1, LNS=1, MHPTR=000 --> 0011 1000
      m_tx_buf[1]=0x20; //1st byte ofAddress of the register
      m_tx_buf[2]=0x00; //2nd byte ofAddress of the register
      m_tx_buf[3]=0x0A; //3rd byte ofAddress of the register
      m_tx_buf[4]=0xF0; //4th byte ofAddress of the register
      m_tx_buf[5] = 0xDC; // FCF --> Frame control field [15:8] MSB
      m_tx_buf[6] = 0x51; // FCF --> Frame control field [7:0] LSB
      m_tx_buf[7] = 0x1C; // SEQ NUM -->Sequence number
      m_tx_buf[8] = 0x22; // Dest PAN Identifier --> [15:8] MSB
      m_tx_buf[9] = 0x22; // Dest PAN Identifier --> [7:0] MSB
      m_tx_buf[10] = 0x00; // Dest Address --> [63:56]
      m_tx_buf[11] = 0x00; // Dest Address --> [55:48]
      m_tx_buf[12] = 0x00; // Dest Address --> [47:40]
      m_tx_buf[13] = 0x00; // Dest Address --> [39:32]
      m_tx_buf[14] = 0x00; // Dest Address --> [31:24]
      m_tx_buf[15] = 0x00; // Dest Address --> [23:16]
      m_tx_buf[16] = 0x22; // Dest Address --> [15:8]
      m_tx_buf[17] = 0x22; // Dest Address --> [7:0]
      m_tx_buf[18] = 0x11; // Source PAN Identifier --> [15:8] MSB
      m_tx_buf[19] = 0x11; // Source PAN Identifier --> [15:8] MSB
      m_tx_buf[20] = 0x00; // Source Address --> [63:56]
      m_tx_buf[21] = 0x00; // Source Address --> [55:48]
      m_tx_buf[22] = 0x00; // Source Address --> [47:40]
      m_tx_buf[23] = 0x00; // Source Address --> [39:32]
      m_tx_buf[24] = 0x00; // Source Address --> [31:24]
      m_tx_buf[25] = 0x00; // Source Address --> [23:16]
      m_tx_buf[26] = 0x11; // Source Address --> [15:8]
      m_tx_buf[27] = 0x11; // Source Address --> [7:0]	
      m_tx_buf[28] = 'H'; // Frame Payload
      m_tx_buf[29] = 'E'; // Frame Payload
      m_tx_buf[30] = 'l'; // Frame Payload
      m_tx_buf[31] = 'l'; // Frame Payload
      m_tx_buf[32] = 'o'; // Frame Payload
      m_tx_buf[33] = 'W'; // Frame Payload
      m_tx_buf[34] = 'o'; // Frame Payload
      m_tx_buf[35] = 'r'; // Frame Payload
      m_tx_buf[36] = 'l'; // Frame Payload
      m_tx_buf[37] = 'd'; // Frame Payload
      m_tx_buf[38] = asn;
      m_tx_buf[39] = asn;
      m_tx_buf[40] = asn;
      m_tx_buf[41] = asn;
      m_tx_buf[42] = asn;
      m_tx_buf[43] = asn;
      m_tx_buf[44] = asn;
    //EXTRA BYTES ADDED TO MAKE THE PACKET 76 BYTES
       m_tx_buf[45] = 0x02;
        m_tx_buf[46] = 0x1C;
        m_tx_buf[47] = 0x00;
        m_tx_buf[48] = 0x0A;
        m_tx_buf[49] = 0x19;//Legth of IE
        m_tx_buf[50] = 0x1B;//Type and Sub ID  
        m_tx_buf[51] = 0x01;// Number of slotframe
        //Start of slot frame descriptor 1
        m_tx_buf[52] = 0x01;// Slotframe handler
        m_tx_buf[53] = 0x00;// Slot frame size
        m_tx_buf[54] = 0x04;// Slot frame size
        m_tx_buf[55] = 0x04; //Number of Links
        // Link1 Information 
        m_tx_buf[56] = 0x00; // Timeslot Number
        m_tx_buf[57] = 0x00; // Timeslot Number
        m_tx_buf[58] = 0x00; // Channel Offset
        m_tx_buf[59] = 0x00; // Channel Offset
        m_tx_buf[60] = 0x00; // Link options is priority
        // Link2 Information 
        m_tx_buf[61] = 0x00; // Timeslot Number
        m_tx_buf[62] = 0x01; // Timeslot Number
        m_tx_buf[63] = 0x00; // Channel Offset
        m_tx_buf[64] = 0x01; // Channel Offset
        m_tx_buf[65] = 0x00; // Link options Rx link
        // Link3 Information 
        m_tx_buf[66] = 0x00; // Timeslot Number
        m_tx_buf[67] = 0x02; // Timeslot Number
        m_tx_buf[68] = 0x00; // Channel Offset
        m_tx_buf[69] = 0x02; // Channel Offset
        m_tx_buf[70] = 0x00; // Link options Rx link
        // Link4 Information 
        m_tx_buf[71] = 0x00; // Timeslot Number
        m_tx_buf[72] = 0x03; // Timeslot Number
        m_tx_buf[73] = 0x00; // Channel Offset
        m_tx_buf[74] = 0x03; // Channel Offset
        m_tx_buf[75] = 0x09; // Link options RX link 
        m_tx_buf[76] = 0x00;
        m_tx_buf[77] = 0x7C;
        m_tx_buf[78] = 0x0F;
      nrf_drv_spi_transfer(&spi,m_tx_buf,length+5,m_rx_buf,2);
      nrf_delay_us(250);
      //printf("Writing to Tx buff\n");
      ////for(i=0;i<4;i++)
      ////printf("m_rx_buff[%d]: %x \n",i, m_rx_buf[i]);
      //for(i=28;i<38;i++)
      //{
      //  printf("%c", m_tx_buf[i]);
      //}
      //printf("%d",m_tx_buf[i]);
      //printf("\n");
}

//Reading the received packet from Packet Memory at 0x20000B70
//PTR_RX_BASE=02DF Rx_payload_buffer=20000000+(2DC*4)=20000B70 
void read_Packet()
{
       // mode 1 memory access mode in read mode  (pg no 47 of ADF software manual)
       m_tx_buf[0]=0x78; // CNM=0, RNW=1, BNR=1, ANP=1, LNS=1, MHPTR=000 (dont care) -->01111000
       m_tx_buf[1]=0x20;  // 1st Byte (MSB)
       m_tx_buf[2]=0x00;  //2nd Byte
       m_tx_buf[3]=0x0C;  //3rd Byte
       m_tx_buf[4]=0x18;  //4th Byte (LSB)
       nrf_drv_spi_transfer(&spi,m_tx_buf,5,m_rx_buf,87);
       //Although we are reading 11 bytes 1st 7 bytes will be status bytes next 4 bytes is the data from ADF to host
       nrf_delay_us(250);
       for(i=30;i<40;i++)
        printf("%c", m_rx_buf[i]);
       printf("%d \n", m_rx_buf[i]);
}
void clear_interrupts()
{
      //read_address(0X40003808);
      //for(i=7;i<11;i++)
      //  printf("\n m_rx_buf[%d] contents for clear_interrupts function is = %d",i,m_rx_buf[i]);
      m_tx_buf[0]=0x38;  //Memory access command CNM=0, RNW=0, BNR=1, ANP=1, LNS=1, MHPTR=000 --> 0011 1000
      m_tx_buf[1]=0x40; //1st byte ofAddress of the register
      m_tx_buf[2]=0x00; //2nd byte ofAddress of the register
      m_tx_buf[3]=0x38; //3rd byte ofAddress of the register
      m_tx_buf[4]=0x08; //4th byte ofAddress of the register
      m_tx_buf[5]=0X09;   //1st byte
      m_tx_buf[6]=0X51;   //2nd byte
      m_tx_buf[7]=0X1F;   //#rd byte
      m_tx_buf[8]=0XFF;   //4th byte
      nrf_drv_spi_transfer(&spi,m_tx_buf,9,m_rx_buf,2);
      nrf_delay_us(100);
}

//writing Acknowledgement to Packet Memory at address 0x20000AF0
//PTR_TX_BASE=02BC Tx_payload_buffer=20000000+(2BC*4)=20000AF0
void write_Ack(long asn)
{
        length=74;
        check_length_and_padding();
        set_GENERIC_PKT_FRAME_CFG5_reg();
//Memory acccess mode 1 in write format
      m_tx_buf[0]=0x38;  //Memory access command CNM=0, RNW=0, BNR=1, ANP=1, LNS=1, MHPTR=000 --> 0011 1000
      m_tx_buf[1]=0x20; //1st byte ofAddress of the register
      m_tx_buf[2]=0x00; //2nd byte ofAddress of the register
      m_tx_buf[3]=0x0A; //3rd byte ofAddress of the register
      m_tx_buf[4]=0xF0; //4th byte ofAddress of the register
      m_tx_buf[5] = 0xDC; // FCF --> Frame control field [15:8] MSB
      m_tx_buf[6] = 0x51; // FCF --> Frame control field [7:0] LSB
      m_tx_buf[7] = 0x1C; // SEQ NUM -->Sequence number
      m_tx_buf[8] = 0x22; // Dest PAN Identifier --> [15:8] MSB
      m_tx_buf[9] = 0x22; // Dest PAN Identifier --> [7:0] MSB
      m_tx_buf[10] = 0x00; // Dest Address --> [63:56]
      m_tx_buf[11] = 0x00; // Dest Address --> [55:48]
      m_tx_buf[12] = 0x00; // Dest Address --> [47:40]
      m_tx_buf[13] = 0x00; // Dest Address --> [39:32]
      m_tx_buf[14] = 0x00; // Dest Address --> [31:24]
      m_tx_buf[15] = 0x00; // Dest Address --> [23:16]
      m_tx_buf[16] = 0x22; // Dest Address --> [15:8]
      m_tx_buf[17] = 0x22; // Dest Address --> [7:0]
      m_tx_buf[18] = 0x11; // Source PAN Identifier --> [15:8] MSB
      m_tx_buf[19] = 0x11; // Source PAN Identifier --> [15:8] MSB
      m_tx_buf[20] = 0x00; // Source Address --> [63:56]
      m_tx_buf[21] = 0x00; // Source Address --> [55:48]
      m_tx_buf[22] = 0x00; // Source Address --> [47:40]
      m_tx_buf[23] = 0x00; // Source Address --> [39:32]
      m_tx_buf[24] = 0x00; // Source Address --> [31:24]
      m_tx_buf[25] = 0x00; // Source Address --> [23:16]
      m_tx_buf[26] = 0x11; // Source Address --> [15:8]
      m_tx_buf[27] = 0x11; // Source Address --> [7:0]	
      m_tx_buf[28] = 'A'; // Frame Payload
      m_tx_buf[29] = 'C'; // Frame Payload
      m_tx_buf[30] = 'K'; // Frame Payload
      m_tx_buf[31] = 'N'; // Frame Payload
      m_tx_buf[32] = 'o'; // Frame Payload
      m_tx_buf[33] = 'L'; // Frame Payload
      m_tx_buf[34] = 'E'; // Frame Payload
      m_tx_buf[35] = 'D'; // Frame Payload
      m_tx_buf[36] = 'G'; // Frame Payload
      m_tx_buf[37] = 'E'; // Frame Payload
      m_tx_buf[38] = asn;
      m_tx_buf[39] = asn;
      m_tx_buf[40] = asn;
      m_tx_buf[41] = asn;
      m_tx_buf[42] = asn;
      m_tx_buf[43] = asn;
      m_tx_buf[44] = asn;
    //EXTRA BYTES ADDED TO MAKE THE PACKET 76 BYTES
       m_tx_buf[45] = 0x02;
        m_tx_buf[46] = 0x1C;
        m_tx_buf[47] = 0x00;
        m_tx_buf[48] = 0x0A;
        m_tx_buf[49] = 0x19;//Legth of IE
        m_tx_buf[50] = 0x1B;//Type and Sub ID  
        m_tx_buf[51] = 0x01;// Number of slotframe
        //Start of slot frame descriptor 1
        m_tx_buf[52] = 0x01;// Slotframe handler
        m_tx_buf[53] = 0x00;// Slot frame size
        m_tx_buf[54] = 0x04;// Slot frame size
        m_tx_buf[55] = 0x04; //Number of Links
        // Link1 Information 
        m_tx_buf[56] = 0x00; // Timeslot Number
        m_tx_buf[57] = 0x00; // Timeslot Number
        m_tx_buf[58] = 0x00; // Channel Offset
        m_tx_buf[59] = 0x00; // Channel Offset
        m_tx_buf[60] = 0x00; // Link options is priority
        // Link2 Information 
        m_tx_buf[61] = 0x00; // Timeslot Number
        m_tx_buf[62] = 0x01; // Timeslot Number
        m_tx_buf[63] = 0x00; // Channel Offset
        m_tx_buf[64] = 0x01; // Channel Offset
        m_tx_buf[65] = 0x00; // Link options Rx link
        // Link3 Information 
        m_tx_buf[66] = 0x00; // Timeslot Number
        m_tx_buf[67] = 0x02; // Timeslot Number
        m_tx_buf[68] = 0x00; // Channel Offset
        m_tx_buf[69] = 0x02; // Channel Offset
        m_tx_buf[70] = 0x00; // Link options Rx link
        // Link4 Information 
        m_tx_buf[71] = 0x00; // Timeslot Number
        m_tx_buf[72] = 0x03; // Timeslot Number
        m_tx_buf[73] = 0x00; // Channel Offset
        m_tx_buf[74] = 0x03; // Channel Offset
        m_tx_buf[75] = 0x09; // Link options RX link 
        m_tx_buf[76] = 0x00;
        m_tx_buf[77] = 0x7C;
        m_tx_buf[78] = 0x0F;
      nrf_drv_spi_transfer(&spi,m_tx_buf,length+5,m_rx_buf,2);
      nrf_delay_us(250);

      //for(i=0;i<4;i++)
      //printf("m_rx_buff[%d]: %x \n",i, m_rx_buf[i]);
      //for(i=28;i<38;i++)
      //{
      //  printf("%c", m_tx_buf[i]);
      //}
      //printf("%d",m_tx_buf[i]);
      printf("\n");
}

//Reading the received Acknowledgement from Packet Memory at 0x20000B70
//PTR_RX_BASE=02DF Rx_payload_buffer=20000000+(2DC*4)=20000B70 
void read_Ack()
{
       // mode 1 memory access mode in read mode  (pg no 47 of ADF software manual)
       m_tx_buf[0]=0x78; // CNM=0, RNW=1, BNR=1, ANP=1, LNS=1, MHPTR=000 (dont care) -->01111000
       m_tx_buf[1]=0x20;  // 1st Byte (MSB)
       m_tx_buf[2]=0x00;  //2nd Byte
       m_tx_buf[3]=0x0C;  //3rd Byte
       m_tx_buf[4]=0x18;  //4th Byte (LSB)
       nrf_drv_spi_transfer(&spi,m_tx_buf,5,m_rx_buf,87);
       //Although we are reading 11 bytes 1st 7 bytes will be status bytes next 4 bytes is the data from ADF to host
       nrf_delay_us(250);
       for(i=30;i<40;i++)
        printf("%c", m_rx_buf[i]);
       printf("%d \n", m_rx_buf[i]);
}
//writing Emhanced beacon to Packet Memory at address 0x20000AF0
//PTR_TX_BASE=02BC Tx_payload_buffer=20000000+(2BC*4)=20000AF0
void write_EB(long asn)
{
        length=74;
        check_length_and_padding();
        set_GENERIC_PKT_FRAME_CFG5_reg();

//Memory acccess mode 1 in write format
        m_tx_buf[0] = 0x38;  //Memory access command CNM=0, RNW=0, BNR=1, ANP=1, LNS=1, MHPTR=000 --> 0011 1000
        m_tx_buf[1] = 0x20; //1st byte ofAddress of the register
        m_tx_buf[2] = 0x00; //2nd byte ofAddress of the register
        m_tx_buf[3] = 0x0A; //3rd byte ofAddress of the register
        m_tx_buf[4] = 0xF0; //4th byte ofAddress of the register
	m_tx_buf[5] = 0x22; // FCF --> Frame control field [15:8] MSB 
	m_tx_buf[6] = 0x00; // FCF --> Frame control field [7:0] LSB
	m_tx_buf[7] = 0x1C; // SEQ NUM -->Sequence number
	m_tx_buf[8] = 0x11; // Dest PAN Identifier --> [15:8] MSB
	m_tx_buf[9] = 0x11; // Dest PAN Identifier --> [7:0] MSB
	m_tx_buf[10] = 0x00; // Dest Address --> [63:56]
	m_tx_buf[11] = 0x00; // Dest Address --> [55:48]
	m_tx_buf[12] = 0x00; // Dest Address --> [47:40]
	m_tx_buf[13] = 0x00; // Dest Address --> [39:32]
	m_tx_buf[14] = 0x00; // Dest Address --> [31:24]
	m_tx_buf[15] = 0x00; // Dest Address --> [23:16]
	m_tx_buf[16] = 0x22; // Dest Address --> [15:8]
	m_tx_buf[17] = 0x22; // Dest Address --> [7:0]
	m_tx_buf[18] = 0x11; // Source PAN Identifier --> [15:8] MSB
	m_tx_buf[19] = 0x11; // Source PAN Identifier --> [15:8] MSB
	m_tx_buf[20] = 0x00; // Source Address --> [63:56]
	m_tx_buf[21] = 0x00; // Source Address --> [55:48]
	m_tx_buf[22] = 0x00; // Source Address --> [47:40]
	m_tx_buf[23] = 0x00; // Source Address --> [39:32]
	m_tx_buf[24] = 0x00; // Source Address --> [31:24]
	m_tx_buf[25] = 0x00; // Source Address --> [23:16]
	m_tx_buf[26] = 0x11; // Source Address --> [15:8]
	m_tx_buf[27] = 0x11; // Source Address --> [7:0]	
        //Header Termination IE HTE1 Length=0, Element ID=7E, Type=0
        m_tx_buf[28] = 0x1F; 
        m_tx_buf[29] = 0x80;
        //For formats of IE  refer page 198 of the standard
        //TSCH Sync IEs  refer Pg no. 201
        m_tx_buf[30] = 0x06;//Length of IE in octects -->(5 bytes asn, 1 byte Join metric)
        m_tx_buf[31] = 0x1A;//0x1A= 0001 1010 ---> Type=0,Sub Id=0x1A 
        m_tx_buf[32] = asn;  //ASN 1st byte 
        m_tx_buf[33] = asn>>8; //ASN 2nd byte
        m_tx_buf[34] = asn>>16; //ASN 3rd byte
        m_tx_buf[35] = asn>>24; //ASN 4th byte
        m_tx_buf[36] = asn>>32; //ASN 5th byte
        m_tx_buf[37] = 0x01;  // Join metric
        //End pf TSCH Sync IEs
        //Channel hopping IE page 235
        m_tx_buf[38] = 0x05; // Length of IE
        m_tx_buf[39] = 0xC8; // Type and Sub ID 
        m_tx_buf[40] = 0x00; // Hopping sequence ID
        //No Channel Page
        m_tx_buf[41] = 0x00; // Number of channels
        m_tx_buf[42] = 0x10; // Number of channels
        // No Phy Configuration
        //MAC extended bit map not present
        // Hopping sequence length and sequence are set to default value
      //  m_tx_buf[41] = 0x04;// Hopping sequence length
        //Refer pg 235 of the standard
        //m_tx_buf[42] = 0x17; //channel hopping number1 in hex
        //m_tx_buf[43] = 0x0F; //channel hopping number2 in hex
        //m_tx_buf[44] = 0x14; //channel hopping number3 in hex
        //m_tx_buf[45] = 0x0B; //channel hopping number4 in hex 
        m_tx_buf[43] = (asn%16)>>8; // current hopping index 
        m_tx_buf[44] = (asn%16); // current hopping index 
        //End of Channel Hopping IEs 
        //TSCH Timeslot IE Refer Pg no 203 
        m_tx_buf[45] = 0x02;// Length of IE
        m_tx_buf[46] = 0x1C;//Type and Sub ID
        m_tx_buf[47] = 0x00;//Timeslot ID It follows default timming of the hardware hence we are mentioning only timeslot length
        m_tx_buf[48] = 0x0A;//Timeslot Length
        //End of TSCH Timeslot IE
        //TSCH Slot Frame and Link IE Refer Pg no. 201
        m_tx_buf[49] = 0x19;//Legth of IE
        m_tx_buf[50] = 0x1B;//Type and Sub ID  
        m_tx_buf[51] = 0x01;// Number of slotframe
        //Start of slot frame descriptor 1
        m_tx_buf[52] = 0x01;// Slotframe handler
        m_tx_buf[53] = 0x00;// Slot frame size
        m_tx_buf[54] = 0x04;// Slot frame size
        m_tx_buf[55] = 0x04; //Number of Links
        // Link1 Information 
        m_tx_buf[56] = 0x00; // Timeslot Number
        m_tx_buf[57] = 0x00; // Timeslot Number
        m_tx_buf[58] = 0x00; // Channel Offset
        m_tx_buf[59] = 0x00; // Channel Offset
        m_tx_buf[60] = 0x00; // Link options is priority
        // Link2 Information 
        m_tx_buf[61] = 0x00; // Timeslot Number
        m_tx_buf[62] = 0x01; // Timeslot Number
        m_tx_buf[63] = 0x00; // Channel Offset
        m_tx_buf[64] = 0x01; // Channel Offset
        m_tx_buf[65] = 0x00; // Link options Rx link
        // Link3 Information 
        m_tx_buf[66] = 0x00; // Timeslot Number
        m_tx_buf[67] = 0x02; // Timeslot Number
        m_tx_buf[68] = 0x00; // Channel Offset
        m_tx_buf[69] = 0x02; // Channel Offset
        m_tx_buf[70] = 0x00; // Link options Rx link
        // Link4 Information 
        m_tx_buf[71] = 0x00; // Timeslot Number
        m_tx_buf[72] = 0x03; // Timeslot Number
        m_tx_buf[73] = 0x00; // Channel Offset
        m_tx_buf[74] = 0x03; // Channel Offset
        m_tx_buf[75] = 0x09; // Link options RX link 
        // Link option  Figure 7-55 
        // Tx Link = 1
        // Rx Link = 0
        // Shared Link = 0
        // Timekeeping = 1
        // Priority = 0
        //End of Slot frame descriptor 1
        //End of TSCH slotframe IE
        //Payload TerminationIE Length=0, Element ID=0xF, Type=1
        m_tx_buf[76] = 0x00;
        m_tx_buf[77] = 0x7C;
        m_tx_buf[78] = 0x0F;
       //// for(i = length+4-pad_data;i<length+5;i++)
       //     {
       //      // m_tx_buf[i] = 0xBB;
       //       printf("\t%x\t%d",m_tx_buf[i],i);

       //     }
//Note if you change the frame payload length, update the PHR Field
	nrf_drv_spi_transfer(&spi,m_tx_buf,length+5,m_rx_buf,2);
        nrf_delay_us(1000);//got till 56th char for 1200
       // printf("EBwrite :\n");
}
//Reading the received EB from Packet Memory at 0x20000B70
//PTR_RX_BASE=02DF Rx_payload_buffer=20000000+(2DC*4)=20000B70 

void read_EB()
{
       // mode 1 memory access mode in read mode  (pg no 47 of ADF software manual)
       m_tx_buf[0]=0x78; // CNM=0, RNW=1, BNR=1, ANP=1, LNS=1, MHPTR=000 (dont care) -->01111000
       m_tx_buf[1]=0x20;  // 1st Byte (MSB)
       m_tx_buf[2]=0x00;  //2nd Byte
       m_tx_buf[3]=0x0C;  //3rd Byte
       m_tx_buf[4]=0x18;  //4th Byte (LSB)
       nrf_drv_spi_transfer(&spi,m_tx_buf,5,m_rx_buf,87);
       //Although we are reading 11 bytes 1st 7 bytes will be status bytes next 4 bytes is the data from ADF to host
       nrf_delay_us(1000);
      for(i = 28;i<=48;i++)
      {
      printf("\t%X",m_rx_buf[i]);
      }
     printf("\n ");
     for(i = 49;i<=62;i++)
      {
      printf("\t%X",m_rx_buf[i]);
      }
     printf("\n ");
         for(i = 63;i<=80;i++)
      {
      printf("\t%X",m_rx_buf[i]);
      }
        i_EB=32;
        do
        { 
          i_EB++;
          type=m_rx_buf[i_EB]&0x80;
          if(type)//type 1
          {
            IE_ID=(m_rx_buf[i_EB]&0x78)>>3;
            len_IE=m_rx_buf[i_EB-1]|((m_rx_buf[i_EB]&0x07)<<8); //length of type 1 IE is 10 bits
          }
           else // type 0 
           {
           IE_ID=m_rx_buf[i_EB];
           len_IE=m_rx_buf[i_EB-1];  // length of type 0 IE is 8 bits
           }
           if(IE_ID==0x7C)
            break;
           printf("IE ID: %d\n",IE_ID);
          printf("IE Length: %d\n",len_IE);
           j=i_EB+1;
          // printf("i=%d \t j=%d\n",i_EB,j);
          switch(IE_ID)
          {  
              case 0x1A:{//time sync IE
                       // printf("Time sync IE");

                       IE[0]=m_rx_buf[j++]|m_rx_buf[j++]<<8|m_rx_buf[j++]<<16;
                       IE[1]=m_rx_buf[j++]|m_rx_buf[j++]<<8;

                        //for (k=0;k<5;k++,j++) //changing to print asn 
                        //{
                        //    IE[k]= m_rx_buf[j];
                        //    printf("#%d ",IE[k]);// index is j 
                        //}
                        k=2;
                        break;
                        }
              case 0x09:{//channel Hopping IE
                        //printf("Channel hopping IE");
                        //j=i+1--> 1st byte of Channel Hopping IE ie hopping sequence IE
                        channel_id=m_rx_buf[++j];//channel sequence ID this should be the channel value that the node should operate and this valure should be set in real time by mac
                        IE[k++]=channel_id;
                        num_of_channels=m_rx_buf[j++]<<8|m_rx_buf[j++];//Number of channel
                        IE[k++]=num_of_channels;
                        break;
                        }
              case 0x1C:  {//TSCH timeslot IE
                       // printf("TSCH timeslot IE");
                        timeslot_len=m_rx_buf[++j];
                        IE[k++]=timeslot_len;
                        break;
                        }
              case 0x1B:{//TSCH slotframe IE
                     //  printf("TSCH slotframe IE");
                       // for(k=0;k<m_rx_buf[j];k++)
                       // { 
                          //slot_frame_handler=m_rx_buf[++j];
                          //slot_frame_size=m_rx_buf[++j];
                          //number_of_links=m_rx_buf[++j];
                           j=j+1;  //+1 to go to next index in array +3 to skip over slotfram_handler
                          IE[k++]=m_rx_buf[j++]|m_rx_buf[j++]<<12|m_rx_buf[j++]<<8|m_rx_buf[j]<<24; // MSB has number of link next 2 byte slotframe size last byte slotframe handler
                          int number_of_links=m_rx_buf[j++];
                          //printf("Number of Links: %d\n",number_of_links);
                          //printf("IE[%d]: %x\n",k-1, IE[k-1]);
                          for(int m=0;m<number_of_links;m++)
                          {
                          timeslot_numb[m]=m_rx_buf[j++]<<8|m_rx_buf[j++];
                          IE[k++]=timeslot_numb[m];
                          channel_offset[m]=m_rx_buf[j++]<<8|m_rx_buf[j++];
                          IE[k++]= channel_offset[m];
                          Link_option[m]=m_rx_buf[j++];
                          IE[k++]= Link_option[m];
                         // printf("\ntimeslot_numb[m] : %d\nchannel_offset[m]: %d\nLink_option[m]: %d\n", timeslot_numb[m], channel_offset[m], Link_option[m]);
                          }
                            
                        //}
                        break;
                        }
              default : break;
          }
         i_EB=i_EB+len_IE+1;
        }
        while(i_EB<=80);
        IE[18]=m_rx_buf[80];
        for(int m=0; m<19; m++)
          printf("IE[%d]: %d\n", m,IE[m]);
}
void check_length_and_padding()
{
  if(length % 4 != 0)
  {
    pad_data = (4 - length%4);
    for(i = length+5;i<length+5+pad_data;i++)
    {
      m_tx_buf[i] = 0xBB;
      printf("\t%x\t%d",m_tx_buf[i],i);

    }
    length = length + pad_data;
    printf("\tthe length after padding is %d",length);
  }
  else 
    printf(" NO Padding ");
}