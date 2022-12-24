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

#include "sen_bmp280.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define dprintf \
    if (1) {    \
    } else      \
        printf

#define A_temp_xlsb 0xFC
#define A_temp_lsb 0xFB
#define A_temp_msb 0xFA
#define A_press_xlsb 0xF9
#define A_press_lsb 0xF8
#define A_press_msb 0xF7
#define A_config 0xF5
#define A_ctrl_meas 0xF4
#define A_status 0xF3
#define A_soft_reset 0xE0
#define A_id 0xD0
#define A_calib23 0xA1
#define A_calib0 0x88

const static struct {
    unsigned short dig_T1;
    short dig_T2;
    short dig_T3;
    unsigned short dig_P1;
    short dig_P2;
    short dig_P3;
    short dig_P4;
    short dig_P5;
    short dig_P6;
    short dig_P7;
    short dig_P8;
    short dig_P9;
    short reserved;
} coefficients = {27504, 26435, -1000, 36477, -10685, 3024, 2855, 140, -7, 15500, -14600, 6000, 0};

static unsigned short* pcoefficients = (unsigned short*)&coefficients;

void sen_bmp280_rst(sen_bmp280_t* bmp280) {
    bitbang_i2c_rst(&bmp280->bb_i2c);
    bitbang_spi_rst(&bmp280->bb_spi);
    bmp280->addr = 0;
    bmp280->ctrl = 0;
    bmp280->temp = 0;
    bmp280->pressure = 0;
    bmp280->data_wc = 0;
    bmp280->i2c_mode = 1;
    dprintf("bmp280 rst\n");
}

void sen_bmp280_init(sen_bmp280_t* bmp280) {
    dprintf("bmp280 init\n");
    bitbang_i2c_init(&bmp280->bb_i2c, 0x77);
    bitbang_spi_init(&bmp280->bb_spi);
    sen_bmp280_rst(bmp280);
}

void sen_bmp280_end(sen_bmp280_t* bmp280) {
    dprintf("bmp280 end\n");
}

void sen_bmp280_set_addr(sen_bmp280_t* bmp280, unsigned char addr) {
    bitbang_i2c_set_addr(&bmp280->bb_i2c, addr);
}

static unsigned char bmp280_read(sen_bmp280_t* bmp280) {
    int temp;

    switch (bmp280->addr) {
        case A_id:
            dprintf("bmp280 read id 0x58\n");
            return 0x58;
            break;
        case A_temp_xlsb:
            dprintf("bmp280 read temp xlsb 0x%02X\n", bmp280->temp & 0x000000FF);
            return bmp280->temp & 0x000000FF;
            break;
        case A_temp_lsb:
            dprintf("bmp280 read temp lsb  0x%02X\n", (bmp280->temp & 0x0000FF00) >> 8);
            return (bmp280->temp & 0x0000FF00) >> 8;
            break;
        case A_temp_msb:
            dprintf("bmp280 read temp msb  0x%02X\n", (bmp280->temp & 0x00FF0000) >> 16);
            return (bmp280->temp & 0x00FF0000) >> 16;
            break;
        case A_press_xlsb:
            dprintf("bmp280 read press xlsb 0x%02X\n", bmp280->pressure & 0x000000FF);
            return bmp280->pressure & 0x000000FF;
            break;
        case A_press_lsb:
            dprintf("bmp280 read press  lsb 0x%02X\n", (bmp280->pressure & 0x0000FF00) >> 8);
            return (bmp280->pressure & 0x0000FF00) >> 8;
            break;
        case A_press_msb:
            dprintf("bmp280 read press  msb 0x%02X\n", (bmp280->pressure & 0x00FF0000) >> 16);
            return (bmp280->pressure & 0x00FF0000) >> 16;
            break;
        case A_calib0 ... A_calib23:
            temp = bmp280->addr - A_calib0;
            if (temp & 0x01) {  // MSB
                dprintf("bmp280 read calib msb 0x%02X\n", (pcoefficients[temp >> 1] & 0xFF00) >> 8);
                return (pcoefficients[temp >> 1] & 0xFF00) >> 8;
            } else {  // LSB
                dprintf("bmp280 read calib lsb 0x%02X\n", (pcoefficients[temp >> 1] & 0x00FF));
                return pcoefficients[temp >> 1] & 0x00FF;
            }
            break;
        default:
            dprintf("bmp280 read invalid addr 0x%02x!!! 0xFF\n", bmp280->addr);
            return 0xFF;
            break;
    }
}

unsigned char sen_bmp280_I2C_io(sen_bmp280_t* bmp280, const unsigned char scl, const unsigned char sda) {
    unsigned char ret = bitbang_i2c_io(&bmp280->bb_i2c, scl, sda);

    switch (bitbang_i2c_get_status(&bmp280->bb_i2c)) {
        case I2C_DATAW:
            dprintf("bmp280 data %02X  byte=%i\n", bmp280->bb_i2c.datar, bmp280->bb_i2c.byte);
            if (bmp280->bb_i2c.byte == 2) {
                dprintf("bmp280 ==>addr %02X\n", bmp280->bb_i2c.datar);
                bmp280->addr = bmp280->bb_i2c.datar;
            } else {
                switch (bmp280->addr) {
                    case A_ctrl_meas:
                        bmp280->ctrl = bmp280->bb_i2c.datar;
                        break;
                    case A_soft_reset:
                        if (bmp280->bb_i2c.datar == 0xB6) {
                            sen_bmp280_rst(bmp280);
                        }
                        break;
                    default:
                        break;
                }
            }
            break;
        case I2C_DATAR:
            dprintf("bmp280 read addr %02X   byte=%i\n", bmp280->addr, bmp280->bb_i2c.byte);
            bitbang_i2c_send(&bmp280->bb_i2c, bmp280_read(bmp280));
            bmp280->addr++;
            break;
    }

    return ret;
}

unsigned short sen_bmp280_io_SPI(sen_bmp280_t* bmp280, unsigned char mosi, unsigned char clk, unsigned char ss) {
    if (!ss)
        bmp280->i2c_mode = 0;

    bitbang_spi_io(&bmp280->bb_spi, clk, mosi, ss);

    switch (bitbang_spi_get_status(&bmp280->bb_spi)) {
        case SPI_DATA:
            switch (bmp280->bb_spi.byte) {
                case 1:
                    bmp280->data_wc = bmp280->bb_spi.data8 & 0x80;
                    bmp280->addr = bmp280->bb_spi.data8 | 0x80;

                    if (bmp280->data_wc & 0x80) {  // Read
                        bitbang_spi_send8(&bmp280->bb_spi, bmp280_read(bmp280));
                        bmp280->addr++;
                    }
                    break;
                default:
                    if ((bmp280->bb_spi.byte == 2) || (bmp280->data_wc & 0x40)) {
                        if (bmp280->data_wc & 0x80) {  // Read
                            bitbang_spi_send8(&bmp280->bb_spi, bmp280_read(bmp280));
                            bmp280->addr++;
                        } else {  // Write
                            dprintf("write bmp280[%04X]=%02X\n", bmp280->addr, bmp280->bb_spi.data8);
                        }
                    }
                    break;
            }
    }
    return (bmp280->bb_spi.outsr & 0x80) > 0;
}

void sen_bmp280_setPressTemp(sen_bmp280_t* bmp280, const float pressureh, const float temp) {
    const float pressure = pressureh * 100.0;  // pressure in Pa

    const float temp2 = temp * temp;

    const float tempf = 14299617.28 - 2621.44 * sqrtf(2.7952369e7 - 12800.0 * temp);
    bmp280->temp = tempf * 16.0;

    const float pressuref = (sqrt(1000.0 * pressure + 1.352061757e9) * (-0.0009718 * temp2 + 0.7518 * temp - 459.848) +
                             35.83 * temp2 - 27701.718 * temp + 17918885.624);

    bmp280->pressure = pressuref * 16.0;
}
