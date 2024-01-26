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

#include "spareparts.h"
#include "oscilloscope.h"
#include "picsimlab.h"

// Global objects;
CSpareParts SpareParts;

CSpareParts::CSpareParts() {
    pboard = NULL;
    partsc = 0;
    partsc_aup = 0;
    useAlias = 0;
    alias_fname = "";
    scale = 1.0;
    LoadConfigFile = "";
    fdtype = -1;

    PropButtonRelease = NULL;
    PropComboChange = NULL;
    PropSpinChange = NULL;
    PartEvent = NULL;
    PartKeyEvent = NULL;
    PartButtonEvent = NULL;

    OnLoadImage = NULL;
    OnCreateImage = NULL;
}

void CSpareParts::Init(CWindow* win) {
    Window = win;
    if (Window) {
        filedialog = (CFileDialog*)win->GetChildByName("filedialog1");
    }
}

void CSpareParts::UpdateAll(const int force) {
    for (int i = 0; i < partsc; i++) {
        parts[i]->SetUpdate(1);
#if defined(_LX_SDL2) || defined(__EMSCRIPTEN__)
        parts[i]->SetScale(parts[i]->GetScale() + 1e-3);
#endif
    }
}

part* CSpareParts::GetPart(const int partn) {
    if (partn < partsc) {
        return parts[partn];
    }
    return NULL;
}

part* CSpareParts::AddPart(const char* partname, const int x, const int y, const float scale, board* pboard_) {
    part* newpart = create_part(partname, x, y, pboard_);
    parts[partsc] = newpart;
    if (parts[partsc] == NULL) {
        Message_sz("Erro creating part: " + std::string(partname), 400, 200);
    } else {
        parts[partsc]->SetId(partsc);
        parts[partsc]->SetScale(scale);
        parts[partsc]->Reset();
        partsc++;
    }

    return newpart;
}

void CSpareParts::DeleteParts(void) {
    int partsc_ = partsc;
    partsc = 0;  // for disable process
    partsc_aup = 0;
    useAlias = 0;

    for (int i = 0; i < partsc_; i++) {
        delete parts[i];
    }
}

void CSpareParts::ClearPinAlias(void) {
    for (int i = 0; i < 256; i++) {
        PinAlias[i] = PinNames[i];
    }
}

void CSpareParts::ResetPullupBus(unsigned char pin) {
    if (pin < IOINIT) {
        pullup_bus[pin]++;  // count i2c devices in bus
    }
}

void CSpareParts::SetPullupBus(unsigned char pin, unsigned char value) {
    if (pin < IOINIT) {
        pullup_bus[pin] &= value;
    }
}

unsigned char CSpareParts::GetPullupBus(unsigned char pin) {
    if (pin < IOINIT)
        return pullup_bus[pin];
    else
        return 0;
}

std::string CSpareParts::GetPinsNames(void) {
    std::string Items = "0  NC,";
    std::string spin;

    for (int i = 1; i <= pboard->MGetPinCount(); i++) {
        if (useAlias) {
            spin = PinAlias[i];
        } else {
            spin = PinNames[i];
        }
        if (PinNames[i].compare("error")) {
            Items = Items + std::to_string(i) + "  " + spin + ",";
        }
    }
    for (int i = IOINIT; i < 256; i++) {
        if (useAlias) {
            spin = PinAlias[i];
        } else {
            spin = PinNames[i];
        }
        if (PinNames[i].length() > 0) {
            Items = Items + std::to_string(i) + "  " + spin + ",";
        }
    }
    return Items;
}

std::string CSpareParts::GetPinName(unsigned char pin) {
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
            return PinNames[pin] + "-" + std::to_string(pin);
        }
    }
}

const picpin* CSpareParts::GetPinsValues(void) {
    return Pins;
}

void CSpareParts::SetPin(unsigned char pin, unsigned char value) {
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

void CSpareParts::SetPinDOV(unsigned char pin, unsigned char ovalue) {
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

void CSpareParts::SetPinDir(unsigned char pin, unsigned char dir) {
    if (pin) {
        if (Pins[pin - 1].dir != dir) {
            if ((pin > PinsCount)) {
                Pins[pin - 1].dir = dir;
            }
        }
    }
}

void CSpareParts::WritePin(unsigned char pin, unsigned char value) {
    if (pin > PinsCount) {
        Pins[pin - 1].lsvalue = value;  // for open collector simulation
        Pins[pin - 1].value = value;
    }
}

void CSpareParts::WritePinA(unsigned char pin, unsigned char avalue) {
    if (pin > PinsCount) {
        Pins[pin - 1].avalue = avalue;
    }
}

void CSpareParts::WritePinOA(unsigned char pin, unsigned short oavalue) {
    if (pin > PinsCount) {
        if (oavalue > 255)
            oavalue = 255;
        Pins[pin - 1].oavalue = oavalue;
    }
}

void CSpareParts::SetAPin(unsigned char pin, float value) {
    pboard->MSetAPin(pin, value);
    if (pin > PinsCount) {
        Pins[pin - 1].avalue = value;
    }
}

unsigned char CSpareParts::RegisterIOpin(std::string pname, unsigned char pin, unsigned char dir) {
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
        if ((PinAlias[ppin].length() == 0) || (PinAlias[ppin][0] == ' ')) {
            PinAlias[ppin] = pname;
        }
        SetPinDir(ppin, dir);
    }

    return ppin;
}

unsigned char CSpareParts::UnregisterIOpin(unsigned char pin) {
    if (PinNames[pin].length() > 0) {
        if (!strncmp(PinNames[pin].c_str(), PinAlias[pin].c_str(), PinNames[pin].size())) {
            PinAlias[pin] = " ";
        }
        PinNames[pin] = "";
        return 1;
    }
    return 0;
}

bool CSpareParts::SavePinAlias(std::string fname) {
    char temp[256];
    std::string pin;
    std::string alias;
    std::vector<std::string> lalias;
    lalias.clear();
    lalias.push_back(
        "//N-PinName -ALias   --The pin name alias must start in column fourteen and have size less than seven chars ");
    for (int i = 1; i < 256; i++) {
        pin = PinNames[i].substr(0, 7);
        if (!pin.size())
            pin = " ";
        alias = PinAlias[i].substr(0, 7);
        if (!alias.size())
            alias = " ";
        snprintf(temp, 256, "%03i-%-7s -%-7s", i, pin.c_str(), alias.c_str());
        lalias.push_back(temp);
    }
    return SaveToFile(lalias, fname.c_str());
}

bool CSpareParts::LoadPinAlias(std::string fname, unsigned char show_error_msg) {
    if (!show_error_msg) {
        if (!lxFileExists(fname)) {
            return 0;
        }
    }
    std::vector<std::string> alias;
    std::string line;
    alias.clear();
    if (LoadFromFile(alias, fname.c_str())) {
        alias_fname = fname;

        for (int i = 0; i < 256; i++) {
            line = alias.at(i);
            if (line.size() > 13) {
                PinAlias[i] = line.substr(13, 7);
            } else {
                PinAlias[i] = "";
            }
        }
        PinAlias[0] = "NC";
        if (show_error_msg) {
            useAlias = 1;
            UpdateAll();
            Oscilloscope.SetBaseTimer();
        }
        return 1;
    }
    return 0;
}

bool CSpareParts::LoadConfig(std::string fname, const int disable_debug) {
    char name[256];
    char temp[4096];
    unsigned int x, y;
    int orient;
    std::vector<std::string> prefs;
    int newformat = 0;
    std::vector<std::string> osc_list;

    pboard = PICSimLab.GetBoard();

    if ((Window) && (Window->GetWin() == NULL)) {
        LoadConfigFile = fname;
        CItemMenu* imenu = (CItemMenu*)Window->GetChildByName("menu1")
                               ->GetChildByName("menu1_Edit")
                               ->GetChildByName("menu1_Edit_Clearpinalias");
        (Window->*(imenu->EvMenuActive))(NULL);
        return 0;
    } else {
        LoadConfigFile = "";
    }

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

    for (int i = PinsCount; i < (256 - PinsCount); i++) {
        Pins[i].avalue = 0;
        Pins[i].lvalue = 0;
        Pins[i].pord = 0;
        Pins[i].port = 0;
        Pins[i].value = 0;
        Pins[i].ptype = PT_DIGITAL;
        Pins[i].dir = PD_IN;
        Pins[i].ovalue = 0;
        Pins[i].oavalue = 55;
    }

    bool ret = lxFileExists(fname);

    if (ret) {
        int partsc_;
        int partsc_aup_;
        LoadFromFile(prefs, fname.c_str());

        DeleteParts();
        partsc_ = 0;
        partsc_aup_ = 0;

        if (prefs.at(0).find("version") != std::string::npos) {
            newformat = 1;
        }

        for (unsigned int i = 0; i < prefs.size(); i++) {
            if (newformat) {
                sscanf(prefs.at(i).c_str(), "%255[^,],%i,%i,%i:%4095[^\n]", name, &x, &y, &orient, temp);
            } else {
                sscanf(prefs.at(i).c_str(), "%255[^,],%i,%i:%4095[^\n]", name, &x, &y, temp);
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
                unsigned char useAlias;
                sscanf(temp, "%hhu", &useAlias);
                SetUseAlias(useAlias);
                Oscilloscope.SetBaseTimer();
            } else if (!strcmp(name, "version")) {
                // use planed in future
            } else if (!strcmp(name, "position")) {
                int w, h;
                w = orient;
                sscanf(temp, "%i", &h);
                if (Window) {
                    Window->SetX(x);
                    Window->SetY(y);
                    if (w > 5000)
                        w = 5000;
                    if (h > 5000)
                        h = 5000;
                    Window->SetWidth(w);
                    Window->SetHeight(h);
                    Window->GetChildByName("draw1")->SetWidth(w - 15);
                    Window->GetChildByName("draw1")->SetHeight(h - 40);
                }
            } else if (!strcmp(name, "boardp")) {
                int w, h;
                w = orient;
                sscanf(temp, "%i", &h);
                if (PICSimLab.GetWindow()) {
                    PICSimLab.GetWindow()->SetX(x);
                    PICSimLab.GetWindow()->SetY(y);
                    PICSimLab.GetWindow()->SetWidth(w);
                    PICSimLab.GetWindow()->SetHeight(h);
                }
            } else if (!strcmp(name, "spare_on")) {
                unsigned char spare_on;
                sscanf(temp, "%hhu", &spare_on);
                PICSimLab.GetBoard()->SetUseSpareParts(spare_on);
                if (Window) {
                    if (spare_on) {
                        Window->Show();
                        PICSimLab.GetBoard()->Reset();
                    } else {
                        Window->Hide();
                    }
                }
            } else if (!strcmp(name, "debug")) {
#ifndef NO_DEBUG
                if (disable_debug) {
                    x = 0;
                }
                if (PICSimLab.GetWindow()) {
                    ((CToggleButton*)PICSimLab.GetWindow()->GetChildByName("togglebutton1"))->SetCheck(x);
                }
#endif
                PICSimLab.SetDebugStatus(x);
                PICSimLab.SetDebugType(y);
            } else if (!strcmp(name, "osc_on")) {
                unsigned char osc_on;
                sscanf(temp, "%hhu", &osc_on);
                PICSimLab.GetBoard()->SetUseOscilloscope(osc_on);
                if (Oscilloscope.GetWindow()) {
                    if (osc_on) {
                        Oscilloscope.GetWindow()->Show();
                    } else {
                        Oscilloscope.GetWindow()->Hide();
                    }
                }
            } else if (!strcmp(name, "osc_cfg")) {
                osc_list.clear();
                osc_list.push_back(prefs.at(i));
            } else if (!strcmp(name, "osc_ch1")) {
                osc_list.push_back(prefs.at(i));
            } else if (!strcmp(name, "osc_ch2")) {
                osc_list.push_back(prefs.at(i));
                Oscilloscope.ReadPreferencesList(osc_list);
            } else if ((parts[partsc_] = create_part(name, x, y, PICSimLab.GetBoard()))) {
                printf("Spare parts: parts[%02i] (%s) created \n", partsc_, name);
                parts[partsc_]->ReadPreferences(temp);
                parts[partsc_]->SetId(partsc_);
                if (newformat) {
                    parts[partsc_]->SetOrientation(orient);
                    parts[partsc_]->SetScale(scale);
                    parts[partsc_]->SetUpdate(1);
                    parts[partsc_]->Reset();
                }
                partsc_++;
            } else {
                printf("Spare parts: Error loading part: %s \n", name);
                char temp[512];
                snprintf(temp, 512, "Spare parts:\nError loading part: %s \n", name);
                PICSimLab.RegisterError(temp);
            }
        }
        partsc = partsc_;
        partsc_aup = partsc_aup_;
    }

    return ret;
}

void CSpareParts::DeletePart(const int partn) {
    int partsc_ = partsc;
    partsc = 0;  // disable process
    partsc_aup = 0;

    delete parts[partn];

    for (int i = partn; i < partsc_ - 1; i++) {
        parts[i] = parts[i + 1];
    }
    partsc_--;

    partsc = partsc_;
}

void CSpareParts::PreProcess(void) {
    int i;

    memset(pullup_bus, 0, PinsCount);

    partsc_aup = 0;
    for (i = 0; i < partsc; i++) {
        parts[i]->PreProcess();
        if (parts[i]->GetAlwaysUpdate()) {
            parts_aup[partsc_aup] = parts[i];
            partsc_aup++;
        }
    }

    pullup_bus_count = 0;
    for (i = 0; i < PinsCount; i++) {
        if (pullup_bus[i] > 0)  // need register bus
        {
            pullup_bus_ptr[pullup_bus_count] = i;
            pullup_bus_count++;
        }
        pullup_bus[i] = 0;
    }
}

void CSpareParts::Process(void) {
    int i;

    if (ioupdated) {
        for (i = 0; i < pullup_bus_count; i++) {
            pullup_bus[pullup_bus_ptr[i]] = 1;
        }
        for (i = 0; i < partsc; i++) {
            parts[i]->Process();
        }
        for (i = 0; i < pullup_bus_count; i++) {
            SetPin(pullup_bus_ptr[i] + 1, pullup_bus[pullup_bus_ptr[i]]);
        }
    } else {
        for (i = 0; i < partsc_aup; i++) {
            parts_aup[i]->Process();
        }
    }
}

void CSpareParts::PostProcess(void) {
    for (int i = 0; i < partsc; i++) {
        parts[i]->PostProcess();
    }
}

void CSpareParts::Reset(void) {
    Pins = (picpin*)pboard->MGetPinsValues();
    for (int i = 0; i < GetCount(); i++) {
        parts[i]->Reset();
        parts[i]->SetUpdate(1);
    }
}

void CSpareParts::ReadPreferences(char* name, char* value) {
    if (!strcmp(name, "spare_position")) {
        if (Window) {
            int x, y, w, h;
            sscanf(value, "%i,%i,%i,%i", &x, &y, &w, &h);
            Window->SetX(x);
            Window->SetY(y);
            if (w > 5000)
                w = 5000;
            if (h > 5000)
                h = 5000;
            Window->SetWidth(w);
            Window->SetHeight(h);
            Window->GetChildByName("draw1")->SetWidth(w - 15);
            Window->GetChildByName("draw1")->SetHeight(h - 40);
        }
    }
}

void CSpareParts::WritePreferences(void) {
    // PICSimLab.SavePrefs("spare_position", std::to_string(Window->GetX()) + "," + std::to_string(Window->GetY()) + ","
    // +
    //                                               std::to_string(Window->GetWidth()) + "," +
    //                                               std::to_string(Window->GetHeight()));
}

bool CSpareParts::SaveConfig(std::string fname) {
    char temp[256];

    std::vector<std::string> prefs;

    // if (Window->GetWin() == NULL)
    //     return 0;

    prefs.clear();

    snprintf(temp, 256, "version,0,0,0:%s", _VERSION_);
    prefs.push_back(temp);
    snprintf(temp, 256, "scale,0,0,0:%f", scale);
    prefs.push_back(temp);
    if (Window) {
        snprintf(temp, 256, "position,%i,%i,%i:%i", Window->GetX(), Window->GetY(), Window->GetWidth(),
                 Window->GetHeight());
        prefs.push_back(temp);
    }
    if (PICSimLab.GetWindow()) {
        snprintf(temp, 256, "boardp,%i,%i,%i:%i", PICSimLab.GetWindow()->GetX(), PICSimLab.GetWindow()->GetY(),
                 PICSimLab.GetWindow()->GetWidth(), PICSimLab.GetWindow()->GetHeight());
        prefs.push_back(temp);
    }
    snprintf(temp, 256, "spare_on,0,0,0:%i", PICSimLab.GetBoard()->GetUseSpareParts());
    prefs.push_back(temp);
    snprintf(temp, 256, "osc_on,0,0,0:%i", PICSimLab.GetBoard()->GetUseOscilloscope());
    prefs.push_back(temp);
    snprintf(temp, 256, "useAlias,0,0,0:%i", GetUseAlias());
    prefs.push_back(temp);
    snprintf(temp, 256, "debug,%i,%i,0:", PICSimLab.GetDebugStatus(), PICSimLab.GetDebugType());
    prefs.push_back(temp);

    std::vector<std::string> osc_list = Oscilloscope.WritePreferencesList();
    for (unsigned int ol = 0; ol < osc_list.size(); ol++) {
        prefs.push_back(osc_list.at(ol));
    }

    for (int i = 0; i < GetCount(); i++) {
        snprintf(temp, 256, "%s,%i,%i,%i:%s", GetPart(i)->GetName().c_str(), GetPart(i)->GetX(), GetPart(i)->GetY(),
                 GetPart(i)->GetOrientation(), GetPart(i)->WritePreferences().c_str());
        prefs.push_back(temp);
    }

    return SaveToFile(prefs, fname.c_str());
}

void CSpareParts::Setfdtype(int value) {
    fdtype = value;
    oldfname = filedialog->GetFileName();
}

void CSpareParts::SetfdOldFilename(const std::string ofn) {
    oldfname = ofn;
}

lxBitmap* CSpareParts::LoadImageFile(const std::string fname, const float scale, const int usealpha,
                                     const int orientation) {
    if (OnLoadImage) {
        return (*OnLoadImage)(fname, scale, usealpha, orientation);
    }
    return NULL;
}

lxBitmap* CSpareParts::CreateBlankImage(const unsigned int width, const unsigned int height, const float scale,
                                        const int usealpha, const int orientation) {
    if (OnCreateImage) {
        return (*OnCreateImage)(width, height, scale, usealpha, orientation);
    }
    return NULL;
}