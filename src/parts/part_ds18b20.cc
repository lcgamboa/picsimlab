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

#include "part_ds18b20.h"
#include "../picsimlab1.h"
#include "../picsimlab4.h"
#include "../picsimlab5.h"

/* outputs */
enum { O_P1, O_F1, O_F2, O_PO1 };

/* inputs */
enum { I_PO1, I_PO2 };

cpart_ds18b20::cpart_ds18b20(unsigned x, unsigned y)
    : font(9, lxFONTFAMILY_TELETYPE, lxFONTSTYLE_NORMAL, lxFONTWEIGHT_BOLD),
      font_p(7, lxFONTFAMILY_TELETYPE, lxFONTSTYLE_NORMAL, lxFONTWEIGHT_BOLD) {
    X = x;
    Y = y;
    ReadMaps();

    LoadImage();

    output_pins[0] = 0;

    values[0] = 0;

    active[0] = 0;

    RegisterRemoteControl();

    sen_ds18b20_init(&ds18b20, Window1.GetBoard());
}

void cpart_ds18b20::RegisterRemoteControl(void) {
    for (int i = 0; i < inputc; i++) {
        switch (input[i].id) {
            case I_PO1:
                input[i].status = &values[0];
                break;
            case I_PO2:
                input[i].status = &values[1];
                break;
        }
    }
}

cpart_ds18b20::~cpart_ds18b20(void) {
    sen_ds18b20_end(&ds18b20);
    delete Bitmap;
    canvas.Destroy();
}

void cpart_ds18b20::Draw(void) {
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
                case O_P1:
                    canvas.SetColor(49, 61, 99);
                    canvas.Rectangle(1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1,
                                     output[i].y2 - output[i].y1);
                    canvas.SetFgColor(255, 255, 255);
                    if (output_pins[output[i].id - O_P1] == 0)
                        canvas.RotatedText("NC", output[i].x1, output[i].y2, 90);
                    else
                        canvas.RotatedText(Window5.GetPinName(output_pins[output[i].id - O_P1]), output[i].x1,
                                           output[i].y2, 90);
                    break;
                case O_F1:
                    canvas.SetColor(49, 61, 99);
                    canvas.Rectangle(1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1,
                                     output[i].y2 - output[i].y1);
                    canvas.SetFgColor(155, 155, 155);
                    canvas.RotatedText("GND", output[i].x1, output[i].y2, 90);
                    break;
                case O_F2:
                    canvas.SetColor(49, 61, 99);
                    canvas.Rectangle(1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1,
                                     output[i].y2 - output[i].y1);
                    canvas.SetFgColor(155, 155, 155);
                    canvas.RotatedText("+5V", output[i].x1, output[i].y2, 90);
                    break;
                case O_PO1:
                    snprintf(val, 10, "%5.1f", (0.6 * (200 - values[0]) - 40));
                    draw_slider(&output[i], values[0], val, font_p);
                    canvas.SetFont(font);
                    break;
            }
        }
    }

    if (Update) {
        canvas.End();
    }
}
void cpart_ds18b20::Reset(void) {
    sen_ds18b20_rst(&ds18b20);
    if (output_pins[0]) {
        Window5.SetPin(output_pins[0], 0);
    }
}

void cpart_ds18b20::PreProcess(void) {
    if (output_pins[0]) {
        sen_ds18b20_setTemp(&ds18b20, (0.6 * (200 - values[0]) - 40));
        Window5.Reset_pullup_bus(output_pins[0] - 1);
    }
}

void cpart_ds18b20::Process(void) {
    if (output_pins[0]) {
        const picpin* ppins = Window5.GetPinsValues();

        if ((ppins[output_pins[0] - 1].dir) && (ppins[output_pins[0] - 1].value != ds18b20.out)) {
            Window5.SetPin(output_pins[0], ds18b20.out);
        }

        Window5.Set_pullup_bus(output_pins[0] - 1, sen_ds18b20_io(&ds18b20, ppins[output_pins[0] - 1].value));
    }
}

void cpart_ds18b20::EvMouseButtonPress(uint button, uint x, uint y, uint state) {
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
            }
        }
    }
}

void cpart_ds18b20::EvMouseButtonRelease(uint button, uint x, uint y, uint state) {
    int i;

    for (i = 0; i < inputc; i++) {
        if (PointInside(x, y, input[i])) {
            switch (input[i].id) {
                case I_PO1:
                    active[0] = 0;
                    output_ids[O_PO1]->update = 1;
                    break;
            }
        }
    }
}

void cpart_ds18b20::EvMouseMove(uint button, uint x, uint y, uint state) {
    int i;

    for (i = 0; i < inputc; i++) {
        if (PointInside(x, y, input[i])) {
            RotateCoords(&x, &y);

            if (active[input[i].id - I_PO1]) {
                values[input[i].id - I_PO1] = ((y - input[i].y1) * 1.66);
                if (values[input[i].id - I_PO1] > 200)
                    values[input[i].id - I_PO1] = 200;
                output_ids[O_PO1 + input[i].id - I_PO1]->update = 1;
            }
        } else {
            active[input[i].id - I_PO1] = 0;
        }
    }
}

unsigned short cpart_ds18b20::get_in_id(char* name) {
    if (strcmp(name, "PO_1") == 0)
        return I_PO1;
    if (strcmp(name, "PO_2") == 0)
        return I_PO2;

    printf("Erro input '%s' don't have a valid id! \n", name);
    return -1;
}

unsigned short cpart_ds18b20::get_out_id(char* name) {
    if (strcmp(name, "PN_1") == 0)
        return O_P1;

    if (strcmp(name, "PN_F1") == 0)
        return O_F1;
    if (strcmp(name, "PN_F2") == 0)
        return O_F2;

    if (strcmp(name, "PO_1") == 0)
        return O_PO1;

    printf("Erro output '%s' don't have a valid id! \n", name);
    return 1;
}

lxString cpart_ds18b20::WritePreferences(void) {
    char prefs[256];

    sprintf(prefs, "%hhu,%hhu,%" SCNu64, output_pins[0], values[0], sen_ds18b20_get_addr(&ds18b20));

    return prefs;
}

void cpart_ds18b20::ReadPreferences(lxString value_) {
    uint64_t addr;
    sscanf(value_.c_str(), "%hhu,%hhu,%" SCNu64, &output_pins[0], &values[0], &addr);
    sen_ds18b20_set_addr(&ds18b20, addr);
}

void cpart_ds18b20::ConfigurePropertiesWindow(CPWindow* WProp) {
    lxString Items = Window5.GetPinsNames();
    lxString spin;

    ((CCombo*)WProp->GetChildByName("combo1"))->SetItems(Items);
    if (output_pins[0] == 0)
        ((CCombo*)WProp->GetChildByName("combo1"))->SetText("0  NC");
    else {
        spin = Window5.GetPinName(output_pins[0]);
        ((CCombo*)WProp->GetChildByName("combo1"))->SetText(itoa(output_pins[0]) + "  " + spin);
    }

    ((CButton*)WProp->GetChildByName("button1"))->EvMouseButtonRelease =
        EVMOUSEBUTTONRELEASE & CPWindow5::PropButtonRelease;
    ((CButton*)WProp->GetChildByName("button1"))->SetTag(1);

    ((CButton*)WProp->GetChildByName("button2"))->EvMouseButtonRelease =
        EVMOUSEBUTTONRELEASE & CPWindow5::PropButtonRelease;
}

void cpart_ds18b20::ReadPropertiesWindow(CPWindow* WProp) {
    output_pins[0] = atoi(((CCombo*)WProp->GetChildByName("combo1"))->GetText());
}

part_init(PART_DHT22_Name, cpart_ds18b20, "Input");
