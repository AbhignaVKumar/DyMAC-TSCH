/*registers*///yoooooooooooooooo







//#define PREAMBLE_NUM_VALIDATE 0x3F3
//#define DR0 0x30E
//#define DR1 0x30F
//#define TX_FD 0x304
//#define DM_CFG0 0x305
//#define DM_CFG1 0x38B
//#define IIRF_CFG  0x389
//#define TX_M  0x306
//#define SYNT  0x335
//#define AGC_MAX 0x3B4
//#define AGC_CFG2 0x3B6
//#define AGC_CFG3  0x3B7
//#define AGC_CFG4  0x3B8
////#define AGC_CFG5  0x3B9 //commented on 29-11-21 cuz we have already defined it
//#define AGC_CFG6  0x3BA
//#define AGC_CFG7  0x3BC
//#define AGC_CFG1  0x3B2
//#define OCL_CFG0  0x3BF
//#define OCL_CFG1  0x3C4
//#define IRQ_SRC0 0x3CB
//#define IRQ_SRC1 0x3CC
//#define IRQ1_EN0  0x3C7
//#define IRQ1_EN1  0x3C8
//#define IRQ2_EN0  0x3C9
//#define IRQ2_EN1  0x3CA
//#define OCL_BW0 0x3D2
//#define OCL_BW1 0x3D3
//#define OCL_BW2 0x3D4
//#define OCL_BW3 0x3D5
//#define OCL_BW4 0x3D6
//#define OCL_BWS 0x3D7
//#define OCL_CFG13 0x3E0
//#define EXT_CTRL  0x100
//#define SYNC_WORD0 0x10C
//#define SYNC_WORD1 0x10D
//#define SYNC_WORD2 0x10E
//#define SYNC_CONFIG 0x10F
//#define BUFFERCFG 0x107
//#define PKT_CFG 0x108
////added on 27-11-21 for cca impl
//#define delaycfg0 0x109 //For specifying the delay before the comencement of cca default value is 192us
//#define delaycfg2 0x10B //For specifying the delay before the comencement of cca
//#define agc_cfg5 0x3B9 //Programming the RSSI averaging time
//#define cca1 0x105//RSSI threshold for CCA
//#define cca2 0x106//for setting the type of CCA: continuous, periodic, automatic 
////added on 29-11-21
//#define rrb 0x30C//RSSI readback value holds the results of the previous RSSI measurement cycle

/*command functions*/
//#define SPI_MEM_WR(x)	(0x18 + (x >> 8)) // Write data to MCR or Packet RAM as random block
//#define SPI_MEM_RD(x)	(0x38 + (x >> 8)) // Read data from MCR or Packet RAM random block
//#define RC_IDLE       0xB2                //Invoke transition of the radio controller into the idle state
//#define RC_PHY_RDY    0xB3                //Invoke transition of the radio controller into the PHY_RDY state
//#define RC_RX         0xB4                //Invoke transition of the radio controller into the RX state
//#define RC_TX         0xB5                //Invoke transition of the radio controller into the TX state
//#define RC_MEAS       0xB6                //Invoke transition of the radio controller into the MEAS state
//#define RC_CCA        0xB7                //Invoke clear channel assessment
//#define RC_PC_RESET   0xC7                //Program counter reset.only used after firmware download to the program RAM 
//#define SPI_PKT_WR  0x10
//#define RC_PC_RESET 0xC7 
//#define SPI_PKT_RD 0x30
//#define RC_CFG  0x13E
//#define SPI_NOP	0xFF
//#define BIT(nr)         (1UL << (nr))
//#define IRQ_CCA_COMPLETE	BIT(0)
//#define IRQ_SFD_RX		BIT(1)
//#define IRQ_SFD_TX		BIT(2)
//#define IRQ_RX_PKT_RCVD		BIT(3)
//#define IRQ_TX_PKT_SENT		BIT(4)

////table 49 pg 82
//#define IRQ_BATT_ALERT          BIT(5)
//#define IRQ_POR                 BIT(4)
//#define IRQ_RC_READY            BIT(3)
//#define IRQ_WAKEUP              BIT(2)
//#define IRQ_POWERUP             BIT(1)  
    

#define SPI_INSTANCE  0

//adf7030 state machine radio commands refer pg no. 14 of ADF7030-1 software manual 
#define PHY_SLEEP 0x00
#define PHY_OFF 0x01
#define PHY_ON 0x02
#define PHY_RX 0x03
#define PHY_TX 0x04
#define CFG_DEV 0x05
#define CCA 0x06
#define DO_CAL 0x09
#define GPCLK 0x10
#define MON 0x0A
#define LFRC_CAL 0x0C

#define MODE_SWITCH 0x00
#define FCS_TYPE 0x01
#define DATA_WHITENING 0x00



#define macTsTxOffset    1800
#define macTsCca          128
#define macTsRxTx         192
//#define macTsRxOffset     2120
#define macTsMaxTx        4256
#define macTsRxAckDelay   800
#define macTsAckWait      400
#define macTsRxOffset     1020
#define macTsRxWait       2200
#define macTsTxAckDelay   1000
#define macTsMaxAck       2400
#define macTsTimeslotLength 10000
