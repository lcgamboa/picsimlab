/* ########################################################################

   PICsimLab - PIC laboratory simulator

   ########################################################################

   Copyright (c) : 2010-2015  Luis Claudio Gambôa Lopes

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


//includes of boards
#include"board_0.h"
#include"board_1.h" 
#include"board_2.h"
#include"board_3.h"
#include"board_4.h"
#include"board_5.h"
#ifdef _EXPERIMENTAL_
#include"board_6.h"
#include"board_7.h"
#include"board_x.h"
//number of last board
#define BOARDS_LAST 9
#else
//number of last board
#define BOARDS_LAST 6
#endif

//boards object creation
board * create_board(int *lab,int *lab_);

extern const char boards_list[BOARDS_LAST][30];

#endif	/* BOARDS_DEFS_H */

