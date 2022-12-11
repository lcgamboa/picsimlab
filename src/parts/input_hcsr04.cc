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

#include "input_hcsr04.h"
#include "../oscilloscope.h"
#include "../picsimlab.h"
#include "../spareparts.h"

/* outputs */
enum { O_P1, O_P2, O_F1, O_F2, O_PO1 };

/* inputs */
enum { I_PO1 };

static PCWProp pcwprop[4] = {{PCW_LABEL, "1 - VCC,+5V"},
                             {PCW_COMBO, "2 - TRIG"},
                             {PCW_COMBO, "3 - ECHO"},
                             {PCW_LABEL, "4 - GND ,GND"}};

cpart_hcsr04::cpart_hcsr04(const unsigned x, const unsigned y, const char* name, const char* type)
    : part(x, y, name, type),
      font(9, lxFONTFAMILY_TELETYPE, lxFONTSTYLE_NORMAL, lxFONTWEIGHT_BOLD),
      font_p(7, lxFONTFAMILY_TELETYPE, lxFONTSTYLE_NORMAL, lxFONTWEIGHT_BOLD) {
    aways_update = 1;

    input_pins[0] = 0;

    output_pins[0] = 0;

    value = 0;
    active = 0;
    old_value = 0;
    delay = 0;
    count = 0;

    SetPCWProperties(pcwprop, 4);
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
            canvas.SetColor(49, 61, 99);
            canvas.Rectangle(1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);
            canvas.SetFgColor(255, 255, 255);
            if (input_pins[output[i].id - O_P1] == 0)
                canvas.RotatedText("NC", output[i].x1, output[i].y2, 90);
            else
                canvas.RotatedText(SpareParts.GetPinName(input_pins[output[i].id - O_P1]), output[i].x1, output[i].y2,
                                   90);
            break;
        case O_P2:
            canvas.SetColor(49, 61, 99);
            canvas.Rectangle(1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);
            canvas.SetFgColor(255, 255, 255);
            if (output_pins[output[i].id - O_P2] == 0)
                canvas.RotatedText("NC", output[i].x1, output[i].y2, 90);
            else
                canvas.RotatedText(SpareParts.GetPinName(output_pins[output[i].id - O_P2]), output[i].x1, output[i].y2,
                                   90);
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
    const picpin* pins = SpareParts.GetPinsValues();

    if ((input_pins[0]) && (output_pins[0])) {
        // output
        if (count) {
            count--;
            if (!count) {
                SpareParts.SetPin(output_pins[0], 0);
            }
        }

        // pulse 48 kHz
        if (delay) {
            delay--;
            if (!delay) {
                count = (200 - value) * 116.144018583e-6 * PICSimLab.GetBoard()->MGetInstClockFreq();
                SpareParts.SetPin(output_pins[0], 1);
            }
        }

        // trigger
        if ((!pins[input_pins[0] - 1].value) && (old_value)) {
            // 200us  8 cycles of 40KHz
            delay = 200e-6 * PICSimLab.GetBoard()->MGetInstClockFreq();
            count = 0;
        }
        old_value = pins[input_pins[0] - 1].value;
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

    printf("Erro input '%s' don't have a valid id! \n", name);
    return -1;
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

    printf("Erro output '%s' don't have a valid id! \n", name);
    return 1;
}

lxString cpart_hcsr04::WritePreferences(void) {
    char prefs[256];

    sprintf(prefs, "%hhu,%hhu,%hhu", input_pins[0], output_pins[0], value);

    return prefs;
}

void cpart_hcsr04::ReadPreferences(lxString value_) {
    sscanf(value_.c_str(), "%hhu,%hhu,%hhu", &input_pins[0], &output_pins[0], &value);
}

void cpart_hcsr04::ConfigurePropertiesWindow(CPWindow* WProp) {
    SetPCWComboWithPinNames(WProp, "combo2", input_pins[0]);
    SetPCWComboWithPinNames(WProp, "combo3", output_pins[0]);
}

void cpart_hcsr04::ReadPropertiesWindow(CPWindow* WProp) {
    input_pins[0] = GetPWCComboSelectedPin(WProp, "combo2");
    output_pins[0] = GetPWCComboSelectedPin(WProp, "combo3");
}

part_init(PART_HCSR0404_Name, cpart_hcsr04, "Input");
