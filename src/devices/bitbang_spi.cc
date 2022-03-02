/* ########################################################################

   PICsimLab - PIC laboratory simulator

   ########################################################################

   Copyright (c) : 2020-2021  Luis Claudio Gambôa Lopes

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

#include "bitbang_spi.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define dprintf \
    if (1) {    \
    } else      \
        printf

void bitbang_spi_rst(bitbang_spi_t* spi) {
    spi->aclk = 1;
    spi->insr = 0;
    spi->outsr = 0;
    spi->bit = 0;
    spi->byte = 0;
    spi->status = 0;
    dprintf("bitbang_spi rst\n");
}

void bitbang_spi_init(bitbang_spi_t* spi, const unsigned char lenght) {
    dprintf("bitbang_spi init \n");

    spi->outbitmask = 1 << (lenght - 1);

    spi->lenght = lenght;
    bitbang_spi_rst(spi);
}

unsigned char bitbang_spi_io(bitbang_spi_t* spi, const unsigned char clk, const unsigned char din,
                             const unsigned char cs) {
    if (cs)  // not selected
    {
        spi->insr = 0;
        spi->outsr = 0;
        spi->bit = 0;
        spi->byte = 0;
        return 1;
    }

    // transicao
    if ((!spi->aclk) && (clk))  // rising edge
    {
        if (din) {
            spi->insr = (spi->insr << 1) | 1;
        } else {
            spi->insr = (spi->insr << 1) & 0xFFFFFFFE;
        }
        spi->outsr = (spi->outsr << 1);

        spi->bit++;

        if (spi->bit == spi->lenght) {
            spi->status = SPI_DATA;
            spi->data8 = spi->insr & 0xFF;
            spi->bit = 0;
            spi->byte++;
        } else {
            spi->status = SPI_BIT;
        }

        spi->ret = ((spi->outsr & spi->outbitmask) > 0);
    }
    spi->aclk = clk;

    return spi->ret;
}

#define ioSPI_clk 0
#define ioSPI_din 1
#define ioSPI_cs 2

unsigned char bitbang_spi_io_(bitbang_spi_t* spi, const unsigned char** pins_value) {
    if (*pins_value[ioSPI_cs])  // not selected
    {
        spi->insr = 0;
        spi->outsr = 0;
        spi->bit = 0;
        spi->byte = 0;
        return 1;
    }

    // transicao
    if ((!spi->aclk) && (*pins_value[ioSPI_clk]))  // rising edge
    {
        if (*pins_value[ioSPI_din]) {
            spi->insr = (spi->insr << 1) | 1;
        } else {
            spi->insr = (spi->insr << 1) & 0xFFFFFFFE;
        }
        spi->outsr = (spi->outsr << 1);

        spi->bit++;

        if (spi->bit == spi->lenght) {
            spi->status = SPI_DATA;
            spi->data8 = spi->insr & 0xFF;
            spi->bit = 0;
            spi->byte++;
        } else {
            spi->status = SPI_BIT;
        }

        spi->ret = ((spi->outsr & spi->outbitmask) > 0);
    }
    spi->aclk = *pins_value[ioSPI_clk];

    return spi->ret;
}

unsigned char bitbang_spi_get_status(bitbang_spi_t* spi) {
    unsigned char status = spi->status;
    spi->status = 0;
    return status;
}

void bitbang_spi_send(bitbang_spi_t* spi, const unsigned int data) {
    spi->outsr = data;
    spi->ret = ((spi->outsr & spi->outbitmask) > 0);
    dprintf("bitbang_spi data to send 0x%02x \n", data);
}