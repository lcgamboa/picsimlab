/* ########################################################################

   PICSimLab - Programmable IC Simulator Laboratory

   ########################################################################

   Copyright (c) : 2025-2026  Luis Claudio Gambôa Lopes <lcgamboa@yahoo.com>

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
   
#ifndef CPWINDOW2 
#define CPWINDOW2

#include<lxrad.h>

class CPWindow2:public CPWindow
{
  public:
//lxrad automatic generated block start, don't edit below!
  /*#Controls*/
#ifndef EXT_BROWSER
  CHtml html1;
  /*#Events*/
  void _EvOnShow(CControl * control);
#endif
  /*#Others*/
  CPWindow2(void);
//lxrad automatic generated block end, don't edit above!

};

extern CPWindow2 Window2 ;

#endif /*#CPWINDOW2*/

