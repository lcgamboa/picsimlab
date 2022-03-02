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

#ifndef IO_MCP23X17
#define IO_MCP23X17

#include "bitbang_i2c.h"
#include "bitbang_spi.h"

/*
 pinout
1  GPB0
2  GPB1
3  GPB2
4  GPB3
5  GPB4
6  GPB5
7  GPB6
8  GPB7
9  VDD
10 VSS
11 NC  - /CS
12 SCK - SCK
12 SDA - SI
14 NC  - SO

15 A0
16 A1
17 A2
18 /RESET
19 INTB
20 INTA
21 GPA0
22 GPA1
23 GPA2
24 GPA3
25 GPA4
26 GPA5
27 GPA6
28 GPA7
 */

// BANK 0 addr
#define IODIRA 0x00
#define IODIRB 0x01
#define IPOLA 0x02
#define IPOLB 0x03
#define GPINTENA 0x04
#define GPINTENB 0x05
#define DEFVALA 0x06
#define DEFVALB 0x07
#define INTCONA 0x08
#define INTCONB 0x09
#define IOCON 0x0A
#define IOCON_ 0x0B
#define GPPUA 0x0C
#define GPPUB 0x0D
#define INTFA 0x0E
#define INTFB 0x0F
#define INTCAPA 0x10
#define INTCAPB 0x11
#define GPIOA 0x12
#define GPIOB 0x13
#define OLATA 0x14
#define OLATB 0x15

typedef struct {
    unsigned char regs[22];
    unsigned char reg_addr;
    unsigned char op;
    unsigned char addr;
    bitbang_spi_t bb_spi;
    bitbang_i2c_t bb_i2c;
} io_MCP23X17_t;

void io_MCP23X17_rst(io_MCP23X17_t* mcp);
void io_MCP23X17_init(io_MCP23X17_t* mcp);
void io_MCP23X17_set_addr(io_MCP23X17_t* mcp, unsigned char addr);

unsigned char io_MCP23X17_SPI_io(io_MCP23X17_t* mcp, unsigned char si, unsigned char sck, unsigned char rst,
                                 unsigned char cs);
unsigned char io_MCP23X17_I2C_io(io_MCP23X17_t* mcp, unsigned char scl, unsigned char sda);

#endif  // IO_MCP23X17
