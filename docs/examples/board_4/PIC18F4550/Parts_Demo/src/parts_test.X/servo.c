/* ########################################################################

   PICsim - PIC simulator http://sourceforge.net/projects/picsim/

   ########################################################################

   Copyright (c) : 2014-2017  Luis Claudio Gambôa Lopes

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
#include"config.h"
#include<xc.h> 
#include"servo.h"


#define T0INI (unsigned int)(65536-(18e-3* _XTAL_FREQ/4.0 )/4.0)
#define T0INIH (unsigned char)((T0INI&0xFF00)>>8) 
#define T0INIL (unsigned char)(T0INI&0x00FF)

#define OUT PORTBbits.RB0
#define TOUT TRISBbits.TRISB0


unsigned char T0PULSEH;
unsigned char T0PULSEL;

void interrupt isr()
{
  if(INTCONbits.TMR0IF)//every ~= 15ms
  {
     if(OUT)
     {    
       OUT=0;  
       TMR0H=T0INIH;
       TMR0L=T0INIL;
     }
     else
     {
       TMR0H=T0PULSEH;
       TMR0L=T0PULSEL;         
       OUT=1;
     }
     INTCONbits.TMR0IF=0;
  }
  
}

void servo_init(void)
{
    //Configura TMR0
   T0CONbits.T08BIT=0;
   T0CONbits.T0CS=0;
   T0CONbits.PSA=0;
   T0CONbits.T0PS2=0;//divide por 4 
   T0CONbits.T0PS1=0;
   T0CONbits.T0PS0=1;

   TMR0H=T0INIH;
   TMR0L=T0INIL;
   
   servo_pos(90);
   
   INTCONbits.TMR0IF=0;
   INTCONbits.TMR0IE=1;
   INTCONbits.GIE=1;
   
   T0CONbits.TMR0ON=1;
   
   TOUT=0;
   OUT=0;   
}
    
 
void servo_pos(unsigned char value)//0 to 180
{
    unsigned int  T0PULSE;
    T0PULSE=(unsigned int)(65536-((1.0e-3+(1.0e-3*value/180.0) )* _XTAL_FREQ/4.0 )/4.0);
    T0PULSEH=(T0PULSE&0xFF00)>>8;
    T0PULSEL=(T0PULSE&0x00FF);   
}