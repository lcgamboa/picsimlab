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

#ifndef LCD_SSD1306
#define LCD_SSD1306

#include <lxrad.h>
#include "bitbang_i2c.h"
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
    unsigned short int ram[128][8];
    unsigned char hrst;
    unsigned char dat;
    unsigned char am;   // address mode
    unsigned char inv;  // inverted
    unsigned char on;   // on/off
    unsigned char x, y;
    unsigned char update;
    unsigned char last_cmd;
    unsigned char cmd_argc;
    unsigned char cmd_val;
    unsigned char dc;

    unsigned char col_start;
    unsigned char col_end;
    unsigned char pag_start;
    unsigned char pag_end;

    bitbang_spi_t bb_spi;
    bitbang_i2c_t bb_i2c;
} lcd_ssd1306_t;

void lcd_ssd1306_rst(lcd_ssd1306_t* lcd);
void lcd_ssd1306_init(lcd_ssd1306_t* lcd);
void lcd_ssd1306_update(lcd_ssd1306_t* lcd);

unsigned char lcd_ssd1306_SPI_io(lcd_ssd1306_t* lcd, unsigned char din, unsigned char clk, unsigned char ncs,
                                 unsigned char nrst, unsigned char dc);
unsigned char lcd_ssd1306_I2C_io(lcd_ssd1306_t* lcd, unsigned char sda, unsigned char scl);

void lcd_ssd1306_draw(lcd_ssd1306_t* lcd, CCanvas* canvas, int x1, int y1, int w1, int h1, int picpwr);

#endif  // LCD_SSD1306
