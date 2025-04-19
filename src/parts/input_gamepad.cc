/* ########################################################################

   PICSimLab - Programmable IC Simulator Laboratory

   ########################################################################

   Copyright (c) : 2010-2025  Luis Claudio Gambôa Lopes <lcgamboa@yahoo.com>

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

#include "input_gamepad.h"
#include "../lib/oscilloscope.h"
#include "../lib/picsimlab.h"
#include "../lib/spareparts.h"
#include "math.h"

/* outputs */
enum { O_P1, O_P2, O_P3, O_P4, O_P5, O_P6, O_P7, O_P8, O_B1, O_B2, O_B3, O_B4, O_B5, O_B6, O_J1 };

/* inputs */
enum { I_B1, I_B2, I_B3, I_B4, I_B5, I_B6, I_J1 };

static PCWProp pcwprop[11] = {{PCW_COMBO, "1 - BTN A"},
                              {PCW_COMBO, "2 - BTN B"},
                              {PCW_COMBO, "3 - BTN C"},
                              {PCW_COMBO, "4 - BTN D"},
                              {PCW_COMBO, "5 - BTN E"},
                              {PCW_COMBO, "6 - BTN F"},
                              {PCW_COMBO, "7 - Axis X"},
                              {PCW_COMBO, "8 - Axis Y"},
                              {PCW_LABEL, "9 - VCC,+5V"},
                              {PCW_LABEL, "10 - GND ,GND"},
                              {PCW_END, ""}};

cpart_gamepad::cpart_gamepad(const unsigned x, const unsigned y, const char* name, const char* type, board* pboard_,
                             const int id_)
    : part(x, y, name, type, pboard_, id_) {
    output_pins[0] = 0;
    output_pins[1] = 0;
    output_pins[2] = 0;
    output_pins[3] = 0;
    output_pins[4] = 0;
    output_pins[5] = 0;
    output_pins[6] = 0;
    output_pins[7] = 0;

    output_value[0] = 1;
    output_value[1] = 1;
    output_value[2] = 1;
    output_value[3] = 1;
    output_value[4] = 1;
    output_value[5] = 1;

    active = 0;
    jr = 10;
    SetPCWProperties(pcwprop);

    PinCount = 8;
    Pins = output_pins;
}

void cpart_gamepad::RegisterRemoteControl(void) {
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
    input_ids[I_J1]->status = value;
    input_ids[I_J1]->update = &output_ids[O_J1]->update;
}

cpart_gamepad::~cpart_gamepad(void) {
    SpareParts.SetPartOnDraw(id);
    SpareParts.CanvasCmd({.cmd = CC_FREEBITMAP, .FreeBitmap{BitmapId}});
    SpareParts.CanvasCmd({.cmd = CC_DESTROY});
}

void cpart_gamepad::PostInit(void) {
    jr = ((output_ids[O_J1]->x2 - output_ids[O_J1]->x1) / 2) - 10;
    value[0] = jr;
    value[1] = jr;
}

void cpart_gamepad::DrawOutput(const unsigned int i) {
    switch (output[i].id) {
        case O_P1:
        case O_P2:
        case O_P3:
        case O_P4:
        case O_P5:
        case O_P6:
        case O_P7:
        case O_P8:
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
        case O_J1:
            SpareParts.CanvasCmd({.cmd = CC_SETCOLOR, .SetColor{26, 26, 26}});
            SpareParts.CanvasCmd(
                {.cmd = CC_CIRCLE, .Circle{1, output[i].x1 + jr + 10, output[i].y1 + jr + 10, (float)(jr + 10)}});
            SpareParts.CanvasCmd({.cmd = CC_SETCOLOR, .SetColor{51, 51, 51}});
            SpareParts.CanvasCmd(
                {.cmd = CC_CIRCLE, .Circle{1, output[i].x1 + jr + 10, output[i].y1 + jr + 10, (float)(jr - 4)}});
            SpareParts.CanvasCmd({.cmd = CC_SETCOLOR, .SetColor{250, 250, 250}});
            SpareParts.CanvasCmd(
                {.cmd = CC_CIRCLE, .Circle{1, output[i].x1 + value[0] + 10, output[i].y1 + value[1] + 10, 8}});
            break;
        case O_B5:
        case O_B6:
            SpareParts.CanvasCmd({.cmd = CC_SETCOLOR, .SetColor{102, 102, 102}});
            SpareParts.CanvasCmd({.cmd = CC_CIRCLE, .Circle{1, output[i].cx, output[i].cy, 10}});
            if (output_value[output[i].id - O_B1]) {
                SpareParts.CanvasCmd({.cmd = CC_SETCOLOR, .SetColor{15, 15, 15}});
            } else {
                SpareParts.CanvasCmd({.cmd = CC_SETCOLOR, .SetColor{55, 55, 55}});
            }
            SpareParts.CanvasCmd({.cmd = CC_CIRCLE, .Circle{1, output[i].cx, output[i].cy, 8}});
            break;
        case O_B1:
        case O_B3:
            if (output_value[output[i].id - O_B1]) {
                SpareParts.CanvasCmd({.cmd = CC_SETCOLOR, .SetColor{244, 244, 0}});
            } else {
                SpareParts.CanvasCmd({.cmd = CC_SETCOLOR, .SetColor{0x9c, 0x94, 0x47}});
            }
            SpareParts.CanvasCmd({.cmd = CC_CIRCLE, .Circle{1, output[i].cx, output[i].cy, 20}});
            break;
        case O_B2:
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

void cpart_gamepad::PreProcess(void) {
    SpareParts.SetPin(output_pins[0], output_value[0]);
    SpareParts.SetPin(output_pins[1], output_value[1]);
    SpareParts.SetPin(output_pins[2], output_value[2]);
    SpareParts.SetPin(output_pins[3], output_value[3]);
    SpareParts.SetPin(output_pins[4], output_value[4]);
    SpareParts.SetPin(output_pins[5], output_value[5]);
    SpareParts.SetAPin(output_pins[6], 2.5 * (value[0]) / jr);
    SpareParts.SetAPin(output_pins[7], 2.5 * (value[1]) / jr);
}

void cpart_gamepad::OnMouseButtonPress(unsigned int inputId, unsigned int button, unsigned int x, unsigned int y,
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
        case I_B6:
            output_value[5] = 0;
            output_ids[O_B6]->update = 1;
            break;
        case I_J1:
            RotateCoords(&x, &y);

            float cx = ((float)fabs(x - input_ids[I_J1]->x1)) - jr;
            float cy = ((float)fabs(y - input_ids[I_J1]->y1)) - jr;

            float module = sqrt(cx * cx + cy * cy);
            float angle = atan2((cy), (cx));

            if (module > jr)
                module = jr;
            value[0] = module * cos(angle) + jr;
            value[1] = module * sin(angle) + jr;

            active = 1;
            output_ids[O_J1]->update = 1;
            break;
    }
}

void cpart_gamepad::OnMouseButtonRelease(unsigned int inputId, unsigned int button, unsigned int x, unsigned int y,
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
        case I_B6:
            output_value[5] = 1;
            output_ids[O_B6]->update = 1;
            break;
        case I_J1:
            active = 0;
            value[0] = jr;
            value[1] = jr;
            output_ids[O_J1]->update = 1;
            break;
    }
}

void cpart_gamepad::OnMouseMove(unsigned int inputId, unsigned int button, unsigned int x, unsigned int y,
                                unsigned int state) {
    switch (inputId) {
        case I_J1:
            if (active) {
                RotateCoords(&x, &y);

                float cx = ((float)fabs(x - input_ids[I_J1]->x1)) - jr;
                float cy = ((float)fabs(y - input_ids[I_J1]->y1)) - jr;

                float module = sqrt(cx * cx + cy * cy);
                float angle = atan2((cy), (cx));

                if (module > jr)
                    module = jr;
                value[0] = module * cos(angle) + jr;
                value[1] = module * sin(angle) + jr;
                output_ids[O_J1]->update = 1;
            }
            break;
        default:
            active = 0;
            break;
    }
}

void cpart_gamepad::EvKeyPress(unsigned int key, unsigned int mask) {
    switch (key) {
        case 'W':
        case 'w':
            value[0] = jr;
            value[1] = 0;
            if (!output_ids[O_J1]->value) {
                output_ids[O_J1]->value = 1;
                output_ids[O_J1]->update = 1;
            }
            break;
        case 'A':
        case 'a':
            value[0] = 0;
            value[1] = jr;
            if (!output_ids[O_J1]->value) {
                output_ids[O_J1]->value = 1;
                output_ids[O_J1]->update = 1;
            }
            break;
        case 'S':
        case 's':
            value[0] = jr;
            value[1] = 2 * jr;
            if (!output_ids[O_J1]->value) {
                output_ids[O_J1]->value = 1;
                output_ids[O_J1]->update = 1;
            }
            break;
        case 'D':
        case 'd':
            value[0] = 2 * jr;
            value[1] = jr;
            if (!output_ids[O_J1]->value) {
                output_ids[O_J1]->value = 1;
                output_ids[O_J1]->update = 1;
            }
            break;
        case 'I':
        case 'i':
            output_value[0] = 0;
            if (!output_ids[O_B1]->value) {
                output_ids[O_B1]->value = 1;
                output_ids[O_B1]->update = 1;
            }
            break;
        case 'J':
        case 'j':
            output_value[3] = 0;
            if (!output_ids[O_B4]->value) {
                output_ids[O_B4]->value = 1;
                output_ids[O_B4]->update = 1;
            }
            break;
        case 'K':
        case 'k':
            output_value[2] = 0;
            if (!output_ids[O_B3]->value) {
                output_ids[O_B3]->value = 1;
                output_ids[O_B3]->update = 1;
            }
            break;
        case 'L':
        case 'l':
            output_value[1] = 0;
            if (!output_ids[O_B2]->value) {
                output_ids[O_B2]->value = 1;
                output_ids[O_B2]->update = 1;
            }
            break;
        case 'E':
        case 'e':
            output_value[4] = 0;
            if (!output_ids[O_B5]->value) {
                output_ids[O_B5]->value = 1;
                output_ids[O_B5]->update = 1;
            }
            break;
        case 'O':
        case 'o':
            output_value[5] = 0;
            if (!output_ids[O_B6]->value) {
                output_ids[O_B6]->value = 1;
                output_ids[O_B6]->update = 1;
            }
            break;
    }
}

void cpart_gamepad::EvKeyRelease(unsigned int key, unsigned int mask) {
    switch (key) {
        case 'W':
        case 'A':
        case 'S':
        case 'D':
        case 'w':
        case 'a':
        case 's':
        case 'd':
            value[0] = jr;
            value[1] = jr;
            output_ids[O_J1]->update = 1;
            output_ids[O_J1]->value = 0;
            break;
        case 'I':
        case 'i':
            output_value[0] = 1;
            output_ids[O_B1]->update = 1;
            output_ids[O_B1]->value = 0;
            break;
        case 'J':
        case 'j':
            output_value[3] = 1;
            output_ids[O_B4]->update = 1;
            output_ids[O_B4]->value = 0;
            break;
        case 'K':
        case 'k':
            output_value[2] = 1;
            output_ids[O_B3]->update = 1;
            output_ids[O_B3]->value = 0;
            break;
        case 'L':
        case 'l':
            output_value[1] = 1;
            output_ids[O_B2]->update = 1;
            output_ids[O_B2]->value = 0;
            break;
        case 'E':
        case 'e':
            output_value[4] = 1;
            output_ids[O_B5]->update = 1;
            output_ids[O_B5]->value = 0;
            break;
        case 'O':
        case 'o':
            output_value[5] = 1;
            output_ids[O_B6]->update = 1;
            output_ids[O_B6]->value = 0;
            break;
    }
}

unsigned short cpart_gamepad::GetInputId(char* name) {
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

    if (strcmp(name, "AJ_1") == 0)
        return I_J1;

    printf("Error input '%s' don't have a valid id! \n", name);
    return INVALID_ID;
};

unsigned short cpart_gamepad::GetOutputId(char* name) {
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

    if (strcmp(name, "AJ_1") == 0)
        return O_J1;

    printf("Error output '%s' don't have a valid id! \n", name);
    return INVALID_ID;
};

std::string cpart_gamepad::WritePreferences(void) {
    char prefs[256];

    sprintf(prefs, "%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu", output_pins[0], output_pins[1], output_pins[2],
            output_pins[3], output_pins[4], output_pins[5], output_pins[6], output_pins[7]);

    return prefs;
};

void cpart_gamepad::ReadPreferences(std::string value) {
    sscanf(value.c_str(), "%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu", &output_pins[0], &output_pins[1], &output_pins[2],
           &output_pins[3], &output_pins[4], &output_pins[5], &output_pins[6], &output_pins[7]);
}

void cpart_gamepad::ConfigurePropertiesWindow(void) {
    SetPCWComboWithPinNames("combo1", output_pins[0]);
    SetPCWComboWithPinNames("combo2", output_pins[1]);
    SetPCWComboWithPinNames("combo3", output_pins[2]);
    SetPCWComboWithPinNames("combo4", output_pins[3]);
    SetPCWComboWithPinNames("combo5", output_pins[4]);
    SetPCWComboWithPinNames("combo6", output_pins[5]);
    SetPCWComboWithPinNames("combo7", output_pins[6]);
    SetPCWComboWithPinNames("combo8", output_pins[7]);
}

void cpart_gamepad::ReadPropertiesWindow(void) {
    output_pins[0] = GetPWCComboSelectedPin("combo1");
    output_pins[1] = GetPWCComboSelectedPin("combo2");
    output_pins[2] = GetPWCComboSelectedPin("combo3");
    output_pins[3] = GetPWCComboSelectedPin("combo4");
    output_pins[4] = GetPWCComboSelectedPin("combo5");
    output_pins[5] = GetPWCComboSelectedPin("combo6");
    output_pins[6] = GetPWCComboSelectedPin("combo7");
    output_pins[7] = GetPWCComboSelectedPin("combo8");
}

part_init(PART_GAMEPAD_Name, cpart_gamepad, "Input");
