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

#include "parts_defs.h"

part_desc parts_list[MAX_PARTS];

int NUM_PARTS = 0;

// boards object creation

part* create_part(lxString name, unsigned int x, unsigned int y) {
    part* part_ = NULL;

    for (int i = 0; i < NUM_PARTS; i++) {
        if (name.compare(parts_list[i].name) == 0) {
            part_ = parts_list[i].pcreate(x, y);
            break;
        }
    }

    return part_;
}

void part_register(const char* name, part_create_func pcreate, const char* menu) {
    int in;

    if (NUM_PARTS == MAX_PARTS) {
        printf("Number of parts greater than MAX_PARTS!\n");
        exit(-1);
    }

    // insert in ascendent order
    in = NUM_PARTS;
    for (int i = NUM_PARTS; i > 0; i--) {
        if (strcmp(name, parts_list[i - 1].name) > 0) {
            break;
        } else {
            in = i - 1;
            memcpy((void*)&parts_list[i], (void*)&parts_list[i - 1], sizeof(part_desc));
        }
    }

    // insert new
    parts_list[in].pcreate = pcreate;
    strncpy(parts_list[in].name, name, 30);
    strncpy(parts_list[in].menu, menu, 30);

    NUM_PARTS++;
}