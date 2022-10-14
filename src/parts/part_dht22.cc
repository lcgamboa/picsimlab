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

#include "part_dht22.h"
#include "../oscilloscope.h"
#include "../picsimlab.h"
#include "../spareparts.h"

/* outputs */
enum { O_P1, O_F3, O_F1, O_F2, O_PO1, O_PO2 };

/* inputs */
enum { I_PO1, I_PO2 };

cpart_dht22::cpart_dht22(unsigned x, unsigned y)
    : font(9, lxFONTFAMILY_TELETYPE, lxFONTSTYLE_NORMAL, lxFONTWEIGHT_BOLD),
      font_p(7, lxFONTFAMILY_TELETYPE, lxFONTSTYLE_NORMAL, lxFONTWEIGHT_BOLD) {
    X = x;
    Y = y;
    ReadMaps();

    LoadImage();

    output_pins[0] = 0;

    values[0] = 0;
    values[1] = 0;

    active[0] = 0;
    active[1] = 0;

    RegisterRemoteControl();

    sen_dhtxx_init(&dht22, PICSimLab.GetBoard(), DHT22);
}

void cpart_dht22::RegisterRemoteControl(void) {
    input_ids[I_PO1]->status = &values[0];
    input_ids[I_PO1]->update = &output_ids[O_PO1]->update;
    input_ids[I_PO2]->status = &values[1];
    input_ids[I_PO2]->update = &output_ids[O_PO2]->update;
}

cpart_dht22::~cpart_dht22(void) {
    sen_dhtxx_end(&dht22);
    delete Bitmap;
    canvas.Destroy();
}

void cpart_dht22::Draw(void) {
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
                        canvas.RotatedText("2-D  NC", output[i].x1, output[i].y2, 90);
                    else
                        canvas.RotatedText("2-D  " + SpareParts.GetPinName(output_pins[output[i].id - O_P1]),
                                           output[i].x1, output[i].y2, 90);
                    break;
                case O_F1:
                    canvas.SetColor(49, 61, 99);
                    canvas.Rectangle(1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1,
                                     output[i].y2 - output[i].y1);
                    canvas.SetFgColor(155, 155, 155);
                    canvas.RotatedText("1-VCC +5V", output[i].x1, output[i].y2, 90);
                    break;
                case O_F2:
                    canvas.SetColor(49, 61, 99);
                    canvas.Rectangle(1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1,
                                     output[i].y2 - output[i].y1);
                    canvas.SetFgColor(155, 155, 155);
                    canvas.RotatedText("4-GND GND", output[i].x1, output[i].y2, 90);
                    break;
                case O_F3:
                    canvas.SetColor(49, 61, 99);
                    canvas.Rectangle(1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1,
                                     output[i].y2 - output[i].y1);
                    canvas.SetFgColor(155, 155, 155);
                    canvas.RotatedText("3-NC ", output[i].x1, output[i].y2, 90);
                    break;
                case O_PO1:
                    snprintf(val, 10, "%5.1f", (0.6 * (200 - values[0]) - 40));
                    draw_slider(&output[i], values[0], val, font_p);
                    canvas.SetFont(font);
                    break;
                case O_PO2:
                    snprintf(val, 10, " %3.0f%%", ((200 - values[1]) / 2.0));
                    draw_slider(&output[i], values[1], val, font_p);
                    canvas.SetFont(font);
                    break;
            }
        }
    }

    if (Update) {
        canvas.End();
    }
}

void cpart_dht22::Reset(void) {
    sen_dhtxx_rst(&dht22);
    if (output_pins[0]) {
        SpareParts.SetPin(output_pins[0], 0);
    }
}

void cpart_dht22::PreProcess(void) {
    if (output_pins[0]) {
        sen_dhtxx_setTempHum(&dht22, (0.6 * (200 - values[0]) - 40), (200 - values[1]) / 2.0);
        SpareParts.Reset_pullup_bus(output_pins[0] - 1);
    }
}

void cpart_dht22::Process(void) {
    if (output_pins[0]) {
        const picpin* ppins = SpareParts.GetPinsValues();

        if ((ppins[output_pins[0] - 1].dir) && (ppins[output_pins[0] - 1].value != dht22.out)) {
            SpareParts.SetPin(output_pins[0], dht22.out);
        }

        SpareParts.Set_pullup_bus(output_pins[0] - 1, sen_dhtxx_io(&dht22, ppins[output_pins[0] - 1].value));
    }
}

void cpart_dht22::EvMouseButtonPress(uint button, uint x, uint y, uint state) {
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

void cpart_dht22::EvMouseButtonRelease(uint button, uint x, uint y, uint state) {
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

void cpart_dht22::EvMouseMove(uint button, uint x, uint y, uint state) {
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

unsigned short cpart_dht22::get_in_id(char* name) {
    if (strcmp(name, "PO_1") == 0)
        return I_PO1;
    if (strcmp(name, "PO_2") == 0)
        return I_PO2;

    printf("Erro input '%s' don't have a valid id! \n", name);
    return -1;
}

unsigned short cpart_dht22::get_out_id(char* name) {
    if (strcmp(name, "PN_1") == 0)
        return O_P1;

    if (strcmp(name, "PN_F1") == 0)
        return O_F1;
    if (strcmp(name, "PN_F2") == 0)
        return O_F2;
    if (strcmp(name, "PN_F3") == 0)
        return O_F3;

    if (strcmp(name, "PO_1") == 0)
        return O_PO1;
    if (strcmp(name, "PO_2") == 0)
        return O_PO2;

    printf("Erro output '%s' don't have a valid id! \n", name);
    return 1;
}

lxString cpart_dht22::WritePreferences(void) {
    char prefs[256];

    sprintf(prefs, "%hhu,%hhu,%hhu", output_pins[0], values[0], values[1]);

    return prefs;
}

void cpart_dht22::ReadPreferences(lxString value_) {
    sscanf(value_.c_str(), "%hhu,%hhu,%hhu", &output_pins[0], &values[0], &values[1]);
}

void cpart_dht22::ConfigurePropertiesWindow(CPWindow* WProp) {
    lxString Items = SpareParts.GetPinsNames();
    lxString spin;

    ((CCombo*)WProp->GetChildByName("combo1"))->SetItems(Items);
    if (output_pins[0] == 0)
        ((CCombo*)WProp->GetChildByName("combo1"))->SetText("0  NC");
    else {
        spin = SpareParts.GetPinName(output_pins[0]);
        ((CCombo*)WProp->GetChildByName("combo1"))->SetText(itoa(output_pins[0]) + "  " + spin);
    }

    ((CButton*)WProp->GetChildByName("button1"))->EvMouseButtonRelease = SpareParts.PropButtonRelease;
    ((CButton*)WProp->GetChildByName("button1"))->SetTag(1);

    ((CButton*)WProp->GetChildByName("button2"))->EvMouseButtonRelease = SpareParts.PropButtonRelease;
}

void cpart_dht22::ReadPropertiesWindow(CPWindow* WProp) {
    output_pins[0] = atoi(((CCombo*)WProp->GetChildByName("combo1"))->GetText());
}

part_init(PART_DHT22_Name, cpart_dht22, "Input");
