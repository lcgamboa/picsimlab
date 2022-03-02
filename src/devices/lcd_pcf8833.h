/* ########################################################################

   PICsimLab - PIC laboratory simulator

   ########################################################################

   Copyright (c) : 2010-2021  Luis Claudio Gambôa Lopes

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

#ifndef LCD_PCF8833
#define LCD_PCF8833

#include <lxrad.h>
#include "bitbang_spi.h"

/* pinout
  1 VCC
  2 NOT_RESET
  3 DATA
  4 CLK
  5 NOT_CS
  6 VCC
  7 NC
  8 GND
  9 VLED-
 10 VLED+
*/

typedef struct {
    unsigned int ram[132][132];
    bitbang_spi_t bb_spi;
    int dc;
    unsigned char colm;
    unsigned char madctl;
    unsigned char hrst;
    unsigned char tp, dat;
    unsigned char x, y, r, g, b;
    unsigned int color;
    unsigned char update;
    unsigned char command;
    unsigned char rmin;
    unsigned char rmax;
    unsigned char cmin;
    unsigned char cmax;
} lcd_pcf8833_t;

void lcd_pcf8833_rst(lcd_pcf8833_t* lcd);
void lcd_pcf8833_init(lcd_pcf8833_t* lcd);
void lcd_pcf8833_update(lcd_pcf8833_t* lcd);

unsigned char lcd_pcf8833_io(lcd_pcf8833_t* lcd, unsigned char pdat, unsigned char clk, unsigned char ncs,
                             unsigned char nrst);

void lcd_pcf8833_draw(lcd_pcf8833_t* lcd, CCanvas* canvas, int x1, int y1, int w1, int h1, int picpwr);

#endif  // LCD_PCF8833
