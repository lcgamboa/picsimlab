/* ########################################################################

   PICsimLab - PIC laboratory simulator

   ########################################################################

   Copyright (c) : 2010-2018  Luis Claudio Gambôa Lopes

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

#include "picsimlab1.h"


CPWindow1::CPWindow1(void)
{
//lxrad automatic generated block start, don't edit below!
  SetFOwner(this);
  SetClass(lxT("CPWindow"));
  SetName(lxT("window1"));
  SetTag(0);
  SetX(256);
  SetY(141);
  SetWidth(1110);
  SetHeight(505);
  SetHint(lxT(""));
  SetEnable(1);
  SetVisible(1);
  SetColor(lxT("#000001"));
  SetPopupMenu(NULL);
  SetTitle(lxT("Picsimlab"));
  SetOverrideRedirect(0);
  SetDragAcceptFiles (true);
  EvOnCreate=EVONCREATE & CPWindow1::_EvOnCreate;
  EvOnDestroy=EVONDESTROY & CPWindow1::_EvOnDestroy;
  EvOnShow=EVONSHOW & CPWindow1::_EvOnShow;
  EvOnDropFile = EVONDROPFILE & CPWindow1::_EvOnDropFile;
  //timer1
  timer1.SetFOwner(this);
  timer1.SetClass(lxT("CTimer"));
  timer1.SetName(lxT("timer1"));
  timer1.SetTag(0);
  timer1.SetTime(100);
  timer1.SetRunState(0);
  timer1.EvOnTime=EVONTIME & CPWindow1::timer1_EvOnTime;
  CreateChild(&timer1);
  //timer2
  timer2.SetFOwner(this);
  timer2.SetClass(lxT("CTimer"));
  timer2.SetName(lxT("timer2"));
  timer2.SetTag(0);
  timer2.SetTime(1000);
  timer2.SetRunState(0);
  timer2.EvOnTime=EVONTIME & CPWindow1::timer2_EvOnTime;
  CreateChild(&timer2);
  //filedialog1
  filedialog1.SetFOwner(this);
  filedialog1.SetClass(lxT("CFileDialog"));
  filedialog1.SetName(lxT("filedialog1"));
  filedialog1.SetTag(0);
  filedialog1.SetFileName(lxT("untitled.hex"));
  filedialog1.SetFilter(lxT("Hex Files (*.hex)|*.hex;*.HEX"));
  filedialog1.SetType(129);
  filedialog1.EvOnClose=EVONCLOSE & CPWindow1::filedialog1_EvOnClose;
  CreateChild(&filedialog1);
  //draw1
  draw1.SetFOwner(this);
  draw1.SetClass(lxT("CDraw"));
  draw1.SetName(lxT("draw1"));
  draw1.SetTag(0);
  draw1.SetX(170);
  draw1.SetY(5);
  draw1.SetWidth(932);
  draw1.SetHeight(467);
  draw1.SetHint(lxT(""));
  draw1.SetEnable(1);
  draw1.SetVisible(1);
  draw1.SetColor(lxT("#000001"));
  draw1.SetPopupMenu(NULL);
  draw1.SetDragAcceptFiles (true);
  draw1.EvMouseMove=EVMOUSEMOVE & CPWindow1::draw1_EvMouseMove;
  draw1.EvMouseButtonPress=EVMOUSEBUTTONPRESS & CPWindow1::draw1_EvMouseButtonPress;
  draw1.EvMouseButtonRelease=EVMOUSEBUTTONRELEASE & CPWindow1::draw1_EvMouseButtonRelease;
  draw1.EvKeyboardPress=EVKEYBOARDPRESS & CPWindow1::draw1_EvKeyboardPress;
  draw1.EvKeyboardRelease=EVKEYBOARDRELEASE & CPWindow1::draw1_EvKeyboardRelease;
  draw1.EvOnDropFile = EVONDROPFILE & CPWindow1::_EvOnDropFile;
  draw1.SetTransparent(0);
  draw1.SetImgFileName(lxT(""));
  CreateChild(&draw1);
  //combo1
  combo1.SetFOwner(this);
  combo1.SetClass(lxT("CCombo"));
  combo1.SetName(lxT("combo1"));
  combo1.SetTag(0);
  combo1.SetX(84);
  combo1.SetY(6);
  combo1.SetWidth(83);
  combo1.SetHeight(25);
  combo1.SetHint(lxT(""));
  combo1.SetEnable(1);
  combo1.SetVisible(1);
  combo1.SetColor(lxT("#000001"));
  combo1.SetPopupMenu(NULL);
  combo1.SetItems(lxT("64,48,40,32,20,16,10,8,4,2,1,"));
  combo1.SetText(lxT("2"));
  combo1.SetReadOnly(0);
  combo1.EvOnComboChange=EVONCOMBOCHANGE & CPWindow1::combo1_EvOnComboChange;
  CreateChild(&combo1);
  //label1
  label1.SetFOwner(this);
  label1.SetClass(lxT("CLabel"));
  label1.SetName(lxT("label1"));
  label1.SetTag(0);
  label1.SetX(6);
  label1.SetY(10);
  label1.SetWidth(73);
  label1.SetHeight(21);
  label1.SetHint(lxT(""));
  label1.SetEnable(1);
  label1.SetVisible(1);
  label1.SetColor(lxT("#000001"));
  label1.SetPopupMenu(NULL);
  label1.SetText(lxT("Clk(MHz)"));
  label1.SetAlign(1);
  CreateChild(&label1);
  //label2
  label2.SetFOwner(this);
  label2.SetClass(lxT("CLabel"));
  label2.SetName(lxT("label2"));
  label2.SetTag(0);
  label2.SetX(6);
  label2.SetY(38);
  label2.SetWidth(76);
  label2.SetHeight(21);
  label2.SetHint(lxT(""));
  label2.SetEnable(1);
  label2.SetVisible(1);
  label2.SetColor(lxT("#000001"));
  label2.SetPopupMenu(NULL);
  label2.SetText(lxT("Spd: 1.00x"));
  label2.SetAlign(1);
  CreateChild(&label2);
  //menu1
  menu1.SetFOwner(this);
  menu1.SetClass(lxT("CMenu"));
  menu1.SetName(lxT("menu1"));
  menu1.SetTag(0);
  menu1.SetMenuItems(lxT("File,Board,Microcontroller,Modules,Tools,Help,"));
  CreateChild(&menu1);
  //menu1_File
  menu1_File.SetFOwner(this);
  menu1_File.SetClass(lxT("CPMenu"));
  menu1_File.SetName(lxT("menu1_File"));
  menu1_File.SetTag(0);
  menu1_File.SetText(lxT("File"));
  menu1_File.SetMenuItems(lxT("Load Hex,Reload Last,Configure,Save Workspace,Load Workspace,Exit,"));
  menu1.CreateChild(&menu1_File);
  //menu1_Board
  menu1_Board.SetFOwner(this);
  menu1_Board.SetClass(lxT("CPMenu"));
  menu1_Board.SetName(lxT("menu1_Board"));
  menu1_Board.SetTag(0);
  menu1_Board.SetText(lxT("Board"));
  menu1_Board.SetMenuItems(lxT(""));
  menu1.CreateChild(&menu1_Board);
  //menu1_Microcontroller
  menu1_Microcontroller.SetFOwner(this);
  menu1_Microcontroller.SetClass(lxT("CPMenu"));
  menu1_Microcontroller.SetName(lxT("menu1_Microcontroller"));
  menu1_Microcontroller.SetTag(0);
  menu1_Microcontroller.SetText(lxT("Microcontroller"));
  menu1_Microcontroller.SetMenuItems(lxT(""));
  menu1.CreateChild(&menu1_Microcontroller);
  //menu1_Modules
  menu1_Modules.SetFOwner(this);
  menu1_Modules.SetClass(lxT("CPMenu"));
  menu1_Modules.SetName(lxT("menu1_Modules"));
  menu1_Modules.SetTag(0);
  menu1_Modules.SetText(lxT("Modules"));
  menu1_Modules.SetMenuItems(lxT("Oscilloscope,Spare parts,"));
  menu1.CreateChild(&menu1_Modules);
  //menu1_Tools
  menu1_Tools.SetFOwner(this);
  menu1_Tools.SetClass(lxT("CPMenu"));
  menu1_Tools.SetName(lxT("menu1_Tools"));
  menu1_Tools.SetTag(0);
  menu1_Tools.SetText(lxT("Tools"));
  menu1_Tools.SetMenuItems(lxT("Serial Term,Serial Remote Tank,Esp8266 Modem Simulator,Arduino Bootloader,MPLABX Debugger Plugin,Pin Viewer,"));
  menu1.CreateChild(&menu1_Tools);
  //menu1_Help
  menu1_Help.SetFOwner(this);
  menu1_Help.SetClass(lxT("CPMenu"));
  menu1_Help.SetName(lxT("menu1_Help"));
  menu1_Help.SetTag(0);
  menu1_Help.SetText(lxT("Help"));
  menu1_Help.SetMenuItems(lxT("Contents,Board,Examples,About Board,About PICSimLab,"));
  menu1.CreateChild(&menu1_Help);
  //menu1_File_LoadHex
  menu1_File_LoadHex.SetFOwner(this);
  menu1_File_LoadHex.SetClass(lxT("CItemMenu"));
  menu1_File_LoadHex.SetName(lxT("menu1_File_LoadHex"));
  menu1_File_LoadHex.SetTag(0);
  menu1_File_LoadHex.SetText(lxT("Load Hex"));
  menu1_File_LoadHex.SetEnable(1);
  menu1_File_LoadHex.SetSubMenu(NULL);
  menu1_File_LoadHex.EvMenuActive=EVMENUACTIVE & CPWindow1::menu1_File_LoadHex_EvMenuActive;
  menu1_File.CreateChild(&menu1_File_LoadHex);
  //menu1_File_ReloadLast
  menu1_File_ReloadLast.SetFOwner(this);
  menu1_File_ReloadLast.SetClass(lxT("CItemMenu"));
  menu1_File_ReloadLast.SetName(lxT("menu1_File_ReloadLast"));
  menu1_File_ReloadLast.SetTag(0);
  menu1_File_ReloadLast.SetText(lxT("Reload Last"));
  menu1_File_ReloadLast.SetEnable(0);
  menu1_File_ReloadLast.SetSubMenu(NULL);
  menu1_File_ReloadLast.EvMenuActive=EVMENUACTIVE & CPWindow1::menu1_File_ReloadLast_EvMenuActive;
  menu1_File.CreateChild(&menu1_File_ReloadLast);
  //menu1_File_SaveHex
  menu1_File_SaveHex.SetFOwner(this);
  menu1_File_SaveHex.SetClass(lxT("CItemMenu"));
  menu1_File_SaveHex.SetName(lxT("menu1_File_SaveHex"));
  menu1_File_SaveHex.SetTag(0);
  menu1_File_SaveHex.SetText(lxT("Save Hex"));
  menu1_File_SaveHex.SetEnable(1);
  menu1_File_SaveHex.SetSubMenu(NULL);
  menu1_File_SaveHex.EvMenuActive=EVMENUACTIVE & CPWindow1::menu1_File_SaveHex_EvMenuActive;
  menu1_File.CreateChild(&menu1_File_SaveHex);
  //menu1_File_Configure
  menu1_File_Configure.SetFOwner(this);
  menu1_File_Configure.SetClass(lxT("CItemMenu"));
  menu1_File_Configure.SetName(lxT("menu1_File_Configure"));
  menu1_File_Configure.SetTag(0);
  menu1_File_Configure.SetText(lxT("Configure"));
  menu1_File_Configure.SetEnable(1);
  menu1_File_Configure.SetSubMenu(NULL);
  menu1_File_Configure.EvMenuActive=EVMENUACTIVE & CPWindow1::menu1_File_Configure_EvMenuActive;
  menu1_File.CreateChild(&menu1_File_Configure);
  //menu1_File_SaveWorkspace
  menu1_File_SaveWorkspace.SetFOwner(this);
  menu1_File_SaveWorkspace.SetClass(lxT("CItemMenu"));
  menu1_File_SaveWorkspace.SetName(lxT("menu1_File_SaveWorkspace"));
  menu1_File_SaveWorkspace.SetTag(0);
  menu1_File_SaveWorkspace.SetText(lxT("Save Workspace"));
  menu1_File_SaveWorkspace.SetEnable(1);
  menu1_File_SaveWorkspace.SetSubMenu(NULL);
  menu1_File_SaveWorkspace.EvMenuActive=EVMENUACTIVE & CPWindow1::menu1_File_SaveWorkspace_EvMenuActive;
  menu1_File.CreateChild(&menu1_File_SaveWorkspace);
  //menu1_File_LoadWorkspace
  menu1_File_LoadWorkspace.SetFOwner(this);
  menu1_File_LoadWorkspace.SetClass(lxT("CItemMenu"));
  menu1_File_LoadWorkspace.SetName(lxT("menu1_File_LoadWorkspace"));
  menu1_File_LoadWorkspace.SetTag(0);
  menu1_File_LoadWorkspace.SetText(lxT("Load Workspace"));
  menu1_File_LoadWorkspace.SetEnable(1);
  menu1_File_LoadWorkspace.SetSubMenu(NULL);
  menu1_File_LoadWorkspace.EvMenuActive=EVMENUACTIVE & CPWindow1::menu1_File_LoadWorkspace_EvMenuActive;
  menu1_File.CreateChild(&menu1_File_LoadWorkspace);
  //menu1_File_Exit
  menu1_File_Exit.SetFOwner(this);
  menu1_File_Exit.SetClass(lxT("CItemMenu"));
  menu1_File_Exit.SetName(lxT("menu1_File_Exit"));
  menu1_File_Exit.SetTag(0);
  menu1_File_Exit.SetText(lxT("Exit"));
  menu1_File_Exit.SetEnable(1);
  menu1_File_Exit.SetSubMenu(NULL);
  menu1_File_Exit.EvMenuActive=EVMENUACTIVE & CPWindow1::menu1_File_Exit_EvMenuActive;
  menu1_File.CreateChild(&menu1_File_Exit);
  //menu1_Modules_Oscilloscope
  menu1_Modules_Oscilloscope.SetFOwner(this);
  menu1_Modules_Oscilloscope.SetClass(lxT("CItemMenu"));
  menu1_Modules_Oscilloscope.SetName(lxT("menu1_Modules_Oscilloscope"));
  menu1_Modules_Oscilloscope.SetTag(0);
  menu1_Modules_Oscilloscope.SetText(lxT("Oscilloscope"));
  menu1_Modules_Oscilloscope.SetEnable(1);
  menu1_Modules_Oscilloscope.SetSubMenu(NULL);
  menu1_Modules_Oscilloscope.EvMenuActive=EVMENUACTIVE & CPWindow1::menu1_Modules_Oscilloscope_EvMenuActive;
  menu1_Modules.CreateChild(&menu1_Modules_Oscilloscope);
  //menu1_Modules_Spareparts
  menu1_Modules_Spareparts.SetFOwner(this);
  menu1_Modules_Spareparts.SetClass(lxT("CItemMenu"));
  menu1_Modules_Spareparts.SetName(lxT("menu1_Modules_Spareparts"));
  menu1_Modules_Spareparts.SetTag(0);
  menu1_Modules_Spareparts.SetText(lxT("Spare parts"));
  menu1_Modules_Spareparts.SetEnable(1);
  menu1_Modules_Spareparts.SetSubMenu(NULL);
  menu1_Modules_Spareparts.EvMenuActive=EVMENUACTIVE & CPWindow1::menu1_Modules_Spareparts_EvMenuActive;
  menu1_Modules.CreateChild(&menu1_Modules_Spareparts);
  //menu1_Tools_SerialTerm
  menu1_Tools_SerialTerm.SetFOwner(this);
  menu1_Tools_SerialTerm.SetClass(lxT("CItemMenu"));
  menu1_Tools_SerialTerm.SetName(lxT("menu1_Tools_SerialTerm"));
  menu1_Tools_SerialTerm.SetTag(0);
  menu1_Tools_SerialTerm.SetText(lxT("Serial Terminal"));
  menu1_Tools_SerialTerm.SetEnable(1);
  menu1_Tools_SerialTerm.SetSubMenu(NULL);
  menu1_Tools_SerialTerm.EvMenuActive=EVMENUACTIVE & CPWindow1::menu1_Tools_SerialTerm_EvMenuActive;
  menu1_Tools.CreateChild(&menu1_Tools_SerialTerm);
  //menu1_Tools_SerialRemoteTank
  menu1_Tools_SerialRemoteTank.SetFOwner(this);
  menu1_Tools_SerialRemoteTank.SetClass(lxT("CItemMenu"));
  menu1_Tools_SerialRemoteTank.SetName(lxT("menu1_Tools_SerialRemoteTank"));
  menu1_Tools_SerialRemoteTank.SetTag(0);
  menu1_Tools_SerialRemoteTank.SetText(lxT("Serial Remote Tank"));
  menu1_Tools_SerialRemoteTank.SetEnable(1);
  menu1_Tools_SerialRemoteTank.SetSubMenu(NULL);
  menu1_Tools_SerialRemoteTank.EvMenuActive=EVMENUACTIVE & CPWindow1::menu1_Tools_SerialRemoteTank_EvMenuActive;
  menu1_Tools.CreateChild(&menu1_Tools_SerialRemoteTank);
  //menu1_Tools_Esp8266ModemSimulator
  menu1_Tools_Esp8266ModemSimulator.SetFOwner(this);
  menu1_Tools_Esp8266ModemSimulator.SetClass(lxT("CItemMenu"));
  menu1_Tools_Esp8266ModemSimulator.SetName(lxT("menu1_Tools_Esp8266ModemSimulator"));
  menu1_Tools_Esp8266ModemSimulator.SetTag(0);
  menu1_Tools_Esp8266ModemSimulator.SetText(lxT("Esp8266 Modem Simulator"));
  menu1_Tools_Esp8266ModemSimulator.SetEnable(1);
  menu1_Tools_Esp8266ModemSimulator.SetSubMenu(NULL);
  menu1_Tools_Esp8266ModemSimulator.EvMenuActive=EVMENUACTIVE & CPWindow1::menu1_Tools_Esp8266ModemSimulator_EvMenuActive;
  menu1_Tools.CreateChild(&menu1_Tools_Esp8266ModemSimulator);
  //menu1_Tools_ArduinoBootloader
  menu1_Tools_ArduinoBootloader.SetFOwner(this);
  menu1_Tools_ArduinoBootloader.SetClass(lxT("CItemMenu"));
  menu1_Tools_ArduinoBootloader.SetName(lxT("menu1_Tools_ArduinoBootloader"));
  menu1_Tools_ArduinoBootloader.SetTag(0);
  menu1_Tools_ArduinoBootloader.SetText(lxT("Arduino Bootloader"));
  menu1_Tools_ArduinoBootloader.SetEnable(1);
  menu1_Tools_ArduinoBootloader.SetSubMenu(NULL);
  menu1_Tools_ArduinoBootloader.EvMenuActive=EVMENUACTIVE & CPWindow1::menu1_Tools_ArduinoBootloader_EvMenuActive;
  menu1_Tools.CreateChild(&menu1_Tools_ArduinoBootloader); 
  //menu1_Tools_MPLABXDebuggerPlugin
  menu1_Tools_MPLABXDebuggerPlugin.SetFOwner(this);
  menu1_Tools_MPLABXDebuggerPlugin.SetClass(lxT("CItemMenu"));
  menu1_Tools_MPLABXDebuggerPlugin.SetName(lxT("menu1_Tools_MPLABXDebuggerPlugin"));
  menu1_Tools_MPLABXDebuggerPlugin.SetTag(0);
  menu1_Tools_MPLABXDebuggerPlugin.SetText(lxT("MPLABX Debugger Plugin"));
  menu1_Tools_MPLABXDebuggerPlugin.SetEnable(1);
  menu1_Tools_MPLABXDebuggerPlugin.SetSubMenu(NULL);
  menu1_Tools_MPLABXDebuggerPlugin.EvMenuActive=EVMENUACTIVE & CPWindow1::menu1_Tools_MPLABXDebuggerPlugin_EvMenuActive;
  menu1_Tools.CreateChild(&menu1_Tools_MPLABXDebuggerPlugin); 
  //menu1_Tools_PinViewer
  menu1_Tools_PinViewer.SetFOwner(this);
  menu1_Tools_PinViewer.SetClass(lxT("CItemMenu"));
  menu1_Tools_PinViewer.SetName(lxT("menu1_Tools_PinViewer"));
  menu1_Tools_PinViewer.SetTag(0);
  menu1_Tools_PinViewer.SetText(lxT("Pin Viewer"));
  menu1_Tools_PinViewer.SetEnable(1);
  menu1_Tools_PinViewer.SetSubMenu(NULL);
  menu1_Tools_PinViewer.EvMenuActive=EVMENUACTIVE & CPWindow1::menu1_Tools_PinViewer_EvMenuActive;
  menu1_Tools.CreateChild(&menu1_Tools_PinViewer); 
  //menu1_Help_Contents
  menu1_Help_Contents.SetFOwner(this);
  menu1_Help_Contents.SetClass(lxT("CItemMenu"));
  menu1_Help_Contents.SetName(lxT("menu1_Help_Contents"));
  menu1_Help_Contents.SetTag(0);
  menu1_Help_Contents.SetText(lxT("Contents"));
  menu1_Help_Contents.SetEnable(1);
  menu1_Help_Contents.SetSubMenu(NULL);
  menu1_Help_Contents.EvMenuActive=EVMENUACTIVE & CPWindow1::menu1_Help_Contents_EvMenuActive;
  menu1_Help.CreateChild(&menu1_Help_Contents);
  //menu1_Help_Board
  menu1_Help_Board.SetFOwner(this);
  menu1_Help_Board.SetClass(lxT("CItemMenu"));
  menu1_Help_Board.SetName(lxT("menu1_Help_Board"));
  menu1_Help_Board.SetTag(0);
  menu1_Help_Board.SetText(lxT("Board"));
  menu1_Help_Board.SetEnable(1);
  menu1_Help_Board.SetSubMenu(NULL);
  menu1_Help_Board.EvMenuActive=EVMENUACTIVE & CPWindow1::menu1_Help_Board_EvMenuActive;
  menu1_Help.CreateChild(&menu1_Help_Board);
  //menu1_Help_Examples
  menu1_Help_Examples.SetFOwner(this);
  menu1_Help_Examples.SetClass(lxT("CItemMenu"));
  menu1_Help_Examples.SetName(lxT("menu1_Help_Examples"));
  menu1_Help_Examples.SetTag(0);
  menu1_Help_Examples.SetText(lxT("Examples"));
  menu1_Help_Examples.SetEnable(1);
  menu1_Help_Examples.SetSubMenu(NULL);
  menu1_Help_Examples.EvMenuActive=EVMENUACTIVE & CPWindow1::menu1_Help_Examples_EvMenuActive;
  menu1_Help.CreateChild(&menu1_Help_Examples);
  //menu1_Help_About_Board
  menu1_Help_About_Board.SetFOwner(this);
  menu1_Help_About_Board.SetClass(lxT("CItemMenu"));
  menu1_Help_About_Board.SetName(lxT("menu1_Help_About_Board"));
  menu1_Help_About_Board.SetTag(0);
  menu1_Help_About_Board.SetText(lxT("About Board"));
  menu1_Help_About_Board.SetEnable(1);
  menu1_Help_About_Board.SetSubMenu(NULL);
  menu1_Help_About_Board.EvMenuActive=EVMENUACTIVE & CPWindow1::menu1_Help_About_Board_EvMenuActive;
  menu1_Help.CreateChild(&menu1_Help_About_Board);
  //menu1_Help_About_PICSimLab
  menu1_Help_About_PICSimLab.SetFOwner(this);
  menu1_Help_About_PICSimLab.SetClass(lxT("CItemMenu"));
  menu1_Help_About_PICSimLab.SetName(lxT("menu1_Help_About_PICSimLab"));
  menu1_Help_About_PICSimLab.SetTag(0);
  menu1_Help_About_PICSimLab.SetText(lxT("About PICSimLab"));
  menu1_Help_About_PICSimLab.SetEnable(1);
  menu1_Help_About_PICSimLab.SetSubMenu(NULL);
  menu1_Help_About_PICSimLab.EvMenuActive=EVMENUACTIVE & CPWindow1::menu1_Help_About_PICSimLab_EvMenuActive;
  menu1_Help.CreateChild(&menu1_Help_About_PICSimLab);
  //statusbar1
  statusbar1.SetFOwner(this);
  statusbar1.SetClass(lxT("CStatusbar"));
  statusbar1.SetName(lxT("statusbar1"));
  statusbar1.SetTag(0);
  statusbar1.SetFields(lxT("port,stats,serial,"));
  CreateChild(&statusbar1);
  //togglebutton1
  togglebutton1.SetFOwner(this);
  togglebutton1.SetClass(lxT("CToggleButton"));
  togglebutton1.SetName(lxT("togglebutton1"));
  togglebutton1.SetTag(0);
  togglebutton1.SetX(84);
  togglebutton1.SetY(34);
  togglebutton1.SetWidth(83);
  togglebutton1.SetHeight(27);
  togglebutton1.SetHint(lxT(""));
  togglebutton1.SetEnable(1);
  togglebutton1.SetVisible(1);
  togglebutton1.SetColor(lxT("#000001"));
  togglebutton1.SetPopupMenu(NULL);
  togglebutton1.SetText(lxT("Debug"));
  togglebutton1.SetCheck(0);
  togglebutton1.EvOnToggleButton=EVONTOGGLEBUTTON & CPWindow1::togglebutton1_EvOnToggleButton;
  CreateChild(&togglebutton1);
  //filedialog2
  filedialog2.SetFOwner(this);
  filedialog2.SetClass(lxT("CFileDialog"));
  filedialog2.SetName(lxT("filedialog2"));
  filedialog2.SetTag(0);
  filedialog2.SetFileName(lxT("untitled.pzw"));
  filedialog2.SetFilter(lxT("PICSimLab Workspace (*.pzw)|*.pzw"));
  filedialog2.SetType(129);
  filedialog2.EvOnClose=EVONCLOSE & CPWindow1::filedialog2_EvOnClose;
  CreateChild(&filedialog2);
  /*#Others*/
//lxrad automatic generated block end, don't edit above!
  
  timer1.SetTime(TIMER*1000.0);

  //thread1
  thread1.SetFOwner(this);
  thread1.SetClass(lxT("CThread"));
  thread1.SetName(lxT("thread1"));
  thread1.SetTag(0);
  thread1.EvThreadRun=EVTHREADRUN & CPWindow1::thread1_EvThreadRun;
  CreateChild(&thread1);

  //thread2
  thread2.SetFOwner(this);
  thread2.SetClass(lxT("CThread"));
  thread2.SetName(lxT("thread2"));
  thread2.SetTag(0);
  thread2.EvThreadRun=EVTHREADRUN & CPWindow1::thread2_EvThreadRun;
  CreateChild(&thread2);
  
  JUMPSTEPS=DEFAULTJS;
  
  NSTEP=NSTEPKT;

  NSTEPJ=NSTEP/JUMPSTEPS;

mcurun=1;
mcupwr=1;
mcurst=0;
mcudbg=0;
debug=0;
debug_type=0;
debug_port=1234;
remotec_port=5000;
osc_on= 0;
spare_on= 0;
status.status=0;
lab=1;
lab_=1;

over=0;

plWidth=10;
plHeight=10;
scale=1.0;


FNAME=lxT(" ");
OldPath=lxT("");
    
crt=1;    
zerocount=0;

pboard=NULL;

cpustate=CPU_RUNNING;

tgo=0;
  
Errors.Clear();

need_resize=0;

settodestroy=0;

sync=0;

PATH="";
HOME="";

#ifndef _NOTHREAD    
cpu_mutex = NULL;
cpu_cond = NULL;
#endif

#ifdef NO_TOOLS
   menu1.DestroyChild (&menu1_Tools);
#endif
   
#ifdef NO_DEBUG
   DestroyChild(&togglebutton1);
#endif   
}
