/* ########################################################################

   PICSimLab - Programmable IC Simulator Laboratory

   ########################################################################

   Copyright (c) : 2021-2024  Luis Claudio Gambôa Lopes <lcgamboa@yahoo.com>

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

#include "bitbang_i2c.h"
#include "bitbang_spi.h"

typedef struct {
    bitbang_i2c_t bb_i2c;
    bitbang_spi_t bb_spi;
    unsigned char addr;
    unsigned char ctrl;
    unsigned int temp;
    unsigned int pressure;
    unsigned char data_wc;
    int i2c_mode;
} sen_bmp280_t;

void sen_bmp280_rst(sen_bmp280_t* bmp280);
void sen_bmp280_init(sen_bmp280_t* bmp280);
void sen_bmp280_end(sen_bmp280_t* bmp280);
void sen_bmp280_setPressTemp(sen_bmp280_t* bmp280, const float pressure, const float temp);
void sen_bmp280_set_addr(sen_bmp280_t* bmp280, unsigned char addr);

unsigned char sen_bmp280_I2C_io(sen_bmp280_t* bmp280, const unsigned char scl, const unsigned char sda);
unsigned short sen_bmp280_io_SPI(sen_bmp280_t* bmp280, unsigned char mosi, unsigned char clk, unsigned char ss);
