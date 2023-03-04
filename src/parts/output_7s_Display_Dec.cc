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

#include "output_7s_Display_Dec.h"
#include "../lib/oscilloscope.h"
#include "../lib/picsimlab.h"
#include "../lib/spareparts.h"

/* outputs */
enum {
    O_A1,
    O_B1,
    O_C1,
    O_D1,
    O_E1,
    O_F1,
    O_G1,
    O_P1,
    O_SS1,
    O_A2,
    O_B2,
    O_C2,
    O_D2,
    O_E2,
    O_F2,
    O_G2,
    O_P2,
    O_SS2,
    O_A3,
    O_B3,
    O_C3,
    O_D3,
    O_E3,
    O_F3,
    O_G3,
    O_P3,
    O_SS3,
    O_A4,
    O_B4,
    O_C4,
    O_D4,
    O_E4,
    O_F4,
    O_G4,
    O_P4,
    O_SS4,
    O_A,
    O_B,
    O_C,
    O_D,
    O_DISP1,
    O_DISP2,
    O_DISP3,
    O_DISP4,
    O_FX1,
    O_FX2
};

static PCWProp pcwprop[9] = {{PCW_COMBO, "A"},  {PCW_COMBO, "B"},  {PCW_COMBO, "C"},
                             {PCW_COMBO, "D"},  {PCW_COMBO, "D1"}, {PCW_COMBO, "D2"},
                             {PCW_COMBO, "D3"}, {PCW_COMBO, "D4"}, {PCW_COMBO, "Type"}};

cpart_7s_display_dec::cpart_7s_display_dec(const unsigned x, const unsigned y, const char* name, const char* type)
    : part(x, y, name, type), font(9, lxFONTFAMILY_TELETYPE, lxFONTSTYLE_NORMAL, lxFONTWEIGHT_BOLD) {
    X = x;
    Y = y;
    always_update = 1;

    input_pins[0] = 0;
    input_pins[1] = 0;
    input_pins[2] = 0;
    input_pins[3] = 0;
    input_pins[4] = 0;
    input_pins[5] = 0;
    input_pins[6] = 0;
    input_pins[7] = 0;

    latchs[0] = 0;
    latchs[1] = 0;
    latchs[2] = 0;
    latchs[3] = 0;

    mcount = 0;

    memset(lm1, 0, 8 * sizeof(unsigned int));
    memset(lm2, 0, 8 * sizeof(unsigned int));
    memset(lm3, 0, 8 * sizeof(unsigned int));
    memset(lm4, 0, 8 * sizeof(unsigned int));

    memset(alm1, 0, 8 * sizeof(unsigned int));
    memset(alm2, 0, 8 * sizeof(unsigned int));
    memset(alm3, 0, 8 * sizeof(unsigned int));
    memset(alm4, 0, 8 * sizeof(unsigned int));

    Bitmap = NULL;
    dtype = 1;  // to force type change
    ChangeType(0);

    SetPCWProperties(pcwprop, 9);

    PinCount = 8;
    Pins = input_pins;
}

cpart_7s_display_dec::~cpart_7s_display_dec(void) {
    delete Bitmap;
    canvas.Destroy();
}

void cpart_7s_display_dec::DrawOutput(const unsigned int i) {
    canvas.SetFgColor(30, 0, 0);

    switch (output[i].id) {
        case O_A:
        case O_B:
        case O_C:
        case O_D:
        case O_DISP1:
        case O_DISP2:
        case O_DISP3:
        case O_DISP4:
            canvas.SetColor(49, 61, 99);
            canvas.Rectangle(1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);
            canvas.SetFgColor(255, 255, 255);
            if (input_pins[output[i].id - O_A] == 0)
                canvas.RotatedText("NC", output[i].x1, output[i].y2, 90.0);
            else
                canvas.RotatedText(SpareParts.GetPinName(input_pins[output[i].id - O_A]), output[i].x1, output[i].y2,
                                   90.0);
            break;
        case O_FX1:
            canvas.SetColor(49, 61, 99);
            canvas.Rectangle(1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);
            canvas.SetFgColor(155, 155, 155);
            canvas.RotatedText("GND", output[i].x1, output[i].y2, 90.0);
            break;
        case O_FX2:
            canvas.SetColor(49, 61, 99);
            canvas.Rectangle(1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);
            canvas.SetFgColor(155, 155, 155);
            canvas.RotatedText("+5V", output[i].x1, output[i].y2, 90.0);
            break;
        case O_A1:
            canvas.SetColor(0, lm1[0], 0);
            canvas.Rectangle(1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);
            break;
        case O_B1:
            canvas.SetColor(0, lm1[1], 0);
            canvas.Rectangle(1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);
            break;
        case O_C1:
            canvas.SetColor(0, lm1[2], 0);
            canvas.Rectangle(1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);
            break;
        case O_D1:
            canvas.SetColor(0, lm1[3], 0);
            canvas.Rectangle(1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);
            break;
        case O_E1:
            canvas.SetColor(0, lm1[4], 0);
            canvas.Rectangle(1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);
            break;
        case O_F1:
            canvas.SetColor(0, lm1[5], 0);
            canvas.Rectangle(1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);
            break;
        case O_G1:
            canvas.SetColor(0, lm1[6], 0);
            canvas.Rectangle(1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);
            break;
        case O_P1:
            canvas.SetColor(0, lm1[7], 0);
            canvas.Circle(1, output[i].x1, output[i].y1, output[i].r);
            break;

        case O_A2:
            canvas.SetColor(0, lm2[0], 0);
            canvas.Rectangle(1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);
            break;
        case O_B2:
            canvas.SetColor(0, lm2[1], 0);
            canvas.Rectangle(1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);
            break;
        case O_C2:
            canvas.SetColor(0, lm2[2], 0);
            canvas.Rectangle(1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);
            break;
        case O_D2:
            canvas.SetColor(0, lm2[3], 0);
            canvas.Rectangle(1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);
            break;
        case O_E2:
            canvas.SetColor(0, lm2[4], 0);
            canvas.Rectangle(1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);
            break;
        case O_F2:
            canvas.SetColor(0, lm2[5], 0);
            canvas.Rectangle(1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);
            break;
        case O_G2:
            canvas.SetColor(0, lm2[6], 0);
            canvas.Rectangle(1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);
            break;
        case O_P2:
            canvas.SetColor(0, lm2[7], 0);
            canvas.Circle(1, output[i].x1, output[i].y1, output[i].r);
            break;

        case O_A3:
            canvas.SetColor(0, lm3[0], 0);
            canvas.Rectangle(1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);
            break;
        case O_B3:
            canvas.SetColor(0, lm3[1], 0);
            canvas.Rectangle(1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);
            break;
        case O_C3:
            canvas.SetColor(0, lm3[2], 0);
            canvas.Rectangle(1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);
            break;
        case O_D3:
            canvas.SetColor(0, lm3[3], 0);
            canvas.Rectangle(1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);
            break;
        case O_E3:
            canvas.SetColor(0, lm3[4], 0);
            canvas.Rectangle(1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);
            break;
        case O_F3:
            canvas.SetColor(0, lm3[5], 0);
            canvas.Rectangle(1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);
            break;
        case O_G3:
            canvas.SetColor(0, lm3[6], 0);
            canvas.Rectangle(1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);
            break;
        case O_P3:
            canvas.SetColor(0, lm3[7], 0);
            canvas.Circle(1, output[i].x1, output[i].y1, output[i].r);
            break;
            break;
        case O_A4:
            canvas.SetColor(0, lm4[0], 0);
            canvas.Rectangle(1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);
            break;
        case O_B4:
            canvas.SetColor(0, lm4[1], 0);
            canvas.Rectangle(1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);
            break;
        case O_C4:
            canvas.SetColor(0, lm4[2], 0);
            canvas.Rectangle(1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);
            break;
        case O_D4:
            canvas.SetColor(0, lm4[3], 0);
            canvas.Rectangle(1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);
            break;
        case O_E4:
            canvas.SetColor(0, lm4[4], 0);
            canvas.Rectangle(1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);
            break;
        case O_F4:
            canvas.SetColor(0, lm4[5], 0);
            canvas.Rectangle(1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);
            break;
        case O_G4:
            canvas.SetColor(0, lm4[6], 0);
            canvas.Rectangle(1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);
            break;
        case O_P4:
            canvas.SetColor(0, lm4[7], 0);
            canvas.Circle(1, output[i].x1, output[i].y1, output[i].r);
            break;
        case O_SS1:
            output_ids[O_A1]->update = 1;
            output_ids[O_B1]->update = 1;
            output_ids[O_C1]->update = 1;
            output_ids[O_D1]->update = 1;
            output_ids[O_E1]->update = 1;
            output_ids[O_F1]->update = 1;
            output_ids[O_G1]->update = 1;
            output_ids[O_P1]->update = 1;
            canvas.SetColor(10, 10, 10);
            canvas.Rectangle(1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);
        case O_SS2:
            output_ids[O_A2]->update = 1;
            output_ids[O_B2]->update = 1;
            output_ids[O_C2]->update = 1;
            output_ids[O_D2]->update = 1;
            output_ids[O_E2]->update = 1;
            output_ids[O_F2]->update = 1;
            output_ids[O_G2]->update = 1;
            output_ids[O_P2]->update = 1;
            canvas.SetColor(10, 10, 10);
            canvas.Rectangle(1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);
        case O_SS3:
            output_ids[O_A3]->update = 1;
            output_ids[O_B3]->update = 1;
            output_ids[O_C3]->update = 1;
            output_ids[O_D3]->update = 1;
            output_ids[O_E3]->update = 1;
            output_ids[O_F3]->update = 1;
            output_ids[O_G3]->update = 1;
            output_ids[O_P3]->update = 1;
            canvas.SetColor(10, 10, 10);
            canvas.Rectangle(1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);
        case O_SS4:
            output_ids[O_A4]->update = 1;
            output_ids[O_B4]->update = 1;
            output_ids[O_C4]->update = 1;
            output_ids[O_D4]->update = 1;
            output_ids[O_E4]->update = 1;
            output_ids[O_F4]->update = 1;
            output_ids[O_G4]->update = 1;
            output_ids[O_P4]->update = 1;
            canvas.SetColor(10, 10, 10);
            canvas.Rectangle(1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);
            break;
    }
}

unsigned short cpart_7s_display_dec::GetInputId(char* name) {
    printf("Erro input '%s' don't have a valid id! \n", name);
    return -1;
}

unsigned short cpart_7s_display_dec::GetOutputId(char* name) {
    if (strcmp(name, "SS_A1") == 0)
        return O_A1;
    if (strcmp(name, "SS_B1") == 0)
        return O_B1;
    if (strcmp(name, "SS_C1") == 0)
        return O_C1;
    if (strcmp(name, "SS_D1") == 0)
        return O_D1;
    if (strcmp(name, "SS_E1") == 0)
        return O_E1;
    if (strcmp(name, "SS_F1") == 0)
        return O_F1;
    if (strcmp(name, "SS_G1") == 0)
        return O_G1;
    if (strcmp(name, "SS_P1") == 0)
        return O_P1;

    if (strcmp(name, "SS_A2") == 0)
        return O_A2;
    if (strcmp(name, "SS_B2") == 0)
        return O_B2;
    if (strcmp(name, "SS_C2") == 0)
        return O_C2;
    if (strcmp(name, "SS_D2") == 0)
        return O_D2;
    if (strcmp(name, "SS_E2") == 0)
        return O_E2;
    if (strcmp(name, "SS_F2") == 0)
        return O_F2;
    if (strcmp(name, "SS_G2") == 0)
        return O_G2;
    if (strcmp(name, "SS_P2") == 0)
        return O_P2;

    if (strcmp(name, "SS_A3") == 0)
        return O_A3;
    if (strcmp(name, "SS_B3") == 0)
        return O_B3;
    if (strcmp(name, "SS_C3") == 0)
        return O_C3;
    if (strcmp(name, "SS_D3") == 0)
        return O_D3;
    if (strcmp(name, "SS_E3") == 0)
        return O_E3;
    if (strcmp(name, "SS_F3") == 0)
        return O_F3;
    if (strcmp(name, "SS_G3") == 0)
        return O_G3;
    if (strcmp(name, "SS_P3") == 0)
        return O_P3;

    if (strcmp(name, "SS_A4") == 0)
        return O_A4;
    if (strcmp(name, "SS_B4") == 0)
        return O_B4;
    if (strcmp(name, "SS_C4") == 0)
        return O_C4;
    if (strcmp(name, "SS_D4") == 0)
        return O_D4;
    if (strcmp(name, "SS_E4") == 0)
        return O_E4;
    if (strcmp(name, "SS_F4") == 0)
        return O_F4;
    if (strcmp(name, "SS_G4") == 0)
        return O_G4;
    if (strcmp(name, "SS_P4") == 0)
        return O_P4;

    if (strcmp(name, "PN_A") == 0)
        return O_A;
    if (strcmp(name, "PN_B") == 0)
        return O_B;
    if (strcmp(name, "PN_C") == 0)
        return O_C;
    if (strcmp(name, "PN_D") == 0)
        return O_D;

    if (strcmp(name, "PN_D1") == 0)
        return O_DISP1;
    if (strcmp(name, "PN_D2") == 0)
        return O_DISP2;
    if (strcmp(name, "PN_D3") == 0)
        return O_DISP3;
    if (strcmp(name, "PN_D4") == 0)
        return O_DISP4;

    if (strcmp(name, "PN_FX1") == 0)
        return O_FX1;
    if (strcmp(name, "PN_FX2") == 0)
        return O_FX2;

    if (strcmp(name, "SS_1") == 0)
        return O_SS1;
    if (strcmp(name, "SS_2") == 0)
        return O_SS2;
    if (strcmp(name, "SS_3") == 0)
        return O_SS3;
    if (strcmp(name, "SS_4") == 0)
        return O_SS4;

    printf("Erro output '%s' don't have a valid id! \n", name);
    return 1;
}

lxString cpart_7s_display_dec::WritePreferences(void) {
    char prefs[256];

    sprintf(prefs, "%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu", input_pins[0], input_pins[1], input_pins[2],
            input_pins[3], input_pins[4], input_pins[5], input_pins[6], input_pins[7], dtype);

    return prefs;
}

void cpart_7s_display_dec::ReadPreferences(lxString value) {
    unsigned char dtype_;

    sscanf(value.c_str(), "%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu", &input_pins[0], &input_pins[1],
           &input_pins[2], &input_pins[3], &input_pins[4], &input_pins[5], &input_pins[6], &input_pins[7], &dtype_);

    ChangeType(dtype_);
}

void cpart_7s_display_dec::RegisterRemoteControl(void) {
    output_ids[O_A1]->status = (void*)&lm1[0];
    output_ids[O_B1]->status = (void*)&lm1[1];
    output_ids[O_C1]->status = (void*)&lm1[2];
    output_ids[O_D1]->status = (void*)&lm1[3];
    output_ids[O_E1]->status = (void*)&lm1[4];
    output_ids[O_F1]->status = (void*)&lm1[5];
    output_ids[O_G1]->status = (void*)&lm1[6];
    output_ids[O_P1]->status = (void*)&lm1[7];
    output_ids[O_A2]->status = (void*)&lm2[0];
    output_ids[O_B2]->status = (void*)&lm2[1];
    output_ids[O_C2]->status = (void*)&lm2[2];
    output_ids[O_D2]->status = (void*)&lm2[3];
    output_ids[O_E2]->status = (void*)&lm2[4];
    output_ids[O_F2]->status = (void*)&lm2[5];
    output_ids[O_G2]->status = (void*)&lm2[6];
    output_ids[O_P2]->status = (void*)&lm2[7];
    output_ids[O_A3]->status = (void*)&lm3[0];
    output_ids[O_B3]->status = (void*)&lm3[1];
    output_ids[O_C3]->status = (void*)&lm3[2];
    output_ids[O_D3]->status = (void*)&lm3[3];
    output_ids[O_E3]->status = (void*)&lm3[4];
    output_ids[O_F3]->status = (void*)&lm3[5];
    output_ids[O_G3]->status = (void*)&lm3[6];
    output_ids[O_P3]->status = (void*)&lm3[7];
    output_ids[O_A4]->status = (void*)&lm4[0];
    output_ids[O_B4]->status = (void*)&lm4[1];
    output_ids[O_C4]->status = (void*)&lm4[2];
    output_ids[O_D4]->status = (void*)&lm4[3];
    output_ids[O_E4]->status = (void*)&lm4[4];
    output_ids[O_F4]->status = (void*)&lm4[5];
    output_ids[O_G4]->status = (void*)&lm4[6];
    output_ids[O_P4]->status = (void*)&lm4[7];
}

void cpart_7s_display_dec::ConfigurePropertiesWindow(CPWindow* WProp) {
    SetPCWComboWithPinNames(WProp, "combo1", input_pins[0]);
    SetPCWComboWithPinNames(WProp, "combo2", input_pins[1]);
    SetPCWComboWithPinNames(WProp, "combo3", input_pins[2]);
    SetPCWComboWithPinNames(WProp, "combo4", input_pins[3]);
    SetPCWComboWithPinNames(WProp, "combo5", input_pins[4]);
    SetPCWComboWithPinNames(WProp, "combo6", input_pins[5]);
    SetPCWComboWithPinNames(WProp, "combo7", input_pins[6]);
    SetPCWComboWithPinNames(WProp, "combo8", input_pins[7]);

    ((CCombo*)WProp->GetChildByName("combo9"))->SetItems("MUX,LATCH,");

    if (dtype == 0)
        ((CCombo*)WProp->GetChildByName("combo9"))->SetText("MUX");
    else {
        ((CCombo*)WProp->GetChildByName("combo9"))->SetText("LATCH");
    }
}

void cpart_7s_display_dec::ReadPropertiesWindow(CPWindow* WProp) {
    input_pins[0] = GetPWCComboSelectedPin(WProp, "combo1");
    input_pins[1] = GetPWCComboSelectedPin(WProp, "combo2");
    input_pins[2] = GetPWCComboSelectedPin(WProp, "combo3");
    input_pins[3] = GetPWCComboSelectedPin(WProp, "combo4");
    input_pins[4] = GetPWCComboSelectedPin(WProp, "combo5");
    input_pins[5] = GetPWCComboSelectedPin(WProp, "combo6");
    input_pins[6] = GetPWCComboSelectedPin(WProp, "combo7");
    input_pins[7] = GetPWCComboSelectedPin(WProp, "combo8");

    unsigned char type_ = (((CCombo*)WProp->GetChildByName("combo9"))->GetText().Cmp("MUX")) != 0;

    ChangeType(type_);
}

void cpart_7s_display_dec::PreProcess(void) {
    memset(alm1, 0, 8 * sizeof(unsigned int));
    memset(alm2, 0, 8 * sizeof(unsigned int));
    memset(alm3, 0, 8 * sizeof(unsigned int));
    memset(alm4, 0, 8 * sizeof(unsigned int));

    JUMPSTEPS_ = PICSimLab.GetJUMPSTEPS();
    mcount = JUMPSTEPS_;
}

void cpart_7s_display_dec::Process(void) {
    int i;
    unsigned char value;

    const picpin* ppins = SpareParts.GetPinsValues();

    mcount++;

    if (mcount > JUMPSTEPS_) {
        value = 0;
        if (input_pins[0] && ppins[input_pins[0] - 1].value)
            value |= 0x01;
        if (input_pins[1] && ppins[input_pins[1] - 1].value)
            value |= 0x02;
        if (input_pins[2] && ppins[input_pins[2] - 1].value)
            value |= 0x04;
        if (input_pins[3] && ppins[input_pins[3] - 1].value)
            value |= 0x08;

        switch (value) {
            case 0:
                value = 0x3F;
                break;
            case 1:
                value = 0x06;
                break;
            case 2:
                value = 0x5B;
                break;
            case 3:
                value = 0x4F;
                break;
            case 4:
                value = 0x66;
                break;
            case 5:
                value = 0x6D;
                break;
            case 6:
                value = 0x7D;
                break;
            case 7:
                value = 0x07;
                break;
            case 8:
                value = 0x7F;
                break;
            case 9:
                value = 0x6F;
                break;
                /*
               case 10:
                value = 0x77;
                break;
               case 11:
                value = 0x7c;
                break;
               case 12:
                value = 0x58;
                break;
               case 13:
                value = 0x5E;
                break;
               case 14:
                value = 0x79;
                break;
               case 15:
                value = 0x71;
                break;
                 */
            default:
                value = 0;
        }

        if (!dtype) {
            // MUX
            for (i = 0; i < 8; i++) {
                if (value & (0x01 << i)) {
                    if (input_pins[4] && ppins[input_pins[4] - 1].value)
                        alm1[i]++;
                    if (input_pins[5] && ppins[input_pins[5] - 1].value)
                        alm2[i]++;
                    if (input_pins[6] && ppins[input_pins[6] - 1].value)
                        alm3[i]++;
                    if (input_pins[7] && ppins[input_pins[7] - 1].value)
                        alm4[i]++;
                }
            }
        } else {
            // LATCH
            if (input_pins[4] && !ppins[input_pins[4] - 1].value)
                latchs[0] = value;
            if (input_pins[5] && !ppins[input_pins[5] - 1].value)
                latchs[1] = value;
            if (input_pins[6] && !ppins[input_pins[6] - 1].value)
                latchs[2] = value;
            if (input_pins[7] && !ppins[input_pins[7] - 1].value)
                latchs[3] = value;

            for (i = 0; i < 8; i++) {
                if (latchs[0] & (0x01 << i))
                    alm1[i]++;
                if (latchs[1] & (0x01 << i))
                    alm2[i]++;
                if (latchs[2] & (0x01 << i))
                    alm3[i]++;
                if (latchs[3] & (0x01 << i))
                    alm4[i]++;
            }
        }

        mcount = -1;
    }
}

void cpart_7s_display_dec::PostProcess(void) {
    long int NSTEPJ = PICSimLab.GetNSTEPJ();

    for (int i = 0; i < 8; i++) {
        lm1[i] = (int)((lm1[i] + (((600.0 * alm1[i]) / NSTEPJ) + 30)) / 2.0);
        lm2[i] = (int)((lm2[i] + (((600.0 * alm2[i]) / NSTEPJ) + 30)) / 2.0);
        lm3[i] = (int)((lm3[i] + (((600.0 * alm3[i]) / NSTEPJ) + 30)) / 2.0);
        lm4[i] = (int)((lm4[i] + (((600.0 * alm4[i]) / NSTEPJ) + 30)) / 2.0);
        if (lm1[i] > 255)
            lm1[i] = 255;
        if (lm2[i] > 255)
            lm2[i] = 255;
        if (lm3[i] > 255)
            lm3[i] = 255;
        if (lm4[i] > 255)
            lm4[i] = 255;
    }

    for (int i = 0; i < 8; i++) {
        // 7s DISP
        if (output_ids[O_A1 + i]->value != lm1[i]) {
            output_ids[O_A1 + i]->value = lm1[i];
            output_ids[O_SS1]->update = 1;
        }
        if (output_ids[O_A2 + i]->value != lm2[i]) {
            output_ids[O_A2 + i]->value = lm2[i];
            output_ids[O_SS2]->update = 1;
        }
        if (output_ids[O_A3 + i]->value != lm3[i]) {
            output_ids[O_A3 + i]->value = lm3[i];
            output_ids[O_SS3]->update = 1;
        }
        if (output_ids[O_A4 + i]->value != lm4[i]) {
            output_ids[O_A4 + i]->value = lm4[i];
            output_ids[O_SS4]->update = 1;
        }
    }
}

lxString cpart_7s_display_dec::GetPictureFileName(void) {
    switch (dtype) {
        case 0:
            return lxT("7 Segments Display (Decoder)/7sdisplay_dec.svg");
            break;
        case 1:
            return lxT("7 Segments Display (Decoder)/7sdisplay_latch.svg");
            break;
    }

    return lxT("7 Segments Display (Decoder)/7sdisplay_dec.svg");
}

lxString cpart_7s_display_dec::GetMapFile(void) {
    switch (dtype) {
        case 0:
            return lxT("7 Segments Display (Decoder)/7sdisplay_dec.map");
            break;
        case 1:
            return lxT("7 Segments Display (Decoder)/7sdisplay_latch.map");
            break;
    }

    return lxT("7 Segments Display (Decoder)/7sdisplay_dec.map");
}

void cpart_7s_display_dec::ChangeType(unsigned char tp) {
    // if same
    if (tp == dtype)
        return;

    if (Bitmap) {
        delete Bitmap;
        canvas.Destroy();
    }

    dtype = tp;

    ReadMaps();

    LoadImage();
}

part_init(PART_7S_DISPLAY_DEC_Name, cpart_7s_display_dec, "Output");
