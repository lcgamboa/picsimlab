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

#include "bitbang_uart.h"

#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#define dprintf \
    if (1) {    \
    } else      \
        printf

void bitbang_uart_rst(bitbang_uart_t* bu) {
    bu->prx = 1;
    bu->insr = 0;
    bu->outsr = 1;
    bu->bcr = 0;
    bu->bcw = 0;
    bu->leds = 0;
    bu->data_recv = 0;
    bu->data_to_send = 0;
    dprintf("uart rst\n");
}

static void bitbang_uart_rx_callback(void* arg) {
    bitbang_uart_t* bu = (bitbang_uart_t*)arg;

    if (bu->bcr == 1) {
        bu->pboard->TimerChange_us(bu->TimerRXID, 1e6 / bu->speed);  // sample on middle
    }

    if (bu->prx) {
        bu->insr = (bu->insr >> 1) | 0x8000;
    } else {
        bu->insr = (bu->insr >> 1) & 0x7FFF;
    }

    bu->bcr++;

    if (bu->bcr > 9)  // start+eight bits+ stop
    {
        if (bu->data_recv) {
            dprintf("uart rx override error !!!!!!!\n");
        }
        bu->datar = bu->insr >> 8;
        bu->data_recv = 1;
        bu->bcr = 0;
        bu->pboard->TimerSetState(bu->TimerRXID, 0);
        ioupdated = 1;  // to check for new bytes
        dprintf("uart rx 0x%02X (%c)\n", bu->datar, bu->datar);

        if (bu->CallbackRX) {
            (*bu->CallbackRX)(bu->ArgRX);
        }
    }
}

static void bitbang_uart_tx_callback(void* arg) {
    bitbang_uart_t* bu = (bitbang_uart_t*)arg;

    bu->outsr = (bu->outsr >> 1);
    bu->bcw++;
    ioupdated = 1;
    if (bu->bcw > 10) {
        bu->bcw = 0;
        bu->pboard->TimerSetState(bu->TimerTXID, 0);
    }
}

void bitbang_uart_init(bitbang_uart_t* bu, board* pboard, void (*CallbackRX)(void* argRX), void* ArgRX) {
    bitbang_uart_rst(bu);
    bu->speed = 9600;
    bu->pboard = pboard;
    bu->TimerRXID = bu->pboard->TimerRegister_ms(1, bitbang_uart_rx_callback, bu);
    bu->pboard->TimerSetState(bu->TimerRXID, 0);
    bu->TimerTXID = bu->pboard->TimerRegister_ms(1, bitbang_uart_tx_callback, bu);
    bu->pboard->TimerSetState(bu->TimerTXID, 0);
    bu->CallbackRX = CallbackRX;
    bu->ArgRX = ArgRX;
    dprintf("uart init\n");
}

void bitbang_uart_end(bitbang_uart_t* bu) {
    bu->pboard->TimerUnregister(bu->TimerRXID);
    bu->pboard->TimerUnregister(bu->TimerTXID);
    dprintf("uart end\n");
}

void bitbang_uart_set_speed(bitbang_uart_t* bu, const unsigned int speed) {
    if (speed) {
        bu->speed = speed;
    } else {
        bu->speed = 1;
    }
    dprintf("uart set speed %u\n", speed);
}

unsigned char bitbang_uart_io(bitbang_uart_t* bu, const unsigned char rx) {
    // start bit
    if ((bu->prx == 1) && (rx == 0) && (!bu->bcr))  // falling edge
    {
        // dprintf("uart start bit \n");
        bu->bcr = 1;
        bu->insr = 0;
        bu->leds |= 0x01;
        bu->pboard->TimerChange_us(bu->TimerRXID, 0.5e6 / bu->speed);  // 0.5 space
        bu->pboard->TimerSetState(bu->TimerRXID, 1);
    }

    bu->prx = rx;

    return (bu->outsr & 0x01);
}

unsigned char bitbang_uart_transmitting(bitbang_uart_t* bu) {
    return (bu->bcw > 0);
}

void bitbang_uart_send(bitbang_uart_t* bu, const unsigned char data) {
    bu->dataw = data;
    bu->data_to_send = 1;

    dprintf("uart tx 0x%02X (%c)\n", bu->dataw, bu->dataw);
    bu->outsr = (bu->dataw << 1) | 0xFE00;
    ioupdated = 1;
    bu->bcw = 1;
    bu->leds |= 0x02;

    bu->pboard->TimerChange_us(bu->TimerTXID, 1e6 / bu->speed);
    bu->pboard->TimerSetState(bu->TimerTXID, 1);
}

unsigned char bitbang_uart_data_available(bitbang_uart_t* bu) {
    return bu->data_recv;
}

unsigned char bitbang_uart_recv(bitbang_uart_t* bu) {
    bu->data_recv = 0;
    return bu->datar;
}
