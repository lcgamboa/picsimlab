/* ########################################################################

   PICsimLab - PIC laboratory simulator

   ########################################################################

   Copyright (c) : 2020-2023  Luis Claudio Gambôa Lopes

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

#include "bitbang_i2c.h"
#include "../boards/board.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ACK 0
#define NACK 1

#define dprintf \
    if (1) {    \
    } else      \
        printf

void bitbang_i2c_set_addr(bitbang_i2c_t* i2c, const unsigned char addr) {
    // dprintf ("bitbang_i2c %2x set_addr 0x%02X \n",i2c->addr>>1, addr);
    i2c->addr = addr << 1;
}

void bitbang_i2c_rst(bitbang_i2c_t* i2c) {
    i2c->sdao = 0;
    i2c->sclo = 1;
    i2c->bit = 0xFF;
    i2c->byte = 0xFF;
    i2c->datab = 0;
    i2c->ret = 0;
    i2c->status = 0;
    i2c->data_reading = 0;
    i2c->ctrl_on = 0;
    dprintf("bitbang_i2c %2x rst\n", i2c->addr >> 1);
}

void bitbang_i2c_init(bitbang_i2c_t* i2c, const unsigned char addr, const unsigned char addr_mask) {
    dprintf("bitbang_i2c %2x init\n", addr);
    i2c->addr = addr << 1;
    i2c->addr_mask = addr_mask;
    bitbang_i2c_rst(i2c);
}

unsigned char bitbang_i2c_io(bitbang_i2c_t* i2c, const unsigned char scl, const unsigned char sda) {
    if ((i2c->sdao == sda) && (i2c->sclo == scl)) {
        // No edge, return the last value
        // dprintf ("bitbang_i2c %2x nret = %i\n", i2c->addr >> 1, i2c->ret);
        return i2c->ret;
    }

    if ((i2c->sdao == 1) && (sda == 0) && (scl == 1) && (i2c->sclo == 1)) {
        // start
        i2c->bit = 0;
        i2c->byte = 0;
        i2c->datab = 0;
        i2c->ret = 0;
        i2c->data_reading = 0;
        dprintf("---->i2c start %02x!\n", i2c->addr >> 1);
        i2c->status = I2C_START;
    }

    if ((i2c->sdao == 0) && (sda == 1) && (scl == 1) && (i2c->sclo == 1)) {
        // stop
        i2c->bit = 0xFF;
        i2c->byte = 0xFF;
        i2c->ret = 0;
        i2c->data_reading = 0;
        dprintf("---> i2c stop %02x!\n", i2c->addr >> 1);
        i2c->status = I2C_STOP;
    }

    if ((i2c->bit < 9) && (i2c->sclo == 0) && (scl == 1)) {
        // data in
        if (i2c->bit < 8) {
            i2c->datab |= (sda << (7 - i2c->bit));
        }
        // dprintf ("recv %02x %i %i (%02X)\n", i2c->addr >> 1, i2c->bit, i2c->ret, i2c->datab);
        i2c->bit++;
    }

    if ((i2c->bit < 9) && (i2c->sclo == 1) && (scl == 0) && i2c->data_reading) {
        // data out
        if (i2c->bit < 8) {
            i2c->ret = ((i2c->datas & (1 << (7 - i2c->bit))) > 0);
            // dprintf ("send %02x %i %i (%02X)\n", i2c->addr >> 1, i2c->bit, i2c->ret, i2c->datas);
        } else {
            i2c->ret = ACK;
        }
    }

    if (i2c->bit == 8) {
        if (i2c->byte == 0)  // ADDR
        {
            if ((i2c->datab & i2c->addr_mask) == i2c->addr) {
                // valid address
                i2c->ret = ACK;
            } else {
                // invalid address
                i2c->ret = NACK;
            }
        } else if (!i2c->data_reading) {
            // data
            i2c->ret = ACK;
        }
    }

    if (i2c->bit == 9) {
        dprintf("bitbang_i2c %02x data %02X\n", i2c->addr >> 1, i2c->datab);

        if (i2c->byte == 0)  // ADDR
        {
            if ((i2c->datab & i2c->addr_mask) == i2c->addr)  // valid address
            {
                dprintf("bitbang_i2c %2x addr OK %02X\n", i2c->addr >> 1, (i2c->datab & i2c->addr_mask) >> 1);

                if (i2c->datab & 0x01) {
                    i2c->data_reading = 1;
                    dprintf("bitbang_i2c %02x READ \n", i2c->addr >> 1);
                    i2c->status = I2C_DATAR;
                } else {
                    i2c->status = I2C_ADDR;
                }
                i2c->datar = i2c->datab;
                i2c->bit = 0;
                i2c->datab = 0;
                i2c->byte++;
                // i2c->ret = 0;
            } else  // invalid address
            {
                dprintf("bitbang_i2c %2x addr NOK %02X\n", i2c->addr >> 1, (i2c->datab & 0xFE) >> 1);
                i2c->bit = 0xFF;
                i2c->byte = 0xFF;
            }
        } else if (!i2c->data_reading)  // data
        {
            dprintf("bitbang_i2c %2x received OK %02X\n", i2c->addr >> 1, i2c->datab);
            i2c->datar = i2c->datab;
            i2c->bit = 0;
            i2c->datab = 0;
            i2c->byte++;
            i2c->status = I2C_DATAW;
        } else if (i2c->data_reading) {
            i2c->datar = i2c->datab;
            i2c->bit = 0;
            i2c->datab = 0;
            i2c->byte++;
            i2c->status = I2C_DATAR;
            dprintf("bitbang_i2c %2x need data to send\n", i2c->addr >> 1);
        }
    }

    i2c->sdao = sda;
    i2c->sclo = scl;
    // dprintf ("bitbang_i2c %2x ret = %i\n", i2c->addr >> 1, i2c->ret);
    return i2c->ret;
}

unsigned char bitbang_i2c_get_status(bitbang_i2c_t* i2c) {
    unsigned char status = i2c->status;
    i2c->status = 0;
    return status;
}

void bitbang_i2c_send(bitbang_i2c_t* i2c, const unsigned char data) {
    i2c->datas = data;
    dprintf("bitbang_i2c %2x data to send 0x%02x \n", i2c->addr >> 1, data);
}

// Controller

static void bitbang_i2c_ctrl_callback(void* arg) {
    bitbang_i2c_t* i2c = (bitbang_i2c_t*)arg;

    switch (i2c->status) {
        case I2C_START:
            if (i2c->clkpc == 0) {
                ioupdated = 1;
                i2c->sda_dir = PD_OUT;
                i2c->sda_value = 0;
                i2c->scl_value = 1;
            } else {
                i2c->status = I2C_IDLE;
                i2c->pboard->TimerSetState(i2c->TimerID, 0);
            }
            i2c->clkpc++;
            break;
        case I2C_STOP:
            if (i2c->clkpc == 0) {
                ioupdated = 1;
                i2c->sda_dir = PD_OUT;
                i2c->sda_value = 1;
                i2c->scl_value = 1;
            } else {
                i2c->status = I2C_IDLE;
                i2c->pboard->TimerSetState(i2c->TimerID, 0);
            }
            i2c->clkpc++;
            break;
        case I2C_DATAW:
            if (i2c->bit < 8) {
                switch (i2c->clkpc) {
                    case 0:
                        ioupdated = 1;
                        i2c->scl_value = 0;
                        break;
                    case 1:
                        i2c->scl_value = 0;
                        i2c->sda_value = (i2c->datab & (0x01 << (7 - i2c->bit))) > 0;
                        break;
                    case 2:
                        ioupdated = 1;
                        i2c->scl_value = 1;
                        break;
                    case 3:
                        i2c->scl_value = 1;
                        i2c->clkpc = -1;
                        i2c->bit++;
                        break;
                }
            } else {  // read ACK
                switch (i2c->clkpc) {
                    case 0:
                        ioupdated = 1;
                        i2c->scl_value = 0;
                        if (i2c->bit > 8) {
                            i2c->sda_dir = PD_OUT;
                            i2c->status = I2C_IDLE;
                            i2c->pboard->TimerSetState(i2c->TimerID, 0);
                        } else {
                            i2c->sda_dir = PD_IN;
                        }
                        break;
                    case 1:
                        i2c->scl_value = 0;
                        break;
                    case 2:
                        ioupdated = 1;
                        i2c->scl_value = 1;
                        i2c->ack = i2c->sda_value;  // FIXME verify ack
                        break;
                    case 3:
                        i2c->scl_value = 1;
                        i2c->clkpc = -1;
                        i2c->bit++;
                        break;
                }
            }
            i2c->clkpc++;
            break;
        case I2C_DATAR:
            if (i2c->bit < 8) {
                switch (i2c->clkpc) {
                    case 0:
                        ioupdated = 1;
                        i2c->scl_value = 0;
                        break;
                    case 1:
                        i2c->scl_value = 0;
                        break;
                    case 2:
                        ioupdated = 1;
                        i2c->scl_value = 1;
                        break;
                    case 3:
                        i2c->datar |= (i2c->sda_value << (7 - i2c->bit));
                        i2c->scl_value = 1;
                        i2c->clkpc = -1;
                        i2c->bit++;
                        break;
                }
            } else {  // read ACK
                switch (i2c->clkpc) {
                    case 0:
                        ioupdated = 1;
                        i2c->scl_value = 0;
                        if (i2c->bit > 8) {
                            i2c->sda_dir = PD_OUT;
                            i2c->status = I2C_IDLE;
                            i2c->pboard->TimerSetState(i2c->TimerID, 0);
                        } else {
                            i2c->sda_dir = PD_OUT;
                        }
                        break;
                    case 1:
                        i2c->scl_value = 0;
                        i2c->sda_value = i2c->ack;  // FIXME verify ack
                        break;
                    case 2:
                        ioupdated = 1;
                        i2c->scl_value = 1;
                        break;
                    case 3:
                        i2c->scl_value = 1;
                        i2c->clkpc = -1;
                        i2c->bit++;
                        break;
                }
            }
            i2c->clkpc++;
            break;
        default:
            break;
    }
}

void bitbang_i2c_ctrl_init(bitbang_i2c_t* i2c, board* pboard) {
    i2c->pboard = pboard;
    // only 100KHz speed
    i2c->TimerID = i2c->pboard->TimerRegister_us(2, bitbang_i2c_ctrl_callback, i2c);
    i2c->pboard->TimerSetState(i2c->TimerID, 0);
    bitbang_i2c_rst(i2c);
}

void bitbang_i2c_ctrl_end(bitbang_i2c_t* i2c) {
    i2c->pboard->TimerUnregister(i2c->TimerID);
}

void bitbang_i2c_ctrl_start(bitbang_i2c_t* i2c) {
    ioupdated = 1;
    i2c->sda_dir = PD_OUT;
    i2c->sda_value = 1;
    i2c->scl_value = 1;
    i2c->clkpc = 0;
    i2c->status = I2C_START;
    i2c->pboard->TimerChange_us(i2c->TimerID, 2);  // FIXME only 100kHz frequency
    i2c->pboard->TimerSetState(i2c->TimerID, 1);
}

void bitbang_i2c_ctrl_restart(bitbang_i2c_t* i2c) {
    bitbang_i2c_ctrl_start(i2c);
}

void bitbang_i2c_ctrl_stop(bitbang_i2c_t* i2c) {
    ioupdated = 1;
    i2c->sda_dir = PD_OUT;
    i2c->sda_value = 0;
    i2c->scl_value = 1;
    i2c->clkpc = 0;
    i2c->status = I2C_STOP;
    i2c->pboard->TimerSetState(i2c->TimerID, 1);
}

void bitbang_i2c_ctrl_write(bitbang_i2c_t* i2c, const unsigned char data) {
    ioupdated = 1;
    i2c->sda_dir = PD_OUT;
    i2c->status = I2C_DATAW;
    i2c->bit = 0;
    i2c->clkpc = 1;
    i2c->datab = data;
    i2c->scl_value = 0;
    i2c->pboard->TimerSetState(i2c->TimerID, 1);
}

void bitbang_i2c_ctrl_read(bitbang_i2c_t* i2c) {
    ioupdated = 1;
    i2c->sda_dir = PD_IN;
    i2c->status = I2C_DATAR;
    i2c->bit = 0;
    i2c->clkpc = 1;
    i2c->datar = 0;
    i2c->scl_value = 0;
    i2c->pboard->TimerSetState(i2c->TimerID, 1);
}
