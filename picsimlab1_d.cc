CPWindow1::CPWindow1(void)
{
  SetFOwner(this);
  SetClass(wxT("CPWindow"));
  SetName(wxT("window1"));
  SetTag(0);
  SetX(254);
  SetY(181);
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
  timer1.SetTime(TIMER*1000.0);
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
  filedialog1.SetFileName(wxT("untitled.txt"));
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
  combo1.SetX(12);
  combo1.SetY(160);
  combo1.SetWidth(130);
  combo1.SetHeight(25);
  combo1.SetHint(wxT(""));
  combo1.SetEnable(1);
  combo1.SetVisible(1);
  combo1.SetPopupMenu(NULL);
  combo1.SetText(wxT("2"));
  combo1.SetItems(wxT("40,32,20,16,10,8,4,2,1,"));
  combo1.EvOnComboChange=EVONCOMBOCHANGE & CPWindow1::combo1_EvOnComboChange;
  CreateChild(&combo1);
  //label1
  label1.SetFOwner(this);
  label1.SetClass(wxT("CLabel"));
  label1.SetName(wxT("label1"));
  label1.SetTag(0);
  label1.SetX(13);
  label1.SetY(133);
  label1.SetWidth(99);
  label1.SetHeight(21);
  label1.SetHint(wxT(""));
  label1.SetEnable(1);
  label1.SetVisible(1);
  label1.SetPopupMenu(NULL);
  label1.SetText(wxT("Clock (MHz)"));
  label1.SetAlign(1);
  CreateChild(&label1);
  //combo2
  combo2.SetFOwner(this);
  combo2.SetClass(wxT("CCombo"));
  combo2.SetName(wxT("combo2"));
  combo2.SetTag(0);
  combo2.SetX(12);
  combo2.SetY(39);
  combo2.SetWidth(132);
  combo2.SetHeight(25);
  combo2.SetHint(wxT(""));
  combo2.SetEnable(1);
  combo2.SetVisible(1);
  combo2.SetPopupMenu(NULL);
  combo2.SetText(wxT("1"));
  combo2.SetItems(wxT("1,2,3,4,"));
  combo2.EvOnComboChange=EVONCOMBOCHANGE & CPWindow1::combo2_EvOnComboChange;
  CreateChild(&combo2);
  //menu1
  menu1.SetFOwner(this);
  menu1.SetClass(wxT("CMenu"));
  menu1.SetName(wxT("menu1"));
  menu1.SetTag(0);
  menu1.SetMenuItems(wxT("File,Help,"));
  CreateChild(&menu1);
  //menu1_File
  menu1_File.SetFOwner(this);
  menu1_File.SetClass(wxT("CPMenu"));
  menu1_File.SetName(wxT("menu1_File"));
  menu1_File.SetTag(0);
  menu1_File.SetText(wxT("File"));
  menu1_File.SetMenuItems(wxT("Load Hex,Reload Last,Configure,Exit,"));
  menu1.CreateChild(&menu1_File);
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
  menu1_File_LoadHex.SetSubMenu(NULL);
  menu1_File_LoadHex.EvMenuActive=EVMENUACTIVE & CPWindow1::menu1_File_LoadHex_EvMenuActive;
  menu1_File.CreateChild(&menu1_File_LoadHex);
  //menu1_File_ReloadLast
  menu1_File_ReloadLast.SetFOwner(this);
  menu1_File_ReloadLast.SetClass(wxT("CItemMenu"));
  menu1_File_ReloadLast.SetName(wxT("menu1_File_ReloadLast"));
  menu1_File_ReloadLast.SetTag(0);
  menu1_File_ReloadLast.SetText(wxT("Reload Last"));
  menu1_File_ReloadLast.SetSubMenu(NULL);
  menu1_File_ReloadLast.EvMenuActive=EVMENUACTIVE & CPWindow1::menu1_File_ReloadLast_EvMenuActive;
  menu1_File.CreateChild(&menu1_File_ReloadLast);
  //menu1_File_Configure
  menu1_File_Configure.SetFOwner(this);
  menu1_File_Configure.SetClass(wxT("CItemMenu"));
  menu1_File_Configure.SetName(wxT("menu1_File_Configure"));
  menu1_File_Configure.SetTag(0);
  menu1_File_Configure.SetText(wxT("Configure"));
  menu1_File_Configure.SetSubMenu(NULL);
  menu1_File_Configure.EvMenuActive=EVMENUACTIVE & CPWindow1::menu1_File_Configure_EvMenuActive;
  menu1_File.CreateChild(&menu1_File_Configure);
  //menu1_File_Exit
  menu1_File_Exit.SetFOwner(this);
  menu1_File_Exit.SetClass(wxT("CItemMenu"));
  menu1_File_Exit.SetName(wxT("menu1_File_Exit"));
  menu1_File_Exit.SetTag(0);
  menu1_File_Exit.SetText(wxT("Exit"));
  menu1_File_Exit.SetSubMenu(NULL);
  menu1_File_Exit.EvMenuActive=EVMENUACTIVE & CPWindow1::menu1_File_Exit_EvMenuActive;
  menu1_File.CreateChild(&menu1_File_Exit);
  //menu1_Help_Contents
  menu1_Help_Contents.SetFOwner(this);
  menu1_Help_Contents.SetClass(wxT("CItemMenu"));
  menu1_Help_Contents.SetName(wxT("menu1_Help_Contents"));
  menu1_Help_Contents.SetTag(0);
  menu1_Help_Contents.SetText(wxT("Contents"));
  menu1_Help_Contents.SetSubMenu(NULL);
  menu1_Help_Contents.EvMenuActive=EVMENUACTIVE & CPWindow1::menu1_Help_Contents_EvMenuActive;
  menu1_Help.CreateChild(&menu1_Help_Contents);
  //menu1_Help_Examples
  menu1_Help_Examples.SetFOwner(this);
  menu1_Help_Examples.SetClass(wxT("CItemMenu"));
  menu1_Help_Examples.SetName(wxT("menu1_Help_Examples"));
  menu1_Help_Examples.SetTag(0);
  menu1_Help_Examples.SetText(wxT("Examples"));
  menu1_Help_Examples.SetSubMenu(NULL);
  menu1_Help_Examples.EvMenuActive=EVMENUACTIVE & CPWindow1::menu1_Help_Examples_EvMenuActive;
  menu1_Help.CreateChild(&menu1_Help_Examples);
  //menu1_Help_About
  menu1_Help_About.SetFOwner(this);
  menu1_Help_About.SetClass(wxT("CItemMenu"));
  menu1_Help_About.SetName(wxT("menu1_Help_About"));
  menu1_Help_About.SetTag(0);
  menu1_Help_About.SetText(wxT("About"));
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

  //label2
  label2.SetFOwner(this);
  label2.SetClass(wxT("CLabel"));
  label2.SetName(wxT("label2"));
  label2.SetTag(0);
  label2.SetX(12);
  label2.SetY(11);
  label2.SetWidth(92);
  label2.SetHeight(22);
  label2.SetHint(wxT(""));
  label2.SetEnable(1);
  label2.SetVisible(1);
  label2.SetPopupMenu(NULL);
  label2.SetText(wxT("Board"));
  label2.SetAlign(1);
  CreateChild(&label2);
  //label7
  label7.SetFOwner(this);
  label7.SetClass(wxT("CLabel"));
  label7.SetName(wxT("label7"));
  label7.SetTag(0);
  label7.SetX(12);
  label7.SetY(69);
  label7.SetWidth(94);
  label7.SetHeight(24);
  label7.SetHint(wxT(""));
  label7.SetEnable(1);
  label7.SetVisible(1);
  label7.SetPopupMenu(NULL);
  label7.SetText(wxT("Processor"));
  label7.SetAlign(1);
  CreateChild(&label7);
  //combo3
  combo3.SetFOwner(this);
  combo3.SetClass(wxT("CCombo"));
  combo3.SetName(wxT("combo3"));
  combo3.SetTag(0);
  combo3.SetX(12);
  combo3.SetY(98);
  combo3.SetWidth(131);
  combo3.SetHeight(26);
  combo3.SetHint(wxT(""));
  combo3.SetEnable(1);
  combo3.SetVisible(1);
  combo3.SetPopupMenu(NULL);
  combo3.SetText(wxT("PIC16F628"));
  combo3.SetItems(wxT("PIC16F628A,PIC16F648A,"));
  combo3.EvOnComboChange=EVONCOMBOCHANGE & CPWindow1::combo3_EvOnComboChange;
  CreateChild(&combo3);


  NSTEP=NSTEPKT;

  NSTEPJ=NSTEP/JUMP;

picrun=1;
picpwr=1;
picrst=0;
picdbg=0;

lab=1;
lab_=1;

over=0;

plWidth=10;
plHeight=10;
scale=1.0;

create=0;

FNAME=wxT(" ");


};
