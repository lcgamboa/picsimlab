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
#include"i2c.h"

char date[10];
char time[10];


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

  IdleI2C();

  StartI2C();
  while(SSPCON2bits.SEN);
  
  WriteI2C(0xD0);
  IdleI2C();

  WriteI2C(0);
  IdleI2C();

  RestartI2C();
  while ( SSPCON2bits.RSEN );

  WriteI2C(0xD1);
  IdleI2C();

  tmp= 0x7F & ReadI2C(); //segundos
  IdleI2C();
  AckI2C();
  while(SSPCON2bits.ACKEN);

  time[5]=':';
  time[6]=getd(tmp);
  time[7]=getu(tmp);
  time[8]=0;

  tmp= 0x7F & ReadI2C(); //minutos
  IdleI2C();
  AckI2C();
  while(SSPCON2bits.ACKEN);

  time[2]=':';
  time[3]=getd(tmp);
  time[4]=getu(tmp);

  tmp= 0x3F & ReadI2C(); //horas
  IdleI2C();
  AckI2C();
  while(SSPCON2bits.ACKEN);

  time[0]=getd(tmp);
  time[1]=getu(tmp);

  ReadI2C(); //dia semana
  IdleI2C();
  AckI2C();
  while(SSPCON2bits.ACKEN);


  tmp= 0x3F & ReadI2C(); //dia
  IdleI2C();
  AckI2C();
  while(SSPCON2bits.ACKEN);

  date[0]=getd(tmp);
  date[1]=getu(tmp);

  tmp= 0x1F & ReadI2C(); //mes
  IdleI2C();
  AckI2C();
  while(SSPCON2bits.ACKEN);

  date[2]='/'; 
  date[3]=getd(tmp);
  date[4]=getu(tmp);

  tmp=  ReadI2C(); //ano
  IdleI2C();
  NotAckI2C();
  while(SSPCON2bits.ACKEN);

  date[5]='/';
  date[6]=getd(tmp);
  date[7]=getu(tmp);
  date[8]=0;

  StopI2C();
  while ( SSPCON2bits.PEN ); 
}

