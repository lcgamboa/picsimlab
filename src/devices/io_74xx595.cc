/* ########################################################################

   PICsimLab - PIC laboratory simulator

   ########################################################################

   Copyright (c) : 2010-2022  Luis Claudio Gambôa Lopes

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


#include"io_74xx595.h"

void
io_74xx595_rst(io_74xx595_t *sr)
{
 sr->asclk = 1;
 sr->alclk = 1;
 sr->dsr = 0;
 sr->sout = 0;
 sr->out = 0;
}

void
io_74xx595_init(io_74xx595_t *sr)
{
 io_74xx595_rst(sr); 
}

unsigned short
io_74xx595_io(io_74xx595_t *sr, unsigned char A, unsigned char sclk, unsigned char lclk, unsigned char rst)
{

 if (!rst)
  {
   io_74xx595_rst(sr);
   return sr->out;
  }


 //transicao
 if ((sr->asclk == 0)&&(sclk == 1))//rising edge
  {
   if (A)
    {
     sr->dsr = (sr->dsr << 1) | 1;
    }
   else
    {
     sr->dsr = (sr->dsr << 1) & 0x1FE;
    }
  }
 sr->asclk = sclk;

 //transicao
 if ((sr->alclk == 0)&&(lclk == 1))//rising edge
  {
   sr->out = sr->dsr;
  }
 sr->alclk = lclk;


 return (sr->dsr & 0x0100)| sr->out ;
}
