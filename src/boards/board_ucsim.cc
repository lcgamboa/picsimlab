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


#include "../../../ucsim-0.6/picsimlab/ucsimlib.h"

#include "board_ucsim.h"
#include"../picsimlab1.h"


const unsigned char UCSIM_PORTS[5] = {0,1,2,3,0xFF};

board_ucsim::board_ucsim(void) { }

void
board_ucsim::MSetSerial(const char * port) {
 /*
 set_serial (&pic,0, port, 0, 0, 0);
 set_serial (&pic,1, "", 0, 0, 0);
  */ }

int
board_ucsim::MInit(const char * processor, const char * fname, float freq)
{


 pins_reset ();

 ucsim_init (S51,"C51", "8M", fname, "/dev/tnt2");

 return 0; //ret;
}

void
board_ucsim::MEnd(void) {
 ucsim_end(); //TODO
}

void
board_ucsim::MEraseFlash(void) {
 //erase_flash ();
}

void
board_ucsim::MSetFreq(float freq_)
{
 freq = freq_;
}

float
board_ucsim::MGetFreq(void)
{
 return freq;
}

float
board_ucsim::MGetInstClock(void)
{
 return freq;
}

int
board_ucsim::CpuInitialized(void)
{
 return 1;
}

void
board_ucsim::DebugLoop(void) { }

String
board_ucsim::MGetPinName(int pin)
{
 String pinname = "error";

 switch (pin)
  {
  case 1:
   pinname = "P1.0";
   break;
  case 2:
   pinname = "P1.1";
   break;
  case 3:
   pinname = "P1.2";
   break;
  case 4:
   pinname = "P1.3";
   break;
  case 5:
   pinname = "P1.4";
   break;
  case 6:
   pinname = "P1.5";
   break;
  case 7:
   pinname = "P1.6";
   break;
  case 8:
   pinname = "P1.7";
   break;
  case 9:
   pinname = "RST";
   break;
  case 10:
   pinname = "P3.0";
   break;
  case 11:
   pinname = "P3.1";
   break;
  case 12:
   pinname = "P3.2";
   break;
  case 13:
   pinname = "P3.3";
   break;
  case 14:
   pinname = "P3.4";
   break;
  case 15:
   pinname = "P3.5";
   break;
  case 16:
   pinname = "P3.6";
   break;
  case 17:
   pinname = "P3.7";
   break;
  case 18:
   pinname = "XTAL2";
   break;
  case 19:
   pinname = "XTAL1";
   break;
  case 20:
   pinname = "GND";
   break;
  case 21:
   pinname = "P2.0";
   break;
  case 22:
   pinname = "P2.1";
   break;
  case 23:
   pinname = "P2.2";
   break;
  case 24:
   pinname = "P2.3";
   break;
  case 25:
   pinname = "P2.4";
   break;
  case 26:
   pinname = "P2.5";
   break;
  case 27:
   pinname = "P2.6";
   break;
  case 28:
   pinname = "P2.7";
   break;
  case 29:
   pinname = "/PSEN";
   break;
  case 30:
   pinname = "ALE";
   break;
  case 31:
   pinname = "/EA";
   break;
  case 32:
   pinname = "P0.7";
   break;
  case 33:
   pinname = "P0.6";
   break;
  case 34:
   pinname = "P0.5";
   break;
  case 35:
   pinname = "P0.4";
   break;
  case 36:
   pinname = "P0.3";
   break;
  case 37:
   pinname = "P0.2";
   break;
  case 38:
   pinname = "P0.1";
   break;
  case 39:
   pinname = "P0.0";
   break;
  case 40:
   pinname = "VCC";
   break;
  }
 return pinname;
}

void
board_ucsim::MDumpMemory(const char * fname) 
{ 
   ucsim_dump (fname);
}

int
board_ucsim::DebugInit(int dtyppe) //argument not used in picm only mplabx
{
 return 0; //!mplabxd_init (this, Window1.Get_debug_port ()) - 1;
}

int
board_ucsim::MGetPinCount(void)
{
 return 40;
}

void
board_ucsim::pins_reset(void)
{
 for (int p = 0; p < MGetPinCount (); p++)
  {
   String pname = MGetPinName (p + 1);
   if (pname[0] == 'P')
    {
     pins[p].port = (unsigned char *)&UCSIM_PORTS[pname[1] - '0'];
     pins[p].pord = pname[3] - '0';
    }
   else
    {
     pins[p].port = (unsigned char *)&UCSIM_PORTS[4];
     pins[p].pord = -1;
    }
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
board_ucsim::MSetPin(int pin, unsigned char value) 
{ 
 ucsim_set_pin (*pins[pin-1].port, pins[pin-1].pord, value);
}

void
board_ucsim::MSetPinDOV(int pin, unsigned char ovalue) {
 //set_pin_DOV (pin, ovalue);
}

void
board_ucsim::MSetAPin(int pin, float value) {
 //set_apin (pin, value);
}

unsigned char
board_ucsim::MGetPin(int pin)
{
 return ucsim_get_pin (*pins[pin-1].port, pins[pin-1].pord);
}

void
board_ucsim::MReset(int flags) 
{ 
 ucsim_reset ();
}

const picpin *
board_ucsim::MGetPinsValues(void)
{
 return pins;
}

void
board_ucsim::MStep(void)
{
 unsigned char p[4];
 
 ucsim_step ();
 
 p[0]=ucsim_get_port (0);
 p[1]=ucsim_get_port (1);
 p[2]=ucsim_get_port (2);
 p[3]=ucsim_get_port (3);
 
 for(int i=0; i< MGetPinCount (); i++)
  {
    if(*pins[i].port < 4)
     {
      pins[i].value = (p[*pins[i].port] &  (1<< pins[i].pord))>0;
     }
  }
 
}

void
board_ucsim::MStepResume(void) {
 //if (pic.s2 == 1)step ();
}

