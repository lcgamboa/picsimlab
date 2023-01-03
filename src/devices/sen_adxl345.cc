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

#include "sen_adxl345.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define dprintf \
    if (0) {    \
    } else      \
        printf

void adxl345_set_addr(adxl345_t* adxl, unsigned char addr) {
    bitbang_i2c_set_addr(&adxl->bb_i2c, addr);
}

void adxl345_rst(adxl345_t* adxl) {
    bitbang_i2c_rst(&adxl->bb_i2c);
    bitbang_spi_rst(&adxl->bb_spi);
    adxl->addr = 0;

    for (int i = 0; i < 0x3A; i++) {
        adxl->regs[i] = 0;
    }

    adxl->update = 1;

    adxl->regs[DEVID] = 0xE5;
    adxl->regs[BW_RATE] = 0x0A;
    adxl->regs[INT_SOURCE] = 0x02;

    adxl->data_wc = 0;
    adxl->i2c_mode = 1;

    dprintf("adxl345 rst\n");
}

void adxl345_init(adxl345_t* adxl) {
    dprintf("adxl345 init\n");
    bitbang_i2c_init(&adxl->bb_i2c, 0x53);
    bitbang_spi_init(&adxl->bb_spi);
    adxl345_rst(adxl);
}

void adxl345_end(adxl345_t* adxl) {
    dprintf("adxl345 end\n");
}

static void adxl345_regwrited(adxl345_t* adxl) {
    switch (adxl->addr) {
        case POWER_CTL:
            break;
        case DATA_FORMAT:
            adxl->update = 1;
            break;
        default:
            dprintf("adxl write to register 0x%02X not defined yet !!!!!!!!\n", adxl->addr);
            break;
    }
}

unsigned char adxl345_io_I2C(adxl345_t* adxl, unsigned char scl, unsigned char sda) {
    unsigned char ret = bitbang_i2c_io(&adxl->bb_i2c, scl, sda);

    switch (bitbang_i2c_get_status(&adxl->bb_i2c)) {
        case I2C_ADDR:
            break;
        case I2C_DATAW:
            if (adxl->bb_i2c.byte == 2) {
                adxl->addr = adxl->bb_i2c.datar;
                dprintf("----> adxl add = %02X\n", adxl->addr);
            }

            if (adxl->bb_i2c.byte > 2) {
                dprintf("write adxl[%04X]=%02X\n", adxl->addr, adxl->bb_i2c.datar);
                adxl->regs[adxl->addr] = adxl->bb_i2c.datar;
                adxl345_regwrited(adxl);
                adxl->addr++;
                if (adxl->addr >= 0x3A) {
                    adxl->addr -= 0x3A;
                }
            }
            break;
        case I2C_DATAR:
            bitbang_i2c_send(&adxl->bb_i2c, adxl->regs[adxl->addr]);
            dprintf("adxl345 read[%04X]=%02X\n", adxl->addr, adxl->regs[adxl->addr]);
            adxl->addr++;
            if (adxl->addr >= 0x3A) {
                adxl->addr -= 0x3A;
            }
            break;
    }

    return ret;
}

unsigned short adxl345_io_SPI(adxl345_t* adxl, unsigned char mosi, unsigned char clk, unsigned char ss) {
    if (!ss)
        adxl->i2c_mode = 0;

    bitbang_spi_io(&adxl->bb_spi, clk, mosi, ss);

    switch (bitbang_spi_get_status(&adxl->bb_spi)) {
        case SPI_DATA:
            /*
                        adxl->regs[DATAX0] = 0x01;
                        adxl->regs[DATAX1] = 0x01;
                        adxl->regs[DATAY0] = 0x02;
                        adxl->regs[DATAY1] = 0x02;
                        adxl->regs[DATAZ0] = 0x03;
                        adxl->regs[DATAZ1] = 0x03;
            */
            // dprintf("adxl345 byte in 0x%02X  out 0x%02X\n", adxl->bb_spi.insr & 0xFF, (adxl->bb_spi.outsr >> 8) &
            // 0xFF);

            switch (adxl->bb_spi.byte) {
                case 1:
                    adxl->data_wc = adxl->bb_spi.data8 & 0xC0;
                    adxl->addr = adxl->bb_spi.data8 & 0x3F;

                    if (adxl->data_wc & 0x80) {  // Read
                        bitbang_spi_send8(&adxl->bb_spi, adxl->regs[adxl->addr]);
                        dprintf("adxl345 read[%04X]=%02X\n", adxl->addr, adxl->regs[adxl->addr]);
                        adxl->addr++;
                        if (adxl->addr >= 0x3A) {
                            adxl->addr -= 0x3A;
                        }
                    }
                    break;
                default:
                    if ((adxl->bb_spi.byte == 2) || (adxl->data_wc & 0x40)) {
                        if (adxl->data_wc & 0x80) {  // Read
                            bitbang_spi_send8(&adxl->bb_spi, adxl->regs[adxl->addr]);
                            dprintf("adxl345 read[%04X]=%02X\n", adxl->addr, adxl->regs[adxl->addr]);
                        } else {  // Write
                            dprintf("write adxl[%04X]=%02X\n", adxl->addr, adxl->bb_spi.data8);
                            adxl->regs[adxl->addr] = adxl->bb_spi.data8;
                            adxl345_regwrited(adxl);
                        }
                        adxl->addr++;
                        if (adxl->addr >= 0x3A) {
                            adxl->addr -= 0x3A;
                        }
                    }
                    break;
            }
    }
    return (adxl->bb_spi.outsr & 0x80) > 0;
}

void adxl345_set_accel(adxl345_t* adxl, double x, double y, double z) {
    double sens;

    if (adxl->regs[DATA_FORMAT] & 0x8) {  // FULL_RES
        sens = 256;
    } else {
        switch (adxl->regs[DATA_FORMAT] & 0x3) {
            case 0:  // 2g
            default:
                sens = 256;
                break;
            case 1:  // 4g
                sens = 128;
                break;
            case 2:  // 8g
                sens = 64;
                break;
            case 3:  // 16g
                sens = 32;
                break;
        }
    }

    short ix = x * sens;
    short iy = y * sens;
    short iz = z * sens;

    adxl->regs[DATAX1] = (ix & 0xFF00) >> 8;
    adxl->regs[DATAX0] = ix & 0x00FF;
    adxl->regs[DATAY1] = (iy & 0xFF00) >> 8;
    adxl->regs[DATAY0] = iy & 0x00FF;
    adxl->regs[DATAZ1] = (iz & 0xFF00) >> 8;
    adxl->regs[DATAZ0] = iz & 0x00FF;
}

void adxl345_set_accel_raw(adxl345_t* adxl, short x, short y, short z) {
    adxl->regs[DATAX1] = (x & 0xFF00) >> 8;
    adxl->regs[DATAX0] = x & 0x00FF;
    adxl->regs[DATAY1] = (y & 0xFF00) >> 8;
    adxl->regs[DATAY0] = y & 0x00FF;
    adxl->regs[DATAZ1] = (z & 0xFF00) >> 8;
    adxl->regs[DATAZ0] = z & 0x00FF;
}
