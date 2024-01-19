/* ########################################################################

   PICSimLab - Programmable IC Simulator Laboratory

   ########################################################################

   Copyright (c) : 2020-2024  Luis Claudio Gambôa Lopes <lcgamboa@yahoo.com>

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

#ifndef BITBANG_UART
#define BITBANG_UART

#include "../lib/board.h"

class board;

typedef struct bitbang_uart_t bitbang_uart_t;

struct bitbang_uart_t {
    unsigned char prx;           // previous rx value
    unsigned short insr;         // input shift register
    unsigned short outsr;        // output shift register
    unsigned int bcr;            // bit counter read
    unsigned int bcw;            // bit counter write
    unsigned long speed;         // serial speed (baud)
    unsigned int leds;           // rx tx leds status
    unsigned char datar;         // data received
    unsigned char data_recv;     // flag data received
    unsigned char dataw;         // data to write
    unsigned char data_to_send;  // flag data to write
    int TimerRXID;
    int TimerTXID;
    board* pboard;
    void (*CallbackRX)(bitbang_uart_t* bu, void* arg);
    void* ArgRX;
    // Controller
    unsigned char ctrl_on;
    unsigned char tx_pin;
    unsigned char tx_value;
    unsigned char rx_pin;
    unsigned char rx_value;
};

void bitbang_uart_rst(bitbang_uart_t* bu);
void bitbang_uart_init(bitbang_uart_t* bu, board* pboard, void (*CallbackRX)(bitbang_uart_t* bu, void* argRX),
                       void* Arg);
void bitbang_uart_end(bitbang_uart_t* bu);
void bitbang_uart_set_speed(bitbang_uart_t* bu, const unsigned int speed);
unsigned char bitbang_uart_transmitting(bitbang_uart_t* bu);
void bitbang_uart_send(bitbang_uart_t* bu, const unsigned char data);
unsigned char bitbang_uart_data_available(bitbang_uart_t* bu);
unsigned char bitbang_uart_recv(bitbang_uart_t* bu);

unsigned char bitbang_uart_io(bitbang_uart_t* bu, const unsigned char rx);

#endif  // BITBANG_UART
