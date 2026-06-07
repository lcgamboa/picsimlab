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

// Project Wizard

#include "picsimlab6.h"
#include "picsimlab6_d.cc"
#include "picsimlab6_data.h"

#include "lib/picsimlab.h"

CPWindow6 Window6;

// Implementation

void CPWindow6::_EvOnCreate(CControl* control) {
    SetTitle(((PICSimLab.GetInstanceNumber() > 0)
                  ? ("PICSimLab[" + std::to_string(PICSimLab.GetInstanceNumber()) + "] - ")
                  : ("PICSimLab - ")) +
             "Project Wizard");
}

void CPWindow6::_EvOnShow(CControl* control) {
    bname = PICSimLab.GetBoard()->GetName();
    pname = PICSimLab.GetBoard()->GetProcessorName();

    label1.SetText("Board: " + bname);
    label2.SetText("Microcontroller: " + pname);

    combo1.SetItems(PICSimLab.GetBoard()->GetSupportedIDEs());
    combo1.SetText(combo1.GetItem(0));
    combo1_EvOnComboChange(NULL);

    ctemplate = combo3.GetText();

    if (ctemplate.compare("N/A")) {
        combo3.SetEnable(1);
        edit1.SetEnable(1);
        button1.SetEnable(1);
        button2.SetEnable(1);
    } else {
        combo3.SetEnable(0);
        edit1.SetEnable(0);
        button1.SetEnable(0);
        button2.SetEnable(0);
    }

    combo3.SetEnable((combo3.GetItemsCount() == 1) ? 0 : 1);
}

void CPWindow6::button1_EvMouseButtonClick(CControl* control, const uint button, const uint x, const uint y,
                                           const uint state) {
    dirdialog1.SetType(lxFD_SAVE | lxFD_CHANGE_DIR);
    run_ide = 0;
    dirdialog1.Run();
}

void CPWindow6::button2_EvMouseButtonClick(CControl* control, const uint button, const uint x, const uint y,
                                           const uint state) {
    dirdialog1.SetType(lxFD_SAVE | lxFD_CHANGE_DIR);
    run_ide = 1;
    dirdialog1.Run();
}

void CPWindow6::button3_EvMouseButtonClick(CControl* control, const uint button, const uint x, const uint y,
                                           const uint state) {
    WDestroy();
}

void CPWindow6::combo1_EvOnComboChange(CControl* control) {
    ide = combo1.GetText();

    if (!ide.compare("PlatformIO IDE for VSCode")) {
        combo2.SetItems("Arduino,");
    } else {
        combo2.SetItems("N/A,");
    }

    if (!ide.compare("N/A")) {
        combo1.SetEnable(0);
        combo2.SetEnable(0);
        combo3.SetEnable(0);
    } else {
        combo1.SetEnable(1);
        combo2.SetEnable(1);
        combo3.SetEnable(1);
    }

    combo1.SetEnable((combo1.GetItemsCount() == 1) ? 0 : 1);

    combo2.SetText(combo2.GetItem(0));
    combo2_EvOnComboChange(NULL);
}

void CPWindow6::combo2_EvOnComboChange(CControl* control) {
    framework = combo2.GetText();

    combo3.SetItems("N/A,");

    if (!ide.compare("PlatformIO IDE for VSCode")) {
        if (!framework.compare("Arduino")) {
            combo3.SetItems("Blink,");
        }
    }

    if (!framework.compare("N/A")) {
        combo2.SetEnable(0);
        combo3.SetEnable(0);
    } else {
        combo2.SetEnable(1);
        combo3.SetEnable(1);
    }

    combo2.SetEnable((combo2.GetItemsCount() == 1) ? 0 : 1);

    combo3.SetText(combo3.GetItem(0));
}

void CPWindow6::dirdialog1_EvOnClose(int retId) {
    if (retId) {
        std::string prjdir = (const char*)dirdialog1.GetDirName().c_str();
        prjdir += std::string("/") + edit1.GetText() + std::string("/");

        if (PICSimLab.SystemCmd(PSC_DIREXISTS, prjdir.c_str())) {
            printf("Existe (%s)\n", (const char*)prjdir.c_str());
            PICSimLab.RegisterError(std::string("Project dir ") + prjdir + " already exists!");
            return;
        } else {
            if (PICSimLab.SystemCmd(PSC_CREATEDIR, prjdir.c_str()) == 0) {
                std::string fzip = PICSimLab.GetSharePath() + "prj_wizard/platformio.zip";
                PICSimLab.SystemCmd(PSC_UNZIPDIR, fzip.c_str(), (void*)prjdir.c_str());

                FILE* fmain = fopen_UTF8((prjdir + "src/main.cpp").c_str(), "w");
                fprintf(fmain, blink_code, "13");
                fclose(fmain);

                FILE* fpio = fopen_UTF8((prjdir + "platformio.ini").c_str(), "w");
#ifdef _WIN_
                fprintf(fpio, platformio_ini, "Windows");
#else
                fprintf(fpio, platformio_ini, "Linux");
#endif
                fclose(fpio);

                if (run_ide) {
// FIXME use configurable IDE path
#ifdef _WIN_
                    lxExecute((std::string("C:\\Program Files\\Microsoft VS Code\\Code.exe ") + prjdir).c_str());
#else
                    lxExecute((std::string("/usr/bin/code ") + prjdir).c_str());
#endif
                }
            } else {
                PICSimLab.RegisterError(std::string("Project dir ") + prjdir + " can't be created!");
                return;
            }
        }
    }
    WDestroy();
}