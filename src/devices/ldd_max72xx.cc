/* ########################################################################

   PICsimLab - PIC laboratory simulator

   ########################################################################

   Copyright (c) : 2010-2019  Luis Claudio Gambôa Lopes

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


#define dprint if (1) {} else printf

#include"ldd_max72xx.h"

void
ldd_max72xx_rst(ldd_max72xx_t *ldd)
{
 int i;
 for (i = 0; i < 8; i++)
  ldd->ram[i] = 0;
 ldd->bc = 0;
 ldd->aclk = 1;
 ldd->update = 1;
 ldd->dat = 0;
 ldd->dout = 0;
}

void
ldd_max72xx_init(ldd_max72xx_t *ldd)
{
 ldd->bc = 0;
 ldd->aclk = 1;
 ldd->update = 1;
 ldd->dat = 0;
}

void
ldd_max72xx_update(ldd_max72xx_t *ldd)
{
 ldd->update = 1;
}

unsigned char
ldd_max72xx_io(ldd_max72xx_t *ldd, unsigned char din, unsigned char clk, unsigned char ncs)
{

 if ((ldd->ancs == 0)&&(ncs == 1))//cs rising edge
  {

   dprint ("data[%i]:%#02X  \n", (0x0F00 & ldd->dat) >> 8, 0x00FF & ldd->dat);


   switch ((0x0F00 & ldd->dat) >> 8)
    {
    case 0://NOP
     break;
    case 1://DIG0
    case 2://DIG1
    case 3://DIG2
    case 4://DIG3
    case 5://DIG4
    case 6://DIG5
    case 7://DIG6
    case 8://DIG7
     ldd->ram[((0x0F00 & ldd->dat) >> 8) - 1] = 0x00FF & ldd->dat;
     break;
    case 9://decode
     //TODO decode 
     break;
    case 10://itensity
     //TODO itensity 
     break;
    case 11://scan limit
     //TODO scan limit
     break;
    case 12://shutdown
     //TODO shutdown
     break;
    case 13://not defined
     break;
    case 14://not defined
     break;
    case 15://display test
     //TODO display test
     break;
    }

   ldd->update = 1;

   ldd->bc = 0;
   ldd->ancs = ncs;
   return 1;
  }
 ldd->ancs = ncs;

 //transicao
 if ((ldd->aclk == 0)&&(clk == 1))//rising edge
  {
/*
   if (ldd->bc == 0)
    {
     ldd->dat = 0;
    }
 *       ldd->dat |= din << (15 - ldd->bc);
 */
  
   ldd->dout= (ldd->dat & 0x8000) > 0;
   ldd->dat= (ldd->dat<<1) | din;
   
   ldd->bc++;

   if (ldd->bc >= 16)//16 bits received
    {
     ldd->bc = 0;
    }

  }

 ldd->aclk = clk;
 return ldd->dout;
}

void
ldd_max72xx_draw(ldd_max72xx_t *ldd, CCanvas * canvas, int x1, int y1, int w1, int h1, int picpwr, int angle)
{
 int x, y, a, b;

 ldd->update = 0;

 for (x = 0; x < 8; x++)
  {
   for (y = 0; y < 8; y++)
    {
     switch (angle)
      {
      case 0:
       a = 7 - x;
       b = y;
       break;
      case 90:
       a = y;
       b = x;
       break;
      case 180:
       a = x;
       b = 7-y;
       break;
      case 270:
       a = 7 - y;
       b = 7 - x;
       break;
      default:
       a = 7 - x;
       b = y;
       break;
      }

     if (ldd->ram[a] & (1 << (b)))
      {
       canvas->SetFgColor (250, 0, 0);
       canvas->SetColor (250, 0, 0);
      }
     else
      {
       canvas->SetFgColor (100, 100, 100);
       canvas->SetColor (100, 100, 100);
      }

     canvas->Circle (1, x1 + (y * 20) + 10, y1 + (x * 20) + 10, 7);
    }
  }
}








