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

/* ----------------------------------------------------------------------- */

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
  unsigned int tmpi;
  unsigned int tout;

  char str[6];

  ADCON1=0x04;
  TRISA=0xFF;
  TRISB=0x00;
  TRISC=0x01;
  TRISD=0x00;
  TRISE=0xFC;

  PORTD=0;

  Lcd_Init();
  ADC_Init();
  UART1_Init(9600);

/*
//teste lcd
  LCD_Cmd(_LCD_CLEAR);
  LCD_Cmd(_LCD_FIRST_ROW);
  Lcd_Out_Cp(" 123 Teste LCD");

  for(i=32;i<128;i++)
  {
    if((i%16) == 0)lcd_cmd(L_L2);
    lcd_dat(i);
    delay_ms(50);
  }

  delay_ms(100);
  lcd_cmd(L_CLR);
  lcd_cmd(L_L1);
  lcd_str("   Teste LCD");
  lcd_cmd(L_L2);
  lcd_str("       Ok");
  delay_ms(500);


 */

 //teste serial
  Lcd_Cmd(_LCD_CLEAR);
  Lcd_Cmd(_LCD_FIRST_ROW);
  Lcd_Out_Cp(codetxt_to_ramtxt("Teste Serial TX"));
  Lcd_Cmd(_LCD_SECOND_ROW+4);
  Lcd_Out_Cp(codetxt_to_ramtxt("9600 8N1"));

  UART1_Write_Text(codetxt_to_ramtxt("\r\n Picsimlab\r\n Teste Serial TX\r\n"));

  for(i=0;i<4;i++)
  {
    UART1_Write(i+0x30);
    UART1_Write_Text(codetxt_to_ramtxt(" PicsimLab\r\n"));
  }
  delay_ms(1000);

  Lcd_Cmd(_LCD_CLEAR);
  Lcd_Cmd(_LCD_FIRST_ROW);
  Lcd_Out_Cp(codetxt_to_ramtxt("Teste Serial RX"));
  UART1_Write_Text(codetxt_to_ramtxt(" Digite!\r\n"));
  for(i=0;i<32;i++)
  {
    if(!(i%16))
    {
       Lcd_Cmd(_LCD_SECOND_ROW);
       UART1_Write_Text(codetxt_to_ramtxt("\r\n"));
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

//teste ADC
  lcd_cmd(_LCD_CLEAR);
  lcd_cmd(_LCD_FIRST_ROW);
  Lcd_Out_Cp(codetxt_to_ramtxt(" Teste ADC (P2)"));

  for(i=0; i< 200; i++)
  {
    tmp=((unsigned int)(ADC_Read(1)*10))/204;
    lcd_cmd(_LCD_SECOND_ROW+6);
    IntToStr(tmp,str);
    if(str[4] != ' ')
      Lcd_Chr_Cp(str[4]);
    else
      Lcd_Chr_Cp('0');
    Lcd_Chr_Cp(',');
    Lcd_Chr_Cp(str[5]);
    Lcd_Chr_Cp('V');
    delay_ms(10);
  }



//teste TEMP
  lcd_cmd(_LCD_CLEAR);
  Lcd_Cmd(_LCD_FIRST_ROW);
  Lcd_Out_Cp(codetxt_to_ramtxt("   Teste TEMP"));

  for(i=0; i< 100; i++)
  {
    tmpi=((unsigned int) (ADC_Read(0)*81))/100;
    Lcd_Cmd(_LCD_SECOND_ROW+5);
    IntToStr(tmpi,str);
    Lcd_Chr_Cp(str[3]);
    Lcd_Chr_Cp(str[4]);
    Lcd_Chr_Cp(',');
    Lcd_Chr_Cp(str[5]);
    Lcd_Chr_Cp('C');
    delay_ms(10);
  }


//teste Aquecimento
  Lcd_Cmd(_LCD_CLEAR);
  Lcd_Cmd(_LCD_FIRST_ROW);
  Lcd_Out_Cp(codetxt_to_ramtxt("   Teste AQUEC"));
  RC2_bit=1;
  for(i=0; i< 100; i++)
  {
    tmpi=((unsigned int) (ADC_Read(0)*81))/100;
    Lcd_Cmd(_LCD_SECOND_ROW+5);
    IntToStr(tmpi,str);
    Lcd_Chr_Cp(str[3]);
    Lcd_Chr_Cp(str[4]);
    Lcd_Chr_Cp(',');
    Lcd_Chr_Cp(str[5]);
    Lcd_Chr_Cp('C');
    delay_ms(10);
  }
  RC2_bit=0;


//teste Resfriamento
  Lcd_Cmd(_LCD_CLEAR);
  Lcd_Cmd(_LCD_FIRST_ROW);
  Lcd_Out_Cp(codetxt_to_ramtxt("   Teste RESFR"));

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
    tmpi=((unsigned int) (ADC_Read(0)*81))/100;
    Lcd_Cmd(_LCD_SECOND_ROW+2);
    IntToStr(tmpi,str);
    Lcd_Chr_Cp(str[3]);
    Lcd_Chr_Cp(str[4]);
    Lcd_Chr_Cp(',');
    Lcd_Chr_Cp(str[5]);
    Lcd_Chr_Cp('C');

    Lcd_Cmd(_LCD_SECOND_ROW+8);
    IntToStr(t1cont,str);
    Lcd_Chr_Cp(str[2]);
    Lcd_Chr_Cp(str[3]);
    Lcd_Chr_Cp(str[4]);
    Lcd_Chr_Cp(str[5]);
    Lcd_Chr_Cp('R');
    Lcd_Chr_Cp('P');
    Lcd_Chr_Cp('S');

    delay_ms(10);
  }

  GIE_bit=0;
  RC1_bit=0;

//fim teste 
  Lcd_Cmd(_LCD_CLEAR);
  Lcd_Cmd(_LCD_FIRST_ROW);
  Lcd_Out_Cp(codetxt_to_ramtxt("      Fim"));
  Lcd_Cmd(_LCD_SECOND_ROW);
  Lcd_Out_Cp(codetxt_to_ramtxt(" Pressione RST"));

  UART1_Write_Text(codetxt_to_ramtxt("\r\n FIM!\r\n"));

   while(1);

}