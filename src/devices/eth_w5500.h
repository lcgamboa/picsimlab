/* ########################################################################

   PICsimLab - PIC laboratory simulator

   ########################################################################

   Copyright (c) : 2010-2020  Luis Claudio Gambôa Lopes

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

   For e-mail suggestions :  lcgamboa@yahoo.com
   ######################################################################## */


#ifndef ETH_W5500
#define	ETH_W5500

#include"bitbang_spi.h"

//BSB
#define B_COMMON 0x00 //00000 Selects Common Register.
#define B_SCK0RG 0x01 //00001 Selects Socket 0 Register
#define B_SCK0TX 0x02 //00010 Selects Socket 0 TX Buffer
#define B_SCK0RX 0x03 //00011 Selects Socket 0 RX Buffer
//#define B_RESERV 0x04 //00100 Reserved
#define B_SCK1RG 0x05 //00101 Selects Socket 1 Register
#define B_SCK1TX 0x06 //00110 Selects Socket 1 TX Buffer
#define B_SCK1RX 0x07 //00111 Selects Socket 1 RX Buffer
//#define B_RESERV 0x08 //01000 Reserved
#define B_SCK2RG 0x09 //01001 Selects Socket 2 Register
#define B_SCK2TX 0x0A //01010 Selects Socket 2 TX Buffer
#define B_SCK2RX 0x0B //01011 Selects Socket 2 RX Buffer
//#define B_RESERV 0x0C //01100 Reserved
#define B_SCK3RG 0x0D //01101 Selects Socket 3 Register
#define B_SCK3TX 0x0E //01110 Selects Socket 3 TX Buffer
#define B_SCK3RX 0x0F //01111 Selects Socket 3 RX Buffer
//#define B_RESERV 0x10 //10000 Reserved
#define B_SCK4RG 0x11 //10001 Selects Socket 4 Register
#define B_SCK4TX 0x12 //10010 Selects Socket 4 TX Buffer
#define B_SCK4RX 0x13 //10011 Selects Socket 4 RX Buffer
//#define B_RESERV 0x14 //10100 Reserved
#define B_SCK5RG 0x15 //10101 Selects Socket 5 Register
#define B_SCK5TX 0x16 //10110 Selects Socket 5 TX Buffer
#define B_SCK5RX 0x17 //10111 Selects Socket 5 RX Buffer
//#define B_RESERV 0x18 //11000 Reserved
#define B_SCK6RG 0x19 //11001 Selects Socket 6 Register
#define B_SCK6TX 0x1A //11010 Selects Socket 6 TX Buffer
#define B_SCK6RX 0x1B //11011 Selects Socket 6 RX Buffer
#define B_RESERV 0x1C //11100 Reserved
#define B_SCK7RG 0x1D //11101 Selects Socket 7 Register
#define B_SCK7TX 0x1E //11110 Selects Socket 7 TX Buffer
#define B_SCK7RX 0x1F //11111 Selects Socket 7 RX Buffer

//Common Register
#define CR_MR    0x0000 //(MR) Mode
#define CR_GAR0  0x0001 //(GAR0) Gateway Address
#define CR_GAR1  0x0002 //(GAR1)
#define CR_GAR2  0x0003 //(GAR2)
#define CR_GAR3  0x0004 //(GAR3)
#define CR_SUBR0 0x0005 //(SUBR0) Subnet Mask Address
#define CR_SUBR1 0x0006 //(SUBR1)
#define CR_SUBR2 0x0007 //(SUBR2)
#define CR_SUBR3 0x0008 //(SUBR3)
#define CR_SHAR0 0x0009 //(SHAR0) Source Hardware Address
#define CR_SHAR1 0x000A //(SHAR1)
#define CR_SHAR2 0x000B //(SHAR2)
#define CR_SHAR3 0x000C //(SHAR3)
#define CR_SHAR4 0x000D //(SHAR4)
#define CR_SHAR5 0x000E //(SHAR5)
#define CR_SIPR0 0x000F //(SIPR0) Source IP Address
#define CR_SIPR1 0x0010 //(SIPR1)
#define CR_SIPR2 0x0011 //(SIPR2)
#define CR_SIPR3 0x0012 //(SIPR3)
#define CR_INTLEVEL0 0x0013 //(INTLEVEL0) Interrupt Low Level Timer
#define CR_INTLEVEL1 0x0014 //(INTLEVEL1)
#define CR_IR    0x0015 //(IR) Interrupt
#define CR_IMR   0x0016 //(IMR) Interrupt Mask
#define CR_SIR   0x0017 //(SIR) Socket Interrupt
#define CR_SIMR  0x0018 //(SIMR) Socket Interrupt Mask
#define CR_RTR0  0x0019 //(RTR0) Retry Time
#define CR_RTR1  0x001A //(RTR1)
#define CR_RCR   0x001B //(RCR) Retry Count
#define CR_PTIMER 0x001C //(PTIMER) PPP LCP Request Timer
#define CR_PMAGIC 0x001D //(PMAGIC) PPP LCP Magic number
#define CR_PHAR0 0x001E //(PHAR0) PPP Destination MAC Address
#define CR_PHAR1 0x001F //(PHAR1)
#define CR_PHAR2 0x0020 //(PHAR2)
#define CR_PHAR3 0x0021 //(PHAR3)
#define CR_PHAR4 0x0022 //(PHAR4)
#define CR_PHAR5 0x0023 //(PHAR5)
#define CR_PSID0 0x0024 //(PSID0) PPP Session Identification
#define CR_PSID1 0x0025 //(PSID1)
#define CR_PMRU0 0x0026 //(PMRU0) PPP Maximum Segment Size
#define CR_PMRU1 0x0027 //(PMRU1)
#define CR_UIPR0 0x0028 //(UIPR0) Unreachable IP address
#define CR_UIPR1 0x0029 //(UIPR1)
#define CR_UIPR2 0x002A //(UIPR2)
#define CR_UIPR3 0x002B //(UIPR3)
#define CR_UPORTR0 0x002C //(UPORTR0) Unreachable Port
#define CR_UPORTR1 0x002D //(UPORTR1)
#define CR_PHYCFGR 0x002E //(PHYCFGR) PHY Configuration
//0x002F	~	Reserved
//0x0038	~
#define CR_VERSIONR 0x0039 //(VERSIONR) Chip version


//Socket Register Block
#define Sn_MR    0x0000 //(Sn_MR) Socket n Mode
#define Sn_CR    0x0001 //(Sn_CR) Socket n Command (Sn_CR)
#define Sn_IR    0x0002 //(Sn_IR) Socket n Interrupt
#define Sn_SR    0x0003 //(Sn_SR) Socket n Status
#define Sn_PORT0 0x0004 //(Sn_PORT0) Socket n Source Port
#define Sn_PORT1 0x0005 //(Sn_PORT1)
#define Sn_DHAR0 0x0006 //(Sn_DHAR0) Socket n Destination Hardware Address
#define Sn_DHAR1 0x0007 //(Sn_DHAR1)
#define Sn_DHAR2 0x0008 //(Sn_DHAR2)
#define Sn_DHAR3 0x0009 //(Sn_DHAR3)
#define Sn_DHAR4 0x000A //(Sn_DHAR4)
#define Sn_DHAR5 0x000B //(Sn_DHAR5)
#define Sn_DIPR0 0x000C //(Sn_DIPR0) Socket n Destination IP Address
#define Sn_DIPR1 0x000D //(Sn_DIPR1)
#define Sn_DIPR2 0x000E //(Sn_DIPR2)
#define Sn_DIPR3 0x000F //(Sn_DIPR3)
#define Sn_DPORT0 0x0010 //(Sn_DPORT0) Socket n Destination Port
#define Sn_DPORT1 0x0011 //(Sn_DPORT1)
#define Sn_MSSR0 0x0012 //(Sn_MSSR0) 	Socket n Maximum Segment Size
#define Sn_MSSR1 0x0013 //(Sn_MSSR1)
//0x0014	Reserved
#define Sn_TOS   0x0015 //(Sn_TOS)  Socket n IP TOS
#define Sn_TTL   0x0016 //(Sn_TTL) Socket n IP TTL
//0x0017	~	Reserved
//0x001D
#define Sn_RXBUF_SIZE 0x001E //(Sn_RXBUF_SIZE)  Socket n Receive Buffer Size
#define Sn_TXBUF_SIZE 0x001F //(Sn_TXBUF_SIZE) Socket n Transmit Buffer Size
#define Sn_TX_FSR0 0x0020 //(Sn_TX_FSR0) Socket n TX Free Size
#define Sn_TX_FSR1 0x0021 //(Sn_TX_FSR1)
#define Sn_TX_RD0 0x0022 //(Sn_TX_RD0) Socket n TX Read Pointer
#define Sn_TX_RD1 0x0023 //(Sn_TX_RD1)
#define Sn_TX_WR0 0x0024 //(Sn_TX_WR0) Socket n TX WritePointer
#define Sn_TX_WR1 0x0025 //(Sn_TX_WR1)
#define Sn_RX_RSR0 0x0026 //(Sn_RX_RSR0) Socket n RX Received Size
#define Sn_RX_RSR1 0x0027 //(Sn_RX_RSR1)
#define Sn_RX_RD0 0x0028 //(Sn_RX_RD0) Socket n RX Read Pointer
#define Sn_RX_RD1 0x0029 //(Sn_RX_RD1)
#define Sn_RX_WR0 0x002A //(Sn_RX_WR0) Socket n RX Write Pointer
#define Sn_RX_WR1 0x002B //(Sn_RX_WR1)
#define Sn_IMR    0x002C //(Sn_IMR) Socket n Interrupt Mask
#define Sn_FRAG0  0x002D //(Sn_FRAG0)  Socket n Fragment Offset in IP header
#define Sn_FRAG1  0x002E //(Sn_FRAG1)
#define Sn_KPALVTR 0x002F //(Sn_KPALVTR) Keep alive timer
//0x0030 ~	Reserved
//0xFFFF


#define Sn_MR_CLOSE  0x00
#define Sn_MR_TCP    0x01
#define Sn_MR_UDP    0x02
#define S0_MR_MACRAW 0x04

#define OPEN      0x01
#define LISTEN    0x02
#define CONNECT   0x04
#define DISCON    0x08
#define CLOSE     0x10
#define SEND      0x20
#define SEND_MAC  0x21
#define SEND_KEEP 0x22
#define RECV      0x40


#define SOCK_CLOSED 0x00
#define SOCK_INIT 0x13
#define SOCK_LISTEN 0x14
#define SOCK_SYNSENT 0x15
#define SOCK_ESTABLISHED 0x17
#define SOCK_CLOSE_WAIT 0x1C
#define SOCK_UDP 0x22
#define SOCK_MACRAW 0x42


#define ER_BIND 1
#define ER_SEND 2
#define ER_RECV 3
#define ER_LIST 4
#define ER_REUSE 5
#define ER_CONN 6
#define ER_SHUT 7


typedef struct
{
unsigned char link;     
unsigned char active;    
bitbang_spi_t bb_spi;
unsigned short addr;
unsigned char control;
unsigned char Common[0x40]; 
unsigned char Socket[8][0x30]; 
int sockfd[8];
int listenfd[8];
unsigned short listenfd_port[8];
unsigned char listenfd_map[8];
unsigned char RX_Mem[0x4000];
unsigned char TX_Mem[0x4000];
unsigned short RX_ptr[8];
unsigned short TX_ptr[8];
unsigned short RX_size[8];
unsigned short TX_size[8];
unsigned short RX_mask[8];
unsigned short TX_mask[8];
unsigned char  status[8];
unsigned short  bindp[8];
}eth_w5500_t;


void eth_w5500_rst(eth_w5500_t *eth);
void eth_w5500_init(eth_w5500_t *eth,unsigned char linkon=1);
void eth_w5500_process(eth_w5500_t *eth);
void eth_w5500_end(eth_w5500_t *eth);
void eth_w5500_set_link(eth_w5500_t *eth, unsigned char on);
unsigned char eth_w5500_get_leds(eth_w5500_t *eth);


unsigned short eth_w5500_io(eth_w5500_t *eth, unsigned char mosi, unsigned char sclk,unsigned char scs, unsigned char rst);

#endif //ETH_W5500

