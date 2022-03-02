/* ########################################################################

   PICsimLab - PIC laboratory simulator

   ########################################################################

   Copyright (c) : 2010-2020  Luis Claudio Gambôa Lopes

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

#include "io_MCP23X17.h"
#include <stdio.h>

#define dprintf \
    if (1) {    \
    } else      \
        printf

// IOCON bits
#define BANK 0x80
#define MIRROR 0x40
#define SEQOP 0x20
#define DISSLW 0x10
#define HAEN 0x08
#define ODR 0x04
#define INTPOL 0x02

static unsigned char decode_addr(io_MCP23X17_t* mcp) {
    if (mcp->regs[IOCON] & BANK) {
        switch (mcp->reg_addr) {
            case 0x00:
                return 0x00;
                break;
            case 0x01:
                return 0x02;
                break;
            case 0x02:
                return 0x04;
                break;
            case 0x03:
                return 0x06;
                break;
            case 0x04:
                return 0x08;
                break;
            case 0x05:
                return 0x0A;
                break;
            case 0x06:
                return 0x0C;
                break;
            case 0x07:
                return 0x0E;
                break;
            case 0x08:
                return 0x10;
                break;
            case 0x09:
                return 0x12;
                break;
            case 0x0A:
                return 0x14;
                break;
                // case 0x0B: return 0x10; break;
                // case 0x0C: return 0x10; break;
                // case 0x0D: return 0x10; break;
                // case 0x0E: return 0x10; break;
                // case 0x0F: return 0x10; break;
            case 0x10:
                return 0x01;
                break;
            case 0x11:
                return 0x03;
                break;
            case 0x12:
                return 0x05;
                break;
            case 0x13:
                return 0x07;
                break;
            case 0x14:
                return 0x09;
                break;
            case 0x15:
                return 0x0B;
                break;
            case 0x16:
                return 0x0D;
                break;
            case 0x17:
                return 0x0F;
                break;
            case 0x18:
                return 0x11;
                break;
            case 0x19:
                return 0x13;
                break;
            case 0x1A:
                return 0x15;
                break;
            default:
                return 0;
                dprintf("addr error!!!\n");
                break;
        }
    } else {
        return mcp->reg_addr;
    }
}

static void write_reg(io_MCP23X17_t* mcp, unsigned char val) {
    unsigned char addr_ = decode_addr(mcp);
    mcp->regs[addr_] = val;

    if ((mcp->regs[IOCON] & SEQOP) == 0) {
        mcp->reg_addr++;

        // FIXME only for BANK=0;
        if (mcp->reg_addr > 0x15)
            mcp->reg_addr = 0;
    }

    switch (addr_) {
        case IOCON:
            mcp->regs[IOCON_] = val;
            break;
        case IOCON_:
            mcp->regs[IOCON] = val;
            break;
        case GPIOA:
            mcp->regs[OLATA] = val;
            break;
        case GPIOB:
            mcp->regs[OLATB] = val;
            break;
    }
}

static unsigned char read_reg(io_MCP23X17_t* mcp) {
    unsigned char addr_ = decode_addr(mcp);
    return mcp->regs[addr_];
}

void io_MCP23X17_rst(io_MCP23X17_t* mcp) {
    for (int i = 0; i < 22; i++)
        mcp->regs[i] = 0;
    mcp->reg_addr = 0;

    bitbang_i2c_rst(&mcp->bb_i2c);
    bitbang_spi_rst(&mcp->bb_spi);

    mcp->op = 0;
    dprintf("mcp rst\n");
}

void io_MCP23X17_init(io_MCP23X17_t* mcp) {
    dprintf("mcp init\n");
    mcp->addr = 0x40;
    bitbang_i2c_init(&mcp->bb_i2c, mcp->addr >> 1);
    bitbang_spi_init(&mcp->bb_spi);
    io_MCP23X17_rst(mcp);
}

void io_MCP23X17_set_addr(io_MCP23X17_t* mcp, unsigned char addr) {
    mcp->addr = addr << 1;
    bitbang_i2c_set_addr(&mcp->bb_i2c, addr);
}

unsigned char io_MCP23X17_SPI_io(io_MCP23X17_t* mcp, unsigned char si, unsigned char sck, unsigned char rst,
                                 unsigned char cs) {
    if (!rst) {
        io_MCP23X17_rst(mcp);
        return 0;
    }

    unsigned char ret = bitbang_spi_io(&mcp->bb_spi, sck, si, cs);

    switch (bitbang_spi_get_status(&mcp->bb_spi)) {
        case SPI_DATA:

            switch (mcp->bb_spi.byte) {
                case 1:
                    if ((mcp->bb_spi.data8 & 0xFE) == mcp->addr) {
                        mcp->op = mcp->bb_spi.data8 & 0x01;
                        dprintf("mcp addr 0x%02X OK\n", mcp->addr);
                    } else {
                        mcp->bb_spi.bit = 0x80;

                        dprintf("mcp addr 0x%02X ERROR\n", mcp->addr);
                    }
                    break;
                case 2:
                    mcp->reg_addr = mcp->bb_spi.data8;
                    dprintf("mcp reg addr 0x%02X\n", mcp->bb_spi.data8);
                    break;
                default:
                    if (mcp->op) {
                        dprintf("mcp data read [0x%02X] 0x%02X\n", mcp->reg_addr, mcp->bb_spi.data8);
                        bitbang_spi_send(&mcp->bb_spi, read_reg(mcp));
                    } else {
                        dprintf("mcp data write [0x%02X] 0x%02X\n", mcp->reg_addr, mcp->bb_spi.data8);
                        write_reg(mcp, mcp->bb_spi.data8);
                    }
                    break;
            }
            break;
    }

    return ret;
}

unsigned char io_MCP23X17_I2C_io(io_MCP23X17_t* mcp, unsigned char scl, unsigned char sda) {
    /*
     NOT tested !!!!

     unsigned char ret = bitbang_i2c_io (&mcp->bb_i2c, scl, sda);

     switch (bitbang_i2c_get_status (&mcp->C))
      {
      case I2C_DATAW:
       dprintf ("write mcp =%02X\n", mcp->bb_i2c.datar);
        switch (mcp->bb_i2c.byte)
        {
        case 1:
         mcp->reg_addr = mcp->bb_i2c.datar;
         dprintf ("mcp reg addr 0x%02X\n", mcp->bb_i2c.datar);
         break;
        default:
         if (mcp->op)
          {
           dprintf ("mcp data read [0x%02X] 0x%02X\n", mcp->reg_addr, mcp->bb_spi.data);
           bitbang_spi_send (&mcp->bb_spi, read_reg (mcp));
          }
         else
          {
           dprintf ("mcp data write [0x%02X] 0x%02X\n", mcp->reg_addr, mcp->bb_spi.data);
           write_reg (mcp, mcp->bb_spi.data);
          }
         break;
        }
       break;
      case I2C_DATAR:
       bitbang_i2c_send (&mcp->bb_i2c, read_reg (mcp));
       dprintf ("mcp read =%02X\n", mcp->bb_i2c.datas);
       break;
      }

     return ret;
     */
    return 0;
}