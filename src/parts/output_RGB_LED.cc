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

#include "output_RGB_LED.h"
#include "../lib/oscilloscope.h"
#include "../lib/picsimlab.h"
#include "../lib/spareparts.h"

/* outputs */
enum { O_P1, O_P2, O_P3, O_L1 };

static PCWProp pcwprop[6] = {{PCW_COMBO, "1-Red"},     {PCW_COMBO, "2-Green"}, {PCW_COMBO, "3-Blue"},
                             {PCW_LABEL, "4-GND,GND"}, {PCW_COMBO, "Active"},  {PCW_END, ""}};

cpart_rgb_led::cpart_rgb_led(const unsigned x, const unsigned y, const char* name, const char* type, board* pboard_,
                             const int id_)
    : part(x, y, name, type, pboard_, id_) {
    X = x;
    Y = y;
    active = 1;

    ReadMaps();

    LoadPartImage();

    input_pins[0] = 0;
    input_pins[1] = 0;
    input_pins[2] = 0;

    color[0] = 0;
    color[1] = 0;
    color[2] = 0;

    SetPCWProperties(pcwprop);

    PinCount = 3;
    Pins = input_pins;
}

cpart_rgb_led::~cpart_rgb_led(void) {
    SpareParts.SetPartOnDraw(id);
    SpareParts.CanvasCmd({.cmd = CC_FREEBITMAP, .FreeBitmap{BitmapId}});
    SpareParts.CanvasCmd({.cmd = CC_DESTROY});
}

void cpart_rgb_led::DrawOutput(const unsigned int i) {
    const picpin* ppins = SpareParts.GetPinsValues();

    switch (output[i].id) {
        case O_P1:
        case O_P2:
        case O_P3:
            SpareParts.CanvasCmd({.cmd = CC_SETCOLOR, .SetColor{49, 61, 99}});
            SpareParts.CanvasCmd(
                {.cmd = CC_RECTANGLE,
                 .Rectangle{1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1}});
            SpareParts.CanvasCmd({.cmd = CC_SETFGCOLOR, .SetFgColor{255, 255, 255}});
            if (input_pins[output[i].id - O_P1] == 0)
                SpareParts.CanvasCmd({.cmd = CC_ROTATEDTEXT, .RotatedText{"NC", output[i].x1, output[i].y1, 0}});
            else
                SpareParts.CanvasCmd({.cmd = CC_ROTATEDTEXT,
                                      .RotatedText{SpareParts.GetPinName(input_pins[output[i].id - O_P1]).c_str(),
                                                   output[i].x1, output[i].y1, 0}});
            break;
        case O_L1:
            if (input_pins[0] > 0)
                color[0] = ppins[input_pins[0] - 1].oavalue;
            if (input_pins[1] > 0)
                color[1] = ppins[input_pins[1] - 1].oavalue;
            if (input_pins[2] > 0)
                color[2] = ppins[input_pins[2] - 1].oavalue;

            if (!active) {
                color[0] = 285 - color[0];
                color[1] = 285 - color[1];
                color[2] = 285 - color[2];
            }

            SpareParts.CanvasCmd({.cmd = CC_SETCOLOR, .SetColor{255, 255, 224}});
            SpareParts.CanvasCmd({.cmd = CC_CIRCLE, .Circle{1, output[i].x1, output[i].y1, output[i].r - 5}});
            SpareParts.CanvasCmd({.cmd = CC_SETFGCOLOR, .SetFgColor{0, 0, 0}});
            SpareParts.CanvasCmd({.cmd = CC_SETBGCOLOR, .SetBgColor{color[0], color[1], color[2]}});
            SpareParts.CanvasCmd({.cmd = CC_CIRCLE, .Circle{1, output[i].x1 - 0.5f, output[i].y1, output[i].r - 7.0f}});
            break;
    }
}

void cpart_rgb_led::PostProcess(void) {
    const picpin* ppins = SpareParts.GetPinsValues();

    unsigned int color = 0;

    if (input_pins[0])
        color |= ((unsigned char)ppins[input_pins[0] - 1].oavalue);
    if (input_pins[1])
        color |= (((unsigned char)ppins[input_pins[1] - 1].oavalue) << 8);
    if (input_pins[2])
        color |= (((unsigned char)ppins[input_pins[2] - 1].oavalue) << 16);

    if (output_ids[O_L1]->value_f != color) {
        output_ids[O_L1]->value_f = color;
        output_ids[O_L1]->update = 1;
    }
}

unsigned short cpart_rgb_led::GetInputId(char* name) {
    printf("Error input '%s' don't have a valid id! \n", name);
    return INVALID_ID;
}

unsigned short cpart_rgb_led::GetOutputId(char* name) {
    if (strcmp(name, "PN_1") == 0)
        return O_P1;
    if (strcmp(name, "PN_2") == 0)
        return O_P2;
    if (strcmp(name, "PN_3") == 0)
        return O_P3;

    if (strcmp(name, "LR_1") == 0)
        return O_L1;

    printf("Error output '%s' don't have a valid id! \n", name);
    return INVALID_ID;
}

std::string cpart_rgb_led::WritePreferences(void) {
    char prefs[256];

    sprintf(prefs, "%hhu,%hhu,%hhu,%hhu", input_pins[0], input_pins[1], input_pins[2], active);

    return prefs;
}

void cpart_rgb_led::ReadPreferences(std::string value) {
    sscanf(value.c_str(), "%hhu,%hhu,%hhu,%hhu", &input_pins[0], &input_pins[1], &input_pins[2], &active);
}

void cpart_rgb_led::RegisterRemoteControl(void) {
    output_ids[O_L1]->status = (void*)color;
}

void cpart_rgb_led::ConfigurePropertiesWindow(void) {
    SetPCWComboWithPinNames("combo1", input_pins[0]);
    SetPCWComboWithPinNames("combo2", input_pins[1]);
    SetPCWComboWithPinNames("combo3", input_pins[2]);

    SpareParts.WPropCmd("combo5", PWA_COMBOSETITEMS, "HIGH,LOW,");
    if (active)
        SpareParts.WPropCmd("combo5", PWA_COMBOSETTEXT, "HIGH");
    else
        SpareParts.WPropCmd("combo5", PWA_COMBOSETTEXT, "LOW ");
}

void cpart_rgb_led::ReadPropertiesWindow(void) {
    input_pins[0] = GetPWCComboSelectedPin("combo1");
    input_pins[1] = GetPWCComboSelectedPin("combo2");
    input_pins[2] = GetPWCComboSelectedPin("combo3");

    char buff[64];
    SpareParts.WPropCmd("combo5", PWA_COMBOGETTEXT, NULL, buff);

    active = (strcmp(buff, "HIGH") == 0);
}

part_init(PART_RGB_LED_Name, cpart_rgb_led, "Output");
