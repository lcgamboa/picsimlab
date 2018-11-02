/* ########################################################################

   PICsim - PIC simulator http://sourceforge.net/projects/picsim/

   ########################################################################

   Copyright (c) : 2011  Luis Claudio Gamb�a Lopes

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


#include "adc.h"


void adc_init(void)
{

	ADCON1= 0x02;
                        		// CONFIGURA CONVERSOR A/D
					// RA0, RA1 E RA3 COMO ANAL�GICO
					// RA2, RA4 E RA5 COMO I/O DIGITAL
					// PORTE COMO I/O DIGITAL
					// JUSTIFICADO � ESQUERDA
					// 8 BITS EM ADRESH E 2 BITS EM ADRESL
					// Vref+ = VDD (+5V)
					// Vref- = GND ( 0V)

	ADCON0=0x49;            	// CONFIGURA CONVERSOR A/D
					// VELOCIDADE -> Fosc/8
					// CANAL 1
					// M�DULO LIGADO

}

unsigned int adc_amostra(unsigned char canal)
{
/*
    ADCON0&=0xC7; //canal       //sdcc bug?
    ADCON0|=(0x07&canal)<<3;
*/
    switch(canal)
    {
      case 0: 
        ADCON0=0x41;
        break;
      case 1: 
        ADCON0=0x49;
        break;
      case 2: 
        ADCON0=0x51;
        break;
    } 
   

    GODONE=1;
    while(GODONE == 1);

   return ((((unsigned int)ADRESH)<<2)|(ADRESL>>6));
}
