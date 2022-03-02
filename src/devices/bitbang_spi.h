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

#ifndef BITBANG_SPI
#define BITBANG_SPI

// operation status
#define SPI_DATA 0x01
#define SPI_BIT 0x02

typedef struct {
    unsigned char aclk;
    unsigned int insr;
    unsigned int outsr;
    unsigned char bit;
    unsigned int byte;
    unsigned short status;
    unsigned char data8;
    unsigned char ret;
    unsigned char lenght;
    unsigned int outbitmask;
} bitbang_spi_t;

void bitbang_spi_init(bitbang_spi_t* spi, const unsigned char lenght = 8);
void bitbang_spi_rst(bitbang_spi_t* spi);
unsigned char bitbang_spi_get_status(bitbang_spi_t* spi);
void bitbang_spi_send(bitbang_spi_t* spi, const unsigned int data);

// periferic
unsigned char bitbang_spi_io(bitbang_spi_t* spi, const unsigned char clk, const unsigned char din,
                             const unsigned char cs);
unsigned char bitbang_spi_io_(bitbang_spi_t* spi, const unsigned char** pins_value);

#endif  // BITBANG_SPI