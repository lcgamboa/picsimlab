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
void mpu6050_set_accel_raw(mpu6050_t *mpu, short x, short y, short z);//g
void mpu6050_set_gyro_raw(mpu6050_t *mpu, short x, short y, short z);//degrees/s

unsigned char mpu6050_io_I2C(mpu6050_t *mpu, unsigned char scl, unsigned char sda);

//REGISTERS ADDRESS
#define SELF_TEST_X       0x0D 
#define SELF_TEST_Y       0x0E 
#define SELF_TEST_Z       0x0F 

#define SELF_TEST_A       0x10 

#define SMPLRT_DIV        0x19  
#define CONFIG            0x1A      
#define GYRO_CONFIG       0x1B 
#define ACCEL_CONFIG      0x1C 

#define FIFO_EN           0x23
#define I2C_MST_CTRL      0x24
//..
#define I2C_MST_STATUS    0x36
#define INT_PIN_CONFIG    0x37
#define INT_ENABLED       0x38

#define INT_STATUS        0x3A
#define ACCEL_XOUT_H      0x3B 
#define ACCEL_XOUT_L      0x3C 
#define ACCEL_YOUT_H      0x3D 
#define ACCEL_YOUT_L      0x3E 
#define ACCEL_ZOUT_H      0x3F 
#define ACCEL_ZOUT_L      0x40 
#define TEMP_OUT_H        0x41 
#define TEMP_OUT_L        0x42 
#define GYRO_XOUT_H       0x43 
#define GYRO_XOUT_L       0x44
#define GYRO_YOUT_H       0x45 
#define GYRO_YOUT_L       0x46
#define GYRO_ZOUT_H       0x47 
#define GYRO_ZOUT_L       0x48
#define EXT_SENS_DATA_00  0x49
//...
#define EXT_SENS_DATA_23  0x60

#define I2C_SLV0_DO       0x63
#define I2C_SLV0_D1       0x64
#define I2C_SLV0_D2       0x65
#define I2C_SLV0_D3       0x66
#define I2C_MST_DELAY_CTRL 0x67
#define SIGNAL_PATH_RESET 0x68 

#define USER_CTRL         0x6A         
#define PWR_MGMT_1        0x6B       
#define PWR_MGMT_2        0x6C    

#define FIFO_COUNT_H      0x72   
#define FIFO_COUNT_L      0x73   
#define FIFO_R_W          0x74   
#define WHO_AM_I          0x75 