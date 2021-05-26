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


#include"mi2c_24CXXX.h"

#include<stdio.h>
#include<string.h>
#include<stdlib.h>

#define dprintf if (1) {} else printf

void
mi2c_set_addr(mi2c_t *mem, unsigned char addr)
{
 if (mem->SIZE >= 4096)
  {
   bitbang_i2c_set_addr (&mem->bb_i2c, addr);
  }
}

void
mi2c_rst(mi2c_t *mem)
{
 bitbang_i2c_rst (&mem->bb_i2c);

 dprintf ("mi2c rst\n");
}

void
mi2c_init_null(mi2c_t *mem)
{
 mem->SIZE = 0;
 mem->data = NULL;
}

void
mi2c_init(mi2c_t *mem, int sizekbits)
{
 mem->SIZE = sizekbits * 128; //*1024/8

 if (mem->SIZE < 4096)
  mem->ADDRB = 1;
 else
  mem->ADDRB = 2;

 dprintf ("mi2c init size=(%i) ADDRB=(%i)\n", sizekbits, mem->ADDRB);

 mem->data = (unsigned char *) calloc (mem->SIZE, sizeof (unsigned char));

 memset (mem->data, 0xFF, mem->SIZE);

 if (mem->ADDRB == 2)
  bitbang_i2c_init (&mem->bb_i2c, 0x50);
 else
  bitbang_i2c_init (&mem->bb_i2c, 0x50, 0xF0);

 mi2c_rst (mem);
}

void
mi2c_end(mi2c_t *mem)
{
 dprintf ("mi2c end\n");
 if (mem->data)
  free (mem->data);
 mem->data = NULL;
}

unsigned char
mi2c_io(mi2c_t *mem, unsigned char scl, unsigned char sda)
{
 unsigned char ret = bitbang_i2c_io (&mem->bb_i2c, scl, sda);

 switch (bitbang_i2c_get_status (&mem->bb_i2c))
  {
  case I2C_ADDR:
   if ((mem->bb_i2c.datar & 0x01) == 0x00)
    {
     if (mem->ADDRB == 2)
      mem->addr = 0;
     else
      {
       mem->addr = ((mem->bb_i2c.datar & 0x0E) << 7);
      }
    }

   break;
  case I2C_DATAW:
   if (mem->ADDRB == 2)
    {
     if (mem->bb_i2c.byte == 2)
      {
       mem->addr |= (mem->bb_i2c.datar << 8);
       dprintf ("----> mem add = %02X\n", mem->addr);
      }
     if (mem->bb_i2c.byte == 3)
      {
       mem->addr |= mem->bb_i2c.datar;
       dprintf ("----> mem add = %02X\n", mem->addr);
      }
    }
   else
    {
     if (mem->bb_i2c.byte == 2)
      {
       mem->addr |= mem->bb_i2c.datar;
       dprintf ("----> mem add = %02X\n", mem->addr);
      }
    }

   if (mem->bb_i2c.byte > mem->ADDRB + 1)
    {
     mem->data[mem->addr] = mem->bb_i2c.datar;
     dprintf ("write mem[%04X]=%02X\n", mem->addr, mem->bb_i2c.datar);
     mem->addr++;
     if (mem->addr >= mem->SIZE)
      {
       mem->addr -= mem->SIZE;
      }
    }
   break;
  case I2C_DATAR:
   bitbang_i2c_send (&mem->bb_i2c, mem->data[mem->addr]);
   dprintf ("mi2c read[%04X]=%02X\n", mem->addr, mem->data[mem->addr]);
   mem->addr++;
   if (mem->addr >= mem->SIZE)
    {
     mem->addr -= mem->SIZE;
    }
   break;
  }

 return ret;
}
