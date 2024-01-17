/* ########################################################################

   PICSimLab - Programmable IC Simulator Laboratory

   ########################################################################

   Copyright (c) : 2010-2023  Luis Claudio Gambôa Lopes <lcgamboa@yahoo.com>

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

/** @mainpage notitle
 *
 * @section intro_sec Introduction
 *
 * This is the documentation to explain the use of picsimlab internal classes.
 *
 * The main objective of this domentation is to enable the expansion of the
 * simulator with the creation of new boards (@ref board) and parts (@ref part).
 *
 * @section partex Part example
 * @subsection pinc output_step.h
 * @include output_step.h
 * @subsection pcode output_step.cc
 * @include output_step.cc
 *
 * @section boardex Board example
 * @subsection binc board_x.h
 * @include board_x.h
 * @subsection bcode board_x.cc
 * @include board_x.cc
 *
 */

#ifndef CPWINDOW1
#define CPWINDOW1

#include <lxrad.h>
#include "lib/board.h"
#include "lib/picsimlab.h"

#define MAX_MIC 140

/**
 * @brief CPWindow1 class
 *
 * class definition of PICSimLab main window (Board).
 */
class CPWindow1 : public CPWindow {
public:
    // lxrad automatic generated block start, don't edit below!
    /*#Controls*/
    CTimer timer1;
    CTimer timer2;
    CFileDialog filedialog1;
    CDraw draw1;
    CCombo combo1;
    CLabel label1;
    CLabel label2;
    CMenu menu1;
    CPMenu menu1_File;
    CPMenu menu1_Board;
    CPMenu menu1_Microcontroller;
    CPMenu menu1_Modules;
    CPMenu menu1_Tools;
    CPMenu menu1_Help;
    CItemMenu menu1_File_LoadHex;
    CItemMenu menu1_File_ReloadLast;
    CItemMenu menu1_File_SaveHex;
    CItemMenu menu1_File_Configure;
    CItemMenu menu1_File_SaveWorkspace;
    CItemMenu menu1_File_LoadWorkspace;
    CItemMenu menu1_File_LoadBoardDemo;
    CItemMenu menu1_File_Exit;
    CItemMenu menu1_Modules_Oscilloscope;
    CItemMenu menu1_Modules_Spareparts;
    CItemMenu menu1_Tools_SerialTerm;
    CItemMenu menu1_Tools_SerialRemoteTank;
    CItemMenu menu1_Tools_Esp8266ModemSimulator;
    CItemMenu menu1_Tools_ArduinoBootloader;
    CItemMenu menu1_Tools_MPLABXDebuggerPlugin;
    CItemMenu menu1_Tools_PinViewer;
    CItemMenu menu1_Help_Contents;
    CItemMenu menu1_Help_Board;
    CItemMenu menu1_Help_Examples;
    CItemMenu menu1_Help_About_Board;
    CItemMenu menu1_Help_About_PICSimLab;
    CStatusbar statusbar1;
    CToggleButton togglebutton1;
    CFileDialog filedialog2;
    /*#Events*/
    void _EvOnCreate(CControl* control);
    void _EvOnDestroy(CControl* control);
    void _EvOnShow(CControl* control);
    void _EvOnDropFile(CControl* control, const std::string fname);
    void timer1_EvOnTime(CControl* control);
    void timer2_EvOnTime(CControl* control);
    void filedialog1_EvOnClose(const int retId);
    void draw1_EvMouseMove(CControl* control, const uint button, const uint x, const uint y, const uint state);
    void draw1_EvMouseButtonPress(CControl* control, const uint button, const uint x, const uint y, const uint state);
    void draw1_EvMouseButtonRelease(CControl* control, const uint button, const uint x, const uint y, const uint state);
    void draw1_EvKeyboardPress(CControl* control, const uint key, const uint hkey, const uint mask);
    void draw1_EvKeyboardRelease(CControl* control, const uint key, const uint hkey, const uint mask);
    void combo1_EvOnComboChange(CControl* control);
    void menu1_File_LoadHex_EvMenuActive(CControl* control);
    void menu1_File_ReloadLast_EvMenuActive(CControl* control);
    void menu1_File_SaveHex_EvMenuActive(CControl* control);
    void menu1_File_Configure_EvMenuActive(CControl* control);
    void menu1_File_SaveWorkspace_EvMenuActive(CControl* control);
    void menu1_File_LoadWorkspace_EvMenuActive(CControl* control);
    void menu1_File_LoadBoardDemo_EvMenuActive(CControl* control);
    void menu1_File_Exit_EvMenuActive(CControl* control);
    void menu1_Modules_Oscilloscope_EvMenuActive(CControl* control);
    void menu1_Modules_Spareparts_EvMenuActive(CControl* control);
    void menu1_Tools_SerialTerm_EvMenuActive(CControl* control);
    void menu1_Tools_SerialRemoteTank_EvMenuActive(CControl* control);
    void menu1_Tools_Esp8266ModemSimulator_EvMenuActive(CControl* control);
    void menu1_Tools_ArduinoBootloader_EvMenuActive(CControl* control);
    void menu1_Tools_MPLABXDebuggerPlugin_EvMenuActive(CControl* control);
    void menu1_Tools_PinViewer_EvMenuActive(CControl* control);
    void menu1_Help_Contents_EvMenuActive(CControl* control);
    void menu1_Help_Examples_EvMenuActive(CControl* control);
    void menu1_Help_Board_EvMenuActive(CControl* control);
    void menu1_Help_About_Board_EvMenuActive(CControl* control);
    void menu1_Help_About_PICSimLab_EvMenuActive(CControl* control);
    void togglebutton1_EvOnToggleButton(CControl* control);
    void filedialog2_EvOnClose(const int retId);

    /*#Others*/
    // lxrad automatic generated block end, don't edit above!

    CThread thread1;  // main simulation
    CThread thread2;  // rcontrol
    CThread thread3;  // boards

    CPWindow1(void);

    void board_Event(CControl* control);
    void board_ButtonEvent(CControl* control, uint button, uint x, uint y, uint state);
    void thread1_EvThreadRun(CControl* control);
    void thread2_EvThreadRun(CControl* control);
    void thread3_EvThreadRun(CControl* control);

    void menu1_EvBoard(CControl* control);
    void menu1_EvMicrocontroller(CControl* control);
    void DrawBoard(void);
    static void OnUpdateStatus(const int field, const std::string msg);
    static void OnConfigure(void);
    static void OnClockSet(const float clk, const int update);
    static void OnReadPreferences(const char* name, const char* value, const int create);
    static void OnSavePrefs(void);
    static void OnLoadHexFile(const std::string fname);
    static void OnOpenLoadHexFileDialog(void);
    static void OnEndSimulation(void);
    static void* OnUpdateGUI(const int id, const PICSimlabGUIType type, const PICSimlabGUIAction action,
                             const void* arg);
    void Configure(void);
    int GetNeedClkUpdate(void) { return need_clkupdate; };
    void SetNeedClkUpdate(const int ncu) { need_clkupdate = ncu; };

private:
    CItemMenu MBoard[BOARDS_MAX];
    CItemMenu MMicro[MAX_MIC];
    int pa;
    float over;
    int crt;
    int zerocount;
    int need_clkupdate;
};

extern CPWindow1 Window1;

#endif /*#CPWINDOW1*/
