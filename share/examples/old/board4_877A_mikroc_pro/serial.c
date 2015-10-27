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

#include"serial.h"
#include"atraso.h"

void serial_init(void)
{
        //Configuracao da serial
    TX9_bit=0;    //transmissao em 8 bits
    TXEN_bit=1;  //habilita transmissao
    SYNC_bit=0;  //modo assincrono
    BRGH_bit=1;  //high baud rate
    SPEN_bit=1;  //habilita porta serial - rx
    RX9_bit=0;   //recepcao em 8 bits
    CREN_bit=1;  //recepcao continua

    SPBRG=50; //baud rate de 9600 - 8MHz
}



void serial_tx(unsigned char val)
{
  TXREG=val;
  while(!TRMT_bit);
}

void serial_tx_str(char* val)
{
  unsigned char i=0;
  
  while(val[i])
  {
    serial_tx(val[i]);
    i++; 
  }
}

unsigned char serial_rx(unsigned int timeout)
{
  unsigned int to=0;

  while(((to < timeout)||(!timeout))&&(!RCIF_bit))
  {
    atraso_ms(20);
    to+=20; 
  }
  if(RCIF_bit)
    return RCREG;
  else
    return 255;
}