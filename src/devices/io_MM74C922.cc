/* ########################################################################

   PICSimLab - Programmable IC Simulator Laboratory

   ########################################################################

   Copyright (c) : 2021-2025  Luis Claudio Gambôa Lopes <lcgamboa@yahoo.com>

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

#include "io_MM74C922.h"

void io_MM74C922_rst(io_MM74C922_t* kc) {
    kc->out = 0;
    kc->count = 0;
    kc->cols = 0x1;
    kc->da = 0;
    kc->key = 0;
    kc->srate = 1000000;
}

void io_MM74C922_init(io_MM74C922_t* kc) {
    io_MM74C922_rst(kc);
}

#include <stdio.h>

unsigned short io_MM74C922_io(io_MM74C922_t* kc, unsigned char Y) {
    if ((Y & 0xF) != 0x0F) {
        // key pressed
        int row = 0;
        int col = 0;
        if (!(Y & 0x01)) {
            row = 0;
        } else if (!(Y & 0x02)) {
            row = 1;
        } else if (!(Y & 0x04)) {
            row = 2;
        } else if (!(Y & 0x08)) {
            row = 3;
        }

        switch (kc->cols) {
            case 1:
                col = 0;
                break;
            case 2:
                col = 1;
                break;
            case 4:
                col = 2;
                break;
            case 8:
                col = 3;
                break;
        }

        kc->da = 1;
        kc->key = col + (row * 4);

    } else {
        kc->da = 0;
        kc->count++;
    }

    kc->out = (kc->key << 5) | (kc->da << 4) | (0x0F & (~kc->cols));

    // increment columns
    if (kc->count > kc->srate) {
        kc->count = 0;
        kc->cols = kc->cols << 1;
        if (kc->cols > 8) {
            kc->cols = 1;
        }
    }

    return kc->out;
}

void io_MM74C922_set_clk_freq(io_MM74C922_t* kc, const unsigned long freq) {
    kc->freq = freq;
    kc->srate = freq / 50;
}