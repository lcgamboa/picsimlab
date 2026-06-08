/* ########################################################################

   PICSimLab - Programmable IC Simulator Laboratory

   ########################################################################

   Copyright (c) : 2010-2026 Luis Claudio Gambôa Lopes <lcgamboa@yahoo.com>

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

CPWindow6::CPWindow6(void) {
    // lxrad automatic generated block start, don't edit below!
    SetFOwner(this);
    SetClass(lxT("CPWindow"));
    SetName(lxT("window6"));
    SetTag(0);
    SetX(501);
    SetY(192);
    SetWidth(464);
    SetHeight(367);
    SetHint(lxT(""));
    SetEnable(1);
    SetVisible(0);
    SetPopupMenu(NULL);
    SetTitle(lxT("PICSimLab Project Wizard"));
    SetOverrideRedirect(0);
    EvOnCreate = EVONCREATE & CPWindow6::_EvOnCreate;
    EvOnShow = EVONSHOW & CPWindow6::_EvOnShow;
    // label1
    label1.SetFOwner(this);
    label1.SetClass(lxT("CLabel"));
    label1.SetName(lxT("label1"));
    label1.SetTag(0);
    label1.SetX(75);
    label1.SetY(29);
    label1.SetWidth(317);
    label1.SetHeight(20);
    label1.SetHint(lxT(""));
    label1.SetEnable(1);
    label1.SetVisible(1);
    label1.SetPopupMenu(NULL);
    label1.SetText(lxT("Board: xxx"));
    label1.SetAlign(1);
    CreateChild(&label1);
    // label2
    label2.SetFOwner(this);
    label2.SetClass(lxT("CLabel"));
    label2.SetName(lxT("label2"));
    label2.SetTag(0);
    label2.SetX(75);
    label2.SetY(66);
    label2.SetWidth(317);
    label2.SetHeight(20);
    label2.SetHint(lxT(""));
    label2.SetEnable(1);
    label2.SetVisible(1);
    label2.SetPopupMenu(NULL);
    label2.SetText(lxT("Microcontroller: xxx"));
    label2.SetAlign(1);
    CreateChild(&label2);
    // combo1
    combo1.SetFOwner(this);
    combo1.SetClass(lxT("CCombo"));
    combo1.SetName(lxT("combo1"));
    combo1.SetTag(0);
    combo1.SetX(152);
    combo1.SetY(103);
    combo1.SetWidth(241);
    combo1.SetHeight(26);
    combo1.SetHint(lxT(""));
    combo1.SetEnable(1);
    combo1.SetVisible(1);
    combo1.SetPopupMenu(NULL);
    combo1.SetItems(lxT(""));
    combo1.SetText(lxT(""));
    combo1.SetReadOnly(1);
    combo1.EvOnComboChange = EVONCOMBOCHANGE & CPWindow6::combo1_EvOnComboChange;
    CreateChild(&combo1);
    // label3
    label3.SetFOwner(this);
    label3.SetClass(lxT("CLabel"));
    label3.SetName(lxT("label3"));
    label3.SetTag(0);
    label3.SetX(75);
    label3.SetY(105);
    label3.SetWidth(67);
    label3.SetHeight(20);
    label3.SetHint(lxT(""));
    label3.SetEnable(1);
    label3.SetVisible(1);
    label3.SetPopupMenu(NULL);
    label3.SetText(lxT("IDE:"));
    label3.SetAlign(1);
    CreateChild(&label3);
    // label4
    label4.SetFOwner(this);
    label4.SetClass(lxT("CLabel"));
    label4.SetName(lxT("label4"));
    label4.SetTag(0);
    label4.SetX(73);
    label4.SetY(139);
    label4.SetWidth(70);
    label4.SetHeight(20);
    label4.SetHint(lxT(""));
    label4.SetEnable(1);
    label4.SetVisible(1);
    label4.SetPopupMenu(NULL);
    label4.SetText(lxT("Framework:"));
    label4.SetAlign(1);
    CreateChild(&label4);
    // combo2
    combo2.SetFOwner(this);
    combo2.SetClass(lxT("CCombo"));
    combo2.SetName(lxT("combo2"));
    combo2.SetTag(0);
    combo2.SetX(152);
    combo2.SetY(136);
    combo2.SetWidth(241);
    combo2.SetHeight(26);
    combo2.SetHint(lxT(""));
    combo2.SetEnable(1);
    combo2.SetVisible(1);
    combo2.SetPopupMenu(NULL);
    combo2.SetItems(lxT(""));
    combo2.SetText(lxT(""));
    combo2.SetReadOnly(1);
    combo2.EvOnComboChange = EVONCOMBOCHANGE & CPWindow6::combo2_EvOnComboChange;
    CreateChild(&combo2);
    // label5
    label5.SetFOwner(this);
    label5.SetClass(lxT("CLabel"));
    label5.SetName(lxT("label5"));
    label5.SetTag(0);
    label5.SetX(73);
    label5.SetY(173);
    label5.SetWidth(70);
    label5.SetHeight(20);
    label5.SetHint(lxT(""));
    label5.SetEnable(1);
    label5.SetVisible(1);
    label5.SetPopupMenu(NULL);
    label5.SetText(lxT("Template:"));
    label5.SetAlign(1);
    CreateChild(&label5);
    // combo3
    combo3.SetFOwner(this);
    combo3.SetClass(lxT("CCombo"));
    combo3.SetName(lxT("combo3"));
    combo3.SetTag(0);
    combo3.SetX(152);
    combo3.SetY(170);
    combo3.SetWidth(240);
    combo3.SetHeight(26);
    combo3.SetHint(lxT(""));
    combo3.SetEnable(1);
    combo3.SetVisible(1);
    combo3.SetPopupMenu(NULL);
    combo3.SetItems(lxT(""));
    combo3.SetText(lxT(""));
    combo3.SetReadOnly(1);
    CreateChild(&combo3);
    // button1
    button1.SetFOwner(this);
    button1.SetClass(lxT("CButton"));
    button1.SetName(lxT("button1"));
    button1.SetTag(0);
    button1.SetX(35);
    button1.SetY(246);
    button1.SetWidth(124);
    button1.SetHeight(28);
    button1.SetHint(lxT(""));
    button1.SetEnable(1);
    button1.SetVisible(1);
    button1.SetPopupMenu(NULL);
    button1.EvMouseButtonClick = EVMOUSEBUTTONCLICK & CPWindow6::button1_EvMouseButtonClick;
    button1.SetText(lxT("Create"));
    CreateChild(&button1);
    // button2
    button2.SetFOwner(this);
    button2.SetClass(lxT("CButton"));
    button2.SetName(lxT("button2"));
    button2.SetTag(0);
    button2.SetX(166);
    button2.SetY(246);
    button2.SetWidth(120);
    button2.SetHeight(28);
    button2.SetHint(lxT(""));
    button2.SetEnable(1);
    button2.SetVisible(1);
    button2.SetPopupMenu(NULL);
    button2.EvMouseButtonClick = EVMOUSEBUTTONCLICK & CPWindow6::button2_EvMouseButtonClick;
    button2.SetText(lxT("Create and Open"));
    CreateChild(&button2);
    // button3
    button3.SetFOwner(this);
    button3.SetClass(lxT("CButton"));
    button3.SetName(lxT("button3"));
    button3.SetTag(0);
    button3.SetX(295);
    button3.SetY(246);
    button3.SetWidth(119);
    button3.SetHeight(28);
    button3.SetHint(lxT(""));
    button3.SetEnable(1);
    button3.SetVisible(1);
    button3.SetPopupMenu(NULL);
    button3.EvMouseButtonClick = EVMOUSEBUTTONCLICK & CPWindow6::button3_EvMouseButtonClick;
    button3.SetText(lxT("Cancel"));
    CreateChild(&button3);
    // label6
    label6.SetFOwner(this);
    label6.SetClass(lxT("CLabel"));
    label6.SetName(lxT("label6"));
    label6.SetTag(0);
    label6.SetX(74);
    label6.SetY(210);
    label6.SetWidth(71);
    label6.SetHeight(20);
    label6.SetHint(lxT(""));
    label6.SetEnable(1);
    label6.SetVisible(1);
    label6.SetPopupMenu(NULL);
    label6.SetText(lxT("Prj. name:"));
    label6.SetAlign(1);
    CreateChild(&label6);
    // edit1
    edit1.SetFOwner(this);
    edit1.SetClass(lxT("CEdit"));
    edit1.SetName(lxT("edit1"));
    edit1.SetTag(0);
    edit1.SetX(152);
    edit1.SetY(205);
    edit1.SetWidth(240);
    edit1.SetHeight(26);
    edit1.SetHint(lxT(""));
    edit1.SetEnable(1);
    edit1.SetVisible(1);
    edit1.SetPopupMenu(NULL);
    edit1.SetText(lxT("Untitled"));
    edit1.SetReadOnly(0);
    CreateChild(&edit1);
    // dirdialog1
    dirdialog1.SetFOwner(this);
    dirdialog1.SetClass(lxT("CDirDialog"));
    dirdialog1.SetName(lxT("dirdialog1"));
    dirdialog1.SetTag(0);
    dirdialog1.SetType(129);
    dirdialog1.EvOnClose = EVONCLOSE & CPWindow6::dirdialog1_EvOnClose;
    CreateChild(&dirdialog1);
    /*#Others*/
    // lxrad automatic generated block end, don't edit above!
};
