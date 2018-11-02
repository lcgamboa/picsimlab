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
#include "config.h"

#define NREP (_XTAL_FREQ/(4000l*50)) //ciclos para 1 ms usando 50 instruções

void delay(unsigned int ms)
{
unsigned int  i;
volatile unsigned char j1;
volatile unsigned char j2=0;

 for (i =0; i< ms; i++)
 {  
    j1=NREP; //2 ciclos
    #asm                      ; para j2=9 e j1 =20 numero de execuções e ciclos
l1: MOVLW 9                   ; 20                j1  
    MOVWF delay@j2        ; 20                j1 
    NOP                       ; 180       j2j1
    NOP                       ; 180       j2j1
    DECFSZ delay@j2, F    ; 160       j2j1   -j1
                              ; 40               2j1
    GOTO $-3                  ; 320      2j2j1  -2j1 
    NOP                       ; 20                j1 
    DECFSZ delay@j1, F    ; 19                j1    -1
                              ; 2                        2
    GOTO l1                   ; 38               2j1    -2 
    NOP                       ; 1                        1
    #endasm;
//                                            5j2j1+5J1-0=ciclos
//                              para j2=9     50j1=ciclos 
 }
}
/*
#pragma interrupt_level 1
#pragma interrupt_level 2
void atraso_ms(unsigned int valor)
{
unsigned int  i;
volatile unsigned char j1;
volatile unsigned char j2=0;

 for (i =0; i< valor; i++)
 {  
    j1=NREP; //2 ciclos
    #asm                      ; para j2=9 e j1 =20 numero de execuções e ciclos
l1: MOVLW 9                   ; 20                j1  
    MOVWF atraso_ms@j2        ; 20                j1 
    NOP                       ; 180       j2j1
    NOP                       ; 180       j2j1
    DECFSZ atraso_ms@j2, F    ; 160       j2j1   -j1
                              ; 40               2j1
    GOTO $-3                  ; 320      2j2j1  -2j1 
    NOP                       ; 20                j1 
    DECFSZ atraso_ms@j1, F    ; 19                j1    -1
                              ; 2                        2
    GOTO l1                   ; 38               2j1    -2 
    NOP                       ; 1                        1
    #endasm;
//                                            5j2j1+5J1-0=ciclos
//                              para j2=9     50j1=ciclos 
 }
}
*/
