/* ########################################################################

   PICsim - PIC simulator http://sourceforge.net/projects/picsim/

   ########################################################################

   Copyright (c) : 2015-2017  Luis Claudio Gambôa Lopes

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


void serial_init();//default 9600
void serial_tx(unsigned char val);
void serial_tx_str(const char* val);
unsigned char serial_rx(unsigned int timeout);  /*timeout=0 no time out*/ 
char*  serial_rx_str(char * buff, unsigned int size, unsigned int timeout);  /*timeout=0 no time out*/ 
char*  serial_rx_str_until(char * buff, unsigned int size, unsigned char term);   
