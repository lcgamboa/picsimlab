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

#include "input_push_buttons.h"
#include "../lib/oscilloscope.h"
#include "../lib/picsimlab.h"
#include "../lib/spareparts.h"

/* outputs */
enum { O_P1, O_P2, O_P3, O_P4, O_P5, O_P6, O_P7, O_P8, O_B1, O_B2, O_B3, O_B4, O_B5, O_B6, O_B7, O_B8, O_J1 };

/* inputs */
enum { I_B1, I_B2, I_B3, I_B4, I_B5, I_B6, I_B7, I_B8, I_J1 };

enum { MODE_NORMAL, MODE_IDEAL };

static PCWProp pcwprop[14] = {{PCW_LABEL, "1 -VCC,+5V"}, {PCW_COMBO, "2 -Out 1"},
                              {PCW_COMBO, "3 -Out 2"},   {PCW_COMBO, "4 -Out 3"},
                              {PCW_COMBO, "5 -Out 4"},   {PCW_COMBO, "6 -Out 5"},
                              {PCW_COMBO, "7 -Out 6"},   {PCW_COMBO, "8 -Out 7"},
                              {PCW_COMBO, "9 -Out 8"},   {PCW_LABEL, "10-GND ,GND"},
                              {PCW_COMBO, "Active"},     {PCW_COMBO, "Mode"},
                              {PCW_SPIN, "Size"},        {PCW_END, ""}};

cpart_pbuttons::cpart_pbuttons(const unsigned x, const unsigned y, const char* name, const char* type, board* pboard_)
    : part(x, y, name, type, pboard_), font(9, lxFONTFAMILY_TELETYPE, lxFONTSTYLE_NORMAL, lxFONTWEIGHT_BOLD) {
    active = 1;
    Size = 0;
    Bitmap = NULL;

    ReadMaps();
    OWidth = Width;
    OHeight = Height;

    ChangeSize(8);

    output_pins[0] = 0;
    output_pins[1] = 0;
    output_pins[2] = 0;
    output_pins[3] = 0;
    output_pins[4] = 0;
    output_pins[5] = 0;
    output_pins[6] = 0;
    output_pins[7] = 0;

    output_value[0] = !active;
    output_value[1] = !active;
    output_value[2] = !active;
    output_value[3] = !active;
    output_value[4] = !active;
    output_value[5] = !active;
    output_value[6] = !active;
    output_value[7] = !active;

    mode = MODE_NORMAL;

    SWBounce_init(&bounce, 8);

    SetPCWProperties(pcwprop);

    PinCount = 8;
    Pins = output_pins;
}

void cpart_pbuttons::RegisterRemoteControl(void) {
    input_ids[I_B1]->status = &output_value[0];
    input_ids[I_B1]->update = &output_ids[O_B1]->update;
    input_ids[I_B2]->status = &output_value[1];
    input_ids[I_B2]->update = &output_ids[O_B2]->update;
    input_ids[I_B3]->status = &output_value[2];
    input_ids[I_B3]->update = &output_ids[O_B3]->update;
    input_ids[I_B4]->status = &output_value[3];
    input_ids[I_B4]->update = &output_ids[O_B4]->update;
    input_ids[I_B5]->status = &output_value[4];
    input_ids[I_B5]->update = &output_ids[O_B5]->update;
    input_ids[I_B6]->status = &output_value[5];
    input_ids[I_B6]->update = &output_ids[O_B6]->update;
    input_ids[I_B7]->status = &output_value[6];
    input_ids[I_B7]->update = &output_ids[O_B7]->update;
    input_ids[I_B8]->status = &output_value[7];
    input_ids[I_B8]->update = &output_ids[O_B8]->update;
}

void cpart_pbuttons::Reset(void) {
    // release all
    output_value[0] = !active;
    output_value[1] = !active;
    output_value[2] = !active;
    output_value[3] = !active;
    output_value[4] = !active;
    output_value[5] = !active;
    output_value[6] = !active;
    output_value[7] = !active;

    // force pin update
    SpareParts.SetPin(output_pins[0], !output_value[0]);
    SpareParts.SetPin(output_pins[1], !output_value[1]);
    SpareParts.SetPin(output_pins[2], !output_value[2]);
    SpareParts.SetPin(output_pins[3], !output_value[3]);
    SpareParts.SetPin(output_pins[4], !output_value[4]);
    SpareParts.SetPin(output_pins[5], !output_value[5]);
    SpareParts.SetPin(output_pins[6], !output_value[6]);
    SpareParts.SetPin(output_pins[7], !output_value[7]);

    SpareParts.SetPin(output_pins[0], output_value[0]);
    SpareParts.SetPin(output_pins[1], output_value[1]);
    SpareParts.SetPin(output_pins[2], output_value[2]);
    SpareParts.SetPin(output_pins[3], output_value[3]);
    SpareParts.SetPin(output_pins[4], output_value[4]);
    SpareParts.SetPin(output_pins[5], output_value[5]);
    SpareParts.SetPin(output_pins[6], output_value[6]);
    SpareParts.SetPin(output_pins[7], output_value[7]);
}

cpart_pbuttons::~cpart_pbuttons(void) {
    delete Bitmap;
    canvas.Destroy();
    SWBounce_end(&bounce);
}

void cpart_pbuttons::DrawOutput(const unsigned int i) {
    switch (output[i].id) {
        case O_P1:
        case O_P2:
        case O_P3:
        case O_P4:
        case O_P5:
        case O_P6:
        case O_P7:
        case O_P8:
            canvas.SetColor(49, 61, 99);
            canvas.Rectangle(1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);
            canvas.SetFgColor(255, 255, 255);
            if (output_pins[output[i].id - O_P1] == 0)
                canvas.RotatedText("NC", output[i].x1, output[i].y1, 0);
            else
                canvas.RotatedText(SpareParts.GetPinName(output_pins[output[i].id - O_P1]), output[i].x1, output[i].y1,
                                   0);
            break;
        case O_B1:
        case O_B2:
        case O_B3:
        case O_B4:
        case O_B5:
        case O_B6:
        case O_B7:
        case O_B8:
            canvas.SetColor(100, 100, 100);
            canvas.Circle(1, output[i].cx, output[i].cy, 10);
            if (output_value[output[i].id - O_B1] == active) {
                canvas.SetColor(55, 55, 55);
            } else {
                canvas.SetColor(15, 15, 15);
            }
            canvas.Circle(1, output[i].cx, output[i].cy, 8);
            break;
    }
}

void cpart_pbuttons::PreProcess(void) {
    if (mode == MODE_NORMAL) {
        const picpin* ppins = SpareParts.GetPinsValues();
        SWBounce_prepare(&bounce, pboard->MGetInstClockFreq());

        for (unsigned int i = 0; i < Size; i++) {
            if (output_pins[i]) {
                if ((ppins[output_pins[i] - 1].dir == PD_IN) && (ppins[output_pins[i] - 1].value != output_value[i])) {
                    SWBounce_bounce(&bounce, i);
                }
            }
        }
        SetAlwaysUpdate(bounce.do_bounce);
    } else {
        for (unsigned int i = 0; i < Size; i++) {
            if (output_pins[i]) {
                SpareParts.SetPin(output_pins[i], output_value[i]);
            }
        }
        SetAlwaysUpdate(0);
    }
}

void cpart_pbuttons::Process(void) {
    if (mode == MODE_NORMAL) {
        const int ret = SWBounce_process(&bounce);
        if (ret) {
            const picpin* ppins = SpareParts.GetPinsValues();

            for (unsigned int i = 0; i < Size; i++) {
                if (bounce.bounce[i]) {
                    if (ret == 1) {
                        SpareParts.SetPin(output_pins[i], !ppins[output_pins[i] - 1].value);
                    } else {
                        SpareParts.SetPin(output_pins[i], output_value[i]);
                    }
                }
            }
        }
    }
}

void cpart_pbuttons::OnMouseButtonPress(uint inputId, uint button, uint x, uint y, uint state) {
    switch (inputId) {
        case I_B1:
            output_value[0] = active;
            output_ids[O_B1]->update = 1;
            break;
        case I_B2:
            output_value[1] = active;
            output_ids[O_B2]->update = 1;
            break;
        case I_B3:
            output_value[2] = active;
            output_ids[O_B3]->update = 1;
            break;
        case I_B4:
            output_value[3] = active;
            output_ids[O_B4]->update = 1;
            break;
        case I_B5:
            output_value[4] = active;
            output_ids[O_B5]->update = 1;
            break;
        case I_B6:
            output_value[5] = active;
            output_ids[O_B6]->update = 1;
            break;
        case I_B7:
            output_value[6] = active;
            output_ids[O_B7]->update = 1;
            break;
        case I_B8:
            output_value[7] = active;
            output_ids[O_B8]->update = 1;
            break;
    }
}

void cpart_pbuttons::OnMouseButtonRelease(uint inputId, uint button, uint x, uint y, uint state) {
    switch (inputId) {
        case I_B1:
            output_value[0] = !active;
            output_ids[O_B1]->update = 1;
            break;
        case I_B2:
            output_value[1] = !active;
            output_ids[O_B2]->update = 1;
            break;
        case I_B3:
            output_value[2] = !active;
            output_ids[O_B3]->update = 1;
            break;
        case I_B4:
            output_value[3] = !active;
            output_ids[O_B4]->update = 1;
            break;
        case I_B5:
            output_value[4] = !active;
            output_ids[O_B5]->update = 1;
            break;
        case I_B6:
            output_value[5] = !active;
            output_ids[O_B6]->update = 1;
            break;
        case I_B7:
            output_value[6] = !active;
            output_ids[O_B7]->update = 1;
            break;
        case I_B8:
            output_value[7] = !active;
            output_ids[O_B8]->update = 1;
            break;
    }
}

unsigned short cpart_pbuttons::GetInputId(char* name) {
    if (strcmp(name, "PB_1") == 0)
        return I_B1;
    if (strcmp(name, "PB_2") == 0)
        return I_B2;
    if (strcmp(name, "PB_3") == 0)
        return I_B3;
    if (strcmp(name, "PB_4") == 0)
        return I_B4;
    if (strcmp(name, "PB_5") == 0)
        return I_B5;
    if (strcmp(name, "PB_6") == 0)
        return I_B6;
    if (strcmp(name, "PB_7") == 0)
        return I_B7;
    if (strcmp(name, "PB_8") == 0)
        return I_B8;

    if (strcmp(name, "JP_1") == 0)
        return I_J1;

    printf("Erro input '%s' don't have a valid id! \n", name);
    return -1;
}

unsigned short cpart_pbuttons::GetOutputId(char* name) {
    if (strcmp(name, "PN_1") == 0)
        return O_P1;
    if (strcmp(name, "PN_2") == 0)
        return O_P2;
    if (strcmp(name, "PN_3") == 0)
        return O_P3;
    if (strcmp(name, "PN_4") == 0)
        return O_P4;
    if (strcmp(name, "PN_5") == 0)
        return O_P5;
    if (strcmp(name, "PN_6") == 0)
        return O_P6;
    if (strcmp(name, "PN_7") == 0)
        return O_P7;
    if (strcmp(name, "PN_8") == 0)
        return O_P8;

    if (strcmp(name, "PB_1") == 0)
        return O_B1;
    if (strcmp(name, "PB_2") == 0)
        return O_B2;
    if (strcmp(name, "PB_3") == 0)
        return O_B3;
    if (strcmp(name, "PB_4") == 0)
        return O_B4;
    if (strcmp(name, "PB_5") == 0)
        return O_B5;
    if (strcmp(name, "PB_6") == 0)
        return O_B6;
    if (strcmp(name, "PB_7") == 0)
        return O_B7;
    if (strcmp(name, "PB_8") == 0)
        return O_B8;

    if (strcmp(name, "JP_1") == 0)
        return O_J1;

    printf("Erro output '%s' don't have a valid id! \n", name);
    return 1;
}

lxString cpart_pbuttons::WritePreferences(void) {
    char prefs[256];

    sprintf(prefs, "%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%u", output_pins[0], output_pins[1],
            output_pins[2], output_pins[3], output_pins[4], output_pins[5], output_pins[6], output_pins[7], active,
            mode, Size);

    return prefs;
}

void cpart_pbuttons::ReadPreferences(lxString value) {
    unsigned int sz;
    sscanf(value.c_str(), "%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%u", &output_pins[0], &output_pins[1],
           &output_pins[2], &output_pins[3], &output_pins[4], &output_pins[5], &output_pins[6], &output_pins[7],
           &active, &mode, &sz);
    output_value[0] = !active;
    output_value[1] = !active;
    output_value[2] = !active;
    output_value[3] = !active;
    output_value[4] = !active;
    output_value[5] = !active;
    output_value[6] = !active;
    output_value[7] = !active;
    ChangeSize(sz);
}

void cpart_pbuttons::ConfigurePropertiesWindow(CPWindow* WProp) {
    SetPCWComboWithPinNames(WProp, "combo2", output_pins[0]);
    SetPCWComboWithPinNames(WProp, "combo3", output_pins[1]);
    SetPCWComboWithPinNames(WProp, "combo4", output_pins[2]);
    SetPCWComboWithPinNames(WProp, "combo5", output_pins[3]);
    SetPCWComboWithPinNames(WProp, "combo6", output_pins[4]);
    SetPCWComboWithPinNames(WProp, "combo7", output_pins[5]);
    SetPCWComboWithPinNames(WProp, "combo8", output_pins[6]);
    SetPCWComboWithPinNames(WProp, "combo9", output_pins[7]);

    CCombo* combo = (CCombo*)WProp->GetChildByName("combo11");

    combo->SetItems("HIGH,LOW,");
    if (active)
        combo->SetText("HIGH");
    else
        combo->SetText("LOW ");

    combo = (CCombo*)WProp->GetChildByName("combo12");
    combo->SetItems("Ideal,Normal,");
    if (mode)
        combo->SetText("Ideal");
    else
        combo->SetText("Normal");

    ((CSpin*)WProp->GetChildByName("spin13"))->SetMax(8);
    ((CSpin*)WProp->GetChildByName("spin13"))->SetMin(1);
    ((CSpin*)WProp->GetChildByName("spin13"))->SetValue(Size);
    ((CSpin*)WProp->GetChildByName("spin13"))->EvOnChangeSpin = SpareParts.PropSpinChange;
    SpinChange(WProp, NULL, Size);
}

void cpart_pbuttons::ReadPropertiesWindow(CPWindow* WProp) {
    output_pins[0] = GetPWCComboSelectedPin(WProp, "combo2");
    output_pins[1] = GetPWCComboSelectedPin(WProp, "combo3");
    output_pins[2] = GetPWCComboSelectedPin(WProp, "combo4");
    output_pins[3] = GetPWCComboSelectedPin(WProp, "combo5");
    output_pins[4] = GetPWCComboSelectedPin(WProp, "combo6");
    output_pins[5] = GetPWCComboSelectedPin(WProp, "combo7");
    output_pins[6] = GetPWCComboSelectedPin(WProp, "combo8");
    output_pins[7] = GetPWCComboSelectedPin(WProp, "combo9");

    active = (((CCombo*)WProp->GetChildByName("combo11"))->GetText().compare("HIGH") == 0);

    mode = (((CCombo*)WProp->GetChildByName("combo12"))->GetText().compare("Ideal") == 0);

    ChangeSize(((CSpin*)WProp->GetChildByName("spin13"))->GetValue());

    output_value[0] = !active;
    output_value[1] = !active;
    output_value[2] = !active;
    output_value[3] = !active;
    output_value[4] = !active;
    output_value[5] = !active;
    output_value[6] = !active;
    output_value[7] = !active;
}

void cpart_pbuttons::SpinChange(CPWindow* WProp, CSpin* control, int value) {
    for (int i = 0; i < 8; i++) {
        char name[20];
        sprintf(name, "combo%i", i + 2);
        ((CCombo*)WProp->GetChildByName(name))->SetEnable(i < value);
    }
}

void cpart_pbuttons::ChangeSize(const unsigned int sz) {
    if (Size != sz) {
        if (Bitmap) {
            delete Bitmap;
        }
        Size = sz;
        if (Size > 8) {
            Size = 8;
        }
        outputc = Size * 2;
        inputc = Size;
        LoadImage();
    }
}

void cpart_pbuttons::LoadImage(void) {
    if (Size < 8) {
        xoff = (8 - Size) * 62;

        Width = OWidth - xoff;
        Height = OHeight;

        if (SpareParts.GetWindow()) {
            lxImage image(SpareParts.GetWindow());
            image.CreateBlank(Width, Height, Orientation, Scale, Scale);

            Bitmap = new lxBitmap(&image, SpareParts.GetWindow());
            image.Destroy();

            canvas.Destroy();
            canvas.Create(SpareParts.GetWindow()->GetWWidget(), Bitmap);

            image.LoadFile(lxGetLocalFile(PICSimLab.GetSharePath() + lxT("parts/") + Type + "/" + GetPictureFileName()),
                           Orientation, Scale, Scale);
            lxBitmap* BackBitmap = new lxBitmap(&image, SpareParts.GetWindow());
            image.Destroy();

            canvas.Init(Scale, Scale, Orientation);
            canvas.SetColor(0x31, 0x3d, 0x63);
            canvas.Rectangle(1, 0, 0, Width, Height);

            canvas.ChangeScale(1.0, 1.0);
            canvas.PutBitmap(BackBitmap, -xoff * Scale, 0);

            canvas.ChangeScale(Scale, Scale);
            canvas.End();

            delete BackBitmap;
        }
    } else {
        Width = OWidth;
        Height = OHeight;
        xoff = 0;
        part::LoadImage();
    }
}

part_init(PART_PUSH_BUTTONS_Name, cpart_pbuttons, "Input");
