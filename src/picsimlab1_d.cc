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
  SetClass(wxT("CPWindow"));
  SetName(wxT("window1"));
  SetTag(0);
  SetX(256);
  SetY(141);
  SetWidth(1110);
  SetHeight(505);
  SetHint(wxT(""));
  SetEnable(1);
  SetVisible(1);
  SetPopupMenu(NULL);
  SetTitle(wxT("Picsimlab"));
  SetOverrideRedirect(0);
  EvOnCreate=EVONCREATE & CPWindow1::_EvOnCreate;
  EvOnDestroy=EVONDESTROY & CPWindow1::_EvOnDestroy;
  EvOnShow=EVONSHOW & CPWindow1::_EvOnShow;
  //timer1
  timer1.SetFOwner(this);
  timer1.SetClass(wxT("CTimer"));
  timer1.SetName(wxT("timer1"));
  timer1.SetTag(0);
  timer1.SetTime(100);
  timer1.SetRunState(0);
  timer1.EvOnTime=EVONTIME & CPWindow1::timer1_EvOnTime;
  CreateChild(&timer1);
  //timer2
  timer2.SetFOwner(this);
  timer2.SetClass(wxT("CTimer"));
  timer2.SetName(wxT("timer2"));
  timer2.SetTag(0);
  timer2.SetTime(1000);
  timer2.SetRunState(0);
  timer2.EvOnTime=EVONTIME & CPWindow1::timer2_EvOnTime;
  CreateChild(&timer2);
  //filedialog1
  filedialog1.SetFOwner(this);
  filedialog1.SetClass(wxT("CFileDialog"));
  filedialog1.SetName(wxT("filedialog1"));
  filedialog1.SetTag(0);
  filedialog1.SetFileName(wxT("untitled.hex"));
  filedialog1.SetFilter(wxT("Hex Files (*.hex)|*.hex;*.HEX"));
  filedialog1.SetType(129);
  CreateChild(&filedialog1);
  //draw1
  draw1.SetFOwner(this);
  draw1.SetClass(wxT("CDraw"));
  draw1.SetName(wxT("draw1"));
  draw1.SetTag(0);
  draw1.SetX(170);
  draw1.SetY(5);
  draw1.SetWidth(932);
  draw1.SetHeight(467);
  draw1.SetHint(wxT(""));
  draw1.SetEnable(1);
  draw1.SetVisible(1);
  draw1.SetColor(wxT("#000001"));
  draw1.SetPopupMenu(NULL);
  draw1.EvMouseButtonPress=EVMOUSEBUTTONPRESS & CPWindow1::draw1_EvMouseButtonPress;
  draw1.EvMouseButtonRelease=EVMOUSEBUTTONRELEASE & CPWindow1::draw1_EvMouseButtonRelease;
  draw1.EvKeyboardPress=EVKEYBOARDPRESS & CPWindow1::draw1_EvKeyboardPress;
  draw1.EvKeyboardRelease=EVKEYBOARDRELEASE & CPWindow1::draw1_EvKeyboardRelease;
  draw1.SetTransparent(0);
  draw1.SetImgFileName(wxT(""));
  CreateChild(&draw1);
  //combo1
  combo1.SetFOwner(this);
  combo1.SetClass(wxT("CCombo"));
  combo1.SetName(wxT("combo1"));
  combo1.SetTag(0);
  combo1.SetX(11);
  combo1.SetY(33);
  combo1.SetWidth(89);
  combo1.SetHeight(25);
  combo1.SetHint(wxT(""));
  combo1.SetEnable(1);
  combo1.SetVisible(1);
  combo1.SetColor(wxT("#000001"));
  combo1.SetPopupMenu(NULL);
  combo1.SetItems(wxT("40,32,20,16,10,8,4,2,1,"));
  combo1.SetText(wxT("2"));
  combo1.SetReadOnly(0);
  combo1.EvOnComboChange=EVONCOMBOCHANGE & CPWindow1::combo1_EvOnComboChange;
  CreateChild(&combo1);
  //label1
  label1.SetFOwner(this);
  label1.SetClass(wxT("CLabel"));
  label1.SetName(wxT("label1"));
  label1.SetTag(0);
  label1.SetX(12);
  label1.SetY(10);
  label1.SetWidth(89);
  label1.SetHeight(21);
  label1.SetHint(wxT(""));
  label1.SetEnable(1);
  label1.SetVisible(1);
  label1.SetColor(wxT("#000001"));
  label1.SetPopupMenu(NULL);
  label1.SetText(wxT("Clock (MHz)"));
  label1.SetAlign(1);
  CreateChild(&label1);
  //menu1
  menu1.SetFOwner(this);
  menu1.SetClass(wxT("CMenu"));
  menu1.SetName(wxT("menu1"));
  menu1.SetTag(0);
  menu1.SetMenuItems(wxT("File,Board,Microcontroller,Modules,Tools,Help,"));
  CreateChild(&menu1);
  //menu1_File
  menu1_File.SetFOwner(this);
  menu1_File.SetClass(wxT("CPMenu"));
  menu1_File.SetName(wxT("menu1_File"));
  menu1_File.SetTag(0);
  menu1_File.SetText(wxT("File"));
  menu1_File.SetMenuItems(wxT("Load Hex,Reload Last,Configure,Save Workspace,Load Workspace,Exit,"));
  menu1.CreateChild(&menu1_File);
  //menu1_Board
  menu1_Board.SetFOwner(this);
  menu1_Board.SetClass(wxT("CPMenu"));
  menu1_Board.SetName(wxT("menu1_Board"));
  menu1_Board.SetTag(0);
  menu1_Board.SetText(wxT("Board"));
  menu1_Board.SetMenuItems(wxT(""));
  menu1.CreateChild(&menu1_Board);
  //menu1_Microcontroller
  menu1_Microcontroller.SetFOwner(this);
  menu1_Microcontroller.SetClass(wxT("CPMenu"));
  menu1_Microcontroller.SetName(wxT("menu1_Microcontroller"));
  menu1_Microcontroller.SetTag(0);
  menu1_Microcontroller.SetText(wxT("Microcontroller"));
  menu1_Microcontroller.SetMenuItems(wxT(""));
  menu1.CreateChild(&menu1_Microcontroller);
  //menu1_Modules
  menu1_Modules.SetFOwner(this);
  menu1_Modules.SetClass(wxT("CPMenu"));
  menu1_Modules.SetName(wxT("menu1_Modules"));
  menu1_Modules.SetTag(0);
  menu1_Modules.SetText(wxT("Modules"));
  menu1_Modules.SetMenuItems(wxT("Oscilloscope,Spare parts,"));
  menu1.CreateChild(&menu1_Modules);
  //menu1_Tools
  menu1_Tools.SetFOwner(this);
  menu1_Tools.SetClass(wxT("CPMenu"));
  menu1_Tools.SetName(wxT("menu1_Tools"));
  menu1_Tools.SetTag(0);
  menu1_Tools.SetText(wxT("Tools"));
  menu1_Tools.SetMenuItems(wxT("Serial Term,Serial Remote Tank,Esp8266 Modem Simulator,"));
  menu1.CreateChild(&menu1_Tools);
  //menu1_Help
  menu1_Help.SetFOwner(this);
  menu1_Help.SetClass(wxT("CPMenu"));
  menu1_Help.SetName(wxT("menu1_Help"));
  menu1_Help.SetTag(0);
  menu1_Help.SetText(wxT("Help"));
  menu1_Help.SetMenuItems(wxT("Contents,Examples,About,"));
  menu1.CreateChild(&menu1_Help);
  //menu1_File_LoadHex
  menu1_File_LoadHex.SetFOwner(this);
  menu1_File_LoadHex.SetClass(wxT("CItemMenu"));
  menu1_File_LoadHex.SetName(wxT("menu1_File_LoadHex"));
  menu1_File_LoadHex.SetTag(0);
  menu1_File_LoadHex.SetText(wxT("Load Hex"));
  menu1_File_LoadHex.SetEnable(1);
  menu1_File_LoadHex.SetSubMenu(NULL);
  menu1_File_LoadHex.EvMenuActive=EVMENUACTIVE & CPWindow1::menu1_File_LoadHex_EvMenuActive;
  menu1_File.CreateChild(&menu1_File_LoadHex);
  //menu1_File_ReloadLast
  menu1_File_ReloadLast.SetFOwner(this);
  menu1_File_ReloadLast.SetClass(wxT("CItemMenu"));
  menu1_File_ReloadLast.SetName(wxT("menu1_File_ReloadLast"));
  menu1_File_ReloadLast.SetTag(0);
  menu1_File_ReloadLast.SetText(wxT("Reload Last"));
  menu1_File_ReloadLast.SetEnable(0);
  menu1_File_ReloadLast.SetSubMenu(NULL);
  menu1_File_ReloadLast.EvMenuActive=EVMENUACTIVE & CPWindow1::menu1_File_ReloadLast_EvMenuActive;
  menu1_File.CreateChild(&menu1_File_ReloadLast);
  //menu1_File_Configure
  menu1_File_Configure.SetFOwner(this);
  menu1_File_Configure.SetClass(wxT("CItemMenu"));
  menu1_File_Configure.SetName(wxT("menu1_File_Configure"));
  menu1_File_Configure.SetTag(0);
  menu1_File_Configure.SetText(wxT("Configure"));
  menu1_File_Configure.SetEnable(1);
  menu1_File_Configure.SetSubMenu(NULL);
  menu1_File_Configure.EvMenuActive=EVMENUACTIVE & CPWindow1::menu1_File_Configure_EvMenuActive;
  menu1_File.CreateChild(&menu1_File_Configure);
  //menu1_File_SaveWorkspace
  menu1_File_SaveWorkspace.SetFOwner(this);
  menu1_File_SaveWorkspace.SetClass(wxT("CItemMenu"));
  menu1_File_SaveWorkspace.SetName(wxT("menu1_File_SaveWorkspace"));
  menu1_File_SaveWorkspace.SetTag(0);
  menu1_File_SaveWorkspace.SetText(wxT("Save Workspace"));
  menu1_File_SaveWorkspace.SetEnable(1);
  menu1_File_SaveWorkspace.SetSubMenu(NULL);
  menu1_File_SaveWorkspace.EvMenuActive=EVMENUACTIVE & CPWindow1::menu1_File_SaveWorkspace_EvMenuActive;
  menu1_File.CreateChild(&menu1_File_SaveWorkspace);
  //menu1_File_LoadWorkspace
  menu1_File_LoadWorkspace.SetFOwner(this);
  menu1_File_LoadWorkspace.SetClass(wxT("CItemMenu"));
  menu1_File_LoadWorkspace.SetName(wxT("menu1_File_LoadWorkspace"));
  menu1_File_LoadWorkspace.SetTag(0);
  menu1_File_LoadWorkspace.SetText(wxT("Load Workspace"));
  menu1_File_LoadWorkspace.SetEnable(1);
  menu1_File_LoadWorkspace.SetSubMenu(NULL);
  menu1_File_LoadWorkspace.EvMenuActive=EVMENUACTIVE & CPWindow1::menu1_File_LoadWorkspace_EvMenuActive;
  menu1_File.CreateChild(&menu1_File_LoadWorkspace);
  //menu1_File_Exit
  menu1_File_Exit.SetFOwner(this);
  menu1_File_Exit.SetClass(wxT("CItemMenu"));
  menu1_File_Exit.SetName(wxT("menu1_File_Exit"));
  menu1_File_Exit.SetTag(0);
  menu1_File_Exit.SetText(wxT("Exit"));
  menu1_File_Exit.SetEnable(1);
  menu1_File_Exit.SetSubMenu(NULL);
  menu1_File_Exit.EvMenuActive=EVMENUACTIVE & CPWindow1::menu1_File_Exit_EvMenuActive;
  menu1_File.CreateChild(&menu1_File_Exit);
  //menu1_Modules_Oscilloscope
  menu1_Modules_Oscilloscope.SetFOwner(this);
  menu1_Modules_Oscilloscope.SetClass(wxT("CItemMenu"));
  menu1_Modules_Oscilloscope.SetName(wxT("menu1_Modules_Oscilloscope"));
  menu1_Modules_Oscilloscope.SetTag(0);
  menu1_Modules_Oscilloscope.SetText(wxT("Oscilloscope"));
  menu1_Modules_Oscilloscope.SetEnable(1);
  menu1_Modules_Oscilloscope.SetSubMenu(NULL);
  menu1_Modules_Oscilloscope.EvMenuActive=EVMENUACTIVE & CPWindow1::menu1_Modules_Oscilloscope_EvMenuActive;
  menu1_Modules.CreateChild(&menu1_Modules_Oscilloscope);
  //menu1_Modules_Spareparts
  menu1_Modules_Spareparts.SetFOwner(this);
  menu1_Modules_Spareparts.SetClass(wxT("CItemMenu"));
  menu1_Modules_Spareparts.SetName(wxT("menu1_Modules_Spareparts"));
  menu1_Modules_Spareparts.SetTag(0);
  menu1_Modules_Spareparts.SetText(wxT("Spare parts"));
  menu1_Modules_Spareparts.SetEnable(1);
  menu1_Modules_Spareparts.SetSubMenu(NULL);
  menu1_Modules_Spareparts.EvMenuActive=EVMENUACTIVE & CPWindow1::menu1_Modules_Spareparts_EvMenuActive;
  menu1_Modules.CreateChild(&menu1_Modules_Spareparts);
  //menu1_Tools_SerialTerm
  menu1_Tools_SerialTerm.SetFOwner(this);
  menu1_Tools_SerialTerm.SetClass(wxT("CItemMenu"));
  menu1_Tools_SerialTerm.SetName(wxT("menu1_Tools_SerialTerm"));
  menu1_Tools_SerialTerm.SetTag(0);
  menu1_Tools_SerialTerm.SetText(wxT("Serial Terminal"));
  menu1_Tools_SerialTerm.SetEnable(1);
  menu1_Tools_SerialTerm.SetSubMenu(NULL);
  menu1_Tools_SerialTerm.EvMenuActive=EVMENUACTIVE & CPWindow1::menu1_Tools_SerialTerm_EvMenuActive;
  menu1_Tools.CreateChild(&menu1_Tools_SerialTerm);
  //menu1_Tools_SerialRemoteTank
  menu1_Tools_SerialRemoteTank.SetFOwner(this);
  menu1_Tools_SerialRemoteTank.SetClass(wxT("CItemMenu"));
  menu1_Tools_SerialRemoteTank.SetName(wxT("menu1_Tools_SerialRemoteTank"));
  menu1_Tools_SerialRemoteTank.SetTag(0);
  menu1_Tools_SerialRemoteTank.SetText(wxT("Serial Remote Tank"));
  menu1_Tools_SerialRemoteTank.SetEnable(1);
  menu1_Tools_SerialRemoteTank.SetSubMenu(NULL);
  menu1_Tools_SerialRemoteTank.EvMenuActive=EVMENUACTIVE & CPWindow1::menu1_Tools_SerialRemoteTank_EvMenuActive;
  menu1_Tools.CreateChild(&menu1_Tools_SerialRemoteTank);
  //menu1_Tools_Esp8266ModemSimulator
  menu1_Tools_Esp8266ModemSimulator.SetFOwner(this);
  menu1_Tools_Esp8266ModemSimulator.SetClass(wxT("CItemMenu"));
  menu1_Tools_Esp8266ModemSimulator.SetName(wxT("menu1_Tools_Esp8266ModemSimulator"));
  menu1_Tools_Esp8266ModemSimulator.SetTag(0);
  menu1_Tools_Esp8266ModemSimulator.SetText(wxT("Esp8266 Modem Simulator"));
  menu1_Tools_Esp8266ModemSimulator.SetEnable(1);
  menu1_Tools_Esp8266ModemSimulator.SetSubMenu(NULL);
  menu1_Tools_Esp8266ModemSimulator.EvMenuActive=EVMENUACTIVE & CPWindow1::menu1_Tools_Esp8266ModemSimulator_EvMenuActive;
  menu1_Tools.CreateChild(&menu1_Tools_Esp8266ModemSimulator);
  //menu1_Help_Contents
  menu1_Help_Contents.SetFOwner(this);
  menu1_Help_Contents.SetClass(wxT("CItemMenu"));
  menu1_Help_Contents.SetName(wxT("menu1_Help_Contents"));
  menu1_Help_Contents.SetTag(0);
  menu1_Help_Contents.SetText(wxT("Contents"));
  menu1_Help_Contents.SetEnable(1);
  menu1_Help_Contents.SetSubMenu(NULL);
  menu1_Help_Contents.EvMenuActive=EVMENUACTIVE & CPWindow1::menu1_Help_Contents_EvMenuActive;
  menu1_Help.CreateChild(&menu1_Help_Contents);
  //menu1_Help_Examples
  menu1_Help_Examples.SetFOwner(this);
  menu1_Help_Examples.SetClass(wxT("CItemMenu"));
  menu1_Help_Examples.SetName(wxT("menu1_Help_Examples"));
  menu1_Help_Examples.SetTag(0);
  menu1_Help_Examples.SetText(wxT("Examples"));
  menu1_Help_Examples.SetEnable(1);
  menu1_Help_Examples.SetSubMenu(NULL);
  menu1_Help_Examples.EvMenuActive=EVMENUACTIVE & CPWindow1::menu1_Help_Examples_EvMenuActive;
  menu1_Help.CreateChild(&menu1_Help_Examples);
  //menu1_Help_About
  menu1_Help_About.SetFOwner(this);
  menu1_Help_About.SetClass(wxT("CItemMenu"));
  menu1_Help_About.SetName(wxT("menu1_Help_About"));
  menu1_Help_About.SetTag(0);
  menu1_Help_About.SetText(wxT("About"));
  menu1_Help_About.SetEnable(1);
  menu1_Help_About.SetSubMenu(NULL);
  menu1_Help_About.EvMenuActive=EVMENUACTIVE & CPWindow1::menu1_Help_About_EvMenuActive;
  menu1_Help.CreateChild(&menu1_Help_About);
  //statusbar1
  statusbar1.SetFOwner(this);
  statusbar1.SetClass(wxT("CStatusbar"));
  statusbar1.SetName(wxT("statusbar1"));
  statusbar1.SetTag(0);
  statusbar1.SetFields(wxT("port,stats,serial,"));
  CreateChild(&statusbar1);
  //togglebutton1
  togglebutton1.SetFOwner(this);
  togglebutton1.SetClass(wxT("CToggleButton"));
  togglebutton1.SetName(wxT("togglebutton1"));
  togglebutton1.SetTag(0);
  togglebutton1.SetX(112);
  togglebutton1.SetY(12);
  togglebutton1.SetWidth(50);
  togglebutton1.SetHeight(50);
  togglebutton1.SetHint(wxT(""));
  togglebutton1.SetEnable(1);
  togglebutton1.SetVisible(1);
  togglebutton1.SetColor(wxT("#000001"));
  togglebutton1.SetPopupMenu(NULL);
  togglebutton1.SetText(wxT("Debug"));
  togglebutton1.SetCheck(0);
  togglebutton1.EvOnToggleButton=EVONTOGGLEBUTTON & CPWindow1::togglebutton1_EvOnToggleButton;
  CreateChild(&togglebutton1);
  //filedialog2
  filedialog2.SetFOwner(this);
  filedialog2.SetClass(wxT("CFileDialog"));
  filedialog2.SetName(wxT("filedialog2"));
  filedialog2.SetTag(0);
  filedialog2.SetFileName(wxT("untitled.pzw"));
  filedialog2.SetFilter(wxT("PICSimLab Workspace (*.pzw)|*.pzw"));
  filedialog2.SetType(129);
  CreateChild(&filedialog2);
  /*#Others*/
//lxrad automatic generated block end, don't edit above!
  
  timer1.SetTime(TIMER*1000.0);

  //thread1
  thread1.SetFOwner(this);
  thread1.SetClass(wxT("CThread"));
  thread1.SetName(wxT("thread1"));
  thread1.SetTag(0);
  thread1.EvThreadRun=EVTHREADRUN & CPWindow1::thread1_EvThreadRun;
  CreateChild(&thread1);

  JUMPSTEPS=DEFAULTJS;
  
  NSTEP=NSTEPKT;

  NSTEPJ=NSTEP/JUMPSTEPS;

picrun=1;
picpwr=1;
picrst=0;
picdbg=0;
debug=0;
osc_on= 0;
spare_on= 0;
status=0;

lab=1;
lab_=1;

over=0;

plWidth=10;
plHeight=10;
scale=1.0;

create=0;

FNAME=wxT(" ");

ondraw=0;    

pboard=NULL;

cpustate=CPU_RUNNING;

tgo=0;
 

 
  for(int i=0; i <BOARDS_LAST; i++)
    {
       MBoard[i].SetFOwner(this);
       MBoard[i].SetName("Board_"+itoa(i+1));
       MBoard[i].SetText(itoa(i+1)+wxT(" ")+boards_list[i]);
       MBoard[i].EvMenuActive=EVMENUACTIVE & CPWindow1::menu1_EvBoard;
       menu1_Board.CreateChild(&MBoard[i]);
    }    

        
};
