/* ########################################################################

   PICsimLab - PIC laboratory simulator

   ########################################################################

   Copyright (c) : 2021-2022  Luis Claudio Gambôa Lopes

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

#include "sen_ds18b20.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define dprintf \
    if (1) {    \
    } else      \
        printf

#define CMD_Search_Rom 0xF0
#define CMD_Read_Rom 0x33
#define CMD_Match_Rom 0x55
#define CMD_Skip_Rom 0xCC
#define CMD_Alarm_Search 0xEC

#define CMD_Convert_T 0x44
#define CMD_Write_Scratchpad 0x4E
#define CMD_Read_Scratchpad 0xBE
#define CMD_Copy_Scratchpad 0x48
#define CMD_Recall_E2 0xB8
#define CMD_Read_Power_Supply 0xB4

enum { OW_IDLE, OW_RESET, OW_PRESENCE, OW_CMD, OW_MROM, OW_RSPAD, OW_RROM, OW_SROM, OW_UNKNOWN };

const char* OWstates[]{"OW_IDLE",  "OW_RESET", "OW_PRESENCE", "OW_CMD",    "OW_MROM",
                       "OW_RSPAD", "OW_RROM",  "OW_SROM",     "OW_UNKNOWN"};

static void calc_crc(unsigned char* spad, int size) {
    unsigned char crc = 0;

    for (int i = 0; i < size; i++) {
        unsigned char val = spad[i];
        for (int bit = 8; bit; bit--) {
            unsigned char mult = (crc ^ val) & 0x01;
            crc >>= 1;
            if (mult) {
                crc ^= 0x8C;
            }
            val >>= 1;
        }
    }
    spad[size] = crc;
}

void sen_ds18b20_rst(sen_ds18b20_t* ds18b20) {
    dprintf("ds18b20 rst\n");
    ds18b20->ldata = 0;
    ds18b20->start = 0;
    ds18b20->out = 1;
    ds18b20->state = OW_IDLE;
    ds18b20->pboard->TimerSetState(ds18b20->TimerID, 0);

    ds18b20->scratchpad[0] = 0xC0;
    ds18b20->scratchpad[1] = 0x01;
    ds18b20->scratchpad[2] = 0x50;
    ds18b20->scratchpad[3] = 0x05;
    ds18b20->scratchpad[4] = 0x7F;
    ds18b20->scratchpad[5] = 0xFF;
    ds18b20->scratchpad[6] = 0x00;
    ds18b20->scratchpad[7] = 0x10;
    calc_crc(ds18b20->scratchpad, 8);
}

static void sen_ds18b20_callback(void* arg) {
    sen_ds18b20_t* ds18b20 = (sen_ds18b20_t*)arg;

    dprintf("ds18b20 callback %i (%s) bit %i ldat 0x%02x\n", ds18b20->state, OWstates[ds18b20->state],
            ds18b20->statebit, ds18b20->ldata);

    switch (ds18b20->state) {
        case OW_PRESENCE:
            switch (ds18b20->statebit) {
                case 0:
                    ds18b20->out = 0;  // odd values are logic one
                    ds18b20->pboard->TimerChange_us(ds18b20->TimerID, 200);
                    ioupdated = 1;
                    ds18b20->statebit++;
                    break;
                case 1:
                    ds18b20->out = 1;  // odd values are logic one
                    ds18b20->pboard->TimerChange_us(ds18b20->TimerID, 200);
                    ioupdated = 1;
                    ds18b20->statebit++;
                    break;
                case 2:
                    ds18b20->out = 1;
                    ioupdated = 1;
                    ds18b20->pboard->TimerSetState(ds18b20->TimerID, 0);
                    ds18b20->state = OW_CMD;
                    ds18b20->statebit = 0;
                    ds18b20->datain = 0;
                    break;
            }
            break;
        case OW_CMD:

            ds18b20->datain |= (ds18b20->ldata << (ds18b20->statebit));

            ds18b20->pboard->TimerSetState(ds18b20->TimerID, 0);
            ds18b20->statebit++;

            if (ds18b20->statebit == 8) {
                ds18b20->state = OW_IDLE;
                ds18b20->out = 1;
                ds18b20->statebit = 0;
                dprintf("ds18b20 cmd = 0x%02X\n", ds18b20->datain);
                switch (ds18b20->datain) {
                    case CMD_Search_Rom:
                        dprintf("ds18b20 cmd Search Rom\n");
                        ds18b20->state = OW_SROM;
                        ds18b20->statebit = 0;
                        ds18b20->addrc = 0;
                        ds18b20->start = 0;
                        break;
                    case CMD_Read_Rom:
                        dprintf("ds18b20 cmd Read Rom\n");
                        ds18b20->state = OW_RROM;
                        ds18b20->statebit = 0;
                        ds18b20->addrc = 0;
                        ds18b20->start = 1;
                        break;
                    case CMD_Match_Rom:
                        dprintf("ds18b20 cmd Match Rom\n");
                        ds18b20->state = OW_MROM;
                        ds18b20->statebit = 0;
                        ds18b20->datain = 0;
                        ds18b20->addrc = 0;
                        break;
                    case CMD_Skip_Rom:
                        dprintf("ds18b20 cmd Skip Rom\n");
                        ds18b20->state = OW_CMD;
                        ds18b20->statebit = 0;
                        break;
                    case CMD_Alarm_Search:
                        dprintf("ds18b20 cmd Alarm Search\n");
                        break;
                    case CMD_Convert_T:
                        dprintf("ds18b20 cmd Convert T\n");
                        break;
                    case CMD_Write_Scratchpad:
                        dprintf("ds18b20 cmd Write Scratchpad\n");
                        break;
                    case CMD_Read_Scratchpad:
                        dprintf("ds18b20 cmd Read Scratchpad\n");
                        ds18b20->state = OW_RSPAD;
                        ds18b20->statebit = 0;
                        ds18b20->addrc = 0;
                        ds18b20->start = 1;
                        break;
                    case CMD_Copy_Scratchpad:
                        dprintf("ds18b20 cmd Copy Scratchpad\n");
                        break;
                    case CMD_Recall_E2:
                        dprintf("ds18b20 cmd Recall E2\n");
                        break;
                    case CMD_Read_Power_Supply:
                        dprintf("ds18b20 cmd Read Power Supply\n");
                        break;
                    default:
                        break;
                }
            }
            break;
        case OW_MROM:

            ds18b20->datain |= (ds18b20->ldata << (ds18b20->statebit & 0x07));

            ds18b20->pboard->TimerSetState(ds18b20->TimerID, 0);

            ds18b20->statebit++;
            if (!(ds18b20->statebit & 0x07)) {
                dprintf("ds18b20 ROM[%i] = 0x%02x\n", ds18b20->addrc, ds18b20->datain);
                ds18b20->addrin[ds18b20->addrc++] = ds18b20->datain;
                ds18b20->datain = 0;
            }

            if (ds18b20->statebit == 64) {
                dprintf("ds18b20 ROM = 0x%02X%02X%02X%02X%02X%02X%02X%02X\n", ds18b20->addrin[0], ds18b20->addrin[1],
                        ds18b20->addrin[2], ds18b20->addrin[3], ds18b20->addrin[4], ds18b20->addrin[5],
                        ds18b20->addrin[6], ds18b20->addrin[7]);

                for (int i = 0; i < 8; i++) {
                    if (ds18b20->addrin[i] != ds18b20->addr[i]) {
                        dprintf("ds18b20 ROM Not Match!!!!\n");
                        ds18b20->state = OW_IDLE;
                        ds18b20->out = 1;
                        ds18b20->pboard->TimerSetState(ds18b20->TimerID, 0);
                        return;
                    }
                }
                dprintf("ds18b20 ROM Match\n");
                ds18b20->state = OW_CMD;
                ds18b20->statebit = 0;
            }
            break;
        case OW_RSPAD:
            if (ds18b20->start) {
                ds18b20->start = 0;
                ds18b20->out = (ds18b20->scratchpad[ds18b20->addrc] & (1 << (ds18b20->statebit & 0x07))) > 0;
                ioupdated = 1;
                ds18b20->pboard->TimerChange_us(ds18b20->TimerID, 15);

                if (!((ds18b20->statebit + 1) & 0x07)) {
                    ds18b20->addrc++;
                }

            } else {
                ds18b20->start = 1;
                if (!ds18b20->out) {
                    ds18b20->out = 1;
                    ioupdated = 1;
                }

                ds18b20->pboard->TimerSetState(ds18b20->TimerID, 0);
                ds18b20->statebit++;
                if (ds18b20->addrc == 9) {
                    dprintf("ds18b20 SPAD read ok!");
                    ds18b20->state = OW_IDLE;
                    ds18b20->statebit = 0;
                }
            }
            break;
        case OW_SROM:
            switch (ds18b20->start) {
                case 0:
                    ds18b20->out = (ds18b20->addr[ds18b20->addrc] & (1 << (ds18b20->statebit & 0x07))) > 0;
                    ioupdated = 1;
                    ds18b20->pboard->TimerChange_us(ds18b20->TimerID, 15);

                    break;
                case 1:
                case 3:
                    if (!ds18b20->out) {
                        ds18b20->out = 1;
                        ioupdated = 1;
                    }
                    ds18b20->pboard->TimerSetState(ds18b20->TimerID, 0);
                    break;
                case 2:
                    ds18b20->out = (ds18b20->addr[ds18b20->addrc] & (1 << (ds18b20->statebit & 0x07))) == 0;
                    ioupdated = 1;
                    ds18b20->pboard->TimerChange_us(ds18b20->TimerID, 15);

                    break;
                case 4:
                    ds18b20->pboard->TimerSetState(ds18b20->TimerID, 0);
                    if (ds18b20->ldata != ((ds18b20->addr[ds18b20->addrc] & (1 << (ds18b20->statebit & 0x07))) > 0)) {
                        ds18b20->state = OW_IDLE;
                        ds18b20->statebit = 0;
                        return;
                    }

                    ds18b20->statebit++;
                    if (!((ds18b20->statebit) & 0x07)) {
                        ds18b20->addrc++;
                    }
                    if (ds18b20->addrc == 8) {
                        dprintf("ds18b20 ROM search ok!");
                        ds18b20->state = OW_IDLE;
                    }
                    break;
            }
            ds18b20->start++;
            if (ds18b20->start > 4)
                ds18b20->start = 0;
            break;
        case OW_RROM:
            if (ds18b20->start) {
                ds18b20->start = 0;
                ds18b20->out = (ds18b20->addr[ds18b20->addrc] & (1 << (ds18b20->statebit & 0x07))) > 0;
                ioupdated = 1;
                ds18b20->pboard->TimerChange_us(ds18b20->TimerID, 15);

                if (!((ds18b20->statebit + 1) & 0x07)) {
                    ds18b20->addrc++;
                }

            } else {
                ds18b20->start = 1;
                if (!ds18b20->out) {
                    ds18b20->out = 1;
                    ioupdated = 1;
                }

                ds18b20->pboard->TimerSetState(ds18b20->TimerID, 0);
                ds18b20->statebit++;
                if (ds18b20->addrc == 8) {
                    dprintf("ds18b20 ROM read ok!");
                    ds18b20->state = OW_IDLE;
                }
            }
            break;
        default:
            ds18b20->state = OW_IDLE;
            ds18b20->out = 1;
            ds18b20->pboard->TimerSetState(ds18b20->TimerID, 0);
            break;
    }
}

void sen_ds18b20_init(sen_ds18b20_t* ds18b20, board* pboard) {
    dprintf("ds18b20 init\n");
    srand(time(NULL));
    ds18b20->pboard = pboard;
    ds18b20->TimerID = ds18b20->pboard->TimerRegister_ms(1, sen_ds18b20_callback, ds18b20);
    ds18b20->pboard->TimerSetState(ds18b20->TimerID, 0);
    ds18b20->addr[0] = 0x28;  // device type
    ds18b20->addr[1] = rand();
    ds18b20->addr[2] = rand();
    ds18b20->addr[3] = rand();
    ds18b20->addr[4] = rand();
    ds18b20->addr[5] = rand();
    ds18b20->addr[6] = rand();
    calc_crc(ds18b20->addr, 7);
    sen_ds18b20_rst(ds18b20);
}

void sen_ds18b20_end(sen_ds18b20_t* ds18b20) {
    dprintf("ds18b20 end\n");
    ds18b20->pboard->TimerUnregister(ds18b20->TimerID);
}

unsigned char sen_ds18b20_io(sen_ds18b20_t* ds18b20, const unsigned char data) {
    // dprintf("data = %i ldata =%i state = %i \n", data, ds18b20->ldata, ds18b20->state);

    if (!data && ds18b20->ldata) {  // falling edge
        switch (ds18b20->state) {
            case OW_IDLE:
                dprintf("ds18b20 reset\n");
                ds18b20->start = ds18b20->pboard->InstCounterGet();
                ds18b20->state = OW_RESET;
                ds18b20->out = 1;
                break;
            case OW_CMD:
                dprintf("ds18b20 CMD data edge %i\n", ds18b20->statebit);
                ds18b20->pboard->TimerChange_us(ds18b20->TimerID, 30);  // sample time
                ds18b20->pboard->TimerSetState(ds18b20->TimerID, 1);
                break;
            case OW_MROM:
                dprintf("ds18b20 MROM data edge %i\n", ds18b20->statebit);
                ds18b20->pboard->TimerChange_us(ds18b20->TimerID, 30);  // sample time
                ds18b20->pboard->TimerSetState(ds18b20->TimerID, 1);
                break;
            case OW_RSPAD:
            case OW_RROM:
                dprintf("ds18b20 RSPAD data edge %i\n", ds18b20->statebit);
                ds18b20->pboard->TimerChange_us(ds18b20->TimerID, 10);  // sample time
                ds18b20->pboard->TimerSetState(ds18b20->TimerID, 1);
                ds18b20->out = 0;
                break;
            case OW_SROM:
                dprintf("ds18b20 RSPAD data edge %i\n", ds18b20->statebit);
                ds18b20->pboard->TimerChange_us(ds18b20->TimerID, (ds18b20->start == 4) ? 30 : 10);  // sample time
                ds18b20->pboard->TimerSetState(ds18b20->TimerID, 1);
                ds18b20->out = 0;
                break;
            default:
                break;
        }

    } else if (data && !ds18b20->ldata) {  // rising edge
        switch (ds18b20->state) {
            case OW_RESET: {
                int pulse = ds18b20->pboard->InstCounterGet_us(ds18b20->start);
                if ((pulse > 450) && (pulse < 10000)) {  // valid start
                    ds18b20->state = OW_PRESENCE;
                    ds18b20->statebit = 0;

                    dprintf("ds18b20 presence\n");
                    ds18b20->out = 1;
                    ds18b20->pboard->TimerChange_us(ds18b20->TimerID, 30);
                    ds18b20->pboard->TimerSetState(ds18b20->TimerID, 1);

                } else {
                    ds18b20->state = OW_IDLE;
                    ds18b20->out = 1;
                    ds18b20->pboard->TimerSetState(ds18b20->TimerID, 0);
                }
            } break;
            default:
                break;
        }
    }
    ds18b20->ldata = (data & 0x01);
    return ds18b20->out;
}

void sen_ds18b20_setTemp(sen_ds18b20_t* ds18b20, const float temp) {
    unsigned short itemp = temp * 16;

    ds18b20->scratchpad[0] = (itemp & 0x00FF);
    ds18b20->scratchpad[1] = (itemp & 0xFF00) >> 8;
    calc_crc(ds18b20->scratchpad, 8);
}

uint64_t sen_ds18b20_get_addr(sen_ds18b20_t* ds18b20) {
    uint64_t* paddr = (uint64_t*)ds18b20->addr;
    return *paddr;
}

void sen_ds18b20_set_addr(sen_ds18b20_t* ds18b20, const uint64_t addr) {
    uint64_t* paddr = (uint64_t*)ds18b20->addr;
    *paddr = addr;
}
