/* ########################################################################

   PICsimLab - PIC laboratory simulator

   ########################################################################

   Copyright (c) : 2010-2015  Luis Claudio Gambôa Lopes

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

typedef struct
{

unsigned char *data;
unsigned short addr;

unsigned char datab;
unsigned char datas;

unsigned char ctrl;

unsigned char sclo;
unsigned char sdao;

unsigned char ret;

unsigned char bit;
unsigned char byte;
}rtc_t;


void rtc_rst(rtc_t *rtc);
void rtc_init(rtc_t *rtc);
void rtc_update(rtc_t *rtc);
void rtc_end(rtc_t *rtc);

unsigned char rtc_io(rtc_t *rtc, unsigned char scl, unsigned char sda);
