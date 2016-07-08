/* ########################################################################

   PICsimLab - PIC laboratory simulator

   ########################################################################

   Copyright (c) : 2010-2015  Luis Claudio Gambôa Lopes

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

CPWindow2::CPWindow2(void)
{
//lxrad automatic generated block start, don't edit below!
  SetFOwner(this);
  SetClass(wxT("CPWindow"));
  SetName(wxT("window2"));
  SetTag(0);
  SetX(312);
  SetY(167);
  SetWidth(733);
  SetHeight(773);
  SetHint(wxT(""));
  SetEnable(1);
  SetVisible(0);
  SetPopupMenu(NULL);
  SetTitle(wxT("picsimlab - Help"));
  SetOverrideRedirect(0);
  EvOnShow=EVONSHOW & CPWindow2::_EvOnShow;
  //html1
  html1.SetFOwner(this);
  html1.SetClass(wxT("CHtml"));
  html1.SetName(wxT("html1"));
  html1.SetTag(0);
  html1.SetX(5);
  html1.SetY(5);
  html1.SetWidth(695);
  html1.SetHeight(713);
  html1.SetHint(wxT(""));
  html1.SetEnable(1);
  html1.SetVisible(1);
  html1.SetPopupMenu(NULL);
  html1.SetLoadText(wxT(""));
  html1.SetLoadUrl(wxT(""));
  html1.SetLoadFile(wxT(""));
  CreateChild(&html1);
  /*#Others*/
//lxrad automatic generated block end, don't edit above!
};
