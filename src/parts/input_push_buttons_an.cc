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

#include "input_push_buttons_an.h"
#include "../oscilloscope.h"
#include "../picsimlab.h"
#include "../spareparts.h"

/* outputs */
enum { O_P1, O_P2, O_P3, O_P4, O_P5, O_P6, O_P7, O_P8, O_B1, O_B2, O_B3, O_B4, O_B5, O_B6, O_B7, O_B8, O_J1 };

/* inputs */
enum { I_B1, I_B2, I_B3, I_B4, I_B5, I_B6, I_B7, I_B8, I_J1 };

static PCWProp pcwprop[4] = {
    {PCW_LABEL, "P1 - VCC,+5V"},
    {PCW_COMBO, "P2 - OUT"},
    {PCW_LABEL, "P3 - GND,GND"},
    {PCW_COMBO, "Open"},
};

cpart_pbuttons_an::cpart_pbuttons_an(const unsigned x, const unsigned y, const char* name, const char* type)
    : part(x, y, name, type), font(9, lxFONTFAMILY_TELETYPE, lxFONTSTYLE_NORMAL, lxFONTWEIGHT_BOLD) {
    active = 1;

    output_value_[0] = !active;
    output_value_[1] = !active;
    output_value_[2] = !active;
    output_value_[3] = !active;
    output_value_[4] = !active;
    output_value_[5] = !active;
    output_value_[6] = !active;
    output_value_[7] = !active;

    output_pins[0] = 0;

    vmax = 5.0;

    output_value = active * vmax;

    SetPCWProperties(pcwprop, 4);
}

void cpart_pbuttons_an::RegisterRemoteControl(void) {
    input_ids[I_B1]->status = &output_value_[0];
    input_ids[I_B1]->update = &output_ids[O_B1]->update;
    input_ids[I_B2]->status = &output_value_[1];
    input_ids[I_B2]->update = &output_ids[O_B2]->update;
    input_ids[I_B3]->status = &output_value_[2];
    input_ids[I_B3]->update = &output_ids[O_B3]->update;
    input_ids[I_B4]->status = &output_value_[3];
    input_ids[I_B4]->update = &output_ids[O_B4]->update;
    input_ids[I_B5]->status = &output_value_[4];
    input_ids[I_B5]->update = &output_ids[O_B5]->update;
    input_ids[I_B6]->status = &output_value_[5];
    input_ids[I_B6]->update = &output_ids[O_B6]->update;
    input_ids[I_B7]->status = &output_value_[6];
    input_ids[I_B7]->update = &output_ids[O_B7]->update;
    input_ids[I_B8]->status = &output_value_[7];
    input_ids[I_B8]->update = &output_ids[O_B8]->update;
}

void cpart_pbuttons_an::Reset(void) {
    // release all
    output_value_[0] = !active;
    output_value_[1] = !active;
    output_value_[2] = !active;
    output_value_[3] = !active;
    output_value_[4] = !active;
    output_value_[5] = !active;
    output_value_[6] = !active;
    output_value_[7] = !active;

    vmax = PICSimLab.GetBoard()->MGetVCC();

    output_value = active * vmax;

    SpareParts.SetAPin(output_pins[0], output_value);
}

cpart_pbuttons_an::~cpart_pbuttons_an(void) {
    delete Bitmap;
    canvas.Destroy();
}

void cpart_pbuttons_an::DrawOutput(const unsigned int i) {
    lxString temp;
    float ftemp;

    switch (output[i].id) {
        case O_P1:
            canvas.SetColor(49, 61, 99);
            canvas.Rectangle(1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1,
                             output[i].y2 - output[i].y1 + 8);
            canvas.SetFgColor(255, 255, 255);
            if (active)
                ftemp = (vmax * (output[i].id - O_P1)) / 8.0;
            else
                ftemp = (vmax * (8 - output[i].id - O_P1)) / 8.0;
            temp = lxString().Format("%3.1f", ftemp) + lxT("V");
            canvas.RotatedText(temp, output[i].x1, output[i].y1, 0);
            if (output_pins[0] == 0)
                canvas.RotatedText("NC", output[i].x1, output[i].y1 + 12, 0);
            else
                canvas.RotatedText(SpareParts.GetPinName(output_pins[0]), output[i].x1, output[i].y1 + 12, 0);
            break;
        case O_P2:
        case O_P3:
        case O_P4:
        case O_P5:
        case O_P6:
        case O_P7:
        case O_P8:
            canvas.SetColor(49, 61, 99);
            canvas.Rectangle(1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);
            canvas.SetFgColor(255, 255, 255);
            if (active)
                ftemp = (vmax * (output[i].id - O_P1)) / 8.0;
            else
                ftemp = (vmax * (8 - output[i].id - O_P1)) / 8.0;

            temp = lxString().Format("%3.1f", ftemp) + lxT("V");
            canvas.RotatedText(temp, output[i].x1, output[i].y1, 0);
            break;
        case O_B1:
        case O_B2:
        case O_B3:
        case O_B4:
        case O_B5:
        case O_B6:
        case O_B7:
        case O_B8:
            canvas.SetColor(100, 100, 100);
            canvas.Circle(1, output[i].cx, output[i].cy, 10);
            if (output_value_[output[i].id - O_B1] == active) {
                canvas.SetColor(55, 55, 55);
            } else {
                canvas.SetColor(15, 15, 15);
            }
            canvas.Circle(1, output[i].cx, output[i].cy, 8);
            break;
    }
}

void cpart_pbuttons_an::PreProcess(void) {
    output_value = active * vmax;

    if (output_value_[0] == active) {
        output_value = (active * vmax * (0.0 / 8.0)) + ((!active) * vmax * (8.0 / 8.0));
    } else if (output_value_[1] == active) {
        output_value = (active * vmax * (1.0 / 8.0)) + ((!active) * vmax * (7.0 / 8.0));
    } else if (output_value_[2] == active) {
        output_value = (active * vmax * (2.0 / 8.0)) + ((!active) * vmax * (6.0 / 8.0));
    } else if (output_value_[3] == active) {
        output_value = (active * vmax * (3.0 / 8.0)) + ((!active) * vmax * (vmax / 8.0));
    } else if (output_value_[4] == active) {
        output_value = (active * vmax * (4.0 / 8.0)) + ((!active) * vmax * (4.0 / 8.0));
    } else if (output_value_[5] == active) {
        output_value = (active * vmax * (vmax / 8.0)) + ((!active) * vmax * (3.0 / 8.0));
    } else if (output_value_[6] == active) {
        output_value = (active * vmax * (6.0 / 8.0)) + ((!active) * vmax * (2.0 / 8.0));
    } else if (output_value_[7] == active) {
        output_value = (active * vmax * (7.0 / 8.0)) + ((!active) * vmax * (1.0 / 8.0));
    }

    SpareParts.SetAPin(output_pins[0], output_value);
}

void cpart_pbuttons_an::OnMouseButtonPress(uint inputId, uint button, uint x, uint y, uint state) {
    switch (inputId) {
        case I_B1:
            output_value_[0] = active;
            output_ids[O_B1]->update = 1;
            break;
        case I_B2:
            output_value_[1] = active;
            output_ids[O_B2]->update = 1;
            break;
        case I_B3:
            output_value_[2] = active;
            output_ids[O_B3]->update = 1;
            break;
        case I_B4:
            output_value_[3] = active;
            output_ids[O_B4]->update = 1;
            break;
        case I_B5:
            output_value_[4] = active;
            output_ids[O_B5]->update = 1;
            break;
        case I_B6:
            output_value_[5] = active;
            output_ids[O_B6]->update = 1;
            break;
        case I_B7:
            output_value_[6] = active;
            output_ids[O_B7]->update = 1;
            break;
        case I_B8:
            output_value_[7] = active;
            output_ids[O_B8]->update = 1;
            break;
    }
}

void cpart_pbuttons_an::OnMouseButtonRelease(uint inputId, uint button, uint x, uint y, uint state) {
    switch (inputId) {
        case I_B1:
            output_value_[0] = !active;
            output_ids[O_B1]->update = 1;
            break;
        case I_B2:
            output_value_[1] = !active;
            output_ids[O_B2]->update = 1;
            break;
        case I_B3:
            output_value_[2] = !active;
            output_ids[O_B3]->update = 1;
            break;
        case I_B4:
            output_value_[3] = !active;
            output_ids[O_B4]->update = 1;
            break;
        case I_B5:
            output_value_[4] = !active;
            output_ids[O_B5]->update = 1;
            break;
        case I_B6:
            output_value_[5] = !active;
            output_ids[O_B6]->update = 1;
            break;
        case I_B7:
            output_value_[6] = !active;
            output_ids[O_B7]->update = 1;
            break;
        case I_B8:
            output_value_[7] = !active;
            output_ids[O_B8]->update = 1;
            break;
    }
}

unsigned short cpart_pbuttons_an::GetInputId(char* name) {
    if (strcmp(name, "PB_1") == 0)
        return I_B1;
    if (strcmp(name, "PB_2") == 0)
        return I_B2;
    if (strcmp(name, "PB_3") == 0)
        return I_B3;
    if (strcmp(name, "PB_4") == 0)
        return I_B4;
    if (strcmp(name, "PB_5") == 0)
        return I_B5;
    if (strcmp(name, "PB_6") == 0)
        return I_B6;
    if (strcmp(name, "PB_7") == 0)
        return I_B7;
    if (strcmp(name, "PB_8") == 0)
        return I_B8;

    if (strcmp(name, "JP_1") == 0)
        return I_J1;

    printf("Erro input '%s' don't have a valid id! \n", name);
    return -1;
}

unsigned short cpart_pbuttons_an::GetOutputId(char* name) {
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

    if (strcmp(name, "PB_1") == 0)
        return O_B1;
    if (strcmp(name, "PB_2") == 0)
        return O_B2;
    if (strcmp(name, "PB_3") == 0)
        return O_B3;
    if (strcmp(name, "PB_4") == 0)
        return O_B4;
    if (strcmp(name, "PB_5") == 0)
        return O_B5;
    if (strcmp(name, "PB_6") == 0)
        return O_B6;
    if (strcmp(name, "PB_7") == 0)
        return O_B7;
    if (strcmp(name, "PB_8") == 0)
        return O_B8;

    if (strcmp(name, "JP_1") == 0)
        return O_J1;

    printf("Erro output '%s' don't have a valid id! \n", name);
    return 1;
}

lxString cpart_pbuttons_an::WritePreferences(void) {
    char prefs[256];

    sprintf(prefs, "%hhu,%hhu", output_pins[0], active);

    return prefs;
}

void cpart_pbuttons_an::ReadPreferences(lxString value) {
    sscanf(value.c_str(), "%hhu,%hhu", &output_pins[0], &active);
    output_value = active * vmax;
    output_value_[0] = !active;
    output_value_[1] = !active;
    output_value_[2] = !active;
    output_value_[3] = !active;
    output_value_[4] = !active;
    output_value_[5] = !active;
    output_value_[6] = !active;
    output_value_[7] = !active;
}

void cpart_pbuttons_an::ConfigurePropertiesWindow(CPWindow* WProp) {
    SetPCWComboWithPinNames(WProp, "combo2", output_pins[0]);

    CCombo* combo = (CCombo*)WProp->GetChildByName("combo4");

    combo->SetItems("HIGH,LOW,");
    if (active)
        combo->SetText("HIGH");
    else
        combo->SetText("LOW ");
}

void cpart_pbuttons_an::ReadPropertiesWindow(CPWindow* WProp) {
    output_pins[0] = GetPWCComboSelectedPin(WProp, "combo2");

    active = (((CCombo*)WProp->GetChildByName("combo4"))->GetText().compare("HIGH") == 0);

    output_value = active * vmax;

    output_value_[0] = !active;
    output_value_[1] = !active;
    output_value_[2] = !active;
    output_value_[3] = !active;
    output_value_[4] = !active;
    output_value_[5] = !active;
    output_value_[6] = !active;
    output_value_[7] = !active;
}

part_init(PART_PUSH_BUTTONS_AN_Name, cpart_pbuttons_an, "Input");
