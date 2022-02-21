/* ########################################################################

   PICsimLab - PIC laboratory simulator

   ########################################################################

   Copyright (c) : 2022-2022  Luis Claudio Gambôa Lopes

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


#ifndef SWBOUNCE
#define	SWBOUNCE

typedef struct{
    int swcount;
    unsigned char * bounce;
    int do_bounce;
    int bcount;
    int btime;  
    int bnum;    
    float system_freq;
}SWBounce_t;

void SWBounce_init(SWBounce_t * swb, const int swcount);
void SWBounce_end(SWBounce_t * swb);
void SWBounce_prepare(SWBounce_t * swb, const float system_freq);
void SWBounce_bounce(SWBounce_t * swb, const int sw);
int SWBounce_process(SWBounce_t * swb);

#endif //SWBOUNCE

