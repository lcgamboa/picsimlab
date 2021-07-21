/* ########################################################################

   PICsimLab - PIC laboratory simulator

   ########################################################################

   Copyright (c) : 2010-2021  Luis Claudio Gambôa Lopes

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


#include"eth_w5500.h"

#ifndef _WIN_
#include<sys/types.h>
#include<sys/socket.h>
#include<sys/un.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#else
#include<winsock2.h>
#include<ws2tcpip.h>
typedef unsigned char u_int8_t;
typedef unsigned short u_int16_t;
typedef unsigned int u_int32_t;
#define SHUT_RDWR SD_BOTH
#define MSG_NOSIGNAL 0
#endif
#define INVALID_SOCKET_VALUE -1

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>


//#define DEBUG
//#define DUMP
//#define TEXTDUMP

#define dprintf if (1) {} else printf
#define dsprintf if (1) {} else printf

#define BSB ((eth->control & 0xF8) >>3)
#define RWB ((eth->control & 0x04) >>2)
#define OM  b(eth->control & 0x03)

#define readWord(x,addr)  ((x[addr] << 8) | x[addr+1])
#define writeWord(x,addr,val)  x[addr] = (val) >> 8;x[addr+1] = (val)& 0x00FF

void setblock(int sock_descriptor);
void setnblock(int sock_descriptor);

#define CONN_TIMEOUT 100000L //10 seconds

#ifdef DEBUG

static const char *
debug_BCOMMON(eth_w5500_t *eth)
{
 switch (eth->addr)
  {
  case CR_MR: return "B_COMMON   CR_MR";
   break;
  case CR_GAR0: return "B_COMMON   CR_GAR0";
   break;
  case CR_GAR1: return "B_COMMON   CR_GAR1";
   break;
  case CR_GAR2: return "B_COMMON   CR_GAR2";
   break;
  case CR_GAR3: return "B_COMMON   CR_GAR3";
   break;
  case CR_SUBR0: return "B_COMMON   CR_SUBR0";
   break;
  case CR_SUBR1: return "B_COMMON   CR_SUBR1";
   break;
  case CR_SUBR2: return "B_COMMON   CR_SUBR2";
   break;
  case CR_SUBR3: return "B_COMMON   CR_SUBR3";
   break;
  case CR_SHAR0: return "B_COMMON   CR_SHAR0";
   break;
  case CR_SHAR1: return "B_COMMON   CR_SHAR1";
   break;
  case CR_SHAR2: return "B_COMMON   CR_SHAR2";
   break;
  case CR_SHAR3: return "B_COMMON   CR_SHAR3";
   break;
  case CR_SHAR4: return "B_COMMON   CR_SHAR4";
   break;
  case CR_SHAR5: return "B_COMMON   CR_SHAR5";
   break;
  case CR_SIPR0: return "B_COMMON   CR_SIPR0";
   break;
  case CR_SIPR1: return "B_COMMON   CR_SIPR1";
   break;
  case CR_SIPR2: return "B_COMMON   CR_SIPR2";
   break;
  case CR_SIPR3: return "B_COMMON   CR_SIPR3";
   break;
  case CR_INTLEVEL0: return "B_COMMON   CR_INTLEVEL0";
   break;
  case CR_INTLEVEL1: return "B_COMMON   CR_INTLEVEL1";
   break;
  case CR_IR: return "B_COMMON   CR_IR";
   break;
  case CR_IMR: return "B_COMMON   CR_IMR";
   break;
  case CR_SIR: return "B_COMMON   CR_SIR";
   break;
  case CR_SIMR: return "B_COMMON   CR_SIMR";
   break;
  case CR_RTR0: return "B_COMMON   CR_RTR0";
   break;
  case CR_RTR1: return "B_COMMON   CR_RTR1";
   break;
  case CR_RCR: return "B_COMMON   CR_RCR";
   break;
  case CR_PTIMER: return "B_COMMON   CR_PTIMER";
   break;
  case CR_PMAGIC: return "B_COMMON   CR_PMAGIC";
   break;
  case CR_PHAR0: return "B_COMMON   CR_PHAR0";
   break;
  case CR_PHAR1: return "B_COMMON   CR_PHAR1";
   break;
  case CR_PHAR2: return "B_COMMON   CR_PHAR2";
   break;
  case CR_PHAR3: return "B_COMMON   CR_PHAR3";
   break;
  case CR_PHAR4: return "B_COMMON   CR_PHAR4";
   break;
  case CR_PHAR5: return "B_COMMON   CR_PHAR5";
   break;
  case CR_PSID0: return "B_COMMON   CR_PSID0";
   break;
  case CR_PSID1: return "B_COMMON   CR_PSID1";
   break;
  case CR_PMRU0: return "B_COMMON   CR_PMRU0";
   break;
  case CR_PMRU1: return "B_COMMON   CR_PMRU1";
   break;
  case CR_UIPR0: return "B_COMMON   CR_UIPR0";
   break;
  case CR_UIPR1: return "B_COMMON   CR_UIPR1";
   break;
  case CR_UIPR2: return "B_COMMON   CR_UIPR2";
   break;
  case CR_UIPR3: return "B_COMMON   CR_UIPR3";
   break;
  case CR_UPORTR0: return "B_COMMON   CR_UPORTR0";
   break;
  case CR_UPORTR1: return "B_COMMON   CR_UPORTR1";
   break;
  case CR_PHYCFGR: return "B_COMMON   CR_PHYCFGR";
   break;
  case CR_VERSIONR: return "B_COMMON   CR_VERSIONR";
   break;
  default: return "B_COMMON   ERROR";
   break;
  }
}

static const char *
debug_B_SCKnRG(unsigned char n, eth_w5500_t *eth)
{
 static char buff[100];
 sprintf (buff, "B_SCK%iRG    ", n);

 switch (eth->addr)
  {
  case Sn_MR: strcat (buff, "Sn_MR");
   break;
  case Sn_CR: strcat (buff, "Sn_CR");
   break;
  case Sn_IR: strcat (buff, "Sn_IR");
   break;
  case Sn_SR: strcat (buff, "Sn_SR");
   break;
  case Sn_PORT0: strcat (buff, "Sn_PORT0");
   break;
  case Sn_PORT1: strcat (buff, "Sn_PORT1");
   break;
  case Sn_DHAR0: strcat (buff, "Sn_DHAR0");
   break;
  case Sn_DHAR1: strcat (buff, "Sn_DHAR1");
   break;
  case Sn_DHAR2: strcat (buff, "Sn_DHAR2");
   break;
  case Sn_DHAR3: strcat (buff, "Sn_DHAR3");
   break;
  case Sn_DHAR4: strcat (buff, "Sn_DHAR4");
   break;
  case Sn_DHAR5: strcat (buff, "Sn_DHAR5");
   break;
  case Sn_DIPR0: strcat (buff, "Sn_DIPR0");
   break;
  case Sn_DIPR1: strcat (buff, "Sn_DIPR1");
   break;
  case Sn_DIPR2: strcat (buff, "Sn_DIPR2");
   break;
  case Sn_DIPR3: strcat (buff, "Sn_DIPR3");
   break;
  case Sn_DPORT0: strcat (buff, "Sn_DPORT0");
   break;
  case Sn_DPORT1: strcat (buff, "Sn_DPORT1");
   break;
  case Sn_MSSR0: strcat (buff, "Sn_MSSR0");
   break;
  case Sn_MSSR1: strcat (buff, "Sn_MSSR1");
   break;
  case Sn_TOS: strcat (buff, "Sn_TOS");
   break;
  case Sn_TTL: strcat (buff, "Sn_TTL");
   break;
  case Sn_RXBUF_SIZE: strcat (buff, "Sn_RXBUF_SIZE");
   break;
  case Sn_TXBUF_SIZE: strcat (buff, "Sn_TXBUF_SIZE");
   break;
  case Sn_TX_FSR0: strcat (buff, "Sn_TX_FSR0");
   break;
  case Sn_TX_FSR1: strcat (buff, "Sn_TX_FSR1");
   break;
  case Sn_TX_RD0: strcat (buff, "Sn_TX_RD0");
   break;
  case Sn_TX_RD1: strcat (buff, "Sn_TX_RD1");
   break;
  case Sn_TX_WR0: strcat (buff, "Sn_TX_WR0");
   break;
  case Sn_TX_WR1: strcat (buff, "Sn_TX_WR1");
   break;
  case Sn_RX_RSR0: strcat (buff, "Sn_RX_RSR0");
   break;
  case Sn_RX_RSR1: strcat (buff, "Sn_RX_RSR1");
   break;
  case Sn_RX_RD0: strcat (buff, "Sn_RX_RD0");
   break;
  case Sn_RX_RD1: strcat (buff, "Sn_RX_RD1");
   break;
  case Sn_RX_WR0: strcat (buff, "Sn_RX_WR0");
   break;
  case Sn_RX_WR1: strcat (buff, "Sn_RX_WR1");
   break;
  case Sn_IMR: strcat (buff, "Sn_IMR");
   break;
  case Sn_FRAG0: strcat (buff, "Sn_FRAG0");
   break;
  case Sn_FRAG1: strcat (buff, "Sn_FRAG1");
   break;
  case Sn_KPALVTR: strcat (buff, "Sn_KPALVTR");
   break;
  default: strcat (buff, "ERROR");
   break;
  }

 return buff;
}

static const char *
debug_BSB(eth_w5500_t *eth)
{
 switch ((eth->control & 0xF8) >> 3)
  {
  case B_COMMON: return debug_BCOMMON (eth);
   break;
  case B_SCK0RG: return debug_B_SCKnRG (0, eth);
   break;
  case B_SCK0TX: return "B_SCK0TX";
   break;
  case B_SCK0RX: return "B_SCK0RX";
   break;
   //case B_RESERV: return "B_RESERV";break;
  case B_SCK1RG: return debug_B_SCKnRG (1, eth);
   break;
  case B_SCK1TX: return "B_SCK1TX";
   break;
  case B_SCK1RX: return "B_SCK1RX";
   break;
   //case B_RESERV: return "B_RESERV";break;
  case B_SCK2RG: return debug_B_SCKnRG (2, eth);
   break;
  case B_SCK2TX: return "B_SCK2TX";
   break;
  case B_SCK2RX: return "B_SCK2RX";
   break;
   //case B_RESERV: return "B_RESERV";break;
  case B_SCK3RG: return debug_B_SCKnRG (3, eth);
   break;
  case B_SCK3TX: return "B_SCK3TX";
   break;
  case B_SCK3RX: return "B_SCK3RX";
   break;
   //case B_RESERV: return "B_RESERV";break;
  case B_SCK4RG: return debug_B_SCKnRG (4, eth);
   break;
  case B_SCK4TX: return "B_SCK4TX";
   break;
  case B_SCK4RX: return "B_SCK4RX";
   break;
   //case B_RESERV: return "B_RESERV";break;
  case B_SCK5RG: return debug_B_SCKnRG (5, eth);
   break;
  case B_SCK5TX: return "B_SCK5TX";
   break;
  case B_SCK5RX: return "B_SCK5RX";
   break;
   //case B_RESERV: return "B_RESERV";break;
  case B_SCK6RG: return debug_B_SCKnRG (6, eth);
   break;
  case B_SCK6TX: return "B_SCK6TX";
   break;
  case B_SCK6RX: return "B_SCK6RX";
   break;
   //case B_RESERV: return "B_RESERV;break;
  case B_SCK7RG: return debug_B_SCKnRG (7, eth);
   break;
  case B_SCK7TX: return "B_SCK7TX";
   break;
  case B_SCK7RX: return "B_SCK7RX";
   break;
  default: return "ERROR";
   break;
  }
}
#endif

void
eth_w5500_rst(eth_w5500_t *eth)
{
 bitbang_spi_rst (&eth->bb_spi);
 eth->control = 0;
 eth->addr = 0;
 eth->active = 0;
 memset (eth->Common, 0, 0x40);
 memset (eth->Socket, 0, 0x30 * 8);
 memset (eth->status, 0, 8);
 memset (eth->bindp, 0, 8 * 2);

 eth->Common[CR_VERSIONR] = 0x04; //version
 if (eth->link)
  {
   eth->Common[CR_PHYCFGR] |= 0x01; //link on
  }
 else
  {
   eth->Common[CR_PHYCFGR] &= ~0x01; //link off 
  }
 //TODO add support to buffer size different of 2k 
 for (int n = 0; n < 8; n++)
  {
   eth->sockfd[n] = INVALID_SOCKET_VALUE;
   eth->Socket[n][Sn_TXBUF_SIZE] = 2;
   eth->Socket[n][Sn_RXBUF_SIZE] = 2;

   eth->RX_size[n] = eth->Socket[n][Sn_RXBUF_SIZE] * 1024;
   eth->TX_size[n] = eth->Socket[n][Sn_TXBUF_SIZE] * 1024;
   eth->RX_mask[n] = eth->RX_size[n] - 1;
   eth->TX_mask[n] = eth->TX_size[n] - 1;

   writeWord (eth->Socket[n], Sn_TX_FSR0, eth->TX_size[n]);

   eth->RX_ptr[n] = eth->RX_size[n] * n;
   eth->TX_ptr[n] = eth->TX_size[n] * n;
   eth->listenfd_map[n] = 0;
  }
 dprintf ("rst w5500\n");
}

void
eth_w5500_init(eth_w5500_t *eth, unsigned char linkon)
{
 eth->link = linkon;
 bitbang_spi_init (&eth->bb_spi);
 eth_w5500_rst (eth);

 for (int n = 0; n < 8; n++)
  {
   eth->listenfd[n] = INVALID_SOCKET_VALUE;
   eth->listenfd_port[n] = 0;
  }
 dprintf ("init w5500\n");
}

void
eth_w5500_end(eth_w5500_t *eth)
{
 for (int n = 0; n < 8; n++)
  {
   if (eth->sockfd[n] != INVALID_SOCKET_VALUE)
    {
     close (eth->sockfd[n]);
    }

   if (eth->listenfd[n] != INVALID_SOCKET_VALUE)
    {
     close (eth->listenfd[n]);
    }
  }
}

void
eth_w5500_set_link(eth_w5500_t *eth, unsigned char on)
{
 eth->link = on;
 if (on)
  {
   eth->Common[CR_PHYCFGR] |= 0x01;
  }
 else
  {
   eth->Common[CR_PHYCFGR] &= ~0x01;
  }
}

unsigned char
eth_w5500_get_leds(eth_w5500_t *eth)
{
 return (eth->Common[CR_PHYCFGR] & 0x03) | ((eth->active > 0) << 3);
}



#ifdef DUMP
static int scont = 0;
#endif


// http://www.tcpipguide.com/free/t_DHCPMessageFormat.htm

typedef struct dhcp
{
 //fixed 240 bytes
 u_int8_t opcode; //Operation Code
 u_int8_t htype; //Hardware Type
 u_int8_t hlen; //Hardware Address Length
 u_int8_t hops; //Hops
 u_int32_t xid; //Transaction Identifier
 u_int16_t secs; //Seconds
 u_int16_t flags;
 u_int8_t ciaddr[4]; //Client IP Address
 u_int8_t yiaddr[4]; //Your IP Address
 u_int8_t siaddr[4]; //Server IP Address
 u_int8_t giaddr[4]; //Gateway IP Address
 u_int8_t chaddr[16]; //Client Hardware Address
 char bp_sname[64]; //Server Name
 char bp_file[128]; //Boot Filename
 u_int32_t magic_cookie;
 //variable 
 u_int8_t bp_options[35]; //Options
} dhcp_t;

/* DHCP message type */
#define DHCP_DISCOVER  1
#define DHCP_OFFER    2
#define DHCP_REQUEST  3
#define DHCP_DECLINE  4
#define DHCP_ACK      5
#define DHCP_NAK      6
#define DHCP_RELEASE  7
#define DHCP_INFORM   8


#define dhcppadOption  0
#define dhcpsubnetMask  1
#define dhcptimerOffset  2
#define dhcproutersOnSubnet 3
#define dhcpdns 6
#define dhcphostName 12
#define dhcpdomainName 15
#define dhcpRequestedIPaddr 50
#define dhcpIPaddrLeaseTime  51
#define dhcpMessageType  53
#define dhcpServerIdentifier  54
#define dhcpParamRequest 55
#define dhcpT1value  58
#define dhcpT2value 59
#define dhcpClientIdentifier 61
#define dhcpendOption  255

static void
eth_w5500_fake_dhcp_reply(eth_w5500_t *eth, int n)
{
 char temp_buff[0x4000];
 char skt_addr[30];
 unsigned short skt_port;
 dhcp_t * dhcp_pack = (dhcp_t *) & eth->TX_Mem[eth->TX_ptr[n]];
 int size;
 unsigned short addr;
 unsigned short addr_base;
 int s;
 int i;
#ifdef DUMP
 char sfname[30];
 FILE * fouts;
#endif

 if (!eth->link)return;

 dhcp_pack->opcode = 2; //reply

 dhcp_pack->yiaddr[0] = 192;
 dhcp_pack->yiaddr[1] = 168;
 dhcp_pack->yiaddr[2] = 0;
 dhcp_pack->yiaddr[3] = 10;

 dhcp_pack->siaddr[0] = 192;
 dhcp_pack->siaddr[1] = 168;
 dhcp_pack->siaddr[2] = 0;
 dhcp_pack->siaddr[3] = 1;

 dhcp_pack->giaddr[0] = 192;
 dhcp_pack->giaddr[1] = 168;
 dhcp_pack->giaddr[2] = 0;
 dhcp_pack->giaddr[3] = 1;

 int op = 0;
 dhcp_pack->bp_options[op++] = dhcpMessageType;
 dhcp_pack->bp_options[op++] = 0x01;

 switch (dhcp_pack->bp_options[2])
  {
  case DHCP_DISCOVER:
   dhcp_pack->bp_options[op++] = DHCP_OFFER;
   break;
  case DHCP_REQUEST:
   dhcp_pack->bp_options[op++] = DHCP_ACK;
   break;
  }

 dhcp_pack->bp_options[op++] = dhcpsubnetMask;
 dhcp_pack->bp_options[op++] = 4;
 dhcp_pack->bp_options[op++] = 255;
 dhcp_pack->bp_options[op++] = 255;
 dhcp_pack->bp_options[op++] = 255;
 dhcp_pack->bp_options[op++] = 0;

 dhcp_pack->bp_options[op++] = dhcproutersOnSubnet;
 dhcp_pack->bp_options[op++] = 4;
 dhcp_pack->bp_options[op++] = 192;
 dhcp_pack->bp_options[op++] = 168;
 dhcp_pack->bp_options[op++] = 0;
 dhcp_pack->bp_options[op++] = 1;

 dhcp_pack->bp_options[op++] = dhcpdns; //need to be real to work
 dhcp_pack->bp_options[op++] = 4;
 dhcp_pack->bp_options[op++] = 8;
 dhcp_pack->bp_options[op++] = 8;
 dhcp_pack->bp_options[op++] = 8;
 dhcp_pack->bp_options[op++] = 8;
 /*
    dhcp_pack->bp_options[op++] = dhcpdomainName;
    dhcp_pack->bp_options[op++] = 15;
    dhcp_pack->bp_options[op++] = 'P';
    dhcp_pack->bp_options[op++] = 'I';
    dhcp_pack->bp_options[op++] = 'C';
    dhcp_pack->bp_options[op++] = 'S';
    dhcp_pack->bp_options[op++] = 'i';
    dhcp_pack->bp_options[op++] = 'm';
    dhcp_pack->bp_options[op++] = 'L';
    dhcp_pack->bp_options[op++] = 'a';
    dhcp_pack->bp_options[op++] = 'b';
    dhcp_pack->bp_options[op++] = 0x12;
    dhcp_pack->bp_options[op++] = 0x23;
    dhcp_pack->bp_options[op++] = 0x34;
    dhcp_pack->bp_options[op++] = 0x45;
    dhcp_pack->bp_options[op++] = 0x56;
    dhcp_pack->bp_options[op++] = 0x67;
  */

 dhcp_pack->bp_options[op++] = dhcpT1value;
 dhcp_pack->bp_options[op++] = 4;
 dhcp_pack->bp_options[op++] = 0x00;
 dhcp_pack->bp_options[op++] = 0x00;
 dhcp_pack->bp_options[op++] = 0x03;
 dhcp_pack->bp_options[op++] = 0x84;

 dhcp_pack->bp_options[op++] = dhcpT2value;
 dhcp_pack->bp_options[op++] = 4;
 dhcp_pack->bp_options[op++] = 0x00;
 dhcp_pack->bp_options[op++] = 0x00;
 dhcp_pack->bp_options[op++] = 0x03;
 dhcp_pack->bp_options[op++] = 0x84;

 dhcp_pack->bp_options[op++] = dhcpendOption;

 //reply fake receive
 sprintf (skt_addr, "%i.%i.%i.%i", eth->Socket[n][Sn_DIPR0], eth->Socket[n][Sn_DIPR1], eth->Socket[n][Sn_DIPR2], eth->Socket[n][Sn_DIPR3]);
 skt_port = readWord (eth->Socket[n], Sn_DPORT0);

 eth->active = 2;

 s = 240 + op;
 memcpy (temp_buff + 8, dhcp_pack, s);

 dsprintf ("Socket %i Received %i from %s:%i\n", n, s, skt_addr, skt_port);

 temp_buff[0] = eth->Socket[n][Sn_DIPR0];
 temp_buff[1] = eth->Socket[n][Sn_DIPR1];
 temp_buff[2] = eth->Socket[n][Sn_DIPR2];
 temp_buff[3] = eth->Socket[n][Sn_DIPR3];
 temp_buff[4] = eth->Socket[n][Sn_DPORT0];
 temp_buff[5] = eth->Socket[n][Sn_DPORT1];
 temp_buff[6] = s >> 8;
 temp_buff[7] = s & 0x00FF;

 s += 8; //add header size to total size

 addr_base = (readWord (eth->Socket[n], Sn_RX_WR0));

 for (i = 0; i < s; i++)
  {
   addr = (addr_base + i) & eth->RX_mask[n];
   eth->RX_Mem[eth->RX_ptr[n] + addr ] = temp_buff[i];
  }

 writeWord (eth->Socket[n], Sn_RX_WR0, addr_base + s);

 size = readWord (eth->Socket[n], Sn_RX_WR0);
 size -= readWord (eth->Socket[n], Sn_RX_RD0);

 if (size < 0)
  {
   size += eth->RX_size[n];
  }
 writeWord (eth->Socket[n], Sn_RX_RSR0, size);

#ifdef DUMP
 sprintf (sfname, "/tmp/%03i_recv_udp.bin", scont++);
 fouts = fopen (sfname, "w");
 fwrite (temp_buff, s, 1, fouts);
 fclose (fouts);
#endif

#ifdef TEXTDUMP
 temp_buff[s] = 0;
 printf (temp_buff);
#endif

}

static unsigned int conn_timeout[8] = {0, 0, 0, 0, 0, 0, 0, 0};

void
eth_w5500_process(eth_w5500_t *eth)
{

 char temp_buff[0x4000];
 char skt_addr[30];
 unsigned short skt_port;
 struct sockaddr_in cli;
 socklen_t clilen = sizeof (cli);
 int size;
 unsigned short addr;
 unsigned short addr_base;
 int s;
 int i;
 int j;
 socklen_t len;
 struct sockaddr_in serv;
#ifdef DUMP
 char sfname[30];
 FILE * fouts;
#endif

 if (eth->active)eth->active--;

 if (!eth->link)return;

 for (int n = 0; n < 8; n++)
  {
   switch (eth->Socket[n][Sn_SR])
    {
    case SOCK_SYNSENT:
     sprintf (skt_addr, "%i.%i.%i.%i", eth->Socket[n][Sn_DIPR0], eth->Socket[n][Sn_DIPR1], eth->Socket[n][Sn_DIPR2], eth->Socket[n][Sn_DIPR3]);
     skt_port = readWord (eth->Socket[n], Sn_DPORT0);

     memset (&serv, 0, sizeof (serv));
     serv.sin_family = AF_INET;
     serv.sin_addr.s_addr = inet_addr (skt_addr);
     serv.sin_port = htons (skt_port);

     if (connect (eth->sockfd[n], (sockaddr *) & serv, sizeof (serv)) < 0)
      {
#ifndef _WIN_         
       if ((errno == EINPROGRESS) || (errno == EALREADY))
#else
       if ((WSAGetLastError () == WSAEINPROGRESS) || (WSAGetLastError () == WSAEALREADY) ||
           (WSAGetLastError () == WSAEWOULDBLOCK))
#endif          
        {
         conn_timeout[n]++;
         if (conn_timeout[n] < CONN_TIMEOUT)
          {
           break;
          }
        }
#ifdef _WIN_         
       if (WSAGetLastError () != WSAEISCONN)
        {
#endif
         printf ("eth_w5500: connect error : %s \n", strerror (errno));
         eth->Socket[n][Sn_SR] = SOCK_CLOSED;
         eth->status[n] = ER_CONN;
         break;
#ifdef _WIN_         
        }
#endif       
      }

     dsprintf ("Socket %i Connected to %s:%i\n", n, skt_addr, skt_port);
     eth->Socket[n][Sn_SR] = SOCK_ESTABLISHED;
     eth->Socket[n][Sn_IR] |= 0x01;

     writeWord (eth->Socket[n], Sn_TX_RD0, 0);
     writeWord (eth->Socket[n], Sn_TX_WR0, 0);
     writeWord (eth->Socket[n], Sn_RX_RD0, 0);
     writeWord (eth->Socket[n], Sn_RX_WR0, 0);
     writeWord (eth->Socket[n], Sn_TX_FSR0, eth->Socket[n][Sn_TXBUF_SIZE] * 1024);
     writeWord (eth->Socket[n], Sn_RX_RSR0, 0);
     setnblock (eth->sockfd[n]);
     break;
    case SOCK_LISTEN:
     if ((eth->sockfd[n] = accept (eth->listenfd[eth->listenfd_map[n]], (sockaddr *) & cli, & clilen)) < 0)
      {
       //printf ("eth_w5500: accept error : %s \n", strerror (errno));
       eth->sockfd[n] = INVALID_SOCKET_VALUE;
      }
     else
      {
       setnblock (eth->sockfd[n]);
       eth->Socket[n][Sn_SR] = SOCK_ESTABLISHED;
       dsprintf ("Socket %i Connected\n", n);
       writeWord (eth->Socket[n], Sn_TX_RD0, 0);
       writeWord (eth->Socket[n], Sn_TX_WR0, 0);
       writeWord (eth->Socket[n], Sn_RX_RD0, 0);
       writeWord (eth->Socket[n], Sn_RX_WR0, 0);
       writeWord (eth->Socket[n], Sn_TX_FSR0, eth->Socket[n][Sn_TXBUF_SIZE] * 1024);
       writeWord (eth->Socket[n], Sn_RX_RSR0, 0);
       eth->bindp[n] = 0;
      }
     break;
    case SOCK_ESTABLISHED:

     size = eth->RX_size[n] - readWord (eth->Socket[n], Sn_RX_RSR0);
     if (size < 0) size = 0;

     if (size)
      {
       if ((s = recv (eth->sockfd[n], temp_buff, size, 0)) < 0)
        {
#ifndef _WIN_         
         if (errno != EAGAIN)
#else
         if (WSAGetLastError () != WSAEWOULDBLOCK)
#endif          
          {
           printf ("eth_w5500: recv tcp error :[%i] %s \n", errno, strerror (errno));
           eth->Socket[n][Sn_SR] = SOCK_CLOSED;
           eth->status[n] = ER_RECV;
          }
        }
       if (s > 0)
        {
         eth->active = 2;
         eth->Socket[n][Sn_IR] |= 0x04;

         addr_base = (readWord (eth->Socket[n], Sn_RX_WR0));

         for (i = 0; i < s; i++)
          {
           addr = (addr_base + i) & eth->RX_mask[n];
           eth->RX_Mem[eth->RX_ptr[n] + addr ] = temp_buff[i];
          }

         dsprintf ("eth_w5500: Socket %i Received %i\n", n, s);
         writeWord (eth->Socket[n], Sn_RX_WR0, readWord (eth->Socket[n], Sn_RX_WR0) + s);

         size = readWord (eth->Socket[n], Sn_RX_WR0);
         size -= readWord (eth->Socket[n], Sn_RX_RD0);



         if (size < 0)
          {
           size += eth->RX_size[n];
          }

         writeWord (eth->Socket[n], Sn_RX_RSR0, size);


#ifdef DUMP
         sprintf (sfname, "/tmp/%03i_recv_tcp.bin", scont++);
         fouts = fopen (sfname, "w");
         fwrite (temp_buff, s, 1, fouts);
         fclose (fouts);
#endif
#ifdef TEXTDUMP
         temp_buff[s] = 0;
         printf (temp_buff);
#endif         
        }
      }
     break;
    case SOCK_UDP:

     sprintf (skt_addr, "%i.%i.%i.%i", eth->Socket[n][Sn_DIPR0], eth->Socket[n][Sn_DIPR1], eth->Socket[n][Sn_DIPR2], eth->Socket[n][Sn_DIPR3]);
     skt_port = readWord (eth->Socket[n], Sn_DPORT0);

     memset (&serv, 0, sizeof (serv));

     // Filling server information
     serv.sin_family = AF_INET;
     serv.sin_port = htons (skt_port);
     serv.sin_addr.s_addr = inet_addr (skt_addr);

     for (j = 0; j < 8; j++)
      {
       if ((eth->listenfd_port[j] == 0) || (eth->listenfd_port[j] == skt_port))
        {
         eth->listenfd_map[n] = j;
         eth->listenfd_port[j] = skt_port;
         break;
        }
      }


     if ((eth->listenfd[eth->listenfd_map[n]] == INVALID_SOCKET_VALUE) && !strcmp (skt_addr, "0.0.0.0") && (skt_port == 0))
      {
       skt_port = readWord (eth->Socket[n], Sn_PORT0);

       if (skt_port < 2000) //avoid system services
        {
         skt_port += 2000;
        }

       eth->bindp[n] = skt_port;
       memset (&serv, 0, sizeof (serv));

       // Filling server information
       serv.sin_family = AF_INET;
       serv.sin_port = htons (skt_port);
       serv.sin_addr.s_addr = htonl (INADDR_ANY);


       eth->listenfd[eth->listenfd_map[n]] = eth->sockfd[n];
       if (bind (eth->listenfd[eth->listenfd_map[n]], (sockaddr *) & serv, sizeof (serv)))
        {
         printf ("eth_w5500: bind error : %s \n", strerror (errno));
         close (eth->sockfd[n]);
         eth->sockfd[n] = INVALID_SOCKET_VALUE;
         eth->Socket[n][Sn_SR] = SOCK_CLOSED;
         eth->status[n] = ER_BIND;
         eth->bindp[n] = 0;
         eth->listenfd[eth->listenfd_map[n]] = INVALID_SOCKET_VALUE;
         eth->listenfd_port[eth->listenfd_map[n]] = 0;
         break;
        }
       dsprintf ("Socket %i listen on port %i \n", n, skt_port);
      }

     size = (eth->Socket[n][Sn_RXBUF_SIZE]*1024);
     if (size < 0) size = 0;

     if (size)
      {
       len = sizeof (serv);

       if ((s = recvfrom (eth->sockfd[n], &temp_buff[8], eth->RX_size[n] - readWord (eth->Socket[n], Sn_RX_RSR0),
                          0 /*MSG_DONTWAIT MSG_WAITALL*/, (struct sockaddr *) &serv, &len)) < 0)
        {
#ifndef _WIN_         
         if (errno != EAGAIN)
#else
         if ((WSAGetLastError () != WSAEWOULDBLOCK) && (WSAGetLastError () != WSAEINVAL))
#endif  
          {
           printf ("eth_w5500: recv udp error : %s \n", strerror (errno));
           eth->Socket[n][Sn_SR] = SOCK_CLOSED;
           eth->status[n] = ER_RECV;
          }
        }

       if (s > 0)
        {
         eth->active = 2;
         eth->Socket[n][Sn_IR] |= 0x04;

         strcpy (skt_addr, inet_ntoa (serv.sin_addr));
         skt_port = ntohs (serv.sin_port);

         eth->Socket[n][Sn_DIPR0] = (serv.sin_addr.s_addr & 0x000000FF);
         eth->Socket[n][Sn_DIPR1] = (serv.sin_addr.s_addr & 0x0000FF00) >> 8;
         eth->Socket[n][Sn_DIPR2] = (serv.sin_addr.s_addr & 0x00FF0000) >> 16;
         eth->Socket[n][Sn_DIPR3] = (serv.sin_addr.s_addr & 0xFF000000) >> 24;
         eth->Socket[n][Sn_DPORT0] = skt_port >> 8;
         eth->Socket[n][Sn_DPORT1] = skt_port & 0xFF;


         dsprintf ("eth_w5500: Socket %i Received %i from %s:%i\n", n, s, skt_addr, skt_port);

         temp_buff[0] = eth->Socket[n][Sn_DIPR0];
         temp_buff[1] = eth->Socket[n][Sn_DIPR1];
         temp_buff[2] = eth->Socket[n][Sn_DIPR2];
         temp_buff[3] = eth->Socket[n][Sn_DIPR3];
         temp_buff[4] = eth->Socket[n][Sn_DPORT0];
         temp_buff[5] = eth->Socket[n][Sn_DPORT1];
         temp_buff[6] = s >> 8;
         temp_buff[7] = s & 0x00FF;

         s += 8; //add header size to total size

         addr_base = (readWord (eth->Socket[n], Sn_RX_WR0));

         for (i = 0; i < s; i++)
          {
           addr = (addr_base + i) & eth->RX_mask[n];
           eth->RX_Mem[eth->RX_ptr[n] + addr ] = temp_buff[i];
          }

         writeWord (eth->Socket[n], Sn_RX_WR0, readWord (eth->Socket[n], Sn_RX_WR0) + s);

         size = readWord (eth->Socket[n], Sn_RX_WR0);
         size -= readWord (eth->Socket[n], Sn_RX_RD0);

         if (size < 0)
          {
           size += eth->RX_size[n];
          }
         writeWord (eth->Socket[n], Sn_RX_RSR0, size);

#ifdef DUMP
         sprintf (sfname, "/tmp/%03i_recv_udp.bin", scont++);
         fouts = fopen (sfname, "w");
         fwrite (temp_buff, s, 1, fouts);
         fclose (fouts);
#endif
#ifdef TEXTDUMP
         temp_buff[s] = 0;
         printf (temp_buff);
#endif         
        }
      }
     break;
    }
  }
}

unsigned short
eth_w5500_io(eth_w5500_t *eth, unsigned char mosi, unsigned char sclk, unsigned char scs, unsigned char rst)
{
 unsigned int offset = 0;
 unsigned char n;
 unsigned short addr;
 int size;
 int s;
 int j;

 struct sockaddr_in serv;

 int reuse = 1;
 char skt_addr[30];
 unsigned short skt_port;

#ifdef DUMP
 char sfname[30];
 FILE * fouts;
#endif
 /*
  if (!rst)
   {
    eth_w5500_rst(eth);
    dprintf("pin rst w5500\n");
    return 0;
   }
  */

 bitbang_spi_io (&eth->bb_spi, sclk, mosi, scs);

 switch (bitbang_spi_get_status (&eth->bb_spi))
  {
  case SPI_DATA:

   switch (eth->bb_spi.byte << 3)
    {
    case 0: //nothing
    case 8: //half address
     break;
    case 16: //full address
     eth->addr = eth->bb_spi.insr;
     dprintf ("----------------------------------------\n");
     dprintf ("dsr %03i w5500= 0x%04X  addr=0x%04X  \n", eth->bb_spi.byte << 3, eth->bb_spi.insr, eth->addr);
     break;
    case 24: //control
     eth->control = eth->bb_spi.insr & 0x00FF;
#ifdef DEBUG     
     dprintf ("dsr %03i w5500= 0x%04X  ctrl=0x%02X  BSB=%s\n", eth->bc, eth->insr, eth->control, debug_BSB (eth));
#endif
     if (!RWB)//read
      {
       switch (BSB)
        {
        case B_COMMON:
         eth->bb_spi.outsr = eth->Common[eth->addr & 0x3F];
         break;

        case B_SCK0RG:
        case B_SCK1RG:
        case B_SCK2RG:
        case B_SCK3RG:
        case B_SCK4RG:
        case B_SCK5RG:
        case B_SCK6RG:
        case B_SCK7RG:
         n = (BSB - B_SCK0RG) / 4;
         switch (eth->addr)
          {
          case Sn_TX_WR0: //for compatibility ?
          case Sn_TX_WR1:
           eth->bb_spi.outsr = 0;
           break;
          default:
           eth->bb_spi.outsr = eth->Socket[n][eth->addr & 0x2F];
           break;
          }
         break;
        case B_SCK0RX:
        case B_SCK1RX:
        case B_SCK2RX:
        case B_SCK3RX:
        case B_SCK4RX:
        case B_SCK5RX:
        case B_SCK6RX:
        case B_SCK7RX:
         n = (BSB - B_SCK0RX) / 4;
         size = (eth->Socket[n][Sn_RXBUF_SIZE]*1024);
         addr = eth->addr & (size - 1);
         eth->bb_spi.outsr = eth->RX_Mem[eth->RX_ptr[n] + addr];
         break;
        case B_SCK0TX:
        case B_SCK1TX:
        case B_SCK2TX:
        case B_SCK3TX:
        case B_SCK4TX:
        case B_SCK5TX:
        case B_SCK6TX:
        case B_SCK7TX:
         n = (BSB - B_SCK0TX) / 4;
         size = (eth->Socket[n][Sn_TXBUF_SIZE]*1024);
         addr = eth->addr & (size - 1);
         eth->bb_spi.outsr = eth->TX_Mem[eth->TX_ptr[n] + addr];
         break;
        default:
         eth->bb_spi.outsr = 0;
         break;
        }
       dprintf ("dsr %03i w5500= 0x%04X  read data=0x%02X (%i)\n", eth->bb_spi.byte << 3, eth->bb_spi.insr, eth->bb_spi.outsr, offset);
      }
     break;
    default: //data
     offset = ((eth->bb_spi.byte << 3) - 32) / 8;
     if (RWB)//write
      {
       switch (BSB)
        {
        case B_COMMON:
         if ((eth->addr + offset) < 0x40)
          {
           eth->Common[(eth->addr + offset) & 0x3F] = eth->bb_spi.insr & 0x00FF;
           switch (eth->addr + offset)
            {
            case CR_MR:
             if (eth->Common[CR_MR] & 0x80)
              {
               //reset
               eth_w5500_rst (eth);
              }
             break;
            }
          }
         break;
        case B_SCK0RG:
        case B_SCK1RG:
        case B_SCK2RG:
        case B_SCK3RG:
        case B_SCK4RG:
        case B_SCK5RG:
        case B_SCK6RG:
        case B_SCK7RG:
         n = (BSB - B_SCK0RG) / 4;
         if ((eth->addr + offset) < 0x30)
          {
           switch (eth->addr + offset)
            {
            case Sn_CR:
             eth->Socket[n][eth->addr + offset] = eth->bb_spi.insr & 0x00FF;
             if (eth->Socket[n][Sn_CR])
              {
               dprintf ("eth_w5500: socket cmd = 0x%02X\n", eth->Socket[n][Sn_CR]);
               switch (eth->Socket[n][Sn_CR])
                {
                case OPEN:
                 dsprintf ("eth_w5500: Socket %i Open type(%i)\n", n, eth->Socket[n][Sn_MR]& 0x0F);

                 switch (eth->Socket[n][Sn_MR]& 0x0F)
                  {
                  case Sn_MR_CLOSE:
                   eth->Socket[n][Sn_SR] = SOCK_CLOSED;
                   eth->sockfd[n] = INVALID_SOCKET_VALUE;
                   break;
                  case Sn_MR_TCP:
                   eth->Socket[n][Sn_SR] = SOCK_INIT;
                   if ((eth->sockfd[n] = socket (PF_INET, SOCK_STREAM, 0)) < 0)
                    {
                     printf ("eth_w5500: socket creation failed\n");
                     eth->sockfd[n] = INVALID_SOCKET_VALUE;
                     eth->Socket[n][Sn_SR] = SOCK_CLOSED;
                    }
                   break;
                  case Sn_MR_UDP:
                   eth->Socket[n][Sn_SR] = SOCK_UDP;
                   if ((eth->sockfd[n] = socket (AF_INET, SOCK_DGRAM, 0)) < 0)
                    {
                     printf ("eth_w5500: socket creation failed\n");
                     eth->sockfd[n] = INVALID_SOCKET_VALUE;
                     eth->Socket[n][Sn_SR] = SOCK_CLOSED;
                    }
                   break;
                  case S0_MR_MACRAW:
                   if (n == 0)
                    {
                     printf ("eth_w5500: S0_MR_MACRAW Not implemented\n");
                    }
                   break;
                  }
                 writeWord (eth->Socket[n], Sn_TX_RD0, 0);
                 writeWord (eth->Socket[n], Sn_TX_WR0, 0);
                 writeWord (eth->Socket[n], Sn_RX_RD0, 0);
                 writeWord (eth->Socket[n], Sn_RX_WR0, 0);
                 writeWord (eth->Socket[n], Sn_TX_FSR0, eth->Socket[n][Sn_TXBUF_SIZE] * 1024);
                 writeWord (eth->Socket[n], Sn_RX_RSR0, 0);
                 eth->status[n] = 0;
                 setnblock (eth->sockfd[n]);
                 break;
                case LISTEN:
                 if ((eth->Socket[n][Sn_MR]& 0x0F) != Sn_MR_TCP)break;

                 skt_port = readWord (eth->Socket[n], Sn_PORT0);

                 if (skt_port < 2000) //avoid system services
                  {
                   skt_port += 2000;
                  }

                 if (eth->Socket[n][Sn_SR] != SOCK_INIT) break;

                 dsprintf ("eth_w5500: Socket %i Listen on port %i\n", n, skt_port);
                 eth->bindp[n] = skt_port;


                 eth->Socket[n][Sn_SR] = SOCK_LISTEN;

                 for (j = 0; j < 8; j++)
                  {
                   if ((eth->listenfd_port[j] == 0) || (eth->listenfd_port[j] == skt_port))
                    {
                     eth->listenfd_map[n] = j;
                     eth->listenfd_port[j] = skt_port;
                     break;
                    }
                  }

                 if (eth->listenfd[eth->listenfd_map[n]] == INVALID_SOCKET_VALUE)
                  {

                   if ((eth->listenfd[eth->listenfd_map[n]] = socket (PF_INET, SOCK_STREAM, 0)) < 0)
                    {
                     eth->sockfd[n] = INVALID_SOCKET_VALUE;
                     eth->Socket[n][Sn_SR] = SOCK_CLOSED;
                     break;
                    }

                   if (setsockopt (eth->listenfd[eth->listenfd_map[n]], SOL_SOCKET, SO_REUSEADDR, (const char*) &reuse, sizeof (reuse)) < 0)
                    {
                     perror ("eth_w5500: setsockopt(SO_REUSEADDR) failed");
                     close (eth->sockfd[n]);
                     eth->sockfd[n] = INVALID_SOCKET_VALUE;
                     eth->Socket[n][Sn_SR] = SOCK_CLOSED;
                     eth->listenfd[eth->listenfd_map[n]] = INVALID_SOCKET_VALUE;
                     eth->status[n] = ER_REUSE;
                     break;
                    }

                   memset (&serv, 0, sizeof (serv));
                   serv.sin_family = AF_INET;
                   serv.sin_addr.s_addr = htonl (INADDR_ANY);
                   serv.sin_port = htons (skt_port);
                   if (bind (eth->listenfd[eth->listenfd_map[n]], (sockaddr *) & serv, sizeof (serv)))
                    {
                     printf ("eth_w5500: bind error : %s \n", strerror (errno));
                     close (eth->sockfd[n]);
                     eth->sockfd[n] = INVALID_SOCKET_VALUE;
                     eth->Socket[n][Sn_SR] = SOCK_CLOSED;
                     eth->status[n] = ER_BIND;
                     eth->listenfd[eth->listenfd_map[n]] = INVALID_SOCKET_VALUE;
                     eth->listenfd_port[eth->listenfd_map[n]] = 0;
                     break;
                    }
                   if (listen (eth->listenfd[eth->listenfd_map[n]], SOMAXCONN))
                    {
                     printf ("eth_w5500: listen error : %s \n", strerror (errno));
                     close (eth->sockfd[n]);
                     eth->sockfd[n] = INVALID_SOCKET_VALUE;
                     eth->Socket[n][Sn_SR] = SOCK_CLOSED;
                     eth->status[n] = ER_LIST;
                     break;
                    }

                   setnblock (eth->listenfd[eth->listenfd_map[n]]);
                  }
                 else
                  {
                   eth->sockfd[n] = INVALID_SOCKET_VALUE;
                  }
                 break;
                case CONNECT:
                 if ((eth->Socket[n][Sn_MR]& 0x0F) != Sn_MR_TCP)break;
                 eth->status[n] = 0;
                 eth->Socket[n][Sn_SR] = SOCK_SYNSENT;
                 conn_timeout[n] = 0;
                 sprintf (skt_addr, "%i.%i.%i.%i", eth->Socket[n][Sn_DIPR0], eth->Socket[n][Sn_DIPR1], eth->Socket[n][Sn_DIPR2], eth->Socket[n][Sn_DIPR3]);
                 skt_port = readWord (eth->Socket[n], Sn_DPORT0);
                 dsprintf ("Socket %i try to connect to %s:%i\n", n, skt_addr, skt_port);

                 break;
                case DISCON:
                 if ((eth->Socket[n][Sn_MR]& 0x0F) != Sn_MR_TCP)break;
                 dsprintf ("eth_w5500:Socket %i Disconnect\n", n);
                 if (shutdown (eth->sockfd[n], SHUT_RDWR) < 0)
                  {
                   printf ("eth_w5500: shutdown error : %s \n", strerror (errno));
                   eth->status[n] = ER_SHUT;
                  }
                case CLOSE:
                 dsprintf ("eth_w5500:Socket %i Close\n", n);
                 eth->Socket[n][Sn_SR] = SOCK_CLOSED;
                 close (eth->sockfd[n]);
                 eth->sockfd[n] = INVALID_SOCKET_VALUE;
                 break;
                case SEND:

                 if (eth->link)eth->active = 2;

                 switch (eth->Socket[n][Sn_MR]& 0x0F)
                  {
                  case Sn_MR_TCP:

                   if (eth->Socket[n][Sn_SR] != SOCK_ESTABLISHED)break;

                   size = readWord (eth->Socket[n], Sn_TX_WR0);
#ifdef DUMP
                   sprintf (sfname, "/tmp/%03i_send_tcp.bin", scont++);
                   fouts = fopen (sfname, "w");
                   fwrite (&eth->TX_Mem[eth->TX_ptr[n]], size, 1, fouts);
                   fclose (fouts);
#endif
#ifdef TEXTDUMP
                   eth->TX_Mem[eth->TX_ptr[n] + size] = 0;
                   printf ((char *) &eth->TX_Mem[eth->TX_ptr[n]]);
#endif                     
                   if ((s = send (eth->sockfd[n], (const char *) &eth->TX_Mem[eth->TX_ptr[n]], size, MSG_NOSIGNAL)) < 0)
                    {
                     printf ("eth_w5500: send error : %s \n", strerror (errno));
                     eth->Socket[n][Sn_SR] = SOCK_CLOSED;
                     eth->status[n] = ER_SEND;
                    }

                   if (s > 0)
                    {
                     //dsprintf ("Socket %i Send %i \n", n, s);
                     dsprintf (".");
                     eth->Socket[n][Sn_IR] |= 0x10;

                     size = (eth->Socket[n][Sn_TXBUF_SIZE]*1024);
                     writeWord (eth->Socket[n], Sn_TX_FSR0, (size));
                     writeWord (eth->Socket[n], Sn_TX_WR0, 0);
                     writeWord (eth->Socket[n], Sn_TX_RD0, 0);
                    }
                   break;
                  case Sn_MR_UDP:

                   sprintf (skt_addr, "%i.%i.%i.%i", eth->Socket[n][Sn_DIPR0], eth->Socket[n][Sn_DIPR1], eth->Socket[n][Sn_DIPR2], eth->Socket[n][Sn_DIPR3]);
                   skt_port = readWord (eth->Socket[n], Sn_DPORT0);

                   memset (&serv, 0, sizeof (serv));

                   // Filling server information
                   serv.sin_family = AF_INET;
                   serv.sin_port = htons (skt_port);
                   serv.sin_addr.s_addr = inet_addr (skt_addr);

                   size = readWord (eth->Socket[n], Sn_TX_WR0);
#ifdef DUMP
                   sprintf (sfname, "/tmp/%03i_send_udp.bin", scont++);
                   fouts = fopen (sfname, "w");
                   fwrite (&eth->TX_Mem[eth->TX_ptr[n]], size, 1, fouts);
                   fclose (fouts);
#endif
#ifdef TEXTDUMP
                   eth->TX_Mem[eth->TX_ptr[n] + size] = 0;
                   printf ((char *) &eth->TX_Mem[eth->TX_ptr[n]]);
#endif                     
                   dsprintf ("Socket %i Send %i to %s:%i\n", n, size, skt_addr, skt_port);
                   if (!strcmp (skt_addr, "255.255.255.255"))//fake broadcast
                    {
                     if (skt_port == 67) //dhcp
                      {
                       eth_w5500_fake_dhcp_reply (eth, n);
                      }
                    }
                   else
                    {
                     sendto (eth->sockfd[n], (const char *) &eth->TX_Mem[eth->TX_ptr[n]], size,
                             0 /*MSG_DONTWAIT MSG_CONFIRM*/, (const struct sockaddr *) &serv,
                             sizeof (serv));
                    }

                   size = (eth->Socket[n][Sn_TXBUF_SIZE]*1024);
                   writeWord (eth->Socket[n], Sn_TX_FSR0, (size));
                   writeWord (eth->Socket[n], Sn_TX_WR0, 0);
                   writeWord (eth->Socket[n], Sn_TX_RD0, 0);
                   eth->Socket[n][Sn_IR] |= 0x10;
                   break;
                  }

                 break;
                case SEND_MAC:
                 dsprintf ("Socket %i SEND_MAC CMD !!!NOT IMPLEMETED!!!\n", n);
                 //if ((eth->Socket[n][Sn_MR]& 0x0F) != Sn_MR_UDP)break;
                case SEND_KEEP:
                 dsprintf ("Socket %i SEND_KEEP CMD !!!NOT IMPLEMETED!!!\n", n);
                 //if ((eth->Socket[n][Sn_MR]& 0x0F) != Sn_MR_TCP)break;
                case RECV:
                 dsprintf ("Socket %i Receive CMD  (%i)\n", n, readWord (eth->Socket[n], Sn_RX_RSR0));
                 //dsprintf ("RX_WR0=%i   RX_RD0=%i  \n", readWord (eth->Socket[n], Sn_RX_WR0), readWord (eth->Socket[n], Sn_RX_RD0));

                 size = readWord (eth->Socket[n], Sn_RX_WR0);
                 size -= readWord (eth->Socket[n], Sn_RX_RD0);
                 if (size < 0)
                  {
                   size += eth->RX_size[n];
                  }
                 writeWord (eth->Socket[n], Sn_RX_RSR0, size);
                 break;
                }
               eth->Socket[n][Sn_CR] = 0;
              }
             break;
            case Sn_IR:
             eth->Socket[n][Sn_IR] &= ~(eth->bb_spi.insr & 0x00FF);
             break;
            default:
             eth->Socket[n][eth->addr + offset] = eth->bb_spi.insr & 0x00FF;
             break;
            }
          }
         break;
        case B_SCK0TX:
        case B_SCK1TX:
        case B_SCK2TX:
        case B_SCK3TX:
        case B_SCK4TX:
        case B_SCK5TX:
        case B_SCK6TX:
        case B_SCK7TX:
         n = (BSB - B_SCK0TX) / 4;
         size = (eth->Socket[n][Sn_TXBUF_SIZE]*1024);
         addr = (eth->addr + offset) % size;
         //printf ("TX write 0x%04X 0x%04X 0x%04X 0x%04X\n", addr, eth->addr + offset, size, readWord (eth->Socket[n], Sn_TX_WR0));
         eth->TX_Mem[eth->TX_ptr[n] + addr] = eth->bb_spi.insr & 0x00FF;

         size = readWord (eth->Socket[n], Sn_TX_FSR0);
         size--;
         writeWord (eth->Socket[n], Sn_TX_FSR0, size);

         break;
        }
       dprintf ("dsr %03i w5500= 0x%04X  write data=0x%02X (%i)\n", eth->bb_spi.byte << 3, eth->bb_spi.insr, eth->bb_spi.insr & 0x00FF, offset);
      }
     else//read
      {
       switch (BSB)
        {
        case B_COMMON:
         eth->bb_spi.outsr = eth->Common[(eth->addr + offset + 1) & 0x3F];
         break;
        case B_SCK0RG:
        case B_SCK1RG:
        case B_SCK2RG:
        case B_SCK3RG:
        case B_SCK4RG:
        case B_SCK5RG:
        case B_SCK6RG:
        case B_SCK7RG:
         n = (BSB - B_SCK0RG) / 4;
         switch (eth->addr)
          {
          case Sn_TX_WR0: //for compatibility ?
          case Sn_TX_WR1:
           eth->bb_spi.outsr = 0;
           break;
          default:
           eth->bb_spi.outsr = eth->Socket[n][(eth->addr + offset + 1) & 0x2F];
           break;
          }
         break;
        case B_SCK0RX:
        case B_SCK1RX:
        case B_SCK2RX:
        case B_SCK3RX:
        case B_SCK4RX:
        case B_SCK5RX:
        case B_SCK6RX:
        case B_SCK7RX:
         n = (BSB - B_SCK0RX) / 4;
         size = (eth->Socket[n][Sn_RXBUF_SIZE]*1024);
         addr = (eth->addr + offset + 1) % size;
         eth->bb_spi.outsr = eth->RX_Mem[eth->RX_ptr[n] + addr];
         break;
        case B_SCK0TX:
        case B_SCK1TX:
        case B_SCK2TX:
        case B_SCK3TX:
        case B_SCK4TX:
        case B_SCK5TX:
        case B_SCK6TX:
        case B_SCK7TX:
         n = (BSB - B_SCK0TX) / 4;
         size = (eth->Socket[n][Sn_TXBUF_SIZE]*1024);
         addr = (eth->addr + offset + 1) & (size - 1);
         eth->bb_spi.outsr = eth->TX_Mem[eth->TX_ptr[n] + addr];
         break;
        default:
         eth->bb_spi.outsr = 0;
         break;
        }
       dprintf ("dsr %03i w5500= 0x%04X  read data=0x%02X (%i)\n", eth->bb_spi.byte << 3, eth->bb_spi.insr, eth->bb_spi.outsr, offset + 1);
      }
     break;
    }
   break;
  }

 return ((eth->bb_spi.outsr & 0x080) > 0);
}
