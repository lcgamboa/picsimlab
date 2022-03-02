/* ########################################################################

   PICsimLab - PIC laboratory simulator

   ########################################################################

   Copyright (c) : 2020-2020  Luis Claudio Gambôa Lopes

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

#ifndef LCD_ILI9341
#define LCD_ILI9341

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
    unsigned long int ram[240][320];
    unsigned char pwr;  // previous wr
    unsigned char prd;  // previous rw
    bitbang_spi_t bb_spi;
    unsigned char hrst;
    unsigned char dat;
    unsigned char am;   // address mode
    unsigned char mac;  // memory access control
    unsigned char pf;   // pixel format
    unsigned char inv;  // inverted
    unsigned char on;   // on/off
    unsigned short x;
    unsigned short y;
    unsigned char update;
    unsigned char last_cmd;
    unsigned char cmd_argc;
    unsigned char cmd_val;
    unsigned char dc;
    unsigned short out;  // output value

    unsigned short col_start;
    unsigned short col_end;
    unsigned short pag_start;
    unsigned short pag_end;

    unsigned long color;
} lcd_ili9341_t;

void lcd_ili9341_rst(lcd_ili9341_t* lcd);
void lcd_ili9341_init(lcd_ili9341_t* lcd);
void lcd_ili9341_update(lcd_ili9341_t* lcd);

unsigned char lcd_ili9341_SPI_io(lcd_ili9341_t* lcd, const unsigned char** pins_value);
unsigned short lcd_ili9341_8_io(lcd_ili9341_t* lcd, const unsigned char** pins_value);

void lcd_ili9341_draw(lcd_ili9341_t* lcd, CCanvas* canvas, const int x1, const int y1, const int w1, const int h1,
                      const int picpwr);

#endif  // LCD_ILI9341
