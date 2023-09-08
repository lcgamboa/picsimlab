/* ########################################################################

   PICSimLab - Programmable IC Simulator Laboratory

   ########################################################################

   Copyright (c) : 2020-2023  Luis Claudio Gambôa Lopes <lcgamboa@yahoo.com>

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

#include "uart.h"

#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#define dprintf \
    if (1) {    \
    } else      \
        printf

void uart_rst(uart_t* sr) {
    bitbang_uart_rst(&sr->bb_uart);
    dprintf("rst uart\n");
}

static void uart_rx_callback(bitbang_uart_t* bu, void* arg) {
    uart_t* sr = (uart_t*)arg;
    unsigned char data = bitbang_uart_recv(&sr->bb_uart);
    if (sr->connected) {
        serial_port_send(sr->serialfd, data);
    }
}

void uart_init(uart_t* sr, board* pboard) {
    bitbang_uart_init(&sr->bb_uart, pboard, uart_rx_callback, sr);
    sr->connected = 0;
    uart_rst(sr);
    sr->serialfd = INVALID_SERIAL;
    dprintf("init uart\n");
}

void uart_end(uart_t* sr) {
    if (sr->connected) {
        serial_port_close(&sr->serialfd);
        sr->connected = 0;
    }
    bitbang_uart_end(&sr->bb_uart);
}

unsigned char uart_io(uart_t* sr, const unsigned char rx) {
    if (!sr->connected) {
        return 1;
    }

    if (!bitbang_uart_transmitting(&sr->bb_uart)) {
        unsigned char data;
        if (serial_port_rec(sr->serialfd, &data)) {
            bitbang_uart_send(&sr->bb_uart, data);
        }
    }
    return bitbang_uart_io(&sr->bb_uart, rx);
}

void uart_set_port(uart_t* sr, const char* port, const unsigned int speed) {
    if (sr->connected) {
        serial_port_close(&sr->serialfd);
        sr->connected = 0;
    }

    if (serial_port_open(&sr->serialfd, port)) {
        sr->connected = 1;
        bitbang_uart_set_speed(&sr->bb_uart, speed);
        serial_port_cfg(sr->serialfd, speed);
        dprintf("uart serial open: %s  speed %i\n", port, speed);
    } else {
        sr->connected = 0;
        dprintf("uart serial error open: %s !!!\n", port);
    }
}