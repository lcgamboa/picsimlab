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

cpart_pot::cpart_pot(const unsigned x, const unsigned y, const char* name, const char* type, board* pboard_,
                     const int id_)
    : part(x, y, name, type, pboard_, id_, 9) {
    Size = 0;
    BitmapId = -1;

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
    SpareParts.SetPartOnDraw(id);
    SpareParts.CanvasCmd({CC_FREEBITMAP, .FreeBitmap{BitmapId}});
    SpareParts.CanvasCmd({CC_DESTROY});
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
            SpareParts.CanvasCmd({CC_SETCOLOR, .SetColor{49, 61, 99}});
            SpareParts.CanvasCmd({CC_RECTANGLE, .Rectangle{1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1,
                                                           output[i].y2 - output[i].y1}});
            SpareParts.CanvasCmd({CC_SETFGCOLOR, .SetFgColor{255, 255, 255}});
            if (output_pins[output[i].id - O_P1] == 0)
                SpareParts.CanvasCmd({CC_ROTATEDTEXT, .RotatedText{"NC", output[i].x1, output[i].y1, 0}});
            else
                SpareParts.CanvasCmd(
                    {CC_ROTATEDTEXT, .RotatedText{SpareParts.GetPinName(output_pins[output[i].id - O_P1]).c_str(),
                                                  output[i].x1, output[i].y1, 0}});
            break;
        case O_PO1:
        case O_PO2:
        case O_PO3:
        case O_PO4:
            snprintf(val, 10, "%4.2f", vmax * (values[output[i].id - O_PO1]) / 200.0);
            DrawPotentiometer(SpareParts.CanvasCmd, &output[i], values[output[i].id - O_PO1], val, 7);
            SpareParts.CanvasCmd({CC_SETFONTSIZE, .SetFontSize{9}});
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

    printf("Error input '%s' don't have a valid id! \n", name);
    return INVALID_ID;
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

    printf("Error output '%s' don't have a valid id! \n", name);
    return INVALID_ID;
}

std::string cpart_pot::WritePreferences(void) {
    char prefs[256];

    sprintf(prefs, "%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%u", output_pins[0], output_pins[1], output_pins[2],
            output_pins[3], values[0], values[1], values[2], values[3], Size);

    return prefs;
}

void cpart_pot::ReadPreferences(std::string value) {
    unsigned int sz;
    sscanf(value.c_str(), "%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%u", &output_pins[0], &output_pins[1],
           &output_pins[2], &output_pins[3], &values[0], &values[1], &values[2], &values[3], &sz);
    ChangeSize(sz);
}

void cpart_pot::ConfigurePropertiesWindow(void) {
    SetPCWComboWithPinNames("combo2", output_pins[0]);
    SetPCWComboWithPinNames("combo3", output_pins[1]);
    SetPCWComboWithPinNames("combo4", output_pins[2]);
    SetPCWComboWithPinNames("combo5", output_pins[3]);

    SpareParts.WPropCmd("spin7", PWA_SPINSETMAX, "4");
    SpareParts.WPropCmd("spin7", PWA_SPINSETMIN, "1");
    SpareParts.WPropCmd("spin7", PWA_SPINSETVALUE, std::to_string(Size).c_str());
    SpareParts.WPropCmd("spin7", PWA_SPINPROPEV, "1");

    SpinChange(NULL, Size);
}

void cpart_pot::ReadPropertiesWindow(void) {
    output_pins[0] = GetPWCComboSelectedPin("combo2");
    output_pins[1] = GetPWCComboSelectedPin("combo3");
    output_pins[2] = GetPWCComboSelectedPin("combo4");
    output_pins[3] = GetPWCComboSelectedPin("combo5");

    int size;
    SpareParts.WPropCmd("spin7", PWA_SPINGETVALUE, NULL, &size);
    ChangeSize(size);
}

void cpart_pot::SpinChange(const char* controlname, int value) {
    for (int i = 0; i < 4; i++) {
        char name[20];
        sprintf(name, "combo%i", i + 2);
        SpareParts.WPropCmd(name, PWA_SETENABLE, std::to_string(i < value).c_str());
    }
}

void cpart_pot::ChangeSize(const unsigned int sz) {
    if (Size != sz) {
        if (BitmapId >= 0) {
            SpareParts.SetPartOnDraw(id);
            SpareParts.CanvasCmd({CC_FREEBITMAP, .FreeBitmap{BitmapId}});
        }
        Size = sz;
        if (Size > 4) {
            Size = 4;
        }
        outputc = Size * 2;
        inputc = Size;
        LoadPartImage();
    }
}

void cpart_pot::LoadPartImage(void) {
    if (Size < 4) {
        xoff = (4 - Size) * 62;

        Width = OWidth - xoff;
        Height = OHeight;

        SpareParts.SetPartOnDraw(id);
        BitmapId = SpareParts.CanvasCmd({CC_CREATEIMAGE, .CreateImage{Width, Height, Scale, 0, Orientation}});

        SpareParts.CanvasCmd({CC_DESTROY});
        SpareParts.CanvasCmd({CC_CREATE, .Create{BitmapId}});

        int BackBitmap = SpareParts.CanvasCmd(
            {CC_LOADIMAGE, .LoadImage{(PICSimLab.GetSharePath() + "parts/" + Type + "/" + GetPictureFileName()).c_str(),
                                      Scale, 0, Orientation}});

        SpareParts.CanvasCmd({CC_INIT, .Init{Scale, Scale, Orientation}});
        SpareParts.CanvasCmd({CC_SETCOLOR, .SetColor{0x31, 0x3d, 0x63}});
        SpareParts.CanvasCmd({CC_RECTANGLE, .Rectangle{1, 0, 0, (float)Width, (float)Height}});

        SpareParts.CanvasCmd({CC_CHANGESCALE, .ChangeScale{1.0, 1.0}});
        SpareParts.CanvasCmd({CC_PUTBITMAP, .PutBitmap{BackBitmap, -xoff * Scale, 0}});

        SpareParts.CanvasCmd({CC_CHANGESCALE, .ChangeScale{Scale, Scale}});
        SpareParts.CanvasCmd({CC_END});

        SpareParts.CanvasCmd({CC_FREEBITMAP, .FreeBitmap{BackBitmap}});

    } else {
        Width = OWidth;
        Height = OHeight;
        xoff = 0;
        part::LoadPartImage();
    }
}

part_init(PART_POT_Name, cpart_pot, "Input");
