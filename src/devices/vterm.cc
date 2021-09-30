/* ########################################################################

   PICsimLab - PIC laboratory simulator

   ########################################################################

   Copyright (c) : 2020-2020  Luis Claudio Gambôa Lopes

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
 bitbang_uart_rst (&vt->bb_uart);
 vt->count_in = 0;
 vt->count_out = 0;
 vt->out_ptr = 0;
 dprintf ("rst uart\n");
}

void
vterm_init(vterm_t *vt)
{
 bitbang_uart_init (&vt->bb_uart);
 vterm_rst (vt);
 dprintf ("init uart\n");
}

void
vterm_end(vterm_t *vt) { }

void
vterm_set_clk_freq(vterm_t *vt, const unsigned long freq)
{
 bitbang_uart_set_clk_freq (&vt->bb_uart, freq);
}

unsigned char
vterm_io(vterm_t *vt, const unsigned char rx)
{
 unsigned char ret;



 if (!bitbang_uart_transmitting (&vt->bb_uart))
  {
   if (vt->count_out)
    {
     bitbang_uart_send (&vt->bb_uart, vt->buff_out[vt->out_ptr++]);
     if (vt->out_ptr == vt->count_out)
      {
       vt->out_ptr = 0;
       vt->count_out = 0;
      }
    }
  }


 ret = bitbang_uart_io (&vt->bb_uart, rx);

 if (bitbang_uart_data_available (&vt->bb_uart))
  {
   vt->buff_in[vt->count_in++] = bitbang_uart_recv (&vt->bb_uart);
   if (vt->count_in >= SBUFFMAX)vt->count_in = 0;
  }

 return ret;

}

void
vterm_set_speed(vterm_t *vt, const unsigned int speed)
{
 bitbang_uart_set_speed (&vt->bb_uart, speed);
}