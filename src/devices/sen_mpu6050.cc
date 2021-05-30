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


#include"sen_mpu6050.h"

#include<stdio.h>
#include<string.h>
#include<stdlib.h>

#define dprintf if (1) {} else printf

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

void
mpu6050_set_addr(mpu6050_t *mpu, unsigned char addr)
{
 bitbang_i2c_set_addr (&mpu->bb_i2c, addr);
}

void
mpu6050_rst(mpu6050_t *mpu)
{
 bitbang_i2c_rst (&mpu->bb_i2c);
 mpu->addr = 0;

 for (int i = 0; i < 0x76; i++)
  {
   mpu->regs[i] = 0;
  }

 mpu->regs[PWR_MGMT_1] = 0x40;
 mpu->regs[WHO_AM_I] = 0x68;

 dprintf ("mpu6050 rst\n");
}

void
mpu6050_init(mpu6050_t *mpu)
{
 dprintf ("mpu6050 init\n");

 bitbang_i2c_init (&mpu->bb_i2c, 0x68);
 mpu6050_rst (mpu);
}

void
mpu6050_end(mpu6050_t *mpu)
{
 dprintf ("mpu6050 end\n");
}

unsigned char
mpu6050_io_I2C(mpu6050_t *mpu, unsigned char scl, unsigned char sda)
{
 unsigned char ret = bitbang_i2c_io (&mpu->bb_i2c, scl, sda);

 switch (bitbang_i2c_get_status (&mpu->bb_i2c))
  {
  case I2C_ADDR:
   break;
  case I2C_DATAW:
   if (mpu->bb_i2c.byte == 2)
    {
     mpu->addr = mpu->bb_i2c.datar;
     dprintf ("----> mpu add = %02X\n", mpu->addr);
    }

   if (mpu->bb_i2c.byte > 2)
    {
     dprintf ("write mpu[%04X]=%02X\n", mpu->addr, mpu->bb_i2c.datar);

     mpu->regs[mpu->addr] = mpu->bb_i2c.datar;

     switch (mpu->addr)
      {
      case PWR_MGMT_1:
       mpu->regs[PWR_MGMT_1] &= 0x7F; //clean reset status
       break;
      case SIGNAL_PATH_RESET:
      case SMPLRT_DIV:
      case CONFIG:
      case GYRO_CONFIG:
      case ACCEL_CONFIG:
      case INT_PIN_CONFIG: 
       break;
      default:
       dprintf ("mpu write to register 0x%02X not defined yet !!!!!!!!\n", mpu->addr);
       break;
      }

     mpu->addr++;
     if (mpu->addr >= 0x76)
      {
       mpu->addr -= 0x76;
      }
    }
   break;
  case I2C_DATAR:
   bitbang_i2c_send (&mpu->bb_i2c, mpu->regs[mpu->addr]);
   dprintf ("mpu6050 read[%04X]=%02X\n", mpu->addr, mpu->regs[mpu->addr]);
   mpu->addr++;
   if (mpu->addr >= 0x76)
    {
     mpu->addr -= 0x76;
    }
   break;
  }

 return ret;
}

void
mpu6050_set_temp(mpu6050_t *mpu, double temp)
{
 //Temperature in degrees C = (TEMP_OUT Register Value as a signed quantity)/340 + 36.53

 short itemp = (temp - 36.53)*340;

 mpu->regs[TEMP_OUT_H] = (itemp & 0xFF00) >> 8;
 mpu->regs[TEMP_OUT_L] = itemp & 0x00FF;

}

void
mpu6050_set_accel(mpu6050_t *mpu, double x, double y, double z)
{
 double sens;

 switch ((mpu->regs[ACCEL_CONFIG]&0x18) >> 3)
  {
  case 0: //2g
   sens = 16384;
   break;
  case 1: //4g
   sens = 8192;
   break;
  case 2: //8g
   sens = 4096;
   break;
  case 3: //16g
   sens = 2048;
   break;
  }

 short ix = x*sens;
 short iy = y*sens;
 short iz = z*sens;

 mpu->regs[ACCEL_XOUT_H] = (ix & 0xFF00) >> 8;
 mpu->regs[ACCEL_XOUT_L] = ix & 0x00FF;
 mpu->regs[ACCEL_YOUT_H] = (iy & 0xFF00) >> 8;
 mpu->regs[ACCEL_YOUT_L] = iy & 0x00FF;
 mpu->regs[ACCEL_ZOUT_H] = (iz & 0xFF00) >> 8;
 mpu->regs[ACCEL_ZOUT_L] = iz & 0x00FF;
}

void
mpu6050_set_gyro(mpu6050_t *mpu, double x, double y, double z)
{
 double sens;

 switch ((mpu->regs[GYRO_CONFIG]&0x18) >> 3)
  {
  case 0: //250g/s
   sens = 131;
   break;
  case 1: //500g/s
   sens = 65.5;
   break;
  case 2: //1000g/s
   sens = 32.8;
   break;
  case 3: //2000g/s
   sens = 16.4;
   break;
  }

 short ix = x*sens;
 short iy = y*sens;
 short iz = z*sens;

 mpu->regs[GYRO_XOUT_H] = (ix & 0xFF00) >> 8;
 mpu->regs[GYRO_XOUT_L] = ix & 0x00FF;
 mpu->regs[GYRO_YOUT_H] = (iy & 0xFF00) >> 8;
 mpu->regs[GYRO_YOUT_L] = iy & 0x00FF;
 mpu->regs[GYRO_ZOUT_H] = (iz & 0xFF00) >> 8;
 mpu->regs[GYRO_ZOUT_L] = iz & 0x00FF;
}