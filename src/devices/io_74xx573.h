/* ########################################################################

   PICsimLab - PIC laboratory simulator

   ########################################################################

   Copyright (c) : 2021-2022  Luis Claudio Gambôa Lopes

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

#ifndef IO_74XX573
#define IO_74XX573

/*
 pinout
1  /OE
2  D0
3  D1
4  D2
5  D3
6  D4
7  D5
8  D6
9  D7
10 GND
11 LE
12 O7
12 O6
14 O5
15 O4
16 O3
17 O2
18 O1
19 O0
20 VCC
*/

typedef struct {
    unsigned char latch;
} io_74xx573_t;

void io_74xx573_rst(io_74xx573_t* lt);
void io_74xx573_init(io_74xx573_t* lt);

unsigned char io_74xx573_io(io_74xx573_t* lt, const unsigned char le, const unsigned char in);

#endif  // IO_74XX573
