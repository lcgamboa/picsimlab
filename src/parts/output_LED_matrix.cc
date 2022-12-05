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

#include "output_LED_matrix.h"
#include "../picsimlab1.h"
#include "../picsimlab4.h"
#include "../spareparts.h"

/* outputs */
enum { O_P1, O_P2, O_P3, O_P4, O_F1, O_F2, O_LED };

static PCWProp pcwprop[7] = {{PCW_LABEL, "1-VCC,+5V"}, {PCW_LABEL, "2-GND,GND"}, {PCW_COMBO, "3-DIN"},
                             {PCW_COMBO, "4-CS"},      {PCW_COMBO, "5-CLK"},     {PCW_LABEL, "6-Dout,NC"},
                             {PCW_COMBO, "Angle"}};

cpart_led_matrix::cpart_led_matrix(const unsigned x, const unsigned y, const char* name, const char* type)
    : part(x, y, name, type), font(8, lxFONTFAMILY_TELETYPE, lxFONTSTYLE_NORMAL, lxFONTWEIGHT_BOLD) {
    X = x;
    Y = y;
    ReadMaps();
    Bitmap = NULL;
    angle = 0;

    LoadImage();

    ldd_max72xx_init(&ldd);
    ldd_max72xx_rst(&ldd);

    input_pins[0] = 0;
    input_pins[1] = 0;
    input_pins[2] = 0;

    output_pins[0] = SpareParts.RegisterIOpin(lxT("DOUT"));

    SetPCWProperties(pcwprop, 7);
}

cpart_led_matrix::~cpart_led_matrix(void) {
    SpareParts.UnregisterIOpin(output_pins[0]);
    delete Bitmap;
    canvas.Destroy();
}

void cpart_led_matrix::Draw(void) {
    int i;

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
                case O_P2:
                case O_P3:
                    canvas.SetColor(49, 61, 99);
                    canvas.Rectangle(1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1,
                                     output[i].y2 - output[i].y1);
                    canvas.SetFgColor(255, 255, 255);
                    if (input_pins[output[i].id - O_P1] == 0)
                        canvas.RotatedText("NC", output[i].x1, output[i].y2, 90.0);
                    else
                        canvas.RotatedText(SpareParts.GetPinName(input_pins[output[i].id - O_P1]), output[i].x1,
                                           output[i].y2, 90.0);
                    break;
                case O_P4:
                    canvas.SetColor(49, 61, 99);
                    canvas.Rectangle(1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1,
                                     output[i].y2 - output[i].y1);
                    canvas.SetFgColor(255, 255, 255);
                    if (output_pins[output[i].id - O_P4] == 0)
                        canvas.RotatedText("NC", output[i].x1, output[i].y2, 90.0);
                    else
                        canvas.RotatedText(itoa(output_pins[output[i].id - O_P4]), output[i].x1, output[i].y2, 90.0);
                    break;
                    break;
                case O_F1:
                    canvas.SetColor(49, 61, 99);
                    canvas.Rectangle(1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1,
                                     output[i].y2 - output[i].y1);
                    canvas.SetFgColor(155, 155, 155);
                    canvas.RotatedText("5V", output[i].x1, output[i].y2, 90.0);
                    break;
                case O_F2:
                    canvas.SetColor(49, 61, 99);
                    canvas.Rectangle(1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1,
                                     output[i].y2 - output[i].y1);
                    canvas.SetFgColor(155, 155, 155);
                    canvas.RotatedText("GND", output[i].x1, output[i].y2, 90.0);
                    break;
                case O_LED:
                    // draw ldd text
                    if (ldd.update) {
                        canvas.SetColor(0, 90 + 40, 0);
                        ldd_max72xx_draw(&ldd, &canvas, output[i].x1, output[i].y1, output[i].x2 - output[i].x1,
                                         output[i].y2 - output[i].y1, 1, angle);
                    }
                    break;
            }
        }
    }

    if (Update) {
        canvas.End();
    }
}

unsigned short cpart_led_matrix::GetInputId(char* name) {
    printf("Erro input '%s' don't have a valid id! \n", name);
    return -1;
};

unsigned short cpart_led_matrix::GetOutputId(char* name) {
    if (strcmp(name, "PN_1") == 0)
        return O_P1;
    if (strcmp(name, "PN_2") == 0)
        return O_P2;
    if (strcmp(name, "PN_3") == 0)
        return O_P3;
    if (strcmp(name, "PN_4") == 0)
        return O_P4;

    if (strcmp(name, "PN_F1") == 0)
        return O_F1;
    if (strcmp(name, "PN_F2") == 0)
        return O_F2;

    if (strcmp(name, "LM_LED") == 0)
        return O_LED;

    printf("Erro output '%s' don't have a valid id! \n", name);
    return 1;
};

lxString cpart_led_matrix::WritePreferences(void) {
    char prefs[256];

    sprintf(prefs, "%hhu,%hhu,%hhu,%hhu,%i", input_pins[0], input_pins[1], input_pins[2], output_pins[0], angle);

    return prefs;
}

void cpart_led_matrix::ReadPreferences(lxString value) {
    unsigned char outp;
    sscanf(value.c_str(), "%hhu,%hhu,%hhu,%hhu,%i", &input_pins[0], &input_pins[1], &input_pins[2], &outp, &angle);

    SpareParts.UnregisterIOpin(output_pins[0]);
    output_pins[0] = SpareParts.RegisterIOpin(lxT("DOUT"), outp);

    Reset();
}

void cpart_led_matrix::RegisterRemoteControl(void) {
    output_ids[O_LED]->status = (void*)&ldd;
}

void cpart_led_matrix::ConfigurePropertiesWindow(CPWindow* WProp) {
    SetPCWComboWithPinNames(WProp, "combo3", input_pins[0]);
    SetPCWComboWithPinNames(WProp, "combo4", input_pins[1]);
    SetPCWComboWithPinNames(WProp, "combo5", input_pins[2]);

    ((CLabel*)WProp->GetChildByName("label_6"))->SetText(itoa(output_pins[0]));

    ((CCombo*)WProp->GetChildByName("combo7"))->SetItems("0,90,180,270,");
    ((CCombo*)WProp->GetChildByName("combo7"))->SetText(itoa(angle));
}

void cpart_led_matrix::ReadPropertiesWindow(CPWindow* WProp) {
    input_pins[0] = GetPWCComboSelectedPin(WProp, "combo3");
    input_pins[1] = GetPWCComboSelectedPin(WProp, "combo4");
    input_pins[2] = GetPWCComboSelectedPin(WProp, "combo5");
    angle = atoi(((CCombo*)WProp->GetChildByName("combo7"))->GetText());
}

void cpart_led_matrix::Process(void) {
    const picpin* ppins = SpareParts.GetPinsValues();

    if ((input_pins[0] > 0) && (input_pins[1] > 0) && (input_pins[2] > 0)) {
        unsigned char out;
        out = ldd_max72xx_io(&ldd, ppins[input_pins[0] - 1].value, ppins[input_pins[2] - 1].value,
                             ppins[input_pins[1] - 1].value);

        if (out != ppins[output_pins[0] - 1].value) {
            SpareParts.WritePin(output_pins[0], out);
        }
    }
}

void cpart_led_matrix::PostProcess(void) {
    if (ldd.update)
        output_ids[O_LED]->update = 1;
}

part_init(PART_LED_MATRIX_Name, cpart_led_matrix, "Output");
