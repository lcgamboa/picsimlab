/* ########################################################################

   PICsimLab - PIC laboratory simulator

   ########################################################################

   Copyright (c) : 2020-2023  Luis Claudio Gambôa Lopes

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

#ifndef RCONTROL_H
#define RCONTROL_H

/* Output defined types
 LD - led
 DS - Display LCD
 MT - DC motor
 DG - step and servo angles
 SS - seven sgments
 VT - virtual term
 */

/* Input defined types
 VS - value short
 PB - push button
 */

// PICSimLab remote control
int rcontrol_init(const unsigned short tcpport, const int reporterror = 0);
int rcontrol_loop(void);
void rcontrol_end(void);
void rcontrol_server_end(void);

#endif /* RCONTROL_H */
