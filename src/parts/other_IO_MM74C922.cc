/* ########################################################################

   PICSimLab - Programmable IC Simulator Laboratory

   ########################################################################

   Copyright (c) : 2019-2023  Luis Claudio Gambôa Lopes <lcgamboa@yahoo.com>

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

#include "other_IO_MM74C922.h"
#include "../lib/oscilloscope.h"
#include "../lib/picsimlab.h"
#include "../lib/spareparts.h"

/* outputs */
enum {
    O_P1,
    O_P2,
    O_P3,
    O_P4,
    O_P5,
    O_P6,
    O_P7,
    O_P8,
    O_P9,
    O_P10,
    O_P11,
    O_P12,
    O_P13,
    O_P14,
    O_P15,
    O_P16,
    O_P17,
    O_P18,
    O_P19,
    O_P20,
    O_IC
};

const char pin_names[18][10] = {"RY1", "RY2", "RY3", "RY4", "OSC", "KBM", "CX4", "CX3", "GND",
                                "CX2", "CX1", "DA",  "/OE", "D",   "C",   "B",   "A",   "VCC"};

const char pin_values[18][10] = {{0}, {1}, {2}, {3},   "1uF", "0.1uF", {4},  {5},  "GND",
                                 {6}, {7}, {8}, "GND", {9},   {10},    {11}, {12}, "+5V"};

static PCWProp pcwprop[19] = {{PCW_LABEL, "1-RY1,NC"},
                              {PCW_LABEL, "2-RY2,NC"},
                              {PCW_LABEL, "3-RY3,NC"},
                              {PCW_LABEL, "4-RY4,NC"},
                              {PCW_LABEL, "5-OSC,C=1uF"},
                              {PCW_LABEL, "6-KBM,C=0.1uF"},
                              {PCW_LABEL, "7-CX4,NC"},
                              {PCW_LABEL, "8-CX3,NC"},
                              {PCW_LABEL, "9-GND,GND"},
                              {PCW_LABEL, "10-CX2,NC"},
                              {PCW_LABEL, "11-CX1,NC"},
                              {PCW_COMBO, "12-DA"},
                              {PCW_LABEL, "13-/OE,GND"},
                              {PCW_COMBO, "14-D"},
                              {PCW_COMBO, "15-C"},
                              {PCW_COMBO, "16-B"},
                              {PCW_COMBO, "17-A"},
                              {PCW_LABEL, "18-VCC,+5V"},
                              {PCW_END, ""}};

cpart_IO_MM74C922::cpart_IO_MM74C922(const unsigned x, const unsigned y, const char* name, const char* type)
    : part(x, y, name, type), font(8, lxFONTFAMILY_TELETYPE, lxFONTSTYLE_NORMAL, lxFONTWEIGHT_BOLD) {
    X = x;
    Y = y;
    always_update = 1;
    ReadMaps();
    Bitmap = NULL;

    LoadImage();

    io_MM74C922_init(&kc);
    io_MM74C922_rst(&kc);

    input_pins[0] = SpareParts.RegisterIOpin(lxT("RY1"));
    input_pins[1] = SpareParts.RegisterIOpin(lxT("RY2"));
    input_pins[2] = SpareParts.RegisterIOpin(lxT("RY3"));
    input_pins[3] = SpareParts.RegisterIOpin(lxT("RY4"));

    output_pins[0] = SpareParts.RegisterIOpin(lxT("CX4"));
    output_pins[1] = SpareParts.RegisterIOpin(lxT("CX3"));
    output_pins[2] = SpareParts.RegisterIOpin(lxT("CX2"));
    output_pins[3] = SpareParts.RegisterIOpin(lxT("CX1"));
    output_pins[4] = 0;
    output_pins[5] = 0;
    output_pins[6] = 0;
    output_pins[7] = 0;
    output_pins[8] = 0;

    SpareParts.SetPinDir(input_pins[0], PD_IN);
    SpareParts.SetPinDir(input_pins[1], PD_IN);
    SpareParts.SetPinDir(input_pins[2], PD_IN);
    SpareParts.SetPinDir(input_pins[3], PD_IN);

    mcount = 0;
    memset(pins_alm, 0, 8 * sizeof(unsigned long));
    _ret = 0;

    SetPCWProperties(pcwprop);
}

cpart_IO_MM74C922::~cpart_IO_MM74C922(void) {
    for (int i = 0; i < 8; i++)
        SpareParts.UnregisterIOpin(output_pins[i]);
    delete Bitmap;
    canvas.Destroy();
}

void cpart_IO_MM74C922::DrawOutput(const unsigned int i) {
    switch (output[i].id) {
        case O_IC:
            canvas.SetColor(26, 26, 26);
            canvas.Rectangle(1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);
            canvas.SetFgColor(255, 255, 255);
            canvas.RotatedText("MM74C922", output[i].x1 + 26, output[i].y2 - 15, 0.0);
            break;
        default:
            canvas.SetColor(49, 61, 99);
            canvas.Rectangle(1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);

            canvas.SetFgColor(255, 255, 255);
            canvas.RotatedText(pin_names[output[i].id - O_P1], output[i].x1, output[i].y2, 90.0);

            int pinv = pin_values[output[i].id - O_P1][0];
            if (pinv > 16) {
                canvas.SetFgColor(155, 155, 155);
                canvas.RotatedText(pin_values[output[i].id - O_P1], output[i].x1, output[i].y2 - 30, 90.0);
            } else if (pinv < 4) {
                if (input_pins[pinv] == 0)
                    canvas.RotatedText("NC", output[i].x1, output[i].y2 - 30, 90.0);
                else
                    canvas.RotatedText(itoa(input_pins[pinv]) /*SpareParts.GetPinName(input_pins[pinv])*/, output[i].x1,
                                       output[i].y2 - 30, 90.0);
            } else if (pinv < 8) {
                if (output_pins[pinv - 4] == 0)
                    canvas.RotatedText("NC", output[i].x1, output[i].y2 - 30, 90.0);
                else
                    canvas.RotatedText(itoa(output_pins[pinv - 4]) /*SpareParts.GetPinName(input_pins[pinv])*/,
                                       output[i].x1, output[i].y2 - 30, 90.0);
            } else {
                if (output_pins[pinv - 4] == 0)
                    canvas.RotatedText("NC", output[i].x1, output[i].y2 - 30, 90.0);
                else
                    canvas.RotatedText(SpareParts.GetPinName(output_pins[pinv - 4]), output[i].x1, output[i].y2 - 30,
                                       90.0);
            }
            break;
    }
}

unsigned short cpart_IO_MM74C922::GetInputId(char* name) {
    printf("Erro input '%s' don't have a valid id! \n", name);
    return -1;
};

unsigned short cpart_IO_MM74C922::GetOutputId(char* name) {
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
    if (strcmp(name, "PN_9") == 0)
        return O_P9;
    if (strcmp(name, "PN_10") == 0)
        return O_P10;
    if (strcmp(name, "PN_11") == 0)
        return O_P11;
    if (strcmp(name, "PN_12") == 0)
        return O_P12;
    if (strcmp(name, "PN_13") == 0)
        return O_P13;
    if (strcmp(name, "PN_14") == 0)
        return O_P14;
    if (strcmp(name, "PN_15") == 0)
        return O_P15;
    if (strcmp(name, "PN_16") == 0)
        return O_P16;
    if (strcmp(name, "PN_17") == 0)
        return O_P17;
    if (strcmp(name, "PN_18") == 0)
        return O_P18;
    if (strcmp(name, "PN_19") == 0)
        return O_P19;
    if (strcmp(name, "PN_20") == 0)
        return O_P20;
    if (strcmp(name, "IC_20") == 0)
        return O_IC;

    printf("Erro output '%s' don't have a valid id! \n", name);
    return 1;
};

lxString cpart_IO_MM74C922::WritePreferences(void) {
    char prefs[256];

    sprintf(prefs, "%hhu,%hhu,%hhu,%hhu,%hhu,%hhu", output_pins[4], output_pins[5], output_pins[6], output_pins[7],
            output_pins[8], input_pins[0]);

    return prefs;
}

void cpart_IO_MM74C922::ReadPreferences(lxString value) {
    unsigned char inp;
    sscanf(value.c_str(), "%hhu,%hhu,%hhu,%hhu,%hhu,%hhu", &output_pins[4], &output_pins[5], &output_pins[6],
           &output_pins[7], &output_pins[8], &inp);

    if (input_pins[0] != inp) {
        for (int i = 0; i < 4; i++) {
            SpareParts.UnregisterIOpin(input_pins[i]);
            SpareParts.UnregisterIOpin(output_pins[i]);
        }
        input_pins[0] = SpareParts.RegisterIOpin(lxT("RY1"), inp++);
        input_pins[1] = SpareParts.RegisterIOpin(lxT("RY2"), inp++);
        input_pins[2] = SpareParts.RegisterIOpin(lxT("RY3"), inp++);
        input_pins[3] = SpareParts.RegisterIOpin(lxT("RY4"), inp++);

        output_pins[0] = SpareParts.RegisterIOpin(lxT("CX4"), inp++);
        output_pins[1] = SpareParts.RegisterIOpin(lxT("CX3"), inp++);
        output_pins[2] = SpareParts.RegisterIOpin(lxT("CX2"), inp++);
        output_pins[3] = SpareParts.RegisterIOpin(lxT("CX1"), inp++);

        SpareParts.SetPinDir(input_pins[0], PD_IN);
        SpareParts.SetPinDir(input_pins[1], PD_IN);
        SpareParts.SetPinDir(input_pins[2], PD_IN);
        SpareParts.SetPinDir(input_pins[3], PD_IN);
    }

    Reset();
}

void cpart_IO_MM74C922::ConfigurePropertiesWindow(CPWindow* WProp) {
    lxString spin;

    for (int i = 0; i < 18; i++) {
        lxString value = "";

        int pinv = pin_values[i][0];
        if (pinv > 16) {
            value = lxString(pin_values[i]);
        } else if (pinv < 4) {
            if (input_pins[pinv] == 0)
                value = "NC";
            else
                value = itoa(input_pins[pinv]);  // + lxT (" ") + SpareParts.GetPinName (output_pins[pinv - 4]);
        } else if (pinv < 8) {
            if (output_pins[pinv - 4] == 0)
                value = "NC";
            else
                value = itoa(output_pins[pinv - 4]);  // + lxT (" ") + SpareParts.GetPinName (output_pins[pinv - 4]);
        }

        ((CLabel*)WProp->GetChildByName("label" + itoa(i + 1)))->SetText(itoa(i + 1) + lxT("-") + pin_names[i]);

        CLabel* label = (CLabel*)WProp->GetChildByName("label_" + itoa(i + 1));
        if (label) {
            label->SetText(value);
        }
    }

    SetPCWComboWithPinNames(WProp, "combo12", output_pins[4]);
    SetPCWComboWithPinNames(WProp, "combo14", output_pins[5]);
    SetPCWComboWithPinNames(WProp, "combo15", output_pins[6]);
    SetPCWComboWithPinNames(WProp, "combo16", output_pins[7]);
    SetPCWComboWithPinNames(WProp, "combo17", output_pins[8]);
}

void cpart_IO_MM74C922::ReadPropertiesWindow(CPWindow* WProp) {
    output_pins[4] = GetPWCComboSelectedPin(WProp, "combo12");
    output_pins[5] = GetPWCComboSelectedPin(WProp, "combo14");
    output_pins[6] = GetPWCComboSelectedPin(WProp, "combo15");
    output_pins[7] = GetPWCComboSelectedPin(WProp, "combo16");
    output_pins[8] = GetPWCComboSelectedPin(WProp, "combo17");
}

void cpart_IO_MM74C922::PreProcess(void) {
    memset(pins_alm, 0, 8 * sizeof(unsigned long));

    JUMPSTEPS_ = PICSimLab.GetJUMPSTEPS();
    mcount = JUMPSTEPS_;

    io_MM74C922_set_clk_freq(&kc, PICSimLab.GetBoard()->MGetInstClockFreq());
}

void cpart_IO_MM74C922::Process(void) {
    const picpin* ppins = SpareParts.GetPinsValues();

    unsigned short ret;
    unsigned char data = 0;

    data |= (ppins[input_pins[3] - 1].value << 3);
    data |= (ppins[input_pins[2] - 1].value << 2);
    data |= (ppins[input_pins[1] - 1].value << 1);
    data |= (ppins[input_pins[0] - 1].value);

    ret = io_MM74C922_io(&kc, data);

    if (_ret != ret) {
        SpareParts.WritePin(output_pins[3], (ret & 0x001) != 0);
        SpareParts.WritePin(output_pins[2], (ret & 0x002) != 0);
        SpareParts.WritePin(output_pins[1], (ret & 0x004) != 0);
        SpareParts.WritePin(output_pins[0], (ret & 0x008) != 0);
        SpareParts.SetPin(output_pins[4], (ret & 0x010) != 0);
        SpareParts.SetPin(output_pins[8], (ret & 0x020) != 0);
        SpareParts.SetPin(output_pins[7], (ret & 0x040) != 0);
        SpareParts.SetPin(output_pins[6], (ret & 0x080) != 0);
        SpareParts.SetPin(output_pins[5], (ret & 0x100) != 0);
    }
    _ret = ret;

    mcount++;
    if (mcount >= JUMPSTEPS_) {
        if (ppins[output_pins[0] - 1].value)
            pins_alm[0]++;
        if (ppins[output_pins[1] - 1].value)
            pins_alm[1]++;
        if (ppins[output_pins[2] - 1].value)
            pins_alm[2]++;
        if (ppins[output_pins[3] - 1].value)
            pins_alm[3]++;
        if (ppins[input_pins[0] - 1].value)
            pins_alm[4]++;
        if (ppins[input_pins[1] - 1].value)
            pins_alm[5]++;
        if (ppins[input_pins[2] - 1].value)
            pins_alm[6]++;
        if (ppins[input_pins[3] - 1].value)
            pins_alm[7]++;
        mcount = -1;
    }
}

void cpart_IO_MM74C922::PostProcess(void) {
    long int NSTEPJ = PICSimLab.GetNSTEPJ();
    const picpin* ppins = SpareParts.GetPinsValues();

    SpareParts.WritePinOA(output_pins[0],
                          (ppins[output_pins[0] - 1].oavalue + ((pins_alm[0] * 200.0) / NSTEPJ) + 55) / 2);
    SpareParts.WritePinOA(output_pins[1],
                          (ppins[output_pins[1] - 1].oavalue + ((pins_alm[1] * 200.0) / NSTEPJ) + 55) / 2);
    SpareParts.WritePinOA(output_pins[2],
                          (ppins[output_pins[2] - 1].oavalue + ((pins_alm[2] * 200.0) / NSTEPJ) + 55) / 2);
    SpareParts.WritePinOA(output_pins[3],
                          (ppins[output_pins[3] - 1].oavalue + ((pins_alm[3] * 200.0) / NSTEPJ) + 55) / 2);
    SpareParts.WritePinOA(input_pins[0],
                          (ppins[input_pins[0] - 1].oavalue + ((pins_alm[4] * 200.0) / NSTEPJ) + 55) / 2);
    SpareParts.WritePinOA(input_pins[1],
                          (ppins[input_pins[1] - 1].oavalue + ((pins_alm[5] * 200.0) / NSTEPJ) + 55) / 2);
    SpareParts.WritePinOA(input_pins[2],
                          (ppins[input_pins[2] - 1].oavalue + ((pins_alm[6] * 200.0) / NSTEPJ) + 55) / 2);
    SpareParts.WritePinOA(input_pins[3],
                          (ppins[input_pins[3] - 1].oavalue + ((pins_alm[7] * 200.0) / NSTEPJ) + 55) / 2);
}

part_init(PART_IO_MM74C922_Name, cpart_IO_MM74C922, "Other");
