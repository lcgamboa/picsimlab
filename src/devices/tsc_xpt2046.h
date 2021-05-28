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


#ifndef TSC_XPT2046
#define	TSC_XPT2046

#include"bitbang_spi.h"

typedef struct
{
unsigned int height;
unsigned int width;
int x;
int y;
unsigned char cmd;
unsigned char pint;
bitbang_spi_t bb_spi;
}tsc_XPT2046_t;


void tsc_XPT2046_rst(tsc_XPT2046_t *tsc_);
void tsc_XPT2046_init(tsc_XPT2046_t *tsc_, unsigned int w, unsigned int h);
void tsc_XPT2046_set_pos(tsc_XPT2046_t *tsc_, int x , int y);

unsigned char  tsc_XPT2046_SPI_io(tsc_XPT2046_t *tsc_, unsigned char clk, unsigned char din, unsigned char cs);

#endif //TSC_XPT2046

