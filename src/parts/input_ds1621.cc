/* ########################################################################

   PICSimLab - Programmable IC Simulator Laboratory

   ########################################################################

   Copyright (c) : 2020-2023  Luis Claudio Gambôa Lopes <lcgamboa@yahoo.com>

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

#include "input_ds1621.h"
#include "../lib/oscilloscope.h"
#include "../lib/picsimlab.h"
#include "../lib/spareparts.h"

/* outputs */
enum { O_P1, O_P2, O_P3, O_P4, O_P5, O_P6, O_P7, O_P8, O_IC, O_PO1 };

/* inputs */
enum { I_PO1 };

const char pin_names[8][10] = {"SDA", "SCL", "Tout", "GND", "A2", "A1", "A0", "VCC"};
const char pin_values[8][10] = {{0}, {1}, {2}, "GND", {3}, {4}, {5}, "+5V"};

static PCWProp pcwprop[9] = {{PCW_COMBO, "P1 - SDA"},     {PCW_COMBO, "P2 - SCL"},     {PCW_COMBO, "P3 - Tout"},
                             {PCW_LABEL, "P4 - GND,GND"}, {PCW_COMBO, "P5 - A2"},      {PCW_COMBO, "P6 - A1"},
                             {PCW_COMBO, "P7 - A0"},      {PCW_LABEL, "P8 - VCC,+5V"}, {PCW_END, ""}};

cpart_ds1621::cpart_ds1621(const unsigned x, const unsigned y, const char* name, const char* type)
    : part(x, y, name, type),
      font(8, lxFONTFAMILY_TELETYPE, lxFONTSTYLE_NORMAL, lxFONTWEIGHT_BOLD),
      font_p(6, lxFONTFAMILY_TELETYPE, lxFONTSTYLE_NORMAL, lxFONTWEIGHT_BOLD) {
    sen_ds1621_init(&ds1621);
    sen_ds1621_rst(&ds1621);

    input_pins[0] = 0;
    input_pins[1] = 0;
    input_pins[2] = 0;
    input_pins[3] = 0;
    input_pins[4] = 0;
    input_pins[5] = 0;

    value = 0;
    active = 0;

    SetPCWProperties(pcwprop);

    PinCount = 6;
    Pins = input_pins;
}

cpart_ds1621::~cpart_ds1621(void) {
    sen_ds1621_end(&ds1621);
    delete Bitmap;
    canvas.Destroy();
}

void cpart_ds1621::DrawOutput(const unsigned int i) {
    char val[10];

    switch (output[i].id) {
        case O_IC:
            canvas.SetFont(font_p);
            canvas.SetColor(26, 26, 26);
            canvas.Rectangle(1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);
            canvas.SetFgColor(255, 255, 255);
            canvas.RotatedText("DS1621", output[i].x1, output[i].y2 - 15, 0.0);
            break;
        case O_PO1:
            snprintf(val, 10, "%5.1f", (0.9 * (200 - value) - 55));
            DrawSlider(&output[i], value, val, font_p);
            canvas.SetFont(font);
            break;
        default:
            canvas.SetColor(49, 61, 99);
            canvas.Rectangle(1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);

            canvas.SetFgColor(255, 255, 255);
            canvas.RotatedText(pin_names[output[i].id - O_P1], output[i].x1, output[i].y2, 90.0);

            int pinv = pin_values[output[i].id - O_P1][0];
            if (pinv > 10) {
                canvas.SetFgColor(155, 155, 155);
                canvas.RotatedText(pin_values[output[i].id - O_P1], output[i].x1, output[i].y2 - 30, 90.0);
            } else {
                if (input_pins[pinv] == 0)
                    canvas.RotatedText("NC", output[i].x1, output[i].y2 - 30, 90.0);
                else
                    canvas.RotatedText(SpareParts.GetPinName(input_pins[pinv]), output[i].x1, output[i].y2 - 30, 90.0);
            }
            break;
    }
}

unsigned short cpart_ds1621::GetInputId(char* name) {
    if (strcmp(name, "PO_1") == 0)
        return I_PO1;

    printf("Erro input '%s' don't have a valid id! \n", name);
    return -1;
}

unsigned short cpart_ds1621::GetOutputId(char* name) {
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

    if (strcmp(name, "IC_8") == 0)
        return O_IC;

    if (strcmp(name, "PO_1") == 0)
        return O_PO1;

    printf("Erro output '%s' don't have a valid id! \n", name);
    return 1;
}

lxString cpart_ds1621::WritePreferences(void) {
    char prefs[256];

    sprintf(prefs, "%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu", input_pins[0], input_pins[1], input_pins[2], input_pins[3],
            input_pins[4], input_pins[5], value);

    return prefs;
}

void cpart_ds1621::ReadPreferences(lxString value_) {
    sscanf(value_.c_str(), "%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu", &input_pins[0], &input_pins[1], &input_pins[2],
           &input_pins[3], &input_pins[4], &input_pins[5], &value);
    Reset();
}

void cpart_ds1621::ConfigurePropertiesWindow(CPWindow* WProp) {
    SetPCWComboWithPinNames(WProp, "combo1", input_pins[0]);
    SetPCWComboWithPinNames(WProp, "combo2", input_pins[1]);
    SetPCWComboWithPinNames(WProp, "combo3", input_pins[2]);
    SetPCWComboWithPinNames(WProp, "combo5", input_pins[3]);
    SetPCWComboWithPinNames(WProp, "combo6", input_pins[4]);
    SetPCWComboWithPinNames(WProp, "combo7", input_pins[5]);
}

void cpart_ds1621::ReadPropertiesWindow(CPWindow* WProp) {
    input_pins[0] = GetPWCComboSelectedPin(WProp, "combo1");
    input_pins[1] = GetPWCComboSelectedPin(WProp, "combo2");
    input_pins[2] = GetPWCComboSelectedPin(WProp, "combo3");
    input_pins[3] = GetPWCComboSelectedPin(WProp, "combo5");
    input_pins[4] = GetPWCComboSelectedPin(WProp, "combo6");
    input_pins[5] = GetPWCComboSelectedPin(WProp, "combo7");
}

void cpart_ds1621::PreProcess(void) {
    const picpin* ppins = SpareParts.GetPinsValues();
    unsigned char addr = 0x48;

    if (input_pins[5]) {
        if (ppins[input_pins[5] - 1].value)
            addr |= 0x01;
    }
    if (input_pins[4]) {
        if (ppins[input_pins[4] - 1].value)
            addr |= 0x02;
    }
    if (input_pins[3]) {
        if (ppins[input_pins[3] - 1].value)
            addr |= 0x04;
    }

    sen_ds1621_set_addr(&ds1621, addr);

    if (input_pins[0] > 0) {
        sen_ds1621_setTemp(&ds1621, (0.9 * (200 - value) - 55));
        // TODO set addr
        SpareParts.ResetPullupBus(input_pins[0] - 1);
    }
}

void cpart_ds1621::Process(void) {
    const picpin* ppins = SpareParts.GetPinsValues();

    if ((input_pins[0] > 0) && (input_pins[1] > 0))
        SpareParts.SetPullupBus(input_pins[0] - 1, sen_ds1621_I2C_io(&ds1621, ppins[input_pins[1] - 1].value,
                                                                     ppins[input_pins[0] - 1].value));

    // TODO implement Tout output
}

void cpart_ds1621::OnMouseButtonPress(uint inputId, uint button, uint x, uint y, uint state) {
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

void cpart_ds1621::OnMouseButtonRelease(uint inputId, uint button, uint x, uint y, uint state) {
    switch (inputId) {
        case I_PO1:
            active = 0;
            output_ids[O_PO1]->update = 1;
            break;
    }
}

void cpart_ds1621::OnMouseMove(uint inputId, uint button, uint x, uint y, uint state) {
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

part_init(PART_DS1621_Name, cpart_ds1621, "Input");
