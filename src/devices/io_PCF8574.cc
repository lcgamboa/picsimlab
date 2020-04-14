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


#include"io_PCF8574.h"

#include<stdio.h>
#include<string.h>
#include<stdlib.h>

#define dprintf if (1) {} else printf

void 
io_PCF8574_set_addr(io_PCF8574_t *ioe8, unsigned char addr)
{
 ioe8->addr = addr << 1;
}


void
io_PCF8574_rst(io_PCF8574_t *ioe8)
{
 //int i;
 ioe8->sdao = -1; //0
 ioe8->sclo = -1; //1
 ioe8->bit = 0xFF;
 ioe8->byte = 0xFF;
 ioe8->datab = 0;
 ioe8->ctrl = 0;
 ioe8->ret = 0;

 dprintf ("ioe8 rst\n");

 //for(i=0;i<16;i++)
 //printf("%02X ",ioe8->data[i]);
 //printf("\n");
}

void
io_PCF8574_init(io_PCF8574_t *ioe8)
{
 dprintf ("ioe8 init\n");
 ioe8->data = 0;
 io_PCF8574_rst (ioe8);
 ioe8->addr=0x27<<1;
}

void
io_PCF8574_end(io_PCF8574_t *ioe8)
{
 dprintf ("ioe8 end\n");
}

unsigned char
io_PCF8574_io(io_PCF8574_t *ioe8, unsigned char scl, unsigned char sda)
{

 if ((ioe8->sdao == sda)&&(ioe8->sclo == scl))return ioe8->ret;

 if ((ioe8->sdao == 1)&&(sda == 0)&&(scl == 1)&&(ioe8->sclo == 1)) //start
  {
   ioe8->bit = 0;
   ioe8->byte = 0;
   ioe8->datab = 0;
   ioe8->ctrl = 0;
   ioe8->ret = 0;
   dprintf ("ioe8 start!\n");
  }

 if ((ioe8->sdao == 0)&&(sda == 1)&&(scl == 1)&&(ioe8->sclo == 1)) //stop
  {
   ioe8->bit = 0xFF;
   ioe8->byte = 0xFF;
   ioe8->ctrl = 0;
   ioe8->ret = 0;
   dprintf ("ioe8 stop!\n");
  }


 if ((ioe8->bit < 9)&&(ioe8->sclo == 0)&&(scl == 1)) //data 
  {

   if (ioe8->bit < 8)
    {
     ioe8->datab |= (sda << (7 - ioe8->bit));
    }

   ioe8->bit++;
  }

 if ((ioe8->bit < 9)&&(ioe8->sclo == 1)&&(scl == 0)&&(ioe8->ctrl == 0x071)) //data 
  {
   if (ioe8->bit < 8)
    {
     ioe8->ret = ((ioe8->datas & (1 << (7 - ioe8->bit))) > 0);
     //dprintf("ioe8 send %i %i (%02X)\n",ioe8->bit,ioe8->ret,ioe8->datas);  
    }
   else
    {
     ioe8->ret = 0;
    }
  }


 if (ioe8->bit == 9)
  {
   dprintf ("ioe8 data %02X\n", ioe8->datab);

   if (ioe8->byte == 0)
    {
     ioe8->ctrl = ioe8->datab;
     dprintf ("ioe8 ctrl = %02X\n", ioe8->ctrl);
     ioe8->ret = 0;

    }

   if ((ioe8->ctrl) == ioe8->addr)
    {

     if (((ioe8->byte > 0)&&(ioe8->ctrl & 0x01) == 0))
      {
       dprintf ("write ioe8 =%02X\n", ioe8->datab);

       ioe8->data = ioe8->datab;

       ioe8->ret = 0;

      }
    }
   else if ((ioe8->ctrl) == (ioe8->addr|1)) //read
    {

     ioe8->datas = ioe8->data;
     dprintf ("ioe8 read =%02X\n", ioe8->datas);
    }




   ioe8->bit = 0;
   ioe8->datab = 0;
   ioe8->byte++;
  }


 ioe8->sdao = sda;
 ioe8->sclo = scl;
 return ioe8->ret;
}
