/* ########################################################################

   PICsimLab - PIC laboratory simulator

   ########################################################################

   Copyright (c) : 2010-2022  Luis Claudio Gambôa Lopes

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

#include "io_PCF8574.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define dprintf \
    if (1) {    \
    } else      \
        printf

void io_PCF8574_set_addr(io_PCF8574_t* ioe8, unsigned char addr) {
    bitbang_i2c_set_addr(&ioe8->bb_i2c, addr);
    // dprintf ("ioe8 set addr = 0x%02X\n", addr);
}

void io_PCF8574_rst(io_PCF8574_t* ioe8) {
    dprintf("ioe8 rst\n");
    bitbang_i2c_rst(&ioe8->bb_i2c);
}

void io_PCF8574_init(io_PCF8574_t* ioe8) {
    dprintf("ioe8 init\n");
    bitbang_i2c_init(&ioe8->bb_i2c, 0x27);
    ioe8->dataIn = 0;
    ioe8->dataOut = 0;
    io_PCF8574_rst(ioe8);
}

void io_PCF8574_end(io_PCF8574_t* ioe8) {
    dprintf("ioe8 end\n");
}

unsigned char io_PCF8574_I2C_io(io_PCF8574_t* ioe8, unsigned char scl, unsigned char sda) {
    unsigned char ret = bitbang_i2c_io(&ioe8->bb_i2c, scl, sda);

    switch (bitbang_i2c_get_status(&ioe8->bb_i2c)) {
        case I2C_DATAW:
            ioe8->dataIn = ioe8->bb_i2c.datar;
            dprintf("ioe8 write =%02X\n", ioe8->dataIn);
            break;
        case I2C_DATAR:
            bitbang_i2c_send(&ioe8->bb_i2c, ioe8->dataOut);
            dprintf("ioe8 read =%02X\n", ioe8->dataOut);
            break;
    }

    return ret;
}
