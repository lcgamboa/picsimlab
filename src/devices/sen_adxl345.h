/* ########################################################################

   PICSimLab - Programmable IC Simulator Laboratory

   ########################################################################

   Copyright (c) : 2021-2024  Luis Claudio Gambôa Lopes <lcgamboa@yahoo.com>

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
#include "bitbang_spi.h"

typedef struct {
    unsigned char regs[0x3A];
    unsigned char addr;
    bitbang_i2c_t bb_i2c;
    bitbang_spi_t bb_spi;
    unsigned char data_wc;
    unsigned int update;
    int i2c_mode;
} adxl345_t;

void adxl345_rst(adxl345_t* adxl);
void adxl345_init(adxl345_t* adxl);
void adxl345_end(adxl345_t* adxl);
void adxl345_set_addr(adxl345_t* adxl, unsigned char addr);

void adxl345_set_accel(adxl345_t* adxl, double x, double y, double z);   // g
void adxl345_set_accel_raw(adxl345_t* adxl, short x, short y, short z);  // g

unsigned char adxl345_io_I2C(adxl345_t* adxl, unsigned char scl, unsigned char sda);
unsigned short adxl345_io_SPI(adxl345_t* adxl, unsigned char mosi, unsigned char clk, unsigned char ss);

// clang-format off

// REGISTERS ADDRESS
#define DEVID           0x00
//0x01 to 0x1C  Reserved
#define THRESH_TAP      0x1D
#define OFSX            0x1E
#define OFSY            0x1F
#define OFSZ            0x20
#define DUR             0x21
#define Latent          0x22
#define WINDOW          0x23
#define THRESH_ACT      0x24
#define THRESH_INACT    0x25
#define TIME_INACT      0x26
#define ACT_INACT_CTL   0x27
#define THRESH_FF       0x28
#define TIME_FF         0x29
#define TAP_AXES        0x2A
#define ACT_TAP_STATUS  0x2B
#define BW_RATE         0x2C
#define POWER_CTL       0x2D 
#define INT_ENABLE      0x2E
#define INT_MAP         0x2F
#define INT_SOURCE      0x30
#define DATA_FORMAT     0x31
#define DATAX0          0x32
#define DATAX1          0x33
#define DATAY0          0x34
#define DATAY1          0x35
#define DATAZ0          0x36
#define DATAZ1          0x37
#define FIFO_CTL        0x38
#define FIFO_STATUS     0x39

// clang-format on