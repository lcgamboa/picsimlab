/* ########################################################################

   PICsim - PIC simulator http://sourceforge.net/projects/picsim/

   ########################################################################

   Copyright (c) : 2015-2017  Luis Claudio Gambôa Lopes

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
#include <xc.h>
#include"eeprom.h"

#ifndef _16F777
unsigned char eeprom_r(unsigned char addr)
{
 EECON1bits.EEPGD=0;   
 EECON1bits.CFGS=0;
 EEADR= addr;
 EECON1bits.RD=1;
 return EEDATA;
}


void eeprom_w(unsigned char addr,unsigned char val)
{
 EECON1bits.EEPGD=0;   
 EECON1bits.CFGS=0;
 EEADR= addr;
 EEDATA=val;
 
 EECON1bits.WREN=1;

 EECON2=0x55;
 EECON2=0xAA;

 EECON1bits.WR=1;
 while(EECON1bits.WR==1);

 EECON1bits.WREN=0;

 return;
}
#endif
