/* ########################################################################

   PICSimLab - Programmable IC Simulator Laboratory

   ########################################################################

   Copyright (c) : 2010-2023  Luis Claudio Gambôa Lopes <lcgamboa@yahoo.com>

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

#include "input_pot_r.h"
#include "../lib/oscilloscope.h"
#include "../lib/picsimlab.h"
#include "../lib/spareparts.h"

/* outputs */
enum { O_PO1, O_PO2, O_PO3, O_PO4, O_P1, O_P2, O_P3, O_P4 };

/* inputs */
enum { I_PO1, I_PO2, I_PO3, I_PO4 };

static PCWProp pcwprop[6] = {{PCW_LABEL, "1-VCC,+5V"}, {PCW_COMBO, "2-POT 1"}, {PCW_COMBO, "3-POT 2"},
                             {PCW_COMBO, "4-POT 3"},   {PCW_COMBO, "5-POT 4"}, {PCW_LABEL, "6-GND ,GND"}};

cpart_pot_r::cpart_pot_r(const unsigned x, const unsigned y, const char* name, const char* type)
    : part(x, y, name, type),
      font(9, lxFONTFAMILY_TELETYPE, lxFONTSTYLE_NORMAL, lxFONTWEIGHT_BOLD),
      font_p(8, lxFONTFAMILY_TELETYPE, lxFONTSTYLE_NORMAL, lxFONTWEIGHT_BOLD) {
    output_pins[0] = 0;
    output_pins[1] = 0;
    output_pins[2] = 0;
    output_pins[3] = 0;

    values[0] = 0;
    values[1] = 0;
    values[2] = 0;
    values[3] = 0;

    active[0] = 0;
    active[1] = 0;
    active[2] = 0;
    active[3] = 0;

    vmax = 5.0;

    SetPCWProperties(pcwprop, 6);
    PinCount = 4;
    Pins = output_pins;
}

void cpart_pot_r::RegisterRemoteControl(void) {
    input_ids[I_PO1]->status = &values[0];
    input_ids[I_PO1]->update = &output_ids[O_PO1]->update;
    input_ids[I_PO2]->status = &values[1];
    input_ids[I_PO2]->update = &output_ids[O_PO2]->update;
    input_ids[I_PO3]->status = &values[2];
    input_ids[I_PO3]->update = &output_ids[O_PO3]->update;
    input_ids[I_PO4]->status = &values[3];
    input_ids[I_PO4]->update = &output_ids[O_PO4]->update;
}

cpart_pot_r::~cpart_pot_r(void) {
    delete Bitmap;
    canvas.Destroy();
}

void cpart_pot_r::Reset(void) {
    vmax = PICSimLab.GetBoard()->MGetVCC();
}

void cpart_pot_r::DrawOutput(const unsigned int i) {
    char val[10];

    switch (output[i].id) {
        case O_P1:
        case O_P2:
        case O_P3:
        case O_P4:
            canvas.SetColor(49, 61, 99);
            canvas.Rectangle(1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);
            canvas.SetFgColor(255, 255, 255);
            if (output_pins[output[i].id - O_P1] == 0)
                canvas.RotatedText("NC", output[i].x1, output[i].y1, 0);
            else
                canvas.RotatedText(SpareParts.GetPinName(output_pins[output[i].id - O_P1]), output[i].x1, output[i].y1,
                                   0);
            break;
        case O_PO1:
        case O_PO2:
        case O_PO3:
        case O_PO4:
            canvas.SetFgColor(0, 0, 0);
            canvas.SetBgColor(66, 109, 246);
            canvas.Rectangle(1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);

            canvas.SetBgColor(250, 250, 250);
            canvas.Circle(1, output[i].cx, output[i].cy, 17);

            canvas.SetBgColor(150, 150, 150);
            int x = -13 * sin((5.585 * (values[output[i].id - O_PO1] / 200.0)) + 0.349);
            int y = 13 * cos((5.585 * (values[output[i].id - O_PO1] / 200.0)) + 0.349);
            canvas.Circle(1, output[i].cx + x, output[i].cy + y, 2);

            canvas.SetColor(49, 61, 99);
            canvas.Rectangle(1, output[i].x1 + 6, output[i].y2 + 6, 30, 15);
            snprintf(val, 10, "%4.2f", vmax * (values[output[i].id - O_PO1]) / 200.0);
            canvas.SetColor(250, 250, 250);
            canvas.SetFont(font_p);
            canvas.RotatedText(val, output[i].x1 + 6, output[i].y2 + 6, 0);
            break;
    }
}

void cpart_pot_r::PreProcess(void) {
    SpareParts.SetAPin(output_pins[0], vmax * (values[0]) / 200.0);
    SpareParts.SetAPin(output_pins[1], vmax * (values[1]) / 200.0);
    SpareParts.SetAPin(output_pins[2], vmax * (values[2]) / 200.0);
    SpareParts.SetAPin(output_pins[3], vmax * (values[3]) / 200.0);
}

void cpart_pot_r::OnMouseButtonPress(uint inputId, uint button, uint x, uint y, uint state) {
    switch (inputId) {
        case I_PO1:
            values[0] = CalcAngle(inputId, x, y);
            active[0] = 1;
            output_ids[O_PO1]->update = 1;
            break;
        case I_PO2:
            values[1] = CalcAngle(inputId, x, y);
            active[1] = 1;
            output_ids[O_PO2]->update = 1;
            break;
        case I_PO3:
            values[2] = CalcAngle(inputId, x, y);
            active[2] = 1;
            output_ids[O_PO3]->update = 1;
            break;
        case I_PO4:
            values[3] = CalcAngle(inputId, x, y);
            active[3] = 1;
            output_ids[O_PO4]->update = 1;
            break;
    }
}

void cpart_pot_r::OnMouseButtonRelease(uint inputId, uint button, uint x, uint y, uint state) {
    switch (inputId) {
        case I_PO1:
            active[0] = 0;
            output_ids[O_PO1]->update = 1;
            break;
        case I_PO2:
            active[1] = 0;
            output_ids[O_PO2]->update = 1;
            break;
        case I_PO3:
            active[2] = 0;
            output_ids[O_PO3]->update = 1;
            break;
        case I_PO4:
            active[3] = 0;
            output_ids[O_PO4]->update = 1;
            break;
    }
}

void cpart_pot_r::OnMouseMove(uint inputId, uint button, uint x, uint y, uint state) {
    switch (inputId) {
        case I_PO1 ... I_PO4:
            if (active[inputId - I_PO1]) {
                values[inputId - I_PO1] = CalcAngle(inputId, x, y);
                output_ids[O_PO1 + inputId - I_PO1]->update = 1;
            }
            break;
        default:
            active[0] = 0;
            active[1] = 0;
            active[2] = 0;
            active[3] = 0;
            break;
    }
}

unsigned char cpart_pot_r::CalcAngle(int inputId, int x, int y) {
    int dx = input_ids[inputId]->cx - x;
    int dy = y - input_ids[inputId]->cy;
    double angle = 0;

    if ((dx >= 0) && (dy >= 0)) {
        angle = atan2(dx, dy) * 180 / M_PI;
    } else if ((dx >= 0) && (dy < 0)) {
        angle = 180 - (atan2(dx, -dy) * 180 / M_PI);
    } else if ((dx < 0) && (dy < 0)) {
        angle = (atan2(-dx, -dy) * 180 / M_PI) + 180;
    } else if ((dx < 0) && (dy >= 0)) {
        angle = 360 - (atan2(-dx, dy) * 180 / M_PI);
    }

    if (angle > 340)
        angle = 340;
    if (angle < 20)
        angle = 20;

    return (200 * (angle - 20) / 320.0);
}

unsigned short cpart_pot_r::GetInputId(char* name) {
    if (strcmp(name, "PO_1") == 0)
        return I_PO1;
    if (strcmp(name, "PO_2") == 0)
        return I_PO2;
    if (strcmp(name, "PO_3") == 0)
        return I_PO3;
    if (strcmp(name, "PO_4") == 0)
        return I_PO4;

    printf("Erro input '%s' don't have a valid id! \n", name);
    return -1;
}

unsigned short cpart_pot_r::GetOutputId(char* name) {
    if (strcmp(name, "PN_1") == 0)
        return O_P1;
    if (strcmp(name, "PN_2") == 0)
        return O_P2;
    if (strcmp(name, "PN_3") == 0)
        return O_P3;
    if (strcmp(name, "PN_4") == 0)
        return O_P4;

    if (strcmp(name, "PO_1") == 0)
        return O_PO1;
    if (strcmp(name, "PO_2") == 0)
        return O_PO2;
    if (strcmp(name, "PO_3") == 0)
        return O_PO3;
    if (strcmp(name, "PO_4") == 0)
        return O_PO4;

    printf("Erro output '%s' don't have a valid id! \n", name);
    return 1;
}

lxString cpart_pot_r::WritePreferences(void) {
    char prefs[256];

    sprintf(prefs, "%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu", output_pins[0], output_pins[1], output_pins[2],
            output_pins[3], values[0], values[1], values[2], values[3]);

    return prefs;
}

void cpart_pot_r::ReadPreferences(lxString value) {
    sscanf(value.c_str(), "%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu", &output_pins[0], &output_pins[1], &output_pins[2],
           &output_pins[3], &values[0], &values[1], &values[2], &values[3]);
}

void cpart_pot_r::ConfigurePropertiesWindow(CPWindow* WProp) {
    SetPCWComboWithPinNames(WProp, "combo2", output_pins[0]);
    SetPCWComboWithPinNames(WProp, "combo3", output_pins[1]);
    SetPCWComboWithPinNames(WProp, "combo4", output_pins[2]);
    SetPCWComboWithPinNames(WProp, "combo5", output_pins[3]);
}

void cpart_pot_r::ReadPropertiesWindow(CPWindow* WProp) {
    output_pins[0] = GetPWCComboSelectedPin(WProp, "combo2");
    output_pins[1] = GetPWCComboSelectedPin(WProp, "combo3");
    output_pins[2] = GetPWCComboSelectedPin(WProp, "combo4");
    output_pins[3] = GetPWCComboSelectedPin(WProp, "combo5");
}

part_init(PART_POTR_Name, cpart_pot_r, "Input");
