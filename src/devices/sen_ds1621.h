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
    unsigned char cmd;
    unsigned char CTR;
    unsigned char TL;
    unsigned char TH;
    short Temp;
} sen_ds1621_t;


void sen_ds1621_rst (sen_ds1621_t *ds1621);
void sen_ds1621_init (sen_ds1621_t *ds1621);
void sen_ds1621_end(sen_ds1621_t *ds1621);
void sen_ds1621_setTemp(sen_ds1621_t *ds1621, float temp);
void sen_ds1621_set_addr(sen_ds1621_t *ds1621, unsigned char addr);

unsigned char sen_ds1621_I2C_io(sen_ds1621_t *ds1621, unsigned char scl, unsigned char sda);
