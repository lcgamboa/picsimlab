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

void
board_pic::MSetSerial(const char * port)
{
 pic_set_serial (&pic, port, 0, 0, 0);
}

int
board_pic::MInit(const char * processor, const char * fname, float freq)
{
 int procn = getprocbyname (processor);
 if (procn == 0)
  {
   printf ("Unknown processor %s !!\n", processor);
   String sproc = GetSupportedDevices ();
   int f = sproc.find (lxT (","));
   procn = getprocbyname (sproc.substr (0, f));
  }

 int ret = pic_init (&pic, procn, fname, 1, freq);
 pic.config[0] |= 0x0800; //disable DEBUG

 pic.pins = (picpin*) realloc (pic.pins, sizeof (picpin)*256);

 return ret;
}

void
board_pic::MEnd(void)
{
 pic_end ();
 //prog_end();
 mplabxd_end ();
}

void
board_pic::MEraseFlash(void)
{
 pic_erase_flash ();
}

void
board_pic::MSetFreq(float freq)
{
 pic.freq = freq;
}

float
board_pic::MGetFreq(void)
{
 return pic.freq;
}

float
board_pic::MGetInstClock(void)
{
 return pic.freq / 4.0;
}

int
board_pic::CpuInitialized(void)
{
 return 1;
}

void
board_pic::MDumpMemory(const char * fname)
{
 switch (getfprocbyname ((const char*) proc.c_str ()))
  {
  case P16:
   write_ihx (fname);
   break;
  case P16E:
   write_ihx16e (fname);
   break;
  case P18:
   write_ihx18 (fname);
   break;
  }


}

int
board_pic::DebugInit(void)
{
 return mplabxd_init (this);
}

void
board_pic::DebugLoop(void)
{
 if (Window1.Get_mcupwr ())
  {
   //prog_loop(&pic);
   mplabxd_loop ();
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

 pinname = getPinName (&pic, pin, cbuf);

 return pinname;
}

void
board_pic::MSetPin(int pin, unsigned char value)
{
 pic_set_pin (pin, value);
}

void
board_pic::MSetPinDOV(int pin, unsigned char ovalue)
{
 pic_set_pin_DOV (pin, ovalue);
}

void
board_pic::MSetAPin(int pin, float value)
{
 pic_set_apin (pin, value);
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

void
board_pic::MStep(void)
{
 pic_step ();
 if (pic.s2 == 1)pic_step ();
}

void
board_pic::MStepResume(void)
{
 if (pic.s2 == 1)pic_step ();
}

int
board_pic::MTestBP(unsigned int bp)
{
 return ((pic.pc == bp)&&(pic.s2 == 0));
}

void
board_pic::MReset(int flags)
{
 pic_reset (flags);
}

unsigned short *
board_pic::MGetProcID_p(void)
{
 return (unsigned short *)&pic.processor;
}

unsigned int 
board_pic::MGetPC(void)
{
 return pic.pc;
}

void 
board_pic::MSetPC(unsigned int pc)
{
 pic.pc= pc;
}

unsigned char *
board_pic::MGetRAM_p(void)
{
 return pic.ram;
}

unsigned char *
board_pic::MGetROM_p(void)
{
 return (unsigned char *) pic.prog;
}

unsigned char *
board_pic::MGetCONFIG_p(void)
{
 return (unsigned char *) pic.config;
}

unsigned char *
board_pic::MGetID_p(void)
{
 return (unsigned char *) pic.id;
}

unsigned char *
board_pic::MGetEEPROM_p(void)
{
 return pic.eeprom;
}

unsigned int
board_pic::MGetRAMSize(void)
{
 return pic.RAMSIZE;
}

unsigned int
board_pic::MGetROMSize(void)
{
 return pic.ROMSIZE*2;
}

unsigned int
board_pic::MGetCONFIGSize(void)
{
 return pic.CONFIGSIZE*2;

}

unsigned int
board_pic::MGetIDSize(void)
{
 return pic.IDSIZE*2;
}

unsigned int
board_pic::MGetEEPROM_Size(void)
{
 return pic.EEPROMSIZE;
}
