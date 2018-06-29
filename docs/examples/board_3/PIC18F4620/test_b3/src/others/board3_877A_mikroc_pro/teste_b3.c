/* ########################################################################

   PICsim - PIC simulator http://sourceforge.net/projects/picsim/

   ########################################################################

   Copyright (c) : 2011  Luis Claudio Gamboa Lopes

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

#include "display7s.h"
//#include "i2c.h"
#include "eeprom_ext.h"

// LCD module connections
sbit LCD_RS at RE0_bit;
sbit LCD_EN at RE1_bit;
sbit LCD_D4 at RD4_bit;
sbit LCD_D5 at RD5_bit;
sbit LCD_D6 at RD6_bit;
sbit LCD_D7 at RD7_bit;

sbit LCD_RS_Direction at TRISE0_bit;
sbit LCD_EN_Direction at TRISE1_bit;
sbit LCD_D4_Direction at TRISD4_bit;
sbit LCD_D5_Direction at TRISD5_bit;
sbit LCD_D6_Direction at TRISD6_bit;
sbit LCD_D7_Direction at TRISD7_bit;
// End LCD module connections


/*
unsigned char cnt;
unsigned int t1cont;

void interrupt() 
{
  cnt--;
  if(!cnt)
  {
//executada a cada 1 segundo
     TMR1ON_bit=0;
     t1cont=(((unsigned int)TMR1H<<8)|(TMR1L))/7; //ventilador com 7 p�s
     cnt=125; 
     TMR1H=0;
     TMR1L=0;
     TMR1ON_bit=1;
  }
  TMR2IF_bit=0;
}
*/
/*
// Software I2C connections
sbit Soft_I2C_Scl           at RC3_bit;
sbit Soft_I2C_Sda           at RC4_bit;
sbit Soft_I2C_Scl_Direction at TRISC3_bit;
sbit Soft_I2C_Sda_Direction at TRISC4_bit;
// End Software I2C connections
*/

char* codetxt_to_ramtxt(const char* ctxt){
static char txt[45];
char i;
  for(i =0; txt[i] = ctxt[i]; i++);
  return txt;
}

void main() 
{
  unsigned char i;
  unsigned char tmp;
 // unsigned int tmpi;
 // unsigned int tout;

//  char str[6];

  ADCON1=0x06;
  TRISA=0xFF;
  TRISB=0x00;
  TRISC=0x01;
  TRISD=0x00;
  TRISE=0xFC;

  lcd_init();
  I2C1_Init(10000);
  
//   UART1_Init(9600);
 // adc_init();


//teste lcd
  lcd_cmd(_LCD_CLEAR);
  lcd_cmd(_LCD_FIRST_ROW);
  Lcd_Out_Cp(codetxt_to_ramtxt("   Teste LCD"));
  
  for(i=32;i<128;i++)
  {
    if((i%16) == 0)lcd_cmd(_LCD_SECOND_ROW);
    Lcd_Chr_Cp(i);
    delay_ms(50);
  }

  delay_ms(100);
  lcd_cmd(_LCD_CLEAR);
  lcd_cmd(_LCD_FIRST_ROW);
  Lcd_Out_Cp(codetxt_to_ramtxt("   Teste LCD"));
  Lcd_Cmd(_LCD_SECOND_ROW);
  Lcd_Out_Cp(codetxt_to_ramtxt("       Ok"));
  delay_ms(500);

//testa display 7s

  Lcd_Cmd(_LCD_CLEAR);
  Lcd_Cmd(_LCD_FIRST_ROW);
  Lcd_Out_Cp(codetxt_to_ramtxt("   Teste 7 Seg"));


    RB7_bit=1;
    for(i=0;i<4;i++)
    {
      PORTB=0x10<<i;  
      for(tmp=0;tmp<16;tmp++)
      {
        PORTD=display7s(tmp);        
        delay_ms(200);
      }
    }

  PORTD=0;
 
//testa LEDs

  Lcd_Cmd(_LCD_CLEAR);
  Lcd_Cmd(_LCD_FIRST_ROW+3);
  Lcd_Out_Cp(codetxt_to_ramtxt("Teste LEDs"));

  for(tmp=0;tmp<3;tmp++)
    {
      for(i=1;i < 16;i=i*2)
      { 
        PORTB=i;
        delay_ms(200);
      }
    }
  PORTB=0;
  for(i=0;i<4;i++)
  {
    PORTB^=0x0F;
    delay_ms(200);
  }  

//testa chaves


  Lcd_Cmd(_LCD_CLEAR);
  Lcd_Cmd(_LCD_FIRST_ROW);
  Lcd_Out_Cp(codetxt_to_ramtxt("  Teste Chaves"));

  tmp=0;

  while((tmp & 0x0F) != 0x0F)
  {
    TRISB=0x0F;

    if(RB0_bit == 0)
    {
      tmp|=0x01;
    }
    if(RB1_bit == 0)
    {
      tmp|=0x02;
    }
    if(RB2_bit == 0)
    {
      tmp|=0x04;
    }
    if(RB3_bit == 0)
    {
      tmp|=0x08;
    }
    
    TRISB=0x00;
    PORTB=tmp;
    delay_ms(10);
  }
  PORTB=0; 


//teste EEPROM INT
  Lcd_Cmd(_LCD_CLEAR);
  Lcd_Cmd(_LCD_FIRST_ROW);
  Lcd_Out_Cp(codetxt_to_ramtxt("Teste EEPROM INT"));
// testar ? 
  Lcd_Cmd(_LCD_SECOND_ROW);
  Lcd_Out_Cp(codetxt_to_ramtxt(" (s=S1 n=S2) ?"));

  TRISB=0x03;

  while(RB0_bit && RB1_bit);

  if(RB0_bit == 0)
  {
    tmp=EEPROM_Read(10);
    Lcd_Chr_Cp(tmp);

    EEPROM_Write(10,0xA5);
    EEPROM_Write(10,0x5A);
    i=EEPROM_Read(10);

    EEPROM_Write(10,tmp);

    Lcd_Cmd(_LCD_CLEAR);
    Lcd_Cmd(_LCD_FIRST_ROW);
    Lcd_Out_Cp(codetxt_to_ramtxt("Teste EEPROM INT"));
    Lcd_Cmd(_LCD_SECOND_ROW);
    if(i == 0x5A) 
      Lcd_Out_Cp(codetxt_to_ramtxt("       OK"));
    else
      Lcd_Out_Cp(codetxt_to_ramtxt("      ERRO"));

    delay_ms(1000);
  }
  else
  {
    while(RB1_bit == 0);
  }

  TRISB=0x00;
  PORTB=0;
      

      
//teste EEPROM EXT
  Lcd_Cmd(_LCD_CLEAR);
  Lcd_Cmd(_LCD_FIRST_ROW);
  Lcd_Out_Cp(codetxt_to_ramtxt("Teste EEPROM EXT"));
// testar ? 
  Lcd_Cmd(_LCD_SECOND_ROW);
  Lcd_Out_Cp(codetxt_to_ramtxt(" (s=S1 n=S2) ?"));



  TRISB=0x03;

  while(RB0_bit && RB1_bit);

  if(RB0_bit == 0)
  {
    tmp=e2pext_r(10);
    Lcd_Chr_Cp(tmp);

    e2pext_w(10,0xA5);
    e2pext_w(10,0x5A);
    i=e2pext_r(10);

    e2pext_w(10,tmp);

    Lcd_Cmd(_LCD_CLEAR);
    Lcd_Cmd(_LCD_FIRST_ROW);
    Lcd_Out_Cp(codetxt_to_ramtxt("Teste EEPROM EXT"));
    Lcd_Cmd(_LCD_SECOND_ROW);
    if(i == 0x5A) 
      Lcd_Out_Cp(codetxt_to_ramtxt("       OK"));
    else
      Lcd_Out_Cp(codetxt_to_ramtxt("      ERRO"));

    delay_ms(1000);
  }
  else
  {
    while(RB1_bit == 0);
  }

  TRISB=0x00;
  PORTB=0;
 

 /*
//teste serial
  Lcd_Cmd(_LCD_CLEAR);
  Lcd_Cmd(_LCD_FIRST_ROW);
  Lcd_Out_Cp("Teste Serial TX");
  Lcd_Cmd(_LCD_SECOND_ROW+4);
  Lcd_Out_Cp("9600 8N1");

  UART1_Write_Text("\r\n Picsimlab\r\n Teste Serial TX\r\n");

  for(i=0;i<4;i++)
  {
    UART1_Write(i+0x30);
    UART1_Write_Text(" PicsimLab\r\n");
  }
  delay_ms(1000);

  Lcd_Cmd(_LCD_CLEAR);
  Lcd_Cmd(_LCD_FIRST_ROW);
  Lcd_Out_Cp("Teste Serial RX");
  UART1_Write_Text(" Digite!\r\n");
  for(i=0;i<32;i++)
  {
    if(!(i%16))
    {
       Lcd_Cmd(_LCD_SECOND_ROW);
       UART1_Write_Text("\r\n");
    }
    tout=0;
    while(!UART1_Data_Ready() && (tout < 2000))
    {
      tout++;
      delay_ms(1);
    }
    
    if (UART1_Data_Ready() == 1) {
     tmp = UART1_Read();
    }
    else
    {
      tmp='-';
    }
    Lcd_Chr_Cp(tmp);
    UART1_Write(tmp);
  }
  delay_ms(100);
 */


 /*
//teste ADC
  Lcd_Cmd(_LCD_CLEAR);
  Lcd_Cmd(_LCD_FIRST_ROW);
  Lcd_Out_Cp(" Teste ADC (P2)");

  for(i=0; i< 200; i++)
  {
    tmp=((unsigned int)adc_amostra(1)*10)/51;
    Lcd_Out_Cp(_LCD_SECOND_ROW+6);
    itoa(tmp,str);
    Lcd_Chr_Cp(str[3]);
    Lcd_Chr_Cp(',');
    Lcd_Chr_Cp(str[4]);
    Lcd_Chr_Cp('V');
    delay_ms(10);
  }



//teste TEMP
  Lcd_Cmd(_LCD_CLEAR);
  Lcd_Cmd(_LCD_FIRST_ROW);
  Lcd_Out_Cp("   Teste TEMP");

  for(i=0; i< 100; i++)
  {
    tmpi=((unsigned int) adc_amostra(0)*324)/100;
    Lcd_Out_Cp(_LCD_SECOND_ROW+5);
    itoa(tmpi,str);
    Lcd_Chr_Cp(str[2]);
    Lcd_Chr_Cp(str[3]);
    Lcd_Chr_Cp(',');
    Lcd_Chr_Cp(str[4]);
    Lcd_Chr_Cp('C');
    delay_ms(10);
  }


//teste Aquecimento
  Lcd_Cmd(_LCD_CLEAR);
  Lcd_Cmd(_LCD_FIRST_ROW);
  Lcd_Out_Cp("   Teste AQUEC");
  RC2_bit=1;
  for(i=0; i< 100; i++)
  {
    tmpi=((unsigned int) adc_amostra(0)*324)/100;
    Lcd_Out_Cp(_LCD_SECOND_ROW+5);
    itoa(tmpi,str);
    Lcd_Chr_Cp(str[2]);
    Lcd_Chr_Cp(str[3]);
    Lcd_Chr_Cp(',');
    Lcd_Chr_Cp(str[4]);
    Lcd_Chr_Cp('C');
    delay_ms(10);
  }
  RC2_bit=0;


//teste Resfriamento
  Lcd_Cmd(_LCD_CLEAR);
  Lcd_Cmd(_LCD_FIRST_ROW);
  Lcd_Out_Cp("   Teste RESFR");

//timer2 temporizador

 
 T2CKPS0_bit=1; // divide por 16
 T2CKPS1_bit=1;

 TOUTPS3_bit=0; //divide por 2
 TOUTPS2_bit=0;
 TOUTPS1_bit=0;
 TOUTPS0_bit=1;

 TMR2IE_bit=1;
 PR2=249;

 TMR2ON_bit=1;

//T = 32x250x125 = 1segundo;

//timer1 contador
 TMR1CS_bit=1;
 T1CKPS1_bit=0;
 T1CKPS0_bit=0;

 TMR2IF_bit=0;
 cnt=125; 
 GIE_bit=1;
 PEIE_bit=1;

 TMR1H=0;
 TMR1L=0;
 TMR1ON_bit=1;

  RC1_bit=1;
  for(i=0; i< 150; i++)
  {
    tmpi=((unsigned int) adc_amostra(0)*324)/100;
    Lcd_Out_Cp(_LCD_SECOND_ROW+2);
    itoa(tmpi,str);
    Lcd_Chr_Cp(str[2]);
    Lcd_Chr_Cp(str[3]);
    Lcd_Chr_Cp(',');
    Lcd_Chr_Cp(str[4]);
    Lcd_Chr_Cp('C');

    Lcd_Out_Cp(_LCD_SECOND_ROW+8);
    itoa(t1cont,str);
    Lcd_Chr_Cp(str[1]);
    Lcd_Chr_Cp(str[2]);
    Lcd_Chr_Cp(str[3]);
    Lcd_Chr_Cp(str[4]);
    Lcd_Chr_Cp('R');
    Lcd_Chr_Cp('P');
    Lcd_Chr_Cp('S');

    delay_ms(10);
  }

  GIE_bit=0;
  RC1_bit=0;
 */


//fim teste 
  Lcd_Cmd(_LCD_CLEAR);
  Lcd_Cmd(_LCD_FIRST_ROW);
  Lcd_Out_Cp(codetxt_to_ramtxt("      Fim"));
  Lcd_Cmd(_LCD_SECOND_ROW);
  Lcd_Out_Cp(codetxt_to_ramtxt(" Pressione RST"));


 //UART1_Write_Text("\r\n FIM!\r\n");


   while(1);

}