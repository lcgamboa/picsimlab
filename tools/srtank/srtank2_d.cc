CPWindow2::CPWindow2(void)
{
//lxrad automatic generated block start, don't edit below!
  SetFOwner(this);
  SetClass(wxT("CPWindow"));
  SetName(wxT("window2"));
  SetTag(0);
  SetX(282);
  SetY(122);
  SetWidth(830);
  SetHeight(574);
  SetHint(wxT(""));
  SetEnable(1);
  SetVisible(0);
  SetColor(wxT("#000001"));
  SetPopupMenu(NULL);
  SetTitle(wxT("srtank - Help"));
  SetOverrideRedirect(0);
  EvOnShow=EVONSHOW & CPWindow2::_EvOnShow;
  //html1
  html1.SetFOwner(this);
  html1.SetClass(wxT("CHtml"));
  html1.SetName(wxT("html1"));
  html1.SetTag(0);
  html1.SetX(13);
  html1.SetY(10);
  html1.SetWidth(800);
  html1.SetHeight(516);
  html1.SetHint(wxT(""));
  html1.SetEnable(1);
  html1.SetVisible(1);
  html1.SetColor(wxT("#000001"));
  html1.SetPopupMenu(NULL);
  html1.SetLoadText(wxT(""));
  html1.SetLoadUrl(wxT(""));
  html1.SetLoadFile(wxT(""));
  CreateChild(&html1);
  /*#Others*/
//lxrad automatic generated block end, don't edit above!
};
