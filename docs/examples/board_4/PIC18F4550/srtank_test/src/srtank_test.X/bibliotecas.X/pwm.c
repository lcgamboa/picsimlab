/* ########################################################################

   PICsim - PIC simulator http://sourceforge.net/projects/picsim/

   ########################################################################

   Copyright (c) : 2015-2017  Luis Claudio Gambôa Lopes

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

#include <xc.h>

#include"config.h"


 void PWM1_Init(unsigned int f)
  {
   unsigned int temp;
 //PWM Period = [(PR2) + 1] * 4 * TOSC *(TMR2 Prescale Value)
 //PWM Duty Cycle = (CCPRXL:CCPXCON<5:4>) *TOSC * (TMR2 Prescale Value)


   //desliga PWM
     CCP1CON=0x00;//CCP disabled
     //TRISCbits.TRISC2=1; //desliga saídas PWM


     PORTCbits.RC2=0; //deliga saídas PWM

     CCPR1L=0;//ou 255?


     //calculo TMR2

      T2CONbits.TMR2ON=0;
             
     temp=_XTAL_FREQ/(f*4l);
    
     if (temp < 256)
     {
       T2CONbits.T2CKPS=0;  //1
       PR2=temp;
     }
     else if(temp/4 < 256 )
     {
       T2CONbits.T2CKPS=1;  //4
       PR2=(temp+2)/4;
     }
     else
     {
       PR2=(temp+8)/16;
       T2CONbits.T2CKPS=2;  //16
     }

#ifndef _18F45K50
     T2CONbits.TOUTPS=0x00;  //1-16
#else     
     T2CONbits.T2OUTPS=0x00;
#endif
             
  }


  void PWM1_Start(void)
  {

      TRISCbits.TRISC2=0; //liga saídas PWM

      CCP1CON=0x0C; 

      T2CONbits.TMR2ON=1;
 
      //espera PWM normalizar

      PIR1bits.TMR2IF=0;
      while(PIR1bits.TMR2IF == 0);
      PIR1bits.TMR2IF=0;
      while(PIR1bits.TMR2IF == 0);
  }

  void PWM1_Stop(void)
  {
     //desliga PWM
     CCP1CON=0x00;//CCP disabled

      T2CONbits.TMR2ON=0;
  }


  void PWM1_Set_Duty(unsigned char d)
  {
      unsigned int temp;
      
      temp=(((unsigned long)(d))*((PR2<<2)|0x03))/255;

      CCPR1L= (0x03FC&temp)>>2;
      CCP1CON=((0x0003&temp)<<4)|0x0C;
  }

  
