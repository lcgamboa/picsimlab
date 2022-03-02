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

// Help window

#include "picsimlab2.h"
#include "picsimlab2_d.cc"

CPWindow2 Window2;

// Implementation
#ifndef EXT_BROWSER

void CPWindow2::_EvOnShow(CControl* control) {
    html1.SetWidth(Window2.GetWidth() - 30);
    html1.SetHeight(Window2.GetHeight() - 82);
}

void CPWindow2::button1_EvMouseButtonClick(CControl* control, uint button, uint x, uint y, uint state) {
    html1.HistoryBack();
}

#endif
