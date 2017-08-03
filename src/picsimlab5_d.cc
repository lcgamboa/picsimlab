CPWindow5::CPWindow5(void)
{
//lxrad automatic generated block start, don't edit below!
  SetFOwner(this);
  SetClass(wxT("CPWindow"));
  SetName(wxT("window5"));
  SetTag(0);
  SetX(118);
  SetY(52);
  SetWidth(1248);
  SetHeight(685);
  SetHint(wxT(""));
  SetEnable(1);
  SetVisible(0);
  SetPopupMenu(NULL);
  SetTitle(wxT("picsimlab"));
  SetOverrideRedirect(0);
  EvOnCreate=EVONCREATE & CPWindow5::_EvOnCreate;
  EvOnShow=EVONSHOW & CPWindow5::_EvOnShow;
  EvOnHide=EVONHIDE & CPWindow5::_EvOnHide;
  //menu1
  menu1.SetFOwner(this);
  menu1.SetClass(wxT("CMenu"));
  menu1.SetName(wxT("menu1"));
  menu1.SetTag(0);
  menu1.SetMenuItems(wxT("File,Add,"));
  CreateChild(&menu1);
  //draw1
  draw1.SetFOwner(this);
  draw1.SetClass(wxT("CDraw"));
  draw1.SetName(wxT("draw1"));
  draw1.SetTag(0);
  draw1.SetX(7);
  draw1.SetY(5);
  draw1.SetWidth(827);
  draw1.SetHeight(356);
  draw1.SetHint(wxT(""));
  draw1.SetEnable(1);
  draw1.SetVisible(1);
  draw1.SetPopupMenu(NULL);
  draw1.EvMouseMove=EVMOUSEMOVE & CPWindow5::draw1_EvMouseMove;
  draw1.EvMouseButtonPress=EVMOUSEBUTTONPRESS & CPWindow5::draw1_EvMouseButtonPress;
  draw1.EvMouseButtonRelease=EVMOUSEBUTTONRELEASE & CPWindow5::draw1_EvMouseButtonRelease;
  draw1.EvKeyboardPress=EVKEYBOARDPRESS & CPWindow5::draw1_EvKeyboardPress;
  draw1.EvKeyboardRelease=EVKEYBOARDRELEASE & CPWindow5::draw1_EvKeyboardRelease;
  draw1.SetTransparent(0);
  draw1.SetImgFileName(wxT(""));
  CreateChild(&draw1);
  //menu1_File
  menu1_File.SetFOwner(this);
  menu1_File.SetClass(wxT("CPMenu"));
  menu1_File.SetName(wxT("menu1_File"));
  menu1_File.SetTag(0);
  menu1_File.SetText(wxT("File"));
  menu1_File.SetMenuItems(wxT("Save configuration,Load configuration,"));
  menu1.CreateChild(&menu1_File);
  //menu1_Add
  menu1_Add.SetFOwner(this);
  menu1_Add.SetClass(wxT("CPMenu"));
  menu1_Add.SetName(wxT("menu1_Add"));
  menu1_Add.SetTag(0);
  menu1_Add.SetText(wxT("Add"));
  menu1_Add.SetMenuItems(wxT(""));
  menu1.CreateChild(&menu1_Add);
  //pmenu2
  pmenu2.SetFOwner(this);
  pmenu2.SetClass(wxT("CPMenu"));
  pmenu2.SetName(wxT("pmenu2"));
  pmenu2.SetTag(0);
  pmenu2.SetText(wxT(""));
  pmenu2.SetMenuItems(wxT("Propierties,Move,Delete,"));
  CreateChild(&pmenu2);
  //pmenu2_Propierties
  pmenu2_Propierties.SetFOwner(this);
  pmenu2_Propierties.SetClass(wxT("CItemMenu"));
  pmenu2_Propierties.SetName(wxT("pmenu2_Propierties"));
  pmenu2_Propierties.SetTag(0);
  pmenu2_Propierties.SetText(wxT("Propierties"));
  pmenu2_Propierties.SetSubMenu(NULL);
  pmenu2_Propierties.EvMenuActive=EVMENUACTIVE & CPWindow5::pmenu2_Propierties_EvMenuActive;
  pmenu2.CreateChild(&pmenu2_Propierties);
  //timer1
  timer1.SetFOwner(this);
  timer1.SetClass(wxT("CTimer"));
  timer1.SetName(wxT("timer1"));
  timer1.SetTag(0);
  timer1.SetTime(100);
  timer1.SetRunState(1);
  timer1.EvOnTime=EVONTIME & CPWindow5::timer1_EvOnTime;
  CreateChild(&timer1);
  //menu1_File_Saveconfiguration
  menu1_File_Saveconfiguration.SetFOwner(this);
  menu1_File_Saveconfiguration.SetClass(wxT("CItemMenu"));
  menu1_File_Saveconfiguration.SetName(wxT("menu1_File_Saveconfiguration"));
  menu1_File_Saveconfiguration.SetTag(0);
  menu1_File_Saveconfiguration.SetText(wxT("Save configuration"));
  menu1_File_Saveconfiguration.SetSubMenu(NULL);
  menu1_File_Saveconfiguration.EvMenuActive=EVMENUACTIVE & CPWindow5::menu1_File_Saveconfiguration_EvMenuActive;
  menu1_File.CreateChild(&menu1_File_Saveconfiguration);
  //menu1_File_Loadconfiguration
  menu1_File_Loadconfiguration.SetFOwner(this);
  menu1_File_Loadconfiguration.SetClass(wxT("CItemMenu"));
  menu1_File_Loadconfiguration.SetName(wxT("menu1_File_Loadconfiguration"));
  menu1_File_Loadconfiguration.SetTag(0);
  menu1_File_Loadconfiguration.SetText(wxT("Load configuration"));
  menu1_File_Loadconfiguration.SetSubMenu(NULL);
  menu1_File_Loadconfiguration.EvMenuActive=EVMENUACTIVE & CPWindow5::menu1_File_Loadconfiguration_EvMenuActive;
  menu1_File.CreateChild(&menu1_File_Loadconfiguration);
  //pmenu2_Move
  pmenu2_Move.SetFOwner(this);
  pmenu2_Move.SetClass(wxT("CItemMenu"));
  pmenu2_Move.SetName(wxT("pmenu2_Move"));
  pmenu2_Move.SetTag(0);
  pmenu2_Move.SetText(wxT("Move"));
  pmenu2_Move.SetSubMenu(NULL);
  pmenu2_Move.EvMenuActive=EVMENUACTIVE & CPWindow5::pmenu2_Move_EvMenuActive;
  pmenu2.CreateChild(&pmenu2_Move);
  //pmenu2_Delete
  pmenu2_Delete.SetFOwner(this);
  pmenu2_Delete.SetClass(wxT("CItemMenu"));
  pmenu2_Delete.SetName(wxT("pmenu2_Delete"));
  pmenu2_Delete.SetTag(0);
  pmenu2_Delete.SetText(wxT("Delete"));
  pmenu2_Delete.SetSubMenu(NULL);
  pmenu2_Delete.EvMenuActive=EVMENUACTIVE & CPWindow5::pmenu2_Delete_EvMenuActive;
  pmenu2.CreateChild(&pmenu2_Delete);
  /*#Others*/
//lxrad automatic generated block end, don't edit above!
  
  for(int i=0; i <NUM_PARTS; i++)
    {
       MParts[i].SetFOwner(this);
       MParts[i].SetName(parts_list[i]);
       MParts[i].SetText(parts_list[i]);
       MParts[i].EvMenuActive=EVMENUACTIVE & CPWindow5::menu1_EvMenuActive;
       menu1_Add.CreateChild(&MParts[i]);
    }
  
  partsc=0;  
  PartToCreate="";
  PartToMove=-1;
  PartSelected=-1;
  mdx=0;
  mdy=0;
 
};
