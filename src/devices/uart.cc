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


#include"uart.h"

#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>


#define dprintf if (1) {} else printf

void
uart_rst(uart_t *sr)
{
 bitbang_uart_rst (&sr->bb_uart);
 dprintf ("rst uart\n");
 sr->rxcount = 0;
 sr->rxmax = 0;
}

void
uart_init(uart_t *sr)
{
 bitbang_uart_init (&sr->bb_uart);
 sr->connected = 0;
 uart_rst (sr);
 sr->serialfd = 0;
 dprintf ("init uart\n");
}

void
uart_end(uart_t *sr)
{
 if (sr->connected)
  {
   serial_port_close (sr->serialfd);
   sr->connected = 0;
  }
 bitbang_uart_end (&sr->bb_uart);
}

void
uart_set_clk_freq(uart_t *sr, const unsigned long freq)
{
 bitbang_uart_set_clk_freq (&sr->bb_uart, freq);
 sr->rxmax = sr->bb_uart.cycle_count * 16;
 sr->rxcount = sr->rxmax + 1;
}

unsigned char
uart_io(uart_t *sr, const unsigned char rx)
{
 unsigned char ret;

 if (!sr->connected)
  {
   return 1;
  }

 if (sr->rxcount > sr->rxmax)//wait to read one byte 
  {
   if (!bitbang_uart_transmitting (&sr->bb_uart))
    {
     unsigned char data;
     if (serial_port_rec (sr->serialfd, &data))
      {
       bitbang_uart_send (&sr->bb_uart, data);
      }
     sr->rxcount = 1;
    }
  }
 else
  {
   sr->rxcount++;
  }

 ret = bitbang_uart_io (&sr->bb_uart, rx);

 if (bitbang_uart_data_available (&sr->bb_uart))
  {
   serial_port_send (sr->serialfd, bitbang_uart_recv (&sr->bb_uart));
  }



 return ret;
}

void
uart_set_port(uart_t *sr, const char * port, const unsigned int speed)
{

 if (sr->connected)
  {
   serial_port_close (sr->serialfd);
   sr->connected = 0;
  }

 if (serial_port_open (&sr->serialfd, port))
  {
   sr->connected = 1;
   bitbang_uart_set_speed (&sr->bb_uart, speed);
   serial_port_cfg (sr->serialfd, speed);
   dprintf ("uart serial open: %s  speed %i\n", port, speed);
  }
 else
  {
   sr->connected = 0;
   dprintf ("uart serial error open: %s !!!\n", port);
  }
}