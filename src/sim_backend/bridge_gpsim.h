/* ########################################################################

   PICSimLab - Programmable IC Simulator Laboratory

   ########################################################################

   Copyright (c) : 2020-2026  Luis Claudio Gambôa Lopes <lcgamboa@yahoo.com>

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

#ifndef BRIGE_GPSIM_H
#define BRIGE_GPSIM_H

#ifdef __cplusplus
extern "C" {
#endif

int bridge_gpsim_init(const char* processor, const char* fileName, float freq);
void bridge_gpsim_reset(void);
unsigned char bridge_gpsim_get_pin_count(void);
const char* bridge_gpsim_get_pin_name(int pin);
unsigned char bridge_gpsim_get_pin_value(int pin);
unsigned char bridge_gpsim_get_pin_dir(int pin);
void bridge_gpsim_set_pin_value(int pin, unsigned char value);
void bridge_gpsim_set_apin_value(int pin, float value);
void bridge_gpsim_set_frequency(double freq);
void bridge_gpsim_step(void);
void bridge_gpsim_end(void);
int bridge_gpsim_dump_memory(const char* fname);
char* bridge_gpsim_get_processor_list(char* buff, unsigned int size);

#ifdef __cplusplus
}
#endif

#endif /* BRIGE_GPSIM_H */
