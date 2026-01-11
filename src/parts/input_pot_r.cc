/* ########################################################################

   PICSimLab - Programmable IC Simulator Laboratory

   ########################################################################

   Copyright (c) : 2010-2026  Luis Claudio Gambôa Lopes <lcgamboa@yahoo.com>

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

#define _USE_MATH_DEFINES

#include "input_pot_r.h"
#include "../lib/oscilloscope.h"
#include "../lib/picsimlab.h"
#include "../lib/spareparts.h"
#include "math.h"

/* outputs */
enum { O_PO1, O_PO2, O_PO3, O_PO4, O_P1, O_P2, O_P3, O_P4 };

/* inputs */
enum { I_PO1, I_PO2, I_PO3, I_PO4 };

static PCWProp pcwprop[8] = {{PCW_LABEL, "1-VCC,+5V"}, {PCW_COMBO, "2-POT 1"}, {PCW_COMBO, "3-POT 2"},
                             {PCW_COMBO, "4-POT 3"},   {PCW_COMBO, "5-POT 4"}, {PCW_LABEL, "6-GND ,GND"},
                             {PCW_SPIN, "Size"},       {PCW_END, ""}};

cpart_pot_r::cpart_pot_r(const unsigned x, const unsigned y, const char* name, const char* type, board* pboard_,
                         const int id_)
    : part(x, y, name, type, pboard_, id_) {
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

void cpart_pot_r::RegisterRemoteControl(void) {
    input_ids[I_PO1]->status = &values[0];
    input_ids[I_PO1]->update = &output_ids[O_PO1]->update;
    input_ids[I_PO2]->status = &values[1];
    input_ids[I_PO2]->update = &output_ids[O_PO2]->update;
    input_ids[I_PO3]->status = &values[2];
    input_ids[I_PO3]->update = &output_ids[O_PO3]->update;
    input_ids[I_PO4]->status = &values[3];
    input_ids[I_PO4]->update = &output_ids[O_PO4]->update;
}

cpart_pot_r::~cpart_pot_r(void) {
    SpareParts.SetPartOnDraw(id);
    SpareParts.CanvasCmd({.cmd = CC_FREEBITMAP, .FreeBitmap{BitmapId}});
    SpareParts.CanvasCmd({.cmd = CC_DESTROY});
}

void cpart_pot_r::Reset(void) {
    vmax = pboard->MGetVCC();
}

void cpart_pot_r::DrawOutput(const unsigned int i) {
    char val[10];

    switch (output[i].id) {
        case O_P1:
        case O_P2:
        case O_P3:
        case O_P4:
            SpareParts.CanvasCmd({.cmd = CC_SETCOLOR, .SetColor{49, 61, 99}});
            SpareParts.CanvasCmd(
                {.cmd = CC_RECTANGLE,
                 .Rectangle{1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1}});
            SpareParts.CanvasCmd({.cmd = CC_SETFGCOLOR, .SetFgColor{255, 255, 255}});
            if (output_pins[output[i].id - O_P1] == 0)
                SpareParts.CanvasCmd({.cmd = CC_ROTATEDTEXT, .RotatedText{"NC", output[i].x1, output[i].y1, 0}});
            else
                SpareParts.CanvasCmd({.cmd = CC_ROTATEDTEXT,
                                      .RotatedText{SpareParts.GetPinName(output_pins[output[i].id - O_P1]).c_str(),
                                                   output[i].x1, output[i].y1, 0}});
            break;
        case O_PO1:
        case O_PO2:
        case O_PO3:
        case O_PO4:
            SpareParts.CanvasCmd({.cmd = CC_SETFGCOLOR, .SetFgColor{0, 0, 0}});
            SpareParts.CanvasCmd({.cmd = CC_SETBGCOLOR, .SetBgColor{66, 109, 246}});
            SpareParts.CanvasCmd(
                {.cmd = CC_RECTANGLE,
                 .Rectangle{1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1}});

            SpareParts.CanvasCmd({.cmd = CC_SETBGCOLOR, .SetBgColor{250, 250, 250}});
            SpareParts.CanvasCmd({.cmd = CC_CIRCLE, .Circle{1, output[i].cx, output[i].cy, 17}});

            SpareParts.CanvasCmd({.cmd = CC_SETBGCOLOR, .SetBgColor{150, 150, 150}});
            int x = -13 * sin((5.585 * (values[output[i].id - O_PO1] / 200.0)) + 0.349);
            int y = 13 * cos((5.585 * (values[output[i].id - O_PO1] / 200.0)) + 0.349);
            SpareParts.CanvasCmd({.cmd = CC_CIRCLE, .Circle{1, output[i].cx + x, output[i].cy + y, 2}});

            SpareParts.CanvasCmd({.cmd = CC_SETCOLOR, .SetColor{49, 61, 99}});
            SpareParts.CanvasCmd({.cmd = CC_RECTANGLE, .Rectangle{1, output[i].x1 + 6, output[i].y2 + 6, 30, 15}});
            snprintf(val, 10, "%4.2f", vmax * (values[output[i].id - O_PO1]) / 200.0);
            SpareParts.CanvasCmd({.cmd = CC_SETCOLOR, .SetColor{250, 250, 250}});
            SpareParts.CanvasCmd({.cmd = CC_SETFONTSIZE, .SetFontSize{8}});
            SpareParts.CanvasCmd({.cmd = CC_ROTATEDTEXT, .RotatedText{val, output[i].x1 + 6, output[i].y2 + 6, 0}});
            break;
    }
}

void cpart_pot_r::PreProcess(void) {
    for (unsigned int i = 0; i < Size; i++) {
        SpareParts.SetAPin(output_pins[i], vmax * (values[i]) / 200.0);
    }
}

void cpart_pot_r::OnMouseButtonPress(unsigned int inputId, unsigned int button, unsigned int x, unsigned int y,
                                     unsigned int state) {
    switch (inputId) {
        case I_PO1:
            values[0] = CalcAngle(inputId, x, y);
            active[0] = 1;
            output_ids[O_PO1]->update = 1;
            break;
        case I_PO2:
            values[1] = CalcAngle(inputId, x, y);
            active[1] = 1;
            output_ids[O_PO2]->update = 1;
            break;
        case I_PO3:
            values[2] = CalcAngle(inputId, x, y);
            active[2] = 1;
            output_ids[O_PO3]->update = 1;
            break;
        case I_PO4:
            values[3] = CalcAngle(inputId, x, y);
            active[3] = 1;
            output_ids[O_PO4]->update = 1;
            break;
    }
}

void cpart_pot_r::OnMouseButtonRelease(unsigned int inputId, unsigned int button, unsigned int x, unsigned int y,
                                       unsigned int state) {
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

void cpart_pot_r::OnMouseMove(unsigned int inputId, unsigned int button, unsigned int x, unsigned int y,
                              unsigned int state) {
    switch (inputId) {
        case I_PO1 ... I_PO4:
            if (active[inputId - I_PO1]) {
                values[inputId - I_PO1] = CalcAngle(inputId, x, y);
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

unsigned char cpart_pot_r::CalcAngle(int inputId, int x, int y) {
    int dx = input_ids[inputId]->cx - x;
    int dy = y - input_ids[inputId]->cy;
    double angle = 0;

    if ((dx >= 0) && (dy >= 0)) {
        angle = atan2(dx, dy) * 180 / M_PI;
    } else if ((dx >= 0) && (dy < 0)) {
        angle = 180 - (atan2(dx, -dy) * 180 / M_PI);
    } else if ((dx < 0) && (dy < 0)) {
        angle = (atan2(-dx, -dy) * 180 / M_PI) + 180;
    } else if ((dx < 0) && (dy >= 0)) {
        angle = 360 - (atan2(-dx, dy) * 180 / M_PI);
    }

    if (angle > 340)
        angle = 340;
    if (angle < 20)
        angle = 20;

    return (200 * (angle - 20) / 320.0);
}

unsigned short cpart_pot_r::GetInputId(char* name) {
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

unsigned short cpart_pot_r::GetOutputId(char* name) {
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

std::string cpart_pot_r::WritePreferences(void) {
    char prefs[256];

    sprintf(prefs, "%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%u", output_pins[0], output_pins[1], output_pins[2],
            output_pins[3], values[0], values[1], values[2], values[3], Size);

    return prefs;
}

void cpart_pot_r::ReadPreferences(std::string value) {
    unsigned int sz;
    sscanf(value.c_str(), "%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%u", &output_pins[0], &output_pins[1],
           &output_pins[2], &output_pins[3], &values[0], &values[1], &values[2], &values[3], &sz);
    ChangeSize(sz);
}

void cpart_pot_r::ConfigurePropertiesWindow(void) {
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

void cpart_pot_r::ReadPropertiesWindow(void) {
    output_pins[0] = GetPWCComboSelectedPin("combo2");
    output_pins[1] = GetPWCComboSelectedPin("combo3");
    output_pins[2] = GetPWCComboSelectedPin("combo4");
    output_pins[3] = GetPWCComboSelectedPin("combo5");

    int size;
    SpareParts.WPropCmd("spin7", PWA_SPINGETVALUE, NULL, &size);
    ChangeSize(size);
}

void cpart_pot_r::SpinChange(const char* controlname, int value) {
    for (int i = 0; i < 4; i++) {
        char name[20];
        sprintf(name, "combo%i", i + 2);
        SpareParts.WPropCmd(name, PWA_SETENABLE, std::to_string(i < value).c_str());
    }
}

void cpart_pot_r::ChangeSize(const unsigned int sz) {
    if (Size != sz) {
        if (BitmapId >= 0) {
            SpareParts.SetPartOnDraw(id);
            SpareParts.CanvasCmd({.cmd = CC_FREEBITMAP, .FreeBitmap{BitmapId}});
            BitmapId = -1;
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

void cpart_pot_r::LoadPartImage(void) {
    if (Size < 4) {
        xoff = (4 - Size) * 62;

        Width = OWidth - xoff;
        Height = OHeight;

        SpareParts.SetPartOnDraw(id);
        if (BitmapId >= 0) {
            SpareParts.CanvasCmd({.cmd = CC_FREEBITMAP, .FreeBitmap{BitmapId}});
        }
        BitmapId = SpareParts.CanvasCmd({.cmd = CC_CREATEIMAGE, .CreateImage{Width, Height, Scale, 0, Orientation}});

        SpareParts.CanvasCmd({.cmd = CC_DESTROY});
        SpareParts.CanvasCmd({.cmd = CC_CREATE, .Create{BitmapId}});

        int BackBitmap = SpareParts.CanvasCmd(
            {.cmd = CC_LOADIMAGE,
             .LoadImage{(PICSimLab.GetSharePath() + "parts/" + Type + "/" + GetPictureFileName()).c_str(), Scale, 0,
                        Orientation}});

        SpareParts.CanvasCmd({.cmd = CC_INIT, .Init{Scale, Scale, Orientation}});
        SpareParts.CanvasCmd({.cmd = CC_SETCOLOR, .SetColor{0x31, 0x3d, 0x63}});
        SpareParts.CanvasCmd({.cmd = CC_RECTANGLE, .Rectangle{1, 0, 0, (float)Width, (float)Height}});
        SpareParts.CanvasCmd({.cmd = CC_CHANGESCALE, .ChangeScale{1.0, 1.0}});
        SpareParts.CanvasCmd({.cmd = CC_PUTBITMAP, .PutBitmap{BackBitmap, -xoff * Scale, 0}});
        SpareParts.CanvasCmd({.cmd = CC_CHANGESCALE, .ChangeScale{Scale, Scale}});
        SpareParts.CanvasCmd({.cmd = CC_END});

        SpareParts.CanvasCmd({.cmd = CC_FREEBITMAP, .FreeBitmap{BackBitmap}});

    } else {
        Width = OWidth;
        Height = OHeight;
        xoff = 0;
        part::LoadPartImage();
    }
}

part_init(PART_POTR_Name, cpart_pot_r, "Input");
