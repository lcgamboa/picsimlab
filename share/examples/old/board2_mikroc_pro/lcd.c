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

#include "lcd.h"


void lcd_wr(unsigned char val)
{
  unsigned char i;

  LCLK=0;

  for(i=0;i<8;i++)
  {
    LDAT=((val>>(7-i))&0x01);
    LCLK=1;
    LCLK=0;
  }
}

void lcd_cmd(unsigned char val)
{
        LENA=1;
        lcd_wr(val);
        LDAT=0;
        delay_ms(3);
        LENA=0;
        delay_ms(3);
        LENA=1;
}
 
void lcd_dat(unsigned char val)
{
        LENA=1;
        lcd_wr(val);
        LDAT=1;
        delay_ms(3);
        LENA=0;
        delay_ms(3);
        LENA=1;
}

void lcd_init(void)
{
        LENA=0;
        LDAT=0;
        delay_ms(20);
        LENA=1;
        
        lcd_cmd(0x30);
        delay_ms(5);
        lcd_cmd(0x30);
        delay_ms(1);
        lcd_cmd(L_CFG); //configura
        lcd_cmd(L_OFF);
        lcd_cmd(L_ON); //liga
        lcd_cmd(L_CLR); //limpa
        lcd_cmd(L_CFG); //configura
        lcd_cmd(L_L1);
}

void lcd_str(char* str)
{
 unsigned char i=0;
  
 while(str[i])
 {
   lcd_dat(str[i]);
   i++;
 }  
}