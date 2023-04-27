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

#include "input_pot.h"
#include "../lib/oscilloscope.h"
#include "../lib/picsimlab.h"
#include "../lib/spareparts.h"

/* outputs */
enum { O_P1, O_P2, O_P3, O_P4, O_PO1, O_PO2, O_PO3, O_PO4 };

/* inputs */
enum { I_PO1, I_PO2, I_PO3, I_PO4 };

static PCWProp pcwprop[8] = {{PCW_LABEL, "1-VCC,+5V"}, {PCW_COMBO, "2-POT 1"}, {PCW_COMBO, "3-POT 2"},
                             {PCW_COMBO, "4-POT 3"},   {PCW_COMBO, "5-POT 4"}, {PCW_LABEL, "6-GND ,GND"},
                             {PCW_SPIN, "Size"},       {PCW_END, ""}};

cpart_pot::cpart_pot(const unsigned x, const unsigned y, const char* name, const char* type, board* pboard_)
    : part(x, y, name, type, pboard_, 9), font_p(7, lxFONTFAMILY_TELETYPE, lxFONTSTYLE_NORMAL, lxFONTWEIGHT_BOLD) {
    Size = 0;
    Bitmap = NULL;

    ReadMaps();
    OWidth = Width;
    OHeight = Height;

    ChangeSize(4);

    output_pins[0] = 0;
    output_pins[1] = 0;
    output_pins[2] = 0;
    output_pins[3] = 0;

    values[0] = 0;
    values[1] = 0;
    values[2] = 0;
    values[3] = 0;

    active[0] = 0;
    active[1] = 0;
    active[2] = 0;
    active[3] = 0;

    vmax = 5.0;

    SetPCWProperties(pcwprop);
    PinCount = 4;
    Pins = output_pins;
}

void cpart_pot::RegisterRemoteControl(void) {
    input_ids[I_PO1]->status = &values[0];
    input_ids[I_PO1]->update = &output_ids[O_PO1]->update;
    input_ids[I_PO2]->status = &values[1];
    input_ids[I_PO2]->update = &output_ids[O_PO2]->update;
    input_ids[I_PO3]->status = &values[2];
    input_ids[I_PO3]->update = &output_ids[O_PO3]->update;
    input_ids[I_PO4]->status = &values[3];
    input_ids[I_PO4]->update = &output_ids[O_PO4]->update;
}

cpart_pot::~cpart_pot(void) {
    delete Bitmap;
    canvas.Destroy();
}

void cpart_pot::Reset(void) {
    vmax = pboard->MGetVCC();
}

void cpart_pot::DrawOutput(const unsigned int i) {
    char val[10];

    switch (output[i].id) {
        case O_P1:
        case O_P2:
        case O_P3:
        case O_P4:
            canvas.SetColor(49, 61, 99);
            canvas.Rectangle(1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);
            canvas.SetFgColor(255, 255, 255);
            if (output_pins[output[i].id - O_P1] == 0)
                canvas.RotatedText("NC", output[i].x1, output[i].y1, 0);
            else
                canvas.RotatedText(SpareParts.GetPinName(output_pins[output[i].id - O_P1]), output[i].x1, output[i].y1,
                                   0);
            break;
        case O_PO1:
        case O_PO2:
        case O_PO3:
        case O_PO4:
            snprintf(val, 10, "%4.2f", vmax * (values[output[i].id - O_PO1]) / 200.0);
            DrawPotentiometer(&output[i], values[output[i].id - O_PO1], val, font_p);
            canvas.SetFont(font);
            break;
    }
}

void cpart_pot::PreProcess(void) {
    for (unsigned int i = 0; i < Size; i++) {
        SpareParts.SetAPin(output_pins[i], vmax * (values[i]) / 200.0);
    }
}

void cpart_pot::OnMouseButtonPress(uint inputId, uint button, uint x, uint y, uint state) {
    switch (inputId) {
        case I_PO1:
            values[0] = 200 - ((y - input_ids[I_PO1]->y1) * 1.66);
            if (values[0] > 200)
                values[0] = 0;
            active[0] = 1;
            output_ids[O_PO1]->update = 1;
            break;
        case I_PO2:
            values[1] = 200 - ((y - input_ids[I_PO2]->y1) * 1.66);
            if (values[1] > 200)
                values[1] = 0;
            active[1] = 1;
            output_ids[O_PO2]->update = 1;
            break;
        case I_PO3:
            values[2] = 200 - ((y - input_ids[I_PO3]->y1) * 1.66);
            if (values[2] > 200)
                values[2] = 0;
            active[2] = 1;
            output_ids[O_PO3]->update = 1;
            break;
        case I_PO4:
            values[3] = 200 - ((y - input_ids[I_PO4]->y1) * 1.66);
            if (values[3] > 200)
                values[3] = 0;
            active[3] = 1;
            output_ids[O_PO4]->update = 1;
            break;
    }
}

void cpart_pot::OnMouseButtonRelease(uint inputId, uint button, uint x, uint y, uint state) {
    switch (inputId) {
        case I_PO1:
            active[0] = 0;
            output_ids[O_PO1]->update = 1;
            break;
        case I_PO2:
            active[1] = 0;
            output_ids[O_PO2]->update = 1;
            break;
        case I_PO3:
            active[2] = 0;
            output_ids[O_PO3]->update = 1;
            break;
        case I_PO4:
            active[3] = 0;
            output_ids[O_PO4]->update = 1;
            break;
    }
}

void cpart_pot::OnMouseMove(uint inputId, uint button, uint x, uint y, uint state) {
    switch (inputId) {
        case I_PO1:
        case I_PO2:
        case I_PO3:
        case I_PO4:
            if (active[inputId - I_PO1]) {
                values[inputId - I_PO1] = 200 - ((y - input_ids[inputId]->y1) * 1.66);
                if (values[inputId - I_PO1] > 200)
                    values[inputId - I_PO1] = 0;
                output_ids[O_PO1 + inputId - I_PO1]->update = 1;
            }
            break;
        default:
            active[0] = 0;
            active[1] = 0;
            active[2] = 0;
            active[3] = 0;
            break;
    }
}

unsigned short cpart_pot::GetInputId(char* name) {
    if (strcmp(name, "PO_1") == 0)
        return I_PO1;
    if (strcmp(name, "PO_2") == 0)
        return I_PO2;
    if (strcmp(name, "PO_3") == 0)
        return I_PO3;
    if (strcmp(name, "PO_4") == 0)
        return I_PO4;

    printf("Erro input '%s' don't have a valid id! \n", name);
    return -1;
}

unsigned short cpart_pot::GetOutputId(char* name) {
    if (strcmp(name, "PN_1") == 0)
        return O_P1;
    if (strcmp(name, "PN_2") == 0)
        return O_P2;
    if (strcmp(name, "PN_3") == 0)
        return O_P3;
    if (strcmp(name, "PN_4") == 0)
        return O_P4;

    if (strcmp(name, "PO_1") == 0)
        return O_PO1;
    if (strcmp(name, "PO_2") == 0)
        return O_PO2;
    if (strcmp(name, "PO_3") == 0)
        return O_PO3;
    if (strcmp(name, "PO_4") == 0)
        return O_PO4;

    printf("Erro output '%s' don't have a valid id! \n", name);
    return 1;
}

lxString cpart_pot::WritePreferences(void) {
    char prefs[256];

    sprintf(prefs, "%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%u", output_pins[0], output_pins[1], output_pins[2],
            output_pins[3], values[0], values[1], values[2], values[3], Size);

    return prefs;
}

void cpart_pot::ReadPreferences(lxString value) {
    unsigned int sz;
    sscanf(value.c_str(), "%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%u", &output_pins[0], &output_pins[1],
           &output_pins[2], &output_pins[3], &values[0], &values[1], &values[2], &values[3], &sz);
    ChangeSize(sz);
}

void cpart_pot::ConfigurePropertiesWindow(CPWindow* WProp) {
    SetPCWComboWithPinNames(WProp, "combo2", output_pins[0]);
    SetPCWComboWithPinNames(WProp, "combo3", output_pins[1]);
    SetPCWComboWithPinNames(WProp, "combo4", output_pins[2]);
    SetPCWComboWithPinNames(WProp, "combo5", output_pins[3]);
    ((CSpin*)WProp->GetChildByName("spin7"))->SetMax(4);
    ((CSpin*)WProp->GetChildByName("spin7"))->SetMin(1);
    ((CSpin*)WProp->GetChildByName("spin7"))->SetValue(Size);
    ((CSpin*)WProp->GetChildByName("spin7"))->EvOnChangeSpin = SpareParts.PropSpinChange;
    SpinChange(WProp, NULL, Size);
}

void cpart_pot::ReadPropertiesWindow(CPWindow* WProp) {
    output_pins[0] = GetPWCComboSelectedPin(WProp, "combo2");
    output_pins[1] = GetPWCComboSelectedPin(WProp, "combo3");
    output_pins[2] = GetPWCComboSelectedPin(WProp, "combo4");
    output_pins[3] = GetPWCComboSelectedPin(WProp, "combo5");

    ChangeSize(((CSpin*)WProp->GetChildByName("spin7"))->GetValue());
}

void cpart_pot::SpinChange(CPWindow* WProp, CSpin* control, int value) {
    for (int i = 0; i < 4; i++) {
        char name[20];
        sprintf(name, "combo%i", i + 2);
        ((CCombo*)WProp->GetChildByName(name))->SetEnable(i < value);
    }
}

void cpart_pot::ChangeSize(const unsigned int sz) {
    if (Size != sz) {
        if (Bitmap) {
            delete Bitmap;
        }
        Size = sz;
        if (Size > 4) {
            Size = 4;
        }
        outputc = Size * 2;
        inputc = Size;
        LoadImage();
    }
}

void cpart_pot::LoadImage(void) {
    if (Size < 4) {
        xoff = (4 - Size) * 62;

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

part_init(PART_POT_Name, cpart_pot, "Input");
