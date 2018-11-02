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

const unsigned char linha[4]= {TL1,TL2,TL3,TL4};

unsigned char tc_tecla(unsigned int timeout)
{
  unsigned int to=0;
  unsigned char i;
  unsigned char ret=0;
  unsigned char tmp=PORTA;

  while(((to < timeout)||(!timeout))&&(!ret))  
  {
    for(i=0;i<4;i++)
    {
      PORTA|=linha[i];
      if(TC1){atraso_ms(20);if(TC1){while(TC1);ret= 1+(i*3);break;}};
      if(TC2){atraso_ms(20);if(TC2){while(TC2);ret= 2+(i*3);break;}};
      if(TC3){atraso_ms(20);if(TC3){while(TC3);ret= 3+(i*3);break;}};
      PORTA&=~linha[i];
    };
    atraso_ms(5);
    to+=5;
  }
  
  if(!ret)ret=255;
  if(ret == 11)ret=0;
  PORTA=tmp;
  return ret;
}


