
#include "parts/parts_defs.h"

CPWindow5::CPWindow5(void)
{
//lxrad automatic generated block start, don't edit below!
  SetFOwner(this);
  SetClass(lxT("CPWindow"));
  SetName(lxT("window5"));
  SetTag(0);
  SetX(264);
  SetY(133);
  SetWidth(950);
  SetHeight(506);
  SetHint(lxT(""));
  SetEnable(1);
  SetVisible(0);
  SetColor(lxT("#000001"));
  SetPopupMenu(NULL);
  SetTitle(lxT("PICSimLab - Spare parts"));
  SetOverrideRedirect(0);
  EvOnCreate=EVONCREATE & CPWindow5::_EvOnCreate;
  EvOnShow=EVONSHOW & CPWindow5::_EvOnShow;
  EvOnHide=EVONHIDE & CPWindow5::_EvOnHide;
  //menu1
  menu1.SetFOwner(this);
  menu1.SetClass(lxT("CMenu"));
  menu1.SetName(lxT("menu1"));
  menu1.SetTag(0);
  menu1.SetMenuItems(lxT("File,Add,Help,"));
  CreateChild(&menu1);
  //draw1
  draw1.SetFOwner(this);
  draw1.SetClass(lxT("CDraw"));
  draw1.SetName(lxT("draw1"));
  draw1.SetTag(0);
  draw1.SetX(5);
  draw1.SetY(5);
  draw1.SetWidth(827);
  draw1.SetHeight(356);
  draw1.SetHint(lxT(""));
  draw1.SetEnable(1);
  draw1.SetVisible(1);
  draw1.SetColor(lxT("#000001"));
  draw1.SetPopupMenu(NULL);
  draw1.EvMouseMove=EVMOUSEMOVE & CPWindow5::draw1_EvMouseMove;
  draw1.EvMouseButtonPress=EVMOUSEBUTTONPRESS & CPWindow5::draw1_EvMouseButtonPress;
  draw1.EvMouseButtonRelease=EVMOUSEBUTTONRELEASE & CPWindow5::draw1_EvMouseButtonRelease;
  draw1.EvKeyboardPress=EVKEYBOARDPRESS & CPWindow5::draw1_EvKeyboardPress;
  draw1.EvKeyboardRelease=EVKEYBOARDRELEASE & CPWindow5::draw1_EvKeyboardRelease;
  draw1.SetTransparent(0);
  draw1.SetImgFileName(lxT(""));
  CreateChild(&draw1);
  //menu1_File
  menu1_File.SetFOwner(this);
  menu1_File.SetClass(lxT("CPMenu"));
  menu1_File.SetName(lxT("menu1_File"));
  menu1_File.SetTag(0);
  menu1_File.SetText(lxT("File"));
  menu1_File.SetMenuItems(lxT("Save configuration,Load configuration,"));
  menu1.CreateChild(&menu1_File);
  //menu1_Add
  menu1_Add.SetFOwner(this);
  menu1_Add.SetClass(lxT("CPMenu"));
  menu1_Add.SetName(lxT("menu1_Add"));
  menu1_Add.SetTag(0);
  menu1_Add.SetText(lxT("Add"));
  menu1_Add.SetMenuItems(lxT(""));
  menu1.CreateChild(&menu1_Add);
  //pmenu2
  pmenu2.SetFOwner(this);
  pmenu2.SetClass(lxT("CPMenu"));
  pmenu2.SetName(lxT("pmenu2"));
  pmenu2.SetTag(0);
  pmenu2.SetText(lxT(""));
  pmenu2.SetMenuItems(lxT("Properties,Move,Delete,Help,About,"));
  CreateChild(&pmenu2);
  //pmenu2_Properties
  pmenu2_Properties.SetFOwner(this);
  pmenu2_Properties.SetClass(lxT("CItemMenu"));
  pmenu2_Properties.SetName(lxT("pmenu2_Properties"));
  pmenu2_Properties.SetTag(0);
  pmenu2_Properties.SetText(lxT("Properties"));
  pmenu2_Properties.SetSubMenu(NULL);
  pmenu2_Properties.EvMenuActive=EVMENUACTIVE & CPWindow5::pmenu2_Properties_EvMenuActive;
  pmenu2.CreateChild(&pmenu2_Properties);
  //timer1
  timer1.SetFOwner(this);
  timer1.SetClass(lxT("CTimer"));
  timer1.SetName(lxT("timer1"));
  timer1.SetTag(0);
  timer1.SetTime(100);
  timer1.SetRunState(0);
  timer1.EvOnTime=EVONTIME & CPWindow5::timer1_EvOnTime;
  CreateChild(&timer1);
  //menu1_File_Newconfiguration
  menu1_File_Newconfiguration.SetFOwner(this);
  menu1_File_Newconfiguration.SetClass(lxT("CItemMenu"));
  menu1_File_Newconfiguration.SetName(lxT("menu1_File_Newconfiguration"));
  menu1_File_Newconfiguration.SetTag(0);
  menu1_File_Newconfiguration.SetText(lxT("New configuration"));
  menu1_File_Newconfiguration.SetSubMenu(NULL);
  menu1_File_Newconfiguration.EvMenuActive=EVMENUACTIVE & CPWindow5::menu1_File_Newconfiguration_EvMenuActive;
  menu1_File.CreateChild(&menu1_File_Newconfiguration);
  //menu1_File_Saveconfiguration
  menu1_File_Saveconfiguration.SetFOwner(this);
  menu1_File_Saveconfiguration.SetClass(lxT("CItemMenu"));
  menu1_File_Saveconfiguration.SetName(lxT("menu1_File_Saveconfiguration"));
  menu1_File_Saveconfiguration.SetTag(0);
  menu1_File_Saveconfiguration.SetText(lxT("Save configuration"));
  menu1_File_Saveconfiguration.SetSubMenu(NULL);
  menu1_File_Saveconfiguration.EvMenuActive=EVMENUACTIVE & CPWindow5::menu1_File_Saveconfiguration_EvMenuActive;
  menu1_File.CreateChild(&menu1_File_Saveconfiguration);
  //menu1_File_Loadconfiguration
  menu1_File_Loadconfiguration.SetFOwner(this);
  menu1_File_Loadconfiguration.SetClass(lxT("CItemMenu"));
  menu1_File_Loadconfiguration.SetName(lxT("menu1_File_Loadconfiguration"));
  menu1_File_Loadconfiguration.SetTag(0);
  menu1_File_Loadconfiguration.SetText(lxT("Load configuration"));
  menu1_File_Loadconfiguration.SetSubMenu(NULL);
  menu1_File_Loadconfiguration.EvMenuActive=EVMENUACTIVE & CPWindow5::menu1_File_Loadconfiguration_EvMenuActive;
  menu1_File.CreateChild(&menu1_File_Loadconfiguration);
  //pmenu2_Move
  pmenu2_Move.SetFOwner(this);
  pmenu2_Move.SetClass(lxT("CItemMenu"));
  pmenu2_Move.SetName(lxT("pmenu2_Move"));
  pmenu2_Move.SetTag(0);
  pmenu2_Move.SetText(lxT("Move"));
  pmenu2_Move.SetSubMenu(NULL);
  pmenu2_Move.EvMenuActive=EVMENUACTIVE & CPWindow5::pmenu2_Move_EvMenuActive;
  pmenu2.CreateChild(&pmenu2_Move);
  //pmenu2_Delete
  pmenu2_Delete.SetFOwner(this);
  pmenu2_Delete.SetClass(lxT("CItemMenu"));
  pmenu2_Delete.SetName(lxT("pmenu2_Delete"));
  pmenu2_Delete.SetTag(0);
  pmenu2_Delete.SetText(lxT("Delete"));
  pmenu2_Delete.SetSubMenu(NULL);
  pmenu2_Delete.EvMenuActive=EVMENUACTIVE & CPWindow5::pmenu2_Delete_EvMenuActive;
  pmenu2.CreateChild(&pmenu2_Delete);
  //pmenu2_Help
  pmenu2_Help.SetFOwner(this);
  pmenu2_Help.SetClass(lxT("CItemMenu"));
  pmenu2_Help.SetName(lxT("pmenu2_Help"));
  pmenu2_Help.SetTag(0);
  pmenu2_Help.SetText(lxT("Help"));
  pmenu2_Help.SetSubMenu(NULL);
  pmenu2_Help.EvMenuActive=EVMENUACTIVE & CPWindow5::pmenu2_Help_EvMenuActive;
  pmenu2.CreateChild(&pmenu2_Help);
  //pmenu2_About
  pmenu2_About.SetFOwner(this);
  pmenu2_About.SetClass(lxT("CItemMenu"));
  pmenu2_About.SetName(lxT("pmenu2_About"));
  pmenu2_About.SetTag(0);
  pmenu2_About.SetText(lxT("About"));
  pmenu2_About.SetSubMenu(NULL);
  pmenu2_About.EvMenuActive=EVMENUACTIVE & CPWindow5::pmenu2_About_EvMenuActive;
  pmenu2.CreateChild(&pmenu2_About);
  //menu1_Help
  menu1_Help.SetFOwner(this);
  menu1_Help.SetClass(lxT("CPMenu"));
  menu1_Help.SetName(lxT("menu1_Help"));
  menu1_Help.SetTag(0);
  menu1_Help.SetText(lxT("Help"));
  menu1_Help.SetMenuItems(lxT("Contents,About,"));
  menu1.CreateChild(&menu1_Help);
  //menu1_Help_Contents
  menu1_Help_Contents.SetFOwner(this);
  menu1_Help_Contents.SetClass(lxT("CItemMenu"));
  menu1_Help_Contents.SetName(lxT("menu1_Help_Contents"));
  menu1_Help_Contents.SetTag(0);
  menu1_Help_Contents.SetText(lxT("Contents"));
  menu1_Help_Contents.SetSubMenu(NULL);
  menu1_Help_Contents.EvMenuActive=EVMENUACTIVE & CPWindow5::menu1_Help_Contents_EvMenuActive;
  menu1_Help.CreateChild(&menu1_Help_Contents);
  //menu1_Help_About
  menu1_Help_About.SetFOwner(this);
  menu1_Help_About.SetClass(lxT("CItemMenu"));
  menu1_Help_About.SetName(lxT("menu1_Help_About"));
  menu1_Help_About.SetTag(0);
  menu1_Help_About.SetText(lxT("About"));
  menu1_Help_About.SetSubMenu(NULL);
  menu1_Help_About.EvMenuActive=EVMENUACTIVE & CPWindow5::menu1_Help_About_EvMenuActive;
  menu1_Help.CreateChild(&menu1_Help_About);
  //filedialog1
  filedialog1.SetFOwner(this);
  filedialog1.SetClass(lxT("CFileDialog"));
  filedialog1.SetName(lxT("filedialog1"));
  filedialog1.SetTag(0);
  filedialog1.SetFileName(lxT("untitled.pcf"));
  filedialog1.SetFilter(lxT("PICSimLab Config. (*.pcf)|*.pcf"));
  filedialog1.SetType(129);
  filedialog1.EvOnClose=EVONCLOSE & CPWindow5::filedialog1_EvOnClose;
  CreateChild(&filedialog1);
  /*#Others*/
//lxrad automatic generated block end, don't edit above!
  
  pboard = NULL; 
  
  partsc=0;  
  PartToCreate="";
  PartToMove=-1;
  PartSelected=-1;
  mdx=0;
  mdy=0;
  scale=1.0;
  LoadConfigFile="";
  fdtype = -1;
}
