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
#include <i2c.h>

unsigned char e2pext_r(unsigned int addr)
{
  unsigned char ret;
  unsigned char ah;
  unsigned char al;

  ah=(addr&0x0100)>>8;
  al=addr&0x00FF;
  
  IdleI2C();
  StartI2C();
  while(SSPCON2bits.SEN);	
  if(ah)
  {
    WriteI2C(0xA2);
  }
  else
  {
    WriteI2C(0xA0);
  }
  IdleI2C();

  WriteI2C(al);
  IdleI2C();

  RestartI2C();
  while ( SSPCON2bits.RSEN );
  if(ah)
  {
    WriteI2C(0xA3);
  }
  else
  {
    WriteI2C(0xA1);
  }
  IdleI2C();
 
  ret=ReadI2C();
  IdleI2C();
  NotAckI2C();
  while(SSPCON2bits.ACKEN);

  StopI2C();
  while ( SSPCON2bits.PEN ); 

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
    IdleI2C();
    StartI2C();
    while(SSPCON2bits.SEN);
    if(ah)
    { 
      WriteI2C(0xA2);
    }
    else
    {
      WriteI2C(0xA0);
    }
    IdleI2C();
    WriteI2C(al);
    IdleI2C();
    WriteI2C(tmp);
    IdleI2C();
    StopI2C();
    while ( SSPCON2bits.PEN ); 
    nt++;
  }
  while((tmp != e2pext_r(addr))&&(nt < 10));
}
