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

#include "output_LED_matrix.h"
#include "../lib/oscilloscope.h"
#include "../lib/picsimlab.h"
#include "../lib/spareparts.h"

/* outputs */
enum { O_P1, O_P2, O_P3, O_P4, O_F1, O_F2, O_LED };

static PCWProp pcwprop[9] = {{PCW_LABEL, "1-VCC,+5V"}, {PCW_LABEL, "2-GND,GND"}, {PCW_COMBO, "3-DIN"},
                             {PCW_COMBO, "4-CS"},      {PCW_COMBO, "5-CLK"},     {PCW_LABEL, "6-Dout,NC"},
                             {PCW_COMBO, "Angle"},     {PCW_COMBO, "Model"},     {PCW_END, ""}};

cpart_led_matrix::cpart_led_matrix(const unsigned x, const unsigned y, const char* name, const char* type,
                                   board* pboard_, const int id_)
    : part(x, y, name, type, pboard_, id_) {
    X = x;
    Y = y;
    ReadMaps();
    BitmapId = -1;
    angle = 0;
    lmode = 0;

    LoadPartImage();

    ldd_max72xx_init(&ldd);
    ldd_max72xx_rst(&ldd);

    input_pins[0] = 0;
    input_pins[1] = 0;
    input_pins[2] = 0;

    output_pins[0] = SpareParts.RegisterIOpin("DOUT");

    SetPCWProperties(pcwprop);

    PinCount = 3;
    Pins = input_pins;
    PinCtrlCount = 1;
    PinsCtrl = output_pins;
}

cpart_led_matrix::~cpart_led_matrix(void) {
    SpareParts.UnregisterIOpin(output_pins[0]);
    SpareParts.SetPartOnDraw(id);
    SpareParts.CanvasCmd({CC_FREEBITMAP, .FreeBitmap{BitmapId}});
    SpareParts.CanvasCmd({CC_DESTROY});
}

void cpart_led_matrix::DrawOutput(const unsigned int i) {
    switch (output[i].id) {
        case O_P1:
        case O_P2:
        case O_P3:
            SpareParts.CanvasCmd({CC_SETFONTSIZE, .SetFontSize{8}});
            SpareParts.CanvasCmd({CC_SETCOLOR, .SetColor{49, 61, 99}});
            SpareParts.CanvasCmd({CC_RECTANGLE, .Rectangle{1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1,
                                                           output[i].y2 - output[i].y1}});
            SpareParts.CanvasCmd({CC_SETFGCOLOR, .SetFgColor{255, 255, 255}});
            if (input_pins[output[i].id - O_P1] == 0)
                SpareParts.CanvasCmd({CC_ROTATEDTEXT, .RotatedText{"NC", output[i].x1, output[i].y2, 90.0}});
            else
                SpareParts.CanvasCmd(
                    {CC_ROTATEDTEXT, .RotatedText{SpareParts.GetPinName(input_pins[output[i].id - O_P1]).c_str(),
                                                  output[i].x1, output[i].y2, 90.0}});
            break;
        case O_P4:
            SpareParts.CanvasCmd({CC_SETCOLOR, .SetColor{49, 61, 99}});
            SpareParts.CanvasCmd({CC_RECTANGLE, .Rectangle{1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1,
                                                           output[i].y2 - output[i].y1}});
            SpareParts.CanvasCmd({CC_SETFGCOLOR, .SetFgColor{255, 255, 255}});
            if (output_pins[output[i].id - O_P4] == 0)
                SpareParts.CanvasCmd({CC_ROTATEDTEXT, .RotatedText{"NC", output[i].x1, output[i].y2, 90.0}});
            else
                SpareParts.CanvasCmd(
                    {CC_ROTATEDTEXT, .RotatedText{std::to_string(output_pins[output[i].id - O_P4]).c_str(),
                                                  output[i].x1, output[i].y2, 90.0}});
            break;
            break;
        case O_F1:
            SpareParts.CanvasCmd({CC_SETCOLOR, .SetColor{49, 61, 99}});
            SpareParts.CanvasCmd({CC_RECTANGLE, .Rectangle{1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1,
                                                           output[i].y2 - output[i].y1}});
            SpareParts.CanvasCmd({CC_SETFGCOLOR, .SetFgColor{155, 155, 155}});
            SpareParts.CanvasCmd({CC_ROTATEDTEXT, .RotatedText{"5V", output[i].x1, output[i].y2, 90.0}});
            break;
        case O_F2:
            SpareParts.CanvasCmd({CC_SETCOLOR, .SetColor{49, 61, 99}});
            SpareParts.CanvasCmd({CC_RECTANGLE, .Rectangle{1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1,
                                                           output[i].y2 - output[i].y1}});
            SpareParts.CanvasCmd({CC_SETFGCOLOR, .SetFgColor{155, 155, 155}});
            SpareParts.CanvasCmd({CC_ROTATEDTEXT, .RotatedText{"GND", output[i].x1, output[i].y2, 90.0}});
            break;
        case O_LED:
            // draw ldd text
            if (ldd.update) {
                SpareParts.CanvasCmd({CC_SETCOLOR, .SetColor{0, 90 + 40, 0}});
                ldd_max72xx_draw(&ldd, SpareParts.CanvasCmd, output[i].x1, output[i].y1, output[i].x2 - output[i].x1,
                                 output[i].y2 - output[i].y1, 1, angle, lmode);
            }
            break;
    }
}

unsigned short cpart_led_matrix::GetInputId(char* name) {
    printf("Error input '%s' don't have a valid id! \n", name);
    return INVALID_ID;
};

unsigned short cpart_led_matrix::GetOutputId(char* name) {
    if (strcmp(name, "PN_1") == 0)
        return O_P1;
    if (strcmp(name, "PN_2") == 0)
        return O_P2;
    if (strcmp(name, "PN_3") == 0)
        return O_P3;
    if (strcmp(name, "PN_4") == 0)
        return O_P4;

    if (strcmp(name, "PN_F1") == 0)
        return O_F1;
    if (strcmp(name, "PN_F2") == 0)
        return O_F2;

    if (strcmp(name, "LM_LED") == 0)
        return O_LED;

    printf("Error output '%s' don't have a valid id! \n", name);
    return INVALID_ID;
};

std::string cpart_led_matrix::WritePreferences(void) {
    char prefs[256];

    sprintf(prefs, "%hhu,%hhu,%hhu,%hhu,%i,%i", input_pins[0], input_pins[1], input_pins[2], output_pins[0], angle,
            lmode);

    return prefs;
}

void cpart_led_matrix::ReadPreferences(std::string value) {
    unsigned char outp;
    sscanf(value.c_str(), "%hhu,%hhu,%hhu,%hhu,%i,%i", &input_pins[0], &input_pins[1], &input_pins[2], &outp, &angle,
           &lmode);

    SpareParts.UnregisterIOpin(output_pins[0]);
    output_pins[0] = SpareParts.RegisterIOpin("DOUT", outp);

    Reset();
}

void cpart_led_matrix::RegisterRemoteControl(void) {
    output_ids[O_LED]->status = (void*)&ldd;
}

void cpart_led_matrix::ConfigurePropertiesWindow(CPWindow* WProp) {
    SetPCWComboWithPinNames(WProp, "combo3", input_pins[0]);
    SetPCWComboWithPinNames(WProp, "combo4", input_pins[1]);
    SetPCWComboWithPinNames(WProp, "combo5", input_pins[2]);

    ((CLabel*)WProp->GetChildByName("label_6"))->SetText(std::to_string(output_pins[0]));

    ((CCombo*)WProp->GetChildByName("combo7"))->SetItems("0,90,180,270,");
    ((CCombo*)WProp->GetChildByName("combo7"))->SetText(std::to_string(angle));

    ((CCombo*)WProp->GetChildByName("combo8"))->SetItems("FC16,Parola,");
    ((CCombo*)WProp->GetChildByName("combo8"))->SetText((!lmode ? "FC16" : "Parola"));
}

void cpart_led_matrix::ReadPropertiesWindow(CPWindow* WProp) {
    input_pins[0] = GetPWCComboSelectedPin(WProp, "combo3");
    input_pins[1] = GetPWCComboSelectedPin(WProp, "combo4");
    input_pins[2] = GetPWCComboSelectedPin(WProp, "combo5");
    angle = atoi(((CCombo*)WProp->GetChildByName("combo7"))->GetText());
    lmode = !(((CCombo*)WProp->GetChildByName("combo8"))->GetText().compare("FC16") == 0);
}

void cpart_led_matrix::Process(void) {
    const picpin* ppins = SpareParts.GetPinsValues();

    if ((input_pins[0] > 0) && (input_pins[1] > 0) && (input_pins[2] > 0)) {
        unsigned char out;
        out = ldd_max72xx_io(&ldd, ppins[input_pins[0] - 1].value, ppins[input_pins[2] - 1].value,
                             ppins[input_pins[1] - 1].value);

        if (out != ppins[output_pins[0] - 1].value) {
            SpareParts.WritePin(output_pins[0], out);
        }
    }
}

void cpart_led_matrix::PostProcess(void) {
    if (ldd.update)
        output_ids[O_LED]->update = 1;
}

part_init(PART_LED_MATRIX_Name, cpart_led_matrix, "Output");
