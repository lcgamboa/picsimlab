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


#include"uart.h"

#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>


#define dprintf if (1) {} else printf

void
uart_rst(uart_t *sr)
{
 sr->arx = 1;
 sr->insr = 0;
 sr->outsr = 1;
 sr->bcr = 0;
 sr->bcw = 0;
 sr->rxc = 0;
 sr->tcountr = 0;
 sr->tcountw = 0;
 sr->leds=0;
 dprintf ("rst uart\n");
}

void
uart_init(uart_t *sr)
{
 sr->connected = 0;
 uart_rst (sr);
 sr->serialfd = 0;
 sr->speed = 9600;
 sr->count = 0;
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
}

void
uart_set_clk(uart_t *sr, unsigned long clk)
{
 sr->count = clk / (16 * sr->speed);
}

unsigned char
uart_io(uart_t *sr, unsigned char rx)
{
 unsigned char data;

 if (!sr->connected)
  {
   return 1;
  }

 //read rx
 if (sr->tcountr)
  {
   sr->tcountr++;

   if (!(sr->tcountr % sr->count))
    {
     if (rx)sr->rxc++;
    }

   if (sr->tcountr >= sr->count << 4)
    {
     sr->tcountr = 1;


     if (sr->rxc > 7)
      {
       sr->insr = (sr->insr >> 1) | 0x80;
      }
     else
      {
       sr->insr = (sr->insr >> 1) & 0xF7FF;
      }
     sr->bcr++;
     sr->rxc = 0;

     if (sr->bcr == 8)//start+eight bits+ stop
      {
       //dprintf ("uart byte in 0x%02X  out 0x%02X\n", sr->insr & 0xFF, sr->outsr >> 8);
       //printf ("%c  0x%04X\n", sr->insr >> 1, sr->insr >> 1);

       serial_port_send (sr->serialfd, (sr->insr >> 1)&0xFF);

      }
    }

   //stop bit
   if ((sr->bcr > 7)&&(sr->arx == 0)&&(rx == 1))//rising edge
    {
     sr->tcountr = 0;
     sr->bcr = 0;
    }

  }
 else
  {
   //start bit
   if ((sr->arx == 1)&&(rx == 0))//falling edge
    {
     //dprintf ("uart start bit \n");
     sr->tcountr = 1;
     sr->bcr = 0;
     sr->insr = 0;
     sr->rxc = 0;
     sr->leds |= 0x01;
    }
  }

 sr->arx = rx;


 //write tx

 sr->tcountw++;

 if (sr->tcountw >= (sr->count << 4))
  {
   sr->tcountw = 0;

   if (!sr->bcw)
    {
     if (serial_port_rec (sr->serialfd, &data))
      {
       dprintf ("uart data rec %c \n", data);
       sr->leds |= 0x02;
       sr->bcw = 1;
       sr->outsr = (data<<1)|0x200;
      }


    }
   else
    {
       sr->outsr = (sr->outsr >> 1);
       sr->bcw++;
       if(sr->bcw >9)
        {
          sr->bcw=0;
        }
    }

  }



 return (sr->outsr & 0x01);
}

void
uart_set_port(uart_t *sr, const char * port, unsigned int speed)
{

 if (sr->connected)
  {
   serial_port_close (sr->serialfd);
   sr->connected = 0;
  }

 if (serial_port_open (&sr->serialfd, port))
  {
   sr->connected = 1;
   sr->speed = speed;
   serial_port_cfg (sr->serialfd, speed);
   dprintf ("uart serial open: %s  speed %i\n", port, speed);
  }
 else
  {
   sr->connected = 0;
   dprintf ("uart serial error open: %s !!!\n", port);
  }


}