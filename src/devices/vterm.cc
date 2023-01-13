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

#include "vterm.h"

#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#define dprintf \
    if (1) {    \
    } else      \
        printf

void vterm_rst(vterm_t* vt) {
    bitbang_uart_rst(&vt->bb_uart);
    vt->count_in = 0;
    vt->count_out = 0;
    vt->out_ptr = 0;
    dprintf("vterm_ uart\n");
}

static void vterm_uart_rx_callback(bitbang_uart_t* bu, void* arg) {
    vterm_t* vt = (vterm_t*)arg;
    unsigned char data;
    vt->inMutex->Lock();
    data = bitbang_uart_recv(&vt->bb_uart);
    vt->inMutex->Unlock();

    if (((data > 0x19) && (data < 0x7F)) || (data == '\r') || (data == '\n')) {
        vt->buff_in[vt->count_in] = data;
        dprintf("vterm buff_in[%i] = %c\n", vt->count_in, vt->buff_in[vt->count_in]);

    } else {
        vt->buff_in[vt->count_in] = '.';
        dprintf("vterm buff_in[%i] = 0x%02X (invalid)\n", vt->count_in, vt->buff_in[vt->count_in]);
    }

    if (vt->ReceiveCallback) {
        vt->ReceiveCallback(data);
    }

    vt->count_in++;
    if (vt->count_in >= SBUFFMAX) {
        vt->count_in = 0;
        dprintf("vterm buffer overflow!\n");
    }
}

void vterm_init(vterm_t* vt, board* pboard) {
    bitbang_uart_init(&vt->bb_uart, pboard, vterm_uart_rx_callback, vt);
    vt->ReceiveCallback = NULL;
    vterm_rst(vt);
    vt->inMutex = new lxMutex();
    dprintf("init vterm\n");
}

void vterm_end(vterm_t* vt) {
    delete vt->inMutex;
}

unsigned char vterm_io(vterm_t* vt, const unsigned char rx) {
    unsigned char ret;

    if (!bitbang_uart_transmitting(&vt->bb_uart)) {
        if (vt->count_out) {
            bitbang_uart_send(&vt->bb_uart, vt->buff_out[vt->out_ptr++]);
            if (vt->out_ptr == vt->count_out) {
                vt->out_ptr = 0;
                vt->count_out = 0;
            }
        }
    }

    ret = bitbang_uart_io(&vt->bb_uart, rx);

    return ret;
}

void vterm_set_speed(vterm_t* vt, const unsigned int speed) {
    bitbang_uart_set_speed(&vt->bb_uart, speed);
}