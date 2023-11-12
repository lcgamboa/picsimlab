/* ########################################################################

   PICSimLab - Programmable IC Simulator Laboratory

   ########################################################################

   Copyright (c) : 2010-2023  Luis Claudio Gambôa Lopes <lcgamboa@yahoo.com>

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

#include "sen_dhtxx.h"
#include "../lib/board.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define dprintf \
    if (1) {    \
    } else      \
        printf

void sen_dhtxx_rst(sen_dhtxx_t* dhtxx) {
    dprintf("dhtxx rst\n");
    dhtxx->TempI = 0;
    dhtxx->TempD = 0;
    dhtxx->HumI = 0;
    dhtxx->HumD = 0;
    dhtxx->ldata = 0;
    dhtxx->start = 0;
    dhtxx->out = 1;
    dhtxx->state = -1;
}

static void sen_dhtxx_callback(void* arg) {
    sen_dhtxx_t* dhtxx = (sen_dhtxx_t*)arg;
    int state = dhtxx->state - 1;
    dprintf("dhtxx callback %i\n", dhtxx->state);
    if (state < 84) {
        dhtxx->out = state & 0x01;  // odd values are logic one
        dhtxx->pboard->TimerChange_us(dhtxx->TimerID, dhtxx->uvalues[state]);
        ioupdated = 1;
        dhtxx->state++;
    } else {
        dhtxx->pboard->TimerSetState(dhtxx->TimerID, 0);
        dhtxx->state = -1;
    }
}

void sen_dhtxx_init(sen_dhtxx_t* dhtxx, board* pboard, int type) {
    dprintf("dhtxx init\n");
    dhtxx->pboard = pboard;
    sen_dhtxx_rst(dhtxx);
    dhtxx->TimerID = dhtxx->pboard->TimerRegister_ms(1, sen_dhtxx_callback, dhtxx);
    dhtxx->pboard->TimerSetState(dhtxx->TimerID, 0);
    dhtxx->type = type;
}

void sen_dhtxx_end(sen_dhtxx_t* dhtxx) {
    dprintf("dhtxx end\n");
    dhtxx->pboard->TimerUnregister(dhtxx->TimerID);
}

unsigned char sen_dhtxx_io(sen_dhtxx_t* dhtxx, const unsigned char data) {
    // dprintf("data = %i ldata =%i state = %i \n", data, dhtxx->ldata, dhtxx->state);

    if (!data && dhtxx->ldata && (dhtxx->state == -1)) {  // falling edge start
        dhtxx->start = dhtxx->pboard->GetInstCounter();
        dhtxx->state = 0;
    } else if (data && !dhtxx->ldata && !dhtxx->state) {
        int pulse = dhtxx->pboard->GetInstCounter_us(dhtxx->start);
        int pmin, pmax;
        if (dhtxx->type == DHT11) {
            pmin = 17500;  // 18000 min - 500 tol
            pmax = 30500;  // 30000 max + 500 tol
        } else {           // DHT22
            pmin = 500;    // 1000 min  - 500 tol
            pmax = 10500;  // 10000 max + 500 tol
        }

        if ((pulse > pmin) && (pulse < pmax)) {  // valid start
            dhtxx->state = 1;

            dprintf("dhtxx start\n");
            dhtxx->pboard->TimerChange_us(dhtxx->TimerID, 30);
            dhtxx->pboard->TimerSetState(dhtxx->TimerID, 1);

            unsigned char checksum = dhtxx->HumI;
            checksum += dhtxx->HumD;
            checksum += dhtxx->TempI;
            checksum += dhtxx->TempD;

            dhtxx->uvalues[0] = 80;
            dhtxx->uvalues[1] = 80;

            for (int i = 0; i < 8; i++) {
                dhtxx->uvalues[2 + (2 * i)] = 50;
                dhtxx->uvalues[3 + (2 * i)] = (((dhtxx->HumI & (0x80 >> i)) > 0) ? 70 : 27);

                dhtxx->uvalues[18 + (2 * i)] = 50;
                dhtxx->uvalues[19 + (2 * i)] = (((dhtxx->HumD & (0x80 >> i)) > 0) ? 70 : 27);

                dhtxx->uvalues[34 + (2 * i)] = 50;
                dhtxx->uvalues[35 + (2 * i)] = (((dhtxx->TempI & (0x80 >> i)) > 0) ? 70 : 27);

                dhtxx->uvalues[50 + (2 * i)] = 50;
                dhtxx->uvalues[51 + (2 * i)] = (((dhtxx->TempD & (0x80 >> i)) > 0) ? 70 : 27);

                dhtxx->uvalues[66 + (2 * i)] = 50;
                dhtxx->uvalues[67 + (2 * i)] = (((checksum & (0x80 >> i)) > 0) ? 70 : 27);
            }
            dhtxx->uvalues[82] = 50;
            dhtxx->uvalues[83] = 50;  // this time is not used
        } else {
            dhtxx->state = -1;
        }
    }

    dhtxx->ldata = data;
    return dhtxx->out;
}

void sen_dhtxx_setTempHum(sen_dhtxx_t* dhtxx, const float temp, const float hum) {
    unsigned short itemp;
    if (dhtxx->type == DHT22) {
        unsigned short ihum = (hum * 10);
        if (temp >= 0) {
            itemp = (temp * 10);
            dhtxx->TempI = (itemp & 0xFF00) >> 8;
            dhtxx->TempD = itemp & 0x00FF;
        } else {
            itemp = (-temp * 10);
            dhtxx->TempI = (itemp & 0xFF00) >> 8;
            dhtxx->TempD = itemp & 0x00FF;
            dhtxx->TempI |= 0x80;
        }
        dhtxx->HumI = (ihum & 0xFF00) >> 8;
        dhtxx->HumD = ihum & 0x00FF;
    } else {  // DHT11
        dhtxx->TempI = temp;
        dhtxx->TempD = (temp - dhtxx->TempI) * 10;

        dhtxx->HumI = hum;
        dhtxx->HumD = (hum - dhtxx->HumI) * 10;
    }
}
