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

/* ----------------------------------------------------------------------- */

// PIC16F628A Configuration Bit Settings

#include "config_628A.h"

#include "atraso.h"
#include "display7s.h"
#include "eeprom.h"


void interrupt isr() {                                                                                                  /* interrupt service routine */
    /* << insert interrupt code >> */
}

void main() {

  unsigned char tmp;
  unsigned char i;

  CMCON=0x07;
  TRISA=0xFE;
  TRISB=0X00;

  
//  tmp=e2prom_r(0x10);
//  e2prom_w(0x10,tmp+1);
  
  while(1)
  {
//testa display 7s
    for(tmp=0;tmp<16;tmp++)
    {
      PORTB=display7s(tmp)|0x10;	
      atraso_ms(200);	
    }
    for(tmp=0;tmp<16;tmp++)
    {
      PORTB=display7s(tmp);
      atraso_ms(200);	
    }
  for(tmp=0;tmp<16;tmp++)
    {
      for(i=0;i<10;i++)
      { 
        PORTB=display7s(tmp);	
        atraso_ms(10);
        PORTB=display7s(tmp)|0x10;	
        atraso_ms(10);
      } 
    }
  PORTB=0;
//testa RA0
  for(i=0;i<4;i++)
  {
    RA0^=1;
    atraso_ms(500);
  }
 
//testa LEDs
  for(tmp=0;tmp<3;tmp++)
    {
      for(i=1;i>0;i=i*2)
      { 
        PORTB=i;
        atraso_ms(200);  
      }
    }
  PORTB=0;
  for(i=0;i<4;i++)
  {
    PORTB^=0xFF;
    atraso_ms(200);
  }  

//testa chaves
  while((PORTB & 0x0F) != 0x0F)
  {
    if(RA1 == 0)
    {
      PORTB|=0x01;
      while(RA1==0);
    }
    if(RA2 == 0)
    {
      PORTB|=0x02;
      while(RA2==0); 
    }
    if(RA3 == 0)
    {
      PORTB|=0x04;
      while(RA3==0); 
    }
    if(RA4 == 0)
    {
      PORTB|=0x08;
      while(RA4==0);
    }
  }
  PORTB=0; 

//testa LEDS2
  TRISA=0xE0;
  PORTA=0;
  for(i=0;i<4;i++)
  {
    RA1^=1;
    atraso_ms(200);
    RA2^=1;
    atraso_ms(200);
    RA3^=1;
    atraso_ms(200);
    RA4^=1;
    atraso_ms(200);
  }
  TRISA=0xFE;
//fim
   while(1);
  }
}
