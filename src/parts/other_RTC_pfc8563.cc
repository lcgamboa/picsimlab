/* ########################################################################

   PICSimLab - Programmable IC Simulator Laboratory

   ########################################################################

   Copyright (c) : 2019-2023  Luis Claudio Gambôa Lopes <lcgamboa@yahoo.com>

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

#include "other_RTC_pfc8563.h"
#include "../lib/oscilloscope.h"
#include "../lib/picsimlab.h"
#include "../lib/spareparts.h"

/* outputs */
enum { O_P1, O_P2, O_P3, O_P4, O_P5, O_P6, O_P7, O_P8, O_IC };

const char pin_names[8][10] = {"OSI", "OSO", "INT", "VSS", "SDA", "SCL", "CLK", "VCC"};
const char pin_values[8][10] = {"OSC1", "OSC2", {0}, "GND", {1}, {2}, {3}, "+5V"};

static PCWProp pcwprop[9] = {{PCW_LABEL, "P1 - OSI,OSC1"}, {PCW_LABEL, "P2 - OSO,OSC2"}, {PCW_COMBO, "P3 - INT"},
                             {PCW_LABEL, "P4 - VSS,GND"},  {PCW_COMBO, "P5 - SDA"},      {PCW_COMBO, "P6 - SCL"},
                             {PCW_COMBO, "P7 - CLK"},      {PCW_LABEL, "P8 - VCC,+5V"},  {PCW_END, ""}};

cpart_RTC_pfc8563::cpart_RTC_pfc8563(const unsigned x, const unsigned y, const char* name, const char* type)
    : part(x, y, name, type, 8), font_p(6, lxFONTFAMILY_TELETYPE, lxFONTSTYLE_NORMAL, lxFONTWEIGHT_BOLD) {
    rtc_pfc8563_init(&rtc);
    rtc_pfc8563_rst(&rtc);

    input_pins[0] = 0;
    input_pins[1] = 0;
    input_pins[2] = 0;
    input_pins[3] = 0;

    SetPCWProperties(pcwprop);

    PinCount = 4;
    Pins = input_pins;
}

cpart_RTC_pfc8563::~cpart_RTC_pfc8563(void) {
    rtc_pfc8563_end(&rtc);
    delete Bitmap;
    canvas.Destroy();
}

void cpart_RTC_pfc8563::DrawOutput(const unsigned int i) {
    switch (output[i].id) {
        case O_IC:
            canvas.SetFont(font_p);
            canvas.SetColor(26, 26, 26);
            canvas.Rectangle(1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);
            canvas.SetFgColor(255, 255, 255);
            canvas.RotatedText("PFC8563", output[i].x1, output[i].y2 - 15, 0.0);
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

unsigned short cpart_RTC_pfc8563::GetInputId(char* name) {
    printf("Erro input '%s' don't have a valid id! \n", name);
    return -1;
};

unsigned short cpart_RTC_pfc8563::GetOutputId(char* name) {
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

    printf("Erro output '%s' don't have a valid id! \n", name);
    return 1;
};

lxString cpart_RTC_pfc8563::WritePreferences(void) {
    char prefs[256];

    sprintf(prefs, "%hhu,%hhu,%hhu,%hhu", input_pins[0], input_pins[1], input_pins[2], input_pins[3]);

    return prefs;
}

void cpart_RTC_pfc8563::ReadPreferences(lxString value) {
    sscanf(value.c_str(), "%hhu,%hhu,%hhu,%hhu", &input_pins[0], &input_pins[1], &input_pins[2], &input_pins[3]);
    Reset();
}

void cpart_RTC_pfc8563::ConfigurePropertiesWindow(CPWindow* WProp) {
    SetPCWComboWithPinNames(WProp, "combo3", input_pins[0]);
    SetPCWComboWithPinNames(WProp, "combo5", input_pins[1]);
    SetPCWComboWithPinNames(WProp, "combo6", input_pins[2]);
    SetPCWComboWithPinNames(WProp, "combo7", input_pins[3]);
}

void cpart_RTC_pfc8563::ReadPropertiesWindow(CPWindow* WProp) {
    input_pins[0] = GetPWCComboSelectedPin(WProp, "combo3");
    input_pins[1] = GetPWCComboSelectedPin(WProp, "combo5");
    input_pins[2] = GetPWCComboSelectedPin(WProp, "combo6");
    input_pins[3] = GetPWCComboSelectedPin(WProp, "combo7");
}

void cpart_RTC_pfc8563::PreProcess(void) {
    if (input_pins[1] > 0) {
        SpareParts.ResetPullupBus(input_pins[1] - 1);
    }
    rtc_pfc8563_update(&rtc);
}

void cpart_RTC_pfc8563::Process(void) {
    const picpin* ppins = SpareParts.GetPinsValues();

    if ((input_pins[1] > 0) && (input_pins[2] > 0))
        SpareParts.SetPullupBus(input_pins[1] - 1, rtc_pfc8563_I2C_io(&rtc, ppins[input_pins[2] - 1].value,
                                                                      ppins[input_pins[1] - 1].value));
}

part_init(PART_RTC_PFC8563_Name, cpart_RTC_pfc8563, "Other");
