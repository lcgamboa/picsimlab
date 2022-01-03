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

#define _TCP_

#ifndef _WIN_
#include<sys/types.h>
#include<sys/socket.h>
#include<sys/un.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#define INVALID_HANDLE_VALUE -1;
#else
#include<winsock2.h>
#include<ws2tcpip.h>
#define MSG_NOSIGNAL 0
#endif

#include "exp_bsim_remote.h"
#include"../picsimlab1.h"
#include"../serial_port.h"

#include <stdint.h>


void setblock(int sock_descriptor);
void setnblock(int sock_descriptor);

static int listenfd = -1;

bsim_remote::bsim_remote(void)
{
 connected = 0;
 sockfd = -1;
 fname_bak[0] = 0;
 fname_[0] = 0;

 memset (&ADCvalues, 0xFF, 32);
}

bsim_remote::~bsim_remote(void) { }

void
bsim_remote::MSetSerial(const char * port) {
 /*
 set_serial (&pic,0, port, 0, 0, 0);
 set_serial (&pic,1, "", 0, 0, 0);
  */ }

int
bsim_remote::MInit(const char * processor, const char * fname, float freq)
{
#ifdef _TCP_
 struct sockaddr_in serv;
#else
 struct sockaddr_un serv;
#endif

 lxString sproc = GetSupportedDevices ();
 if (!sproc.Contains (processor))
  {
   Proc = "Ripes";
   printf ("PICSimLab: Unknown processor %s, loading default !\n", processor);
  }

 sockfd = -1;

 pins_reset ();

 serialfd[0] = INVALID_HANDLE_VALUE;
 serialfd[1] = INVALID_HANDLE_VALUE;
 serialfd[2] = INVALID_HANDLE_VALUE;
 serialfd[3] = INVALID_HANDLE_VALUE;

 if (listenfd < 0)
  {
#ifdef _TCP_
   int reuse = 1;
   if ((listenfd = socket (PF_INET, SOCK_STREAM, 0)) < 0)
    {
     printf ("picsimlab: socket error : %s \n", strerror (errno));
     exit (1);
    }

   if (setsockopt (listenfd, SOL_SOCKET, SO_REUSEADDR, (const char*) &reuse, sizeof (reuse)) < 0)
    perror ("setsockopt(SO_REUSEADDR) failed");

   memset (&serv, 0, sizeof (serv));
   serv.sin_family = AF_INET;
   serv.sin_addr.s_addr = htonl (INADDR_ANY);
   serv.sin_port = htons (7890);
#else
   if ((listenfd = socket (PF_UNIX, SOCK_STREAM, 0)) < 0)
    {
     printf ("picsimlab: socket error : %s \n", strerror (errno));
     exit (1);
    }

   memset (&serv, 0, sizeof (serv));
   serv.sun_family = AF_UNIX;
   serv.sun_path[0] = 0;
   strncpy (serv.sun_path + 1, "picsimlab_qemu", sizeof (serv.sun_path) - 2);
#endif

   if (bind (listenfd, (sockaddr *) & serv, sizeof (serv)))
    {
     printf ("picsimlab: remote bind error : %s \n", strerror (errno));
     exit (1);
    }

   if (listen (listenfd, SOMAXCONN))
    {
     printf ("picsimlab: remote listen error : %s \n", strerror (errno));
     exit (1);
    }

  }

 Window1.menu1_File_LoadHex.SetEnable (0);
 Window1.menu1_File_SaveHex.SetEnable (0);

 setnblock (listenfd);

 return 0; //ret;
}

const int
bsim_remote::TestConnection(void)
{
 if (!connected)
  {
#ifdef _TCP_
   struct sockaddr_in cli;
#else
   struct sockaddr_un cli;
#endif   
#ifdef _WIN_
   int clilen;
#else
   unsigned int clilen;
#endif

   clilen = sizeof (cli);
   if ((sockfd = accept (listenfd, (sockaddr *) & cli, & clilen)) < 0)
    {
     sockfd = -1;
     connected = 0;
     return 0;
    }
   printf ("picsimlab: Ripes connected to PICSimLab!\n");

   connected = 1;
   StartThread ();
  }
 return connected;
}

const int
bsim_remote::DataAvaliable(void)
{
 if (!connected) return 0;

 char dp;
#ifndef _WIN_         
 int ret = recv (sockfd, &dp, 1, MSG_PEEK | MSG_DONTWAIT);
#else
 setnblock (sockfd);
 int ret = recv (sockfd, &dp, 1, MSG_PEEK);
 setblock (sockfd);
#endif   

 if (ret != 1)
  {
#ifndef _WIN_         
   if (errno != EAGAIN)
#else
   if (WSAGetLastError () != WSAEWOULDBLOCK)
#endif   
    {
     ConnectionError ("recv");
    }
   return 0;
  }

 return 1;
}

void
bsim_remote::ConnectionError(const char * s_error)
{
 printf ("picsimlab: remote %s error : %s \n", s_error, strerror (errno));
 Disconnect ();
}

void
bsim_remote::Disconnect(void)
{
 if (connected)
  {
   if (sockfd >= 0)close (sockfd);
   sockfd = -1;
   connected = 0;
   Window1.Set_mcupwr (0);
  }
}

void
bsim_remote::MEnd(void)
{

 Disconnect ();

 Window1.menu1_File_LoadHex.SetEnable (1);
 Window1.menu1_File_SaveHex.SetEnable (1);

 connected = 0;
}

void
bsim_remote::EndServers(void)
{
 if (listenfd >= 0)close (listenfd);
 listenfd = -1;
 
 if (sockfd >= 0)close (sockfd);
 sockfd = -1;
}

void
bsim_remote::MEraseFlash(void) {
 //erase_flash ();
}

void
bsim_remote::MSetFreq(float freq_)
{
 freq = freq_;
}

float
bsim_remote::MGetFreq(void)
{
 return freq;
}

float
bsim_remote::MGetInstClockFreq(void)
{
 return freq;
}

int
bsim_remote::CpuInitialized(void)
{
 return 1;
}

void
bsim_remote::DebugLoop(void) { }

lxString
bsim_remote::MGetPinName(int pin)
{
 lxString pinname = "error";

 if (!Proc.compare ("Ripes"))
  {
   switch (pin)
    {
    case 1:
     pinname = "PA0";
     break;
    case 2:
     pinname = "PA1";
     break;
    case 3:
     pinname = "PA2";
     break;
    case 4:
     pinname = "PA3";
     break;
    case 5:
     pinname = "PA4";
     break;
    case 6:
     pinname = "PA5";
     break;
    case 7:
     pinname = "PA6";
     break;
    case 8:
     pinname = "PA7";
     break;
    case 9:
     pinname = "PA8";
     break;
    case 10:
     pinname = "PA9";
     break;
    case 11:
     pinname = "PAa";
     break;
    case 12:
     pinname = "PAb";
     break;
    case 13:
     pinname = "PAc";
     break;
    case 14:
     pinname = "PAd";
     break;
    case 15:
     pinname = "PAe";
     break;
    case 16:
     pinname = "PAf";
     break;
    case 17:
     pinname = "PB0";
     break;
    case 18:
     pinname = "PB1";
     break;
    case 19:
     pinname = "PB2";
     break;
    case 20:
     pinname = "PB3";
     break;
    case 21:
     pinname = "PB4";
     break;
    case 22:
     pinname = "PB5";
     break;
    case 23:
     pinname = "PB6";
     break;
    case 24:
     pinname = "PB7";
     break;
    case 25:
     pinname = "PB8";
     break;
    case 26:
     pinname = "PB9";
     break;
    case 27:
     pinname = "PBa";
     break;
    case 28:
     pinname = "PBb";
     break;
    case 29:
     pinname = "PBc";
     break;
    case 30:
     pinname = "PBd";
     break;
    case 31:
     pinname = "PBe";
     break;
    case 32:
     pinname = "PBf";
     break;
    case 33:
     pinname = "X0";
     break;
    case 34:
     pinname = "X1";
     break;
    case 35:
     pinname = "X2";
     break;
    case 36:
     pinname = "X3";
     break;
    case 37:
     pinname = "X4";
     break;
    case 38:
     pinname = "X5";
     break;
    case 39:
     pinname = "X6";
     break;
    case 40:
     pinname = "X7";
     break;
    case 41:
     pinname = "X8";
     break;
    case 42:
     pinname = "X9";
     break;
    case 43:
     pinname = "Xa";
     break;
    case 44:
     pinname = "Xb";
     break;
    case 45:
     pinname = "Xc";
     break;
    case 46:
     pinname = "Xd";
     break;
    case 47:
     pinname = "VSS";
     break;
    case 48:
     pinname = "VDD";
     break;
    }
  }

 return pinname;
}

void
bsim_remote::MDumpMemory(const char * fname) { }

int
bsim_remote::DebugInit(int dtyppe) //argument not used in picm only mplabx
{
 return 0; //!mplabxd_init (this, Window1.Get_debug_port ()) - 1;
}

int
bsim_remote::MGetPinCount(void)
{
 if (!Proc.compare ("Ripes")) return 48;
 return 0;
}

void
bsim_remote::pins_reset(void)
{
 lxString pname;
 for (int p = 0; p < MGetPinCount (); p++)
  {
   pins[p].avalue = 0;
   pins[p].lvalue = 0;
   pins[p].value = 0;
   pins[p].ptype = PT_DIGITAL;
   pins[p].dir = PD_IN;
   pins[p].ovalue = 0;
   pins[p].oavalue = 0;

   pname = MGetPinName (p + 1);

   if (pname[1] == 'A')
    {
     pins[p].port = (unsigned char *) &Ports[0];
     pins[p].pord = pname[2] - '0';
     if (pins[p].pord > 15)pins[p].pord -= 39;
    }
   else if (pname[1] == 'B')
    {
     pins[p].port = (unsigned char *) &Ports[1];
     pins[p].pord = pname[2] - '0';
     if (pins[p].pord > 15)pins[p].pord -= 39;
    }
   else
    {
     pins[p].port = NULL;
     pins[p].pord = -1;
    }

   if (pname.Contains ("VDD"))
    {
     pins[p].value = 1;
     pins[p].dir = PD_OUT;
    }

  }
}

void
bsim_remote::MSetPin(int pin, unsigned char value)
{
 if (!pin)return;
 if ((pins[pin - 1].dir == PD_IN)&&(pins[pin - 1].value != value))
  {
   pins[pin - 1].value = value;
   unsigned short * port = (unsigned short *) pins[pin - 1].port;

   if (value)
    {
     *port |= 1 << pins[pin - 1].pord;
    }
   else
    {
     *port &= ~(1 << pins[pin - 1].pord);
    }
  }
}

void
bsim_remote::MSetPinDOV(int pin, unsigned char ovalue) {
 //set_pin_DOV (pin, ovalue);
}

void
bsim_remote::MSetAPin(int pin, float value)
{
 if (!pin)return;
 if ((connected)&&(pins[pin - 1].avalue != value))
  {

   unsigned char channel = 0xFF;

   pins[pin - 1].avalue = value;

   if (!Proc.compare ("Ripes"))
    {
     switch (pin)
      {
      case 33: //PC0
       channel = 10;
       break;
      case 34: //PC1
       channel = 11;
       break;
      case 35: //PC2
       channel = 12;
       break;
      case 36: //PC3
       channel = 13;
       break;
      case 37: //PA0
       channel = 0;
       break;
      case 38: //PA1
       channel = 1;
       break;
      case 39: //PA2
       channel = 2;
       break;
      case 40: //PA3
       channel = 3;
       break;
      case 41: //PA4
       channel = 4;
       break;
      case 42: //PA5
       channel = 5;
       break;
      case 43: //PA6
       channel = 6;
       break;
      case 44: //PA7
       channel = 7;
       break;
      case 45: //PC4
       channel = 14;
       break;
      case 46: //PC5
       channel = 15;
       break;
      }
    }
   if (channel != 0xFF)
    {
     if (value > 3.3)value = 3.3;
     if (value < 0)value = 0;

     unsigned short svalue = (unsigned short) (4096 * value / 3.3);

     pins[pin - 1].ptype = PT_ANALOG;

     if (ADCvalues[channel] != svalue)
      {
       /*
       unsigned char buff[3];

       buff[0] = channel | 0x40;
       buff[1] = svalue & 0xFF;
       buff[2] = svalue >> 8;
        */
       ADCvalues[channel] = svalue;
      }
    }
  }
}

unsigned char
bsim_remote::MGetPin(int pin)
{
 if ((pin) && (pin < MGetPinCount ()))
  {
   return pins[pin - 1].value;
  }
 return 0;
}

void
bsim_remote::MReset(int flags)
{
 Disconnect ();
 Dirs[0]  = 0xFF;
 Dirs[1]  = 0xFF;
 Ports[0] = 0x00;
 Ports[1] = 0x00;
 t0CNT = 0;
 t0STA = 0;
 t0CON = 0x7FFF;
 t0PR  = 0xFFFF;
 t0iclk = 0;
}

const picpin *
bsim_remote::MGetPinsValues(void)
{
 return pins;
}

void
bsim_remote::MStepResume(void) {
 //if (pic.s2 == 1)step ();
}


//===================== Ripes protocol =========================================

int32_t
bsim_remote::recv_payload(char * buff, const uint32_t payload_size)
{
 char * dp = buff;
 int ret = 0;
 uint32_t size = payload_size;
 do
  {
   if ((ret = recv (sockfd, dp, size, MSG_WAITALL)) != (int) size)
    {
     printf ("receive error : %s \n", strerror (errno));
     return -1;
    }
   size -= ret;
   dp += ret;
  }
 while (size);

 return ret;
}

int32_t
bsim_remote::send_cmd(const uint32_t cmd, const char * payload, const uint32_t payload_size)
{
#define BSIZE 1024

 char buffer[BSIZE];
 uint32_t ret;
 cmd_header_t cmd_header;

 cmd_header.msg_type = htonl (cmd);
 cmd_header.payload_size = htonl (payload_size);

 unsigned int dsize = sizeof ( cmd_header);
 const char * dp = (const char *) &cmd_header;


 if (payload_size)
  {
   for (unsigned int i = 0; i < dsize; i++)
    {
     buffer[i] = dp[i];
    }
   for (unsigned int i = 0; i < payload_size; i++)
    {
     buffer[dsize + i] = payload[i];
    }
   dsize += payload_size;
   if (dsize > BSIZE)
    {
     printf ("PICSimLab: Send Overflow\n");
     exit (-1);
    }
   dp = buffer;
  }

 if ((ret = send (sockfd, dp, dsize, MSG_NOSIGNAL)) != dsize)
  {
   printf ("send error : %s \n", strerror (errno));
   return -1;
  }
 return ret;
}

int32_t
bsim_remote::recv_cmd(cmd_header_t * cmd_header)
{

 char * dp = (char *) (cmd_header);
 int ret = 0;
 int size = sizeof ( cmd_header);
 do
  {
   if ((ret = recv (sockfd, dp, size, MSG_WAITALL)) != size)
    {
     printf ("receive error : %s \n", strerror (errno));
     return -1;
    }

   size -= ret;
   dp += ret;
  }
 while (size);

 cmd_header->msg_type = ntohl (cmd_header->msg_type);
 cmd_header->payload_size = ntohl (cmd_header->payload_size);

 return ret;
}


//==============================================================================
