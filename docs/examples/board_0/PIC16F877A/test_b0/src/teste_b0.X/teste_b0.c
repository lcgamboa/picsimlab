/* ########################################################################

   PICsim - PIC simulator http://sourceforge.net/projects/picsim/

   ########################################################################

   Copyright (c) : 2020  Luis Claudio Gambôa Lopes

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

#include <xc.h>

#ifdef _18F452
#include"config_452.h"
#endif
#ifdef _18F4620
#include"config_4620.h"
#endif
#ifdef _18F4520
#include"config_4520.h"
#endif
#ifdef _18F4550
#include"config_4550.h"
#endif
#ifdef _18F45K50
#include"config_45K50.h"
#endif
#ifdef _16F877A
#include"config_877A.h"
#endif
#ifdef _16F777
#include"config_777.h"
#endif
#ifdef _16F1939
#include"config_1939.h"
#endif

#include "atraso.h"
#include "lcd.h"
#include "display7s.h"
#if !defined(_18F4550) && !defined(_18F45K50) 
#include "i2c.h"
#include "eeprom_ext.h"
#include "rtc.h"
#endif
#include "serial.h"
#ifndef _16F777
#include "eeprom.h"
#endif
#include "adc.h"
#include "itoa.h"
#include "teclado.h"


#ifndef _16F777
__EEPROM_DATA(0x00,0x11,0x22,0x33,0x44,0x55,0x66,0x77);
__EEPROM_DATA(0x88,0x99,0xAA,0xBB,0xCC,0xDD,0xEE,0xFF);
#endif

unsigned char cnt;
unsigned int t1cont;

void main() 
{
  unsigned char i;
  unsigned char tmp;
  unsigned int tmpi;

  char str[6];


  TRISA=0xC3;
  TRISB=0x01;
  TRISC=0x01;
  TRISD=0x00;
  TRISE=0x00;
#if defined(_16F1939)
  //all digital
  ANSELA=0x00;
  ANSELB=0x00;
  ANSELD=0x00;
  ANSELE=0x00;
#endif  
  

#if defined(_16F877A) || defined(_16F777)  

#elif defined(_16F1939) || defined (_16F1789) || defined (_16F1788) 
  // a voir s(il y a quelque chose a faire sur RBPU)    
#else  
  INTCON2bits.RBPU=0;
#endif

  lcd_init();
  lcd_cmd(L_NCR);
#if !defined(_18F4550) && !defined(_18F45K50) 
  i2c_init();
#endif  
  serial_init();
  adc_init();
    
#if defined(_18F452)
  ADCON1=0x06;
#elif defined(_18F45K50) || defined(_16F1939)
  ANSELA=0x00;
#else
  ADCON1=0x0F;
  CMCON=0x07;
#endif
/*
//dip
  TRISB=0x03;
  lcd_cmd(L_CLR);
  lcd_cmd(L_L1);
  lcd_str("To start");
  lcd_cmd(L_L2);
  lcd_str("Press. RB1");
  while(PORTBbits.RB1);

//testa caracter especial
  lcd_cmd(L_CLR);
  lcd_cmd(L_L1);
  lcd_str("   LCD Test ");
  
  for(i=0;i<10;i++)
  {
    atraso_ms(200);
    lcd_cmd(0x18);//display shift
  }
  
  for(i=0;i<10;i++)
  {
    atraso_ms(200);
    lcd_cmd(0x1C);//display shift
  }
   lcd_cmd(L_CR);
  for(i=0;i<10;i++)
  {
    atraso_ms(200);
    lcd_cmd(0x10);
  }
  for(i=0;i<10;i++)
  {
    atraso_ms(200);
    lcd_cmd(0x14);
  }
   lcd_cmd(L_NCR);

  lcd_cmd(0x40);//endereço

  lcd_dat(0x11);
  lcd_dat(0x19);
  lcd_dat(0x15);
  lcd_dat(0x13);
  lcd_dat(0x13);
  lcd_dat(0x15);
  lcd_dat(0x19);
  lcd_dat(0x11);

  lcd_dat(0x0E);
  lcd_dat(0x11);
  lcd_dat(0x0E);
  lcd_dat(0x05);
  lcd_dat(0x0E);
  lcd_dat(0x14);
  lcd_dat(0x0A);
  lcd_dat(0x11);


  lcd_cmd(L_L2);

  for(i=0;i<16;i++)
  {
    lcd_dat(i%2);
    atraso_ms(100);
  }

//teste lcd
  lcd_cmd(L_CLR);
  lcd_cmd(L_L1);
  lcd_str("   LCD Test");
  
  for(i=32;i>=32;i++)
  {
    if((i%16) == 0)lcd_cmd(L_L2); 
    lcd_dat(i);
    atraso_ms(50);
  }

  atraso_ms(100);
  lcd_cmd(L_CLR);
  lcd_cmd(L_L1);
  lcd_str("   LCD Test");
  lcd_cmd(L_L2);
  lcd_str("       Ok");
  atraso_ms(500);

//testa display 7s

  lcd_cmd(L_CLR);
  lcd_cmd(L_L1);
  lcd_str("   7 Seg Test ");


#ifdef _18F452
  ADCON1=0x06;
#else
#ifndef _18F45K50
  ADCON1=0x0F;
#else
  ANSELA=0x00;  
#endif  
#endif
    for(i=0;i<4;i++)
    {
      switch(i)
      {
         case 0: 
           PORTA=0x20;
           break;
         case 1: 
           PORTA=0x10;
           break;
         case 2: 
           PORTA=0x08;
           break;
         case 3: 
           PORTA=0x04;
           break;
       }

      for(tmp=0;tmp<16;tmp++)
      {
        PORTD=display7s(tmp);	 
        atraso_ms(200);	
      }
    }

    
  PORTD=0;


//testa LEDs
  TRISB=0x00;
  lcd_cmd(L_CLR);
  lcd_cmd(L_L1+3);
  lcd_str("LEDs Test ");

  for(tmp=0;tmp<3;tmp++)
    {
      for(i=1;i > 0;i=i*2)
      { 
        PORTB=i;
        PORTD=i; 
        atraso_ms(200);  
      }
    }
  PORTB=0;
  PORTD=0;
  for(i=0;i<4;i++)
  {
    PORTB^=0xFF;
    PORTD^=0xFF;
    atraso_ms(200);
  }  

//testa chaves


  lcd_cmd(L_CLR);
  lcd_cmd(L_L1);
  lcd_str("  Switchs Test");

  tmp=0;
  
  TRISA|=0x20;

  TRISB=0x3F;
  lcd_cmd(L_L2);
  lcd_str(" Press RB0          ");
  while(PORTBbits.RB0);
  lcd_cmd(L_L2+12);
  lcd_str("Ok");
  atraso_ms(500);
  
  lcd_cmd(L_L2);
  lcd_str(" Press RB1          ");
  while(PORTBbits.RB1);
  lcd_cmd(L_L2+12);
  lcd_str("Ok");
  atraso_ms(500);
  
  lcd_cmd(L_L2);
  lcd_str(" Press RB2          ");
  while(PORTBbits.RB2);
  lcd_cmd(L_L2+12);
  lcd_str("Ok");
  atraso_ms(500);
  
  lcd_cmd(L_L2);
  lcd_str(" Press RB3          ");
  while(PORTBbits.RB3);
  lcd_cmd(L_L2+12);
  lcd_str("Ok");
  atraso_ms(500);
  
  lcd_cmd(L_L2);
  lcd_str(" Press RB4          ");
  while(PORTBbits.RB4);
  lcd_cmd(L_L2+12);
  lcd_str("Ok");
  atraso_ms(500);
  
  
  lcd_cmd(L_L2);
  lcd_str(" Press RB5          ");
  while(PORTBbits.RB5);
  lcd_cmd(L_L2+12);
  lcd_str("Ok");
  atraso_ms(500);
  
  lcd_cmd(L_L2);
  lcd_str(" Press RA5          ");
  while(PORTAbits.RA5);
  lcd_cmd(L_L2+12);
  lcd_str("Ok");
  atraso_ms(500);
          
  PORTB=0; 
 
  
//teste serial
 lcd_cmd(L_CLR);
 lcd_cmd(L_L1);
 lcd_str("Serial TX Test");
// testar ? 
  lcd_cmd(L_L2);
  lcd_str(" (Y=RB0 N=RB1) ?");

  TRISB=0x03;
  while(PORTBbits.RB0 && PORTBbits.RB1 );

  if(!PORTBbits.RB0)
  {

  TRISCbits.TRISC7=1; //RX
  TRISCbits.TRISC6=0; //TX


  lcd_cmd(L_L2+4);
  lcd_str("9600 8N1");

  serial_tx_str("\r\n Picsimlab\r\n Serial Test TX\r\n");

  for(i=0;i<4;i++)
  {
    serial_tx(i+0x30);
    serial_tx_str(" PicsimLab\r\n");
  }
  atraso_ms(1000);

  lcd_cmd(L_CLR);
  lcd_cmd(L_L1);
  lcd_str("Serial RX Test");
  serial_tx_str(" Type!\r\n");
  for(i=0;i<32;i++)
  {
    if(!(i%16))
    {
       lcd_cmd(L_L2);
       serial_tx_str("\r\n");
    }
    tmp=serial_rx(2000);
    lcd_dat(tmp);
    serial_tx(tmp);
  }
  atraso_ms(100);
  }
  */
//teste ADC
  
  TRISA=0x07;
  
#if defined (_18F452)
  ADCON1=0x02;
#elif defined(_18F45K50) || defined(_16F1939)
  ANSELA=0x07; 
#else
  ADCON1=0x0B;
#endif  

  adc_init();
  
  lcd_cmd(L_CLR);
  lcd_cmd(L_L1);
  lcd_str(" ADC (P1) Test");

  for(i=0; i< 200; i++)
  {
    tmp=(adc_amostra(0)*10)/204;
    lcd_cmd(L_L2+6);
    itoa(tmp,str);
    lcd_dat(str[3]);
    lcd_dat(',');
    lcd_dat(str[4]);
    lcd_dat('V');
    atraso_ms(10);
  }

  lcd_cmd(L_CLR);
  lcd_cmd(L_L1);
  lcd_str(" ADC (P2) Test");

  for(i=0; i< 200; i++)
  {
    tmp=((unsigned int)adc_amostra(1)*10)/204;
    lcd_cmd(L_L2+6);
    itoa(tmp,str);
    lcd_dat(str[3]);
    lcd_dat(',');
    lcd_dat(str[4]);
    lcd_dat('V');
    atraso_ms(10);
  }


//teste TEMP
  lcd_cmd(L_CLR);
  lcd_cmd(L_L1);
  lcd_str("   TEMP. Test");
  

  for(i=0; i< 100; i++)
  {
    tmpi=(adc_amostra(2)*10)/2;
    lcd_cmd(L_L2+5);
    itoa(tmpi,str);
    lcd_dat(str[2]);
    lcd_dat(str[3]);
    lcd_dat(',');
    lcd_dat(str[4]);
    lcd_dat('C');
    atraso_ms(20);
  }


//teste Aquecimento
  lcd_cmd(L_CLR);
  lcd_cmd(L_L1);
  lcd_str("   Heater Test");
  PORTCbits.RC5=1;
  for(i=0; i< 100; i++)
  {
    tmpi=(adc_amostra(2)*10)/2;
    lcd_cmd(L_L2+5);
    itoa(tmpi,str);
    lcd_dat(str[2]);
    lcd_dat(str[3]);
    lcd_dat(',');
    lcd_dat(str[4]);
    lcd_dat('C');
    atraso_ms(50);
  }
  PORTCbits.RC5=0;


//teste Resfriamento

  TRISCbits.TRISC0=1;
  lcd_cmd(L_CLR);
  lcd_cmd(L_L1);
  lcd_str("   Cooler Test");

//timer0 temporizador
#if defined (_16F1939) || defined (_16F1789)|| defined (_16F1788)
    // cofuguration de base à écrire
 OPTION_REGbits.TMR0CS=0;
 OPTION_REGbits.PSA=0;
 OPTION_REGbits.PS0=0;
 OPTION_REGbits.PS1=0;
 OPTION_REGbits.PS2=1;   
#elif  defined(_16F877A) || defined(_16F777) 
 OPTION_REGbits.T0CS=0;
 OPTION_REGbits.PSA=0;
 OPTION_REGbits.PS0=0;
 OPTION_REGbits.PS1=0;
 OPTION_REGbits.PS2=1;
#else
 T0CONbits.T0CS=0;
 T0CONbits.PSA=0;
 T0CONbits.T08BIT=1;
 T0CONbits.T0PS0=0; // divide por 32
 T0CONbits.T0PS1=0;
 T0CONbits.T0PS2=1;
 T0CONbits.TMR0ON=1;
#endif
 
 
 INTCONbits.T0IE=1;

//T = 32x250x125 = 1segundo;

//timer1 contador
 T1CONbits.TMR1CS=1;
 T1CONbits.T1CKPS1=0;
 T1CONbits.T1CKPS0=0;


 INTCONbits.T0IF=0;
#if defined(_16F877A) || defined(_16F777)  || defined(_16F1939) 
 TMR0=6;
#else
 TMR0H=0;
 TMR0L=6; //250
#endif
 cnt=125; 
 INTCONbits.GIE=1;

 TMR1H=0;
 TMR1L=0;
 T1CONbits.TMR1ON=1;

  PORTCbits.RC2=1;
  for(i=0; i< 150; i++)
  {
    tmpi=(adc_amostra(2)*10)/2;
    lcd_cmd(L_L2+2);
    itoa(tmpi,str);
    lcd_dat(str[2]);
    lcd_dat(str[3]);
    lcd_dat(',');
    lcd_dat(str[4]);
    lcd_dat('C');

    lcd_cmd(L_L2+8);
    itoa(t1cont,str);
    lcd_dat(str[1]);
    lcd_dat(str[2]);
    lcd_dat(str[3]);
    lcd_dat(str[4]);
    lcd_dat('R');
    lcd_dat('P');
    lcd_dat('S');

    atraso_ms(10);
  }

  INTCONbits.GIE=0;
  PORTCbits.RC2=0;


#ifdef _18F452
  ADCON1=0x06;
#elif defined(_18F45K50) || defined(_16F1939)
  ANSELA=0x00;
#else
  ADCON1=0x0F;    
#endif
/*
#if !defined(_18F4550) && !defined(_18F45K50)
//teste RTC
  lcd_cmd(L_CLR);
  lcd_cmd(L_L1);
  lcd_str("RTC Test");
  
//ajuste  rtc_w();

//  rtc_w();
 
  rtc_r();
  lcd_cmd(L_L2);
  lcd_str((const char *)date);
   atraso_ms(2000);
  for(i=0;i<20;i++)
  {
    rtc_r();
    lcd_cmd(L_L2);;
    lcd_str((const char *)time);
    atraso_ms(500); 
  }
#endif


  
  TRISB=0xF0;
  
//teste Teclado
  lcd_cmd(L_CLR);
  lcd_cmd(L_L1+2);
  lcd_str("Keypad Test");

  lcd_cmd(L_L2+1);

  i=0;
  while(i<14)
  {
    TRISD=0x0F;
    tmp=tc_tecla(2000);//+0x30;
    TRISD=0x00;
    lcd_dat(tmp);
    i++;
  }

*/
  
#ifndef _16F777
//teste EEPROM INT
  lcd_cmd(L_CLR);
  lcd_cmd(L_L1);
  lcd_str("INT EEPROM Test");
// testar ? 
  lcd_cmd(L_L2);
  lcd_str(" (Y=RB0 N=RB1) ?");

  TRISB=0x03;
   do
  {
      tmp=PORTB & 0x03;
  }
  while(tmp == 0x03);

  if(tmp == 0x02)
  {
    tmp=e2prom_r(10);
    lcd_dat(tmp);

    e2prom_w(10,0xA5);
    e2prom_w(10,0x5A);
    i=e2prom_r(10);

    e2prom_w(10,tmp);

    lcd_cmd(L_CLR);
    lcd_cmd(L_L1);
    lcd_str("INT EEPROM Test");
    lcd_cmd(L_L2);
    if(i == 0x5A) 
      lcd_str("       OK");
    else
      lcd_str("      ERROR");

    atraso_ms(1000);
  }
  else
  {
    while(PORTBbits.RB1 == 0);
  }
#endif

#if !defined(_18F4550) && !defined(_18F45K50)
//teste EEPROM EXT
  lcd_cmd(L_CLR);
  lcd_cmd(L_L1);
  lcd_str("EXT EEPROM Test");
// testar ? 
  lcd_cmd(L_L2);
  lcd_str(" (Y=RB0 N=RB1) ?");

 
  TRISB=0x03;
  do
  {
      tmp=PORTB & 0x03;
  }
  while(tmp == 0x03);

  if(tmp == 0x02)
  {
    tmp=e2pext_r(10);
    lcd_dat(tmp);

    e2pext_w(10,0xA5);
    e2pext_w(10,0x5A);
    i=e2pext_r(10);

    e2pext_w(10,tmp);

    lcd_cmd(L_CLR);
    lcd_cmd(L_L1);
    lcd_str("EXT EEPROM Test");
    lcd_cmd(L_L2);
    if(i == 0x5A) 
      lcd_str("       OK");
    else
      lcd_str("      ERROR");

    atraso_ms(1000);
  }
  else
  {
    while(PORTBbits.RB1 == 0);
  }
#endif
  TRISB=0x00;
  PORTB=0;


//fim teste 
  lcd_cmd(L_CLR);
  lcd_cmd(L_L1);
  lcd_str("      End");
  lcd_cmd(L_L2);
  lcd_str(" Press RST");

  serial_tx_str("\r\n End!\r\n");


   while(1);

}



//----------------------------------------------------------------------------
// High priority interrupt routine

void  interrupt isrh()
{
  cnt--; 
  if(!cnt)
  {
//executada a cada 1 segundo
     t1cont=(((unsigned int)TMR1H<<8)|(TMR1L))/7; //ventilador com 7 pas
     cnt=125;     
     TMR1H=0;
     TMR1L=0;     
  }
  INTCONbits.T0IF=0;
#if defined(_16F877A) || defined(_16F777) || defined(_16F1939) 
  TMR0=6;  
#else
  TMR0H=0;
  TMR0L=6; //250 
#endif
}

//----------------------------------------------------------------------------

#if defined(_16F877A) || defined(_16F777) || defined(_16F1939)  
#else
void  interrupt low_priority isrl()
{
#asm
	NOP
#endasm
}
#endif