/* ########################################################################

   PICsim - PIC simulator http://sourceforge.net/projects/picsim/

   ########################################################################

   Copyright (c) : 2011  Luis Claudio Gambôa Lopes

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

#include"eeprom.h"

unsigned char e2prom_r(unsigned char addr)
{
 EEADR= addr;
 RD_bit=1;
 return EEDATA;
}


void e2prom_w(unsigned char addr,unsigned char val)
{
 EEADR= addr;
 EEDATA=val;
 
 WREN_bit=1;

 EECON2=0x55;
 EECON2=0xAA;

 WR_bit=1;
 while(WR_bit ==1);

 WREN_bit=0;

 return;
}