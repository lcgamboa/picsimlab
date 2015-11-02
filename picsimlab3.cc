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

#include"picsimlab3.h"
#include"picsimlab3_d.cc"

#include"picsimlab1.h"

CPWindow3 Window3;

extern char PROGDEVICE[100];

extern char SERIALDEVICE[100];


//Implementation

void
CPWindow3::_EvOnCreate(CControl * control)
{
  combo1.SetText(String::FromAscii(SERIALDEVICE));
  combo2.SetText(String::FromAscii(PROGDEVICE));
};


void
CPWindow3::button1_EvMouseButtonClick(CControl * control, uint button, uint x, uint y,uint state)
{

  if(combo1.GetText()==combo2.GetText())
  {
    Message(wxT("Use diferent ports!"));
    return;
  }	

  strcpy(SERIALDEVICE,(char*)combo1.GetText().char_str());
  strcpy(PROGDEVICE,(char*)combo2.GetText().char_str());
  
  Window1._EvOnDestroy(control);
  Window1._EvOnCreate(control);

  WDestroy();
};






