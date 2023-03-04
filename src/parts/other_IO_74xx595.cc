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

#include "other_IO_74xx595.h"
#include "../lib/oscilloscope.h"
#include "../lib/picsimlab.h"
#include "../lib/spareparts.h"

/* outputs */
enum { O_P1, O_P2, O_P3, O_P4, O_P5, O_P6, O_P7, O_P8, O_P9, O_P10, O_P11, O_P12, O_P13, O_P14, O_P15, O_P16, O_IC };

const char pin_names[16][10] = {"QB",   "QC",   "QD",   "QE",   "QF",  "QG",  "QH", "GND",
                                "SOUT", "/RST", "SCLK", "LCLK", "/OE", "SER", "QA", "VCC"};

const char pin_values[16][10] = {{5},  {6}, {7}, {8}, {9},   {10}, {11}, "GND",
                                 {12}, {0}, {1}, {2}, "GND", {3},  {4},  "+5V"};

static PCWProp pcwprop[16] = {
    {PCW_LABEL, "1-QB,NC"},    {PCW_LABEL, "2-QC,NC"}, {PCW_LABEL, "3-QD,NC"},  {PCW_LABEL, "4-QE,NC"},
    {PCW_LABEL, "5-QF,NC"},    {PCW_LABEL, "6-QG,NC"}, {PCW_LABEL, "7-QH,NC"},  {PCW_LABEL, "8-GND,GND"},
    {PCW_LABEL, "9-SOUT,NC"},  {PCW_COMBO, "10-/RST"}, {PCW_COMBO, "11-SCLK"},  {PCW_COMBO, "12-LCLK"},
    {PCW_LABEL, "13-/OE,GND"}, {PCW_COMBO, "14-SER"},  {PCW_LABEL, "15-QA,NC"}, {PCW_LABEL, "16-VCC,+5V"}};

cpart_IO_74xx595::cpart_IO_74xx595(const unsigned x, const unsigned y, const char* name, const char* type)
    : part(x, y, name, type, 8) {
    always_update = 1;

    io_74xx595_init(&sr8);
    io_74xx595_rst(&sr8);

    input_pins[0] = 0;
    input_pins[1] = 0;
    input_pins[2] = 0;
    input_pins[3] = 0;

    output_pins[0] = SpareParts.RegisterIOpin(lxT("QA"));
    output_pins[1] = SpareParts.RegisterIOpin(lxT("QB"));
    output_pins[2] = SpareParts.RegisterIOpin(lxT("QC"));
    output_pins[3] = SpareParts.RegisterIOpin(lxT("QD"));
    output_pins[4] = SpareParts.RegisterIOpin(lxT("QE"));
    output_pins[5] = SpareParts.RegisterIOpin(lxT("QF"));
    output_pins[6] = SpareParts.RegisterIOpin(lxT("QG"));
    output_pins[7] = SpareParts.RegisterIOpin(lxT("QH"));
    output_pins[8] = SpareParts.RegisterIOpin(lxT("SOUT"));

    mcount = 0;
    memset(output_pins_alm, 0, 9 * sizeof(unsigned long));
    _ret = 0;

    SetPCWProperties(pcwprop, 16);

    PinCount = 4;
    Pins = input_pins;
}

cpart_IO_74xx595::~cpart_IO_74xx595(void) {
    for (int i = 0; i < 9; i++)
        SpareParts.UnregisterIOpin(output_pins[i]);
    delete Bitmap;
    canvas.Destroy();
}

void cpart_IO_74xx595::DrawOutput(const unsigned int i) {
    switch (output[i].id) {
        case O_IC:
            canvas.SetColor(26, 26, 26);
            canvas.Rectangle(1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);
            canvas.SetFgColor(255, 255, 255);
            canvas.RotatedText("74XX595", output[i].x1 + 16, output[i].y2 - 15, 0.0);
            break;
        default:
            canvas.SetColor(49, 61, 99);
            canvas.Rectangle(1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);

            canvas.SetFgColor(255, 255, 255);
            canvas.RotatedText(pin_names[output[i].id - O_P1], output[i].x1, output[i].y2, 90.0);

            int pinv = pin_values[output[i].id - O_P1][0];
            if (pinv > 12) {
                canvas.SetFgColor(155, 155, 155);
                canvas.RotatedText(pin_values[output[i].id - O_P1], output[i].x1, output[i].y2 - 30, 90.0);
            } else if (pinv < 4) {
                if (input_pins[pinv] == 0)
                    canvas.RotatedText("NC", output[i].x1, output[i].y2 - 30, 90.0);
                else
                    canvas.RotatedText(SpareParts.GetPinName(input_pins[pinv]), output[i].x1, output[i].y2 - 30, 90.0);
            } else {
                if (output_pins[pinv - 4] == 0)
                    canvas.RotatedText("NC", output[i].x1, output[i].y2 - 30, 90.0);
                else
                    canvas.RotatedText(
                        itoa(output_pins[pinv - 4]) /* + lxT (" ") + SpareParts.GetPinName (output_pins[pinv - 4])*/,
                        output[i].x1, output[i].y2 - 30, 90.0);
            }
            break;
    }
}

unsigned short cpart_IO_74xx595::GetInputId(char* name) {
    printf("Erro input '%s' don't have a valid id! \n", name);
    return -1;
};

unsigned short cpart_IO_74xx595::GetOutputId(char* name) {
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

    if (strcmp(name, "IC_16") == 0)
        return O_IC;

    printf("Erro output '%s' don't have a valid id! \n", name);
    return 1;
};

lxString cpart_IO_74xx595::WritePreferences(void) {
    char prefs[256];

    sprintf(prefs, "%hhu,%hhu,%hhu,%hhu,%hhu", input_pins[0], input_pins[1], input_pins[2], input_pins[3],
            output_pins[0]);

    return prefs;
}

void cpart_IO_74xx595::ReadPreferences(lxString value) {
    unsigned char outp;
    sscanf(value.c_str(), "%hhu,%hhu,%hhu,%hhu,%hhu", &input_pins[0], &input_pins[1], &input_pins[2], &input_pins[3],
           &outp);

    if (output_pins[0] != outp) {
        for (int i = 0; i < 9; i++)
            SpareParts.UnregisterIOpin(output_pins[i]);

        output_pins[0] = SpareParts.RegisterIOpin(lxT("QA"), outp++);
        output_pins[1] = SpareParts.RegisterIOpin(lxT("QB"), outp++);
        output_pins[2] = SpareParts.RegisterIOpin(lxT("QC"), outp++);
        output_pins[3] = SpareParts.RegisterIOpin(lxT("QD"), outp++);
        output_pins[4] = SpareParts.RegisterIOpin(lxT("QE"), outp++);
        output_pins[5] = SpareParts.RegisterIOpin(lxT("QF"), outp++);
        output_pins[6] = SpareParts.RegisterIOpin(lxT("QG"), outp++);
        output_pins[7] = SpareParts.RegisterIOpin(lxT("QH"), outp++);
        output_pins[8] = SpareParts.RegisterIOpin(lxT("SOUT"), outp++);
    }

    Reset();
}

void cpart_IO_74xx595::ConfigurePropertiesWindow(CPWindow* WProp) {
    lxString Items = SpareParts.GetPinsNames();
    lxString spin;

    for (int i = 0; i < 16; i++) {
        lxString value = "";

        int pinv = pin_values[i][0];
        if (pinv > 12) {
            value = lxString(pin_values[i]);
        } else if (pinv >= 4) {
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

    SetPCWComboWithPinNames(WProp, "combo10", input_pins[0]);
    SetPCWComboWithPinNames(WProp, "combo11", input_pins[1]);
    SetPCWComboWithPinNames(WProp, "combo12", input_pins[2]);
    SetPCWComboWithPinNames(WProp, "combo14", input_pins[3]);
}

void cpart_IO_74xx595::ReadPropertiesWindow(CPWindow* WProp) {
    input_pins[0] = GetPWCComboSelectedPin(WProp, "combo10");
    input_pins[1] = GetPWCComboSelectedPin(WProp, "combo11");
    input_pins[2] = GetPWCComboSelectedPin(WProp, "combo12");
    input_pins[3] = GetPWCComboSelectedPin(WProp, "combo14");
}

void cpart_IO_74xx595::PreProcess(void) {
    memset(output_pins_alm, 0, 9 * sizeof(unsigned long));

    JUMPSTEPS_ = PICSimLab.GetJUMPSTEPS();
    mcount = JUMPSTEPS_;
}

void cpart_IO_74xx595::Process(void) {
    const picpin* ppins = SpareParts.GetPinsValues();

    unsigned short ret;

    ret = io_74xx595_io(&sr8, ppins[input_pins[3] - 1].value, ppins[input_pins[1] - 1].value,
                        ppins[input_pins[2] - 1].value, ppins[input_pins[0] - 1].value);

    if (_ret != ret) {
        SpareParts.WritePin(output_pins[0], (ret & 0x01) != 0);
        SpareParts.WritePin(output_pins[1], (ret & 0x02) != 0);
        SpareParts.WritePin(output_pins[2], (ret & 0x04) != 0);
        SpareParts.WritePin(output_pins[3], (ret & 0x08) != 0);
        SpareParts.WritePin(output_pins[4], (ret & 0x10) != 0);
        SpareParts.WritePin(output_pins[5], (ret & 0x20) != 0);
        SpareParts.WritePin(output_pins[6], (ret & 0x40) != 0);
        SpareParts.WritePin(output_pins[7], (ret & 0x80) != 0);
        SpareParts.WritePin(output_pins[8], (ret & 0x100) != 0);
    }
    _ret = ret;

    mcount++;
    if (mcount >= JUMPSTEPS_) {
        if (ppins[output_pins[0] - 1].value)
            output_pins_alm[0]++;
        if (ppins[output_pins[1] - 1].value)
            output_pins_alm[1]++;
        if (ppins[output_pins[2] - 1].value)
            output_pins_alm[2]++;
        if (ppins[output_pins[3] - 1].value)
            output_pins_alm[3]++;
        if (ppins[output_pins[4] - 1].value)
            output_pins_alm[4]++;
        if (ppins[output_pins[5] - 1].value)
            output_pins_alm[5]++;
        if (ppins[output_pins[6] - 1].value)
            output_pins_alm[6]++;
        if (ppins[output_pins[7] - 1].value)
            output_pins_alm[7]++;

        if (ppins[output_pins[8] - 1].value)
            output_pins_alm[8]++;

        mcount = -1;
    }
}

void cpart_IO_74xx595::PostProcess(void) {
    long int NSTEPJ = PICSimLab.GetNSTEPJ();
    const picpin* ppins = SpareParts.GetPinsValues();

    SpareParts.WritePinOA(output_pins[0],
                          (ppins[output_pins[0] - 1].oavalue + ((output_pins_alm[0] * 200.0) / NSTEPJ) + 55) / 2);
    SpareParts.WritePinOA(output_pins[1],
                          (ppins[output_pins[1] - 1].oavalue + ((output_pins_alm[1] * 200.0) / NSTEPJ) + 55) / 2);
    SpareParts.WritePinOA(output_pins[2],
                          (ppins[output_pins[2] - 1].oavalue + ((output_pins_alm[2] * 200.0) / NSTEPJ) + 55) / 2);
    SpareParts.WritePinOA(output_pins[3],
                          (ppins[output_pins[3] - 1].oavalue + ((output_pins_alm[3] * 200.0) / NSTEPJ) + 55) / 2);
    SpareParts.WritePinOA(output_pins[4],
                          (ppins[output_pins[4] - 1].oavalue + ((output_pins_alm[4] * 200.0) / NSTEPJ) + 55) / 2);
    SpareParts.WritePinOA(output_pins[5],
                          (ppins[output_pins[5] - 1].oavalue + ((output_pins_alm[5] * 200.0) / NSTEPJ) + 55) / 2);
    SpareParts.WritePinOA(output_pins[6],
                          (ppins[output_pins[6] - 1].oavalue + ((output_pins_alm[6] * 200.0) / NSTEPJ) + 55) / 2);
    SpareParts.WritePinOA(output_pins[7],
                          (ppins[output_pins[7] - 1].oavalue + ((output_pins_alm[7] * 200.0) / NSTEPJ) + 55) / 2);
    SpareParts.WritePinOA(output_pins[8],
                          (ppins[output_pins[8] - 1].oavalue + ((output_pins_alm[8] * 200.0) / NSTEPJ) + 55) / 2);
}

part_init(PART_IO_74XX595_Name, cpart_IO_74xx595, "Other");
