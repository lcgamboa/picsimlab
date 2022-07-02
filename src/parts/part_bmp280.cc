/* ########################################################################

   PICsimLab - PIC laboratory simulator

   ########################################################################

   Copyright (c) : 2020-2022  Luis Claudio Gambôa Lopes

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

#include "part_bmp280.h"
#include "../picsimlab1.h"
#include "../picsimlab4.h"
#include "../picsimlab5.h"

/* outputs */
enum { O_P1, O_P2, O_PF1, O_PF2, O_PO1, O_PO2 };

/* inputs */
enum { I_PO1, I_PO2 };

const char pin_names[4][10] = {"SCL", "SDA", "VCC", "GND"};
const char pin_values[4][10] = {{0}, {1}, "+5V", "GND"};

cpart_bmp280::cpart_bmp280(unsigned x, unsigned y)
    : font(8, lxFONTFAMILY_TELETYPE, lxFONTSTYLE_NORMAL, lxFONTWEIGHT_BOLD),
      font_p(6, lxFONTFAMILY_TELETYPE, lxFONTSTYLE_NORMAL, lxFONTWEIGHT_BOLD) {
    X = x;
    Y = y;
    ReadMaps();
    Bitmap = NULL;

    LoadImage();

    sen_bmp280_init(&bmp280);
    sen_bmp280_rst(&bmp280);

    input_pins[0] = 0;
    input_pins[1] = 0;

    values[0] = 0;
    values[1] = 0;
    active[0] = 0;
    active[0] = 0;
}

cpart_bmp280::~cpart_bmp280(void) {
    sen_bmp280_end(&bmp280);
    delete Bitmap;
    canvas.Destroy();
}

void cpart_bmp280::Draw(void) {
    int i;
    char val[10];

    Update = 0;

    for (i = 0; i < outputc; i++) {
        if (output[i].update)  // only if need update
        {
            output[i].update = 0;

            if (!Update) {
                canvas.Init(Scale, Scale, Orientation);
                canvas.SetFont(font);
            }
            Update++;  // set to update buffer

            switch (output[i].id) {
                case O_PO1:
                    snprintf(val, 10, "%6.0f", (4.0 * (200 - values[0]) + 300));
                    draw_slider(&output[i], values[0], val, font_p);
                    canvas.SetFont(font);
                    break;
                case O_PO2:
                    snprintf(val, 10, "%5.1f", (0.625 * (200 - values[1]) - 40));
                    draw_slider(&output[i], values[1], val, font_p);
                    canvas.SetFont(font);
                    break;
                default:
                    canvas.SetColor(49, 61, 99);
                    canvas.Rectangle(1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1,
                                     output[i].y2 - output[i].y1);

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
                            canvas.RotatedText(Window5.GetPinName(input_pins[pinv]), output[i].x1, output[i].y2 - 30,
                                               90.0);
                    }
                    break;
            }
        }
    }

    if (Update) {
        canvas.End();
    }
}

unsigned short cpart_bmp280::get_in_id(char* name) {
    if (strcmp(name, "PO_1") == 0)
        return I_PO1;
    if (strcmp(name, "PO_2") == 0)
        return I_PO2;

    printf("Erro input '%s' don't have a valid id! \n", name);
    return -1;
}

unsigned short cpart_bmp280::get_out_id(char* name) {
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

    printf("Erro output '%s' don't have a valid id! \n", name);
    return 1;
}

lxString cpart_bmp280::WritePreferences(void) {
    char prefs[256];

    sprintf(prefs, "%hhu,%hhu,%hhu,%hhu", input_pins[0], input_pins[1], values[0], values[1]);

    return prefs;
}

void cpart_bmp280::ReadPreferences(lxString value_) {
    sscanf(value_.c_str(), "%hhu,%hhu,%hhu,%hhu", &input_pins[0], &input_pins[1], &values[0], &values[1]);
    Reset();
}

void cpart_bmp280::ConfigurePropertiesWindow(CPWindow* WProp) {
    lxString Items = Window5.GetPinsNames();
    lxString spin;

    ((CCombo*)WProp->GetChildByName("combo1"))->SetItems(Items);
    if (input_pins[0] == 0)
        ((CCombo*)WProp->GetChildByName("combo1"))->SetText("0  NC");
    else {
        spin = Window5.GetPinName(input_pins[0]);
        ((CCombo*)WProp->GetChildByName("combo1"))->SetText(itoa(input_pins[0]) + "  " + spin);
    }

    ((CCombo*)WProp->GetChildByName("combo2"))->SetItems(Items);
    if (input_pins[1] == 0)
        ((CCombo*)WProp->GetChildByName("combo2"))->SetText("0  NC");
    else {
        spin = Window5.GetPinName(input_pins[1]);
        ((CCombo*)WProp->GetChildByName("combo2"))->SetText(itoa(input_pins[1]) + "  " + spin);
    }

    ((CButton*)WProp->GetChildByName("button1"))->EvMouseButtonRelease =
        EVMOUSEBUTTONRELEASE & CPWindow5::PropButtonRelease;
    ((CButton*)WProp->GetChildByName("button1"))->SetTag(1);

    ((CButton*)WProp->GetChildByName("button2"))->EvMouseButtonRelease =
        EVMOUSEBUTTONRELEASE & CPWindow5::PropButtonRelease;
}

void cpart_bmp280::ReadPropertiesWindow(CPWindow* WProp) {
    input_pins[0] = atoi(((CCombo*)WProp->GetChildByName("combo1"))->GetText());
    input_pins[1] = atoi(((CCombo*)WProp->GetChildByName("combo2"))->GetText());
}

void cpart_bmp280::PreProcess(void) {
    if ((input_pins[0] > 0) && (input_pins[1] > 0)) {
        sen_bmp280_setPressTemp(&bmp280, (4.0 * (200 - values[0]) + 300), (0.625 * (200 - values[1]) - 40));
        Window5.Reset_pullup_bus(input_pins[1] - 1);
    }
}

void cpart_bmp280::Process(void) {
    const picpin* ppins = Window5.GetPinsValues();

    if ((input_pins[0] > 0) && (input_pins[1] > 0))
        Window5.Set_pullup_bus(input_pins[1] - 1, sen_bmp280_I2C_io(&bmp280, ppins[input_pins[0] - 1].value,
                                                                    ppins[input_pins[1] - 1].value));
}

void cpart_bmp280::EvMouseButtonPress(uint button, uint x, uint y, uint state) {
    int i;

    for (i = 0; i < inputc; i++) {
        if (PointInside(x, y, input[i])) {
            RotateCoords(&x, &y);
            switch (input[i].id) {
                case I_PO1:
                    values[0] = (y - input[i].y1) * 1.66;
                    if (values[0] > 200)
                        values[0] = 200;
                    active[0] = 1;
                    output_ids[O_PO1]->update = 1;
                    break;
                case I_PO2:
                    values[1] = (y - input[i].y1) * 1.66;
                    if (values[1] > 200)
                        values[1] = 200;
                    active[1] = 1;
                    output_ids[O_PO2]->update = 1;
                    break;
            }
        }
    }
}

void cpart_bmp280::EvMouseButtonRelease(uint button, uint x, uint y, uint state) {
    int i;

    for (i = 0; i < inputc; i++) {
        if (PointInside(x, y, input[i])) {
            switch (input[i].id) {
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
    }
}

void cpart_bmp280::EvMouseMove(uint button, uint x, uint y, uint state) {
    int i;

    for (i = 0; i < inputc; i++) {
        if (PointInside(x, y, input[i])) {
            RotateCoords(&x, &y);

            switch (input[i].id) {
                case I_PO1:
                    if (active[0]) {
                        values[0] = (y - input[i].y1) * 1.66;
                        if (values[0] > 200)
                            values[0] = 200;
                        output_ids[O_PO1]->update = 1;
                    }
                    break;
                case I_PO2:
                    if (active[1]) {
                        values[1] = (y - input[i].y1) * 1.66;
                        if (values[1] > 200)
                            values[1] = 200;
                        output_ids[O_PO2]->update = 1;
                    }
                    break;
            }
        } else {
            active[i] = 0;
        }
    }
}

part_init(PART_BMP280_Name, cpart_bmp280, "Input");
