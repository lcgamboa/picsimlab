/* ########################################################################

   PICsimLab - PIC laboratory simulator

   ########################################################################

   Copyright (c) : 2010-2019  Luis Claudio Gambôa Lopes

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

#ifndef LDD_MAX72XX
#define LDD_MAX72XX

#include <lxrad.h>
#include "bitbang_spi.h"

/* pinout
  1 DIN
  2 DIG0
  3 DIG4
  4 GND
  5 DIG6
  6 DIG2
  7 DIG3
  8 DIG7
  9 GND
  10 DIG5
  11 DIG1
  12 /CS
  13 CLK
  14 SEGA
  15 SEGF
  16 SEGB
  17 SEGG
  18 ISET
  19 VCC
  20 SEGC
  21 SEGE
  22 SEGDP
  23 SEGD
  24 DOUT
*/

typedef struct {
    unsigned char ram[8];
    bitbang_spi_t bb_spi;
    unsigned short ancs;
    unsigned short dat;
    unsigned char update;
    unsigned short dout;
} ldd_max72xx_t;

void ldd_max72xx_rst(ldd_max72xx_t* ldd);
void ldd_max72xx_init(ldd_max72xx_t* ldd);
void ldd_max72xx_update(ldd_max72xx_t* ldd);

unsigned char ldd_max72xx_io(ldd_max72xx_t* ldd, unsigned char din, unsigned char clk, unsigned char ncs);

void ldd_max72xx_draw(ldd_max72xx_t* ldd, CCanvas* canvas, int x1, int y1, int w1, int h1, int picpwr, int angle);

#endif  // LDD_MAX72XX
