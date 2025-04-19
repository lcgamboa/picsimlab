/* ########################################################################

   PICSimLab - Programmable IC Simulator Laboratory

   ########################################################################

   Copyright (c) : 2019-2025  Luis Claudio Gambôa Lopes <lcgamboa@yahoo.com>

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

#include "other_IO_MCP23017.h"
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
                                "NC",   "SCK",  "SDA",  "NC",   "A0",   "A1",   "A2",   "/RST", "INTB", "INTA",
                                "GPA0", "GPA1", "GPA2", "GPA3", "GPA4", "GPA5", "GPA6", "GPA7"};

const char pin_values[28][10] = {
    {16}, {17}, {18}, {19}, {20}, {21}, {22}, {23}, "+5V", "GND", "NC", {0},  {1},  "NC",
    {2},  {3},  {4},  {5},  {6},  {7},  {8},  {9},  {10},  {11},  {12}, {13}, {14}, {15},
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
                              {PCW_LABEL, "10- GND,GND"},
                              {PCW_LABEL, "11-NC"},
                              {PCW_COMBO, "12-SCK"},
                              {PCW_COMBO, "13-SI"},
                              {PCW_LABEL, "14-NC"},
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

cpart_IO_MCP23017::cpart_IO_MCP23017(const unsigned x, const unsigned y, const char* name, const char* type,
                                     board* pboard_, const int id_)
    : part(x, y, name, type, pboard_, id_) {
    X = x;
    Y = y;
    always_update = 1;
    ReadMaps();
    BitmapId = -1;

    LoadPartImage();

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

    output_pins[0] = SpareParts.RegisterIOpin("GPA0");
    output_pins[1] = SpareParts.RegisterIOpin("GPA1");
    output_pins[2] = SpareParts.RegisterIOpin("GPA2");
    output_pins[3] = SpareParts.RegisterIOpin("GPA3");
    output_pins[4] = SpareParts.RegisterIOpin("GPA4");
    output_pins[5] = SpareParts.RegisterIOpin("GPA5");
    output_pins[6] = SpareParts.RegisterIOpin("GPA6");
    output_pins[7] = SpareParts.RegisterIOpin("GPA7");
    output_pins[8] = SpareParts.RegisterIOpin("GPB0");
    output_pins[9] = SpareParts.RegisterIOpin("GPB1");
    output_pins[10] = SpareParts.RegisterIOpin("GPB2");
    output_pins[11] = SpareParts.RegisterIOpin("GPB3");
    output_pins[12] = SpareParts.RegisterIOpin("GPB4");
    output_pins[13] = SpareParts.RegisterIOpin("GPB5");
    output_pins[14] = SpareParts.RegisterIOpin("GPB6");
    output_pins[15] = SpareParts.RegisterIOpin("GPB7");

    mcount = 0;
    memset(output_pins_alm, 0, 16 * sizeof(unsigned long));

    _PA = 0xFA;  // dummy value to force update
    _PB = 0xFA;
    _PA_INT = 0xFA;
    _PB_INT = 0xFA;
    _DIRA = 0xFA;
    _DIRB = 0xFA;

    SetPCWProperties(pcwprop);

    PinCount = 10;
    Pins = input_pins;
}

cpart_IO_MCP23017::~cpart_IO_MCP23017(void) {
    for (int i = 0; i < 16; i++)
        SpareParts.UnregisterIOpin(output_pins[i]);

    SpareParts.SetPartOnDraw(id);
    SpareParts.CanvasCmd({.cmd = CC_FREEBITMAP, .FreeBitmap{BitmapId}});
    SpareParts.CanvasCmd({.cmd = CC_DESTROY});
}

void cpart_IO_MCP23017::DrawOutput(const unsigned int i) {
    switch (output[i].id) {
        case O_IC:
            SpareParts.CanvasCmd({.cmd = CC_SETFONTSIZE, .SetFontSize{8}});
            SpareParts.CanvasCmd({.cmd = CC_SETCOLOR, .SetColor{26, 26, 26}});
            SpareParts.CanvasCmd(
                {.cmd = CC_RECTANGLE,
                 .Rectangle{1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1}});
            SpareParts.CanvasCmd({.cmd = CC_SETFGCOLOR, .SetFgColor{255, 255, 255}});
            SpareParts.CanvasCmd(
                {.cmd = CC_ROTATEDTEXT, .RotatedText{"MCP23017", output[i].x1 + 50, output[i].y2 - 15, 0.0}});
            break;
        default:
            SpareParts.CanvasCmd({.cmd = CC_SETCOLOR, .SetColor{49, 61, 99}});
            SpareParts.CanvasCmd(
                {.cmd = CC_RECTANGLE,
                 .Rectangle{1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1}});

            SpareParts.CanvasCmd({.cmd = CC_SETFGCOLOR, .SetFgColor{255, 255, 255}});
            SpareParts.CanvasCmd({.cmd = CC_ROTATEDTEXT,
                                  .RotatedText{pin_names[output[i].id - O_P1], output[i].x1, output[i].y2, 90.0}});

            int pinv = pin_values[output[i].id - O_P1][0];
            if (pinv > 25) {
                SpareParts.CanvasCmd({.cmd = CC_SETFGCOLOR, .SetFgColor{155, 155, 155}});
                SpareParts.CanvasCmd(
                    {.cmd = CC_ROTATEDTEXT,
                     .RotatedText{pin_values[output[i].id - O_P1], output[i].x1, output[i].y2 - 30, 90.0}});
            } else if (pinv < 8) {
                if (input_pins[pinv] == 0)
                    SpareParts.CanvasCmd(
                        {.cmd = CC_ROTATEDTEXT, .RotatedText{"NC", output[i].x1, output[i].y2 - 30, 90.0}});
                else
                    SpareParts.CanvasCmd({.cmd = CC_ROTATEDTEXT,
                                          .RotatedText{SpareParts.GetPinName(input_pins[pinv]).c_str(), output[i].x1,
                                                       output[i].y2 - 30, 90.0}});
            } else {
                if (output_pins[pinv - 8] == 0)
                    SpareParts.CanvasCmd(
                        {.cmd = CC_ROTATEDTEXT, .RotatedText{"NC", output[i].x1, output[i].y2 - 30, 90.0}});
                else
                    SpareParts.CanvasCmd({.cmd = CC_ROTATEDTEXT,
                                          .RotatedText{std::to_string(output_pins[pinv - 8]).c_str(), output[i].x1,
                                                       output[i].y2 - 30, 90.0}});
            }
            break;
    }
}

unsigned short cpart_IO_MCP23017::GetInputId(char* name) {
    printf("Error input '%s' don't have a valid id! \n", name);
    return INVALID_ID;
};

unsigned short cpart_IO_MCP23017::GetOutputId(char* name) {
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

    printf("Error output '%s' don't have a valid id! \n", name);
    return INVALID_ID;
};

std::string cpart_IO_MCP23017::WritePreferences(void) {
    char prefs[256];

    sprintf(prefs, "%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu", input_pins[0], input_pins[1], input_pins[2],
            input_pins[3], input_pins[4], input_pins[5], input_pins[6], input_pins[7], output_pins[0]);

    return prefs;
}

void cpart_IO_MCP23017::ReadPreferences(std::string value) {
    unsigned char outp;
    sscanf(value.c_str(), "%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu", &input_pins[0], &input_pins[1],
           &input_pins[2], &input_pins[3], &input_pins[4], &input_pins[5], &input_pins[6], &input_pins[7], &outp);

    if (output_pins[0] != outp) {
        for (int i = 0; i < 9; i++)
            SpareParts.UnregisterIOpin(output_pins[i]);

        output_pins[0] = SpareParts.RegisterIOpin("GPA0", outp++);
        output_pins[1] = SpareParts.RegisterIOpin("GPA1", outp++);
        output_pins[2] = SpareParts.RegisterIOpin("GPA2", outp++);
        output_pins[3] = SpareParts.RegisterIOpin("GPA3", outp++);
        output_pins[4] = SpareParts.RegisterIOpin("GPA4", outp++);
        output_pins[5] = SpareParts.RegisterIOpin("GPA5", outp++);
        output_pins[6] = SpareParts.RegisterIOpin("GPA6", outp++);
        output_pins[7] = SpareParts.RegisterIOpin("GPA7", outp++);
        output_pins[8] = SpareParts.RegisterIOpin("GPB0", outp++);
        output_pins[9] = SpareParts.RegisterIOpin("GPB1", outp++);
        output_pins[10] = SpareParts.RegisterIOpin("GPB2", outp++);
        output_pins[11] = SpareParts.RegisterIOpin("GPB3", outp++);
        output_pins[12] = SpareParts.RegisterIOpin("GPB4", outp++);
        output_pins[13] = SpareParts.RegisterIOpin("GPB5", outp++);
        output_pins[14] = SpareParts.RegisterIOpin("GPB6", outp++);
        output_pins[15] = SpareParts.RegisterIOpin("GPB7", outp++);
    }

    Reset();
}

void cpart_IO_MCP23017::ConfigurePropertiesWindow(void) {
    std::string spin;

    for (int i = 0; i < 28; i++) {
        std::string value = "";

        int pinv = pin_values[i][0];
        if (pinv > 25) {
            value = std::string(pin_values[i]);
        } else if (pinv > 7) {
            if (output_pins[pinv - 8] == 0)
                value = "NC";
            else
                value = std::to_string(output_pins[pinv - 8]);
        }

        SpareParts.WPropCmd(("label" + std::to_string(i + 1)).c_str(), PWA_LABELSETTEXT,
                            (std::to_string(i + 1) + "-" + pin_names[i]).c_str());

        SpareParts.WPropCmd(("label_" + std::to_string(i + 1)).c_str(), PWA_LABELSETTEXT, value.c_str());
    }

    SetPCWComboWithPinNames("combo12", input_pins[0]);
    SetPCWComboWithPinNames("combo13", input_pins[1]);
    SetPCWComboWithPinNames("combo15", input_pins[2]);
    SetPCWComboWithPinNames("combo16", input_pins[3]);
    SetPCWComboWithPinNames("combo17", input_pins[4]);
    SetPCWComboWithPinNames("combo18", input_pins[5]);
    SetPCWComboWithPinNames("combo19", input_pins[6]);
    SetPCWComboWithPinNames("combo20", input_pins[7]);
}

void cpart_IO_MCP23017::ReadPropertiesWindow(void) {
    input_pins[0] = GetPWCComboSelectedPin("combo12");
    input_pins[1] = GetPWCComboSelectedPin("combo13");
    input_pins[2] = GetPWCComboSelectedPin("combo15");
    input_pins[3] = GetPWCComboSelectedPin("combo16");
    input_pins[4] = GetPWCComboSelectedPin("combo17");
    input_pins[5] = GetPWCComboSelectedPin("combo18");
    input_pins[6] = GetPWCComboSelectedPin("combo19");
    input_pins[7] = GetPWCComboSelectedPin("combo20");
}
void cpart_IO_MCP23017::Reset(void) {
    io_MCP23X17_rst(&mcp);
    _PA = 0xFA;  // dummy value to force update
    _PB = 0xFA;
    _PA_INT = 0xFA;
    _PB_INT = 0xFA;
    _DIRA = 0xFA;
    _DIRB = 0xFA;
}

void cpart_IO_MCP23017::PreProcess(void) {
    const picpin* ppins = SpareParts.GetPinsValues();
    memset(output_pins_alm, 0, 16 * sizeof(unsigned long));
    JUMPSTEPS_ = PICSimLab.GetJUMPSTEPS() * 4.0 / PICSimLab.GetBoard()->MGetClocksPerInstructions();
    mcount = JUMPSTEPS_;

    if (input_pins[2] && input_pins[3] && input_pins[4]) {
        unsigned char addr = (ppins[input_pins[4] - 1].value << 2) | (ppins[input_pins[3] - 1].value << 1) |
                             (ppins[input_pins[2] - 1].value);
        io_MCP23X17_set_addr(&mcp, addr);
    }

    if (input_pins[1]) {
        SpareParts.ResetPullupBus(input_pins[1] - 1);
    }
}

void cpart_IO_MCP23017::Process(void) {
    const picpin* ppins = SpareParts.GetPinsValues();

    /*
    "NC",
    "SCK", 0
    "SI",  1
    "NC",
    "A0", 2
    "A1", 3
    "A2", 4
    "/RST",5
    "INTB",6
    "INTA",7
     */

    // TODO only write support implemented

    if (ioupdated) {
        if (input_pins[5] && !ppins[input_pins[5] - 1].value) {
            io_MCP23X17_rst(&mcp);
        } else if (input_pins[0] & input_pins[1]) {
            SpareParts.SetPullupBus(input_pins[1] - 1, io_MCP23X17_I2C_io(&mcp, ppins[input_pins[0] - 1].value,
                                                                          ppins[input_pins[1] - 1].value));
        }

        // update pin dir
        if (_DIRA != mcp.regs[IODIRA]) {
            SpareParts.SetPinDir(output_pins[0], (mcp.regs[IODIRA] & 0x01) ? PD_IN : PD_OUT);
            SpareParts.SetPinDir(output_pins[1], (mcp.regs[IODIRA] & 0x02) ? PD_IN : PD_OUT);
            SpareParts.SetPinDir(output_pins[2], (mcp.regs[IODIRA] & 0x04) ? PD_IN : PD_OUT);
            SpareParts.SetPinDir(output_pins[3], (mcp.regs[IODIRA] & 0x08) ? PD_IN : PD_OUT);
            SpareParts.SetPinDir(output_pins[4], (mcp.regs[IODIRA] & 0x10) ? PD_IN : PD_OUT);
            SpareParts.SetPinDir(output_pins[5], (mcp.regs[IODIRA] & 0x20) ? PD_IN : PD_OUT);
            SpareParts.SetPinDir(output_pins[6], (mcp.regs[IODIRA] & 0x40) ? PD_IN : PD_OUT);
            SpareParts.SetPinDir(output_pins[7], (mcp.regs[IODIRA] & 0x80) ? PD_IN : PD_OUT);
        }
        if (_DIRB != mcp.regs[IODIRB]) {
            SpareParts.SetPinDir(output_pins[8], (mcp.regs[IODIRB] & 0x01) ? PD_IN : PD_OUT);
            SpareParts.SetPinDir(output_pins[9], (mcp.regs[IODIRB] & 0x02) ? PD_IN : PD_OUT);
            SpareParts.SetPinDir(output_pins[10], (mcp.regs[IODIRB] & 0x04) ? PD_IN : PD_OUT);
            SpareParts.SetPinDir(output_pins[11], (mcp.regs[IODIRB] & 0x08) ? PD_IN : PD_OUT);
            SpareParts.SetPinDir(output_pins[12], (mcp.regs[IODIRB] & 0x10) ? PD_IN : PD_OUT);
            SpareParts.SetPinDir(output_pins[13], (mcp.regs[IODIRB] & 0x20) ? PD_IN : PD_OUT);
            SpareParts.SetPinDir(output_pins[14], (mcp.regs[IODIRB] & 0x40) ? PD_IN : PD_OUT);
            SpareParts.SetPinDir(output_pins[15], (mcp.regs[IODIRB] & 0x80) ? PD_IN : PD_OUT);
        }
        _DIRA = mcp.regs[IODIRA];
        _DIRB = mcp.regs[IODIRB];

        // update output pins value
        if (_PA != mcp.regs[OLATA]) {
            if (!(mcp.regs[IODIRA] & 0x01))
                SpareParts.WritePin(output_pins[0], (mcp.regs[OLATA] & 0x01) != 0);
            if (!(mcp.regs[IODIRA] & 0x02))
                SpareParts.WritePin(output_pins[1], (mcp.regs[OLATA] & 0x02) != 0);
            if (!(mcp.regs[IODIRA] & 0x04))
                SpareParts.WritePin(output_pins[2], (mcp.regs[OLATA] & 0x04) != 0);
            if (!(mcp.regs[IODIRA] & 0x08))
                SpareParts.WritePin(output_pins[3], (mcp.regs[OLATA] & 0x08) != 0);
            if (!(mcp.regs[IODIRA] & 0x10))
                SpareParts.WritePin(output_pins[4], (mcp.regs[OLATA] & 0x10) != 0);
            if (!(mcp.regs[IODIRA] & 0x20))
                SpareParts.WritePin(output_pins[5], (mcp.regs[OLATA] & 0x20) != 0);
            if (!(mcp.regs[IODIRA] & 0x40))
                SpareParts.WritePin(output_pins[6], (mcp.regs[OLATA] & 0x40) != 0);
            if (!(mcp.regs[IODIRA] & 0x80))
                SpareParts.WritePin(output_pins[7], (mcp.regs[OLATA] & 0x80) != 0);
        }

        if (_PB != mcp.regs[OLATB]) {
            if (!(mcp.regs[IODIRB] & 0x01))
                SpareParts.WritePin(output_pins[8], (mcp.regs[OLATB] & 0x01) != 0);
            if (!(mcp.regs[IODIRB] & 0x02))
                SpareParts.WritePin(output_pins[9], (mcp.regs[OLATB] & 0x02) != 0);
            if (!(mcp.regs[IODIRB] & 0x04))
                SpareParts.WritePin(output_pins[10], (mcp.regs[OLATB] & 0x04) != 0);
            if (!(mcp.regs[IODIRB] & 0x08))
                SpareParts.WritePin(output_pins[11], (mcp.regs[OLATB] & 0x08) != 0);
            if (!(mcp.regs[IODIRB] & 0x10))
                SpareParts.WritePin(output_pins[12], (mcp.regs[OLATB] & 0x10) != 0);
            if (!(mcp.regs[IODIRB] & 0x20))
                SpareParts.WritePin(output_pins[13], (mcp.regs[OLATB] & 0x20) != 0);
            if (!(mcp.regs[IODIRB] & 0x40))
                SpareParts.WritePin(output_pins[14], (mcp.regs[OLATB] & 0x40) != 0);
            if (!(mcp.regs[IODIRB] & 0x80))
                SpareParts.WritePin(output_pins[15], (mcp.regs[OLATB] & 0x80) != 0);
        }
        _PA = mcp.regs[OLATA];
        _PB = mcp.regs[OLATB];
    }

    mcount++;
    if (mcount >= JUMPSTEPS_) {
        // update input pins value
        unsigned char ipa = 0;
        unsigned char ipb = 0;
        for (int bit = 0; bit < 8; bit++) {
            unsigned char bit_mask = 1 << bit;

            if (ppins[output_pins[bit] - 1].value) {
                ipa |= bit_mask;
            }

            if (ppins[output_pins[bit + 8] - 1].value) {
                ipb |= bit_mask;
            }
        }

        io_MCP23X17_set_inputs(&mcp, ipa, ipb);

        if ((_PA_INT != mcp.inta_value) && (input_pins[7])) {
            SpareParts.SetPin(input_pins[7], mcp.inta_value);
        }
        if ((_PB_INT != mcp.intb_value) && (input_pins[6])) {
            SpareParts.SetPin(input_pins[6], mcp.intb_value);
        }
        _PA_INT = mcp.inta_value;
        _PB_INT = mcp.intb_value;

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

void cpart_IO_MCP23017::PostProcess(void) {
    long int NSTEPJ = PICSimLab.GetNSTEPJ();

    SpareParts.WritePinOA(output_pins[0], ((output_pins_alm[0] * 200.0) / NSTEPJ) + 55);
    SpareParts.WritePinOA(output_pins[1], ((output_pins_alm[1] * 200.0) / NSTEPJ) + 55);
    SpareParts.WritePinOA(output_pins[2], ((output_pins_alm[2] * 200.0) / NSTEPJ) + 55);
    SpareParts.WritePinOA(output_pins[3], ((output_pins_alm[3] * 200.0) / NSTEPJ) + 55);
    SpareParts.WritePinOA(output_pins[4], ((output_pins_alm[4] * 200.0) / NSTEPJ) + 55);
    SpareParts.WritePinOA(output_pins[5], ((output_pins_alm[5] * 200.0) / NSTEPJ) + 55);
    SpareParts.WritePinOA(output_pins[6], ((output_pins_alm[6] * 200.0) / NSTEPJ) + 55);
    SpareParts.WritePinOA(output_pins[7], ((output_pins_alm[7] * 200.0) / NSTEPJ) + 55);
    SpareParts.WritePinOA(output_pins[8], ((output_pins_alm[8] * 200.0) / NSTEPJ) + 55);
    SpareParts.WritePinOA(output_pins[9], ((output_pins_alm[9] * 200.0) / NSTEPJ) + 55);
    SpareParts.WritePinOA(output_pins[10], ((output_pins_alm[10] * 200.0) / NSTEPJ) + 55);
    SpareParts.WritePinOA(output_pins[11], ((output_pins_alm[11] * 200.0) / NSTEPJ) + 55);
    SpareParts.WritePinOA(output_pins[12], ((output_pins_alm[12] * 200.0) / NSTEPJ) + 55);
    SpareParts.WritePinOA(output_pins[13], ((output_pins_alm[13] * 200.0) / NSTEPJ) + 55);
    SpareParts.WritePinOA(output_pins[14], ((output_pins_alm[14] * 200.0) / NSTEPJ) + 55);
    SpareParts.WritePinOA(output_pins[15], ((output_pins_alm[15] * 200.0) / NSTEPJ) + 55);
}

part_init(PART_IO_MCP23017_Name, cpart_IO_MCP23017, "Other");
