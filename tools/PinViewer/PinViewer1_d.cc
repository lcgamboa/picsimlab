CPWindow1::CPWindow1(void)
{
//lxrad automatic generated block start, don't edit below!
  SetFOwner(this);
  SetClass(lxT("CPWindow"));
  SetName(lxT("window1"));
  SetTag(0);
  SetX(427);
  SetY(200);
  SetWidth(400);
  SetHeight(400);
  SetHint(lxT(""));
  SetEnable(1);
  SetVisible(1);
  SetColor(lxT("#000001"));
  SetPopupMenu(NULL);
  SetTitle(lxT("PinViewer"));
  SetOverrideRedirect(0);
  EvOnDestroy=EVONDESTROY & CPWindow1::_EvOnDestroy;
  EvOnShow=EVONSHOW & CPWindow1::_EvOnShow;
  EvOnCreate=EVONCREATE & CPWindow1::_EvOnCreate;
  EvOnHide=EVONHIDE & CPWindow1::_EvOnHide;
  //menu1
  menu1.SetFOwner(this);
  menu1.SetClass(lxT("CMenu"));
  menu1.SetName(lxT("menu1"));
  menu1.SetTag(0);
  menu1.SetMenuItems(lxT("File,Help,"));
  CreateChild(&menu1);
  //statusbar1
  statusbar1.SetFOwner(this);
  statusbar1.SetClass(lxT("CStatusbar"));
  statusbar1.SetName(lxT("statusbar1"));
  statusbar1.SetTag(0);
  statusbar1.SetFields(lxT("Not Connect, Scale = 1.0, Offset 0 0,"));
  CreateChild(&statusbar1);
  //draw1
  draw1.SetFOwner(this);
  draw1.SetClass(lxT("CDraw"));
  draw1.SetName(lxT("draw1"));
  draw1.SetTag(0);
  draw1.SetX(2);
  draw1.SetY(2);
  draw1.SetWidth(383);
  draw1.SetHeight(360);
  draw1.SetHint(lxT(""));
  draw1.SetEnable(1);
  draw1.SetVisible(1);
  draw1.SetColor(lxT("#000001"));
  draw1.SetPopupMenu(NULL);
  draw1.EvMouseMove=EVMOUSEMOVE & CPWindow1::draw1_EvMouseMove;
  draw1.EvMouseButtonPress=EVMOUSEBUTTONPRESS & CPWindow1::draw1_EvMouseButtonPress;
  draw1.EvMouseButtonRelease=EVMOUSEBUTTONRELEASE & CPWindow1::draw1_EvMouseButtonRelease;
  draw1.EvMouseWheel=EVMOUSEWHEEL & CPWindow1::draw1_EvMouseWheel;
  draw1.SetTransparent(0);
  draw1.SetImgFileName(lxT(""));
  CreateChild(&draw1);
  //menu1_File
  menu1_File.SetFOwner(this);
  menu1_File.SetClass(lxT("CPMenu"));
  menu1_File.SetName(lxT("menu1_File"));
  menu1_File.SetTag(0);
  menu1_File.SetText(lxT("File"));
  menu1_File.SetMenuItems(lxT("Exit,"));
  menu1.CreateChild(&menu1_File);
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
  menu1_Help_Contents.SetEnable(1);
  menu1_Help_Contents.SetSubMenu(NULL);
  menu1_Help_Contents.EvMenuActive=EVMENUACTIVE & CPWindow1::menu1_Help_Contents_EvMenuActive;
  menu1_Help.CreateChild(&menu1_Help_Contents);
  //menu1_Help_About
  menu1_Help_About.SetFOwner(this);
  menu1_Help_About.SetClass(lxT("CItemMenu"));
  menu1_Help_About.SetName(lxT("menu1_Help_About"));
  menu1_Help_About.SetTag(0);
  menu1_Help_About.SetText(lxT("About"));
  menu1_Help_About.SetEnable(1);
  menu1_Help_About.SetSubMenu(NULL);
  menu1_Help_About.EvMenuActive=EVMENUACTIVE & CPWindow1::menu1_Help_About_EvMenuActive;
  menu1_Help.CreateChild(&menu1_Help_About);
  //timer1
  timer1.SetFOwner(this);
  timer1.SetClass(lxT("CTimer"));
  timer1.SetName(lxT("timer1"));
  timer1.SetTag(0);
  timer1.SetTime(250);
  timer1.SetRunState(0);
  timer1.EvOnTime=EVONTIME & CPWindow1::timer1_EvOnTime;
  CreateChild(&timer1);
  /*#Others*/
//lxrad automatic generated block end, don't edit above!
  need_resize = 0;
  sockfd = -1;
  connected = 0;
  pincount = 0;
  offsetx=0;
  offsety=0;
  scale = 1.0;
  mouse_scroll = 0;
};
