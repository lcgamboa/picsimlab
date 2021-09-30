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


#ifndef BITBANG_UART
#define BITBANG_UART

#include <stdio.h>


typedef struct {
    unsigned char prx;          //previous rx value
    unsigned short insr;        //input shift register  
    unsigned short outsr;       //output shift register
    unsigned int bcr;           //bit counter read
    unsigned long tcountr;      //clock counter read
    unsigned int bcw;           //bit counter write 
    unsigned long tcountw;      //clock counter write
    unsigned long speed;        //serial speed (baud)
    unsigned int cycle_count;   //speed /clock cycle counter         
    unsigned int rxc;           //rc clock counter
    unsigned int leds;          //rx tx leds status 
    unsigned char datar;        //data received 
    unsigned char data_recv;    //flag data received 
    unsigned char dataw;        //data to write
    unsigned char data_to_send; //flag data to write
    unsigned long freq;
} bitbang_uart_t;


void bitbang_uart_rst(bitbang_uart_t *bu);
void bitbang_uart_init(bitbang_uart_t *bu);
void bitbang_uart_end(bitbang_uart_t *bu);
void bitbang_uart_set_clk_freq(bitbang_uart_t *bu, const unsigned long freq);
void bitbang_uart_set_speed(bitbang_uart_t *bu, const unsigned int speed);
unsigned char  bitbang_uart_transmitting(bitbang_uart_t *bu);
void bitbang_uart_send(bitbang_uart_t *bu, const unsigned char data);
unsigned char  bitbang_uart_data_available(bitbang_uart_t *bu);
unsigned char  bitbang_uart_recv(bitbang_uart_t *bu);

unsigned char bitbang_uart_io(bitbang_uart_t *bu, const unsigned char rx);

#endif //BITBANG_UART

