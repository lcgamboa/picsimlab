/* ########################################################################

   PICSimLab - Programmable IC Simulator Laboratory

   ########################################################################

   Copyright (c) : 2021-2026 Luis Claudio Gambôa Lopes <lcgamboa@yahoo.com>

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

#include <inttypes.h>
class board;

typedef struct {
    board* pboard;
    unsigned char ldata;
    unsigned char out;
    unsigned char addr[8];
    unsigned char addrc;
    unsigned char addrin[8];
    unsigned char scratchpad[9];
    uint32_t start;
    unsigned char datain;
    int state;
    int statebit;
    int TimerID;
} sen_ds18b20_t;

void sen_ds18b20_rst(sen_ds18b20_t* ds18b20);
void sen_ds18b20_init(sen_ds18b20_t* ds18b20, board* pboard);
void sen_ds18b20_end(sen_ds18b20_t* ds18b20);
void sen_ds18b20_setTemp(sen_ds18b20_t* ds18b20, const float temp);

unsigned char sen_ds18b20_io(sen_ds18b20_t* ds18b20, const unsigned char data);

uint64_t sen_ds18b20_get_addr(sen_ds18b20_t* ds18b20);

void sen_ds18b20_set_addr(sen_ds18b20_t* ds18b20, const uint64_t addr);
