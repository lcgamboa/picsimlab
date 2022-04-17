/* ########################################################################

   PICsimLab - PIC laboratory simulator

   ########################################################################

   Copyright (c) : 2019-2022  Luis Claudio Gambôa Lopes

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

#include "part_IO_MM74C922.h"
#include "../picsimlab1.h"
#include "../picsimlab4.h"
#include "../picsimlab5.h"

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

cpart_IO_MM74C922::cpart_IO_MM74C922(unsigned x, unsigned y)
    : font(8, lxFONTFAMILY_TELETYPE, lxFONTSTYLE_NORMAL, lxFONTWEIGHT_BOLD) {
    X = x;
    Y = y;
    aways_update = 1;
    ReadMaps();
    Bitmap = NULL;

    LoadImage();

    io_MM74C922_init(&kc);
    io_MM74C922_rst(&kc);

    input_pins[0] = Window5.RegisterIOpin(lxT("RY1"));
    input_pins[1] = Window5.RegisterIOpin(lxT("RY2"));
    input_pins[2] = Window5.RegisterIOpin(lxT("RY3"));
    input_pins[3] = Window5.RegisterIOpin(lxT("RY4"));

    output_pins[0] = Window5.RegisterIOpin(lxT("CX4"));
    output_pins[1] = Window5.RegisterIOpin(lxT("CX3"));
    output_pins[2] = Window5.RegisterIOpin(lxT("CX2"));
    output_pins[3] = Window5.RegisterIOpin(lxT("CX1"));
    output_pins[4] = 0;
    output_pins[5] = 0;
    output_pins[6] = 0;
    output_pins[7] = 0;
    output_pins[8] = 0;

    Window5.SetPinDir(input_pins[0], PD_IN);
    Window5.SetPinDir(input_pins[1], PD_IN);
    Window5.SetPinDir(input_pins[2], PD_IN);
    Window5.SetPinDir(input_pins[3], PD_IN);

    mcount = 0;
    memset(pins_alm, 0, 8 * sizeof(unsigned long));
    _ret = 0;
}

cpart_IO_MM74C922::~cpart_IO_MM74C922(void) {
    for (int i = 0; i < 8; i++)
        Window5.UnregisterIOpin(output_pins[i]);
    delete Bitmap;
    canvas.Destroy();
}

void cpart_IO_MM74C922::Draw(void) {
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
                case O_IC:
                    canvas.SetColor(26, 26, 26);
                    canvas.Rectangle(1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1,
                                     output[i].y2 - output[i].y1);
                    canvas.SetFgColor(255, 255, 255);
                    canvas.RotatedText("MM74C922", output[i].x1 + 26, output[i].y2 - 15, 0.0);
                    break;
                default:
                    canvas.SetColor(49, 61, 99);
                    canvas.Rectangle(1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1,
                                     output[i].y2 - output[i].y1);

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
                            canvas.RotatedText(itoa(input_pins[pinv]) /*Window5.GetPinName(input_pins[pinv])*/,
                                               output[i].x1, output[i].y2 - 30, 90.0);
                    } else if (pinv < 8) {
                        if (output_pins[pinv - 4] == 0)
                            canvas.RotatedText("NC", output[i].x1, output[i].y2 - 30, 90.0);
                        else
                            canvas.RotatedText(itoa(output_pins[pinv - 4]) /*Window5.GetPinName(input_pins[pinv])*/,
                                               output[i].x1, output[i].y2 - 30, 90.0);
                    } else {
                        if (output_pins[pinv - 4] == 0)
                            canvas.RotatedText("NC", output[i].x1, output[i].y2 - 30, 90.0);
                        else
                            canvas.RotatedText(Window5.GetPinName(output_pins[pinv - 4]), output[i].x1,
                                               output[i].y2 - 30, 90.0);
                    }
                    break;
            }
        }
    }

    if (Update) {
        canvas.End();
    }
}

unsigned short cpart_IO_MM74C922::get_in_id(char* name) {
    printf("Erro input '%s' don't have a valid id! \n", name);
    return -1;
};

unsigned short cpart_IO_MM74C922::get_out_id(char* name) {
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
            Window5.UnregisterIOpin(input_pins[i]);
            Window5.UnregisterIOpin(output_pins[i]);
        }
        input_pins[0] = Window5.RegisterIOpin(lxT("RY1"), inp++);
        input_pins[1] = Window5.RegisterIOpin(lxT("RY2"), inp++);
        input_pins[2] = Window5.RegisterIOpin(lxT("RY3"), inp++);
        input_pins[3] = Window5.RegisterIOpin(lxT("RY4"), inp++);

        output_pins[0] = Window5.RegisterIOpin(lxT("CX4"), inp++);
        output_pins[1] = Window5.RegisterIOpin(lxT("CX3"), inp++);
        output_pins[2] = Window5.RegisterIOpin(lxT("CX2"), inp++);
        output_pins[3] = Window5.RegisterIOpin(lxT("CX1"), inp++);

        Window5.SetPinDir(input_pins[0], PD_IN);
        Window5.SetPinDir(input_pins[1], PD_IN);
        Window5.SetPinDir(input_pins[2], PD_IN);
        Window5.SetPinDir(input_pins[3], PD_IN);
    }

    Reset();
}

void cpart_IO_MM74C922::ConfigurePropertiesWindow(CPWindow* WProp) {
    lxString Items = Window5.GetPinsNames();
    lxString spin;

    for (int i = 0; i < 18; i++) {
        lxString value = "";

        int pinv = pin_values[i][0];
        if (pinv > 16) {
            value = lxT("          ") + lxString(pin_values[i]);
        } else if (pinv < 4) {
            if (input_pins[pinv] == 0)
                value = "          NC";
            else
                value = lxT("          ") +
                        itoa(input_pins[pinv]);  // + lxT (" ") + Window5.GetPinName (output_pins[pinv - 4]);
        } else if (pinv < 8) {
            if (output_pins[pinv - 4] == 0)
                value = "          NC";
            else
                value = lxT("          ") +
                        itoa(output_pins[pinv - 4]);  // + lxT (" ") + Window5.GetPinName (output_pins[pinv - 4]);
        }

        ((CLabel*)WProp->GetChildByName("label" + itoa(i + 1)))->SetText(itoa(i + 1) + lxT("-") + pin_names[i] + value);
    }

    ((CCombo*)WProp->GetChildByName("combo1"))->SetItems(Items);
    if (output_pins[5] == 0)
        ((CCombo*)WProp->GetChildByName("combo1"))->SetText("0  NC");
    else {
        spin = Window5.GetPinName(output_pins[5]);
        ((CCombo*)WProp->GetChildByName("combo1"))->SetText(itoa(output_pins[5]) + "  " + spin);
    }

    ((CCombo*)WProp->GetChildByName("combo2"))->SetItems(Items);
    if (output_pins[6] == 0)
        ((CCombo*)WProp->GetChildByName("combo2"))->SetText("0  NC");
    else {
        spin = Window5.GetPinName(output_pins[6]);
        ((CCombo*)WProp->GetChildByName("combo2"))->SetText(itoa(output_pins[6]) + "  " + spin);
    }

    ((CCombo*)WProp->GetChildByName("combo3"))->SetItems(Items);
    if (output_pins[7] == 0)
        ((CCombo*)WProp->GetChildByName("combo3"))->SetText("0  NC");
    else {
        spin = Window5.GetPinName(output_pins[7]);
        ((CCombo*)WProp->GetChildByName("combo3"))->SetText(itoa(output_pins[7]) + "  " + spin);
    }

    ((CCombo*)WProp->GetChildByName("combo4"))->SetItems(Items);
    if (output_pins[8] == 0)
        ((CCombo*)WProp->GetChildByName("combo4"))->SetText("0  NC");
    else {
        spin = Window5.GetPinName(output_pins[8]);
        ((CCombo*)WProp->GetChildByName("combo4"))->SetText(itoa(output_pins[8]) + "  " + spin);
    }

    ((CCombo*)WProp->GetChildByName("combo5"))->SetItems(Items);
    if (output_pins[4] == 0)
        ((CCombo*)WProp->GetChildByName("combo5"))->SetText("0  NC");
    else {
        spin = Window5.GetPinName(output_pins[4]);
        ((CCombo*)WProp->GetChildByName("combo5"))->SetText(itoa(output_pins[4]) + "  " + spin);
    }

    ((CButton*)WProp->GetChildByName("button1"))->EvMouseButtonRelease =
        EVMOUSEBUTTONRELEASE & CPWindow5::PropButtonRelease;
    ((CButton*)WProp->GetChildByName("button1"))->SetTag(1);

    ((CButton*)WProp->GetChildByName("button2"))->EvMouseButtonRelease =
        EVMOUSEBUTTONRELEASE & CPWindow5::PropButtonRelease;
}

void cpart_IO_MM74C922::ReadPropertiesWindow(CPWindow* WProp) {
    output_pins[4] = atoi(((CCombo*)WProp->GetChildByName("combo5"))->GetText());
    output_pins[5] = atoi(((CCombo*)WProp->GetChildByName("combo1"))->GetText());
    output_pins[6] = atoi(((CCombo*)WProp->GetChildByName("combo2"))->GetText());
    output_pins[7] = atoi(((CCombo*)WProp->GetChildByName("combo3"))->GetText());
    output_pins[8] = atoi(((CCombo*)WProp->GetChildByName("combo4"))->GetText());
}

void cpart_IO_MM74C922::PreProcess(void) {
    memset(pins_alm, 0, 8 * sizeof(unsigned long));

    JUMPSTEPS_ = Window1.GetJUMPSTEPS();
    mcount = JUMPSTEPS_;

    io_MM74C922_set_clk_freq(&kc, Window1.GetBoard()->MGetInstClockFreq());
}

void cpart_IO_MM74C922::Process(void) {
    const picpin* ppins = Window5.GetPinsValues();

    unsigned short ret;
    unsigned char data = 0;

    data |= (ppins[input_pins[3] - 1].value << 3);
    data |= (ppins[input_pins[2] - 1].value << 2);
    data |= (ppins[input_pins[1] - 1].value << 1);
    data |= (ppins[input_pins[0] - 1].value);

    ret = io_MM74C922_io(&kc, data);

    if (_ret != ret) {
        Window5.WritePin(output_pins[3], (ret & 0x001) != 0);
        Window5.WritePin(output_pins[2], (ret & 0x002) != 0);
        Window5.WritePin(output_pins[1], (ret & 0x004) != 0);
        Window5.WritePin(output_pins[0], (ret & 0x008) != 0);
        Window5.SetPin(output_pins[4], (ret & 0x010) != 0);
        Window5.SetPin(output_pins[8], (ret & 0x020) != 0);
        Window5.SetPin(output_pins[7], (ret & 0x040) != 0);
        Window5.SetPin(output_pins[6], (ret & 0x080) != 0);
        Window5.SetPin(output_pins[5], (ret & 0x100) != 0);
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
    long int NSTEPJ = Window1.GetNSTEPJ();
    const picpin* ppins = Window5.GetPinsValues();

    Window5.WritePinOA(output_pins[0], (ppins[output_pins[0] - 1].oavalue + ((pins_alm[0] * 200.0) / NSTEPJ) + 55) / 2);
    Window5.WritePinOA(output_pins[1], (ppins[output_pins[1] - 1].oavalue + ((pins_alm[1] * 200.0) / NSTEPJ) + 55) / 2);
    Window5.WritePinOA(output_pins[2], (ppins[output_pins[2] - 1].oavalue + ((pins_alm[2] * 200.0) / NSTEPJ) + 55) / 2);
    Window5.WritePinOA(output_pins[3], (ppins[output_pins[3] - 1].oavalue + ((pins_alm[3] * 200.0) / NSTEPJ) + 55) / 2);
    Window5.WritePinOA(input_pins[0], (ppins[input_pins[0] - 1].oavalue + ((pins_alm[4] * 200.0) / NSTEPJ) + 55) / 2);
    Window5.WritePinOA(input_pins[1], (ppins[input_pins[1] - 1].oavalue + ((pins_alm[5] * 200.0) / NSTEPJ) + 55) / 2);
    Window5.WritePinOA(input_pins[2], (ppins[input_pins[2] - 1].oavalue + ((pins_alm[6] * 200.0) / NSTEPJ) + 55) / 2);
    Window5.WritePinOA(input_pins[3], (ppins[input_pins[3] - 1].oavalue + ((pins_alm[7] * 200.0) / NSTEPJ) + 55) / 2);
}

part_init(PART_IO_MM74C922_Name, cpart_IO_MM74C922, "Other");
