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


#include<stdio.h>
#include"tsc_xpt2046.h"

#define dprintf if (0) {} else printf

#define BIT_S       0x80
#define BIT_A2      0x40 
#define BIT_A1      0x20
#define BIT_A0      0x10
#define BIT_SMODE   0x08
#define BIT_SER_DFR 0x04
#define BIT_PD1     0x02
#define BIT_PD0     0x01

void
tsc_XPT2046_rst(tsc_XPT2046_t *tsc_)
{
 tsc_->x = -1;
 tsc_->y = -1;

 tsc_->data = 0;
 tsc_->bit = 0xFF;
 tsc_->byte = 0xFF;
 tsc_->pclk = -1;
 tsc_->cmd = 0;

 dprintf ("tsc_ rst\n");
}

void
tsc_XPT2046_init(tsc_XPT2046_t *tsc_)
{
 dprintf ("tsc_ init\n");
 tsc_XPT2046_rst (tsc_);
}

void
tsc_XPT2046_set_pos(tsc_XPT2046_t *tsc_, int x, int y)
{
 tsc_->x = x;
 tsc_->y = y;
 dprintf ("tsc_ set pos %i %i \n", x, y);
}

unsigned char
tsc_XPT2046_SPI_io(tsc_XPT2046_t *tsc_, unsigned char clk, unsigned char din, unsigned char cs)
{

 if (cs)
  {
   tsc_->bit = 0;
   tsc_->byte = 0;
   return 0;
  }


 //transicao
 if ((tsc_->pclk == 0)&&(clk == 1))//rising edge
  {
   if (tsc_->bit == 8) tsc_->bit = 0;

   if (tsc_->bit < 8)
    {
     if (din)
      {
       tsc_->data = (tsc_->data << 1) | 1;
      }
     else
      {
       tsc_->data = (tsc_->data << 1) & 0xFE;
      }
     tsc_->bit++;
     
     tsc_->datas = tsc_->datas >> 1; 
    }

   if (tsc_->bit == 8)
    {
     
     if (tsc_->data & BIT_S)
      {
       tsc_->byte = 0;
      }

     //dprintf ("-------tsc_ data 0x%02X  byte=%i\n", tsc_->data,tsc_->byte);
     tsc_->bit = 0;
     switch (tsc_->byte)
      {
      case 0:
       if (tsc_->data & BIT_S)
        {
         tsc_->cmd = tsc_->data;
         tsc_->byte++;
         dprintf ("tsc_ cmd 0x%02X OK\n", tsc_->cmd);
         tsc_->datas = 0x55;
        }
       else
        {
         tsc_->bit = 0xFF;
         tsc_->byte = 0xFF;
         dprintf ("tsc_ cmd 0x%02X ERROR\n", tsc_->cmd);
        }
       break;
      case 1:
       tsc_->byte++;
       tsc_->datas = 0x55;
       break;
      default:
       tsc_->byte++;
       break;
      }
    }
  }

 tsc_->pclk = clk;

 return tsc_->datas & 0x01;
}
