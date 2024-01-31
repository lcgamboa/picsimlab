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

#include "input_sht3x.h"
#include "../lib/oscilloscope.h"
#include "../lib/picsimlab.h"
#include "../lib/spareparts.h"

/* outputs */
enum { O_P1, O_P2, O_F1, O_F2, O_PO1, O_PO2 };

/* inputs */
enum { I_PO1, I_PO2 };

static PCWProp pcwprop[6] = {{PCW_LABEL, "Pin 1        +5V"},  {PCW_COMBO, "Pin 2"},  {PCW_COMBO, "Pin 3"},
                             {PCW_LABEL, "Pin 4         GND"}, {PCW_COMBO, "Output"}, {PCW_END, ""}};

cpart_sht3x::cpart_sht3x(const unsigned x, const unsigned y, const char* name, const char* type, board* pboard_,
                         const int id_)
    : part(x, y, name, type, pboard_, id_) {
    output_pins[0] = 0;
    output_pins[1] = 0;

    values[0] = 0;
    values[1] = 0;

    active[0] = 0;
    active[1] = 0;

    SetPCWProperties(pcwprop);

    PinCount = 2;
    Pins = output_pins;
}

void cpart_sht3x::RegisterRemoteControl(void) {
    input_ids[I_PO1]->status = &values[0];
    input_ids[I_PO1]->update = &output_ids[O_PO1]->update;
    input_ids[I_PO2]->status = &values[1];
    input_ids[I_PO2]->update = &output_ids[O_PO2]->update;
}

cpart_sht3x::~cpart_sht3x(void) {
    SpareParts.SetPartOnDraw(id);
    SpareParts.CanvasCmd({CC_FREEBITMAP, .FreeBitmap{BitmapId}});
    SpareParts.CanvasCmd({CC_DESTROY});
}

void cpart_sht3x::DrawOutput(const unsigned int i) {
    char val[10];

    switch (output[i].id) {
        case O_P1:
            SpareParts.CanvasCmd({CC_SETCOLOR, .SetColor{49, 61, 99}});
            SpareParts.CanvasCmd({CC_RECTANGLE, .Rectangle{1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1,
                                                           output[i].y2 - output[i].y1}});
            SpareParts.CanvasCmd({CC_SETFGCOLOR, .SetFgColor{255, 255, 255}});
            if (output_pins[output[i].id - O_P1] == 0)
                SpareParts.CanvasCmd({CC_ROTATEDTEXT, .RotatedText{"2-T   NC", output[i].x1, output[i].y2, 90}});
            else
                SpareParts.CanvasCmd(
                    {CC_ROTATEDTEXT,
                     .RotatedText{("2-T   " + SpareParts.GetPinName(output_pins[output[i].id - O_P1])).c_str(),
                                  output[i].x1, output[i].y2, 90}});
            break;
        case O_P2:
            SpareParts.CanvasCmd({CC_SETCOLOR, .SetColor{49, 61, 99}});
            SpareParts.CanvasCmd({CC_RECTANGLE, .Rectangle{1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1,
                                                           output[i].y2 - output[i].y1}});
            SpareParts.CanvasCmd({CC_SETFGCOLOR, .SetFgColor{255, 255, 255}});
            if (output_pins[output[i].id - O_P1] == 0)
                SpareParts.CanvasCmd({CC_ROTATEDTEXT, .RotatedText{"3-RH  NC", output[i].x1, output[i].y2, 90}});
            else
                SpareParts.CanvasCmd(
                    {CC_ROTATEDTEXT,
                     .RotatedText{("3-RH  " + SpareParts.GetPinName(output_pins[output[i].id - O_P1])).c_str(),
                                  output[i].x1, output[i].y2, 90}});
            break;
        case O_F1:
            SpareParts.CanvasCmd({CC_SETCOLOR, .SetColor{49, 61, 99}});
            SpareParts.CanvasCmd({CC_RECTANGLE, .Rectangle{1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1,
                                                           output[i].y2 - output[i].y1}});
            SpareParts.CanvasCmd({CC_SETFGCOLOR, .SetFgColor{155, 155, 155}});
            SpareParts.CanvasCmd({CC_ROTATEDTEXT, .RotatedText{"1-VCC +5V", output[i].x1, output[i].y2, 90}});
            break;
        case O_F2:
            SpareParts.CanvasCmd({CC_SETCOLOR, .SetColor{49, 61, 99}});
            SpareParts.CanvasCmd({CC_RECTANGLE, .Rectangle{1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1,
                                                           output[i].y2 - output[i].y1}});
            SpareParts.CanvasCmd({CC_SETFGCOLOR, .SetFgColor{155, 155, 155}});
            SpareParts.CanvasCmd({CC_ROTATEDTEXT, .RotatedText{"4-GND GND", output[i].x1, output[i].y2, 90}});
            break;
        case O_PO1:
            snprintf(val, 10, "%5.1f", (0.825 * (200 - values[0]) - 40));
            DrawSlider(SpareParts.CanvasCmd, &output[i], values[0], val, 7);
            SpareParts.CanvasCmd({CC_SETFONTSIZE, .SetFontSize{9}});
            break;
        case O_PO2:
            snprintf(val, 10, " %3.0f%%", ((200 - values[1]) / 2.0));
            DrawSlider(SpareParts.CanvasCmd, &output[i], values[1], val, 7);
            SpareParts.CanvasCmd({CC_SETFONTSIZE, .SetFontSize{9}});
            break;
    }
}

void cpart_sht3x::PostProcess(void) {
    if (output_pins[0]) {
        SpareParts.SetAPin(output_pins[0], (((((0.825 * (200 - values[0]) - 40)) + 66.875) / 218.75) * 5.0));
    }
    if (output_pins[1]) {
        SpareParts.SetAPin(output_pins[1], ((((200 - values[1]) / 2.0) + 12.5) / 125.0) * 5.0);
    }
}

void cpart_sht3x::OnMouseButtonPress(uint inputId, uint button, uint x, uint y, uint state) {
    switch (inputId) {
        case I_PO1:
            values[0] = (y - input_ids[I_PO1]->y1) * 1.66;
            if (values[0] > 200)
                values[0] = 200;
            active[0] = 1;
            output_ids[O_PO1]->update = 1;
            break;
        case I_PO2:
            values[1] = (y - input_ids[I_PO2]->y1) * 1.66;
            if (values[1] > 200)
                values[1] = 200;
            active[1] = 1;
            output_ids[O_PO2]->update = 1;
            break;
    }
}

void cpart_sht3x::OnMouseButtonRelease(uint inputId, uint button, uint x, uint y, uint state) {
    switch (inputId) {
        case I_PO1:
            active[0] = 0;
            output_ids[O_PO1]->update = 1;
            break;
        case I_PO2:
            active[1] = 0;
            output_ids[O_PO2]->update = 1;
            break;
    }
}

void cpart_sht3x::OnMouseMove(uint inputId, uint button, uint x, uint y, uint state) {
    switch (inputId) {
        case I_PO1:
            if (active[0]) {
                values[0] = (y - input_ids[I_PO1]->y1) * 1.66;
                if (values[0] > 200)
                    values[0] = 200;
                output_ids[O_PO1]->update = 1;
            }
            break;
        default:
            active[0] = 0;
            break;
    }
}

unsigned short cpart_sht3x::GetInputId(char* name) {
    if (strcmp(name, "PO_1") == 0)
        return I_PO1;
    if (strcmp(name, "PO_2") == 0)
        return I_PO2;

    printf("Error input '%s' don't have a valid id! \n", name);
    return INVALID_ID;
}

unsigned short cpart_sht3x::GetOutputId(char* name) {
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
    if (strcmp(name, "PO_2") == 0)
        return O_PO2;

    printf("Error output '%s' don't have a valid id! \n", name);
    return INVALID_ID;
}

std::string cpart_sht3x::WritePreferences(void) {
    char prefs[256];

    sprintf(prefs, "%hhu,%hhu,%hhu,%hhu", output_pins[0], output_pins[1], values[0], values[1]);

    return prefs;
}

void cpart_sht3x::ReadPreferences(std::string value_) {
    sscanf(value_.c_str(), "%hhu,%hhu,%hhu,%hhu", &output_pins[0], &output_pins[1], &values[0], &values[1]);
}

void cpart_sht3x::ConfigurePropertiesWindow(void) {
    SetPCWComboWithPinNames("combo2", output_pins[0]);
    SetPCWComboWithPinNames("combo3", output_pins[1]);

    SpareParts.WPropCmd("combo5", WPA_COMBOSETITEMS, "Analog,");
    SpareParts.WPropCmd("combo5", WPA_COMBOSETTEXT, "Analog");
}

void cpart_sht3x::ReadPropertiesWindow(void) {
    output_pins[0] = GetPWCComboSelectedPin("combo2");
    output_pins[1] = GetPWCComboSelectedPin("combo3");
}

part_init(PART_SHT3X_Name, cpart_sht3x, "Input");
