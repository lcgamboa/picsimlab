/* ########################################################################

   PICSimLab - Programmable IC Simulator Laboratory

   ########################################################################

   Copyright (c) : 2019-2024  Luis Claudio Gambôa Lopes <lcgamboa@yahoo.com>

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

#include "other_RTC_ds1307.h"
#include "../lib/oscilloscope.h"
#include "../lib/picsimlab.h"
#include "../lib/spareparts.h"

/* outputs */
enum { O_P1, O_P2, O_P3, O_P4, O_P5, O_P6, O_P7, O_P8, O_IC };

const char pin_names[8][10] = {"X1", "X2", "Vbat", "GND", "SDA", "SCL", "SQW", "VCC"};
const char pin_values[8][10] = {"OSC1", "OSC2", "BAT", "GND", {0}, {1}, {2}, "+5V"};

static PCWProp pcwprop[9] = {{PCW_LABEL, "P1 - X1,OSC1"}, {PCW_LABEL, "P2 - X2,OSC2"}, {PCW_LABEL, "P3 - Vbat,BAT"},
                             {PCW_LABEL, "P4 - GND,GND"}, {PCW_COMBO, "P5 - SDA"},     {PCW_COMBO, "P6 - SCL"},
                             {PCW_COMBO, "P7 - SQW"},     {PCW_LABEL, "P8 - VCC,+5V"}, {PCW_END, ""}};

cpart_RTC_ds1307::cpart_RTC_ds1307(const unsigned x, const unsigned y, const char* name, const char* type,
                                   board* pboard_, const int id_)
    : part(x, y, name, type, pboard_, id_) {
    rtc_ds1307_init(&rtc2, pboard);
    rtc_ds1307_rst(&rtc2);
    input_pins[0] = 0;
    input_pins[1] = 0;
    input_pins[2] = 0;

    SetPCWProperties(pcwprop);

    PinCount = 3;
    Pins = input_pins;
}

cpart_RTC_ds1307::~cpart_RTC_ds1307(void) {
    rtc_ds1307_end(&rtc2);
    SpareParts.SetPartOnDraw(id);
    SpareParts.CanvasCmd({CC_FREEBITMAP, .FreeBitmap{BitmapId}});
    SpareParts.CanvasCmd({CC_DESTROY});
}

void cpart_RTC_ds1307::DrawOutput(const unsigned int i) {
    switch (output[i].id) {
        case O_IC:
            SpareParts.CanvasCmd({CC_SETFONTSIZE, .SetFontSize{6}});
            SpareParts.CanvasCmd({CC_SETCOLOR, .SetColor{26, 26, 26}});
            SpareParts.CanvasCmd({CC_RECTANGLE, .Rectangle{1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1,
                                                           output[i].y2 - output[i].y1}});
            SpareParts.CanvasCmd({CC_SETFGCOLOR, .SetFgColor{255, 255, 255}});
            SpareParts.CanvasCmd({CC_ROTATEDTEXT, .RotatedText{"DS1307", output[i].x1, output[i].y2 - 15, 0.0}});
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

unsigned short cpart_RTC_ds1307::GetInputId(char* name) {
    printf("Error input '%s' don't have a valid id! \n", name);
    return INVALID_ID;
};

unsigned short cpart_RTC_ds1307::GetOutputId(char* name) {
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

    printf("Error output '%s' don't have a valid id! \n", name);
    return INVALID_ID;
};

std::string cpart_RTC_ds1307::WritePreferences(void) {
    char prefs[256];

    sprintf(prefs, "%hhu,%hhu,%hhu", input_pins[0], input_pins[1], input_pins[2]);

    return prefs;
}

void cpart_RTC_ds1307::ReadPreferences(std::string value) {
    sscanf(value.c_str(), "%hhu,%hhu,%hhu", &input_pins[0], &input_pins[1], &input_pins[2]);
    Reset();
}

void cpart_RTC_ds1307::ConfigurePropertiesWindow(CPWindow* WProp) {
    SetPCWComboWithPinNames(WProp, "combo5", input_pins[0]);
    SetPCWComboWithPinNames(WProp, "combo6", input_pins[1]);
    SetPCWComboWithPinNames(WProp, "combo7", input_pins[2]);
}

void cpart_RTC_ds1307::ReadPropertiesWindow(CPWindow* WProp) {
    input_pins[0] = GetPWCComboSelectedPin(WProp, "combo5");
    input_pins[1] = GetPWCComboSelectedPin(WProp, "combo6");
    input_pins[2] = GetPWCComboSelectedPin(WProp, "combo7");
}

void cpart_RTC_ds1307::PreProcess(void) {
    if (input_pins[0] > 0) {
        SpareParts.ResetPullupBus(input_pins[0] - 1);
    }
}

void cpart_RTC_ds1307::Process(void) {
    const picpin* ppins = SpareParts.GetPinsValues();

    if ((input_pins[0] > 0) && (input_pins[1] > 0))
        SpareParts.SetPullupBus(input_pins[0] - 1, rtc_ds1307_I2C_io(&rtc2, ppins[input_pins[1] - 1].value,
                                                                     ppins[input_pins[0] - 1].value));
}

part_init(PART_RTC_DS1307_Name, cpart_RTC_ds1307, "Other");
