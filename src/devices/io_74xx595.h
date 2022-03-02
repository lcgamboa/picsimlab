/* ########################################################################

   PICsimLab - PIC laboratory simulator

   ########################################################################

   Copyright (c) : 2010-2020  Luis Claudio Gambôa Lopes

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

#ifndef IO_74XX595
#define IO_74XX595

/*
 pinout
1  QB
2  QC
3  QD
4  QE
5  QF
6  QG
7  QH
8  GND
9  SQH  -> serial out
10 /RESET -> reset only ff
11 SHIFT CLK
12 LATCH CLK
12 /OE
14 A   -> serial in
15 QA
16 VCC
*/

typedef struct {
    unsigned char asclk;
    unsigned char alclk;
    unsigned short dsr;
    unsigned char out;
    unsigned char sout;
} io_74xx595_t;

void io_74xx595_rst(io_74xx595_t* sr);
void io_74xx595_init(io_74xx595_t* sr);

unsigned short io_74xx595_io(io_74xx595_t* sr, unsigned char A, unsigned char sclk, unsigned char lclk,
                             unsigned char rst);

#endif  // IO_74XX595
