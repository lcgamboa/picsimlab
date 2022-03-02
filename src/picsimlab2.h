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

#ifndef CPWINDOW2
#define CPWINDOW2

#include <lxrad.h>

class CPWindow2 : public CPWindow {
public:
#ifndef EXT_BROWSER
    /*#Controls*/
    CHtml html1;
    CButton button1;
    /*#Events*/
    void _EvOnShow(CControl* control);
    void button1_EvMouseButtonClick(CControl* control, const uint button, const uint x, const uint y, const uint state);
#endif
    /*#Others*/
    CPWindow2(void);
};

extern CPWindow2 Window2;

#endif /*#CPWINDOW2*/
