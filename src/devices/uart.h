/* ########################################################################

   PICSimLab - Programmable IC Simulator Laboratory

   ########################################################################

   Copyright (c) : 2020-2025  Luis Claudio Gambôa Lopes <lcgamboa@yahoo.com>

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

#ifndef UART
#define UART

#include "bitbang_uart.h"

#include <stdio.h>
#include "../lib/serial_port.h"

typedef struct {
    unsigned char connected;
    serialfd_t serialfd;
    bitbang_uart_t bb_uart;
} uart_t;

void uart_rst(uart_t* sr);
void uart_init(uart_t* sr, board* pboard);
void uart_end(uart_t* sr);
void uart_set_port(uart_t* sr, const char* port, const unsigned int speed);

unsigned char uart_io(uart_t* sr, const unsigned char rx);

#endif  // UART
