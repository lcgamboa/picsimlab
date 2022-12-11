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

#include "input_fixedv.h"
#include "../picsimlab1.h"
#include "../picsimlab4.h"
#include "../spareparts.h"

/* outputs */
enum { O_P1, O_F1, O_F2, O_VOLT };

static PCWProp pcwprop[4] = {
    {PCW_LABEL, "1 - VCC,+5V"},
    {PCW_COMBO, "2 - OUT"},
    {PCW_LABEL, "3 - GND ,GND"},
    {PCW_SPIND, "4 - Value"},
};

cpart_fixedv::cpart_fixedv(const unsigned x, const unsigned y, const char* name, const char* type)
    : part(x, y, name, type),
      font(9, lxFONTFAMILY_TELETYPE, lxFONTSTYLE_NORMAL, lxFONTWEIGHT_BOLD),
      font_b(12, lxFONTFAMILY_TELETYPE, lxFONTSTYLE_NORMAL, lxFONTWEIGHT_BOLD) {
    output_pins[0] = 0;

    value = 0;
    active = 0;

    SetPCWProperties(pcwprop, 4);
}

void cpart_fixedv::RegisterRemoteControl(void) {}

cpart_fixedv::~cpart_fixedv(void) {
    delete Bitmap;
    canvas.Destroy();
}

void cpart_fixedv::DrawOutput(const unsigned int i) {
    char val[10];

    switch (output[i].id) {
        case O_P1:
            canvas.SetColor(49, 61, 99);
            canvas.Rectangle(1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);
            canvas.SetFgColor(255, 255, 255);
            if (output_pins[output[i].id - O_P1] == 0)
                canvas.RotatedText("NC", output[i].x1, output[i].y2, 90);
            else
                canvas.RotatedText(SpareParts.GetPinName(output_pins[output[i].id - O_P1]), output[i].x1, output[i].y2,
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
        case O_VOLT:
            canvas.SetColor(49, 61, 99);
            canvas.Rectangle(1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);
            snprintf(val, 10, "%5.2f V", value / 40.0);
            canvas.SetColor(250, 250, 250);
            canvas.SetFont(font_b);
            canvas.RotatedText(val, output[i].x1, output[i].y1, 0);
            canvas.SetFont(font);
            break;
    }
}

void cpart_fixedv::PostProcess(void) {
    if (output_pins[0]) {
        SpareParts.SetAPin(output_pins[0], value / 40.0);
    }
}

unsigned short cpart_fixedv::GetInputId(char* name) {
    printf("Erro input '%s' don't have a valid id! \n", name);
    return -1;
}

unsigned short cpart_fixedv::GetOutputId(char* name) {
    if (strcmp(name, "PN_1") == 0)
        return O_P1;

    if (strcmp(name, "PN_F1") == 0)
        return O_F1;
    if (strcmp(name, "PN_F2") == 0)
        return O_F2;

    if (strcmp(name, "DI_VOLT") == 0)
        return O_VOLT;

    printf("Erro output '%s' don't have a valid id! \n", name);
    return 1;
}

lxString cpart_fixedv::WritePreferences(void) {
    char prefs[256];

    sprintf(prefs, "%hhu,%hhu", output_pins[0], value);

    return prefs;
}

void cpart_fixedv::ReadPreferences(lxString value_) {
    sscanf(value_.c_str(), "%hhu,%hhu", &output_pins[0], &value);
}

void cpart_fixedv::ConfigurePropertiesWindow(CPWindow* WProp) {
    SetPCWComboWithPinNames(WProp, "combo2", output_pins[0]);

    CSpind* spind = (CSpind*)WProp->GetChildByName("spind4");
    spind->SetValue(value / 40.0);
    spind->SetMax(5.0);
    spind->SetMin(0.0);
}

void cpart_fixedv::ReadPropertiesWindow(CPWindow* WProp) {
    output_pins[0] = GetPWCComboSelectedPin(WProp, "combo2");

    value = ((CSpind*)WProp->GetChildByName("spind4"))->GetValue() * 40.0;
}

part_init(PART_FIXEDV_Name, cpart_fixedv, "Input");
