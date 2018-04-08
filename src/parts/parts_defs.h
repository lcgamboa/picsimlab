/* ########################################################################

   PICsimLab - PIC laboratory simulator

   ########################################################################

   Copyright (c) : 2010-2017  Luis Claudio Gambôa Lopes

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

#ifndef PARTS_DEFS_H
#define	PARTS_DEFS_H

#include"part.h" 

#define MAX_PARTS 100

#ifdef _EXPERIMENTAL_
#define NUM_PARTS 15
#else
#define NUM_PARTS 8
#endif

extern const char parts_list[NUM_PARTS][30];

part * create_part(String name, unsigned int x, unsigned int y);

#endif	/* PARTS_DEFS_H */

