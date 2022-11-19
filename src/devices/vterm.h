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

#ifndef VTERM
#define VTERM

#include <stdio.h>
#include "bitbang_uart.h"

#define SBUFFMAX 512

typedef struct {
    bitbang_uart_t bb_uart;
    unsigned char buff_in[SBUFFMAX];
    unsigned int count_in;
    unsigned char buff_out[SBUFFMAX];
    unsigned int count_out;
    unsigned char out_ptr;
    lxMutex* inMutex;
} vterm_t;

void vterm_rst(vterm_t* vt);
void vterm_init(vterm_t* vt, board* pboard);
void vterm_end(vterm_t* vt);
void vterm_set_speed(vterm_t* vt, const unsigned int speed);

unsigned char vterm_io(vterm_t* vt, const unsigned char rx);

#endif  // VTERM
