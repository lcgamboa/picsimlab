/* ########################################################################

   PICsim - PIC simulator http://sourceforge.net/projects/picsim/

   ########################################################################

   Copyright (c) : 2015  Luis Claudio Gambôa Lopes

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
#include"i2c.h"

volatile char date[10];
volatile char time[10];


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

  i2c_start();
  i2c_wb(0xA2);
  i2c_wb(2);

  i2c_start();
  i2c_wb(0xA3);
  tmp= 0x7F & i2c_rb(0); //segundos
  time[5]=':';
  time[6]=getd(tmp);
  time[7]=getu(tmp);
  time[8]=0;

  tmp= 0x7F & i2c_rb(0); //minutos
  time[2]=':';
  time[3]=getd(tmp);
  time[4]=getu(tmp);

  tmp= 0x3F & i2c_rb(0); //horas
  time[0]=getd(tmp);
  time[1]=getu(tmp);

  tmp= 0x3F & i2c_rb(0); //dia
  date[0]=getd(tmp);
  date[1]=getu(tmp);

  i2c_rb(0); //dia semana

  tmp= 0x1F & i2c_rb(0); //mes
  date[2]='/'; 
  date[3]=getd(tmp);
  date[4]=getu(tmp);

  tmp=  i2c_rb(1); //ano
  date[5]='/';
  date[6]=getd(tmp);
  date[7]=getu(tmp);
  date[8]=0;

  i2c_stop();

}

