/* ########################################################################

   PICSimLab - Programmable IC Simulator Laboratory

   ########################################################################

   Copyright (c) : 2023-2026  Luis Claudio Gambôa Lopes <lcgamboa@yahoo.com>

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

#include "bitbang_out.h"
#include "../lib/board.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define dprintf \
    if (1) {    \
    } else      \
        printf

void bitbang_out_rst(bitbang_out_t* out) {
    dprintf("bitbang_out rst\n");
    for (int i = 0; i < out->channels_count; i++) {
        out->out[i] = 0;
    }
}

void bitbang_out_init(bitbang_out_t* out, board* pboard, const unsigned char channels_count) {
    dprintf("bitbang_out init\n");
    out->channels_count = channels_count;
    bitbang_out_rst(out);
    out->pboard = pboard;
    memset(out->pins, 0, OUT_MAX);
    memset(out->out, 0, OUT_MAX);
}

void bitbang_out_end(bitbang_out_t* out) {
    dprintf("bitbang_out end\n");
}
