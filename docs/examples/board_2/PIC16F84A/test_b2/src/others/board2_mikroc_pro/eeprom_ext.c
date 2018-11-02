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

#include "i2c.h"
#include"eeprom_ext.h"

unsigned char e2pext_r(unsigned int addr)
{
  unsigned char ret;
  unsigned char ah;
  unsigned char al;

  ah=(addr&0xFF00)>>8;
  al=addr&0x00FF;

  Soft_I2C_Start();
  Soft_I2C_Write(0xA0);
  Soft_I2C_Write(ah);
  Soft_I2C_Write(al);
  Soft_I2C_Start();
  Soft_I2C_Write(0xA1);
  ret=Soft_I2C_Read(1);
  Soft_I2C_Stop();

  return ret;        
}


void e2pext_w(unsigned int addr, unsigned char val)
 {
  unsigned int tmp;
  unsigned char ah;
  unsigned char al;
  unsigned char nt=0;

  tmp=val;
  ah=(addr&0xFF00)>>8;
  al=addr&0x00FF;

  do
  {
    Soft_I2C_Start();
    Soft_I2C_Write(0xA0);
    Soft_I2C_Write(ah);
    Soft_I2C_Write(al);
    Soft_I2C_Write(tmp);
    Soft_I2C_Stop();

    nt++;
  }
  while((tmp != e2pext_r(addr))&&(nt < 10));
}