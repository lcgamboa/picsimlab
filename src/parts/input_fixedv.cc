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

#include "input_fixedv.h"
#include "../lib/oscilloscope.h"
#include "../lib/picsimlab.h"
#include "../lib/spareparts.h"

/* outputs */
enum { O_P1, O_F1, O_F2, O_VOLT };

static PCWProp pcwprop[5] = {{PCW_LABEL, "1 - VCC,+5V"},
                             {PCW_COMBO, "2 - OUT"},
                             {PCW_LABEL, "3 - GND ,GND"},
                             {PCW_SPIND, "4 - Value"},
                             {PCW_END, ""}};

cpart_fixedv::cpart_fixedv(const unsigned x, const unsigned y, const char* name, const char* type, board* pboard_,
                           const int id_)
    : part(x, y, name, type, pboard_, id_) {
    output_pins[0] = 0;

    value = 0;
    active = 0;

    SetPCWProperties(pcwprop);

    PinCount = 1;
    Pins = output_pins;
}

void cpart_fixedv::RegisterRemoteControl(void) {}

cpart_fixedv::~cpart_fixedv(void) {
    SpareParts.SetPartOnDraw(id);
    SpareParts.CanvasCmd({.cmd = CC_FREEBITMAP, .FreeBitmap{BitmapId}});
    SpareParts.CanvasCmd({.cmd = CC_DESTROY});
}

void cpart_fixedv::DrawOutput(const unsigned int i) {
    char val[10];

    switch (output[i].id) {
        case O_P1:
            SpareParts.CanvasCmd({.cmd = CC_SETCOLOR, .SetColor{49, 61, 99}});
            SpareParts.CanvasCmd(
                {.cmd = CC_RECTANGLE,
                 .Rectangle{1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1}});
            SpareParts.CanvasCmd({.cmd = CC_SETFGCOLOR, .SetFgColor{255, 255, 255}});
            if (output_pins[output[i].id - O_P1] == 0)
                SpareParts.CanvasCmd({.cmd = CC_ROTATEDTEXT, .RotatedText{"NC", output[i].x1, output[i].y2, 90}});
            else
                SpareParts.CanvasCmd({.cmd = CC_ROTATEDTEXT,
                                      .RotatedText{SpareParts.GetPinName(output_pins[output[i].id - O_P1]).c_str(),
                                                   output[i].x1, output[i].y2, 90}});
            break;
        case O_F1:
            SpareParts.CanvasCmd({.cmd = CC_SETCOLOR, .SetColor{49, 61, 99}});
            SpareParts.CanvasCmd(
                {.cmd = CC_RECTANGLE,
                 .Rectangle{1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1}});
            SpareParts.CanvasCmd({.cmd = CC_SETFGCOLOR, .SetFgColor{155, 155, 155}});
            SpareParts.CanvasCmd({.cmd = CC_ROTATEDTEXT, .RotatedText{"+5V", output[i].x1, output[i].y2, 90}});
            break;
        case O_F2:
            SpareParts.CanvasCmd({.cmd = CC_SETCOLOR, .SetColor{49, 61, 99}});
            SpareParts.CanvasCmd(
                {.cmd = CC_RECTANGLE,
                 .Rectangle{1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1}});
            SpareParts.CanvasCmd({.cmd = CC_SETFGCOLOR, .SetFgColor{155, 155, 155}});
            SpareParts.CanvasCmd({.cmd = CC_ROTATEDTEXT, .RotatedText{"GND", output[i].x1, output[i].y2, 90}});
            break;
        case O_VOLT:
            SpareParts.CanvasCmd({.cmd = CC_SETCOLOR, .SetColor{49, 61, 99}});
            SpareParts.CanvasCmd(
                {.cmd = CC_RECTANGLE,
                 .Rectangle{1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1}});
            snprintf(val, 10, "%5.2f V", value / 40.0);
            SpareParts.CanvasCmd({.cmd = CC_SETCOLOR, .SetColor{250, 250, 250}});
            SpareParts.CanvasCmd({.cmd = CC_SETFONTSIZE, .SetFontSize{12}});
            SpareParts.CanvasCmd({.cmd = CC_ROTATEDTEXT, .RotatedText{val, output[i].x1, output[i].y1, 0}});
            SpareParts.CanvasCmd({.cmd = CC_SETFONTSIZE, .SetFontSize{9}});
            break;
    }
}

void cpart_fixedv::PostProcess(void) {
    if (output_pins[0]) {
        SpareParts.SetAPin(output_pins[0], value / 40.0);
    }
}

unsigned short cpart_fixedv::GetInputId(char* name) {
    printf("Error input '%s' don't have a valid id! \n", name);
    return INVALID_ID;
}

unsigned short cpart_fixedv::GetOutputId(char* name) {
    if (strcmp(name, "PN_1") == 0)
        return O_P1;

    if (strcmp(name, "PN_F1") == 0)
        return O_F1;
    if (strcmp(name, "PN_F2") == 0)
        return O_F2;

    if (strcmp(name, "DI_VOLT") == 0)
        return O_VOLT;

    printf("Error output '%s' don't have a valid id! \n", name);
    return INVALID_ID;
}

std::string cpart_fixedv::WritePreferences(void) {
    char prefs[256];

    sprintf(prefs, "%hhu,%hhu", output_pins[0], value);

    return prefs;
}

void cpart_fixedv::ReadPreferences(std::string value_) {
    sscanf(value_.c_str(), "%hhu,%hhu", &output_pins[0], &value);
}

void cpart_fixedv::ConfigurePropertiesWindow(void) {
    SetPCWComboWithPinNames("combo2", output_pins[0]);

    SpareParts.WPropCmd("spind4", PWA_SPINDSETMAX, "5.0");
    SpareParts.WPropCmd("spind4", PWA_SPINDSETMIN, "0.0");
    SpareParts.WPropCmd("spind4", PWA_SPINDSETVALUE, std::to_string(value / 40.0).c_str());
}

void cpart_fixedv::ReadPropertiesWindow(void) {
    output_pins[0] = GetPWCComboSelectedPin("combo2");

    float valuef;
    SpareParts.WPropCmd("spind4", PWA_SPINDGETVALUE, NULL, &valuef);

    value = valuef * 40.0;
}

part_init(PART_FIXEDV_Name, cpart_fixedv, "Input");
