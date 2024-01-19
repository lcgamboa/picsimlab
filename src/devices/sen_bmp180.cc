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

#include "sen_bmp180.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define dprintf \
    if (1) {    \
    } else      \
        printf

#define A_out_xlsb 0xF8
#define A_out_lsb 0xF7
#define A_out_msb 0xF6
#define A_ctrl_meas 0xF4
#define A_soft_reset 0xE0
#define A_id 0xD0
#define A_calib21 0xBF
#define A_calib0 0xAA

#define C_TEMP 0x2E
#define C_PRESS0 0x34
#define C_PRESS1 0x74
#define C_PRESS2 0xB4
#define C_PRESS3 0xF4

const static struct {
    short AC1;
    short AC2;
    short AC3;
    unsigned short AC4;
    unsigned short AC5;
    unsigned short AC6;
    short B1;
    short B2;
    short MB;
    short MC;
    short MD;
} coefficients = {408, -72, -14383, 32741, 32757, 23153, 6190, 4, -32768, -8711, 2868};

static unsigned short* pcoefficients = (unsigned short*)&coefficients;

void sen_bmp180_rst(sen_bmp180_t* bmp180) {
    bitbang_i2c_rst(&bmp180->bb_i2c);
    bmp180->addr = 0;
    bmp180->ctrl = 0;
    bmp180->temp = 0;
    bmp180->pressure = 0;
    dprintf("bmp180 rst\n");
}

void sen_bmp180_init(sen_bmp180_t* bmp180) {
    dprintf("bmp180 init\n");
    bitbang_i2c_init(&bmp180->bb_i2c, 0x77);
    sen_bmp180_rst(bmp180);
}

void sen_bmp180_end(sen_bmp180_t* bmp180) {
    dprintf("bmp180 end\n");
}

unsigned char sen_bmp180_I2C_io(sen_bmp180_t* bmp180, const unsigned char scl, const unsigned char sda) {
    unsigned char ret = bitbang_i2c_io(&bmp180->bb_i2c, scl, sda);
    int temp;

    switch (bitbang_i2c_get_status(&bmp180->bb_i2c)) {
        case I2C_DATAW:
            dprintf("data %02X  byte=%i\n", bmp180->bb_i2c.datar, bmp180->bb_i2c.byte);
            if (bmp180->bb_i2c.byte == 2) {
                dprintf("==>addr %02X\n", bmp180->bb_i2c.datar);
                bmp180->addr = bmp180->bb_i2c.datar;
            } else {
                switch (bmp180->addr) {
                    case A_ctrl_meas:
                        bmp180->ctrl = bmp180->bb_i2c.datar;
                        break;
                    case A_soft_reset:
                        if (bmp180->bb_i2c.datar == 0xB6) {
                            sen_bmp180_rst(bmp180);
                        }
                        break;
                    default:
                        break;
                }
            }
            break;
        case I2C_DATAR:
            dprintf("read addr %02X   byte=%i\n", bmp180->addr, bmp180->bb_i2c.byte);
            switch (bmp180->addr) {
                case A_id:
                    bitbang_i2c_send(&bmp180->bb_i2c, 0x55);
                    break;
                case A_out_xlsb:
                    if ((bmp180->ctrl & 0x1F) == 0x0E) {  // Temp
                        bitbang_i2c_send(&bmp180->bb_i2c, bmp180->temp & 0x000000FF);
                    } else {  // Press
                        bitbang_i2c_send(&bmp180->bb_i2c, bmp180->pressure & 0x000000FF);
                    }
                    break;
                case A_out_lsb:
                    if ((bmp180->ctrl & 0x1F) == 0x0E) {  // Temp
                        bitbang_i2c_send(&bmp180->bb_i2c, (bmp180->temp & 0x0000FF00) >> 8);
                    } else {  // Press
                        bitbang_i2c_send(&bmp180->bb_i2c, (bmp180->pressure & 0x0000FF00) >> 8);
                    }
                    break;
                case A_out_msb:
                    if ((bmp180->ctrl & 0x1F) == 0x0E) {  // Temp
                        bitbang_i2c_send(&bmp180->bb_i2c, (bmp180->temp & 0x00FF0000) >> 16);
                    } else {  // Press
                        bitbang_i2c_send(&bmp180->bb_i2c, (bmp180->pressure & 0x00FF0000) >> 16);
                    }
                    break;
                case A_calib0 ... A_calib21:
                    temp = bmp180->addr - A_calib0;
                    if (temp & 0x01) {  // MSB
                        bitbang_i2c_send(&bmp180->bb_i2c, pcoefficients[temp >> 1] & 0x00FF);
                    } else {  // LSB
                        bitbang_i2c_send(&bmp180->bb_i2c, (pcoefficients[temp >> 1] & 0xFF00) >> 8);
                    }
                    break;
                default:
                    bitbang_i2c_send(&bmp180->bb_i2c, 0xFF);
                    break;
            }
            bmp180->addr++;
            break;
    }

    return ret;
}

void sen_bmp180_setPressTemp(sen_bmp180_t* bmp180, const float pressureh, const float temp) {
    const float pressure = pressureh * 100.0;  // pressure in Pa

    const float temp1 = temp + 0.05;
    const float temp2 = temp1 * temp1;
    const float temp3 = temp2 * temp1;
    const float temp4 = temp3 * temp1;

    const float tempf = (2.0 * sqrtf(1600.0 * temp2 + 57200.0 * temp1 + 4971257.0) + 80.027 * temp1 + 21714.517);

    bmp180->temp = tempf * 256.0;

    const float pressuref8 =
        sqrt((0.003148 * pressure + 17551.552) * temp4 + (-3.311 * pressure - 18463196.865) * temp3 +
             (2347.735 * pressure + 13090220384.25) * temp2 + (-776800.897 * pressure - 4331194520799.89) * temp +
             173228411.978 * pressure + 965866480733890.0) -
        132.461 * temp2 + 69670.773 * temp - 31075491.201;

    bmp180->pressure = pressuref8 * 32.0;
}
