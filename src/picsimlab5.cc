/* ########################################################################

   PICSimLab - Programmable IC Simulator Laboratory

   ########################################################################

   Copyright (c) : 2010-2024  Luis Claudio Gambôa Lopes <lcgamboa@yahoo.com>

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

#include "lib/oscilloscope.h"
#include "lib/picsimlab.h"
#include "lib/spareparts.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

CPWindow5 Window5;

// Implementation

CPWindow5::~CPWindow5(void) {
    delete Windows[PW_WPROP];
}

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

    if (SpareParts.GetCount() < MAX_PARTS) {
        part* Part = SpareParts.AddPart((const char*)PartToCreate.c_str(), 50 - offsetx, 50 - offsety,
                                        SpareParts.GetScale(), PICSimLab.GetBoard());
        _EvOnShow(control);
        PartToCreate = "";
        PartToMove = Part->GetId();
        lxSetCursor(lxCursor(lxCURSOR_SIZENWSE));
        mdx = -10 - offsetx;
        mdy = -10 - offsety;
    }
}

void CPWindow5::_EvOnCreate(CControl* control) {
    SpareParts.OnCanvasCmd = &CPWindow5::OnCanvasCmd;
    SpareParts.OnWindowCmd = &CPWindow5::OnWindowCmd;

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
                  ? ("PICSimLab[" + std::to_string(PICSimLab.GetInstanceNumber()) + "] - ")
                  : ("PICSimLab - ")) +
             "Spare parts");

    msleep(BASETIMER);
    PICSimLab.GetBoard()->Reset();
}

void CPWindow5::draw1_EvMouseButtonPress(CControl* control, unsigned int button, unsigned int x, unsigned int y,
                                         unsigned int state) {
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

void CPWindow5::draw1_EvMouseButtonRelease(CControl* control, unsigned int button, unsigned int x, unsigned int y,
                                           unsigned int state) {
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
        Windows[PW_WPROP]->SetName("window1");  // must be the same as in xml
        Application->ACreateWindow(Windows[PW_WPROP]);
        Windows[PW_WPROP]->DestroyChilds();
        Windows[PW_WPROP]->SetTitle(SpareParts.GetPart(PartSelected)->GetName());
        Windows[PW_WPROP]->SetWidth(370);

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
                    label->SetFOwner(Windows[PW_WPROP]);
                    label->SetText(items[i].label);
                    label->SetWidth(60);
                    label->SetHeight(20);
                    label->SetX(x);
                    label->SetY(y + 3);
                    Windows[PW_WPROP]->CreateChild(label);

                    sprintf(name, "combo%i", i + 1);
                    combo = new CCombo;
                    combo->SetName(name);
                    combo->SetFOwner(Windows[PW_WPROP]);
                    combo->SetWidth(200);
                    combo->SetHeight(26);
                    combo->SetX(x + 68);
                    combo->SetY(y);
                    Windows[PW_WPROP]->CreateChild(combo);
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
                    label->SetFOwner(Windows[PW_WPROP]);
                    label->SetText(lb1);
                    label->SetWidth(60);
                    label->SetHeight(20);
                    label->SetX(x);
                    label->SetY(y + 3);
                    Windows[PW_WPROP]->CreateChild(label);

                    sprintf(name, "label_%i", i + 1);
                    label = new CLabel;
                    label->SetName(name);
                    label->SetFOwner(Windows[PW_WPROP]);
                    label->SetText(lb2);
                    label->SetWidth(200);
                    label->SetHeight(20);
                    label->SetX(x + 76);
                    label->SetY(y + 3);
                    Windows[PW_WPROP]->CreateChild(label);
                } break;
                case PCW_SPIND:
                    sprintf(name, "label%i", i + 1);
                    label = new CLabel;
                    label->SetName(name);
                    label->SetFOwner(Windows[PW_WPROP]);
                    label->SetText(items[i].label);
                    label->SetWidth(60);
                    label->SetHeight(20);
                    label->SetX(x);
                    label->SetY(y + 3);
                    Windows[PW_WPROP]->CreateChild(label);

                    sprintf(name, "spind%i", i + 1);
                    spind = new CSpind;
                    spind->SetName(name);
                    spind->SetFOwner(Windows[PW_WPROP]);
                    spind->SetWidth(200);
                    spind->SetHeight(30);
                    spind->SetX(x + 68);
                    spind->SetY(y);
                    Windows[PW_WPROP]->CreateChild(spind);
                    break;
                case PCW_SPIN:
                    sprintf(name, "label%i", i + 1);
                    label = new CLabel;
                    label->SetName(name);
                    label->SetFOwner(Windows[PW_WPROP]);
                    label->SetText(items[i].label);
                    label->SetWidth(60);
                    label->SetHeight(20);
                    label->SetX(x);
                    label->SetY(y + 3);
                    Windows[PW_WPROP]->CreateChild(label);

                    sprintf(name, "spin%i", i + 1);
                    spin = new CSpin;
                    spin->SetName(name);
                    spin->SetFOwner(Windows[PW_WPROP]);
                    spin->SetWidth(200);
                    spin->SetHeight(30);
                    spin->SetX(x + 68);
                    spin->SetY(y);
                    Windows[PW_WPROP]->CreateChild(spin);
                    break;
                case PCW_EDIT:
                    sprintf(name, "label%i", i + 1);
                    label = new CLabel;
                    label->SetName(name);
                    label->SetFOwner(Windows[PW_WPROP]);
                    label->SetText(items[i].label);
                    label->SetWidth(60);
                    label->SetHeight(20);
                    label->SetX(x);
                    label->SetY(y + 3);
                    Windows[PW_WPROP]->CreateChild(label);

                    sprintf(name, "edit%i", i + 1);
                    edit = new CEdit;
                    edit->SetName(name);
                    edit->SetFOwner(Windows[PW_WPROP]);
                    edit->SetWidth(200);
                    edit->SetHeight(26);
                    edit->SetX(x + 68);
                    edit->SetY(y);
                    Windows[PW_WPROP]->CreateChild(edit);
                    break;
                case PCW_TEXT:
                    sprintf(name, "text%i", i + 1);
                    text = new CText;
                    text->SetName(name);
                    text->SetFOwner(Windows[PW_WPROP]);
                    text->SetWidth(340);
                    text->SetHeight(128);
                    text->SetX(x - 44);
                    text->SetY(y);
                    Windows[PW_WPROP]->CreateChild(text);
                    y += 110;
                    break;
                case PCW_DCOMBO:
                    sprintf(name, "label%i", i + 1);
                    label = new CLabel;
                    label->SetName(name);
                    label->SetFOwner(Windows[PW_WPROP]);
                    label->SetText(items[i].label);
                    label->SetWidth(60);
                    label->SetHeight(20);
                    label->SetX(x);
                    label->SetY(y + 3);
                    Windows[PW_WPROP]->CreateChild(label);

                    sprintf(name, "combo%i", i + 1);
                    combo = new CCombo;
                    combo->SetName(name);
                    combo->SetFOwner(Windows[PW_WPROP]);
                    combo->SetWidth(200);
                    combo->SetHeight(26);
                    combo->SetX(x + 68);
                    combo->SetY(y);
                    Windows[PW_WPROP]->CreateChild(combo);

                    sprintf(name, "combo_%i", i + 1);
                    combo = new CCombo;
                    combo->SetName(name);
                    combo->SetFOwner(Windows[PW_WPROP]);
                    combo->SetWidth(150);
                    combo->SetHeight(26);
                    combo->SetX(x + 278);
                    combo->SetY(y);
                    Windows[PW_WPROP]->CreateChild(combo);

                    Windows[PW_WPROP]->SetWidth(540);
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
                Windows[PW_WPROP]->SetWidth(370 * 2);
            }
        }

        CButton* button;
        button = new CButton;
        button->SetName("button1");
        button->SetFOwner(Windows[PW_WPROP]);
        button->SetText("Ok");
        button->SetWidth(65);
        button->SetHeight(28);
        button->SetX((Windows[PW_WPROP]->GetWidth() / 2) - 75);
        button->SetY(y + 32);
        button->SetTag(1);
        button->EvMouseButtonRelease = EVMOUSEBUTTONRELEASE & CPWindow5::PropButtonRelease;
        Windows[PW_WPROP]->CreateChild(button);

        button = new CButton;
        button->SetName("button2");
        button->SetFOwner(Windows[PW_WPROP]);
        button->SetText("Cancel");
        button->SetWidth(65);
        button->SetHeight(28);
        button->SetX((Windows[PW_WPROP]->GetWidth() / 2) + 10);
        button->SetY(y + 32);
        button->EvMouseButtonRelease = EVMOUSEBUTTONRELEASE & CPWindow5::PropButtonRelease;
        Windows[PW_WPROP]->CreateChild(button);

        Windows[PW_WPROP]->SetHeight(y + 130);

        SpareParts.GetPart(PartSelected)->ConfigurePropertiesWindow();

        Windows[PW_WPROP]->SetX(SpareParts.GetPart(PartSelected)->GetX() + GetX() - offsetx);
        Windows[PW_WPROP]->SetY(SpareParts.GetPart(PartSelected)->GetY() + GetY() - offsety);

        Windows[PW_WPROP]->Draw();
        Windows[PW_WPROP]->ShowExclusive();
    } else {
        std::string fname =
            PICSimLab.GetSharePath() + "parts/" + SpareParts.GetPart(PartSelected)->GetPropertiesWindowFile();

        if (lxFileExists(fname)) {
            CButton* button;
            Windows[PW_WPROP]->SetName("window1");  // must be the same as in xml
            Application->ACreateWindow(Windows[PW_WPROP]);
            Windows[PW_WPROP]->DestroyChilds();
            if (Windows[PW_WPROP]->LoadXMLContextAndCreateChilds(fname)) {
                // Windows[WPROP]->SetCanDestroy (false);

                SpareParts.GetPart(PartSelected)->ConfigurePropertiesWindow();

                button = (CButton*)Windows[PW_WPROP]->GetChildByName("button1");
                if (button) {
                    button->EvMouseButtonRelease = EVMOUSEBUTTONRELEASE & CPWindow5::PropButtonRelease;
                    button->SetTag(1);
                }

                button = (CButton*)Windows[PW_WPROP]->GetChildByName("button2");
                if (button) {
                    button->EvMouseButtonRelease = EVMOUSEBUTTONRELEASE & CPWindow5::PropButtonRelease;
                }

                Windows[PW_WPROP]->SetX(SpareParts.GetPart(PartSelected)->GetX() + GetX() - offsetx);
                Windows[PW_WPROP]->SetY(SpareParts.GetPart(PartSelected)->GetY() + GetY() - offsety);

                Windows[PW_WPROP]->Draw();
                Windows[PW_WPROP]->ShowExclusive();
            }

        } else {
            PICSimLab.RegisterError(SpareParts.GetPart(PartSelected)->GetName() + ": File not found! \n" + fname);
            printf("PICSimLab: (%s) File not found! %s\n",
                   (const char*)SpareParts.GetPart(PartSelected)->GetName().c_str(), (const char*)fname.c_str());
        }
    }
}

void CPWindow5::PropClose(int tag) {
    if (tag) {
        SpareParts.GetPart(PartSelected)->ReadPropertiesWindow();
    }
    Windows[PW_WPROP]->HideExclusive();
    // Windows[WPROP]->SetCanDestroy (true);
    Windows[PW_WPROP]->WDestroy();
    update_all = 1;
}

void CPWindow5::PropButtonRelease(CControl* control, unsigned int button, unsigned int x, unsigned int y,
                                  unsigned int state) {
    switch (control->GetTag()) {
        case 0:
        case 1:
            Window5.PropClose(control->GetTag());
            break;
        default:  // browse filedialog
            Window5.filedialog1.SetType(lxFD_OPEN | lxFD_CHANGE_DIR);
            Window5.filedialog1.SetFilter("All Files (*.*)|*.*");
            Setfdtype(control->GetTag() - 2);
            Window5.filedialog1.Run();
            break;
    }
}

void CPWindow5::PropComboChange(CCombo* control) {
    Window5.Windows[PW_WPROP]->HideExclusive();
    Window5.Windows[PW_WPROP]->WDestroy();

    SpareParts.GetPart(Window5.PartSelected)
        ->ComboChange(control->GetName().c_str(), (const char*)control->GetText().c_str());

    Window5.pmenu2_Properties_EvMenuActive(this);
}

void CPWindow5::PropSpinChange(CSpin* control) {
    SpareParts.GetPart(Window5.PartSelected)->SpinChange(control->GetName().c_str(), control->GetValue());
}

void CPWindow5::PartButtonEvent(CControl* control, unsigned int button, unsigned int x, unsigned int y,
                                unsigned int state) {
    if (control->GetTag() < (unsigned int)SpareParts.GetCount()) {
        SpareParts.GetPart(control->GetTag())->ButtonEvent(control->GetName().c_str(), button, x, y, state);
    }
}

void CPWindow5::PartKeyEvent(CControl* control, unsigned int keysym, unsigned int ukeysym, unsigned int state) {
    if (control->GetTag() < (unsigned int)SpareParts.GetCount()) {
        SpareParts.GetPart(control->GetTag())->KeyEvent(control->GetName().c_str(), keysym, ukeysym, state);
    }
}

void CPWindow5::PartEvent(CControl* control) {
    if (control->GetTag() < (unsigned int)SpareParts.GetCount()) {
        SpareParts.GetPart(control->GetTag())->Event(control->GetName().c_str());
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
        SpareParts.SetPartOnDraw(SpareParts.GetPart(i)->GetId());
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
        draw1.Canvas.SetFontWeight(lxFONTWEIGHT_BOLD);

        if (update_all) {
            draw1.Canvas.SetFgColor(50, 50, 50);
            draw1.Canvas.SetBgColor(50, 50, 50);
            draw1.Canvas.Rectangle(1, 0, 0, draw1.GetWidth(), draw1.GetHeight());
            update_all = 0;
        }

        for (int i = 0; i < SpareParts.GetCount(); i++) {
            if (SpareParts.GetPart(i)->GetUpdate()) {
                draw1.Canvas.PutBitmap(Bitmaps[SpareParts.GetPart(i)->GetBitmap()],
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
        char field[100];
        snprintf(field, 100, "Use Alias: %s", (SpareParts.GetUseAlias() == 1) ? "On" : "Off");
        statusbar1.SetField(0, field);

        snprintf(field, 100, "Scale: %3.1f", SpareParts.GetScale());
        statusbar1.SetField(1, field);

        snprintf(field, 100, "Offset: %3i %3i", offsetx, offsety);
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

void CPWindow5::draw1_EvMouseMove(CControl* control, unsigned int button, unsigned int x, unsigned int y,
                                  unsigned int state) {
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

void CPWindow5::draw1_EvKeyboardPress(CControl* control, const unsigned int key, const unsigned int hkey,
                                      const unsigned int mask) {
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

void CPWindow5::draw1_EvKeyboardRelease(CControl* control, const unsigned int key, const unsigned int hkey,
                                        const unsigned int mask) {
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
    filedialog1.SetFilter("PICSimLab Config. (*.pcf)|*.pcf");
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
    filedialog1.SetFilter("PICSimLab Config. (*.pcf)|*.pcf");
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
    filedialog1.SetFilter("PICSimLab Pin Alias. (*.ppa)|*.ppa");
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
    filedialog1.SetFilter("PICSimLab Pin Alias. (*.ppa)|*.ppa");
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
    std::string alias_fname = SpareParts.GetAliasFname();
    if (lxFileExists(alias_fname)) {
        SpareParts.SavePinAlias(alias_fname);
#ifdef _WIN_
        lxExecute("notepad.exe " + alias_fname);
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
    PartToMove = -1;
    SpareParts.DeletePart(PartSelected);
    update_all = 1;
}

void CPWindow5::pmenu2_Help_EvMenuActive(CControl* control) {
    char stemp[256];
    snprintf(stemp, 256, "https://lcgamboa.github.io/picsimlab_docs/%s/%s", _VERSION_,
             (const char*)SpareParts.GetPart(PartSelected)->GetHelpURL().c_str());
    lxLaunchDefaultBrowser(stemp);
}

void CPWindow5::pmenu2_About_EvMenuActive(CControl* control) {
    Message_sz("Part " + SpareParts.GetPart(PartSelected)->GetName() + "\nDeveloped by " +
                   SpareParts.GetPart(PartSelected)->GetAboutInfo(),
               400, 200);
}

void CPWindow5::menu1_Help_Contents_EvMenuActive(CControl* control) {
#ifdef EXT_BROWSER
    // lxLaunchDefaultBrowser("file://"+share + "docs/picsimlab.html");
    char stemp[256];
    snprintf(stemp, 256, "https://lcgamboa.github.io/picsimlab_docs/%s/SpareParts.html", _VERSION_);
    lxLaunchDefaultBrowser(stemp);
#else
    Window2.html1.SetLoadFile(PICSimLab.GetSharePath() + "docs/picsimlab.html");
    Window2.Show();
#endif
}

void CPWindow5::menu1_Help_About_EvMenuActive(CControl* control) {
    char stemp[256];
    snprintf(stemp, 256, "Developed by L.C. Gamboa\n <lcgamboa@yahoo.com>\n Version: %s %s %s %s", _VERSION_, _DATE_,
             _ARCH_, _PKG_);
    Message_sz(stemp, 400, 200);
}

void CPWindow5::filedialog1_EvOnClose(int retId) {
    if (retId && (SpareParts.Getfdtype() == -1)) {
        if ((filedialog1.GetType() == (lxFD_SAVE | lxFD_CHANGE_DIR))) {
            if (lxFileExists(filedialog1.GetFileName())) {
                if (!Dialog_sz(std::string("Overwriting file: ") +
                                   ((const char*)basename(filedialog1.GetFileName()).c_str()) + "?",
                               400, 200))
                    return;
            }
            SpareParts.SaveConfig((const char*)filedialog1.GetFileName().c_str());
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
            SpareParts.LoadConfig((const char*)filedialog1.GetFileName().c_str());
        }
    } else if (retId && (SpareParts.Getfdtype() == -2)) {
        if ((filedialog1.GetType() == (lxFD_SAVE | lxFD_CHANGE_DIR))) {
            if (lxFileExists(filedialog1.GetFileName())) {
                if (!Dialog_sz(std::string("Overwriting file: ") +
                                   ((const char*)basename(filedialog1.GetFileName()).c_str()) + "?",
                               400, 200))
                    return;
            }
            SpareParts.SavePinAlias((const char*)filedialog1.GetFileName().c_str());
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
            SpareParts.LoadPinAlias((const char*)filedialog1.GetFileName().c_str(), 1);
        }
    } else if (SpareParts.Getfdtype() >= 0) {
        SpareParts.GetPart(SpareParts.Getfdtype())->filedialog_EvOnClose(retId);
        SpareParts.Setfdtype(-1);
    }
}

int CPWindow5::OnCanvasCmd(const CanvasCmd_t cmd) {
    int partn = SpareParts.GetPartOnDraw();
    switch (cmd.cmd) {
        case CC_INIT:
            Window5.Canvas[partn].Init(cmd.Init.sx, cmd.Init.sy, cmd.Init.angle);
            return 0;
            break;
        case CC_CHANGESCALE:
            Window5.Canvas[partn].ChangeScale(cmd.ChangeScale.sx, cmd.ChangeScale.sy);
            return 0;
            break;
        case CC_END:
            Window5.Canvas[partn].End();
            return 0;
            break;
        case CC_SETBITMAP:
            Window5.Canvas[partn].SetBitmap(Window5.Bitmaps[cmd.SetBitmap.BitmapId], cmd.SetBitmap.xs,
                                            cmd.SetBitmap.ys);
            return 0;
            break;
        case CC_SETCOLOR:
            Window5.Canvas[partn].SetColor(cmd.SetColor.r, cmd.SetColor.g, cmd.SetColor.b);
            return 0;
            break;
        case CC_SETFGCOLOR:
            Window5.Canvas[partn].SetFgColor(cmd.SetFgColor.r, cmd.SetFgColor.g, cmd.SetFgColor.b);
            return 0;
            break;
        case CC_SETBGCOLOR:
            Window5.Canvas[partn].SetBgColor(cmd.SetBgColor.r, cmd.SetBgColor.g, cmd.SetBgColor.b);
            return 0;
            break;
        case CC_SETFONTSIZE:
            Window5.Canvas[partn].SetFontSize(cmd.SetFontSize.pointsize);
            return 0;
            break;
        case CC_SETFONTWEIGHT:
            Window5.Canvas[partn].SetFontWeight(cmd.SetFontWeight.weight);
            return 0;
            break;
        case CC_SETLINEWIDTH:
            Window5.Canvas[partn].SetLineWidth(cmd.SetLineWidth.lwidth);
            return 0;
            break;
        case CC_POINT:
            Window5.Canvas[partn].Point(cmd.Point.x, cmd.Point.y);
            return 0;
            break;
        case CC_LINE:
            Window5.Canvas[partn].Line(cmd.Line.x1, cmd.Line.y1, cmd.Line.x2, cmd.Line.y2);
            return 0;
            break;
        case CC_RECTANGLE:
            Window5.Canvas[partn].Rectangle(cmd.Rectangle.filled, cmd.Rectangle.x, cmd.Rectangle.y, cmd.Rectangle.width,
                                            cmd.Rectangle.height);
            return 0;
            break;
        case CC_CIRCLE:
            Window5.Canvas[partn].Circle(cmd.Circle.filled, cmd.Circle.x, cmd.Circle.y, cmd.Circle.radius);
            return 0;
            break;
        case CC_ROTATEDTEXT:
            Window5.Canvas[partn].RotatedText(cmd.RotatedText.str, cmd.RotatedText.x, cmd.RotatedText.y,
                                              cmd.RotatedText.angle);
            return 0;
            break;
        case CC_TEXTONRECT: {
            lxRect rect;
            rect.x = cmd.TextOnRect.rect.x;
            rect.y = cmd.TextOnRect.rect.y;
            rect.width = cmd.TextOnRect.rect.width;
            rect.height = cmd.TextOnRect.rect.height;
            Window5.Canvas[partn].TextOnRect(cmd.TextOnRect.str, rect, cmd.TextOnRect.align);
            return 0;
        } break;
        case CC_POLYGON:
            Window5.Canvas[partn].Polygon(cmd.Polygon.filled, (lxPoint*)cmd.Polygon.points, cmd.Polygon.npoints);
            return 0;
            break;
        case CC_PUTBITMAP:
            Window5.Canvas[partn].PutBitmap(Window5.Bitmaps[cmd.PutBitmap.BitmapId], cmd.PutBitmap.x, cmd.PutBitmap.y);
            return 0;
            break;
        case CC_GETBGCOLOR: {
            lxColor bgc = Window5.Canvas[partn].GetBgColor();
            *cmd.GetBgColor.r = bgc.Red();
            *cmd.GetBgColor.g = bgc.Green();
            *cmd.GetBgColor.b = bgc.Blue();
            return 0;
        } break;
        case CC_CREATE:
            Window5.Canvas[partn].Create(Window5.GetWWidget(), Window5.Bitmaps[cmd.Create.BitmapId]);
            return 0;
            break;
        case CC_DESTROY:
            Window5.Canvas[partn].Destroy();
            break;
        case CC_FREEBITMAP:
            if (Window5.Bitmaps[cmd.FreeBitmap.BitmapId]) {
                delete Window5.Bitmaps[cmd.FreeBitmap.BitmapId];
                Window5.Bitmaps[cmd.FreeBitmap.BitmapId] = NULL;
                return 0;
            }
            break;
        case CC_GETBITMAPSIZE: {
            lxSize ps = Window5.Bitmaps[cmd.GetBitmapSize.BitmapId]->GetSize();
            *cmd.GetBitmapSize.w = ps.GetWidth();
            *cmd.GetBitmapSize.h = ps.GetHeight();
            return 0;
        } break;
        case CC_LOADIMAGE: {
            lxImage image(&Window5);
            if (image.LoadFile(GetLocalFile(cmd.LoadImage.fname), cmd.LoadImage.orientation, cmd.LoadImage.scale,
                               cmd.LoadImage.scale, cmd.LoadImage.usealpha)) {
                // find enpty bitmap
                int bid = -1;
                for (int i = 0; i < (MAX_PARTS * 2); i++) {
                    if (Window5.Bitmaps[i] == NULL) {
                        bid = i;
                        break;
                    }
                }
                if ((bid >= 0) && (bid < (MAX_PARTS * 2))) {
                    Window5.Bitmaps[bid] = new lxBitmap(&image, &Window5);
                    image.Destroy();
                    return bid;
                }
            }
            return -1;
        } break;
        case CC_CREATEIMAGE: {
            lxImage image(&Window5);
            if (image.CreateBlank(cmd.CreateImage.width, cmd.CreateImage.height, cmd.CreateImage.orientation,
                                  cmd.CreateImage.scale, cmd.CreateImage.scale)) {
                // find enpty bitmap
                int bid = -1;
                for (int i = 0; i < (MAX_PARTS * 2); i++) {
                    if (Window5.Bitmaps[i] == NULL) {
                        bid = i;
                        break;
                    }
                }
                if ((bid >= 0) && (bid < (MAX_PARTS * 2))) {
                    Window5.Bitmaps[bid] = new lxBitmap(&image, &Window5);
                    image.Destroy();
                    return bid;
                }
            }
            return -1;
        } break;
        case CC_LAST:
        default:
            break;
    }
    return -1;
}

int CPWindow5::OnWindowCmd(const int id, const char* ControlName, const PICSimLabWindowAction action, const char* Value,
                           void* ReturnBuff) {
    CControl* ctrl = NULL;
    int wid = -1;

    if (id >= 0) {
        if (ControlName) {
            ctrl = Window5.Windows[id]->GetChildByName(ControlName);
        } else {
            ctrl = Window5.Windows[id];
        }

        if (ctrl == NULL) {
            return -1;
        }
    } else if (action == PWA_WINDOWCREATE) {  // find empty window
        for (int i = 0; i < MAX_PARTS; i++) {
            if (Window5.Windows[i] == NULL) {
                wid = i;
                break;
            }
        }

        if (wid == -1) {
            return -1;
        }
    } else {
        return -1;
    }

    switch (action) {
        case PWA_COMBOPROPEV:
            if (!strcmp(Value, "1")) {
                ((CCombo*)ctrl)->EvOnComboChange = EVONCOMBOCHANGE & CPWindow5::PropComboChange;
            } else {
                ((CCombo*)ctrl)->EvOnComboChange = NULL;
            }
            break;
        case PWA_COMBOPARTEV:
            if (!strcmp(Value, "1")) {
                ((CCombo*)ctrl)->EvOnComboChange = EVONCOMBOCHANGE & CPWindow5::PartEvent;
            } else {
                ((CCombo*)ctrl)->EvOnComboChange = NULL;
            }
            break;

        case PWA_SPINPROPEV:
            if (!strcmp(Value, "1")) {
                ((CSpin*)ctrl)->EvOnChangeSpin = EVONCHANGESPIN & CPWindow5::PropSpinChange;
            } else {
                ((CSpin*)ctrl)->EvOnChangeSpin = NULL;
            }
            break;

        case PWA_EDITPARTEV:
            if (!strcmp(Value, "1")) {
                ((CEdit*)ctrl)->EvKeyboardPress = EVKEYBOARDPRESS & CPWindow5::PartKeyEvent;
            } else {
                ((CEdit*)ctrl)->EvKeyboardPress = NULL;
            }
            break;

        case PWA_WINDOWCREATE: {
            Window5.Windows[wid] = new CPWindow();
            ctrl = Window5.Windows[wid];
            ctrl->SetName(ControlName);  // must be the same as in xml
            ctrl->SetVisible(0);
            ((CPWindow*)ctrl)->SetCanDestroy(false);
            Application->ACreateWindow(((CPWindow*)ctrl));
            return wid;
        } break;
        case PWA_WINDOWPARTEV:
            if (!strcmp(Value, "1")) {
                ((CPWindow*)ctrl)->EvOnShow = EVONCOMBOCHANGE & CPWindow5::PartEvent;
            } else {
                ((CPWindow*)ctrl)->EvOnShow = NULL;
            }
            break;

        case PWA_BUTTONPARTEV:
            if (!strcmp(Value, "1")) {
                ((CButton*)ctrl)->EvMouseButtonRelease = EVMOUSEBUTTONRELEASE & CPWindow5::PartButtonEvent;
            } else {
                ((CButton*)ctrl)->EvMouseButtonRelease = NULL;
            }
            break;

        default:
            return Window1.WinCmd(ctrl, action, Value, ReturnBuff);
            break;
    }

    return 0;
}