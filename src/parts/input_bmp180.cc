/* ########################################################################

   PICSimLab - Programmable IC Simulator Laboratory

   ########################################################################

   Copyright (c) : 2020-2024  Luis Claudio Gambôa Lopes <lcgamboa@yahoo.com>

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

#include "input_bmp180.h"
#include "../lib/oscilloscope.h"
#include "../lib/picsimlab.h"
#include "../lib/spareparts.h"

/* outputs */
enum { O_P1, O_P2, O_PF1, O_PF2, O_PO1, O_PO2 };

/* inputs */
enum { I_PO1, I_PO2 };

const char pin_names[4][10] = {"SCL", "SDA", "VCC", "GND"};
const char pin_values[4][10] = {{0}, {1}, "+5V", "GND"};

static PCWProp pcwprop[5] = {{PCW_COMBO, "P1 - SCL"},
                             {PCW_COMBO, "P2 - SDA"},
                             {PCW_LABEL, "P3 - VCC,+5V"},
                             {PCW_LABEL, "P4 - GND,GND"},
                             {PCW_END, ""}};

cpart_bmp180::cpart_bmp180(const unsigned x, const unsigned y, const char* name, const char* type, board* pboard_,
                           const int id_)
    : part(x, y, name, type, pboard_, id_) {
    sen_bmp180_init(&bmp180);
    sen_bmp180_rst(&bmp180);

    input_pins[0] = 0;
    input_pins[1] = 0;

    values[0] = 0;
    values[1] = 0;
    active[0] = 0;
    active[1] = 0;

    SetPCWProperties(pcwprop);

    PinCount = 2;
    Pins = input_pins;
}

cpart_bmp180::~cpart_bmp180(void) {
    sen_bmp180_end(&bmp180);
    delete Bitmap;
    SpareParts.SetPartOnDraw(id);
    SpareParts.CanvasCmd({CC_DESTROY});
}

void cpart_bmp180::DrawOutput(const unsigned int i) {
    char val[10];

    switch (output[i].id) {
        case O_PO1:
            snprintf(val, 10, "%6.0f", (4.0 * (200 - values[0]) + 300));
            DrawSlider(SpareParts.CanvasCmd, &output[i], values[0], val, 6);
            SpareParts.CanvasCmd({CC_SETFONTSIZE, .SetFontSize{8}});
            break;
        case O_PO2:
            snprintf(val, 10, "%5.1f", (0.625 * (200 - values[1]) - 40));
            DrawSlider(SpareParts.CanvasCmd, &output[i], values[1], val, 6);
            SpareParts.CanvasCmd({CC_SETFONTSIZE, .SetFontSize{8}});
            break;
        default:
            SpareParts.CanvasCmd({CC_SETCOLOR, .SetColor{49, 61, 99}});
            SpareParts.CanvasCmd({CC_RECTANGLE, .Rectangle{1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1,
                                                           output[i].y2 - output[i].y1}});

            SpareParts.CanvasCmd({CC_SETFGCOLOR, .SetFgColor{255, 255, 255}});
            SpareParts.CanvasCmd(
                {CC_ROTATEDTEXT, .RotatedText{pin_names[output[i].id - O_P1], output[i].x1, output[i].y2, 90.0}});

            int pinv = pin_values[output[i].id - O_P1][0];
            if (pinv > 10) {
                SpareParts.CanvasCmd({CC_SETFGCOLOR, .SetFgColor{155, 155, 155}});
                SpareParts.CanvasCmd({CC_ROTATEDTEXT, .RotatedText{pin_values[output[i].id - O_P1], output[i].x1,
                                                                   output[i].y2 - 30, 90.0}});
            } else {
                if (input_pins[pinv] == 0)
                    SpareParts.CanvasCmd({CC_ROTATEDTEXT, .RotatedText{"NC", output[i].x1, output[i].y2 - 30, 90.0}});
                else
                    SpareParts.CanvasCmd({CC_ROTATEDTEXT, .RotatedText{SpareParts.GetPinName(input_pins[pinv]).c_str(),
                                                                       output[i].x1, output[i].y2 - 30, 90.0}});
            }
            break;
    }
}

unsigned short cpart_bmp180::GetInputId(char* name) {
    if (strcmp(name, "PO_1") == 0)
        return I_PO1;
    if (strcmp(name, "PO_2") == 0)
        return I_PO2;

    printf("Error input '%s' don't have a valid id! \n", name);
    return INVALID_ID;
}

unsigned short cpart_bmp180::GetOutputId(char* name) {
    if (strcmp(name, "PN_1") == 0)
        return O_P1;
    if (strcmp(name, "PN_2") == 0)
        return O_P2;
    if (strcmp(name, "PN_F1") == 0)
        return O_PF1;
    if (strcmp(name, "PN_F2") == 0)
        return O_PF2;

    if (strcmp(name, "PO_1") == 0)
        return O_PO1;
    if (strcmp(name, "PO_2") == 0)
        return O_PO2;

    printf("Error output '%s' don't have a valid id! \n", name);
    return INVALID_ID;
}

std::string cpart_bmp180::WritePreferences(void) {
    char prefs[256];

    sprintf(prefs, "%hhu,%hhu,%hhu,%hhu", input_pins[0], input_pins[1], values[0], values[1]);

    return prefs;
}

void cpart_bmp180::ReadPreferences(std::string value_) {
    sscanf(value_.c_str(), "%hhu,%hhu,%hhu,%hhu", &input_pins[0], &input_pins[1], &values[0], &values[1]);
    Reset();
}

void cpart_bmp180::ConfigurePropertiesWindow(CPWindow* WProp) {
    SetPCWComboWithPinNames(WProp, "combo1", input_pins[0]);
    SetPCWComboWithPinNames(WProp, "combo2", input_pins[1]);
}

void cpart_bmp180::ReadPropertiesWindow(CPWindow* WProp) {
    input_pins[0] = GetPWCComboSelectedPin(WProp, "combo1");
    input_pins[1] = GetPWCComboSelectedPin(WProp, "combo2");
}

void cpart_bmp180::PreProcess(void) {
    if ((input_pins[0] > 0) && (input_pins[1] > 0)) {
        sen_bmp180_setPressTemp(&bmp180, (4.0 * (200 - values[0]) + 300), (0.625 * (200 - values[1]) - 40));
        SpareParts.ResetPullupBus(input_pins[1] - 1);
    }
}

void cpart_bmp180::Process(void) {
    const picpin* ppins = SpareParts.GetPinsValues();

    if ((input_pins[0] > 0) && (input_pins[1] > 0))
        SpareParts.SetPullupBus(input_pins[1] - 1, sen_bmp180_I2C_io(&bmp180, ppins[input_pins[0] - 1].value,
                                                                     ppins[input_pins[1] - 1].value));
}

void cpart_bmp180::OnMouseButtonPress(uint inputId, uint button, uint x, uint y, uint state) {
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

void cpart_bmp180::OnMouseButtonRelease(uint inputId, uint button, uint x, uint y, uint state) {
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

void cpart_bmp180::OnMouseMove(uint inputId, uint button, uint x, uint y, uint state) {
    switch (inputId) {
        case I_PO1:
            if (active[0]) {
                values[0] = (y - input_ids[I_PO1]->y1) * 1.66;
                if (values[0] > 200)
                    values[0] = 200;
                output_ids[O_PO1]->update = 1;
            }
            break;
        case I_PO2:
            if (active[1]) {
                values[1] = (y - input_ids[I_PO2]->y1) * 1.66;
                if (values[1] > 200)
                    values[1] = 200;
                output_ids[O_PO2]->update = 1;
            }
            break;
        default:
            active[0] = 0;
            active[1] = 0;
            break;
    }
}

part_init(PART_BMP180_Name, cpart_bmp180, "Input");
