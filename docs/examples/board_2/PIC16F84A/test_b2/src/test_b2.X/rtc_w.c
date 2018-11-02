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

#include"rtc.h"
#include"i2c.h"

void rtc_w(void)
{

   i2c_start();          // issue start signal
   i2c_wb(0xA2);         // address PCF8563
   i2c_wb(0);            // start from word at address 0 (configuration word)
   i2c_wb(0x20);         // write $20 to config. (pause counter...)
   i2c_wb(0);            // write 0 to cents word
   i2c_wb(0x20);         // write $20 to seconds word
   i2c_wb(0x34);         // write $30 to minutes word
   i2c_wb(0x11);         // write $11 to hours word
   i2c_wb(0x24);         // write $24 to day word
   i2c_wb(0x04);         // write $04 to weekday
   i2c_wb(0x02);         // write $08 to month
   i2c_wb(0x14);         // write $08 to year
   i2c_stop();           // issue stop signal
   i2c_start();          // issue start signal
   i2c_wb(0xA2);         // address PCF8530
   i2c_wb(0);            // start from word at address 0
   i2c_wb(0);            // write 0 to config word (enable counting)
   i2c_stop();           // issue stop signal

}

