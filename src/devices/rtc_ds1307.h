/* ########################################################################

   PICsimLab - PIC laboratory simulator

   ########################################################################

   Copyright (c) : 2010-2021  Luis Claudio Gambôa Lopes

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

#include<time.h>
#include"bitbang_i2c.h"

typedef struct {
    bitbang_i2c_t bb_i2c;

    unsigned char data[64];
    unsigned short addr;
    
    struct tm dtime;
    time_t systime;
    time_t rtctime;
    int ucont;
} rtc_ds1307_t;


void rtc_ds1307_rst (rtc_ds1307_t *rtc);
void rtc_ds1307_init (rtc_ds1307_t *rtc);
void rtc_ds1307_update(rtc_ds1307_t *rtc);
void rtc_ds1307_end(rtc_ds1307_t *rtc);
void rtc_ds1307_setUtime(rtc_ds1307_t *rtc, time_t utime);
time_t rtc_ds1307_getUtime(rtc_ds1307_t *rtc);

unsigned char rtc_ds1307_I2C_io(rtc_ds1307_t *rtc, unsigned char scl, unsigned char sda);
