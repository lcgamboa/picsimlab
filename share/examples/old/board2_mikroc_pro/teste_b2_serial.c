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




#include "lcd.h"
#include "teclado.h"

char* codetxt_to_ramtxt(const char* ctxt){
static char txt[45];
char i;
  for(i =0; txt[i] = ctxt[i]; i++);
  return txt;
}

void main() {
  unsigned char i,tmp;
  unsigned int tout;

  CMCON=0x07; 
  TRISA=0x30;
  TRISB=0xE7;
  
  PORTA=0;
  PORTB=0;

  lcd_init();

  UART1_Init(9600);

//teste serial
  lcd_cmd(L_CLR);
  lcd_cmd(L_L1);
  lcd_str(codetxt_to_ramtxt("Teste Serial TX"));
  lcd_cmd(L_L2+2);
  lcd_str(codetxt_to_ramtxt("9600 8N1"));

  UART1_Write_Text(codetxt_to_ramtxt("\r\n Picsimlab\r\n Teste Serial TX\r\n"));

  for(i=0;i<4;i++)
  {
    UART1_Write(i+0x30);
    UART1_Write_Text(codetxt_to_ramtxt(" PicsimLab\r\n"));
  }
  delay_ms(1000);

  lcd_cmd(L_CLR);
  lcd_cmd(L_L1);
  lcd_str(codetxt_to_ramtxt("Teste Serial RX"));
  UART1_Write_Text(codetxt_to_ramtxt(" Digite!\r\n"));
  for(i=0;i<32;i++)
  {
    if(!(i%16))
    {
       lcd_cmd(L_L2);
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

    
    lcd_dat(tmp);
    UART1_Write(tmp);
  }
  delay_ms(100);

  lcd_cmd(L_CLR);
  lcd_cmd(L_L1);
  lcd_str(codetxt_to_ramtxt("Teste Teclado TX"));
  UART1_Write_Text(codetxt_to_ramtxt("\r\n Aguarde!\r\n"));
  for(i=0;i<32;i++)
  {
    if(!(i%16))
    {
      lcd_cmd(L_L2);
      UART1_Write_Text(codetxt_to_ramtxt("\r\n"));
    }
    tmp=tc_tecla(2000)+0x30;
    lcd_dat(tmp);
    UART1_Write(tmp);
  }
  delay_ms(100);


//fim teste 
  lcd_cmd(L_CLR);
  lcd_cmd(L_L1+4);
  lcd_str(codetxt_to_ramtxt("Fim"));
  lcd_cmd(L_L2+1);
  lcd_str(codetxt_to_ramtxt("Pressione RST"));

  UART1_Write_Text(codetxt_to_ramtxt("\r\n FIM!\r\n"));

  while(1);


}