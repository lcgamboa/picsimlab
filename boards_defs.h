/* ########################################################################

   PICsimLab - PIC laboratory simulator

   ########################################################################

   Copyright (c) : 2015  Luis Claudio Gambôa Lopes

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

#ifndef BOARDS_DEFS_H
#define	BOARDS_DEFS_H

#include"picsimlab1.h"

//includes of boards
#include"board_1.h" 
#include"board_2.h"
#include"board_3.h"
#include"board_4.h"

//number of last board
#define BOARDS_LAST 4

//boards object creation
board * create_board(int *lab,int *lab_)
{
   board * pboard; 
        switch(*lab)
        {
          case 1:
            pboard= new cboard_1();
            break;
          case 2:
            pboard= new cboard_2();
            break;
          case 3:
            pboard= new cboard_3();
            break;
          case 4:
            pboard= new cboard_4();
            break;       
          default:
            mprint(wxT("Invalid Board! Using Default!\n"));
            *lab=1;//default  
            *lab_=1;//default  
            Window1.combo2.SetText(wxT("1"));
            pboard= new cboard_1();
          break;
        }

    return pboard; 
}

#endif	/* BOARDS_DEFS_H */

