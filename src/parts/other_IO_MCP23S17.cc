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

#include "other_IO_MCP23S17.h"
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
    O_P21,
    O_P22,
    O_P23,
    O_P24,
    O_P25,
    O_P26,
    O_P27,
    O_P28,
    O_IC
};

const char pin_names[28][10] = {"GPB0", "GPB1", "GPB2", "GPB3", "GPB4", "GPB5", "GPB6", "GPB7", "VDD",  "VSS",
                                "/CS",  "SCK",  "SI",   "SO",   "A0",   "A1",   "A2",   "/RST", "INTB", "INTA",
                                "GPA0", "GPA1", "GPA2", "GPA3", "GPA4", "GPA5", "GPA6", "GPA7"};

const char pin_values[28][10] = {
    {18}, {19}, {20}, {21}, {22}, {23}, {24}, {25}, "+5V", "GND", {0},  {1},  {2},  {3},
    {4},  {5},  {6},  {7},  {8},  {9},  {10}, {11}, {12},  {13},  {14}, {15}, {16}, {17},
};

static PCWProp pcwprop[29] = {{PCW_LABEL, "1-GPB0,NC"},
                              {PCW_LABEL, "2-GPB1,NC"},
                              {PCW_LABEL, "3-GPB2,NC"},
                              {PCW_LABEL, "4-GPB3,NC"},
                              {PCW_LABEL, "5-GPB4,NC"},
                              {PCW_LABEL, "6-GPB5,NC"},
                              {PCW_LABEL, "7-GPB6,NC"},
                              {PCW_LABEL, "8-GPB7,NC"},
                              {PCW_LABEL, "9- VCC,+5V"},
                              {PCW_LABEL, "10 -GND,GND"},
                              {PCW_COMBO, "11-/CS"},
                              {PCW_COMBO, "12-SCK"},
                              {PCW_COMBO, "13-SI"},
                              {PCW_COMBO, "14-SO"},
                              {PCW_COMBO, "15-AO"},
                              {PCW_COMBO, "16-A1"},
                              {PCW_COMBO, "17-A2"},
                              {PCW_COMBO, "18-/RST"},
                              {PCW_COMBO, "19-INTB"},
                              {PCW_COMBO, "20-INTA"},
                              {PCW_LABEL, "21-GPA0,NC"},
                              {PCW_LABEL, "22-GPA1,NC"},
                              {PCW_LABEL, "23-GPA2,NC"},
                              {PCW_LABEL, "24-GPA3,NC"},
                              {PCW_LABEL, "25-GPA4,NC"},
                              {PCW_LABEL, "26-GPA5,NC"},
                              {PCW_LABEL, "27-GPA6,NC"},
                              {PCW_LABEL, "28-GPA7,NC"},
                              {PCW_END, ""}};

cpart_IO_MCP23S17::cpart_IO_MCP23S17(const unsigned x, const unsigned y, const char* name, const char* type)
    : part(x, y, name, type), font(8, lxFONTFAMILY_TELETYPE, lxFONTSTYLE_NORMAL, lxFONTWEIGHT_BOLD) {
    X = x;
    Y = y;
    always_update = 1;
    ReadMaps();
    Bitmap = NULL;

    LoadImage();

    io_MCP23X17_init(&mcp);
    io_MCP23X17_rst(&mcp);

    input_pins[0] = 0;
    input_pins[1] = 0;
    input_pins[2] = 0;
    input_pins[3] = 0;
    input_pins[4] = 0;
    input_pins[5] = 0;
    input_pins[6] = 0;
    input_pins[7] = 0;
    input_pins[8] = 0;
    input_pins[9] = 0;

    output_pins[0] = SpareParts.RegisterIOpin(lxT("GPA0"));
    output_pins[1] = SpareParts.RegisterIOpin(lxT("GPA1"));
    output_pins[2] = SpareParts.RegisterIOpin(lxT("GPA2"));
    output_pins[3] = SpareParts.RegisterIOpin(lxT("GPA3"));
    output_pins[4] = SpareParts.RegisterIOpin(lxT("GPA4"));
    output_pins[5] = SpareParts.RegisterIOpin(lxT("GPA5"));
    output_pins[6] = SpareParts.RegisterIOpin(lxT("GPA6"));
    output_pins[7] = SpareParts.RegisterIOpin(lxT("GPA7"));
    output_pins[8] = SpareParts.RegisterIOpin(lxT("GPB0"));
    output_pins[9] = SpareParts.RegisterIOpin(lxT("GPB1"));
    output_pins[10] = SpareParts.RegisterIOpin(lxT("GPB2"));
    output_pins[11] = SpareParts.RegisterIOpin(lxT("GPB3"));
    output_pins[12] = SpareParts.RegisterIOpin(lxT("GPB4"));
    output_pins[13] = SpareParts.RegisterIOpin(lxT("GPB5"));
    output_pins[14] = SpareParts.RegisterIOpin(lxT("GPB6"));
    output_pins[15] = SpareParts.RegisterIOpin(lxT("GPB7"));

    mcount = 0;
    memset(output_pins_alm, 0, 16 * sizeof(unsigned long));

    _PA = 0xFA;  // dummy value
    _PB = 0xFA;

    SetPCWProperties(pcwprop);

    PinCount = 10;
    Pins = input_pins;
}

cpart_IO_MCP23S17::~cpart_IO_MCP23S17(void) {
    for (int i = 0; i < 16; i++)
        SpareParts.UnregisterIOpin(output_pins[i]);
    delete Bitmap;
    canvas.Destroy();
}

void cpart_IO_MCP23S17::DrawOutput(const unsigned int i) {
    switch (output[i].id) {
        case O_IC:
            canvas.SetColor(26, 26, 26);
            canvas.Rectangle(1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);
            canvas.SetFgColor(255, 255, 255);
            canvas.RotatedText("MCP23S17", output[i].x1 + 50, output[i].y2 - 15, 0.0);
            break;
        default:
            canvas.SetColor(49, 61, 99);
            canvas.Rectangle(1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);

            canvas.SetFgColor(255, 255, 255);
            canvas.RotatedText(pin_names[output[i].id - O_P1], output[i].x1, output[i].y2, 90.0);

            int pinv = pin_values[output[i].id - O_P1][0];
            if (pinv > 25) {
                canvas.SetFgColor(155, 155, 155);
                canvas.RotatedText(pin_values[output[i].id - O_P1], output[i].x1, output[i].y2 - 30, 90.0);
            } else if (pinv < 10) {
                if (input_pins[pinv] == 0)
                    canvas.RotatedText("NC", output[i].x1, output[i].y2 - 30, 90.0);
                else
                    canvas.RotatedText(SpareParts.GetPinName(input_pins[pinv]), output[i].x1, output[i].y2 - 30, 90.0);
            } else {
                if (output_pins[pinv - 10] == 0)
                    canvas.RotatedText("NC", output[i].x1, output[i].y2 - 30, 90.0);
                else
                    canvas.RotatedText(itoa(output_pins[pinv - 10]), output[i].x1, output[i].y2 - 30, 90.0);
            }
            break;
    }
}

unsigned short cpart_IO_MCP23S17::GetInputId(char* name) {
    printf("Erro input '%s' don't have a valid id! \n", name);
    return -1;
};

unsigned short cpart_IO_MCP23S17::GetOutputId(char* name) {
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
    if (strcmp(name, "PN_21") == 0)
        return O_P21;
    if (strcmp(name, "PN_22") == 0)
        return O_P22;
    if (strcmp(name, "PN_23") == 0)
        return O_P23;
    if (strcmp(name, "PN_24") == 0)
        return O_P24;
    if (strcmp(name, "PN_25") == 0)
        return O_P25;
    if (strcmp(name, "PN_26") == 0)
        return O_P26;
    if (strcmp(name, "PN_27") == 0)
        return O_P27;
    if (strcmp(name, "PN_28") == 0)
        return O_P28;

    if (strcmp(name, "IC_28") == 0)
        return O_IC;

    printf("Erro output '%s' don't have a valid id! \n", name);
    return 1;
};

lxString cpart_IO_MCP23S17::WritePreferences(void) {
    char prefs[256];

    sprintf(prefs, "%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu", input_pins[0], input_pins[1],
            input_pins[2], input_pins[3], input_pins[4], input_pins[5], input_pins[6], input_pins[7], input_pins[8],
            input_pins[9], output_pins[0]);

    return prefs;
}

void cpart_IO_MCP23S17::ReadPreferences(lxString value) {
    unsigned char outp;
    sscanf(value.c_str(), "%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu", &input_pins[0], &input_pins[1],
           &input_pins[2], &input_pins[3], &input_pins[4], &input_pins[5], &input_pins[6], &input_pins[7],
           &input_pins[8], &input_pins[9], &outp);

    if (output_pins[0] != outp) {
        for (int i = 0; i < 9; i++)
            SpareParts.UnregisterIOpin(output_pins[i]);

        output_pins[0] = SpareParts.RegisterIOpin(lxT("GPA0"), outp++);
        output_pins[1] = SpareParts.RegisterIOpin(lxT("GPA1"), outp++);
        output_pins[2] = SpareParts.RegisterIOpin(lxT("GPA2"), outp++);
        output_pins[3] = SpareParts.RegisterIOpin(lxT("GPA3"), outp++);
        output_pins[4] = SpareParts.RegisterIOpin(lxT("GPA4"), outp++);
        output_pins[5] = SpareParts.RegisterIOpin(lxT("GPA5"), outp++);
        output_pins[6] = SpareParts.RegisterIOpin(lxT("GPA6"), outp++);
        output_pins[7] = SpareParts.RegisterIOpin(lxT("GPA7"), outp++);
        output_pins[8] = SpareParts.RegisterIOpin(lxT("GPB0"), outp++);
        output_pins[9] = SpareParts.RegisterIOpin(lxT("GPB1"), outp++);
        output_pins[10] = SpareParts.RegisterIOpin(lxT("GPB2"), outp++);
        output_pins[11] = SpareParts.RegisterIOpin(lxT("GPB3"), outp++);
        output_pins[12] = SpareParts.RegisterIOpin(lxT("GPB4"), outp++);
        output_pins[13] = SpareParts.RegisterIOpin(lxT("GPB5"), outp++);
        output_pins[14] = SpareParts.RegisterIOpin(lxT("GPB6"), outp++);
        output_pins[15] = SpareParts.RegisterIOpin(lxT("GPB7"), outp++);
    }

    Reset();
}

void cpart_IO_MCP23S17::ConfigurePropertiesWindow(CPWindow* WProp) {
    lxString spin;

    for (int i = 0; i < 28; i++) {
        lxString value = "";

        int pinv = pin_values[i][0];
        if (pinv > 25) {
            value = lxString(pin_values[i]);
        } else if (pinv > 9) {
            if (output_pins[pinv - 10] == 0)
                value = "NC";
            else
                value = itoa(output_pins[pinv - 10]);
        }

        ((CLabel*)WProp->GetChildByName("label" + itoa(i + 1)))->SetText(itoa(i + 1) + lxT("-") + pin_names[i]);

        CLabel* label = (CLabel*)WProp->GetChildByName("label_" + itoa(i + 1));
        if (label) {
            label->SetText(value);
        }
    }

    SetPCWComboWithPinNames(WProp, "combo11", input_pins[0]);
    SetPCWComboWithPinNames(WProp, "combo12", input_pins[1]);
    SetPCWComboWithPinNames(WProp, "combo13", input_pins[2]);
    SetPCWComboWithPinNames(WProp, "combo14", input_pins[3]);
    SetPCWComboWithPinNames(WProp, "combo15", input_pins[4]);
    SetPCWComboWithPinNames(WProp, "combo16", input_pins[5]);
    SetPCWComboWithPinNames(WProp, "combo17", input_pins[6]);
    SetPCWComboWithPinNames(WProp, "combo18", input_pins[7]);
    SetPCWComboWithPinNames(WProp, "combo19", input_pins[8]);
    SetPCWComboWithPinNames(WProp, "combo20", input_pins[9]);
}

void cpart_IO_MCP23S17::ReadPropertiesWindow(CPWindow* WProp) {
    input_pins[0] = GetPWCComboSelectedPin(WProp, "combo11");
    input_pins[1] = GetPWCComboSelectedPin(WProp, "combo12");
    input_pins[2] = GetPWCComboSelectedPin(WProp, "combo13");
    input_pins[3] = GetPWCComboSelectedPin(WProp, "combo14");
    input_pins[4] = GetPWCComboSelectedPin(WProp, "combo15");
    input_pins[5] = GetPWCComboSelectedPin(WProp, "combo16");
    input_pins[6] = GetPWCComboSelectedPin(WProp, "combo17");
    input_pins[7] = GetPWCComboSelectedPin(WProp, "combo18");
    input_pins[8] = GetPWCComboSelectedPin(WProp, "combo19");
    input_pins[9] = GetPWCComboSelectedPin(WProp, "combo20");
}

void cpart_IO_MCP23S17::PreProcess(void) {
    memset(output_pins_alm, 0, 16 * sizeof(unsigned long));
    JUMPSTEPS_ = PICSimLab.GetJUMPSTEPS();
    mcount = JUMPSTEPS_;
}

void cpart_IO_MCP23S17::Process(void) {
    const picpin* ppins = SpareParts.GetPinsValues();

    /*
    "/CS", 0
    "SCK", 1
    "SI",  2
    "SO",
    "A0",
    "A1",
    "A2",
    "/RST",7
    "INTB",
    "INTA",
     */

    // TODO only write support implemented
    io_MCP23X17_SPI_io(&mcp, ppins[input_pins[2] - 1].value, ppins[input_pins[1] - 1].value,
                       ppins[input_pins[7] - 1].value, ppins[input_pins[0] - 1].value);

    if (_PA != mcp.regs[OLATA]) {
        SpareParts.WritePin(output_pins[0], (mcp.regs[OLATA] & 0x01) != 0);
        SpareParts.WritePin(output_pins[1], (mcp.regs[OLATA] & 0x02) != 0);
        SpareParts.WritePin(output_pins[2], (mcp.regs[OLATA] & 0x04) != 0);
        SpareParts.WritePin(output_pins[3], (mcp.regs[OLATA] & 0x08) != 0);
        SpareParts.WritePin(output_pins[4], (mcp.regs[OLATA] & 0x10) != 0);
        SpareParts.WritePin(output_pins[5], (mcp.regs[OLATA] & 0x20) != 0);
        SpareParts.WritePin(output_pins[6], (mcp.regs[OLATA] & 0x40) != 0);
        SpareParts.WritePin(output_pins[7], (mcp.regs[OLATA] & 0x80) != 0);
    }

    if (_PB != mcp.regs[OLATB]) {
        SpareParts.WritePin(output_pins[8], (mcp.regs[OLATB] & 0x01) != 0);
        SpareParts.WritePin(output_pins[9], (mcp.regs[OLATB] & 0x02) != 0);
        SpareParts.WritePin(output_pins[10], (mcp.regs[OLATB] & 0x04) != 0);
        SpareParts.WritePin(output_pins[11], (mcp.regs[OLATB] & 0x08) != 0);
        SpareParts.WritePin(output_pins[12], (mcp.regs[OLATB] & 0x10) != 0);
        SpareParts.WritePin(output_pins[13], (mcp.regs[OLATB] & 0x20) != 0);
        SpareParts.WritePin(output_pins[14], (mcp.regs[OLATB] & 0x40) != 0);
        SpareParts.WritePin(output_pins[15], (mcp.regs[OLATB] & 0x80) != 0);
    }
    _PA = mcp.regs[OLATA];
    _PB = mcp.regs[OLATB];

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
        if (ppins[output_pins[9] - 1].value)
            output_pins_alm[9]++;
        if (ppins[output_pins[10] - 1].value)
            output_pins_alm[10]++;
        if (ppins[output_pins[11] - 1].value)
            output_pins_alm[11]++;
        if (ppins[output_pins[12] - 1].value)
            output_pins_alm[12]++;
        if (ppins[output_pins[13] - 1].value)
            output_pins_alm[13]++;
        if (ppins[output_pins[14] - 1].value)
            output_pins_alm[14]++;
        if (ppins[output_pins[15] - 1].value)
            output_pins_alm[15]++;
        mcount = -1;
    }
}

void cpart_IO_MCP23S17::PostProcess(void) {
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
    SpareParts.WritePinOA(output_pins[9],
                          (ppins[output_pins[9] - 1].oavalue + ((output_pins_alm[9] * 200.0) / NSTEPJ) + 55) / 2);
    SpareParts.WritePinOA(output_pins[10],
                          (ppins[output_pins[10] - 1].oavalue + ((output_pins_alm[10] * 200.0) / NSTEPJ) + 55) / 2);
    SpareParts.WritePinOA(output_pins[11],
                          (ppins[output_pins[11] - 1].oavalue + ((output_pins_alm[11] * 200.0) / NSTEPJ) + 55) / 2);
    SpareParts.WritePinOA(output_pins[12],
                          (ppins[output_pins[12] - 1].oavalue + ((output_pins_alm[12] * 200.0) / NSTEPJ) + 55) / 2);
    SpareParts.WritePinOA(output_pins[13],
                          (ppins[output_pins[13] - 1].oavalue + ((output_pins_alm[13] * 200.0) / NSTEPJ) + 55) / 2);
    SpareParts.WritePinOA(output_pins[14],
                          (ppins[output_pins[14] - 1].oavalue + ((output_pins_alm[14] * 200.0) / NSTEPJ) + 55) / 2);
    SpareParts.WritePinOA(output_pins[15],
                          (ppins[output_pins[15] - 1].oavalue + ((output_pins_alm[15] * 200.0) / NSTEPJ) + 55) / 2);
}

part_init(PART_IO_MCP23S17_Name, cpart_IO_MCP23S17, "Other");
