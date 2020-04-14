/* ########################################################################

   PICsimLab - PIC laboratory simulator

   ########################################################################

   Copyright (c) : 2010-2015  Luis Claudio Gambôa Lopes

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
   mem->maddr = addr << 1;
  }
}

void
mi2c_rst(mi2c_t *mem)
{

 mem->sdao = 0;
 mem->sclo = 1;
 mem->bit = 0xFF;
 mem->byte = 0xFF;
 mem->datab = 0;
 mem->ctrl = 0;
 mem->ret = 0;
 dprintf ("mi2c rst\n");

 //for(i=0;i<100;i++)
 //printf("%02X ",mem->data[i]);
 //printf("\n");
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
 mi2c_rst (mem);
 mem->maddr = 0x50<<1;
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

 if ((mem->sdao == 1)&&(sda == 0)&&(scl == 1)&&(mem->sclo == 1)) //start
  {
   mem->bit = 0;
   mem->byte = 0;
   mem->datab = 0;
   mem->ctrl = 0;
   mem->ret = 0;
   dprintf ("---->mem start!\n");
  }

 if ((mem->sdao == 0)&&(sda == 1)&&(scl == 1)&&(mem->sclo == 1)) //stop
  {
   mem->bit = 0xFF;
   mem->byte = 0xFF;
   mem->ctrl = 0;
   mem->ret = 0;
   dprintf ("---> mem stop!\n");
  }


 if ((mem->bit < 9)&&(mem->sclo == 0)&&(scl == 1)) //data 
  {

   if (mem->bit < 8)
    {
     mem->datab |= (sda << (7 - mem->bit));
    }

   mem->bit++;
  }

 if ((mem->bit < 9)&&(mem->sclo == 1)&&(scl == 0)&&(mem->ctrl == 0x0A1)) //data 
  {
   if (mem->bit < 8)
    {
     mem->ret = ((mem->datas & (1 << (7 - mem->bit))) > 0);
     //dprintf("send %i %i (%02X)\n",mem->bit,mem->ret,mem->datas);  
    }
   else
    {
     mem->ret = 0;
    }
  }


 if (mem->bit == 9)
  {
   dprintf ("mi2c data %02X\n", mem->datab);

   if (mem->byte == 0)
    {
     if (mem->ADDRB == 2)
      mem->ctrl = mem->datab;
     else
      mem->ctrl = mem->datab & 0xF1;

     dprintf ("----> mem ctrl = %02X\n", mem->ctrl);
     mem->ret = 0;

     if ((mem->ctrl & 0x01) == 0x00)
      {
       if (mem->ADDRB == 2)
        mem->addr = 0;
       else
        {
         mem->addr = ((mem->datab & 0x0E) << 7);
        }
      }

    }

   if (mem->ctrl == mem->maddr)
    {
     if (mem->ADDRB == 2)
      {
       if (mem->byte == 1)
        {
         mem->addr |= (mem->datab << 8);
         dprintf ("----> mem add = %02X\n", mem->addr);
        }
       if (mem->byte == 2)
        {
         mem->addr |= mem->datab;
         dprintf ("----> mem add = %02X\n", mem->addr);
        }
      }
     else
      {
       if (mem->byte == 1)
        {
         mem->addr |= mem->datab;
         dprintf ("----> mem add = %02X\n", mem->addr);
        };
      }

     if ((mem->byte > mem->ADDRB)&&((mem->ctrl & 0x01) == 0))
      {
       mem->data[mem->addr] = mem->datab;
       dprintf ("write mem[%04X]=%02X\n", mem->addr, mem->datab);
       mem->addr++;
      }
     mem->ret = 0;
    }
   else if (mem->ctrl == (mem->maddr | 1)) //read
    {
     if (mem->byte < mem->SIZE)
      {
       mem->datas = mem->data[mem->addr];
       dprintf ("mi2c read[%04X]=%02X\n", mem->addr, mem->datas);
       mem->addr++;
      }
     else
      {
       mem->ctrl = 0xFF;
      }
    }



   mem->bit = 0;
   mem->datab = 0;
   mem->byte++;
  }


 mem->sdao = sda;
 mem->sclo = scl;
 return mem->ret;
}
