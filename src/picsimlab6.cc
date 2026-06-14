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
#include "picsimlab1.h"
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

    edit1.SetText("Untitled");
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
    dirdialog1.SetDirName(PICSimLab.GetBoard()->GetPWActiveProject());  // TODO get platformio default project dir
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
        lxString prjdir = dirdialog1.GetDirName();
        prjdir += lxString("/") + edit1.GetText() + "/";

        if (PICSimLab.SystemCmd(PSC_DIREXISTS, prjdir.utf8_str())) {
            PICSimLab.RegisterError("PICSimLab",
                                    (const char*)(lxString("Project dir ") + prjdir + " already exists!").utf8_str());
            return;
        } else {
            if (PICSimLab.SystemCmd(PSC_CREATEDIR, prjdir.utf8_str()) == 0) {
                lxString fzip = PICSimLab.GetSharePath() + "prj_wizard/platformio.zip";
                PICSimLab.SystemCmd(PSC_UNZIPDIR, fzip.utf8_str(), (void*)((const char*)prjdir.utf8_str()));

                // board selection
                lxString pioboard = "";
                lxString ledpin = "";
                lxString env_extra = "";
                if (!bname.compare("Arduino Uno")) {
                    pioboard = "uno";
                    ledpin = "13";
                    env_extra = "";
                } else if (!bname.compare("Arduino Nano")) {
                    pioboard = "nanoatmega328";
                    ledpin = "13";
                    env_extra = "";
                } else if (!bname.compare("Arduino Mega")) {
                    pioboard = "megaatmega2560";
                    ledpin = "13";
                    env_extra = "";
                } else if (!bname.compare("Franzininho DIY")) {
                    pioboard = "attiny85";
                    ledpin = "1";
                    env_extra = "board_build.f_cpu = 16000000L\nbuild_flags = -DCLOCK_SOURCE=6\n";
                }

                // main
                FILE* fmain = fopen_UTF8((prjdir + "src/main.cpp").utf8_str(), "w");
                if (fmain == NULL) {
                    PICSimLab.RegisterError(
                        "PICSimLab",
                        (const char*)(lxString("File ") + prjdir + "src/main.cpp can't be open!").utf8_str());
                    return;
                }
                fprintf(fmain, blink_code, (const char*)ledpin.c_str());
                fclose(fmain);
                // platformio.ini
                FILE* fpio = fopen_UTF8((prjdir + "platformio.ini").utf8_str(), "w");
                if (fpio == NULL) {
                    PICSimLab.RegisterError(
                        "PICSimLab",
                        (const char*)(lxString("File ") + prjdir + "platformio.ini can't be open!").utf8_str());
                    return;
                }
#ifdef _WIN_
                fprintf(fpio, platformio_ini, "Windows", (const char*)pioboard.c_str(), (const char*)pioboard.c_str(),
                        (const char*)env_extra.c_str(), (const char*)pioboard.c_str());
#else
                fprintf(fpio, platformio_ini, "Linux", (const char*)pioboard.c_str(), (const char*)pioboard.c_str(),
                        (const char*)env_extra.c_str(), (const char*)pioboard.c_str());
#endif
                fclose(fpio);

                PICSimLab.GetBoard()->SetPWActiveProject((const char*)prjdir.utf8_str());
                PICSimLab.GetBoard()->SetPWProjectType((const char*)ide.c_str());
                Window1.menu1_Code_Open_Active_Project.SetEnable(1);

                if (run_ide) {
                    OpenProject(prjdir, ide);
                }

            } else {
                PICSimLab.RegisterError(
                    "PICSimLab", (const char*)(lxString("Project dir ") + prjdir + " can't be created!").utf8_str());
                return;
            }
        }
    }
    WDestroy();
}

void CPWindow6::filedialog1_EvOnClose(const int retId) {
    if (retId) {
        PICSimLab.SetPWVscodePath((const char*)filedialog1.GetFileName().utf8_str());
        OpenProject(PICSimLab.GetBoard()->GetPWActiveProject(), PICSimLab.GetBoard()->GetPWProjectType());
    }
}

int CPWindow6::OpenProject(lxString path, lxString type) {
    if (!type.compare("PlatformIO IDE for VSCode")) {
        char vscode_path[1024];

        if (!PICSimLab.SystemCmd(PSC_DIREXISTS, path.utf8_str())) {
            PICSimLab.RegisterError("PICSimLab", (const char*)("Project dir not found!\n" + path).utf8_str());
            PICSimLab.GetBoard()->SetPWActiveProject(" ");
            Window1.menu1_Code_Open_Active_Project.SetEnable(0);
            return 1;
        }

        strncpy(vscode_path, PICSimLab.GetPWVscodePath().c_str(), 1023);

        if (!PICSimLab.SystemCmd(PSC_FILEEXISTS, vscode_path)) {
#ifdef _WIN_

            lxString vscode_user_path = "";
            strncpy(vscode_path, (const char*)lxGetUserDataDir("picsimlab"), 1023);
            printf("%s\n", (const char*)vscode_path);
            char* ptr = strstr(vscode_path, "AppData\\");
            if (ptr) {
                ptr[8] = 0;
                vscode_user_path = (lxString(vscode_path) + "Local\\Programs\\Microsoft VS Code\\Code.exe").utf8_str();
            }

            if (PICSimLab.SystemCmd(PSC_FILEEXISTS, (const char*)vscode_user_path.utf8_str())) {
                strncpy(vscode_path, vscode_user_path.utf8_str(), 1023);
            } else if (PICSimLab.SystemCmd(PSC_FILEEXISTS, "C:\\Program Files\\Microsoft VS Code\\Code.exe")) {
                strncpy(vscode_path, "C:\\Program Files\\Microsoft VS Code\\Code.exe", 1023);
            } else {
                if (Dialog_sz("VS code executable not found!\n Search on disk?", 400, 200)) {
                    filedialog1.SetFileName(vscode_path);
                    filedialog1.SetFilter(lxT("All Files (*.exe)|*.exe"));
                    filedialog1.Run();
                }
                return 1;
            }
#else

            if (PICSimLab.SystemCmd(PSC_FILEEXISTS, "/usr/bin/code")) {
                strncpy(vscode_path, "/usr/bin/code", 1023);
            } else if (PICSimLab.SystemCmd(PSC_FILEEXISTS, "/snap/bin/code")) {
                strncpy(vscode_path, "/snap/bin/code", 1023);
            } else {
                if (Dialog_sz("VS code executable not found!\n Search on disk?", 400, 200)) {
                    filedialog1.SetFileName(vscode_path);
                    filedialog1.SetFilter(lxT("All Files (*)|*"));
                    filedialog1.Run();
                }
                return 1;
            }

#endif
            PICSimLab.SetPWVscodePath(vscode_path);
        }

        printf("PICSimLab: Open project [%s]\n", (const char*)(lxString(vscode_path) + " \"" + path + "\"").utf8_str());
        lxExecute((lxString(vscode_path) + " \"" + path + "\""));

        return 0;
    } else {
        PICSimLab.RegisterError("PICSimLab", "Project type not supported!");
        Window1.menu1_Code_Open_Active_Project.SetEnable(0);
        Window1.menu1_Code_Project_Wizard.SetEnable(0);
    }
    return -1;
}
