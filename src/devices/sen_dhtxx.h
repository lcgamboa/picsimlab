/* ########################################################################

   PICSimLab - Programmable IC Simulator Laboratory

   ########################################################################

   Copyright (c) : 2010-2024  Luis Claudio Gambôa Lopes <lcgamboa@yahoo.com>

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
    unsigned char TempI;
    unsigned char TempD;
    unsigned char HumI;
    unsigned char HumD;
    unsigned char ldata;
    unsigned char out;
    uint32_t start;
    int state;
    int uvalues[84];
    int TimerID;
    int type;
} sen_dhtxx_t;

#define DHT11 0
#define DHT22 1

void sen_dhtxx_rst(sen_dhtxx_t* dhtxx);
void sen_dhtxx_init(sen_dhtxx_t* dhtxx, board* pboard, int type);
void sen_dhtxx_end(sen_dhtxx_t* dhtxx);
void sen_dhtxx_setTempHum(sen_dhtxx_t* dhtxx, const float temp, const float hum);

unsigned char sen_dhtxx_io(sen_dhtxx_t* dhtxx, const unsigned char data);
