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

#include "boards_defs.h"

board_desc boards_list[BOARDS_MAX];

int BOARDS_LAST = 0;

//boards object creation

board *
create_board(int *lab, int *lab_)
{
 board * pboard = NULL;


 if ((*lab >= 0)&&(*lab < BOARDS_LAST))
  {
   pboard = boards_list[*lab].bcreate ();
  }
 else
  {
   mprint (lxT ("PICSimLab: Invalid board! Using default!\n"));
   *lab = 0; //default  
   *lab_ = 0; //default  
   pboard = boards_list[0].bcreate ();
  }

 return pboard;
}

void
board_register(const char * name, board_create_func bcreate)
{
 int in;

 if (BOARDS_LAST == BOARDS_MAX)
  {
   printf ("Number of boards greater than BOARDS_MAX!\n");
   exit (-1);
  }

 //insert in ascendent order
 in = BOARDS_LAST;
 for (int i = BOARDS_LAST; i > 0; i--)
  {
   if ( strcmp(name, boards_list[i - 1].name) > 0)
    {
     break;
    }
   else
    {
     in = i - 1;
     memcpy ((void *) &boards_list[i], (void *) &boards_list[i - 1], sizeof (board_desc));
    }
  }

 //insert new
 boards_list[in].bcreate = bcreate;
 strncpy (boards_list[in].name, name, 30);

 for (unsigned int i = 0; i <= strlen (name); i++)
  {
   if (name[i] != ' ')
    {
     boards_list[in].name_[i] = name[i];
    }
   else
    {
     boards_list[in].name_[i] = '_';
    }
  }

 BOARDS_LAST++;
}