/* ########################################################################

   PICsim - PIC simulator http://sourceforge.net/projects/picsim/

   ########################################################################

   Copyright (c) : 2010  Luis Claudio Gambôa Lopes

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

#include"eeprom_ext.h"

unsigned char e2pext_r(unsigned int addr)
{
  unsigned char ret;
  unsigned char ah;
  unsigned char al;

  ah=(addr&0x0100)>>8;
  al=addr&0x00FF;

  i2c1_start();
  if(ah)
  {
    i2c1_wr(0xA2);
  }
  else
  {
    i2c1_wr(0xA0);
  }
  i2c1_wr(al);

  i2c1_repeated_start();
  if(ah)
  {
    i2c1_wr(0xA3);
  }
  else
  {
    i2c1_wr(0xA1);
  }
  ret=i2c1_rd(0);
  i2c1_stop();

  return ret;        
}


void e2pext_w(unsigned int addr, unsigned char val)
{
  unsigned int tmp;
  unsigned char ah;
  unsigned char al;
  unsigned char nt;

  tmp=val;
  ah=(addr&0x0100)>>8;
  al=addr&0x00FF;
  nt=0;

  do
  {
    i2c1_start();
    if(ah)
    { 
      i2c1_wr(0xA2);
    }
    else
    {
      i2c1_wr(0xA0);
    }
    i2c1_wr(al);
    i2c1_wr(tmp);
    i2c1_stop();

    nt++;
  }
  while((tmp != e2pext_r(addr))&&(nt < 10));
}