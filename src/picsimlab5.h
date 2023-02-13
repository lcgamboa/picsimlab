/* ########################################################################

   PICsimLab - PIC laboratory simulator

   ########################################################################

   Copyright (c) : 2010-2021  Luis Claudio Gambôa Lopes

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

#ifndef CPWINDOW5
#define CPWINDOW5

#include <lxrad.h>
#include "spareparts.h"

/**
 * @brief CPWindow5 class
 *
 * class definition of PICSimLab spare parts window.
 */
class CPWindow5 : public CPWindow {
public:
    // lxrad automatic generated block start, don't edit below!
    /*#Controls*/
    CMenu menu1;
    CDraw draw1;
    CPMenu menu1_File;
    CPMenu menu1_Inputs;
    CPMenu menu1_Outputs;
    CPMenu menu1_Others;
    CPMenu menu1_Virtual;
    CPMenu pmenu2;
    CItemMenu pmenu2_Properties;
    CItemMenu pmenu2_Move;
    CItemMenu pmenu2_Rotate;
    CItemMenu pmenu2_Delete;
    CItemMenu pmenu2_Help;
    CItemMenu pmenu2_About;
    CTimer timer1;
    CItemMenu menu1_File_Newconfiguration;
    CItemMenu menu1_File_Saveconfiguration;
    CItemMenu menu1_File_Loadconfiguration;
    CItemMenu menu1_File_Savepinalias;
    CItemMenu menu1_File_Loadpinalias;
    CPMenu menu1_Edit;
    CItemMenu menu1_Edit_Clearpinalias;
    CItemMenu menu1_Edit_Togglepinalias;
    CItemMenu menu1_Edit_Editpinalias;
    CItemMenu menu1_Edit_Reloadpinalias;
    CItemMenu menu1_Edit_Zoomin;
    CItemMenu menu1_Edit_Zoomout;
    CPMenu menu1_Help;
    CItemMenu menu1_Help_Contents;
    CItemMenu menu1_Help_About;
    CFileDialog filedialog1;
    CStatusbar statusbar1;
    /*#Events*/
    void _EvOnCreate(CControl* control);
    void _EvOnShow(CControl* control);
    void _EvOnHide(CControl* control);
    void draw1_EvMouseMove(CControl* control, const uint button, const uint x, const uint y, const uint state);
    void draw1_EvMouseButtonPress(CControl* control, const uint button, const uint x, const uint y, const uint state);
    void draw1_EvMouseButtonRelease(CControl* control, const uint button, const uint x, const uint y, const uint state);
    void draw1_EvKeyboardPress(CControl* control, const uint key, const uint hkey, const uint mask);
    void draw1_EvKeyboardRelease(CControl* control, const uint key, const uint hkey, const uint mask);
    void draw1_EvMouseWheel(CControl* control, const int rotation);
    void pmenu2_Properties_EvMenuActive(CControl* control);
    void timer1_EvOnTime(CControl* control);
    void menu1_File_Newconfiguration_EvMenuActive(CControl* control);
    void menu1_File_Saveconfiguration_EvMenuActive(CControl* control);
    void menu1_File_Loadconfiguration_EvMenuActive(CControl* control);
    void menu1_File_Loadpinalias_EvMenuActive(CControl* control);
    void menu1_File_Savepinalias_EvMenuActive(CControl* control);
    void menu1_Edit_Clearpinalias_EvMenuActive(CControl* control);
    void menu1_Edit_Togglepinalias_EvMenuActive(CControl* control);
    void menu1_Edit_Editpinalias_EvMenuActive(CControl* control);
    void menu1_Edit_Reloadpinalias_EvMenuActive(CControl* control);
    void menu1_Edit_Zoomin_EvMenuActive(CControl* control);
    void menu1_Edit_Zoomout_EvMenuActive(CControl* control);
    void pmenu2_Move_EvMenuActive(CControl* control);
    void pmenu2_Rotate_EvMenuActive(CControl* control);
    void pmenu2_Delete_EvMenuActive(CControl* control);
    void pmenu2_Help_EvMenuActive(CControl* control);
    void pmenu2_About_EvMenuActive(CControl* control);
    void menu1_Help_Contents_EvMenuActive(CControl* control);
    void menu1_Help_About_EvMenuActive(CControl* control);
    void filedialog1_EvOnClose(const int retId);

    /*#Others*/
    CPWindow5(void);
    // lxrad automatic generated block end, don't edit above!
    void menu1_EvMenuActive(CControl* control);

    void PropButtonRelease(CControl* control, uint button, uint x, uint y, uint state);
    void PropComboChange(CCombo* control);
    void PropClose(int tag);
    void PartButtonEvent(CControl* control, uint button, uint x, uint y, uint state);
    void PartKeyEvent(CControl* control, uint keysm, uint ukeysym, uint state);
    void PartEvent(CControl* control);
    void DeleteParts(void);

private:
    CItemMenu MParts[MAX_PARTS];
    lxString PartToCreate;
    int PartSelected;
    int PartToMove;
    int mdx, mdy;
    CPWindow wprop;
    int offsetx;
    int offsety;
    int mouse_scroll;
    int need_resize;
    int update_all;
};

extern CPWindow5 Window5;

#endif /*#CPWINDOW5*/
