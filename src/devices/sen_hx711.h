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

#include "../lib/board.h"
#include "bitbang_spi.h"

typedef struct {
    uint32_t tstart;
    bitbang_spi_t bb_spi;
    board* pboard;
    unsigned int weight;
    unsigned char power;
    unsigned char aclk;
    unsigned char ready;
} sen_hx711_t;

void sen_hx711_rst(sen_hx711_t* hx711);
void sen_hx711_init(sen_hx711_t* hx711, board* pboard);
void sen_hx711_end(sen_hx711_t* hx711);
void sen_hx711_setWeight(sen_hx711_t* hx711, const float weight);

unsigned short sen_hx711_io_SPI(sen_hx711_t* hx711, unsigned char clk);
