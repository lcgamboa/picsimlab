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

#include "input_gamepad_an.h"
#include "../lib/oscilloscope.h"
#include "../lib/picsimlab.h"
#include "../lib/spareparts.h"

/* outputs */
enum { O_P1, O_B1, O_B2, O_B3, O_B4, O_B5 };

/* inputs */
enum { I_B1, I_B2, I_B3, I_B4, I_B5 };

static PCWProp pcwprop[5] = {{PCW_COMBO, "Pin 1"},
                             {PCW_LABEL, "Pin2 - VCC,+5V"},
                             {PCW_LABEL, "Pin3 - GND,GND"},
                             {PCW_COMBO, "Open"},
                             {PCW_END, ""}};

cpart_gamepad_an::cpart_gamepad_an(const unsigned x, const unsigned y, const char* name, const char* type,
                                   board* pboard_, const int id_)
    : part(x, y, name, type, pboard_, id_) {
    active = 1;

    output_value[0] = 1;
    output_value[1] = 1;
    output_value[2] = 1;
    output_value[3] = 1;
    output_value[4] = 1;

    output_pins[0] = 0;

    vmax = 5.0;

    SetPCWProperties(pcwprop);

    PinCount = 1;
    Pins = output_pins;
}

void cpart_gamepad_an::RegisterRemoteControl(void) {
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
}

void cpart_gamepad_an::Reset(void) {
    // release all
    vmax = pboard->MGetVCC();

    output_value_an = active * vmax;

    output_value[0] = 1;
    output_value[1] = 1;
    output_value[2] = 1;
    output_value[3] = 1;
    output_value[4] = 1;

    SpareParts.SetAPin(output_pins[0], output_value_an);
}

cpart_gamepad_an::~cpart_gamepad_an(void) {
    SpareParts.SetPartOnDraw(id);
    SpareParts.CanvasCmd({.cmd = CC_FREEBITMAP, .FreeBitmap{BitmapId}});
    SpareParts.CanvasCmd({.cmd = CC_DESTROY});
}

void cpart_gamepad_an::DrawOutput(const unsigned int i) {
    std::string temp;

    switch (output[i].id) {
        case O_P1:
            SpareParts.CanvasCmd({.cmd = CC_SETCOLOR, .SetColor{49, 61, 99}});
            SpareParts.CanvasCmd({.cmd = CC_RECTANGLE,
                                  .Rectangle{1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1 + 10,
                                             output[i].y2 - output[i].y1 + 20}});
            SpareParts.CanvasCmd({.cmd = CC_SETFGCOLOR, .SetFgColor{155, 155, 155}});

            temp = FloatStrFormat("%3.1f", output_value_an) + "V";
            SpareParts.CanvasCmd(
                {.cmd = CC_ROTATEDTEXT, .RotatedText{("Out " + temp).c_str(), output[i].x1, output[i].y1 + 20, 0}});
            SpareParts.CanvasCmd({.cmd = CC_SETFGCOLOR, .SetFgColor{255, 255, 255}});
            if (output_pins[0] == 0)
                SpareParts.CanvasCmd({.cmd = CC_ROTATEDTEXT, .RotatedText{"NC", output[i].x1, output[i].y1, 0}});
            else
                SpareParts.CanvasCmd(
                    {.cmd = CC_ROTATEDTEXT,
                     .RotatedText{SpareParts.GetPinName(output_pins[0]).c_str(), output[i].x1, output[i].y1, 0}});
            break;
        case O_B5:
            SpareParts.CanvasCmd({.cmd = CC_SETCOLOR, .SetColor{102, 102, 102}});
            SpareParts.CanvasCmd({.cmd = CC_CIRCLE, .Circle{1, output[i].cx, output[i].cy, 10}});
            if (output_value[output[i].id - O_B1]) {
                SpareParts.CanvasCmd({.cmd = CC_SETCOLOR, .SetColor{15, 15, 15}});
            } else {
                SpareParts.CanvasCmd({.cmd = CC_SETCOLOR, .SetColor{55, 55, 55}});
            }
            SpareParts.CanvasCmd({.cmd = CC_CIRCLE, .Circle{1, output[i].cx, output[i].cy, 8}});
            break;
        case O_B2:
        case O_B3:
            if (output_value[output[i].id - O_B1]) {
                SpareParts.CanvasCmd({.cmd = CC_SETCOLOR, .SetColor{244, 244, 0}});
            } else {
                SpareParts.CanvasCmd({.cmd = CC_SETCOLOR, .SetColor{0x9c, 0x94, 0x47}});
            }
            SpareParts.CanvasCmd({.cmd = CC_CIRCLE, .Circle{1, output[i].cx, output[i].cy, 20}});
            break;
        case O_B1:
        case O_B4:
            if (output_value[output[i].id - O_B1]) {
                SpareParts.CanvasCmd({.cmd = CC_SETCOLOR, .SetColor{0, 0, 214}});
            } else {
                SpareParts.CanvasCmd({.cmd = CC_SETCOLOR, .SetColor{64, 87, 106}});
            }
            SpareParts.CanvasCmd({.cmd = CC_CIRCLE, .Circle{1, output[i].cx, output[i].cy, 20}});
            break;
    }
}

void cpart_gamepad_an::PreProcess(void) {
    output_value_an = vmax * active;

    if (!output_value[0]) {
        output_value_an = (active * 0) + ((!active) * 5);
    } else if (!output_value[1]) {
        output_value_an = (active * 0.708) + ((!active) * 4.292);
    } else if (!output_value[2]) {
        output_value_an = (active * 1.61) + ((!active) * 3.39);
    } else if (!output_value[3]) {
        output_value_an = (active * 2.468) + ((!active) * 2.532);
    } else if (!output_value[4]) {
        output_value_an = (active * 3.621) + ((!active) * 1.379);
    }

    SpareParts.SetAPin(output_pins[0], output_value_an);

    if (output_value_an != output_value_an_) {
        output_ids[O_P1]->update = 1;
    }
    output_value_an_ = output_value_an;
}

void cpart_gamepad_an::OnMouseButtonPress(unsigned int inputId, unsigned int button, unsigned int x, unsigned int y,
                                          unsigned int state) {
    switch (inputId) {
        case I_B1:
            output_value[0] = 0;
            output_ids[O_B1]->update = 1;
            break;
        case I_B2:
            output_value[1] = 0;
            output_ids[O_B2]->update = 1;
            break;
        case I_B3:
            output_value[2] = 0;
            output_ids[O_B3]->update = 1;
            break;
        case I_B4:
            output_value[3] = 0;
            output_ids[O_B4]->update = 1;
            break;
        case I_B5:
            output_value[4] = 0;
            output_ids[O_B5]->update = 1;
            break;
    }
}

void cpart_gamepad_an::OnMouseButtonRelease(unsigned int inputId, unsigned int button, unsigned int x, unsigned int y,
                                            unsigned int state) {
    switch (inputId) {
        case I_B1:
            output_value[0] = 1;
            output_ids[O_B1]->update = 1;
            break;
        case I_B2:
            output_value[1] = 1;
            output_ids[O_B2]->update = 1;
            break;
        case I_B3:
            output_value[2] = 1;
            output_ids[O_B3]->update = 1;
            break;
        case I_B4:
            output_value[3] = 1;
            output_ids[O_B4]->update = 1;
            break;
        case I_B5:
            output_value[4] = 1;
            output_ids[O_B5]->update = 1;
            break;
    }
}

void cpart_gamepad_an::EvKeyPress(unsigned int key, unsigned int mask) {
    switch (key) {
        case 'I':
        case 'i':
            output_value[1] = 0;
            output_ids[O_B2]->update = 1;
            break;
        case 'J':
        case 'j':
            output_value[3] = 0;
            output_ids[O_B4]->update = 1;
            break;
        case 'K':
        case 'k':
            output_value[2] = 0;
            output_ids[O_B3]->update = 1;
            break;
        case 'L':
        case 'l':
            output_value[0] = 0;
            output_ids[O_B1]->update = 1;
            break;
        case 'O':
        case 'o':
            output_value[4] = 0;
            output_ids[O_B5]->update = 1;
            break;
    }
}

void cpart_gamepad_an::EvKeyRelease(unsigned int key, unsigned int mask) {
    switch (key) {
        case 'I':
        case 'i':
            output_value[1] = 1;
            output_ids[O_B2]->update = 1;
            break;
        case 'J':
        case 'j':
            output_value[3] = 1;
            output_ids[O_B4]->update = 1;
            break;
        case 'K':
        case 'k':
            output_value[2] = 1;
            output_ids[O_B3]->update = 1;
            break;
        case 'L':
        case 'l':
            output_value[0] = 1;
            output_ids[O_B1]->update = 1;
            break;
        case 'O':
        case 'o':
            output_value[4] = 1;
            output_ids[O_B5]->update = 1;
            break;
    }
}

unsigned short cpart_gamepad_an::GetInputId(char* name) {
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

    printf("Error input '%s' don't have a valid id! \n", name);
    return INVALID_ID;
}

unsigned short cpart_gamepad_an::GetOutputId(char* name) {
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

    if (strcmp(name, "PN_1") == 0)
        return O_P1;

    printf("Error output '%s' don't have a valid id! \n", name);
    return INVALID_ID;
}

std::string cpart_gamepad_an::WritePreferences(void) {
    char prefs[256];

    sprintf(prefs, "%hhu,%hhu", output_pins[0], active);

    return prefs;
}

void cpart_gamepad_an::ReadPreferences(std::string value) {
    sscanf(value.c_str(), "%hhu,%hhu", &output_pins[0], &active);
    output_value_an = active * vmax;
}

void cpart_gamepad_an::ConfigurePropertiesWindow(void) {
    SetPCWComboWithPinNames("combo1", output_pins[0]);

    SpareParts.WPropCmd("combo4", PWA_COMBOSETITEMS, "HIGH,LOW,");
    if (active)
        SpareParts.WPropCmd("combo4", PWA_COMBOSETTEXT, "HIGH");
    else
        SpareParts.WPropCmd("combo4", PWA_COMBOSETTEXT, "LOW");
}

void cpart_gamepad_an::ReadPropertiesWindow(void) {
    output_pins[0] = GetPWCComboSelectedPin("combo1");

    char buff[64];
    SpareParts.WPropCmd("combo4", PWA_COMBOGETTEXT, NULL, buff);
    active = (strcmp(buff, "HIGH") == 0);
}

part_init(PART_GAMEPAD_AN_Name, cpart_gamepad_an, "Input");
