/* ########################################################################

   PICSimLab - Programmable IC Simulator Laboratory

   ########################################################################

   Copyright (c) : 2010-2025  Luis Claudio Gambôa Lopes <lcgamboa@yahoo.com>

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

#ifndef CPWINDOW4
#define CPWINDOW4

#include <lxrad.h>
#include <picsim/picsim.h>
#include "lib/types.h"

/**
 * @brief CPWindow4 class
 *
 * class definition of PICSimLab oscilloscope window.
 */
class CPWindow4 : public CPWindow {
public:
    // lxrad automatic generated block start, don't edit below!
    /*#Controls*/
    CDraw draw1;
    CSpind spind1;
    CSpind spind2;
    CSpind spind3;
    CSpind spind4;
    CSpind spind5;
    CSpind spind6;
    CToggleButton togglebutton1;
    CButton button1;
    CToggleButton togglebutton2;
    CButton button2;
    CToggleButton togglebutton3;
    CToggleButton togglebutton4;
    CColorDialog colordialog1;
    CToggleButton togglebutton5;
    CSpind spind7;
    CCombo combo1;
    CTimer timer1;
    CCombo combo2;
    CCombo combo3;
    CLabel label1;
    CLabel label2;
    CLabel label3;
    CLabel label4;
    CLabel label5;
    CLabel label6;
    CLabel label7;
    CLabel label8;
    CLabel label9;
    CLabel label10;
    CLabel label11;
    CButton button3;
    CToggleButton togglebutton6;
    CToggleButton togglebutton7;
    CButton button4;
    CFileDialog filedialog1;
    CButton button5;
    CButton button6;
    CButton button7;
    CButton button8;
    CButton button9;
    /*#Events*/
    void _EvOnCreate(CControl* control);
    void _EvOnDestroy(CControl* control);
    void _EvOnShow(CControl* control);
    void _EvOnHide(CControl* control);
    void draw1_EvMouseButtonClick(CControl* control, const unsigned int button, const unsigned int x,
                                  const unsigned int y, const unsigned int state);
    void spind5_EvOnChangeSpinDouble(CControl* control);
    void spind6_EvOnChangeSpinDouble(CControl* control);
    void button1_EvMouseButtonClick(CControl* control, const unsigned int button, const unsigned int x,
                                    const unsigned int y, const unsigned int state);
    void button2_EvMouseButtonClick(CControl* control, const unsigned int button, const unsigned int x,
                                    const unsigned int y, const unsigned int state);
    void togglebutton5_EvOnToggleButton(CControl* control);
    void spind7_EvOnChangeSpinDouble(CControl* control);
    void timer1_EvOnTime(CControl* control);
    void combo2_EvOnComboChange(CControl* control);
    void combo3_EvOnComboChange(CControl* control);
    void button3_EvMouseButtonClick(CControl* control, const unsigned int button, const unsigned int x,
                                    const unsigned int y, const unsigned int state);
    void togglebutton6_EvOnToggleButton(CControl* control);
    void button4_EvMouseButtonClick(CControl* control, const unsigned int button, const unsigned int x,
                                    const unsigned int y, const unsigned int state);
    void filedialog1_EvOnClose(const int retId);
    void colordialog1_EvOnClose(const int retId);
    void button5_EvMouseButtonPress(CControl* control, const unsigned int button, const unsigned int x,
                                    const unsigned int y, const unsigned int state);
    void button6_EvMouseButtonPress(CControl* control, const unsigned int button, const unsigned int x,
                                    const unsigned int y, const unsigned int state);
    void button7_EvMouseButtonPress(CControl* control, const unsigned int button, const unsigned int x,
                                    const unsigned int y, const unsigned int state);
    void button8_EvMouseButtonPress(CControl* control, const unsigned int button, const unsigned int x,
                                    const unsigned int y, const unsigned int state);
    void button9_EvMouseButtonPress(CControl* control, const unsigned int button, const unsigned int x,
                                    const unsigned int y, const unsigned int state);

    /*#Others*/
    // lxrad automatic generated block end, don't edit above!
    CPWindow4(void);
    void DrawScreen(void);
    static int OnWindowCmd(const int id, const char* ControlName, const PICSimLabWindowAction action, const char* Value,
                           void* ReturnBuff);

private:
    CButton* ctrl;
    int update_pos;
};

extern CPWindow4 Window4;

#endif /*#CPWINDOW4*/
