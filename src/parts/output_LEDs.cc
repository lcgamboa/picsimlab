/* ########################################################################

   PICsimLab - PIC laboratory simulator

   ########################################################################

   Copyright (c) : 2010-2022  Luis Claudio Gambôa Lopes

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

#include "output_LEDs.h"
#include "../picsimlab1.h"
#include "../picsimlab4.h"
#include "../spareparts.h"

/* outputs */
enum { O_P1, O_P2, O_P3, O_P4, O_P5, O_P6, O_P7, O_P8, O_L1, O_L2, O_L3, O_L4, O_L5, O_L6, O_L7, O_L8 };

enum { C_RED = 0, C_GREEN, C_BLUE, C_YELLOW, C_WHITE, C_END };

static const char Colorname[C_END][7] = {"Red", "Green", "Blue", "Yellow", "White"};

typedef struct {
    unsigned char r, g, b;
} colorval_t;

static const colorval_t colortable[C_END] = {{1, 0, 0}, {0, 1, 0}, {0, 0, 1}, {1, 1, 0}, {1, 1, 1}};

static PCWProp pcwprop[9] = {{PCW_DCOMBO, "1-D1"}, {PCW_DCOMBO, "2-D2"}, {PCW_DCOMBO, "3-D3"},
                             {PCW_DCOMBO, "4-D4"}, {PCW_DCOMBO, "5-D5"}, {PCW_DCOMBO, "6-D6"},
                             {PCW_DCOMBO, "7-D7"}, {PCW_DCOMBO, "8-D8"}, {PCW_COMBO, "Active"}};

cpart_leds::cpart_leds(const unsigned x, const unsigned y, const char* name, const char* type)
    : part(x, y, name, type, 9) {
    X = x;
    Y = y;
    active = 1;

    ReadMaps();

    LoadImage();

    input_pins[0] = 0;
    input_pins[1] = 0;
    input_pins[2] = 0;
    input_pins[3] = 0;
    input_pins[4] = 0;
    input_pins[5] = 0;
    input_pins[6] = 0;
    input_pins[7] = 0;

    colors[0] = 0;
    colors[1] = 0;
    colors[2] = 0;
    colors[3] = 0;
    colors[4] = 0;
    colors[5] = 0;
    colors[6] = 0;
    colors[7] = 0;

    SetPCWProperties(pcwprop, 9);
};

cpart_leds::~cpart_leds(void) {
    delete Bitmap;
    canvas.Destroy();
}

void cpart_leds::DrawOutput(const unsigned int i) {
    colorval_t col;

    const picpin* ppins = SpareParts.GetPinsValues();

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
            if (input_pins[output[i].id - O_P1] == 0)
                canvas.RotatedText("NC", output[i].x1, output[i].y1, 0);
            else
                canvas.RotatedText(SpareParts.GetPinName(input_pins[output[i].id - O_P1]), output[i].x1, output[i].y1,
                                   0);
            break;
        case O_L1:
        case O_L2:
        case O_L3:
        case O_L4:
        case O_L5:
        case O_L6:
        case O_L7:
        case O_L8:
            col = colortable[colors[output[i].id - O_L1]];
            if (input_pins[output[i].id - O_L1] > 0) {
                if (active) {
                    canvas.SetBgColor(col.r * ppins[input_pins[output[i].id - O_L1] - 1].oavalue,
                                      col.g * ppins[input_pins[output[i].id - O_L1] - 1].oavalue,
                                      col.b * ppins[input_pins[output[i].id - O_L1] - 1].oavalue);
                } else {
                    canvas.SetBgColor(col.r * (310 - ppins[input_pins[output[i].id - O_L1] - 1].oavalue),
                                      col.g * (310 - ppins[input_pins[output[i].id - O_L1] - 1].oavalue),
                                      col.b * (310 - ppins[input_pins[output[i].id - O_L1] - 1].oavalue));
                }
            } else {
                canvas.SetBgColor(col.r * 55, col.g * 55, col.b * 55);
            }
            canvas.SetFgColor(0, 0, 0);
            // draw a circle
            color1 = canvas.GetBgColor();
            int r = color1.Red() - 120;
            int g = color1.Green() - 120;
            int b = color1.Blue() - 120;
            if (r < 0)
                r = 0;
            if (g < 0)
                g = 0;
            if (b < 0)
                b = 0;
            color2.Set(r, g, b);
            canvas.SetBgColor(color2);
            canvas.Circle(1, output[i].x1, output[i].y1, output[i].r);
            canvas.SetBgColor(color1);
            canvas.Circle(1, output[i].x1, output[i].y1, output[i].r - 3);
            break;
    }
}

void cpart_leds::PostProcess(void) {
    const picpin* ppins = SpareParts.GetPinsValues();

    for (int i = 0; i < 8; i++) {
        if (input_pins[i] && (output_ids[O_L1 + i]->value != ppins[input_pins[i] - 1].oavalue)) {
            output_ids[O_L1 + i]->value = ppins[input_pins[i] - 1].oavalue;
            output_ids[O_L1 + i]->update = 1;
        }
    }
}

unsigned short cpart_leds::GetInputId(char* name) {
    printf("Erro input '%s' don't have a valid id! \n", name);
    return -1;
};

unsigned short cpart_leds::GetOutputId(char* name) {
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

    if (strcmp(name, "LD_1") == 0)
        return O_L1;
    if (strcmp(name, "LD_2") == 0)
        return O_L2;
    if (strcmp(name, "LD_3") == 0)
        return O_L3;
    if (strcmp(name, "LD_4") == 0)
        return O_L4;
    if (strcmp(name, "LD_5") == 0)
        return O_L5;
    if (strcmp(name, "LD_6") == 0)
        return O_L6;
    if (strcmp(name, "LD_7") == 0)
        return O_L7;
    if (strcmp(name, "LD_8") == 0)
        return O_L8;

    printf("Erro output '%s' don't have a valid id! \n", name);
    return 1;
};

lxString cpart_leds::WritePreferences(void) {
    char prefs[256];

    sprintf(prefs, "%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu",
            input_pins[0], input_pins[1], input_pins[2], input_pins[3], input_pins[4], input_pins[5], input_pins[6],
            input_pins[7], active, colors[0], colors[1], colors[2], colors[3], colors[4], colors[5], colors[6],
            colors[7]);

    return prefs;
}

void cpart_leds::ReadPreferences(lxString value) {
    sscanf(value.c_str(), "%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu",
           &input_pins[0], &input_pins[1], &input_pins[2], &input_pins[3], &input_pins[4], &input_pins[5],
           &input_pins[6], &input_pins[7], &active, &colors[0], &colors[1], &colors[2], &colors[3], &colors[4],
           &colors[5], &colors[6], &colors[7]);
}

void cpart_leds::RegisterRemoteControl(void) {
    const picpin* ppins = SpareParts.GetPinsValues();

    for (int i = 0; i > 8; i++) {
        if (input_pins[i]) {
            output_ids[O_L1 + i]->status = (void*)&ppins[input_pins[i] - 1].oavalue;
        }
    }
}

void cpart_leds::ConfigurePropertiesWindow(CPWindow* WProp) {
    lxString Colors = "";

    for (int i = 0; i < C_END; i++) {
        Colors += Colorname[i];
        Colors += ",";
    }

    SetPCWComboWithPinNames(WProp, "combo1", input_pins[0]);
    SetPCWComboWithPinNames(WProp, "combo2", input_pins[1]);
    SetPCWComboWithPinNames(WProp, "combo3", input_pins[2]);
    SetPCWComboWithPinNames(WProp, "combo4", input_pins[3]);
    SetPCWComboWithPinNames(WProp, "combo5", input_pins[4]);
    SetPCWComboWithPinNames(WProp, "combo6", input_pins[5]);
    SetPCWComboWithPinNames(WProp, "combo7", input_pins[6]);
    SetPCWComboWithPinNames(WProp, "combo8", input_pins[7]);

    ((CCombo*)WProp->GetChildByName("combo9"))->SetItems("HIGH,LOW,");
    if (active)
        ((CCombo*)WProp->GetChildByName("combo9"))->SetText("HIGH");
    else
        ((CCombo*)WProp->GetChildByName("combo9"))->SetText("LOW ");

    ((CCombo*)WProp->GetChildByName("combo_1"))->SetItems(Colors);
    ((CCombo*)WProp->GetChildByName("combo_1"))->SetText(Colorname[colors[0]]);
    ((CCombo*)WProp->GetChildByName("combo_2"))->SetItems(Colors);
    ((CCombo*)WProp->GetChildByName("combo_2"))->SetText(Colorname[colors[1]]);
    ((CCombo*)WProp->GetChildByName("combo_3"))->SetItems(Colors);
    ((CCombo*)WProp->GetChildByName("combo_3"))->SetText(Colorname[colors[2]]);
    ((CCombo*)WProp->GetChildByName("combo_4"))->SetItems(Colors);
    ((CCombo*)WProp->GetChildByName("combo_4"))->SetText(Colorname[colors[3]]);
    ((CCombo*)WProp->GetChildByName("combo_5"))->SetItems(Colors);
    ((CCombo*)WProp->GetChildByName("combo_5"))->SetText(Colorname[colors[4]]);
    ((CCombo*)WProp->GetChildByName("combo_6"))->SetItems(Colors);
    ((CCombo*)WProp->GetChildByName("combo_6"))->SetText(Colorname[colors[5]]);
    ((CCombo*)WProp->GetChildByName("combo_7"))->SetItems(Colors);
    ((CCombo*)WProp->GetChildByName("combo_7"))->SetText(Colorname[colors[6]]);
    ((CCombo*)WProp->GetChildByName("combo_8"))->SetItems(Colors);
    ((CCombo*)WProp->GetChildByName("combo_8"))->SetText(Colorname[colors[7]]);
}

void cpart_leds::ReadPropertiesWindow(CPWindow* WProp) {
    input_pins[0] = GetPWCComboSelectedPin(WProp, "combo1");
    input_pins[1] = GetPWCComboSelectedPin(WProp, "combo2");
    input_pins[2] = GetPWCComboSelectedPin(WProp, "combo3");
    input_pins[3] = GetPWCComboSelectedPin(WProp, "combo4");
    input_pins[4] = GetPWCComboSelectedPin(WProp, "combo5");
    input_pins[5] = GetPWCComboSelectedPin(WProp, "combo6");
    input_pins[6] = GetPWCComboSelectedPin(WProp, "combo7");
    input_pins[7] = GetPWCComboSelectedPin(WProp, "combo8");

    active = (((CCombo*)WProp->GetChildByName("combo9"))->GetText().compare("HIGH") == 0);

    for (int i = 0; i < 8; i++) {
        lxString cname;
        cname.Printf("combo_%i", 1 + i);
        lxString val = ((CCombo*)WProp->GetChildByName(cname))->GetText();
        for (int j = 0; j < C_END; j++) {
            if (!val.compare(Colorname[j])) {
                colors[i] = j;
                break;
            }
        }
    }
}

part_init(PART_LEDS_Name, cpart_leds, "Output");
