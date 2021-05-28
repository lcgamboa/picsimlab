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


#include"bitbang_spi.h"

#include<stdio.h>
#include<string.h>
#include<stdlib.h>

#define dprintf if (1) {} else printf

void
bitbang_spi_rst(bitbang_spi_t *spi)
{
 spi->aclk = 1;
 spi->insr = 0;
 spi->outsr = 0;
 spi->bit = 0;
 spi->byte = 0;
 spi->status = 0;
 dprintf ("bitbang_spi rst\n");
}

void
bitbang_spi_init(bitbang_spi_t *spi, unsigned char lenght)
{
 dprintf ("bitbang_spi init \n");

 spi->outbitmask = 1 << (lenght - 1);

 spi->lenght = lenght;
 bitbang_spi_rst (spi);
}

unsigned char
bitbang_spi_io(bitbang_spi_t *spi, unsigned char clk, unsigned char mosi, unsigned char cs)
{

 if (cs)//not selected
  {
   spi->insr = 0;
   spi->outsr = 0;
   spi->bit = 0;
   spi->byte = 0;
   return 1;
  }

 //transicao
 if ((spi->aclk == 0)&&(clk == 1))//rising edge
  {
   if (mosi)
    {
     spi->insr = (spi->insr << 1) | 1;
    }
   else
    {
     spi->insr = (spi->insr << 1) & 0xFFFFFFFE;
    }
   spi->outsr = (spi->outsr << 1);

   spi->bit++;

   if (spi->bit == spi->lenght)
    {
     spi->status = SPI_DATA;
     spi->data8 = spi->insr & 0xFF;
     spi->bit = 0;
     spi->byte++;
    }

   spi->ret = ((spi->outsr & spi->outbitmask) > 0);
  }
 spi->aclk = clk;

 return spi->ret;
}

unsigned char
bitbang_spi_get_status(bitbang_spi_t *spi)
{
 unsigned char status = spi->status;
 spi->status = 0;
 return status;
}

void
bitbang_spi_send(bitbang_spi_t *spi, unsigned int data)
{
 spi->outsr = data;
 spi->ret = ((spi->outsr & spi->outbitmask) > 0);
 dprintf ("bitbang_spi data to send 0x%02x \n", data);
}