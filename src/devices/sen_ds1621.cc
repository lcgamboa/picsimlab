/* ########################################################################

   PICsimLab - PIC laboratory simulator

   ########################################################################

   Copyright (c) : 2010-2021  Luis Claudio Gambôa Lopes

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

#include "sen_ds1621.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define dprintf \
    if (1) {    \
    } else      \
        printf

#define CMD_ReadTempe 0xAA
#define CMD_ReadCount 0xA8
#define CMD_ReadSlope 0xA9
#define CMD_StartConv 0xEE
#define CMD_StopConv 0x22
#define CMD_AccessTH 0xA1
#define CMD_AccessTL 0xA2
#define CMD_AccessCTR 0xAC

void sen_ds1621_rst(sen_ds1621_t* ds1621) {
    bitbang_i2c_rst(&ds1621->bb_i2c);
    ds1621->cmd = 0;
    ds1621->CTR = 0;
    ds1621->TH = 0;
    ds1621->TL = 0;
    dprintf("ds1621 rst\n");
}

void sen_ds1621_init(sen_ds1621_t* ds1621) {
    dprintf("ds1621 init\n");
    bitbang_i2c_init(&ds1621->bb_i2c, 0x48);
    sen_ds1621_rst(ds1621);
    ds1621->Temp = 0;
}

void sen_ds1621_end(sen_ds1621_t* ds1621) {
    dprintf("ds1621 end\n");
}

unsigned char sen_ds1621_I2C_io(sen_ds1621_t* ds1621, unsigned char scl, unsigned char sda) {
    unsigned char ret = bitbang_i2c_io(&ds1621->bb_i2c, scl, sda);

    switch (bitbang_i2c_get_status(&ds1621->bb_i2c)) {
        case I2C_DATAW:
            dprintf("data %02X  byte=%i\n", ds1621->bb_i2c.datar, ds1621->bb_i2c.byte);
            if (ds1621->bb_i2c.byte == 2) {
                dprintf("==>cmd %02X\n", ds1621->bb_i2c.datar);
                ds1621->cmd = ds1621->bb_i2c.datar;
                /*
                switch (ds1621->cmd)
                 {
                 case CMD_ReadTempe:
                  break;
                 case CMD_ReadCount:
                  break;
                 case CMD_ReadSlope:
                  break;
                 case CMD_StartConv:
                  break;
                 case CMD_StopConv:
                  break;
                 case CMD_AccessTH:
                  break;
                 case CMD_AccessTL:
                  break;
                 case CMD_AccessCTR:
                  break;
                 default:
                  break;
                 }
                 */
            } else if (ds1621->bb_i2c.byte == 3) {
                switch (ds1621->cmd) {
                    case CMD_AccessTH:
                        ds1621->TH = ds1621->bb_i2c.datar;
                        break;
                    case CMD_AccessTL:
                        ds1621->TL = ds1621->bb_i2c.datar;
                        break;
                    case CMD_AccessCTR:
                        ds1621->CTR = ds1621->bb_i2c.datar;
                        break;
                    default:
                        break;
                }
            }
            break;
        case I2C_DATAR:
            dprintf("read cmd %02X   byte=%i\n", ds1621->cmd, ds1621->bb_i2c.byte);
            if (ds1621->bb_i2c.byte == 1) {
                switch (ds1621->cmd) {
                    case CMD_ReadTempe:
                        bitbang_i2c_send(&ds1621->bb_i2c, (ds1621->Temp & 0xFF00) >> 8);
                        break;
                    case CMD_AccessTH:
                        bitbang_i2c_send(&ds1621->bb_i2c, ds1621->TH);
                        break;
                    case CMD_AccessTL:
                        bitbang_i2c_send(&ds1621->bb_i2c, ds1621->TL);
                        break;
                    case CMD_AccessCTR:
                        bitbang_i2c_send(&ds1621->bb_i2c, ds1621->CTR);
                        break;
                        // case CMD_ReadCount:
                        // case CMD_ReadSlope:
                    default:
                        bitbang_i2c_send(&ds1621->bb_i2c, 0xFF);
                        break;
                }
            } else if (ds1621->bb_i2c.byte == 2) {
                switch (ds1621->cmd) {
                    case CMD_ReadTempe:
                        bitbang_i2c_send(&ds1621->bb_i2c, ds1621->Temp & 0x00FF);
                        break;
                    default:
                        bitbang_i2c_send(&ds1621->bb_i2c, 0xFF);
                        break;
                }
            } else {
                bitbang_i2c_send(&ds1621->bb_i2c, 0xFF);
            }
            break;
    }

    return ret;
}

void sen_ds1621_setTemp(sen_ds1621_t* ds1621, float temp) {
    ds1621->Temp = (temp * 2);
    ds1621->Temp = ds1621->Temp << 7;
}

void sen_ds1621_set_addr(sen_ds1621_t* ds1621, unsigned char addr) {
    dprintf("set addr to %02X \n", addr);
    bitbang_i2c_set_addr(&ds1621->bb_i2c, addr);
}