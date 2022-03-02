/* ########################################################################

   PICsimLab - PIC laboratory simulator

   ########################################################################

   Copyright (c) : 2010-2018  Luis Claudio Gambôa Lopes

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

#ifndef LCD_H
#define LCD_H

#include <lxrad.h>

#define DDRMAX 80

#define L_FNT 0x0200  // Sets character font
#define L_NLI 0x0100  // Sets number of display line

#define L_DL 0x0080  // Sets interface data length

#define L_LR 0x0040  // Sets shift direction
#define L_CD 0x0020  // Sets cursor-move or display-shift

#define L_DID 0x0010  // Sets cursor move direction
#define L_DSH 0x0008  // specifies to shift the display

#define L_DON 0x0004  // Sets On/Off of all display
#define L_CON 0x0002  // Sets cursor On/Off
#define L_CBL 0x0001  // Set blink of cursor position character

/*
I/D  	0 = Decrement cursor position  	1 = Increment cursor position
S 	0 = No display shift 		1 = Display shift
D 	0 = Display off 		1 = Display on
C 	0 = Cursor off 			1 = Cursor on
B 	0 = Cursor blink off 		1 = Cursor blink on
S/C 	0 = Move cursor 		1 = Shift display
R/L 	0 = Shift left 			1 = Shift right
DL 	0 = 4-bit interface 		1 = 8-bit interface
N 	0 = 1/8 or 1/11 Duty (1 line) 	1 = 1/16 Duty (2 lines)
F 	0 = 5x7 dots 			1 = 5x10 dots
BF  	0 = Can accept instruction  	1 = Internal operation in progress
*/

#define LCD_ADDR_CGRAM 0
#define LCD_ADDR_DDRAM 1

typedef struct {
    unsigned short int flags;
    unsigned char addr_counter;
    unsigned char addr_mode;
    unsigned char update;     // redraw
    unsigned int blinkc;      // blink count timer
    unsigned char blink;      // cursor state
    char shift;               // display shift
    char ddram_char[DDRMAX];  // ddram
    char cgram[8][5];         // cgram font mapped
    char cgram_char[64];      // cgram
    char bc;
    char buff;
    unsigned char cnum;  // number of columns 16 or 20
    unsigned char lnum;  // number of lines 1,2 or 4
} lcd_t;

void lcd_cmd(lcd_t* lcd, char cmd);

void lcd_data(lcd_t* lcd, char data);

unsigned char lcd_read_busyf_acounter(lcd_t* lcd);

char lcd_read_data(lcd_t* lcd);

void lcd_rst(lcd_t* lcd);

void lcd_init(lcd_t* lcd, unsigned char cnum, unsigned char lnum);

void lcd_on(lcd_t* lcd, int onoff);

void lcd_blink(lcd_t* lcd);

void lcd_draw(lcd_t* lcd, CCanvas* canvas, int x1, int y1, int w1, int h1, int picpwr);

#endif
