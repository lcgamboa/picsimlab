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

// Configuration window

#include "picsimlab3.h"
#include "picsimlab3_d.cc"

#include "lib/picsimlab.h"
#include "lib/serial_port.h"
#include "picsimlab1.h"

CPWindow3 Window3;

#ifdef _USE_PICSTARTP_
extern char PROGDEVICE[100];
#endif

extern char SERIALDEVICE[100];

// Implementation

void CPWindow3::_EvOnCreate(CControl* control) {
    combo1.SetText(std::string(SERIALDEVICE));
#ifdef _USE_PICSTARTP_
    combo2.SetText(std::string(PROGDEVICE));
    combo2.SetVisible(true);
    label2.SetVisible(true);
#else
    combo2.SetVisible(false);
    label2.SetVisible(false);
#endif
}

void CPWindow3::button1_EvMouseButtonClick(CControl* control, unsigned int button, unsigned int x, unsigned int y,
                                           unsigned int state) {
    int osc_on = PICSimLab.GetBoard()->GetUseOscilloscope();
    int spare_on = PICSimLab.GetBoard()->GetUseSpareParts();

    PICSimLab.SetDebugType(combo3.GetText().compare("GDB") == 0);

    PICSimLab.SetDebugPort(spin1.GetValue());

    if (PICSimLab.GetDebugStatus()) {
        Window1.togglebutton1.SetCheck(0);
        Window1.togglebutton1_EvOnToggleButton(NULL);
    }

    PICSimLab.SetRemotecPort(spin2.GetValue());

#ifdef _USE_PICSTARTP_
    if (combo1.GetText() == combo2.GetText()) {
        Message_sz("Use diferent ports!", , 400, 200);
        return;
    }
#endif
    strcpy(SERIALDEVICE, (const char*)combo1.GetText().c_str());
#ifdef _USE_PICSTARTP_
    strcpy(PROGDEVICE, (const char*)combo2.GetText().c_str());
#endif

    PICSimLab.SetUseDSRReset(checkbox1.GetCheck());

    PICSimLab.EndSimulation();
    PICSimLab.Configure(PICSimLab.GetHomePath().c_str());

    if (osc_on)
        Window1.menu1_Modules_Oscilloscope_EvMenuActive(this);
    if (spare_on)
        Window1.menu1_Modules_Spareparts_EvMenuActive(this);

    PICSimLab.GetBoard()->EvOnShow();

    WDestroy();
}

void CPWindow3::button2_EvMouseButtonClick(CControl* control, unsigned int button, unsigned int x, unsigned int y,
                                           unsigned int state) {
    WDestroy();
}

void CPWindow3::button3_EvMouseButtonClick(CControl* control, unsigned int button, unsigned int x, unsigned int y,
                                           unsigned int state) {
#ifdef _WIN_
    std::string cmd = "explorer ";
#else
    std::string cmd = "xdg-open ";
#endif
    cmd += PICSimLab.GetHomePath();
    lxExecute(cmd);
}

void CPWindow3::_EvOnShow(CControl* control) {
    char* resp = serial_port_list();

    if (resp) {
        std::string temp;
        temp = combo1.GetText();
        combo1.SetItems(resp);
        combo1.SetText(temp);

        temp = combo2.GetText();
        combo2.SetItems(resp);
        combo2.SetText(temp);

        free(resp);
    } else {
        printf("No serial ports found!\n");
    }

    if (PICSimLab.GetDebugType()) {
        combo3.SetText("GDB");
    } else {
        combo3.SetText("MDB");
    }

    spin1.SetValue(PICSimLab.GetDebugPort());

    spin2.SetValue(PICSimLab.GetRemotecPort());

    checkbox1.SetCheck(PICSimLab.GetUseDSRReset());

    if (PICSimLab.GetInstanceNumber()) {
        spin1.SetEnable(0);
        spin2.SetEnable(0);
    }
}
