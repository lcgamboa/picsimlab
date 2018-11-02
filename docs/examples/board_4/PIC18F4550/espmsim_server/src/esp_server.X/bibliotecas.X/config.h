    
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

#ifndef CONFIG_H
#define	CONFIG_H

#define _XTAL_FREQ  8000000

#define INPUT 1
#define OUTPUT 0

#define LOW 0
#define HIGH 1

#define _RA0 0x00
#define _RA1 0x01
#define _RA2 0x02
#define _RA3 0x03
#define _RA4 0x04
#define _RA5 0x05
#define _RA6 0x06
#define _RA7 0x07

#define _RB0 0x10
#define _RB1 0x11
#define _RB2 0x12
#define _RB3 0x13
#define _RB4 0x14
#define _RB5 0x15
#define _RB6 0x16
#define _RB7 0x17

#define _RC0 0x20
#define _RC1 0x21
#define _RC2 0x22
#define _RC3 0x23
#define _RC4 0x24
#define _RC5 0x25
#define _RC6 0x26
#define _RC7 0x27

#define _RD0 0x30
#define _RD1 0x31
#define _RD2 0x32
#define _RD3 0x33
#define _RD4 0x34
#define _RD5 0x35
#define _RD6 0x36
#define _RD7 0x37

#define _RE0 0x40
#define _RE1 0x41
#define _RE2 0x42
#define _RE3 0x43
#define _RE4 0x44
#define _RE5 0x45
#define _RE6 0x46
#define _RE7 0x47

#define _AN0 0
#define _AN1 1
#define _AN2 2
#define _AN3 3
#define _AN4 4
#define _AN5 5
#define _AN6 6
#define _AN7 7


unsigned char digitalRead(unsigned char pin);
void digitalWrite(unsigned char pin, unsigned char value);
void pinMode(unsigned char pin, unsigned char value);
void analogWrite(unsigned char pin, unsigned char value);

#define analogRead(ch) adc_sample(ch)
#define Serial_print(x) serial_tx_str(x)
#define Serial_read(x) serial_rx(x)
#define Serial_write(x) serial_tx(x)

#include<xc.h>
#include"delay.h"
#include"adc.h"
#include"pwm.h"
#include"serial.h"

#endif	/* CONFIG_H */
