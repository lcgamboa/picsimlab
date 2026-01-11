/* ########################################################################

   PICSimLab - Programmable IC Simulator Laboratory

   ########################################################################

   Copyright (c) : 2021-2026  Luis Claudio Gambôa Lopes <lcgamboa@yahoo.com>

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

#include "sen_hx711.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define dprintf \
    if (1) {    \
    } else      \
        printf

void sen_hx711_rst(sen_hx711_t* hx711) {
    bitbang_spi_rst(&hx711->bb_spi);
    hx711->weight = 0;
    hx711->power = 1;
    hx711->aclk = 0;
    hx711->ready = 0;
    dprintf("hx711 rst\n");
}

void sen_hx711_init(sen_hx711_t* hx711, board* pboard) {
    dprintf("hx711 init\n");
    hx711->pboard = pboard;
    bitbang_spi_init(&hx711->bb_spi, 25);
    sen_hx711_rst(hx711);
}

void sen_hx711_end(sen_hx711_t* hx711) {
    dprintf("hx711 end\n");
}

unsigned short sen_hx711_io_SPI(sen_hx711_t* hx711, unsigned char clk) {
    if ((!hx711->aclk) && (clk)) {  // rising edge
        hx711->tstart = hx711->pboard->GetInstCounter();
    } else if ((hx711->aclk) && (!clk)) {  // falling edge

        if (!hx711->power) {
            bitbang_spi_rst(&hx711->bb_spi);
            dprintf("hx711 power up\n");
        }
        hx711->power = 1;
    }
    hx711->aclk = clk;

    if (clk) {
        int pulse = hx711->pboard->GetInstCounter_us(hx711->tstart);
        if (pulse > 60) {
            if (hx711->power) {
                dprintf("hx711 power down\n");
                hx711->power = 0;
            }
        }
    }

    if (hx711->power) {
        bitbang_spi_io(&hx711->bb_spi, clk, 0, !hx711->ready);

        switch (bitbang_spi_get_status(&hx711->bb_spi)) {
            case SPI_DATA:
                hx711->ready = 0;
                break;
        }
    }
    return hx711->bb_spi.ret;
}

void sen_hx711_setWeight(sen_hx711_t* hx711, const float weight) {
    hx711->weight = (weight * 2280 * 2) + 926785;

    hx711->weight |= 0xFE000001;

    if (hx711->power) {
        hx711->ready = 1;
        bitbang_spi_send16(&hx711->bb_spi, hx711->weight);
        hx711->bb_spi.ret = 0;
    } else {
        hx711->bb_spi.ret = 1;
    }

    ioupdated = 1;
}
