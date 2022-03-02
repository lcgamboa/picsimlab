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

#ifndef CPWINDOW3
#define CPWINDOW3

#include <lxrad.h>

class CPWindow3 : public CPWindow {
public:
    /*#Controls*/
    CLabel label1;
    CLabel label2;
    CButton button1;
    CCombo combo1;
    CCombo combo2;
    CLabel label3;
    CCombo combo3;
    CLabel label4;
    CSpin spin1;
    CSpin spin2;
    CLabel label5;
    CButton button2;
    /*#Events*/
    void _EvOnCreate(CControl* control);
    void _EvOnShow(CControl* control);
    void button1_EvMouseButtonClick(CControl* control, const uint button, const uint x, const uint y, const uint state);
    void button2_EvMouseButtonClick(CControl* control, const uint button, const uint x, const uint y, const uint state);
    /*#Others*/
    CPWindow3(void);
};

extern CPWindow3 Window3;

#endif /*#CPWINDOW3*/
