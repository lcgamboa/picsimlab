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

#include "config_628A.h"

#include "atraso.h"
#include "lcd.h"
#include "teclado.h"
#include "i2c.h"
#include "eeprom_ext.h"
#include "rtc.h"
#include "serial.h"


void main() {
  unsigned char i,tmp;

  CMCON=0x07; 
  TRISA=0x30;
  TRISB=0xE7;
  
  PORTA=0xFE;
  PORTB=0x00;

  lcd_init();

  i2c_init();
  
  serial_init();

//teste lcd
  lcd_cmd(L_CLR); 
  lcd_cmd(L_L1);
  for(i=0;i<16;i++)
  {
    lcd_dat('A'+i);
  }
  lcd_cmd(L_L2);
  for(i=0;i<16;i++)
  {
    lcd_dat('a'+i);
  }

  atraso_ms(200);
  lcd_cmd(L_CLR);
  lcd_cmd(L_L1+3);
  lcd_str("Teste LCD");
  
  for(i=32;i<128;i++)
  {
    if((i%16) == 0)lcd_cmd(L_L2);
    lcd_dat(i);
    atraso_ms(50);
  }

  atraso_ms(100);
  lcd_cmd(L_CLR);
  lcd_cmd(L_L1+3);
  lcd_str("Teste LCD");
  lcd_cmd(L_L2+7);
  lcd_str("Ok");
  atraso_ms(500);

//teste LEDS
  lcd_cmd(L_CLR);
  lcd_cmd(L_L1+1);
  lcd_str("Teste LEDs");
 
for(i=0;i<4;i++)
  {
    atraso_ms(100);
    RA1^=1;
    atraso_ms(100);
    RA2^=1;
    atraso_ms(100);
    PORTA^=0x40; //RA6=1;
    atraso_ms(100);
    PORTA^=0x80; //RA7=1;
    atraso_ms(100);
  }


//teste Teclado
  lcd_cmd(L_CLR);
  lcd_cmd(L_L1+2);
  lcd_str("Teste Teclado");

  lcd_cmd(L_L2+1);

  i=0;
  while(i<14)
  {
    lcd_dat(tc_tecla(3000)+0x30);
    i++;
  }

//teste EEPROM EXT
  lcd_cmd(L_CLR);
  lcd_cmd(L_L1);
  lcd_str("Teste EEPROM EXT");
// testar ? 
  lcd_cmd(L_L2);
  lcd_str("Testar (1/0) ?");
 
  if(tc_tecla(0) == 1)
  {
    tmp=e2pext_r(10);
    lcd_dat(tmp);

    e2pext_w(10,0xA5);
    e2pext_w(10,0x5A);
    i=e2pext_r(10);

    e2pext_w(10,tmp);

    lcd_cmd(L_CLR);
    lcd_cmd(L_L1);
    lcd_str("Teste EEPROM EXT");
    lcd_cmd(L_L2);
    if(i == 0x5A) 
      lcd_str("     OK");
    else
      lcd_str("     ERRO");

    atraso_ms(1000);
  }


//teste RTC
  lcd_cmd(L_CLR);
  lcd_cmd(L_L1);
  lcd_str("Teste RTC");
  
//ajuste  rtc_w();

//  rtc_w();
 
  rtc_r();
  lcd_cmd(L_L2);
  lcd_str((char *)date);
   atraso_ms(1500);
  for(i=0;i<16;i++)
  {
    rtc_r();
    lcd_cmd(L_L2);;
    lcd_str((char *)time);
    atraso_ms(500); 
  }

   
   //teste serial
   
 //teste EEPROM EXT
  lcd_cmd(L_CLR);
  lcd_cmd(L_L1);
  lcd_str("Teste Serial");
// testar ? 
  lcd_cmd(L_L2);
  lcd_str("Testar (1/0) ?");
  if(tc_tecla(0) == 1)
  {  
  lcd_cmd(L_CLR);
  lcd_cmd(L_L1);
  lcd_str("Teste Serial TX");
  lcd_cmd(L_L2+2);
  lcd_str("9600 8N1");

  serial_tx_str("\r\n Picsimlab\r\n Teste Serial TX\r\n");

  for(i=0;i<4;i++)
  {
    serial_tx(i+0x30);
    serial_tx_str(" PicsimLab\r\n");
  }
  atraso_ms(1000);

  lcd_cmd(L_CLR);
  lcd_cmd(L_L1);
  lcd_str("Teste Serial RX");
  serial_tx_str(" Digite!\r\n");
  for(i=0;i<32;i++)
  {
    if(!(i%16))
    {
       lcd_cmd(L_L2);
       serial_tx_str("\r\n");
    }
    tmp=serial_rx(3000);
    lcd_dat(tmp);
    serial_tx(tmp);
  }
  atraso_ms(100);
  
  
  lcd_cmd(L_CLR);
  lcd_cmd(L_L1);
  lcd_str("Teste Teclado TX");
  serial_tx_str("\r\n Aguarde!\r\n");
  for(i=0;i<32;i++)
  {
    if(!(i%16))
    {
      lcd_cmd(L_L2);
      serial_tx_str("\r\n");
    }
    tmp=tc_tecla(2000)+0x30;
    lcd_dat(tmp);
    serial_tx(tmp);
  }
  atraso_ms(100);
  }

//fim teste 
  lcd_cmd(L_CLR);
  lcd_cmd(L_L1+4);
  lcd_str("Fim");
  lcd_cmd(L_L2+1);
  lcd_str("Pressione RST");

  serial_tx_str("\r\n FIM!\r\n");  

  while(1);


}
