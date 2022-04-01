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

#include "io_74xx573.h"

void io_74xx573_rst(io_74xx573_t* lt) {
    lt->latch = 0;
}

void io_74xx573_init(io_74xx573_t* lt) {
    io_74xx573_rst(lt);
}

unsigned char io_74xx573_io(io_74xx573_t* lt, const unsigned char le, const unsigned char in) {
    if (le) {
        lt->latch = in;
    }
    return lt->latch;
}
