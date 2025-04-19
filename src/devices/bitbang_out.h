/* ########################################################################

   PICSimLab - Programmable IC Simulator Laboratory

   ########################################################################

   Copyright (c) : 2023-2025  Luis Claudio Gambôa Lopes <lcgamboa@yahoo.com>

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

#ifndef BITBANG_OUT
#define BITBANG_OUT

class board;

#define OUT_MAX 32

typedef struct {
    unsigned char channels_count;
    board* pboard;
    unsigned char pins[OUT_MAX];
    unsigned char out[OUT_MAX];
} bitbang_out_t;

void bitbang_out_init(bitbang_out_t* out, board* pboard, const unsigned char channels = 8);
void bitbang_out_rst(bitbang_out_t* out);
void bitbang_out_end(bitbang_out_t* out);

#endif  // BITBANG_OUT