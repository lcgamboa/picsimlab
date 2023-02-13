/* ########################################################################

   PICSimLab - Programmable IC Simulator Laboratory

   ########################################################################

   Copyright (c) : 2021-2023  Luis Claudio Gambôa Lopes <lcgamboa@yahoo.com>

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

#ifndef IO_MM74C922
#define IO_MM74C922

/*
 pinout
1  ROW Y1
2  ROW Y2
3  ROW Y3
4  ROW Y4
5  OSC    1 uF  (50Hz scan rate)
6  KBM    0.1uF (1ms debounce time)
7  COL X4
8  COL X3
9  GND
10 COL X2
11 COL X1
12 DA
12 /OE
14 D
15 C
16 B
17 A
18 VCC
*/

typedef struct {
    unsigned long freq;
    unsigned int count;
    unsigned char cols;
    unsigned short out;
    unsigned char da;
    unsigned char key;
    unsigned int srate;
} io_MM74C922_t;

void io_MM74C922_rst(io_MM74C922_t* kc);
void io_MM74C922_init(io_MM74C922_t* kc);
void io_MM74C922_set_clk_freq(io_MM74C922_t* kc, const unsigned long freq);

unsigned short io_MM74C922_io(io_MM74C922_t* kc, unsigned char Y);

#endif  // IO_MM74C922
