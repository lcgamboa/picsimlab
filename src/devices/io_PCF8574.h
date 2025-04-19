/* ########################################################################

   PICSimLab - Programmable IC Simulator Laboratory

   ########################################################################

   Copyright (c) : 2010-2025  Luis Claudio Gambôa Lopes <lcgamboa@yahoo.com>

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

typedef struct {
    bitbang_i2c_t bb_i2c;
    unsigned char dataIn;
    unsigned char dataOut;
} io_PCF8574_t;

void io_PCF8574_rst(io_PCF8574_t* ioe8);
void io_PCF8574_init(io_PCF8574_t* ioe8);
void io_PCF8574_end(io_PCF8574_t* ioe8);
void io_PCF8574_set_addr(io_PCF8574_t* ioe8, unsigned char addr);

unsigned char io_PCF8574_I2C_io(io_PCF8574_t* ioe8, unsigned char scl, unsigned char sda);
