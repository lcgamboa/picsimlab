/* ########################################################################

   PICsimLab - PIC laboratory simulator

   ########################################################################

   Copyright (c) : 2010-2022  Luis Claudio Gambôa Lopes

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
#define PARTS_DEFS_H

#include "part.h"

#define MAX_PARTS 100

extern int NUM_PARTS;

#define part_init(name, function, menu)                                                            \
    static part* function##_create(unsigned int x, unsigned int y) { return new function(x, y); }; \
    static void __attribute__((constructor)) function##_init(void);                                \
    static void function##_init(void) { part_register(name, function##_create, menu); }

typedef part* (*part_create_func)(unsigned int x, unsigned int y);

void part_register(const char* name, part_create_func pcreate, const char* menu);

part* create_part(lxString name, unsigned int x, unsigned int y);

typedef struct {
    char name[30];
    part_create_func pcreate;
    char menu[30];
} part_desc;

extern part_desc parts_list[MAX_PARTS];

#endif /* PARTS_DEFS_H */
