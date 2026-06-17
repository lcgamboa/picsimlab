/* ########################################################################

   PICSimLab - Programmable IC Simulator Laboratory

   ########################################################################

   Copyright (c) : 2025-2026  Luis Claudio Gambôa Lopes <lcgamboa@yahoo.com>

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
  SetClass(lxT("CPWindow"));
  SetName(lxT("window2"));
  SetTag(0);
  SetX(282);
  SetY(122);
  SetWidth(830);
  SetHeight(574);
  SetHint(lxT(""));
  SetEnable(1);
  SetVisible(0);
  SetColor(lxT("#000001"));
  SetPopupMenu(NULL);
  SetTitle(lxT("srtank - Help"));
  SetOverrideRedirect(0);
#ifndef EXT_BROWSER
  EvOnShow=EVONSHOW & CPWindow2::_EvOnShow;
  //html1
  html1.SetFOwner(this);
  html1.SetClass(lxT("CHtml"));
  html1.SetName(lxT("html1"));
  html1.SetTag(0);
  html1.SetX(13);
  html1.SetY(10);
  html1.SetWidth(800);
  html1.SetHeight(516);
  html1.SetHint(lxT(""));
  html1.SetEnable(1);
  html1.SetVisible(1);
  html1.SetColor(lxT("#000001"));
  html1.SetPopupMenu(NULL);
  html1.SetLoadText(lxT(""));
  html1.SetLoadUrl(lxT(""));
  html1.SetLoadFile(lxT(""));
  CreateChild(&html1);
#endif
  /*#Others*/
//lxrad automatic generated block end, don't edit above!
};
