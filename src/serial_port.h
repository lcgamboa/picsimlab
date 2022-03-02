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

#ifndef SERIAL_PORT_H
#define SERIAL_PORT_H

#ifdef _WIN_
#include <windows.h>
#define serialfd_t HANDLE
#else
#define serialfd_t int
#endif

unsigned long serial_port_send(serialfd_t serialfd, unsigned char c);
unsigned long serial_port_rec(serialfd_t serialfd, unsigned char* c);
int serial_port_get_dsr(serialfd_t serialfd);
int serial_port_open(serialfd_t* serialfd, const char* SERIALDEVICE);
int serial_port_cfg(serialfd_t serialfd, float serialexbaud);
int serial_port_close(serialfd_t serialfd);
char* serial_port_list(void);

#endif /* SERIAL_PORT_H */
