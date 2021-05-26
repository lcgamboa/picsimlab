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

#include"bitbang_i2c.h"

typedef struct {
    bitbang_i2c_t bb_i2c;

    unsigned int SIZE;
    unsigned char ADDRB;

    unsigned char *data;
    unsigned short addr;

} mi2c_t;


void mi2c_init_null(mi2c_t *mem);
void mi2c_rst(mi2c_t *mem);
void mi2c_init(mi2c_t *mem, int sizekbits);
void mi2c_end(mi2c_t *mem);
void mi2c_set_addr(mi2c_t *mem, unsigned char addr);

unsigned char mi2c_io(mi2c_t *mem, unsigned char scl, unsigned char sda);
