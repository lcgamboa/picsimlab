/* ########################################################################

   PICsimLab - PIC laboratory simulator

   ########################################################################

   Copyright (c) : 2010-2017  Luis Claudio Gambôa Lopes

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


#include "board_pic.h"

#include"../picsimlab1.h"

//mplabx debugger
int mplabxd_init(void);
int mplabxd_loop(void);
void mplabxd_end(void);
int mplabxd_testbp(void);

void 
board_pic::MSetSerial(const char * port)
{
   pic_set_serial(&pic,port,0,0,0);
}


int
board_pic::MInit(const char * processor, const char * fname, float freq)
{
   int procn=getprocbyname (processor);
   if(procn == 0)
   {
     printf("Unknown processor %s !!\n",processor) ;  
     return 0;
   }
  
   int ret=pic_init(&pic,procn,fname,1,freq);
   pic.config[0] |= 0x0800; //disable DEBUG
   return ret;
}

void 
board_pic::MEnd(void)
{
  pic_end();
  //prog_end();
  mplabxd_end();
};

void 
board_pic::MEraseFlash(void)
{
   pic_erase_flash();
};


void 
board_pic::MSetFreq(float freq)
{
  pic.freq=freq;
};

float 
board_pic::MGetFreq(void)
{
  return pic.freq;
}

float 
board_pic::MGetInstClock(void)
{
  return pic.freq/4.0;
}

int 
board_pic::CpuInitialized(void)
{
  return 1;
}

void 
board_pic::MDumpMemory(const char * fname)
{
switch(getfprocbyname((const char*)proc.c_str ()))
  {
    case P16:
    case P16E:
    case P16E2:    
      write_ihx(fname);
      break;
    case P18:
      write_ihx18(fname);
      break;
  }


}

int
board_pic::DebugInit(void)
{
 return mplabxd_init();
}

void
board_pic::DebugLoop(void)
{
   if(Window1.Get_picpwr())
   {
      //prog_loop(&pic);
      mplabxd_loop();
   }
}

int 
board_pic::MGetPinCount(void)
{
  return pic.PINCOUNT;
}

String 
board_pic::MGetPinName(int pin)
{
  char cbuf[10];
  String pinname;

  pinname= getPinName(&pic,pin,cbuf);

  return pinname;
}

void 
board_pic::MSetPin(int pin, unsigned char value)
{
  pic_set_pin (pin,value);
}

void 
board_pic::MSetAPin(int pin, float value)
{
  pic_set_apin (pin,value);
}

unsigned char 
board_pic::MGetPin(int pin)
{
  return pic_get_pin (pin); 
}

const picpin * 
board_pic::MGetPinsValues(void)
{
  return pic.pins; 
}  
