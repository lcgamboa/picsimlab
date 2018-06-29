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

#include"rtc.h"

volatile char date[9];
volatile char time[9];


unsigned char getd(unsigned char nn)
{
 return ((nn & 0xF0)>>4)+0x30;
}

unsigned char getu(unsigned char nn)
{
  return (nn  & 0x0F)+0x30;
}

//--------------------- Reads time and date information from RTC (PCF8563)
void rtc_r(void) 
{
  unsigned char tmp;

  Soft_I2C_Start();
  Soft_I2C_Write(0xA2);
  Soft_I2C_Write(2);

  Soft_I2C_Start();
  Soft_I2C_Write(0xA3);
  tmp= 0x7F & Soft_I2C_Read(1); //segundos
  time[5]=':';
  time[6]=getd(tmp);
  time[7]=getu(tmp);
  time[8]=0;

  tmp= 0x7F & Soft_I2C_Read(1); //minutos
  time[2]=':';
  time[3]=getd(tmp);
  time[4]=getu(tmp);

  tmp= 0x3F & Soft_I2C_Read(1); //horas
  time[0]=getd(tmp);
  time[1]=getu(tmp);

  tmp= 0x3F & Soft_I2C_Read(1); //dia
  date[0]=getd(tmp);
  date[1]=getu(tmp);

  Soft_I2C_Read(1); //dia semana

  tmp= 0x1F & Soft_I2C_Read(1); //mes
  date[2]='/'; 
  date[3]=getd(tmp);
  date[4]=getu(tmp);

  tmp=  Soft_I2C_Read(1); //ano
  date[5]='/';
  date[6]=getd(tmp);
  date[7]=getu(tmp);
  date[8]=0;

  Soft_I2C_Stop();

}