/* ########################################################################

   PICsimLab - PIC laboratory simulator

   ########################################################################

   Copyright (c) : 2021-2022  Luis Claudio Gambôa Lopes

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

#include <time.h>
#include "bitbang_i2c.h"

typedef struct {
    bitbang_i2c_t bb_i2c;
    unsigned char addr;
    unsigned char ctrl;
    unsigned int temp;
    unsigned int pressure;
} sen_bmp280_t;

void sen_bmp280_rst(sen_bmp280_t* bmp280);
void sen_bmp280_init(sen_bmp280_t* bmp280);
void sen_bmp280_end(sen_bmp280_t* bmp280);
void sen_bmp280_setPressTemp(sen_bmp280_t* bmp280, const float pressure, const float temp);

unsigned char sen_bmp280_I2C_io(sen_bmp280_t* bmp280, const unsigned char scl, const unsigned char sda);
