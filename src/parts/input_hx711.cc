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

#include "input_hx711.h"
#include "../lib/oscilloscope.h"
#include "../lib/picsimlab.h"
#include "../lib/spareparts.h"

/* outputs */
enum { O_PF1, O_PF2, O_P1, O_P2, O_PO1 };

/* inputs */
enum { I_PO1 };

const char pin_names[4][10] = {"GND", "VCC", "DT", "SCK"};
const char pin_values[4][10] = {"GND", "+5V", {0}, {1}};

static PCWProp pcwprop[5] = {{PCW_LABEL, "1-GND,GND"},
                             {PCW_COMBO, "2-DT"},
                             {PCW_COMBO, "3-SCK"},
                             {PCW_LABEL, "4-VCC,+5V"},
                             {PCW_END, ""}};

cpart_hx711::cpart_hx711(const unsigned x, const unsigned y, const char* name, const char* type, board* pboard_)
    : part(x, y, name, type, pboard_),
      font(8, lxFONTFAMILY_TELETYPE, lxFONTSTYLE_NORMAL, lxFONTWEIGHT_BOLD),
      font_p(6, lxFONTFAMILY_TELETYPE, lxFONTSTYLE_NORMAL, lxFONTWEIGHT_BOLD) {
    sen_hx711_init(&hx711, pboard);
    sen_hx711_rst(&hx711);

    pins[0] = 0;
    pins[1] = 0;

    values[0] = 0;
    active[0] = 0;

    SetPCWProperties(pcwprop);

    PinCount = 2;
    Pins = pins;
}

cpart_hx711::~cpart_hx711(void) {
    sen_hx711_end(&hx711);
    delete Bitmap;
    canvas.Destroy();
}

void cpart_hx711::DrawOutput(const unsigned int i) {
    char val[10];

    switch (output[i].id) {
        case O_PO1:
            snprintf(val, 10, "%6.1f", (0.5 * (200 - values[0])));
            DrawSlider(&output[i], values[0], val, font_p);
            canvas.SetFont(font);
            break;
        default:
            canvas.SetColor(49, 61, 99);
            canvas.Rectangle(1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);

            canvas.SetFgColor(255, 255, 255);
            canvas.RotatedText(pin_names[output[i].id - O_PF1], output[i].x1, output[i].y2, 90.0);

            int pinv = pin_values[output[i].id - O_PF1][0];
            if (pinv > 10) {
                canvas.SetFgColor(155, 155, 155);
                canvas.RotatedText(pin_values[output[i].id - O_PF1], output[i].x1, output[i].y2 - 30, 90.0);
            } else {
                if (pins[pinv] == 0)
                    canvas.RotatedText("NC", output[i].x1, output[i].y2 - 30, 90.0);
                else
                    canvas.RotatedText(SpareParts.GetPinName(pins[pinv]), output[i].x1, output[i].y2 - 30, 90.0);
            }
            break;
    }
}

unsigned short cpart_hx711::GetInputId(char* name) {
    if (strcmp(name, "PO_1") == 0)
        return I_PO1;

    printf("Error input '%s' don't have a valid id! \n", name);
    return INVALID_ID;
}

unsigned short cpart_hx711::GetOutputId(char* name) {
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

    printf("Error output '%s' don't have a valid id! \n", name);
    return INVALID_ID;
}

std::string cpart_hx711::WritePreferences(void) {
    char prefs[256];

    sprintf(prefs, "%hhu,%hhu,%hhu", pins[0], pins[1], values[0]);

    return prefs;
}

void cpart_hx711::ReadPreferences(std::string value_) {
    sscanf(value_.c_str(), "%hhu,%hhu,%hhu", &pins[0], &pins[1], &values[0]);
    Reset();
}

void cpart_hx711::ConfigurePropertiesWindow(CPWindow* WProp) {
    SetPCWComboWithPinNames(WProp, "combo2", pins[0]);
    SetPCWComboWithPinNames(WProp, "combo3", pins[1]);
}

void cpart_hx711::ReadPropertiesWindow(CPWindow* WProp) {
    pins[0] = GetPWCComboSelectedPin(WProp, "combo2");
    pins[1] = GetPWCComboSelectedPin(WProp, "combo3");
}

void cpart_hx711::PreProcess(void) {
    sen_hx711_setWeight(&hx711, (0.5 * (200 - values[0])));
}

void cpart_hx711::Process(void) {
    const picpin* ppins = SpareParts.GetPinsValues();

    if (pins[0] && pins[1]) {
        unsigned char ret = sen_hx711_io_SPI(&hx711, ppins[pins[1] - 1].value);

        if (ret_ != ret) {
            SpareParts.SetPin(pins[0], ret);
        }
        ret_ = ret;
    }
}

void cpart_hx711::Reset(void) {
    sen_hx711_rst(&hx711);
}

void cpart_hx711::OnMouseButtonPress(uint inputId, uint button, uint x, uint y, uint state) {
    switch (inputId) {
        case I_PO1:
            values[0] = (y - input_ids[I_PO1]->y1) * 1.66;
            if (values[0] > 200)
                values[0] = 200;
            active[0] = 1;
            output_ids[O_PO1]->update = 1;
            break;
    }
}

void cpart_hx711::OnMouseButtonRelease(uint inputId, uint button, uint x, uint y, uint state) {
    switch (inputId) {
        case I_PO1:
            active[0] = 0;
            output_ids[O_PO1]->update = 1;
            break;
    }
}

void cpart_hx711::OnMouseMove(uint inputId, uint button, uint x, uint y, uint state) {
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

part_init(PART_BMP280_Name, cpart_hx711, "Input");
