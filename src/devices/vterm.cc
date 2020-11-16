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


#include"vterm.h"

#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>


#define dprintf if (1) {} else printf

void
vterm_rst(vterm_t *vt)
{
 vt->arx = 1;
 vt->invt = 0;
 vt->outvt = 1;
 vt->bcr = 0;
 vt->bcw = 0;
 vt->rxc = 0;
 vt->tcountr = 0;
 vt->tcountw = 0;
 vt->leds = 0;
 vt->count_in = 0;
 vt->count_out = 0;
 vt->out_ptr = 0;
 dprintf ("rst uart\n");
}

void
vterm_init(vterm_t *vt)
{
 vterm_rst (vt);
 vt->speed = 9600;
 vt->count = 0;
 dprintf ("init uart\n");
}

void
vterm_end(vterm_t *vt) {
 }

void
vterm_set_clk(vterm_t *vt, unsigned long clk)
{
 vt->count = clk / (16 * vt->speed);
}

unsigned char
vterm_io(vterm_t *vt, unsigned char rx)
{
 //unsigned char data;

 //read rx
 if (vt->tcountr)
  {
   vt->tcountr++;

   if (!(vt->tcountr % vt->count))
    {
     if (rx)vt->rxc++;
    }

   if (vt->tcountr >= vt->count << 4)
    {
     vt->tcountr = 1;


     if (vt->rxc > 7)
      {
       vt->invt = (vt->invt >> 1) | 0x80;
      }
     else
      {
       vt->invt = (vt->invt >> 1) & 0xF7FF;
      }
     vt->bcr++;
     vt->rxc = 0;

     if (vt->bcr == 8)//start+eight bits+ stop
      {
       //dprintf ("uart byte in 0x%02X  out 0x%02X\n", vt->invt & 0xFF, vt->outvt >> 8);
       //printf ("%c  0x%04X\n", vt->invt >> 1, vt->invt >> 1);

       vt->buff_in[vt->count_in++] = vt->invt >> 1;
       if (vt->count_in >= SBUFFMAX)vt->count_in = 0;
       //serial_port_send (vt->serialfd, (vt->invt >> 1)&0xFF);

      }
    }

   //stop bit
   if ((vt->bcr > 7)&&(vt->arx == 0)&&(rx == 1))//rising edge
    {
     vt->tcountr = 0;
     vt->bcr = 0;
    }

  }
 else
  {
   //start bit
   if ((vt->arx == 1)&&(rx == 0))//falling edge
    {
     //dprintf ("uart start bit \n");
     vt->tcountr = 1;
     vt->bcr = 0;
     vt->invt = 0;
     vt->rxc = 0;
     vt->leds |= 0x01;
    }
  }

 vt->arx = rx;


 //write tx

 vt->tcountw++;

 if (vt->tcountw >= (vt->count << 4))
  {
   vt->tcountw = 0;

   if (!vt->bcw)
    {

     if (vt->count_out)
      {
       char data = vt->buff_out[vt->out_ptr++];

       if (vt->out_ptr == vt->count_out)
	{
	 vt->out_ptr = 0;
	 vt->count_out = 0;
	}

       dprintf ("uart data rec %c \n", data);
       vt->leds |= 0x02;
       vt->bcw = 1;
       vt->outvt = (data << 1) | 0x200;
      }

    }
   else
    {
     vt->outvt = (vt->outvt >> 1);
     vt->bcw++;
     if (vt->bcw > 9)
      {
       vt->bcw = 0;
      }
    }

  }



 return (vt->outvt & 0x01);
}

void
vterm_set_speed(vterm_t *vt, unsigned int speed)
{
 vt->speed = speed;
}