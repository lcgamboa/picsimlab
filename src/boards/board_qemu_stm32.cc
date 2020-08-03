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


#include "board_qemu_stm32.h"
#include"../picsimlab1.h"

void setblock(int sock_descriptor);
void setnblock(int sock_descriptor);

#ifndef _WIN_
#include<sys/types.h>
#include<sys/socket.h>
#include<sys/un.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#else
#include<winsock2.h>
#include<ws2tcpip.h>
WORD wVersionRequested = 2;
WSADATA wsaData;
#define SHUT_RDWR SD_BOTH 
#endif

board_qemu_stm32::board_qemu_stm32(void)
{
 connected = 0;
}

void
board_qemu_stm32::MSetSerial(const char * port) {
 /*
 set_serial (&pic,0, port, 0, 0, 0);
 set_serial (&pic,1, "", 0, 0, 0);
  */ }

int
board_qemu_stm32::MInit(const char * processor, const char * fname, float freq)
{
 struct sockaddr_in serv, cli;
 char buff[100];
 int n;

#ifdef _WIN_
 int clilen;
#else
 unsigned int clilen;
#endif

 sockfd = -1;
 sockmon = -1;

 pins_reset ();

 serialfd[0] = -1;
 serialfd[1] = -1;
 serialfd[2] = -1;
 serialfd[3] = -1;


 if ((listenfd = socket (PF_INET, SOCK_STREAM, 0)) < 0)
  {
   printf ("socket error : %s \n", strerror (errno));
   exit (1);
  }

 int reuse = 1;
 if (setsockopt (listenfd, SOL_SOCKET, SO_REUSEADDR, (const char*) &reuse, sizeof (reuse)) < 0)
  perror ("setsockopt(SO_REUSEADDR) failed");

 memset (&serv, 0, sizeof (serv));
 serv.sin_family = AF_INET;
 serv.sin_addr.s_addr = htonl (INADDR_ANY);
 serv.sin_port = htons (2200);

 if (bind (listenfd, (sockaddr *) & serv, sizeof (serv)) < 0)
  {
   printf ("bind error : %s \n", strerror (errno));
   exit (1);
  }

 if (listen (listenfd, SOMAXCONN) < 0)
  {
   printf ("listen error : %s \n", strerror (errno));
   exit (1);
  }

 char fname_[300];
 //change .hex to .bin
 strncpy (fname_, fname, 299);
 strncpy (fname_ + (strlen (fname_) - 3), "bin", 299);


 if (!lxFileExists (fname_))
  {
   //create a empty memory
   FILE *fout;
   fout = fopen (fname_, "w");
   if (fout)
    {
     unsigned int val[4] = {0, 0, 0, 0};
     fwrite (&val, 4, sizeof (int), fout);
     fclose (fout);
    }
   else
    {
     printf ("Erro creating file %s \n", fname_);
     exit (-1);
    }
  }

 char cmd[500];
 snprintf (cmd, 499, "cd %s; ./qemu-system-arm  -M stm32-f103c8-picsimlab  -pflash %s -serial %s -qmp tcp:localhost:2500,server,nowait &",
             "/home/gamboa/projetos/qemu_stm32/arm-softmmu/",
             fname_,
             "/dev/tnt2"
             );

 printf ("%s", (const char *)cmd);
 system (cmd);

 clilen = sizeof (cli);
 if (
     (sockfd =
      accept (listenfd, (sockaddr *) & cli, & clilen)) < 0)
  {
   printf ("accept error : %s \n", strerror (errno));
   exit (1);
  }

 close (listenfd);

 printf ("Qemu connected!\n");

 setnblock (sockfd);

 //monitor  
 if ((sockmon = socket (PF_INET, SOCK_STREAM, 0)) < 0)
  {
   printf ("socket error : %s \n", strerror (errno));
   exit (1);
  }
 memset (&serv, 0, sizeof (serv));
 serv.sin_family = AF_INET;
 serv.sin_addr.s_addr = inet_addr ("127.0.0.1");
 serv.sin_port = htons (2500);

 if (connect (sockmon, (sockaddr *) & serv, sizeof (serv)) < 0)
  {
   printf ("connect error : %s \n", strerror (errno));
   exit (1);
  }

 //read monitor qemu first mensage
 if ((n = recv (sockmon, buff, 99, 0)) < 0)
  {
   printf ("connect error : %s \n", strerror (errno));
   exit (1);
  }
 buff[n] = 0;
 printf ("%s", buff);

 qemu_cmd ("qmp_capabilities");

 connected = 1;
 
 Window1.menu1_File_LoadHex.SetText ("Load Bin");
 Window1.menu1_File_SaveHex.SetEnable (0);
 Window1.filedialog1.SetFileName(lxT("untitled.bin"));
 Window1.filedialog1.SetFilter(lxT("Bin Files (*.bin)|*.bin;*.BIN"));
 return 0; //ret;
}

void
board_qemu_stm32::MEnd(void)
{
 if (connected)
  {
   qemu_cmd ("quit");
  }
 close (sockfd);
 connected = 0;
 
 Window1.menu1_File_LoadHex.SetText ("Load Hex");
 Window1.menu1_File_SaveHex.SetEnable (1);
 Window1.filedialog1.SetFileName(lxT("untitled.hex"));
 Window1.filedialog1.SetFilter(lxT("Hex Files (*.hex)|*.hex;*.HEX"));
 sleep(1);
}

void
board_qemu_stm32::MEraseFlash(void) {
 //erase_flash ();
}

void
board_qemu_stm32::MSetFreq(float freq_)
{
 freq = freq_;
}

float
board_qemu_stm32::MGetFreq(void)
{
 return freq;
}

float
board_qemu_stm32::MGetInstClock(void)
{
 return freq;
}

int
board_qemu_stm32::CpuInitialized(void)
{
 return 1;
}

void
board_qemu_stm32::DebugLoop(void) {

}


String
board_qemu_stm32::MGetPinName(int pin)
{
 String pinname = "error";

 switch (pin)
  {
  case 1:
   pinname = "VBAT";
   break;
  case 2:
   pinname = "PC13";
   break;
  case 3:
   pinname = "PC14";
   break;
  case 4:
   pinname = "PC15";
   break;
  case 5:
   pinname = "PD0";
   break;
  case 6:
   pinname = "PD1";
   break;
  case 7:
   pinname = "NRST";
   break;
  case 8:
   pinname = "VSSA";
   break;
  case 9:
   pinname = "VDDA";
   break;
  case 10:
   pinname = "PA0";
   break;
  case 11:
   pinname = "PA1";
   break;
  case 12:
   pinname = "PA2";
   break;
  case 13:
   pinname = "PA3";
   break;
  case 14:
   pinname = "PA4";
   break;
  case 15:
   pinname = "PA5";
   break;
  case 16:
   pinname = "PA6";
   break;
  case 17:
   pinname = "PA7";
   break;
  case 18:
   pinname = "PB0";
   break;
  case 19:
   pinname = "PB1";
   break;
  case 20:
   pinname = "PB2";
   break;
  case 21:
   pinname = "PB10";
   break;
  case 22:
   pinname = "PB11";
   break;
  case 23:
   pinname = "VSS";
   break;
  case 24:
   pinname = "VDD";
   break;
  case 25:
   pinname = "PB12";
   break;
  case 26:
   pinname = "PB13";
   break;
  case 27:
   pinname = "PB14";
   break;
  case 28:
   pinname = "PB15";
   break;
  case 29:
   pinname = "PA8";
   break;
  case 30:
   pinname = "PA9";
   break;
  case 31:
   pinname = "PA10";
   break;
  case 32:
   pinname = "PA11";
   break;
  case 33:
   pinname = "PA12";
   break;
  case 34:
   pinname = "PA13";
   break;
  case 35:
   pinname = "VSS";
   break;
  case 36:
   pinname = "VDD";
   break;
  case 37:
   pinname = "PA14";
   break;
  case 38:
   pinname = "PA15";
   break;
  case 39:
   pinname = "PB3";
   break;
  case 40:
   pinname = "PB4";
   break;
  case 41:
   pinname = "PB5";
   break;
  case 42:
   pinname = "PB6";
   break;
  case 43:
   pinname = "PB7";
   break;
  case 44:
   pinname = "BOOT0";
   break;
  case 45:
   pinname = "PB8";
   break;
  case 46:
   pinname = "PB9";
   break;
  case 47:
   pinname = "VSS";
   break;
  case 48:
   pinname = "VDD";
   break;
  }
 return pinname;
}

void
board_qemu_stm32::MDumpMemory(const char * fname)
{
 char cmd[500];
 char fname_[300];

 //change .hex to .bin
 strncpy (fname_, fname, 299);
 strncpy (fname_ + (strlen (fname_) - 3), "bin", 299);

 qemu_cmd ("stop");
 snprintf (cmd, 500, "{ \"execute\": \"pmemsave\",\"arguments\": { \"val\": 134217728, \"size\": 65536, \"filename\": \"%s\" } }\n", fname_);
 qemu_cmd (cmd, 1);
 qemu_cmd ("cont");
}

int
board_qemu_stm32::DebugInit(int dtyppe) //argument not used in picm only mplabx
{
 return 0; //!mplabxd_init (this, Window1.Get_debug_port ()) - 1;
}


int
board_qemu_stm32::MGetPinCount(void)
{
 return 48;
}

void
board_qemu_stm32::pins_reset(void)
{
 for (int p = 0; p < MGetPinCount (); p++)
  {
   pins[p].avalue = 0;
   pins[p].lvalue = 0;
   pins[p].value = 0;
   pins[p].ptype = PT_CMOS;
   pins[p].dir = PD_IN;
   pins[p].ovalue = 0;
   pins[p].oavalue = 0;
  }

 //TODO
 //VCC pins
 //pins[6].value = 1;
 //pins[19].value = 1;
}

void
board_qemu_stm32::MSetPin(int pin, unsigned char value)
{
 if ((connected)&&(pins[pin - 1].value != value))
  {
   unsigned char val = (0x7F & pin);
   
   if(value)
      val|=0x80;
   if (send (sockfd, &val, 1, 0) != 1)
    {
     printf ("send error : %s \n", strerror (errno));
     exit (1);
    }
   pins[pin - 1].value = value;
  }
}

void
board_qemu_stm32::MSetPinDOV(int pin, unsigned char ovalue) {
 //set_pin_DOV (pin, ovalue);
}

void
board_qemu_stm32::MSetAPin(int pin, float value) {
 //set_apin (pin, value);
}

unsigned char
board_qemu_stm32::MGetPin(int pin)
{
 return 0; //get_pin (pin);
}


void
board_qemu_stm32::MReset(int flags)
{
 if (connected)
  {
   qemu_cmd ("system_reset");
  }
}


const picpin *
board_qemu_stm32::MGetPinsValues(void)
{
 return pins;
}

void
board_qemu_stm32::MStep(void)
{
 char buff;
 int n;

 if (connected)
  {
   if ((n = recv (sockfd, &buff, 1, 0)) < 0)
    {
     //printf ("receive error : %s \n", strerror (errno));
     //exit (1);
     return;
    }
   pins[(0x7F & buff) - 1].value = ((0x80 & buff) > 0);
  }
}

void
board_qemu_stm32::MStepResume(void) {
 //if (pic.s2 == 1)step ();
}


int
board_qemu_stm32::qemu_cmd(const char * cmd, int raw)
{
 int n;
 char buffin[400];
 char buffout[400];
 unsigned int size;

 /*
 //clear messages
  if ((n = recv (sockmon, buffout, 399, 0)) < 0)
  {
   return 1;
  }
 buffout[n] = 0;

 printf ("(%s)=(%s) \n", buffin, buffout);
 */
 
 if (raw)
  {
   strcpy (buffin, cmd);
  }
 else
  {
   sprintf (buffin, "{ \"execute\": \"%s\" }\n", cmd);
  }

 size = strlen (buffin);

 if (send (sockmon, buffin, size, 0) != size)
  {
   printf ("mon send error : %s \n", strerror (errno));
   exit (1);
  }

 usleep (1000);

 if ((n = recv (sockmon, buffout, 399, 0)) < 0)
  {
   return 1;
  }
 buffout[n] = 0;

 printf ("(%s)=(%s) \n", buffin, buffout);
 return 0;
}
