/* ########################################################################

   LXRAD -  GUI for X programing

   ########################################################################

   Copyright (c) : Luis Claudio Gamboa Lopes

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


#include"simple1.h"
#include"simple1_d.cc"

CPWindow1 Window1;

//Implementation
void
CPWindow1::button1_EvMouseButtonPress(CControl * control, uint button, uint x, uint y,uint state)
{
//Add
  label1.SetText(lxT("+"));
  edit3.SetText(ftoa(atof(edit1.GetText())+atof((edit2.GetText()))));
};

void
CPWindow1::button2_EvMouseButtonPress(CControl * control, uint button, uint x, uint y,uint state)
{
//Sub
  label1.SetText(lxT("-"));
  edit3.SetText(ftoa(atof(edit1.GetText())-atof((edit2.GetText()))));
};

void
CPWindow1::button3_EvMouseButtonPress(CControl * control, uint button, uint x, uint y,uint state)
{
//Mul
  label1.SetText(lxT("*"));
  edit3.SetText(ftoa(atof(edit1.GetText())*atof((edit2.GetText()))));
};

void
CPWindow1::button4_EvMouseButtonPress(CControl * control, uint button, uint x, uint y,uint state)
{
//Div
  label1.SetText(lxT("/"));
  edit3.SetText(ftoa(atof(edit1.GetText())/atof((edit2.GetText()))));
};

void
CPWindow1::button5_EvMouseButtonPress(CControl * control, uint button, uint x, uint y,uint state)
{
  Window1.WDestroy();
};

void
CPWindow1::_EvMouseButtonPress(CControl * control, uint button, uint x, uint y,uint state)
{
 Message(lxT("You clicked in the Window!"));
};


void
CPWindow1::menu1_File_Exit_EvMenuActive(CControl * control)
{
  Window1.WDestroy();
};

void
CPWindow1::menu1_Help_About_EvMenuActive(CControl * control)
{
   Message(lxT("LXRAD Simple Program by LCGamboa"));
};











