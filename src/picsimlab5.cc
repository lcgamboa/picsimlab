/* ########################################################################

   PICSimLab - Programmable IC Simulator Laboratory

   ########################################################################

   Copyright (c) : 2010-2023  Luis Claudio Gambôa Lopes <lcgamboa@yahoo.com>

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

#include "lib/oscilloscope.h"
#include "lib/picsimlab.h"
#include "lib/spareparts.h"

#include "picsimlab1.h"
#include "picsimlab2.h"
#include "picsimlab4.h"
#include "picsimlab5.h"
#include "picsimlab5_d.cc"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

CPWindow5 Window5;

// Implementation

static void Setfdtype(int value) {}

void CPWindow5::_EvOnShow(CControl* control) {
    if (Visible) {
        need_resize = 0;
        timer1.SetRunState(1);
        SpareParts.UpdateAll(1);
    }
}

void CPWindow5::menu1_EvMenuActive(CControl* control) {
    PartToCreate = ((CItemMenu*)control)->GetText();
    lxSetCursor(lxCursor(lxCURSOR_CROSS));
}

void CPWindow5::_EvOnCreate(CControl* control) {
    if (SpareParts.GetLoadConfigFile().length() > 0)
        SpareParts.LoadConfig(SpareParts.GetLoadConfigFile());

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

    SetTitle(((PICSimLab.GetInstanceNumber() > 0)
                  ? (lxT("PICSimLab[") + itoa(PICSimLab.GetInstanceNumber()) + lxT("] - "))
                  : (lxT("PICSimLab - "))) +
             "Spare parts");
}

void CPWindow5::draw1_EvMouseButtonPress(CControl* control, uint button, uint x, uint y, uint state) {
    x = x / SpareParts.GetScale();
    y = y / SpareParts.GetScale();

    for (int i = 0; i < SpareParts.GetCount(); i++) {
        if (SpareParts.GetPart(i)->PointInside((int)(x - offsetx), (int)(y - offsety))) {
            SpareParts.GetPart(i)->EvMouseButtonPress(button, (x - offsetx) - SpareParts.GetPart(i)->GetX(),
                                                      (y - offsety) - SpareParts.GetPart(i)->GetY(), state);
            if (button == 3) {
                PartSelected = i;
                pmenu2.SetX(x * SpareParts.GetScale());
                pmenu2.SetY(y * SpareParts.GetScale());
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
    if ((button == 1) && (PartToCreate.size() > 0) && (SpareParts.GetCount() < MAX_PARTS)) {
        // timer1.SetRunState (0);
        lxSetCursor(lxCursor(lxCURSOR_ARROW));
        SpareParts.AddPart((char*)PartToCreate.char_str(), x - offsetx, y - offsety, SpareParts.GetScale(),
                           PICSimLab.GetBoard());
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

void CPWindow5::draw1_EvMouseButtonRelease(CControl* control, uint button, uint x, uint y, uint state) {
    x = x / SpareParts.GetScale();
    y = y / SpareParts.GetScale();

    PartToMove = -1;

    mouse_scroll = 0;

    lxSetCursor(lxCursor(lxCURSOR_ARROW));
    mdx = 0;
    mdy = 0;

    for (int i = 0; i < SpareParts.GetCount(); i++) {
        if (SpareParts.GetPart(i)->PointInside(x - offsetx, y - offsety)) {
            SpareParts.GetPart(i)->EvMouseButtonRelease(button, (x - offsetx) - SpareParts.GetPart(i)->GetX(),
                                                        (y - offsety) - SpareParts.GetPart(i)->GetY(), state);
            return;
        }
    }
}

void CPWindow5::pmenu2_Properties_EvMenuActive(CControl* control) {
    int itemc = SpareParts.GetPart(PartSelected)->GetPCWCount();
    if (itemc) {
        const PCWProp* items = SpareParts.GetPart(PartSelected)->GetPCWProperties();
        wprop.SetName("window1");  // must be the same as in xml
        Application->ACreateWindow(&wprop);
        wprop.DestroyChilds();
        wprop.SetTitle(SpareParts.GetPart(PartSelected)->GetName());
        wprop.SetWidth(370);

        CLabel* label;
        CCombo* combo;
        CSpind* spind;
        CSpin* spin;
        CText* text;
        CEdit* edit;

        char name[20];
        int y = 32;
        int x = 54;
        int dual = (itemc > 15) ? itemc / 2 : itemc * 2;
        for (int i = 0; i < itemc; i++) {
            switch (items[i].pcw_type) {
                case PCW_COMBO:
                    sprintf(name, "label%i", i + 1);
                    label = new CLabel;
                    label->SetName(name);
                    label->SetFOwner(&wprop);
                    label->SetText(items[i].label);
                    label->SetWidth(60);
                    label->SetHeight(20);
                    label->SetX(x);
                    label->SetY(y + 3);
                    wprop.CreateChild(label);

                    sprintf(name, "combo%i", i + 1);
                    combo = new CCombo;
                    combo->SetName(name);
                    combo->SetFOwner(&wprop);
                    combo->SetWidth(200);
                    combo->SetHeight(26);
                    combo->SetX(x + 68);
                    combo->SetY(y);
                    wprop.CreateChild(combo);
                    break;
                case PCW_LABEL: {
                    char lb[21];
                    strncpy(lb, items[i].label, 20);
                    lb[19] = 0;
                    char* lb1 = strtok(lb, ",\n");
                    char* lb2 = strtok(NULL, ",\n");

                    sprintf(name, "label%i", i + 1);
                    label = new CLabel;
                    label->SetName(name);
                    label->SetFOwner(&wprop);
                    label->SetText(lb1);
                    label->SetWidth(60);
                    label->SetHeight(20);
                    label->SetX(x);
                    label->SetY(y + 3);
                    wprop.CreateChild(label);

                    sprintf(name, "label_%i", i + 1);
                    label = new CLabel;
                    label->SetName(name);
                    label->SetFOwner(&wprop);
                    label->SetText(lb2);
                    label->SetWidth(200);
                    label->SetHeight(20);
                    label->SetX(x + 76);
                    label->SetY(y + 3);
                    wprop.CreateChild(label);
                } break;
                case PCW_SPIND:
                    sprintf(name, "label%i", i + 1);
                    label = new CLabel;
                    label->SetName(name);
                    label->SetFOwner(&wprop);
                    label->SetText(items[i].label);
                    label->SetWidth(60);
                    label->SetHeight(20);
                    label->SetX(x);
                    label->SetY(y + 3);
                    wprop.CreateChild(label);

                    sprintf(name, "spind%i", i + 1);
                    spind = new CSpind;
                    spind->SetName(name);
                    spind->SetFOwner(&wprop);
                    spind->SetWidth(200);
                    spind->SetHeight(30);
                    spind->SetX(x + 68);
                    spind->SetY(y);
                    wprop.CreateChild(spind);
                    break;
                case PCW_SPIN:
                    sprintf(name, "label%i", i + 1);
                    label = new CLabel;
                    label->SetName(name);
                    label->SetFOwner(&wprop);
                    label->SetText(items[i].label);
                    label->SetWidth(60);
                    label->SetHeight(20);
                    label->SetX(x);
                    label->SetY(y + 3);
                    wprop.CreateChild(label);

                    sprintf(name, "spin%i", i + 1);
                    spin = new CSpin;
                    spin->SetName(name);
                    spin->SetFOwner(&wprop);
                    spin->SetWidth(200);
                    spin->SetHeight(30);
                    spin->SetX(x + 68);
                    spin->SetY(y);
                    wprop.CreateChild(spin);
                    break;
                case PCW_EDIT:
                    sprintf(name, "label%i", i + 1);
                    label = new CLabel;
                    label->SetName(name);
                    label->SetFOwner(&wprop);
                    label->SetText(items[i].label);
                    label->SetWidth(60);
                    label->SetHeight(20);
                    label->SetX(x);
                    label->SetY(y + 3);
                    wprop.CreateChild(label);

                    sprintf(name, "edit%i", i + 1);
                    edit = new CEdit;
                    edit->SetName(name);
                    edit->SetFOwner(&wprop);
                    edit->SetWidth(200);
                    edit->SetHeight(26);
                    edit->SetX(x + 68);
                    edit->SetY(y);
                    wprop.CreateChild(edit);
                    break;
                case PCW_TEXT:
                    sprintf(name, "text%i", i + 1);
                    text = new CText;
                    text->SetName(name);
                    text->SetFOwner(&wprop);
                    text->SetWidth(340);
                    text->SetHeight(128);
                    text->SetX(x - 44);
                    text->SetY(y);
                    wprop.CreateChild(text);
                    y += 110;
                    break;
                case PCW_DCOMBO:
                    sprintf(name, "label%i", i + 1);
                    label = new CLabel;
                    label->SetName(name);
                    label->SetFOwner(&wprop);
                    label->SetText(items[i].label);
                    label->SetWidth(60);
                    label->SetHeight(20);
                    label->SetX(x);
                    label->SetY(y + 3);
                    wprop.CreateChild(label);

                    sprintf(name, "combo%i", i + 1);
                    combo = new CCombo;
                    combo->SetName(name);
                    combo->SetFOwner(&wprop);
                    combo->SetWidth(200);
                    combo->SetHeight(26);
                    combo->SetX(x + 68);
                    combo->SetY(y);
                    wprop.CreateChild(combo);

                    sprintf(name, "combo_%i", i + 1);
                    combo = new CCombo;
                    combo->SetName(name);
                    combo->SetFOwner(&wprop);
                    combo->SetWidth(150);
                    combo->SetHeight(26);
                    combo->SetX(x + 278);
                    combo->SetY(y);
                    wprop.CreateChild(combo);

                    wprop.SetWidth(540);
                    break;
                default:
                    printf("PICSimLab: Unknown PCW type number %i\n", items[i].pcw_type);
                    exit(-1);
                    break;
            }
            y += 32;

            if (i + 1 >= dual) {
                dual = 2 * itemc;
                y = 32;
                x = 54 + 370;
                wprop.SetWidth(370 * 2);
            }
        }

        CButton* button;
        button = new CButton;
        button->SetName("button1");
        button->SetFOwner(&wprop);
        button->SetText("Ok");
        button->SetWidth(65);
        button->SetHeight(28);
        button->SetX((wprop.GetWidth() / 2) - 75);
        button->SetY(y + 32);
        button->SetTag(1);
        button->EvMouseButtonRelease = SpareParts.PropButtonRelease;
        wprop.CreateChild(button);

        button = new CButton;
        button->SetName("button2");
        button->SetFOwner(&wprop);
        button->SetText("Cancel");
        button->SetWidth(65);
        button->SetHeight(28);
        button->SetX((wprop.GetWidth() / 2) + 10);
        button->SetY(y + 32);
        button->EvMouseButtonRelease = SpareParts.PropButtonRelease;
        wprop.CreateChild(button);

        wprop.SetHeight(y + 130);

        SpareParts.GetPart(PartSelected)->ConfigurePropertiesWindow(&wprop);

        wprop.SetX(SpareParts.GetPart(PartSelected)->GetX() + GetX() - offsetx);
        wprop.SetY(SpareParts.GetPart(PartSelected)->GetY() + GetY() - offsety);

        wprop.Draw();
        wprop.ShowExclusive();
    } else {
        lxString fname =
            PICSimLab.GetSharePath() + lxT("parts/") + SpareParts.GetPart(PartSelected)->GetPropertiesWindowFile();

        if (lxFileExists(fname)) {
            CButton* button;
            wprop.SetName("window1");  // must be the same as in xml
            Application->ACreateWindow(&wprop);
            wprop.DestroyChilds();
            if (wprop.LoadXMLContextAndCreateChilds(fname)) {
                // wprop.SetCanDestroy (false);

                SpareParts.GetPart(PartSelected)->ConfigurePropertiesWindow(&wprop);

                button = (CButton*)wprop.GetChildByName("button1");
                if (button) {
                    button->EvMouseButtonRelease = SpareParts.PropButtonRelease;
                    button->SetTag(1);
                }

                button = (CButton*)wprop.GetChildByName("button2");
                if (button) {
                    button->EvMouseButtonRelease = SpareParts.PropButtonRelease;
                }

                wprop.SetX(SpareParts.GetPart(PartSelected)->GetX() + GetX() - offsetx);
                wprop.SetY(SpareParts.GetPart(PartSelected)->GetY() + GetY() - offsety);

                wprop.Draw();
                wprop.ShowExclusive();
            }
        } else {
            PICSimLab.RegisterError(SpareParts.GetPart(PartSelected)->GetName() + ": File not found! \n" + fname);
            printf("PICSimLab: (%s) File not found! %s\n", (const char*)SpareParts.GetPart(PartSelected)->GetName(),
                   (const char*)fname.c_str());
        }
    }
}

void CPWindow5::PropClose(int tag) {
    if (tag) {
        SpareParts.GetPart(PartSelected)->ReadPropertiesWindow(&wprop);
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
            Setfdtype(control->GetTag() - 2);
            Window5.filedialog1.Run();
            break;
    }
}

void CPWindow5::PropComboChange(CCombo* control) {
    Window5.wprop.HideExclusive();
    Window5.wprop.WDestroy();

    SpareParts.GetPart(Window5.PartSelected)->ComboChange(&Window5.wprop, control, control->GetText());

    Window5.pmenu2_Properties_EvMenuActive(this);
}

void CPWindow5::PropSpinChange(CSpin* control) {
    SpareParts.GetPart(Window5.PartSelected)->SpinChange(&Window5.wprop, control, control->GetValue());
}

void CPWindow5::PartButtonEvent(CControl* control, uint button, uint x, uint y, uint state) {
    if (control->GetTag() < (unsigned int)SpareParts.GetCount()) {
        SpareParts.GetPart(control->GetTag())->ButtonEvent(control, button, x, y, state);
    }
}

void CPWindow5::PartKeyEvent(CControl* control, uint keysym, uint ukeysym, uint state) {
    if (control->GetTag() < (unsigned int)SpareParts.GetCount()) {
        SpareParts.GetPart(control->GetTag())->KeyEvent(control, keysym, ukeysym, state);
    }
}

void CPWindow5::PartEvent(CControl* control) {
    if (control->GetTag() < (unsigned int)SpareParts.GetCount()) {
        SpareParts.GetPart(control->GetTag())->Event(control);
    }
}

void CPWindow5::timer1_EvOnTime(CControl* control) {
    static int tc = 0;
    int update = 0;

    if (need_resize == 1) {
        int w = GetClientWidth() - 10;
        int h = GetClientHeight() - 10;
        draw1.SetWidth((w > 0) ? w : 1);
        draw1.SetHeight((h > 0) ? h : 1);
        Oscilloscope.SetBaseTimer();
        update_all = 1;
    }

    need_resize++;

    for (int i = 0; i < SpareParts.GetCount(); i++) {
        SpareParts.GetPart(i)->Draw();
        if (SpareParts.GetPart(i)->GetUpdate())
            update++;
    }

    if (update_all) {
        SpareParts.UpdateAll();
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

        for (int i = 0; i < SpareParts.GetCount(); i++) {
            if (SpareParts.GetPart(i)->GetUpdate()) {
                draw1.Canvas.PutBitmap(SpareParts.GetPart(i)->GetBitmap(),
                                       (SpareParts.GetPart(i)->GetX() + offsetx) * SpareParts.GetScale(),
                                       (SpareParts.GetPart(i)->GetY() + offsety) * SpareParts.GetScale());
            }
        }

        draw1.Canvas.End();
        draw1.Update();
        statusbar1.Draw();
    }
#ifndef _WIN_
    Draw();
#endif
    tc++;

    if (tc > 3) {
        tc = 0;
        lxString field;
        field.Printf("Use Alias: %s", (SpareParts.GetUseAlias() == 1) ? "On" : "Off");
        statusbar1.SetField(0, field);

        field.Printf("Scale: %3.1f", SpareParts.GetScale());
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
    x = x / SpareParts.GetScale();
    y = y / SpareParts.GetScale();

    if (mouse_scroll) {
        offsetx -= mdx - x;
        offsety -= mdy - y;
        mdx = x;
        mdy = y;
        update_all = 1;
    } else if (PartToMove >= 0) {
        if ((mdx == 0) && (mdy == 0)) {
            mdx = SpareParts.GetPart(PartToMove)->GetX() - x;
            mdy = SpareParts.GetPart(PartToMove)->GetY() - y;
        }

        SpareParts.GetPart(PartToMove)->SetX(x + mdx);
        SpareParts.GetPart(PartToMove)->SetY(y + mdy);
        update_all = 1;
    } else {
        for (int i = 0; i < SpareParts.GetCount(); i++) {
            if (SpareParts.GetPart(i)->PointInside(x - offsetx, y - offsety)) {
                SpareParts.GetPart(i)->EvMouseMove(button, (x - offsetx) - SpareParts.GetPart(i)->GetX(),
                                                   (y - offsety) - SpareParts.GetPart(i)->GetY(), state);
                return;
            }
        }
    }
}

void CPWindow5::draw1_EvKeyboardPress(CControl* control, const uint key, const uint hkey, const uint mask) {
    switch (key) {
        case 'P':
        case 'p':
            SpareParts.SetUseAlias(!SpareParts.GetUseAlias());
            update_all = 1;
            Oscilloscope.SetBaseTimer();
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
            for (int i = 0; i < SpareParts.GetCount(); i++) {
                SpareParts.GetPart(i)->EvKeyPress(key, mask);
            }
            break;
    }
}

void CPWindow5::draw1_EvKeyboardRelease(CControl* control, const uint key, const uint hkey, const uint mask) {
    for (int i = 0; i < SpareParts.GetCount(); i++) {
        SpareParts.GetPart(i)->EvKeyRelease(key, mask);
    }
}

void CPWindow5::DeleteParts(void) {
    SpareParts.SetScale(1.0);
    if (Window4.GetVisible()) {
        Oscilloscope.SetBaseTimer();
    }
    // delete previous parts
    SpareParts.DeleteParts();
}

void CPWindow5::menu1_File_Newconfiguration_EvMenuActive(CControl* control) {
    if (SpareParts.GetCount() > 0) {
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
    SpareParts.Setfdtype(-1);
#ifdef __EMSCRIPTEN__
    filedialog1.SetDir("/tmp/");
    filedialog1.SetFileName("untitled.pcf");
    filedialog1_EvOnClose(1);
#else
    SpareParts.Setfdtype(-1);
    filedialog1.SetFileName(SpareParts.GetOldFilename());
    filedialog1.SetFilter(lxT("PICSimLab Config. (*.pcf)|*.pcf"));
    filedialog1.Run();
#endif
}

void CPWindow5::menu1_File_Loadconfiguration_EvMenuActive(CControl* control) {
#ifdef __EMSCRIPTEN__
    EM_ASM_({ toggle_load_panel(); });
#else
    filedialog1.SetType(lxFD_OPEN | lxFD_CHANGE_DIR);
    SpareParts.Setfdtype(-1);
    filedialog1.SetFileName(SpareParts.GetOldFilename());
    filedialog1.SetFilter(lxT("PICSimLab Config. (*.pcf)|*.pcf"));
    filedialog1.Run();
#endif
}

void CPWindow5::menu1_File_Savepinalias_EvMenuActive(CControl* control) {
    filedialog1.SetType(lxFD_SAVE | lxFD_CHANGE_DIR);
    SpareParts.Setfdtype(-1);
#ifdef __EMSCRIPTEN__
    filedialog1.SetDir("/tmp/");
    filedialog1.SetFileName("untitled.ppa");
    filedialog1_EvOnClose(1);
#else
    SpareParts.Setfdtype(-2);
    filedialog1.SetFileName(SpareParts.GetOldFilename());
    filedialog1.SetFilter(lxT("PICSimLab Pin Alias. (*.ppa)|*.ppa"));
    filedialog1.Run();
#endif
}

void CPWindow5::menu1_File_Loadpinalias_EvMenuActive(CControl* control) {
#ifdef __EMSCRIPTEN__
    EM_ASM_({ toggle_load_panel(); });
#else
    filedialog1.SetType(lxFD_OPEN | lxFD_CHANGE_DIR);
    SpareParts.Setfdtype(-2);
    filedialog1.SetFileName(SpareParts.GetOldFilename());
    filedialog1.SetFilter(lxT("PICSimLab Pin Alias. (*.ppa)|*.ppa"));
    filedialog1.Run();
#endif
}

void CPWindow5::menu1_Edit_Clearpinalias_EvMenuActive(CControl* control) {
    SpareParts.ClearPinAlias();
    update_all = 1;
}

void CPWindow5::menu1_Edit_Togglepinalias_EvMenuActive(CControl* control) {
    SpareParts.SetUseAlias(!SpareParts.GetUseAlias());
    update_all = 1;
    Oscilloscope.SetBaseTimer();
}

void CPWindow5::menu1_Edit_Editpinalias_EvMenuActive(CControl* control) {
    lxString alias_fname = SpareParts.GetAliasFname();
    if (lxFileExists(alias_fname)) {
        SpareParts.SavePinAlias(alias_fname);
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
    SpareParts.LoadPinAlias(SpareParts.GetAliasFname());
    update_all = 1;
}

void CPWindow5::menu1_Edit_Zoomin_EvMenuActive(CControl* control) {
    SpareParts.SetScale(SpareParts.GetScale() + 0.1);
    if (SpareParts.GetScale() > 2)
        SpareParts.SetScale(2);

    SpareParts.SetScale(trunc(SpareParts.GetScale() * 10) / 10.0);

    for (int i = 0; i < SpareParts.GetCount(); i++) {
        SpareParts.GetPart(i)->SetScale(SpareParts.GetScale());
    }
    update_all = 1;
}

void CPWindow5::menu1_Edit_Zoomout_EvMenuActive(CControl* control) {
    SpareParts.SetScale(SpareParts.GetScale() - 0.1);
    if (SpareParts.GetScale() < 0.1)
        SpareParts.SetScale(0.1);

    SpareParts.SetScale(trunc(SpareParts.GetScale() * 10) / 10.0);

    for (int i = 0; i < SpareParts.GetCount(); i++) {
        SpareParts.GetPart(i)->SetScale(SpareParts.GetScale());
    }
    update_all = 1;
}

void CPWindow5::_EvOnHide(CControl* control) {
    timer1.SetRunState(0);
    board* pboard = PICSimLab.GetBoard();
    if (pboard) {
        pboard->SetUseSpareParts(0);
    }
    if (Window4.GetVisible()) {
        Oscilloscope.SetBaseTimer();
    }
    for (int i = 0; i < SpareParts.GetCount(); i++) {
        SpareParts.GetPart(i)->Stop();
    }
}

void CPWindow5::pmenu2_Move_EvMenuActive(CControl* control) {
    PartToMove = PartSelected;
    lxSetCursor(lxCursor(lxCURSOR_SIZENWSE));
}

void CPWindow5::pmenu2_Rotate_EvMenuActive(CControl* control) {
    int orientation = SpareParts.GetPart(PartSelected)->GetOrientation();

    orientation++;
    if (orientation > 3)
        orientation = 0;

    SpareParts.GetPart(PartSelected)->SetOrientation(orientation);

    update_all = 1;
}

void CPWindow5::pmenu2_Delete_EvMenuActive(CControl* control) {
    SpareParts.DeletePart(PartSelected);
    update_all = 1;
}

void CPWindow5::pmenu2_Help_EvMenuActive(CControl* control) {
    lxString stemp;
    stemp.Printf(lxT("https://lcgamboa.github.io/picsimlab_docs/%s/%s"), lxT(_VERSION_),
                 (const char*)SpareParts.GetPart(PartSelected)->GetHelpURL().c_str());
    lxLaunchDefaultBrowser(stemp);
}

void CPWindow5::pmenu2_About_EvMenuActive(CControl* control) {
    Message_sz(lxT("Part ") + SpareParts.GetPart(PartSelected)->GetName() + lxT("\nDeveloped by ") +
                   SpareParts.GetPart(PartSelected)->GetAboutInfo(),
               400, 200);
}

void CPWindow5::menu1_Help_Contents_EvMenuActive(CControl* control) {
#ifdef EXT_BROWSER
    // lxLaunchDefaultBrowser(lxT("file://")+share + lxT ("docs/picsimlab.html"));
    lxString stemp;
    stemp.Printf(lxT("https://lcgamboa.github.io/picsimlab_docs/%s/SpareParts.html"), lxT(_VERSION_));
    lxLaunchDefaultBrowser(stemp);
#else
    Window2.html1.SetLoadFile(PICSimLab.GetSharePath() + lxT("docs/picsimlab.html"));
    Window2.Show();
#endif
}

void CPWindow5::menu1_Help_About_EvMenuActive(CControl* control) {
    lxString stemp;
    stemp.Printf(lxT("Developed by L.C. Gamboa\n <lcgamboa@yahoo.com>\n Version: %s %s %s %s"), lxT(_VERSION_),
                 lxT(_DATE_), lxT(_ARCH_), lxT(_PKG_));
    Message_sz(stemp, 400, 200);
}

void CPWindow5::filedialog1_EvOnClose(int retId) {
    if (retId && (SpareParts.Getfdtype() == -1)) {
        if ((filedialog1.GetType() == (lxFD_SAVE | lxFD_CHANGE_DIR))) {
            if (lxFileExists(filedialog1.GetFileName())) {
                if (!Dialog_sz(lxString("Overwriting file: ") + basename(filedialog1.GetFileName()) + "?", 400, 200))
                    return;
            }
            SpareParts.SaveConfig(filedialog1.GetFileName());
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
            SpareParts.LoadConfig(filedialog1.GetFileName());
        }
    } else if (retId && (SpareParts.Getfdtype() == -2)) {
        if ((filedialog1.GetType() == (lxFD_SAVE | lxFD_CHANGE_DIR))) {
            if (lxFileExists(filedialog1.GetFileName())) {
                if (!Dialog_sz(lxString("Overwriting file: ") + basename(filedialog1.GetFileName()) + "?", 400, 200))
                    return;
            }
            SpareParts.SavePinAlias(filedialog1.GetFileName());
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
            SpareParts.LoadPinAlias(filedialog1.GetFileName(), 1);
        }
    } else if (SpareParts.Getfdtype() >= 0) {
        SpareParts.GetPart(SpareParts.Getfdtype())->filedialog_EvOnClose(retId);
        SpareParts.Setfdtype(-1);
    }
}
