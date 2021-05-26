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

#ifndef BITBANG_I2C
#define BITBANG_I2C

//operation status
#define I2C_START 0x01
#define I2C_STOP  0x02
#define I2C_ADDR  0x04
#define I2C_DATAW 0x08
#define I2C_DATAR 0x10

typedef struct {
    unsigned char addr;     //device addr
    unsigned char addr_mask; //addr mask 

    unsigned char datab; //input data shift register
    unsigned char datas; //output data shift register

    unsigned char sclo; //previous scl
    unsigned char sdao; //previous sda

    unsigned char ret; //last returned value

    unsigned char bit; //bit counter
    unsigned char byte; //byte counter

    unsigned char status; //operation status

    unsigned char datar; //data receveid  

    unsigned char data_reading; //reading operation

} bitbang_i2c_t;


void bitbang_i2c_init(bitbang_i2c_t *i2c, unsigned char addr, unsigned char addr_mask = 0xFE);
void bitbang_i2c_rst(bitbang_i2c_t *i2c);
void bitbang_i2c_set_addr(bitbang_i2c_t *i2c, unsigned char addr);
unsigned char bitbang_i2c_get_status(bitbang_i2c_t *i2c);
void bitbang_i2c_send(bitbang_i2c_t *i2c, unsigned char data);

//periferic 
unsigned char bitbang_i2c_io(bitbang_i2c_t *i2c, unsigned char scl, unsigned char sda);

#endif //BITBANG_I2C