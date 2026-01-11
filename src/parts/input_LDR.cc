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

#include "input_LDR.h"
#include "../lib/oscilloscope.h"
#include "../lib/picsimlab.h"
#include "../lib/spareparts.h"
#include "math.h"

/* outputs */
enum { O_P1, O_P2, O_F1, O_F2, O_PO1, O_LED };

/* inputs */
enum { I_PO1 };

static PCWProp pcwprop[6] = {{PCW_LABEL, "1 - VCC,+5V"}, {PCW_COMBO, "2 - D0"}, {PCW_COMBO, "3 - A0"},
                             {PCW_LABEL, "4 - VSS,GND"}, {PCW_SPIND, "Vth"},    {PCW_END, ""}};

cpart_LDR::cpart_LDR(const unsigned x, const unsigned y, const char* name, const char* type, board* pboard_,
                     const int id_)
    : part(x, y, name, type, pboard_, id_) {
    output_pins[0] = 0;
    output_pins[1] = 0;
    vmax = 5.0;
    vthreshold = 2.5;
    value = 0;
    active = 0;

    SetPCWProperties(pcwprop);

    PinCount = 2;
    Pins = output_pins;
}

void cpart_LDR::RegisterRemoteControl(void) {
    input_ids[I_PO1]->status = &value;
    input_ids[I_PO1]->update = &output_ids[O_PO1]->update;
}

cpart_LDR::~cpart_LDR(void) {
    SpareParts.SetPartOnDraw(id);
    SpareParts.CanvasCmd({.cmd = CC_FREEBITMAP, .FreeBitmap{BitmapId}});
    SpareParts.CanvasCmd({.cmd = CC_DESTROY});
}

void cpart_LDR::Reset(void) {
    vmax = pboard->MGetVCC();
}

void cpart_LDR::DrawOutput(const unsigned int i) {
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
        case O_P2:
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
        case O_PO1:
            lux = powf(10, ((200 - value) / 33.33) - 1);
            if (log10(lux) < 3) {
                snprintf(val, 10, "%4.1f", lux);
            } else {
                snprintf(val, 10, "%4.0fk", lux / 1000);
            }
            DrawSlider(SpareParts.CanvasCmd, &output[i], value, val, 7);
            SpareParts.CanvasCmd({.cmd = CC_SETFONTSIZE, .SetFontSize{9}});
            break;
        case O_LED:
            SpareParts.CanvasCmd({.cmd = CC_SETCOLOR, .SetColor{(unsigned int)((vout > vthreshold) ? 255 : 55), 0, 0}});
            SpareParts.CanvasCmd(
                {.cmd = CC_RECTANGLE,
                 .Rectangle{1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1}});
            break;
    }
}

void cpart_LDR::PostProcess(void) {
    const float gamma = 0.7;
    const float r10 = 20000.0;
    const float res = r10 / (powf(10, gamma * log10(lux / 10.0)));
    vout = (res * vmax) / (res + 10000.0);
    const int vd = (vout > vthreshold) ? 1 : 0;

    if (output_ids[O_LED]->value != vd) {
        if (output_pins[0]) {
            SpareParts.SetPin(output_pins[0], vd);
        }
        output_ids[O_LED]->update = 1;
        output_ids[O_LED]->value = vd;
    }

    if (output_pins[1]) {
        SpareParts.SetAPin(output_pins[1], vout);
    }
}

void cpart_LDR::OnMouseButtonPress(unsigned int inputId, unsigned int button, unsigned int x, unsigned int y,
                                   unsigned int state) {
    switch (inputId) {
        case I_PO1:
            value = (y - input_ids[I_PO1]->y1) * 1.66;
            if (value > 200)
                value = 200;
            active = 1;
            output_ids[O_PO1]->update = 1;
            break;
    }
}

void cpart_LDR::OnMouseButtonRelease(unsigned int inputId, unsigned int button, unsigned int x, unsigned int y,
                                     unsigned int state) {
    switch (inputId) {
        case I_PO1:
            active = 0;
            output_ids[O_PO1]->update = 1;
            break;
    }
}

void cpart_LDR::OnMouseMove(unsigned int inputId, unsigned int button, unsigned int x, unsigned int y,
                            unsigned int state) {
    switch (inputId) {
        case I_PO1:
            if (active) {
                value = (y - input_ids[I_PO1]->y1) * 1.66;
                if (value > 200)
                    value = 200;
                output_ids[O_PO1]->update = 1;
            }
            break;
        default:
            active = 0;
            break;
    }
}

unsigned short cpart_LDR::GetInputId(char* name) {
    if (strcmp(name, "PO_1") == 0)
        return I_PO1;

    printf("Error input '%s' don't have a valid id! \n", name);
    return INVALID_ID;
}

unsigned short cpart_LDR::GetOutputId(char* name) {
    if (strcmp(name, "PN_1") == 0)
        return O_P1;
    if (strcmp(name, "PN_2") == 0)
        return O_P2;

    if (strcmp(name, "PN_F1") == 0)
        return O_F1;
    if (strcmp(name, "PN_F2") == 0)
        return O_F2;

    if (strcmp(name, "PO_1") == 0)
        return O_PO1;

    if (strcmp(name, "LED") == 0)
        return O_LED;

    printf("Error output '%s' don't have a valid id! \n", name);
    return INVALID_ID;
}

std::string cpart_LDR::WritePreferences(void) {
    char prefs[256];

    sprintf(prefs, "%hhu,%hhu,%hhu,%f", output_pins[0], output_pins[1], value, vthreshold);

    return prefs;
}

void cpart_LDR::ReadPreferences(std::string value_) {
    sscanf(value_.c_str(), "%hhu,%hhu,%hhu,%f", &output_pins[0], &output_pins[1], &value, &vthreshold);
}

void cpart_LDR::ConfigurePropertiesWindow(void) {
    SetPCWComboWithPinNames("combo2", output_pins[0]);
    SetPCWComboWithPinNames("combo3", output_pins[1]);

    SpareParts.WPropCmd("spind5", PWA_SPINDSETMAX, std::to_string(vmax).c_str());
    SpareParts.WPropCmd("spind5", PWA_SPINDSETMIN, "0");
    SpareParts.WPropCmd("spind5", PWA_SPINDSETVALUE, std::to_string(vthreshold).c_str());
}

void cpart_LDR::ReadPropertiesWindow(void) {
    output_pins[0] = GetPWCComboSelectedPin("combo2");
    output_pins[1] = GetPWCComboSelectedPin("combo3");

    SpareParts.WPropCmd("spind5", PWA_SPINDGETVALUE, NULL, &vthreshold);
}

part_init(PART_LDR_Name, cpart_LDR, "Input");
