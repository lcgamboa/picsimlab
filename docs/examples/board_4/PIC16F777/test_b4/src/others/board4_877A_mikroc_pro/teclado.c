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

#include "teclado.h"
#include "atraso.h"

const unsigned char linha[4]= {TL1,TL2,TL3};

unsigned char tc_tecla(unsigned int timeout)
{
  unsigned int to=0;
  unsigned char i;
  unsigned char ret=0;
  unsigned char tmp;

  tmp=PORTB;

  while(((to < timeout)||(!timeout))&&(!ret))  
  {
    for(i=0;i<3;i++)
    {
      PORTB|=~linha[i];
      if(!TC1){atraso_ms(20);if(!TC1){while(!TC1);ret= 1+i;break;}};
      if(!TC2){atraso_ms(20);if(!TC2){while(!TC2);ret= 4+i;break;}};
      if(!TC3){atraso_ms(20);if(!TC3){while(!TC3);ret= 7+i;break;}};
      if(!TC4){atraso_ms(20);if(!TC4){while(!TC4);ret= 10+i;break;}};
      PORTB &=linha[i];
    };
    atraso_ms(5);
    to+=5;
  }
  
  if(!ret)ret=255;
  if(ret == 11)ret=0;
  PORTB=tmp;
  return ret;
}