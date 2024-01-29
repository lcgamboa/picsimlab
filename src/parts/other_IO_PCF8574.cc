/* ########################################################################

   PICSimLab - Programmable IC Simulator Laboratory

   ########################################################################

   Copyright (c) : 2019-2024  Luis Claudio Gambôa Lopes <lcgamboa@yahoo.com>

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

#include "other_IO_PCF8574.h"
#include "../lib/oscilloscope.h"
#include "../lib/picsimlab.h"
#include "../lib/spareparts.h"

/* outputs */
enum { O_P1, O_P2, O_P3, O_P4, O_P5, O_P6, O_P7, O_P8, O_P9, O_P10, O_P11, O_P12, O_P13, O_P14, O_P15, O_P16, O_IC };

const char pin_names[16][10] = {"A0", "A1", "A2", "P0", "P1",   "P2",  "P3",  "VSS",
                                "P4", "P5", "P6", "P7", "/INT", "SCL", "SDA", "VDD"};

const char pin_values[16][10] = {{2}, {3},  {4},  {5},  {6},  {7}, {8}, "GND",
                                 {9}, {10}, {11}, {12}, {13}, {0}, {1}, "+5V"};

static PCWProp pcwprop[17] = {{PCW_COMBO, "1-A0"},
                              {PCW_COMBO, "2-A1"},
                              {PCW_COMBO, "3-A2"},
                              {PCW_LABEL, "4-P0,NC"},
                              {PCW_LABEL, "5-P1,NC"},
                              {PCW_LABEL, "6-P2,NC"},
                              {PCW_LABEL, "7-P3,NC"},
                              {PCW_LABEL, "8-GND,GND"},
                              {PCW_LABEL, "9-P4,NC"},
                              {PCW_LABEL, "10-P5,NC"},
                              {PCW_LABEL, "11-P6,NC"},
                              {PCW_LABEL, "12-P7,NC"},
                              {PCW_LABEL, "13-/INT,NC"},
                              {PCW_COMBO, "14-SCL"},
                              {PCW_COMBO, "15-SDA"},
                              {PCW_LABEL, "16-VDD,+5V"},
                              {PCW_END, ""}};

cpart_IO_PCF8574::cpart_IO_PCF8574(const unsigned x, const unsigned y, const char* name, const char* type,
                                   board* pboard_, const int id_)
    : part(x, y, name, type, pboard_, id_, 8) {
    X = x;
    Y = y;
    always_update = 1;
    ReadMaps();
    Bitmap = NULL;

    LoadPartImage();

    io_PCF8574_init(&ioe8);
    io_PCF8574_rst(&ioe8);

    input_pins[0] = 0;
    input_pins[1] = 0;
    input_pins[2] = 0;
    input_pins[3] = 0;
    input_pins[4] = 0;

    output_pins[0] = SpareParts.RegisterIOpin("P0");
    output_pins[1] = SpareParts.RegisterIOpin("P1");
    output_pins[2] = SpareParts.RegisterIOpin("P2");
    output_pins[3] = SpareParts.RegisterIOpin("P3");
    output_pins[4] = SpareParts.RegisterIOpin("P4");
    output_pins[5] = SpareParts.RegisterIOpin("P5");
    output_pins[6] = SpareParts.RegisterIOpin("P6");
    output_pins[7] = SpareParts.RegisterIOpin("P7");
    output_pins[8] = SpareParts.RegisterIOpin("/INT");

    mcount = 0;
    memset(output_pins_alm, 0, 9 * sizeof(unsigned long));

    _ret = 255;

    SetPCWProperties(pcwprop);

    PinCount = 5;
    Pins = input_pins;
}

cpart_IO_PCF8574::~cpart_IO_PCF8574(void) {
    for (int i = 0; i < 9; i++)
        SpareParts.UnregisterIOpin(output_pins[i]);
    delete Bitmap;
    SpareParts.SetPartOnDraw(id);
    SpareParts.CanvasCmd({CC_DESTROY});
}

const unsigned char* cpart_IO_PCF8574::GetOutputPins(void) {
    return output_pins;
}

void cpart_IO_PCF8574::DrawOutput(const unsigned int i) {
    switch (output[i].id) {
        case O_IC:
            SpareParts.CanvasCmd({CC_SETCOLOR, .SetColor{26, 26, 26}});
            SpareParts.CanvasCmd({CC_RECTANGLE, .Rectangle{1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1,
                                                           output[i].y2 - output[i].y1}});
            SpareParts.CanvasCmd({CC_SETFGCOLOR, .SetFgColor{255, 255, 255}});
            SpareParts.CanvasCmd({CC_ROTATEDTEXT, .RotatedText{"PCF8574", output[i].x1 + 14, output[i].y2 - 15, 0.0}});
            break;
        default:
            SpareParts.CanvasCmd({CC_SETCOLOR, .SetColor{49, 61, 99}});
            SpareParts.CanvasCmd({CC_RECTANGLE, .Rectangle{1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1,
                                                           output[i].y2 - output[i].y1}});

            SpareParts.CanvasCmd({CC_SETFGCOLOR, .SetFgColor{255, 255, 255}});
            SpareParts.CanvasCmd(
                {CC_ROTATEDTEXT, .RotatedText{pin_names[output[i].id - O_P1], output[i].x1, output[i].y2, 90.0}});

            int pinv = pin_values[output[i].id - O_P1][0];
            if (pinv > 13) {
                SpareParts.CanvasCmd({CC_SETFGCOLOR, .SetFgColor{155, 155, 155}});
                SpareParts.CanvasCmd({CC_ROTATEDTEXT, .RotatedText{pin_values[output[i].id - O_P1], output[i].x1,
                                                                   output[i].y2 - 30, 90.0}});
            } else if (pinv < 5) {
                if (input_pins[pinv] == 0)
                    SpareParts.CanvasCmd({CC_ROTATEDTEXT, .RotatedText{"NC", output[i].x1, output[i].y2 - 30, 90.0}});
                else
                    SpareParts.CanvasCmd({CC_ROTATEDTEXT, .RotatedText{SpareParts.GetPinName(input_pins[pinv]).c_str(),
                                                                       output[i].x1, output[i].y2 - 30, 90.0}});
            } else {
                if (output_pins[pinv - 5] == 0)
                    SpareParts.CanvasCmd({CC_ROTATEDTEXT, .RotatedText{"NC", output[i].x1, output[i].y2 - 30, 90.0}});
                else
                    SpareParts.CanvasCmd(
                        {CC_ROTATEDTEXT,
                         .RotatedText{std::to_string(output_pins[pinv - 5])
                                          .c_str() /*+ " " + SpareParts.GetPinName (output_pins[pinv - 5])*/,
                                      output[i].x1, output[i].y2 - 30, 90.0}});
            }
            break;
    }
}

unsigned short cpart_IO_PCF8574::GetInputId(char* name) {
    printf("Error input '%s' don't have a valid id! \n", name);
    return INVALID_ID;
};

unsigned short cpart_IO_PCF8574::GetOutputId(char* name) {
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

    printf("Error output '%s' don't have a valid id! \n", name);
    return INVALID_ID;
};

std::string cpart_IO_PCF8574::WritePreferences(void) {
    char prefs[256];

    sprintf(prefs, "%hhu,%hhu,%hhu,%hhu,%hhu,%hhu", input_pins[0], input_pins[1], input_pins[2], input_pins[3],
            input_pins[4], output_pins[0]);

    return prefs;
}

void cpart_IO_PCF8574::ReadPreferences(std::string value) {
    unsigned char outp;
    sscanf(value.c_str(), "%hhu,%hhu,%hhu,%hhu,%hhu,%hhu", &input_pins[0], &input_pins[1], &input_pins[2],
           &input_pins[3], &input_pins[4], &outp);

    if (output_pins[0] != outp) {
        for (int i = 0; i < 9; i++)
            SpareParts.UnregisterIOpin(output_pins[i]);

        output_pins[0] = SpareParts.RegisterIOpin("P0", outp++);
        output_pins[1] = SpareParts.RegisterIOpin("P1", outp++);
        output_pins[2] = SpareParts.RegisterIOpin("P2", outp++);
        output_pins[3] = SpareParts.RegisterIOpin("P3", outp++);
        output_pins[4] = SpareParts.RegisterIOpin("P4", outp++);
        output_pins[5] = SpareParts.RegisterIOpin("P5", outp++);
        output_pins[6] = SpareParts.RegisterIOpin("P6", outp++);
        output_pins[7] = SpareParts.RegisterIOpin("P7", outp++);
        output_pins[8] = SpareParts.RegisterIOpin("/INT", outp++);
    }

    Reset();
}

void cpart_IO_PCF8574::ConfigurePropertiesWindow(CPWindow* WProp) {
    std::string spin;

    for (int i = 0; i < 16; i++) {
        std::string value = "";

        int pinv = pin_values[i][0];
        if (pinv > 13) {
            value = std::string(pin_values[i]);
        } else if (pinv >= 5) {
            if (output_pins[pinv - 5] == 0)
                value = "NC";
            else
                value = std::to_string(output_pins[pinv - 5]);  //+ " " + SpareParts.GetPinName (output_pins[pinv - 5]);
        }

        ((CLabel*)WProp->GetChildByName("label" + std::to_string(i + 1)))
            ->SetText(std::to_string(i + 1) + "-" + pin_names[i]);

        CLabel* label = (CLabel*)WProp->GetChildByName("label_" + std::to_string(i + 1));
        if (label) {
            label->SetText(value);
        }
    }

    SetPCWComboWithPinNames(WProp, "combo1", input_pins[2]);
    SetPCWComboWithPinNames(WProp, "combo2", input_pins[3]);
    SetPCWComboWithPinNames(WProp, "combo3", input_pins[4]);
    SetPCWComboWithPinNames(WProp, "combo14", input_pins[0]);
    SetPCWComboWithPinNames(WProp, "combo15", input_pins[1]);
}

void cpart_IO_PCF8574::ReadPropertiesWindow(CPWindow* WProp) {
    input_pins[0] = GetPWCComboSelectedPin(WProp, "combo14");
    input_pins[1] = GetPWCComboSelectedPin(WProp, "combo15");
    input_pins[2] = GetPWCComboSelectedPin(WProp, "combo1");
    input_pins[3] = GetPWCComboSelectedPin(WProp, "combo2");
    input_pins[4] = GetPWCComboSelectedPin(WProp, "combo3");
}

void cpart_IO_PCF8574::PreProcess(void) {
    const picpin* ppins = SpareParts.GetPinsValues();
    unsigned char addr = 0x27;

    memset(output_pins_alm, 0, 9 * sizeof(unsigned long));

    JUMPSTEPS_ = PICSimLab.GetJUMPSTEPS();
    mcount = JUMPSTEPS_;

    if (input_pins[2]) {
        if (!ppins[input_pins[2] - 1].value)
            addr &= ~0x01;
    }
    if (input_pins[3]) {
        if (!ppins[input_pins[3] - 1].value)
            addr &= ~0x02;
    }
    if (input_pins[4]) {
        if (!ppins[input_pins[4] - 1].value)
            addr &= ~0x04;
    }

    io_PCF8574_set_addr(&ioe8, addr);

    if (input_pins[1] > 0) {
        SpareParts.ResetPullupBus(input_pins[1] - 1);
    }
}

void cpart_IO_PCF8574::Process(void) {
    const picpin* ppins = SpareParts.GetPinsValues();

    if (ioupdated) {
        ioe8.dataOut = 0x00;
        ioe8.dataOut |= ppins[output_pins[0] - 1].lsvalue;
        ioe8.dataOut |= ppins[output_pins[1] - 1].lsvalue << 1;
        ioe8.dataOut |= ppins[output_pins[2] - 1].lsvalue << 2;
        ioe8.dataOut |= ppins[output_pins[3] - 1].lsvalue << 3;
        ioe8.dataOut |= ppins[output_pins[4] - 1].lsvalue << 4;
        ioe8.dataOut |= ppins[output_pins[5] - 1].lsvalue << 5;
        ioe8.dataOut |= ppins[output_pins[6] - 1].lsvalue << 6;
        ioe8.dataOut |= ppins[output_pins[7] - 1].lsvalue << 7;
        ioe8.dataOut &= ioe8.dataIn;  // mask with input

        if ((input_pins[0] > 0) && (input_pins[1] > 0))
            SpareParts.SetPullupBus(input_pins[1] - 1, io_PCF8574_I2C_io(&ioe8, ppins[input_pins[0] - 1].value,
                                                                         ppins[input_pins[1] - 1].value));

        if (_ret != ioe8.dataIn) {
            SpareParts.WritePin(output_pins[0], (ioe8.dataIn & 0x01) != 0);
            SpareParts.WritePin(output_pins[1], (ioe8.dataIn & 0x02) != 0);
            SpareParts.WritePin(output_pins[2], (ioe8.dataIn & 0x04) != 0);
            SpareParts.WritePin(output_pins[3], (ioe8.dataIn & 0x08) != 0);
            SpareParts.WritePin(output_pins[4], (ioe8.dataIn & 0x10) != 0);
            SpareParts.WritePin(output_pins[5], (ioe8.dataIn & 0x20) != 0);
            SpareParts.WritePin(output_pins[6], (ioe8.dataIn & 0x40) != 0);
            SpareParts.WritePin(output_pins[7], (ioe8.dataIn & 0x80) != 0);
            // SpareParts.WritePin (output_pins[8], (ioe8.data & 0x100) != 0);
        }
        _ret = ioe8.dataIn;
    }

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

void cpart_IO_PCF8574::PostProcess(void) {
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

part_init(PART_IO_PCF8574_Name, cpart_IO_PCF8574, "Other");
