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

#include "input_hcsr04.h"
#include "../lib/oscilloscope.h"
#include "../lib/picsimlab.h"
#include "../lib/spareparts.h"

/* outputs */
enum { O_P1, O_P2, O_F1, O_F2, O_PO1 };

/* inputs */
enum { I_PO1 };

static PCWProp pcwprop[5] = {{PCW_LABEL, "1 - VCC,+5V"},
                             {PCW_COMBO, "2 - TRIG"},
                             {PCW_COMBO, "3 - ECHO"},
                             {PCW_LABEL, "4 - GND ,GND"},
                             {PCW_END, ""}};

cpart_hcsr04::cpart_hcsr04(const unsigned x, const unsigned y, const char* name, const char* type, board* pboard_)
    : part(x, y, name, type, pboard_),
      font(9, lxFONTFAMILY_TELETYPE, lxFONTSTYLE_NORMAL, lxFONTWEIGHT_BOLD),
      font_p(7, lxFONTFAMILY_TELETYPE, lxFONTSTYLE_NORMAL, lxFONTWEIGHT_BOLD) {
    always_update = 1;

    pins[0] = 0;

    pins[1] = 0;

    value = 0;
    active = 0;
    old_value = 0;
    delay = 0;
    count = 0;

    SetPCWProperties(pcwprop);

    PinCount = 2;
    Pins = pins;
}

void cpart_hcsr04::RegisterRemoteControl(void) {
    input_ids[I_PO1]->status = &value;
    input_ids[I_PO1]->update = &output_ids[O_PO1]->update;
}

cpart_hcsr04::~cpart_hcsr04(void) {
    delete Bitmap;
    canvas.Destroy();
}

void cpart_hcsr04::DrawOutput(const unsigned int i) {
    char val[10];

    switch (output[i].id) {
        case O_P1:
        case O_P2:
            canvas.SetColor(49, 61, 99);
            canvas.Rectangle(1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);
            canvas.SetFgColor(255, 255, 255);
            if (pins[output[i].id - O_P1] == 0)
                canvas.RotatedText("NC", output[i].x1, output[i].y2, 90);
            else
                canvas.RotatedText(SpareParts.GetPinName(pins[output[i].id - O_P1]), output[i].x1, output[i].y2, 90);
            break;
        case O_F1:
            canvas.SetColor(49, 61, 99);
            canvas.Rectangle(1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);
            canvas.SetFgColor(155, 155, 155);
            canvas.RotatedText("+5V", output[i].x1, output[i].y2, 90);
            break;
        case O_F2:
            canvas.SetColor(49, 61, 99);
            canvas.Rectangle(1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);
            canvas.SetFgColor(155, 155, 155);
            canvas.RotatedText("GND", output[i].x1, output[i].y2, 90);
            break;
        case O_PO1:
            snprintf(val, 10, " %3i", (int)(400.0 * (200 - value) / 200.0));
            DrawSlider(&output[i], value, val, font_p);
            canvas.SetFont(font);
            break;
    }
}

void cpart_hcsr04::Process(void) {
    const picpin* pins_ = SpareParts.GetPinsValues();

    if ((pins[0]) && (pins[1])) {
        // output
        if (count) {
            count--;
            if (!count) {
                SpareParts.SetPin(pins[1], 0);
            }
        }

        // pulse 48 kHz
        if (delay) {
            delay--;
            if (!delay) {
                count = (200 - value) * 116.144018583e-6 * pboard->MGetInstClockFreq();
                SpareParts.SetPin(pins[1], 1);
            }
        }

        // trigger
        if ((!pins_[pins[0] - 1].value) && (old_value)) {
            // 200us  8 cycles of 40KHz
            delay = 200e-6 * pboard->MGetInstClockFreq();
            count = 0;
        }
        old_value = pins_[pins[0] - 1].value;
    }
}

void cpart_hcsr04::OnMouseButtonPress(uint inputId, uint button, uint x, uint y, uint state) {
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

void cpart_hcsr04::OnMouseButtonRelease(uint inputId, uint button, uint x, uint y, uint state) {
    switch (inputId) {
        case I_PO1:
            active = 0;
            output_ids[O_PO1]->update = 1;
            break;
    }
}

void cpart_hcsr04::OnMouseMove(uint inputId, uint button, uint x, uint y, uint state) {
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

unsigned short cpart_hcsr04::GetInputId(char* name) {
    if (strcmp(name, "PO_1") == 0)
        return I_PO1;

    printf("Error input '%s' don't have a valid id! \n", name);
    return INVALID_ID;
}

unsigned short cpart_hcsr04::GetOutputId(char* name) {
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

    printf("Error output '%s' don't have a valid id! \n", name);
    return INVALID_ID;
}

std::string cpart_hcsr04::WritePreferences(void) {
    char prefs[256];

    sprintf(prefs, "%hhu,%hhu,%hhu", pins[0], pins[1], value);

    return prefs;
}

void cpart_hcsr04::ReadPreferences(std::string value_) {
    sscanf(value_.c_str(), "%hhu,%hhu,%hhu", &pins[0], &pins[1], &value);
}

void cpart_hcsr04::ConfigurePropertiesWindow(CPWindow* WProp) {
    SetPCWComboWithPinNames(WProp, "combo2", pins[0]);
    SetPCWComboWithPinNames(WProp, "combo3", pins[1]);
}

void cpart_hcsr04::ReadPropertiesWindow(CPWindow* WProp) {
    pins[0] = GetPWCComboSelectedPin(WProp, "combo2");
    pins[1] = GetPWCComboSelectedPin(WProp, "combo3");
}

part_init(PART_HCSR0404_Name, cpart_hcsr04, "Input");
