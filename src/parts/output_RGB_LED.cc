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

#include "output_RGB_LED.h"
#include "../lib/oscilloscope.h"
#include "../lib/picsimlab.h"
#include "../lib/spareparts.h"

/* outputs */
enum { O_P1, O_P2, O_P3, O_L1 };

static PCWProp pcwprop[5] = {{PCW_COMBO, "1-Red"},
                             {PCW_COMBO, "2-Green"},
                             {PCW_COMBO, "3-Blue"},
                             {PCW_LABEL, "4-GND,GND"},
                             {PCW_COMBO, "Active"}};

cpart_rgb_led::cpart_rgb_led(const unsigned x, const unsigned y, const char* name, const char* type)
    : part(x, y, name, type), font(9, lxFONTFAMILY_TELETYPE, lxFONTSTYLE_NORMAL, lxFONTWEIGHT_BOLD) {
    X = x;
    Y = y;
    active = 1;

    ReadMaps();

    LoadImage();

    input_pins[0] = 0;
    input_pins[1] = 0;
    input_pins[2] = 0;

    color[0] = 0;
    color[1] = 0;
    color[2] = 0;

    SetPCWProperties(pcwprop, 5);

    PinCount = 3;
    Pins = input_pins;
}

cpart_rgb_led::~cpart_rgb_led(void) {
    delete Bitmap;
    canvas.Destroy();
}

void cpart_rgb_led::DrawOutput(const unsigned int i) {
    const picpin* ppins = SpareParts.GetPinsValues();

    switch (output[i].id) {
        case O_P1:
        case O_P2:
        case O_P3:
            canvas.SetColor(49, 61, 99);
            canvas.Rectangle(1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);
            canvas.SetFgColor(255, 255, 255);
            if (input_pins[output[i].id - O_P1] == 0)
                canvas.RotatedText("NC", output[i].x1, output[i].y1, 0);
            else
                canvas.RotatedText(SpareParts.GetPinName(input_pins[output[i].id - O_P1]), output[i].x1, output[i].y1,
                                   0);
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

            canvas.SetColor(255, 255, 224);
            canvas.Circle(1, output[i].x1, output[i].y1, output[i].r - 5);
            canvas.SetFgColor(0, 0, 0);
            canvas.SetBgColor(color[0], color[1], color[2]);
            canvas.Circle(1, output[i].x1 - 0.5, output[i].y1, output[i].r - 7);
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
    printf("Erro input '%s' don't have a valid id! \n", name);
    return -1;
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

    printf("Erro output '%s' don't have a valid id! \n", name);
    return 1;
}

lxString cpart_rgb_led::WritePreferences(void) {
    char prefs[256];

    sprintf(prefs, "%hhu,%hhu,%hhu,%hhu", input_pins[0], input_pins[1], input_pins[2], active);

    return prefs;
}

void cpart_rgb_led::ReadPreferences(lxString value) {
    sscanf(value.c_str(), "%hhu,%hhu,%hhu,%hhu", &input_pins[0], &input_pins[1], &input_pins[2], &active);
}

void cpart_rgb_led::RegisterRemoteControl(void) {
    output_ids[O_L1]->status = (void*)color;
}

void cpart_rgb_led::ConfigurePropertiesWindow(CPWindow* WProp) {
    SetPCWComboWithPinNames(WProp, "combo1", input_pins[0]);
    SetPCWComboWithPinNames(WProp, "combo2", input_pins[1]);
    SetPCWComboWithPinNames(WProp, "combo3", input_pins[2]);

    ((CCombo*)WProp->GetChildByName("combo5"))->SetItems("HIGH,LOW,");
    if (active)
        ((CCombo*)WProp->GetChildByName("combo5"))->SetText("HIGH");
    else
        ((CCombo*)WProp->GetChildByName("combo5"))->SetText("LOW ");
}

void cpart_rgb_led::ReadPropertiesWindow(CPWindow* WProp) {
    input_pins[0] = GetPWCComboSelectedPin(WProp, "combo1");
    input_pins[1] = GetPWCComboSelectedPin(WProp, "combo2");
    input_pins[2] = GetPWCComboSelectedPin(WProp, "combo3");

    active = (((CCombo*)WProp->GetChildByName("combo5"))->GetText().compare("HIGH") == 0);
}

part_init(PART_RGB_LED_Name, cpart_rgb_led, "Output");
