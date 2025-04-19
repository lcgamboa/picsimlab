/* ########################################################################

   PICSimLab - Programmable IC Simulator Laboratory

   ########################################################################

   Copyright (c) : 2020-2025  Luis Claudio Gambôa Lopes <lcgamboa@yahoo.com>

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

#include "input_bmp280.h"
#include "../lib/oscilloscope.h"
#include "../lib/picsimlab.h"
#include "../lib/spareparts.h"

/* outputs */
enum { O_PF1, O_PF2, O_P1, O_P2, O_P3, O_P4, O_PO1, O_PO2 };

/* inputs */
enum { I_PO1, I_PO2 };

const char pin_names[6][10] = {"VCC", "GND", "SCL", "SDA", "CSB", "SDO"};
const char pin_values[6][10] = {"+5V", "GND", {0}, {1}, {2}, {3}};

static PCWProp pcwprop[7] = {
    {PCW_LABEL, "1-VCC,+5V"}, {PCW_LABEL, "2-GND,GND"}, {PCW_COMBO, "3-SCL"}, {PCW_COMBO, "4-SDA"},
    {PCW_COMBO, "5-CSB"},     {PCW_COMBO, "6-SDO"},     {PCW_END, ""}};

cpart_bmp280::cpart_bmp280(const unsigned x, const unsigned y, const char* name, const char* type, board* pboard_,
                           const int id_)
    : part(x, y, name, type, pboard_, id_) {
    sen_bmp280_init(&bmp280);
    sen_bmp280_rst(&bmp280);

    input_pins[0] = 0;
    input_pins[1] = 0;
    input_pins[2] = 0;

    output_pins[0] = 0;

    values[0] = 0;
    values[1] = 0;
    active[0] = 0;
    active[1] = 0;

    SetPCWProperties(pcwprop);

    PinCount = 3;
    Pins = input_pins;
}

cpart_bmp280::~cpart_bmp280(void) {
    sen_bmp280_end(&bmp280);
    SpareParts.SetPartOnDraw(id);
    SpareParts.CanvasCmd({.cmd = CC_FREEBITMAP, .FreeBitmap{BitmapId}});
    SpareParts.CanvasCmd({.cmd = CC_DESTROY});
}

void cpart_bmp280::DrawOutput(const unsigned int i) {
    char val[10];

    switch (output[i].id) {
        case O_PO1:
            snprintf(val, 10, "%6.0f", (4.0 * (200 - values[0]) + 300));
            DrawSlider(SpareParts.CanvasCmd, &output[i], values[0], val, 6);
            SpareParts.CanvasCmd({.cmd = CC_SETFONTSIZE, .SetFontSize{8}});
            break;
        case O_PO2:
            snprintf(val, 10, "%5.1f", (0.625 * (200 - values[1]) - 40));
            DrawSlider(SpareParts.CanvasCmd, &output[i], values[1], val, 6);
            SpareParts.CanvasCmd({.cmd = CC_SETFONTSIZE, .SetFontSize{8}});
            break;
        default:
            SpareParts.CanvasCmd({.cmd = CC_SETCOLOR, .SetColor{49, 61, 99}});
            SpareParts.CanvasCmd(
                {.cmd = CC_RECTANGLE,
                 .Rectangle{1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1}});

            SpareParts.CanvasCmd({.cmd = CC_SETFGCOLOR, .SetFgColor{255, 255, 255}});
            SpareParts.CanvasCmd({.cmd = CC_ROTATEDTEXT,
                                  .RotatedText{pin_names[output[i].id - O_PF1], output[i].x1, output[i].y2, 90.0}});

            int pinv = pin_values[output[i].id - O_PF1][0];
            if (pinv > 10) {
                SpareParts.CanvasCmd({.cmd = CC_SETFGCOLOR, .SetFgColor{155, 155, 155}});
                SpareParts.CanvasCmd(
                    {.cmd = CC_ROTATEDTEXT,
                     .RotatedText{pin_values[output[i].id - O_PF1], output[i].x1, output[i].y2 - 30, 90.0}});
            } else if (pinv < 3) {
                if (input_pins[pinv] == 0)
                    SpareParts.CanvasCmd(
                        {.cmd = CC_ROTATEDTEXT, .RotatedText{"NC", output[i].x1, output[i].y2 - 30, 90.0}});
                else
                    SpareParts.CanvasCmd({.cmd = CC_ROTATEDTEXT,
                                          .RotatedText{SpareParts.GetPinName(input_pins[pinv]).c_str(), output[i].x1,
                                                       output[i].y2 - 30, 90.0}});
            } else {
                if (output_pins[pinv - 3] == 0)
                    SpareParts.CanvasCmd(
                        {.cmd = CC_ROTATEDTEXT, .RotatedText{"NC", output[i].x1, output[i].y2 - 30, 90.0}});
                else
                    SpareParts.CanvasCmd({.cmd = CC_ROTATEDTEXT,
                                          .RotatedText{SpareParts.GetPinName(output_pins[pinv - 3]).c_str(),
                                                       output[i].x1, output[i].y2 - 30, 90.0}});
            }
            break;
    }
}

unsigned short cpart_bmp280::GetInputId(char* name) {
    if (strcmp(name, "PO_1") == 0)
        return I_PO1;
    if (strcmp(name, "PO_2") == 0)
        return I_PO2;

    printf("Error input '%s' don't have a valid id! \n", name);
    return INVALID_ID;
}

unsigned short cpart_bmp280::GetOutputId(char* name) {
    if (strcmp(name, "PN_1") == 0)
        return O_P1;
    if (strcmp(name, "PN_2") == 0)
        return O_P2;
    if (strcmp(name, "PN_3") == 0)
        return O_P3;
    if (strcmp(name, "PN_4") == 0)
        return O_P4;
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

std::string cpart_bmp280::WritePreferences(void) {
    char prefs[256];

    sprintf(prefs, "%hhu,%hhu,%hhu,%hhu,%hhu,%hhu", input_pins[0], input_pins[1], input_pins[2], output_pins[0],
            values[0], values[1]);

    return prefs;
}

void cpart_bmp280::ReadPreferences(std::string value_) {
    sscanf(value_.c_str(), "%hhu,%hhu,%hhu,%hhu,%hhu,%hhu", &input_pins[0], &input_pins[1], &input_pins[2],
           &output_pins[0], &values[0], &values[1]);
    Reset();
}

void cpart_bmp280::ConfigurePropertiesWindow(void) {
    SetPCWComboWithPinNames("combo3", input_pins[0]);
    SetPCWComboWithPinNames("combo4", input_pins[1]);
    SetPCWComboWithPinNames("combo5", input_pins[2]);
    SetPCWComboWithPinNames("combo6", output_pins[0]);
}

void cpart_bmp280::ReadPropertiesWindow(void) {
    input_pins[0] = GetPWCComboSelectedPin("combo3");
    input_pins[1] = GetPWCComboSelectedPin("combo4");
    input_pins[2] = GetPWCComboSelectedPin("combo5");
    output_pins[0] = GetPWCComboSelectedPin("combo6");
}

void cpart_bmp280::PreProcess(void) {
    const picpin* ppins = SpareParts.GetPinsValues();
    sen_bmp280_setPressTemp(&bmp280, (4.0 * (200 - values[0]) + 300), (0.625 * (200 - values[1]) - 40));
    if ((bmp280.i2c_mode) && (input_pins[2]) && (ppins[input_pins[2] - 1].value)) {
        unsigned char addr = 0x77;

        if (output_pins[0]) {
            if (ppins[output_pins[0] - 1].value == 0)
                addr = 0x76;
        }

        sen_bmp280_set_addr(&bmp280, addr);

        if (input_pins[1]) {
            SpareParts.ResetPullupBus(input_pins[1] - 1);
        }
    }
}

void cpart_bmp280::Process(void) {
    const picpin* ppins = SpareParts.GetPinsValues();

    if (input_pins[0] && input_pins[1] && (input_pins[2])) {
        if ((bmp280.i2c_mode) && (ppins[input_pins[2] - 1].value)) {  // I2C mode
            SpareParts.SetPullupBus(input_pins[1] - 1, sen_bmp280_I2C_io(&bmp280, ppins[input_pins[0] - 1].value,
                                                                         ppins[input_pins[1] - 1].value));
        } else {  // SPI mode
            unsigned char ret = sen_bmp280_io_SPI(&bmp280, ppins[input_pins[1] - 1].value,
                                                  ppins[input_pins[0] - 1].value, ppins[input_pins[2] - 1].value);

            if ((output_pins[0])) {
                if (!ppins[input_pins[2] - 1].value)  // if SS is active, update output
                {
                    if (ret_ != ret) {
                        SpareParts.SetPin(output_pins[0], ret);
                    }
                    ret_ = ret;
                } else {
                    ret_ = 0xFF;  // invalid value
                }
            }
        }
    }
}

void cpart_bmp280::OnMouseButtonPress(unsigned int inputId, unsigned int button, unsigned int x, unsigned int y,
                                      unsigned int state) {
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

void cpart_bmp280::OnMouseButtonRelease(unsigned int inputId, unsigned int button, unsigned int x, unsigned int y,
                                        unsigned int state) {
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

void cpart_bmp280::OnMouseMove(unsigned int inputId, unsigned int button, unsigned int x, unsigned int y,
                               unsigned int state) {
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

part_init(PART_BMP280_Name, cpart_bmp280, "Input");
