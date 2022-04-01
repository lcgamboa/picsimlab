/* ########################################################################

   PICsimLab - PIC laboratory simulator

   ########################################################################

   Copyright (c) : 2010-2022  Luis Claudio Gambôa Lopes

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

// Spare parts

#include "picsimlab5.h"
#include "picsimlab1.h"
#include "picsimlab2.h"
#include "picsimlab4.h"
#include "picsimlab5_d.cc"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

CPWindow5 Window5;

// Implementation

void CPWindow5::_EvOnShow(CControl* control) {
    if (Visible) {
        need_resize = 0;
        timer1.SetRunState(1);
        for (int i = 0; i < partsc; i++) {
            parts[i]->SetUpdate(1);
#if defined(_LX_SDL2) || defined(__EMSCRIPTEN__)
            parts[i]->SetScale(parts[i]->GetScale() + 1e-3);
#endif
        }
    }
}

void CPWindow5::menu1_EvMenuActive(CControl* control) {
    PartToCreate = ((CItemMenu*)control)->GetText();
    lxSetCursor(lxCursor(lxCURSOR_CROSS));
}

void CPWindow5::_EvOnCreate(CControl* control) {
    if (LoadConfigFile.length() > 0)
        LoadConfig(LoadConfigFile);

    for (int i = 0; i < NUM_PARTS; i++) {
        MParts[i].SetFOwner(this);
        MParts[i].SetName(parts_list[i].name);
        MParts[i].SetText(parts_list[i].name);
        MParts[i].EvMenuActive = EVMENUACTIVE & CPWindow5::menu1_EvMenuActive;

        if (!strcmp(parts_list[i].menu, "Input")) {
            menu1_Inputs.CreateChild(&MParts[i]);
        } else if (!strcmp(parts_list[i].menu, "Output")) {
            menu1_Outputs.CreateChild(&MParts[i]);
        } else if (!strcmp(parts_list[i].menu, "Virtual")) {
            menu1_Virtual.CreateChild(&MParts[i]);
        } else {
            menu1_Others.CreateChild(&MParts[i]);
        }
    }
}

void CPWindow5::draw1_EvMouseButtonPress(CControl* control, uint button, uint x, uint y, uint state) {
    x = x / scale;
    y = y / scale;

    for (int i = 0; i < partsc; i++) {
        if (parts[i]->PointInside((int)(x - offsetx), (int)(y - offsety))) {
            parts[i]->EvMouseButtonPress(button, (x - offsetx) - parts[i]->GetX(), (y - offsety) - parts[i]->GetY(),
                                         state);
            if (button == 3) {
                PartSelected = i;
                pmenu2.SetX(x * scale);
                pmenu2.SetY(y * scale);
#if defined(__WXGTK__) || defined(__WXMSW__)
                SetPopupMenu(&pmenu2);
#else
                draw1.SetPopupMenu(&pmenu2);
#endif
            }
            return;
        }
    }

    // clique fora
    if ((button == 1) && (PartToCreate.size() > 0) && (partsc < MAX_PARTS)) {
        // timer1.SetRunState (0);
        lxSetCursor(lxCursor(lxCURSOR_ARROW));
        AddPart((char*)PartToCreate.char_str(), x - offsetx, y - offsety);
        PartToCreate = "";
        _EvOnShow(control);
        return;
    }

    if (button == 1) {
        lxSetCursor(lxCursor(lxCURSOR_SIZENWSE));
        mouse_scroll = 1;
        mdx = x;
        mdy = y;
    } else if (button == 3) {
        offsetx = 0;
        offsety = 0;
        update_all = 1;
    }
}

part* CPWindow5::AddPart(const char* partname, const int x, const int y) {
    part* newpart = create_part(partname, x, y);

    parts[partsc] = newpart;

    if (parts[partsc] == NULL) {
        Message_sz(lxT("Erro creating part: ") + lxString(partname), 400, 200);
    } else {
        parts[partsc]->SetId(partsc);
        parts[partsc]->SetScale(scale);
        parts[partsc]->Reset();
        partsc++;
    }

    return newpart;
}

void CPWindow5::draw1_EvMouseButtonRelease(CControl* control, uint button, uint x, uint y, uint state) {
    x = x / scale;
    y = y / scale;

    PartToMove = -1;

    mouse_scroll = 0;

    lxSetCursor(lxCursor(lxCURSOR_ARROW));
    mdx = 0;
    mdy = 0;

    for (int i = 0; i < partsc; i++) {
        if (parts[i]->PointInside(x - offsetx, y - offsety)) {
            parts[i]->EvMouseButtonRelease(button, (x - offsetx) - parts[i]->GetX(), (y - offsety) - parts[i]->GetY(),
                                           state);
            return;
        }
    }
}

void CPWindow5::pmenu2_Properties_EvMenuActive(CControl* control) {
    lxString fname = Window1.GetSharePath() + lxT("parts/") + parts[PartSelected]->GetPropertiesWindowFile();

    if (lxFileExists(fname)) {
        wprop.SetName("window1");  // must be the same as in xml
        Application->ACreateWindow(&wprop);
        wprop.DestroyChilds();
        if (wprop.LoadXMLContextAndCreateChilds(fname)) {
            // wprop.SetCanDestroy (false);

            parts[PartSelected]->ConfigurePropertiesWindow(&wprop);

            wprop.SetX(parts[PartSelected]->GetX() + GetX() - offsetx);
            wprop.SetY(parts[PartSelected]->GetY() + GetY() - offsety);

            wprop.Draw();
            wprop.ShowExclusive();
        }
    } else {
        Window1.RegisterError("File " + fname + " not found!");
    }
}

void CPWindow5::PropClose(int tag) {
    if (tag) {
        parts[PartSelected]->ReadPropertiesWindow(&wprop);
    }
    wprop.HideExclusive();
    // wprop.SetCanDestroy (true);
    wprop.WDestroy();
    update_all = 1;
}

void CPWindow5::PropButtonRelease(CControl* control, uint button, uint x, uint y, uint state) {
    switch (control->GetTag()) {
        case 0:
        case 1:
            Window5.PropClose(control->GetTag());
            break;
        default:  // browse filedialog
            Window5.filedialog1.SetType(lxFD_OPEN | lxFD_CHANGE_DIR);
            Window5.filedialog1.SetFilter(lxT("All Files (*.*)|*.*"));
            Window5.Setfdtype(control->GetTag() - 2);
            Window5.filedialog1.Run();
            break;
    }
}

void CPWindow5::PropComboChange(CCombo* control) {
    Window5.wprop.HideExclusive();
    // Window5.wprop.SetCanDestroy (true);
    Window5.wprop.WDestroy();

    Window5.parts[Window5.PartSelected]->ComboChange(control, control->GetText());

    Window5.pmenu2_Properties_EvMenuActive(this);
}

void CPWindow5::PartButtonEvent(CControl* control, uint button, uint x, uint y, uint state) {
    if (control->GetTag() < (unsigned int)Window5.partsc) {
        Window5.parts[control->GetTag()]->ButtonEvent(control, button, x, y, state);
    }
}

void CPWindow5::PartKeyEvent(CControl* control, uint keysym, uint ukeysym, uint state) {
    if (control->GetTag() < (unsigned int)Window5.partsc) {
        Window5.parts[control->GetTag()]->KeyEvent(control, keysym, ukeysym, state);
    }
}

void CPWindow5::PartEvent(CControl* control) {
    if (control->GetTag() < (unsigned int)Window5.partsc) {
        Window5.parts[control->GetTag()]->Event(control);
    }
}

void CPWindow5::timer1_EvOnTime(CControl* control) {
    static int tc = 0;
    int update = 0;

    if (need_resize == 1) {
        draw1.SetWidth(Width - 15);
        // draw1.SetHeight (Height - 40);
#ifdef _WIN_
        draw1.SetHeight(Height - 75);
#else
        draw1.SetHeight(Height - 90);
#endif
        Window4.SetBaseTimer();

        update_all = 1;
    }

    need_resize++;

    for (int i = 0; i < partsc; i++) {
        parts[i]->Draw();
        if (parts[i]->GetUpdate())
            update++;
    }

    if (update_all) {
        for (int i = 0; i < partsc; i++) {
            parts[i]->SetUpdate(1);
        }
        update = 1;
    }

    // printf ("part update %i\n", update);

    if (update) {
        draw1.Canvas.Init(1.0, 1.0);

        if (update_all) {
            draw1.Canvas.SetFgColor(50, 50, 50);
            draw1.Canvas.SetBgColor(50, 50, 50);
            draw1.Canvas.Rectangle(1, 0, 0, draw1.GetWidth(), draw1.GetHeight());
            update_all = 0;
        }

        for (int i = 0; i < partsc; i++) {
            if (parts[i]->GetUpdate()) {
                draw1.Canvas.PutBitmap(parts[i]->GetBitmap(), (parts[i]->GetX() + offsetx) * scale,
                                       (parts[i]->GetY() + offsety) * scale);
            }
        }

        draw1.Canvas.End();
        draw1.Update();
    }

    tc++;

    if (tc > 3) {
        tc = 0;
        lxString field;
        field.Printf("Use Alias: %s", (useAlias == 1) ? "On" : "Off");
        statusbar1.SetField(0, field);

        field.Printf("Scale: %3.1f", scale);
        statusbar1.SetField(1, field);

        field.Printf("Offset: %3i %3i", offsetx, offsety);
        statusbar1.SetField(2, field);
    }
}

void CPWindow5::draw1_EvMouseWheel(CControl* control, const int rotation) {
    if (rotation > 0) {
        menu1_Edit_Zoomin_EvMenuActive(this);
    } else {
        menu1_Edit_Zoomout_EvMenuActive(this);
    }
}

void CPWindow5::draw1_EvMouseMove(CControl* control, uint button, uint x, uint y, uint state) {
    x = x / scale;
    y = y / scale;

    if (mouse_scroll) {
        offsetx -= mdx - x;
        offsety -= mdy - y;
        mdx = x;
        mdy = y;
        update_all = 1;
    } else if (PartToMove >= 0) {
        if ((mdx == 0) && (mdy == 0)) {
            mdx = parts[PartToMove]->GetX() - x;
            mdy = parts[PartToMove]->GetY() - y;
        }

        parts[PartToMove]->SetX(x + mdx);
        parts[PartToMove]->SetY(y + mdy);
        update_all = 1;
    } else {
        for (int i = 0; i < partsc; i++) {
            if (parts[i]->PointInside(x - offsetx, y - offsety)) {
                parts[i]->EvMouseMove(button, (x - offsetx) - parts[i]->GetX(), (y - offsety) - parts[i]->GetY(),
                                      state);
                return;
            }
        }
    }
}

void CPWindow5::draw1_EvKeyboardPress(CControl* control, const uint key, const uint hkey, const uint mask) {
    switch (key) {
        case 'P':
        case 'p':
            useAlias = !useAlias;
            update_all = 1;
            Window4.SetBaseTimer();
            break;
        case '=':  //+
            menu1_Edit_Zoomin_EvMenuActive(this);
            break;
        case '-':
            menu1_Edit_Zoomout_EvMenuActive(this);
            break;
        case LXK_UP:
            offsety += 10;
            update_all = 1;
            break;
        case LXK_DOWN:
            offsety -= 10;
            update_all = 1;
            break;
        case LXK_LEFT:
            offsetx += 10;
            update_all = 1;
            break;
        case LXK_RIGHT:
            offsetx -= 10;
            update_all = 1;
            break;
        default:
            for (int i = 0; i < partsc; i++) {
                parts[i]->EvKeyPress(key, mask);
            }
            break;
    }
}

void CPWindow5::draw1_EvKeyboardRelease(CControl* control, const uint key, const uint hkey, const uint mask) {
    for (int i = 0; i < partsc; i++) {
        parts[i]->EvKeyRelease(key, mask);
    }
}

bool CPWindow5::SaveConfig(lxString fname) {
    lxString temp;

    lxStringList prefs;

    if (GetWin() == NULL)
        return 0;

    prefs.Clear();

    temp.Printf("version,0,0,0:%s", _VERSION_);
    prefs.AddLine(temp);
    temp.Printf("scale,0,0,0:%f", scale);
    prefs.AddLine(temp);
    temp.Printf("useAlias,0,0,0:%i", useAlias);
    prefs.AddLine(temp);

    for (int i = 0; i < partsc; i++) {
        temp.Printf("%s,%i,%i,%i:%s", parts[i]->GetName().c_str(), parts[i]->GetX(), parts[i]->GetY(),
                    parts[i]->GetOrientation(), parts[i]->WritePreferences().c_str());
        prefs.AddLine(temp);
    }

    return prefs.SaveToFile(fname);
}

bool CPWindow5::LoadConfig(lxString fname) {
    char name[256];
    char temp[256];
    unsigned int x, y;
    int orient;
    lxStringList prefs;
    int newformat = 0;

    pboard = Window1.GetBoard();

    for (int i = 0; i < 256; i++) {
        if ((i > 0) && (i <= pboard->MGetPinCount())) {
            PinNames[i] = pboard->MGetPinName(i);
        } else {
            PinNames[i] = "";
        }
    }

    PinsCount = pboard->MGetPinCount();
    Pins = (picpin*)pboard->MGetPinsValues();

    memset(&Pins[PinsCount], 0, sizeof(picpin) * (256 - PinsCount));

    if (GetWin() == NULL) {
        LoadConfigFile = fname;
        menu1_Edit_Clearpinalias_EvMenuActive(this);
        return 0;
    } else {
        LoadConfigFile = "";
    }

    bool ret = lxFileExists(fname);

    if (ret) {
        int partsc_;
        int partsc_aup_;
        prefs.LoadFromFile(fname);

        DeleteParts();
        partsc_ = 0;
        partsc_aup_ = 0;

        if (prefs.GetLine(0).Contains("version")) {
            newformat = 1;
        }

        for (unsigned int i = 0; i < prefs.GetLinesCount(); i++) {
            if (newformat) {
                sscanf(prefs.GetLine(i).c_str(), "%255[^,],%i,%i,%i:%255[^\n]", name, &x, &y, &orient, temp);
            } else {
                sscanf(prefs.GetLine(i).c_str(), "%255[^,],%i,%i:%255[^\n]", name, &x, &y, temp);
            }

            // typo fix
            if (!strcmp(name, "Switchs")) {
                strcpy(name, "Switches");
            } else if (!strcmp(name, "IO Virtual term")) {
                strcpy(name, "IO Virtual Term");
            } else if (!strcmp(name, "Push buttons")) {
                strcpy(name, "Push Buttons");
            } else if (!strcmp(name, "Push buttons (Analogic)")) {
                strcpy(name, "Push Buttons (Analogic)");
            }

            if (!strcmp(name, "scale")) {
                sscanf(temp, "%f", &scale);
            } else if (!strcmp(name, "useAlias")) {
                sscanf(temp, "%hhu", &useAlias);
                Window4.SetBaseTimer();
            } else if (!strcmp(name, "version")) {
                // use planed in future
            } else if ((parts[partsc_] = create_part(name, x, y))) {
                parts[partsc_]->ReadPreferences(temp);
                parts[partsc_]->SetId(partsc_);
                if (newformat) {
                    parts[partsc_]->SetOrientation(orient);
                    parts[partsc_]->SetScale(scale);
                }
                partsc_++;
            } else {
                printf("Erro loading part: %s \n", name);
                lxString temp;
                temp.Printf("Spare parts:\nErro loading part: %s \n", name);
                Window1.RegisterError(temp);
            }
        }
        partsc = partsc_;
        partsc_aup = partsc_aup_;
    }

    update_all = 1;

    return ret;
}

bool CPWindow5::SavePinAlias(lxString fname) {
    lxString temp;
    lxString pin;
    lxString alias;
    lxStringList lalias;
    lalias.Clear();
    lalias.AddLine(
        "//N-PinName -ALias   --The pin name alias must start in column fourteen and have size less than seven chars ");
    for (int i = 1; i < 256; i++) {
        pin = PinNames[i].substr(0, 7);
        if (!pin.size())
            pin = " ";
        alias = PinAlias[i].substr(0, 7);
        if (!alias.size())
            alias = " ";
        temp.Printf("%03i-%-7s -%-7s", i, pin.c_str(), alias.c_str());
        lalias.AddLine(temp);
    }
    return lalias.SaveToFile(fname);
}

bool CPWindow5::LoadPinAlias(lxString fname, unsigned char show_error_msg) {
    if (!show_error_msg) {
        if (!lxFileExists(fname)) {
            return 0;
        }
    }
    lxStringList alias;
    lxString line;
    alias.Clear();
    if (alias.LoadFromFile(fname)) {
        alias_fname = fname;

        for (int i = 0; i < 256; i++) {
            line = alias.GetLine(i);
            if (line.size() > 13) {
                PinAlias[i] = line.substr(13, 7);
            } else {
                PinAlias[i] = "";
            }
        }
        PinAlias[0] = "NC";
        if (show_error_msg) {
            useAlias = 1;
            update_all = 1;
            Window4.SetBaseTimer();
        }
        return 1;
    } else {
        return 0;
    }
}

void CPWindow5::DeleteParts(void) {
    int partsc_ = partsc;
    partsc = 0;  // for disable process
    partsc_aup = 0;

    scale = 1.0;
    useAlias = 0;
    if (Window4.GetVisible()) {
        Window4.SetBaseTimer();
    }
    // delete previous parts

    for (int i = 0; i < partsc_; i++) {
        delete parts[i];
    }
}

void CPWindow5::menu1_File_Newconfiguration_EvMenuActive(CControl* control) {
    if (partsc > 0) {
#ifndef __EMSCRIPTEN__
        if (Dialog_sz("Save current configuration?", 400, 200)) {
            menu1_File_Saveconfiguration_EvMenuActive(control);
        }
#endif
        DeleteParts();
        update_all = 1;
    }
}

void CPWindow5::menu1_File_Saveconfiguration_EvMenuActive(CControl* control) {
    filedialog1.SetType(lxFD_SAVE | lxFD_CHANGE_DIR);
    fdtype = -1;
#ifdef __EMSCRIPTEN__
    filedialog1.SetDir("/tmp/");
    filedialog1.SetFileName("untitled.pcf");
    filedialog1_EvOnClose(1);
#else
    fdtype = -1;
    filedialog1.SetFileName(oldfname);
    filedialog1.SetFilter(lxT("PICSimLab Config. (*.pcf)|*.pcf"));
    filedialog1.Run();
#endif
}

void CPWindow5::menu1_File_Loadconfiguration_EvMenuActive(CControl* control) {
#ifdef __EMSCRIPTEN__
    EM_ASM_({ toggle_load_panel(); });
#else
    filedialog1.SetType(lxFD_OPEN | lxFD_CHANGE_DIR);
    fdtype = -1;
    filedialog1.SetFileName(oldfname);
    filedialog1.SetFilter(lxT("PICSimLab Config. (*.pcf)|*.pcf"));
    filedialog1.Run();
#endif
}

void CPWindow5::menu1_File_Savepinalias_EvMenuActive(CControl* control) {
    filedialog1.SetType(lxFD_SAVE | lxFD_CHANGE_DIR);
    fdtype = -1;
#ifdef __EMSCRIPTEN__
    filedialog1.SetDir("/tmp/");
    filedialog1.SetFileName("untitled.ppa");
    filedialog1_EvOnClose(1);
#else
    fdtype = -2;
    filedialog1.SetFileName(oldfname);
    filedialog1.SetFilter(lxT("PICSimLab Pin Alias. (*.ppa)|*.ppa"));
    filedialog1.Run();
#endif
}

void CPWindow5::menu1_File_Loadpinalias_EvMenuActive(CControl* control) {
#ifdef __EMSCRIPTEN__
    EM_ASM_({ toggle_load_panel(); });
#else
    filedialog1.SetType(lxFD_OPEN | lxFD_CHANGE_DIR);
    fdtype = -2;
    filedialog1.SetFileName(oldfname);
    filedialog1.SetFilter(lxT("PICSimLab Pin Alias. (*.ppa)|*.ppa"));
    filedialog1.Run();
#endif
}

void CPWindow5::menu1_Edit_Clearpinalias_EvMenuActive(CControl* control) {
    for (int i = 0; i < 256; i++) {
        PinAlias[i] = PinNames[i];
    }
    update_all = 1;
}

void CPWindow5::menu1_Edit_Togglepinalias_EvMenuActive(CControl* control) {
    useAlias = !useAlias;
    update_all = 1;
    Window4.SetBaseTimer();
}

void CPWindow5::menu1_Edit_Editpinalias_EvMenuActive(CControl* control) {
    if (lxFileExists(alias_fname)) {
        SavePinAlias(alias_fname);
#ifdef _WIN_
        lxExecute(lxT("notepad.exe ") + alias_fname);
#else
        lxExecute("gedit " + alias_fname, lxEXEC_MAKE_GROUP_LEADER);
#endif
    } else {
        Message_sz("Pin alias file don't exist!", 400, 200);
    }
}

void CPWindow5::menu1_Edit_Reloadpinalias_EvMenuActive(CControl* control) {
    LoadPinAlias(alias_fname);
    update_all = 1;
}

void CPWindow5::menu1_Edit_Zoomin_EvMenuActive(CControl* control) {
    scale += 0.1;
    if (scale > 2)
        scale = 2;

    scale = trunc(scale * 10) / 10.0;

    for (int i = 0; i < partsc; i++) {
        parts[i]->SetScale(scale);
    }
    update_all = 1;
}

void CPWindow5::menu1_Edit_Zoomout_EvMenuActive(CControl* control) {
    scale -= 0.1;
    if (scale < 0.1)
        scale = 0.1;

    scale = trunc(scale * 10) / 10.0;

    for (int i = 0; i < partsc; i++) {
        parts[i]->SetScale(scale);
    }
    update_all = 1;
}

void CPWindow5::PreProcess(void) {
    int i;

    memset(i2c_bus, 0, PinsCount);

    partsc_aup = 0;
    for (i = 0; i < partsc; i++) {
        parts[i]->PreProcess();
        if (parts[i]->GetAwaysUpdate()) {
            parts_aup[partsc_aup] = parts[i];
            partsc_aup++;
        }
    }

    i2c_bus_count = 0;
    for (i = 0; i < PinsCount; i++) {
        if (i2c_bus[i] > 0)  // need register bus
        {
            i2c_bus_ptr[i2c_bus_count] = i;
            i2c_bus_count++;
        }
        i2c_bus[i] = 0;
    }
}

void CPWindow5::Process(void) {
    int i;

    if (ioupdated) {
        for (i = 0; i < i2c_bus_count; i++) {
            i2c_bus[i2c_bus_ptr[i]] = 1;
        }
        for (i = 0; i < partsc; i++) {
            parts[i]->Process();
        }
        for (i = 0; i < i2c_bus_count; i++) {
            Window5.SetPin(i2c_bus_ptr[i] + 1, i2c_bus[i2c_bus_ptr[i]]);
        }
    } else {
        for (i = 0; i < partsc_aup; i++) {
            parts_aup[i]->Process();
        }
    }
}

void CPWindow5::PostProcess(void) {
    for (int i = 0; i < partsc; i++) {
        parts[i]->PostProcess();
    }
}

void CPWindow5::Reset(void) {
    for (int i = 0; i < partsc; i++) {
        parts[i]->Reset();
    }
    update_all = 1;
}

void CPWindow5::_EvOnHide(CControl* control) {
    timer1.SetRunState(0);
    pboard = Window1.GetBoard();
    if (pboard) {
        pboard->SetUseSpareParts(0);
    }
    if (Window4.GetVisible()) {
        Window4.SetBaseTimer();
    }
    for (int i = 0; i < partsc; i++) {
        parts[i]->Stop();
    }
}

void CPWindow5::pmenu2_Move_EvMenuActive(CControl* control) {
    PartToMove = PartSelected;
    lxSetCursor(lxCursor(lxCURSOR_SIZENWSE));
}

void CPWindow5::pmenu2_Rotate_EvMenuActive(CControl* control) {
    int orientation = parts[PartSelected]->GetOrientation();

    orientation++;
    if (orientation > 3)
        orientation = 0;

    parts[PartSelected]->SetOrientation(orientation);

    update_all = 1;
}

void CPWindow5::pmenu2_Delete_EvMenuActive(CControl* control) {
    int partsc_ = partsc;
    partsc = 0;  // disable process
    partsc_aup = 0;

    delete parts[PartSelected];

    for (int i = PartSelected; i < partsc_ - 1; i++) {
        parts[i] = parts[i + 1];
    }
    partsc_--;

    partsc = partsc_;

    update_all = 1;
}

void CPWindow5::pmenu2_Help_EvMenuActive(CControl* control) {
    lxString stemp;
    stemp.Printf(lxT("https://lcgamboa.github.io/picsimlab_docs/%s/%s"), lxT(_VERSION_),
                 (const char*)parts[PartSelected]->GetHelpURL().c_str());
    lxLaunchDefaultBrowser(stemp);
}

void CPWindow5::pmenu2_About_EvMenuActive(CControl* control) {
    Message_sz(
        lxT("Part ") + parts[PartSelected]->GetName() + lxT("\nDeveloped by ") + parts[PartSelected]->GetAboutInfo(),
        400, 200);
}

void CPWindow5::menu1_Help_Contents_EvMenuActive(CControl* control) {
#ifdef EXT_BROWSER
    // lxLaunchDefaultBrowser(lxT("file://")+share + lxT ("docs/picsimlab.html"));
    lxString stemp;
    stemp.Printf(lxT("https://lcgamboa.github.io/picsimlab_docs/%s/SpareParts.html"), lxT(_VERSION_));
    lxLaunchDefaultBrowser(stemp);
#else
    Window2.html1.SetLoadFile(Window1.GetSharePath() + lxT("docs/picsimlab.html"));
    Window2.Show();
#endif
}

void CPWindow5::menu1_Help_About_EvMenuActive(CControl* control) {
    lxString stemp;
    stemp.Printf(lxT("Developed by L.C. Gamboa\n <lcgamboa@yahoo.com>\n Version: %s %s %s"), lxT(_VERSION_),
                 lxT(_DATE_), lxT(_ARCH_));
    Message_sz(stemp, 400, 200);
}

void CPWindow5::WritePreferences(void) {
    Window1.saveprefs(lxT("spare_position"), itoa(GetX()) + lxT(",") + itoa(GetY()) + lxT(",") + itoa(GetWidth()) +
                                                 lxT(",") + itoa(GetHeight()));
}

void CPWindow5::ReadPreferences(char* name, char* value) {
    if (!strcmp(name, "spare_position")) {
        int x, y, w, h;
        sscanf(value, "%i,%i,%i,%i", &x, &y, &w, &h);
        SetX(x);
        SetY(y);
        if (w > 5000)
            w = 5000;
        if (h > 5000)
            h = 5000;
        SetWidth(w);
        SetHeight(h);
        draw1.SetWidth(w - 15);
        draw1.SetHeight(h - 40);
    }
}

void CPWindow5::filedialog1_EvOnClose(int retId) {
    if (retId && (fdtype == -1)) {
        if ((filedialog1.GetType() == (lxFD_SAVE | lxFD_CHANGE_DIR))) {
            if (lxFileExists(filedialog1.GetFileName())) {
                if (!Dialog_sz(lxString("Overwriting file: ") + basename(filedialog1.GetFileName()) + "?", 400, 200))
                    return;
            }
            SaveConfig(filedialog1.GetFileName());
#ifdef __EMSCRIPTEN__
            EM_ASM_(
                {
                    var filename = UTF8ToString($0);
                    var buf = FS.readFile(filename);
                    var blob = new Blob([buf], { "type" : "application/octet-stream" });
                    var text = URL.createObjectURL(blob);

                    var element = document.createElement('a');
                    element.setAttribute('href', text);
                    element.setAttribute('download', filename);

                    element.style.display = 'none';
                    document.body.appendChild(element);

                    element.click();

                    document.body.removeChild(element);
                    URL.revokeObjectURL(text);
                },
                filedialog1.GetFileName().c_str());
#endif
        }

        if ((filedialog1.GetType() == (lxFD_OPEN | lxFD_CHANGE_DIR))) {
            LoadConfig(filedialog1.GetFileName());
        }
    } else if (retId && (fdtype == -2)) {
        if ((filedialog1.GetType() == (lxFD_SAVE | lxFD_CHANGE_DIR))) {
            if (lxFileExists(filedialog1.GetFileName())) {
                if (!Dialog_sz(lxString("Overwriting file: ") + basename(filedialog1.GetFileName()) + "?", 400, 200))
                    return;
            }
            SavePinAlias(filedialog1.GetFileName());
#ifdef __EMSCRIPTEN__
            EM_ASM_(
                {
                    var filename = UTF8ToString($0);
                    var buf = FS.readFile(filename);
                    var blob = new Blob([buf], { "type" : "application/octet-stream" });
                    var text = URL.createObjectURL(blob);

                    var element = document.createElement('a');
                    element.setAttribute('href', text);
                    element.setAttribute('download', filename);

                    element.style.display = 'none';
                    document.body.appendChild(element);

                    element.click();

                    document.body.removeChild(element);
                    URL.revokeObjectURL(text);
                },
                filedialog1.GetFileName().c_str());
#endif
        }

        if ((filedialog1.GetType() == (lxFD_OPEN | lxFD_CHANGE_DIR))) {
            LoadPinAlias(filedialog1.GetFileName(), 1);
        }
    } else if (fdtype >= 0) {
        parts[fdtype]->filedialog_EvOnClose(retId);
        fdtype = -1;
    }
}

void CPWindow5::Reset_i2c_bus(unsigned char pin) {
    if (pin < IOINIT) {
        i2c_bus[pin]++;  // count i2c devices in bus
    }
}

void CPWindow5::Set_i2c_bus(unsigned char pin, unsigned char value) {
    if (pin < IOINIT) {
        i2c_bus[pin] &= value;
    }
}

unsigned char CPWindow5::Get_i2c_bus(unsigned char pin) {
    if (pin < IOINIT)
        return i2c_bus[pin];
    else
        return 0;
}

lxString CPWindow5::GetPinsNames(void) {
    lxString Items = "0  NC,";
    lxString spin;

    for (int i = 1; i <= pboard->MGetPinCount(); i++) {
        if (useAlias) {
            spin = PinAlias[i];
        } else {
            spin = PinNames[i];
        }
        if (PinNames[i].Cmp(lxT("error"))) {
            Items = Items + itoa(i) + "  " + spin + ",";
        }
    }
    for (int i = IOINIT; i < 256; i++) {
        if (useAlias) {
            spin = PinAlias[i];
        } else {
            spin = PinNames[i];
        }
        if (PinNames[i].length() > 0) {
            Items = Items + itoa(i) + "  " + spin + ",";
        }
    }
    return Items;
}

lxString CPWindow5::GetPinName(unsigned char pin) {
    if (!pin)
        return "NC";
    if (!pboard)
        return "NC";

    if (pin <= pboard->MGetPinCount()) {
        if (useAlias) {
            return PinAlias[pin];
        } else {
            return PinNames[pin];
        }
    } else {
        if (useAlias) {
            return PinAlias[pin];
        } else {
            return PinNames[pin] + "-" + itoa(pin);
        }
    }
}

const picpin* CPWindow5::GetPinsValues(void) {
    return Pins;
}

void CPWindow5::SetPin(unsigned char pin, unsigned char value) {
    if (pin) {
        if ((pin > PinsCount)) {
            Pins[pin - 1].lsvalue = value;  // for open collector simulation
        }

        if ((Pins[pin - 1].dir) && ((Pins[pin - 1].value != value))) {
            if ((pin > PinsCount)) {
                Pins[pin - 1].value = value;
            } else {
                pboard->MSetPin(pin, value);
            }
        }
    }
}

void CPWindow5::SetPinDOV(unsigned char pin, unsigned char ovalue) {
    if (pin) {
        if (Pins[pin - 1].ovalue != ovalue) {
            if ((pin > PinsCount)) {
                Pins[pin - 1].ovalue = ovalue;
            } else {
                pboard->MSetPinDOV(pin, ovalue);
            }
        }
    }
}

void CPWindow5::SetPinDir(unsigned char pin, unsigned char dir) {
    if (pin) {
        if (Pins[pin - 1].dir != dir) {
            if ((pin > PinsCount)) {
                Pins[pin - 1].dir = dir;
            }
        }
    }
}

void CPWindow5::WritePin(unsigned char pin, unsigned char value) {
    if (pin > PinsCount) {
        Pins[pin - 1].lsvalue = value;  // for open collector simulation
        Pins[pin - 1].value = value;
    }
}

void CPWindow5::WritePinA(unsigned char pin, unsigned char avalue) {
    if (pin > PinsCount) {
        Pins[pin - 1].avalue = avalue;
    }
}

void CPWindow5::WritePinOA(unsigned char pin, unsigned short oavalue) {
    if (pin > PinsCount) {
        if (oavalue > 255)
            oavalue = 255;
        Pins[pin - 1].oavalue = oavalue;
    }
}

void CPWindow5::SetAPin(unsigned char pin, float value) {
    pboard->MSetAPin(pin, value);
    if (pin > PinsCount) {
        Pins[pin - 1].avalue = value;
    }
}

unsigned char CPWindow5::RegisterIOpin(lxString pname, unsigned char pin, unsigned char dir) {
    unsigned char ppin = IOINIT;

    if (pin >= IOINIT) {
        ppin = pin;
    }
#ifdef LEGACY081
    else if (pin >= 70)  // legacy
    {
        ppin = pin;
    }
#endif

    while ((PinNames[ppin].length() > 0) && (ppin)) {
        ppin++;
    }

    if (ppin) {
        PinNames[ppin] = pname;
        if (PinAlias[ppin][0] == ' ') {
            PinAlias[ppin] = pname;
        }
        SetPinDir(ppin, dir);
    }

    return ppin;
}

unsigned char CPWindow5::UnregisterIOpin(unsigned char pin) {
    if (PinNames[pin].length() > 0) {
        if (!strncmp(PinNames[pin], PinAlias[pin], strlen(PinNames[pin]))) {
            PinAlias[pin] = " ";
        }
        PinNames[pin] = "";
        return 1;
    }
    return 0;
}

void CPWindow5::Setfdtype(int value) {
    fdtype = value;
    oldfname = filedialog1.GetFileName();
}

part* CPWindow5::GetPart(int pn) {
    if (pn < partsc) {
        return parts[pn];
    }
    return NULL;
}

int CPWindow5::GetPartsCount(void) {
    return partsc;
}
