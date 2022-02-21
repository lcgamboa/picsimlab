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

#include <stdlib.h>
#include <string.h>
#include"swbounce.h"

void 
SWBounce_init(SWBounce_t * swb, const int swcount)
{
  swb->swcount = swcount;
  swb->bounce = (unsigned char *) malloc(swcount);
  swb->do_bounce =0 ;
}

void 
SWBounce_end(SWBounce_t * swb)
{
  free(swb->bounce);
}

void 
SWBounce_prepare(SWBounce_t * swb, const float system_freq)
{
 swb->do_bounce = 0;
 swb->bnum = 0;
 swb->system_freq = system_freq;
 memset(swb->bounce, 0, swb->swcount);
}

void SWBounce_bounce(SWBounce_t * swb, const int sw)
{
       if (!swb->bnum)
        {
         swb->bnum = (90.0 * rand () / RAND_MAX) + 10;
         swb->btime = swb->system_freq / (1000 * swb->bnum); 
         swb->bcount = swb->btime;
        }
       swb->bounce[sw] = swb->bnum;
       swb->do_bounce = 1;
}

int 
SWBounce_process(SWBounce_t * swb)
{
 int ret=0; 
if (swb->do_bounce)
  {
   swb->btime--;
   if (!swb->btime)
    {
     swb->btime = swb->bcount;
     ret = 1;
     for (int i = 0; i < swb->swcount; i++)
      {
       if (swb->bounce[i])
        {
         swb->bounce[i]--;
         if (!swb->bounce[i])
          {
           swb->bounce[i]--; 
           swb->do_bounce = 0;
           ret = 2;
          }
        }
      }
    }
  }  
  return ret;
}