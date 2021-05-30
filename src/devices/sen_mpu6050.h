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

#include"bitbang_i2c.h"

typedef struct {
    unsigned char regs[0x76];
    unsigned char addr;
    bitbang_i2c_t bb_i2c;
} mpu6050_t;


void mpu6050_rst(mpu6050_t *mpu);
void mpu6050_init(mpu6050_t *mpu);
void mpu6050_end(mpu6050_t *mpu);
void mpu6050_set_addr(mpu6050_t *mpu, unsigned char addr);

void mpu6050_set_temp(mpu6050_t *mpu, double temp);//degrees
void mpu6050_set_accel(mpu6050_t *mpu, double x, double y, double z);//g
void mpu6050_set_gyro(mpu6050_t *mpu, double x, double y, double z);//degrees/s

unsigned char mpu6050_io_I2C(mpu6050_t *mpu, unsigned char scl, unsigned char sda);
