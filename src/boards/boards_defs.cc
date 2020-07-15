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
#include"../picsimlab1.h"

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
   mprint (lxT ("Invalid Board! Using Default!\n"));
   *lab = 1; //default  
   *lab_ = 1; //default  
   pboard = new cboard_1 ();
  }

 return pboard;
}

void
board_register(int num, const char * name, board_create_func bcreate)
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
   if (num > boards_list[i-1].num)
    {
     break;
    }
   else
    {
     in=i-1;
     memcpy ((void *) &boards_list[i], (void *) &boards_list[i-1], sizeof (board_desc));
    }
  }
 
 //insert new
 boards_list[in].num = num;
 boards_list[in].bcreate = bcreate;
 strncpy (boards_list[in].name, name, 30);

 BOARDS_LAST++;
}