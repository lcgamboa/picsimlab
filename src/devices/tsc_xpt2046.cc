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

#define dprintf if (1) {} else printf

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
 tsc_->x = 0;
 tsc_->y = 0;
 tsc_->press = 0;

 bitbang_spi_rst (&tsc_->bb_spi);
 tsc_->cmd = 0;
 tsc_->ret = 2;

 dprintf ("tsc_ rst\n");
}

void
tsc_XPT2046_init(tsc_XPT2046_t *tsc_, unsigned int w, unsigned int h)
{
 tsc_->width = w;
 tsc_->height = h;
 dprintf ("tsc_ init %i %i\n", w, h);
 bitbang_spi_init (&tsc_->bb_spi);
 tsc_XPT2046_rst (tsc_);
}

void
tsc_XPT2046_set_pos(tsc_XPT2046_t *tsc_, int x, int y)
{

 if ((x >= 0)&&(y >= 0))
  {
   tsc_->ret &= ~2;
   tsc_->x = x;
   tsc_->y = y;
   tsc_->press = 1000;
  }
 else
  {
   tsc_->ret |= 2;
   tsc_->press = 0;
  }

 dprintf ("tsc_ set pos %i %i \n", x, y);
}

#define ioSPI_clk 0
#define ioSPI_din 1
#define ioSPI_cs  2

unsigned char
tsc_XPT2046_SPI_io(tsc_XPT2046_t *tsc_, const unsigned char **pins_value)
{

 //restart
 if ((*pins_value[ioSPI_din])&&(tsc_->bb_spi.byte))
  {
   tsc_->bb_spi.byte = 0;
   tsc_->bb_spi.bit = 0;
  }


 bitbang_spi_io_ (&tsc_->bb_spi, pins_value);

 switch (bitbang_spi_get_status (&tsc_->bb_spi))
  {
  case SPI_DATA:
   if (tsc_->bb_spi.data8 & BIT_S)
    {
     tsc_->bb_spi.byte = 0;
    }

   //dprintf ("-------tsc_ data 0x%02X  byte=%i\n", tsc_->data,tsc_->byte);

   switch (tsc_->bb_spi.byte)
    {
    case 0:
     if (tsc_->bb_spi.data8 & BIT_S)
      {
       tsc_->cmd = tsc_->bb_spi.data8;
       dprintf ("tsc_ cmd 0x%02X OK\n", tsc_->cmd);

       switch ((tsc_->cmd & 0x70) >> 4)
        {
        case 1: // Y -Position
         tsc_->bb_spi.outsr = (tsc_->x * 4095) / tsc_->height;
         break;
        case 3: // Z1
         tsc_->bb_spi.outsr = tsc_->press;
         break;
        case 4: // Z2
         tsc_->bb_spi.outsr = 4095;
         break;
        case 5: // X -Position
         tsc_->bb_spi.outsr = (tsc_->y * 4095) / tsc_->width;
         break;
        default:
         tsc_->bb_spi.outsr = 0;
         break;
        }
      }
     else
      {
       tsc_->bb_spi.bit = 0x80;
       tsc_->bb_spi.byte = 0xFF;
       dprintf ("tsc_ cmd 0x%02X ERROR\n", tsc_->cmd);
      }
     break;
    }
   //break;
  case SPI_BIT:
   if (tsc_->bb_spi.outsr & 0x1000)
    {
     tsc_->ret |= 1;
    }
   else
    {
     tsc_->ret &= ~1;
    }
   break;
  }

 return tsc_->ret;
}
