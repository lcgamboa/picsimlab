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


#include"bitbang_i2c.h"

#include<stdio.h>
#include<string.h>
#include<stdlib.h>

#define dprintf if (1) {} else printf

void
bitbang_i2c_set_addr(bitbang_i2c_t *i2c, unsigned char addr)
{
 dprintf ("bitbang_i2c set_addr 0x%02X \n", addr);
 i2c->addr = addr << 1;
}

void
bitbang_i2c_rst(bitbang_i2c_t *i2c)
{

 i2c->sdao = 0;
 i2c->sclo = 1;
 i2c->bit = 0xFF;
 i2c->byte = 0xFF;
 i2c->datab = 0;
 i2c->ret = 0;
 i2c->status = 0;
 i2c->data_reading = 0;
 dprintf ("bitbang_i2c rst\n");
}

void
bitbang_i2c_init(bitbang_i2c_t *i2c, unsigned char addr, unsigned char addr_mask)
{
 dprintf ("bitbang_i2c init 0x%02X \n", addr);
 bitbang_i2c_rst (i2c);
 i2c->addr = addr << 1;
 i2c->addr_mask = addr_mask;
}

unsigned char
bitbang_i2c_io(bitbang_i2c_t *i2c, unsigned char scl, unsigned char sda)
{

 if ((i2c->sdao == sda)&&(i2c->sclo == scl))return i2c->ret;


 if ((i2c->sdao == 1)&&(sda == 0)&&(scl == 1)&&(i2c->sclo == 1)) //start
  {
   i2c->bit = 0;
   i2c->byte = 0;
   i2c->datab = 0;
   i2c->ret = 0;
   i2c->data_reading = 0;
   dprintf ("---->i2c start!\n");
   i2c->status = I2C_START;
  }

 if ((i2c->sdao == 0)&&(sda == 1)&&(scl == 1)&&(i2c->sclo == 1)) //stop
  {
   i2c->bit = 0xFF;
   i2c->byte = 0xFF;
   i2c->ret = 0;
   i2c->data_reading = 0;
   dprintf ("---> i2c stop!\n");
   i2c->status = I2C_STOP;
  }


 if ((i2c->bit < 9)&&(i2c->sclo == 0)&&(scl == 1)) //data in
  {

   if (i2c->bit < 8)
    {
     i2c->datab |= (sda << (7 - i2c->bit));
    }

   i2c->bit++;
  }

 if ((i2c->bit < 9)&&(i2c->sclo == 1)&&(scl == 0) && i2c->data_reading) //data out
  {
   if (i2c->bit < 8)
    {
     i2c->ret = ((i2c->datas & (1 << (7 - i2c->bit))) > 0);
     //dprintf("send %i %i (%02X)\n",i2c->bit,i2c->ret,i2c->datas);  
    }
   else
    {
     i2c->ret = 0;
    }
  }


 if (i2c->bit == 9)
  {
   dprintf ("bitbang_i2c data %02X\n", i2c->datab);

   if (i2c->byte == 0)//ADDR
    {
     if ((i2c->datab & i2c->addr_mask) == i2c->addr)//valid address
      {
       dprintf ("bitbang_i2c addr OK %02X\n", (i2c->datab & i2c->addr_mask) >> 1);

       if (i2c->datab & 0x01)
        {
         i2c->data_reading = 1;
         dprintf ("bitbang_i2c READ \n");
        }
       i2c->status = I2C_ADDR;
       i2c->datar = i2c->datab;
       i2c->bit = 0;
       i2c->datab = 0;
       i2c->byte++;
       i2c->ret = 0;
      }
     else //invalid address
      {
       dprintf ("bitbang_i2c addr NOK %02X\n", (i2c->datab & 0xFE) >> 1);
       i2c->bit = 0xFF;
       i2c->byte = 0xFF;
      }
    }
   else if (i2c->data_reading == 0)//data
    {
     dprintf ("bitbang_i2c received OK %02X\n", i2c->datab);
     i2c->datar = i2c->datab;
     i2c->bit = 0;
     i2c->datab = 0;
     i2c->byte++;
     i2c->status = I2C_DATAW;
    }

   if (i2c->data_reading)
    {
     i2c->datar = i2c->datab;
     i2c->bit = 0;
     i2c->datab = 0;
     i2c->byte++;
     i2c->status = I2C_DATAR;
     dprintf ("bitbang_i2c need data to send\n");
    }
  }

 i2c->sdao = sda;
 i2c->sclo = scl;
 return i2c->ret;
}

unsigned char
bitbang_i2c_get_status(bitbang_i2c_t *i2c)
{
 unsigned char status = i2c->status;
 i2c->status = 0;
 return status;
}

void
bitbang_i2c_send(bitbang_i2c_t *i2c, unsigned char data)
{
 i2c->datas = data;
 dprintf ("bitbang_i2c data to send 0x%02x \n", data);
}