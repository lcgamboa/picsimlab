/* ########################################################################

   PICSimLab - Programmable IC Simulator Laboratory

   ########################################################################

   Copyright (c) : 2010-2026 Luis Claudio Gambôa Lopes <lcgamboa@yahoo.com>

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

#ifndef CPWINDOW6
#define CPWINDOW6

#include <lxrad.h>

class CPWindow6 : public CPWindow {
public:
    // lxrad automatic generated block start, don't edit below!
    /*#Controls*/
    CLabel label1;
    CLabel label2;
    CCombo combo1;
    CLabel label3;
    CLabel label4;
    CCombo combo2;
    CLabel label5;
    CCombo combo3;
    CButton button1;
    CButton button2;
    CButton button3;
    CLabel label6;
    CEdit edit1;
    CDirDialog dirdialog1;
    CFileDialog filedialog1;
    /*#Events*/
    void _EvOnCreate(CControl* control);
    void _EvOnShow(CControl* control);
    void button1_EvMouseButtonClick(CControl* control, const uint button, const uint x, const uint y, const uint state);
    void button2_EvMouseButtonClick(CControl* control, const uint button, const uint x, const uint y, const uint state);
    void button3_EvMouseButtonClick(CControl* control, const uint button, const uint x, const uint y, const uint state);
    void combo1_EvOnComboChange(CControl* control);
    void combo2_EvOnComboChange(CControl* control);
    void dirdialog1_EvOnClose(const int retId);
    void filedialog1_EvOnClose(const int retId);

    /*#Others*/
    CPWindow6(void);
    // lxrad automatic generated block end, don't edit above!

    int OpenProject(lxString path, lxString type);

private:
    lxString bname;
    lxString pname;
    lxString ide;
    lxString framework;
    lxString ctemplate;
    int run_ide;
};

extern CPWindow6 Window6;

#endif /*#CPWINDOW6*/
