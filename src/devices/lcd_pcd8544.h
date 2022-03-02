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

#ifndef LCD_PCD8544
#define LCD_PCD8544

#include <lxrad.h>
#include "bitbang_spi.h"

/* pinout
  1 /RST
  2 /CE
  3 DC
  4 DIN
  5 CLK
  6 VCC
  7 BL
  8 GND
 */

typedef struct {
    unsigned short int ram[84][6];
    bitbang_spi_t bb_spi;
    unsigned char hrst;
    unsigned char dat;
    unsigned char h, v, d, e, pd;
    unsigned char x, y;
    unsigned char update;
} lcd_pcd8544_t;

void lcd_pcd8544_rst(lcd_pcd8544_t* lcd);
void lcd_pcd8544_init(lcd_pcd8544_t* lcd);
void lcd_pcd8544_update(lcd_pcd8544_t* lcd);

unsigned char lcd_pcd8544_io(lcd_pcd8544_t* lcd, unsigned char din, unsigned char clk, unsigned char ncs,
                             unsigned char nrst, unsigned char dc);

void lcd_pcd8544_draw(lcd_pcd8544_t* lcd, CCanvas* canvas, int x1, int y1, int w1, int h1, int picpwr);

#endif  // LCD_PCD8544
