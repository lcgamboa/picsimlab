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

#include "other_jumpers.h"
#include "../lib/oscilloscope.h"
#include "../lib/picsimlab.h"
#include "../lib/spareparts.h"

/* outputs */
enum {
    O_I1,
    O_I2,
    O_I3,
    O_I4,
    O_I5,
    O_I6,
    O_I7,
    O_I8,
    O_I9,
    O_I10,
    O_I11,
    O_I12,
    O_I13,
    O_I14,
    O_I15,
    O_I16,
    O_O1,
    O_O2,
    O_O3,
    O_O4,
    O_O5,
    O_O6,
    O_O7,
    O_O8,
    O_O9,
    O_O10,
    O_O11,
    O_O12,
    O_O13,
    O_O14,
    O_O15,
    O_O16,
    O_L1,
    O_L2,
    O_L3,
    O_L4,
    O_L5,
    O_L6,
    O_L7,
    O_L8,
    O_L9,
    O_L10,
    O_L11,
    O_L12,
    O_L13,
    O_L14,
    O_L15,
    O_L16
};

static PCWProp pcwprop[35] = {
    {PCW_COMBO, "Input Type"}, {PCW_COMBO, "1-I0"},        {PCW_COMBO, "2-I1"},  {PCW_COMBO, "3-I2"},
    {PCW_COMBO, "4-I3"},       {PCW_COMBO, "5-I4"},        {PCW_COMBO, "6-I5"},  {PCW_COMBO, "7-I6"},
    {PCW_COMBO, "8-I7"},       {PCW_COMBO, "9-I8"},        {PCW_COMBO, "10-I9"}, {PCW_COMBO, "11-IA"},
    {PCW_COMBO, "12-IB"},      {PCW_COMBO, "13-IC"},       {PCW_COMBO, "14-ID"}, {PCW_COMBO, "15-IE"},
    {PCW_COMBO, "16-IF"},      {PCW_COMBO, "Output Type"}, {PCW_COMBO, "1-O0"},  {PCW_COMBO, "2-O1"},
    {PCW_COMBO, "3-O2"},       {PCW_COMBO, "4-O3"},        {PCW_COMBO, "5-O4"},  {PCW_COMBO, "6-O5"},
    {PCW_COMBO, "7-O6"},       {PCW_COMBO, "8-O7"},        {PCW_COMBO, "9-O8"},  {PCW_COMBO, "10-O9"},
    {PCW_COMBO, "11-OA"},      {PCW_COMBO, "12-OB"},       {PCW_COMBO, "13-OC"}, {PCW_COMBO, "14-OD"},
    {PCW_COMBO, "15-OE"},      {PCW_COMBO, "16-OF"},       {PCW_END, ""}};

int cpart_Jumpers::countID = 0;

cpart_Jumpers::cpart_Jumpers(const unsigned x, const unsigned y, const char* name, const char* type, board* pboard_,
                             const int id_)
    : part(x, y, name, type, pboard_, id_) {
    char buff[2];

    ID = countID++;

    X = x;
    Y = y;
    always_update = 1;
    ReadMaps();
    BitmapId = -1;

    LoadPartImage();

    buff[0] = 'A' + ID;
    buff[1] = 0;

    jname = buff;

    input_pins[0] = SpareParts.RegisterIOpin(jname + "I0", 0, PD_IN);
    input_pins[1] = SpareParts.RegisterIOpin(jname + "I1", 0, PD_IN);
    input_pins[2] = SpareParts.RegisterIOpin(jname + "I2", 0, PD_IN);
    input_pins[3] = SpareParts.RegisterIOpin(jname + "I3", 0, PD_IN);
    input_pins[4] = SpareParts.RegisterIOpin(jname + "I4", 0, PD_IN);
    input_pins[5] = SpareParts.RegisterIOpin(jname + "I5", 0, PD_IN);
    input_pins[6] = SpareParts.RegisterIOpin(jname + "I6", 0, PD_IN);
    input_pins[7] = SpareParts.RegisterIOpin(jname + "I7", 0, PD_IN);
    input_pins[8] = SpareParts.RegisterIOpin(jname + "I8", 0, PD_IN);
    input_pins[9] = SpareParts.RegisterIOpin(jname + "I9", 0, PD_IN);
    input_pins[10] = SpareParts.RegisterIOpin(jname + "IA", 0, PD_IN);
    input_pins[11] = SpareParts.RegisterIOpin(jname + "IB", 0, PD_IN);
    input_pins[12] = SpareParts.RegisterIOpin(jname + "IC", 0, PD_IN);
    input_pins[13] = SpareParts.RegisterIOpin(jname + "ID", 0, PD_IN);
    input_pins[14] = SpareParts.RegisterIOpin(jname + "IE", 0, PD_IN);
    input_pins[15] = SpareParts.RegisterIOpin(jname + "IF", 0, PD_IN);

    output_pins[0] = SpareParts.RegisterIOpin(jname + "O0");
    output_pins[1] = SpareParts.RegisterIOpin(jname + "O1");
    output_pins[2] = SpareParts.RegisterIOpin(jname + "O2");
    output_pins[3] = SpareParts.RegisterIOpin(jname + "O3");
    output_pins[4] = SpareParts.RegisterIOpin(jname + "O4");
    output_pins[5] = SpareParts.RegisterIOpin(jname + "O5");
    output_pins[6] = SpareParts.RegisterIOpin(jname + "O6");
    output_pins[7] = SpareParts.RegisterIOpin(jname + "O7");
    output_pins[8] = SpareParts.RegisterIOpin(jname + "O8");
    output_pins[9] = SpareParts.RegisterIOpin(jname + "O9");
    output_pins[10] = SpareParts.RegisterIOpin(jname + "OA");
    output_pins[11] = SpareParts.RegisterIOpin(jname + "OB");
    output_pins[12] = SpareParts.RegisterIOpin(jname + "OC");
    output_pins[13] = SpareParts.RegisterIOpin(jname + "OD");
    output_pins[14] = SpareParts.RegisterIOpin(jname + "OE");
    output_pins[15] = SpareParts.RegisterIOpin(jname + "OF");

    jtype = JWT_MM;

    mcount = 0;
    memset(output_pins_alm, 0, 16 * sizeof(unsigned long));

    SetPCWProperties(pcwprop);
}

cpart_Jumpers::~cpart_Jumpers(void) {
    for (int i = 0; i < 16; i++) {
        SpareParts.UnregisterIOpin(input_pins[i]);
        SpareParts.UnregisterIOpin(output_pins[i]);
    }
    SpareParts.SetPartOnDraw(id);
    SpareParts.CanvasCmd({CC_FREEBITMAP, .FreeBitmap{BitmapId}});
    SpareParts.CanvasCmd({CC_DESTROY});
}

void cpart_Jumpers::DrawOutput(const unsigned int i) {
    const picpin* ppins = SpareParts.GetPinsValues();
    char pname[256];
    unsigned char c;

    switch (output[i].id) {
        case O_L1 ... O_L16:
            c = ppins[output_pins[output[i].id - O_L1] - 1].oavalue;
            SpareParts.CanvasCmd({CC_SETCOLOR, .SetColor{c, c, 0}});
            SpareParts.CanvasCmd({CC_RECTANGLE, .Rectangle{1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1,
                                                           output[i].y2 - output[i].y1}});
            break;
        case O_I1 ... O_I16:
            SpareParts.CanvasCmd({CC_SETFONTSIZE, .SetFontSize{10}});
            SpareParts.CanvasCmd({CC_SETCOLOR, .SetColor{49, 61, 99}});
            SpareParts.CanvasCmd({CC_RECTANGLE, .Rectangle{1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1,
                                                           output[i].y2 - output[i].y1}});
            snprintf(pname, 256, "%2i-%s", output[i].id - O_I1 + 1,
                     SpareParts.GetPinName(input_pins[output[i].id - O_I1]).c_str());
            SpareParts.CanvasCmd({CC_SETFGCOLOR, .SetFgColor{255, 255, 255}});
            SpareParts.CanvasCmd({CC_ROTATEDTEXT, .RotatedText{pname, output[i].x1, output[i].y2, 90.0}});
            break;
        case O_O1 ... O_O16:
            SpareParts.CanvasCmd({CC_SETCOLOR, .SetColor{49, 61, 99}});
            SpareParts.CanvasCmd({CC_RECTANGLE, .Rectangle{1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1,
                                                           output[i].y2 - output[i].y1}});
            snprintf(pname, 256, "%2i-%s", output[i].id - O_O1 + 1,
                     SpareParts.GetPinName(output_pins[output[i].id - O_O1]).c_str());
            SpareParts.CanvasCmd({CC_SETFGCOLOR, .SetFgColor{255, 255, 255}});
            SpareParts.CanvasCmd({CC_ROTATEDTEXT, .RotatedText{pname, output[i].x1, output[i].y2, 90.0}});
            break;
        default:
            break;
    }
}

unsigned short cpart_Jumpers::GetInputId(char* name) {
    printf("Error input '%s' don't have a valid id! \n", name);
    return INVALID_ID;
};

unsigned short cpart_Jumpers::GetOutputId(char* name) {
    if (strcmp(name, "PN_I1") == 0)
        return O_I1;
    if (strcmp(name, "PN_I2") == 0)
        return O_I2;
    if (strcmp(name, "PN_I3") == 0)
        return O_I3;
    if (strcmp(name, "PN_I4") == 0)
        return O_I4;
    if (strcmp(name, "PN_I5") == 0)
        return O_I5;
    if (strcmp(name, "PN_I6") == 0)
        return O_I6;
    if (strcmp(name, "PN_I7") == 0)
        return O_I7;
    if (strcmp(name, "PN_I8") == 0)
        return O_I8;
    if (strcmp(name, "PN_I9") == 0)
        return O_I9;
    if (strcmp(name, "PN_I10") == 0)
        return O_I10;
    if (strcmp(name, "PN_I11") == 0)
        return O_I11;
    if (strcmp(name, "PN_I12") == 0)
        return O_I12;
    if (strcmp(name, "PN_I13") == 0)
        return O_I13;
    if (strcmp(name, "PN_I14") == 0)
        return O_I14;
    if (strcmp(name, "PN_I15") == 0)
        return O_I15;
    if (strcmp(name, "PN_I16") == 0)
        return O_I16;

    if (strcmp(name, "PN_O1") == 0)
        return O_O1;
    if (strcmp(name, "PN_O2") == 0)
        return O_O2;
    if (strcmp(name, "PN_O3") == 0)
        return O_O3;
    if (strcmp(name, "PN_O4") == 0)
        return O_O4;
    if (strcmp(name, "PN_O5") == 0)
        return O_O5;
    if (strcmp(name, "PN_O6") == 0)
        return O_O6;
    if (strcmp(name, "PN_O7") == 0)
        return O_O7;
    if (strcmp(name, "PN_O8") == 0)
        return O_O8;
    if (strcmp(name, "PN_O9") == 0)
        return O_O9;
    if (strcmp(name, "PN_O10") == 0)
        return O_O10;
    if (strcmp(name, "PN_O11") == 0)
        return O_O11;
    if (strcmp(name, "PN_O12") == 0)
        return O_O12;
    if (strcmp(name, "PN_O13") == 0)
        return O_O13;
    if (strcmp(name, "PN_O14") == 0)
        return O_O14;
    if (strcmp(name, "PN_O15") == 0)
        return O_O15;
    if (strcmp(name, "PN_O16") == 0)
        return O_O16;

    if (strcmp(name, "LD_1") == 0)
        return O_L1;
    if (strcmp(name, "LD_2") == 0)
        return O_L2;
    if (strcmp(name, "LD_3") == 0)
        return O_L3;
    if (strcmp(name, "LD_4") == 0)
        return O_L4;
    if (strcmp(name, "LD_5") == 0)
        return O_L5;
    if (strcmp(name, "LD_6") == 0)
        return O_L6;
    if (strcmp(name, "LD_7") == 0)
        return O_L7;
    if (strcmp(name, "LD_8") == 0)
        return O_L8;
    if (strcmp(name, "LD_9") == 0)
        return O_L9;
    if (strcmp(name, "LD_10") == 0)
        return O_L10;
    if (strcmp(name, "LD_11") == 0)
        return O_L11;
    if (strcmp(name, "LD_12") == 0)
        return O_L12;
    if (strcmp(name, "LD_13") == 0)
        return O_L13;
    if (strcmp(name, "LD_14") == 0)
        return O_L14;
    if (strcmp(name, "LD_15") == 0)
        return O_L15;
    if (strcmp(name, "LD_16") == 0)
        return O_L16;

    printf("Error output '%s' don't have a valid id! \n", name);
    return INVALID_ID;
};

std::string cpart_Jumpers::WritePreferences(void) {
    char prefs[256];

    char c = jname[0];
    sprintf(prefs,
            "%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%"
            "hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%c",
            input_pins[0], input_pins[1], input_pins[2], input_pins[3], input_pins[4], input_pins[5], input_pins[6],
            input_pins[7], input_pins[8], input_pins[9], input_pins[10], input_pins[11], input_pins[12], input_pins[13],
            input_pins[14], input_pins[15], output_pins[0], output_pins[1], output_pins[2], output_pins[3],
            output_pins[4], output_pins[5], output_pins[6], output_pins[7], output_pins[8], output_pins[9],
            output_pins[10], output_pins[11], output_pins[12], output_pins[13], output_pins[14], output_pins[15], jtype,
            c);

    return prefs;
}

void cpart_Jumpers::ReadPreferences(std::string value) {
    unsigned char inp[16];
    unsigned char outp[16];

    char c;
    unsigned char ntype;

    sscanf(value.c_str(),
           "%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%"
           "hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%c",
           &inp[0], &inp[1], &inp[2], &inp[3], &inp[4], &inp[5], &inp[6], &inp[7], &inp[8], &inp[9], &inp[10], &inp[11],
           &inp[12], &inp[13], &inp[14], &inp[15], &outp[0], &outp[1], &outp[2], &outp[3], &outp[4], &outp[5], &outp[6],
           &outp[7], &outp[8], &outp[9], &outp[10], &outp[11], &outp[12], &outp[13], &outp[14], &outp[15], &ntype, &c);

    ChangeType(ntype);

    if ((ntype & 0x02)) {
        for (int i = 0; i < 16; i++) {
            input_pins[i] = inp[i];
        }
    } else {
        if ((input_pins[0] != inp[0]) || (ID != (c - 'A'))) {
            char buff[2];

            buff[0] = c;
            buff[1] = 0;

            jname = buff;

            ID = c - 'A';
            if (ID >= countID) {
                countID = ID + 1;
            }

            for (int i = 0; i < 16; i++) {
                SpareParts.UnregisterIOpin(input_pins[i]);
            }

            input_pins[0] = SpareParts.RegisterIOpin(jname + "I0", inp[0], PD_IN);
            input_pins[1] = SpareParts.RegisterIOpin(jname + "I1", inp[1], PD_IN);
            input_pins[2] = SpareParts.RegisterIOpin(jname + "I2", inp[2], PD_IN);
            input_pins[3] = SpareParts.RegisterIOpin(jname + "I3", inp[3], PD_IN);
            input_pins[4] = SpareParts.RegisterIOpin(jname + "I4", inp[4], PD_IN);
            input_pins[5] = SpareParts.RegisterIOpin(jname + "I5", inp[5], PD_IN);
            input_pins[6] = SpareParts.RegisterIOpin(jname + "I6", inp[6], PD_IN);
            input_pins[7] = SpareParts.RegisterIOpin(jname + "I7", inp[7], PD_IN);
            input_pins[8] = SpareParts.RegisterIOpin(jname + "I8", inp[8], PD_IN);
            input_pins[9] = SpareParts.RegisterIOpin(jname + "I9", inp[9], PD_IN);
            input_pins[10] = SpareParts.RegisterIOpin(jname + "IA", inp[10], PD_IN);
            input_pins[11] = SpareParts.RegisterIOpin(jname + "IB", inp[11], PD_IN);
            input_pins[12] = SpareParts.RegisterIOpin(jname + "IC", inp[12], PD_IN);
            input_pins[13] = SpareParts.RegisterIOpin(jname + "ID", inp[13], PD_IN);
            input_pins[14] = SpareParts.RegisterIOpin(jname + "IE", inp[14], PD_IN);
            input_pins[15] = SpareParts.RegisterIOpin(jname + "IF", inp[15], PD_IN);
        }
    }

    if (ntype & 0x01) {
        for (int i = 0; i < 16; i++) {
            output_pins[i] = outp[i];
        }
    } else {
        if ((output_pins[0] != outp[0]) || (ID != (c - 'A'))) {
            char buff[2];

            buff[0] = c;
            buff[1] = 0;

            jname = buff;

            ID = c - 'A';
            if (ID >= countID) {
                countID = ID + 1;
            }

            for (int i = 0; i < 16; i++) {
                SpareParts.UnregisterIOpin(output_pins[i]);
            }

            output_pins[0] = SpareParts.RegisterIOpin(jname + "O0", outp[0]);
            output_pins[1] = SpareParts.RegisterIOpin(jname + "O1", outp[1]);
            output_pins[2] = SpareParts.RegisterIOpin(jname + "O2", outp[2]);
            output_pins[3] = SpareParts.RegisterIOpin(jname + "O3", outp[3]);
            output_pins[4] = SpareParts.RegisterIOpin(jname + "O4", outp[4]);
            output_pins[5] = SpareParts.RegisterIOpin(jname + "O5", outp[5]);
            output_pins[6] = SpareParts.RegisterIOpin(jname + "O6", outp[6]);
            output_pins[7] = SpareParts.RegisterIOpin(jname + "O7", outp[7]);
            output_pins[8] = SpareParts.RegisterIOpin(jname + "O8", outp[8]);
            output_pins[9] = SpareParts.RegisterIOpin(jname + "O9", outp[9]);
            output_pins[10] = SpareParts.RegisterIOpin(jname + "OA", outp[10]);
            output_pins[11] = SpareParts.RegisterIOpin(jname + "OB", outp[11]);
            output_pins[12] = SpareParts.RegisterIOpin(jname + "OC", outp[12]);
            output_pins[13] = SpareParts.RegisterIOpin(jname + "OD", outp[13]);
            output_pins[14] = SpareParts.RegisterIOpin(jname + "OE", outp[14]);
            output_pins[15] = SpareParts.RegisterIOpin(jname + "OF", outp[15]);
        }
    }

    Reset();
}

void cpart_Jumpers::RegisterRemoteControl(void) {
    const picpin* ppins = SpareParts.GetPinsValues();

    for (int i = 0; i < 16; i++) {
        if (input_pins[i]) {
            output_ids[O_L1 + 1]->status = (void*)&ppins[input_pins[i] - 1].oavalue;
        }
    }
}

void cpart_Jumpers::ConfigurePropertiesWindow(void) {
    char childname[256];

    SpareParts.WPropCmd("combo1", WPA_COMBOSETITEMS, "F,M,");
    SpareParts.WPropCmd("combo18", WPA_COMBOSETITEMS, "F,M,");

    if (jtype & 0x02)
        SpareParts.WPropCmd("combo1", WPA_COMBOSETTEXT, "F");
    else
        SpareParts.WPropCmd("combo1", WPA_COMBOSETTEXT, "M");

    if (jtype & 0x01)
        SpareParts.WPropCmd("combo18", WPA_COMBOSETTEXT, "F");
    else
        SpareParts.WPropCmd("combo18", WPA_COMBOSETTEXT, "M");

    for (int i = 0; i < 16; i++) {
        // input
        snprintf(childname, 256, "combo%i", i + 2);

        SetPCWComboWithPinNames(childname, input_pins[i]);

        if (jtype & 0x02) {
            SpareParts.WPropCmd(childname, WPA_SETENABLE, "1");
        } else {
            SpareParts.WPropCmd(childname, WPA_SETENABLE, "0");
        }

        // output
        snprintf(childname, 256, "combo%i", i + 19);

        SetPCWComboWithPinNames(childname, output_pins[i]);

        if (jtype & 0x01) {
            SpareParts.WPropCmd(childname, WPA_SETENABLE, "1");
        } else {
            SpareParts.WPropCmd(childname, WPA_SETENABLE, "0");
        }
    }
}

void cpart_Jumpers::ReadPropertiesWindow(void) {
    char childname[256];

    char buff[64];
    SpareParts.WPropCmd("combo1", WPA_COMBOGETTEXT, NULL, buff);

    if (strcmp(buff, "M"))
        jtype |= 0x02;
    else
        jtype &= ~0x02;

    SpareParts.WPropCmd("combo18", WPA_COMBOGETTEXT, NULL, buff);
    if (strcmp(buff, "M"))
        jtype |= 0x01;
    else
        jtype &= ~0x01;

    for (int i = 0; i < 16; i++) {
        // input
        if (jtype & 0x02) {
            snprintf(childname, 256, "combo%i", i + 2);
            input_pins[i] = GetPWCComboSelectedPin(childname);
        }
        // output
        if (jtype & 0x01) {
            snprintf(childname, 256, "combo%i", i + 19);
            output_pins[i] = GetPWCComboSelectedPin(childname);
        }
    }
}

void cpart_Jumpers::PreProcess(void) {
    memset(output_pins_alm, 0, 16 * sizeof(unsigned long));
    JUMPSTEPS_ = PICSimLab.GetJUMPSTEPS();
    mcount = JUMPSTEPS_;
}

void cpart_Jumpers::Process(void) {
    int i;

    const picpin* ppins = SpareParts.GetPinsValues();

    for (i = 0; i < 16; i++) {
        if (ppins[input_pins[i] - 1].value != ppins[output_pins[i] - 1].value) {
            if (jtype & 0x01) {
                SpareParts.SetPin(output_pins[i], ppins[input_pins[i] - 1].value);
            } else {
                SpareParts.WritePin(output_pins[i], ppins[input_pins[i] - 1].value);
            }
        }
        /*
        if (ppins[input_pins[i] - 1].avalue != ppins[output_pins[i] - 1].avalue)
        {
     SpareParts.WritePinA (output_pins[i], ppins[input_pins[i] - 1].avalue);
        }
         */
    }

    mcount++;
    if (mcount >= JUMPSTEPS_) {
        for (i = 0; i < 16; i++) {
            if (ppins[output_pins[i] - 1].value)
                output_pins_alm[i]++;
        }
        mcount = -1;
    }
}

void cpart_Jumpers::PostProcess(void) {
    long int NSTEPJ = PICSimLab.GetNSTEPJ();
    const picpin* ppins = SpareParts.GetPinsValues();

    for (int i = 0; i < 16; i++) {
        SpareParts.WritePinOA(output_pins[i],
                              (ppins[output_pins[i] - 1].oavalue + ((output_pins_alm[i] * 200.0) / NSTEPJ) + 55) / 2);

        if (output_pins[i] && (output_ids[O_L1 + i]->value != ppins[output_pins[i] - 1].oavalue)) {
            output_ids[O_L1 + i]->value = ppins[output_pins[i] - 1].oavalue;
            output_ids[O_L1 + i]->update = 1;
        }
    }
}

void cpart_Jumpers::ChangeType(unsigned char ntype) {
    // if same

    if (jtype == ntype)
        return;

    if (!(jtype & 0x02) && (ntype & 0x02)) {
        for (int i = 0; i < 16; i++) {
            SpareParts.UnregisterIOpin(input_pins[i]);
            input_pins[i] = 0;
        }
    }
    if ((jtype & 0x02) && !(ntype & 0x02)) {
        input_pins[0] = SpareParts.RegisterIOpin(jname + "I0", 0, PD_IN);
        input_pins[1] = SpareParts.RegisterIOpin(jname + "I1", 0, PD_IN);
        input_pins[2] = SpareParts.RegisterIOpin(jname + "I2", 0, PD_IN);
        input_pins[3] = SpareParts.RegisterIOpin(jname + "I3", 0, PD_IN);
        input_pins[4] = SpareParts.RegisterIOpin(jname + "I4", 0, PD_IN);
        input_pins[5] = SpareParts.RegisterIOpin(jname + "I5", 0, PD_IN);
        input_pins[6] = SpareParts.RegisterIOpin(jname + "I6", 0, PD_IN);
        input_pins[7] = SpareParts.RegisterIOpin(jname + "I7", 0, PD_IN);
        input_pins[8] = SpareParts.RegisterIOpin(jname + "I8", 0, PD_IN);
        input_pins[9] = SpareParts.RegisterIOpin(jname + "I9", 0, PD_IN);
        input_pins[10] = SpareParts.RegisterIOpin(jname + "IA", 0, PD_IN);
        input_pins[11] = SpareParts.RegisterIOpin(jname + "IB", 0, PD_IN);
        input_pins[12] = SpareParts.RegisterIOpin(jname + "IC", 0, PD_IN);
        input_pins[13] = SpareParts.RegisterIOpin(jname + "ID", 0, PD_IN);
        input_pins[14] = SpareParts.RegisterIOpin(jname + "IE", 0, PD_IN);
        input_pins[15] = SpareParts.RegisterIOpin(jname + "IF", 0, PD_IN);
    }

    if (!(jtype & 0x01) && (ntype & 0x01)) {
        for (int i = 0; i < 16; i++) {
            SpareParts.UnregisterIOpin(output_pins[i]);
            output_pins[i] = 0;
        }
    }

    if ((jtype & 0x01) && !(ntype & 0x01)) {
        output_pins[0] = SpareParts.RegisterIOpin(jname + "O0");
        output_pins[1] = SpareParts.RegisterIOpin(jname + "O1");
        output_pins[2] = SpareParts.RegisterIOpin(jname + "O2");
        output_pins[3] = SpareParts.RegisterIOpin(jname + "O3");
        output_pins[4] = SpareParts.RegisterIOpin(jname + "O4");
        output_pins[5] = SpareParts.RegisterIOpin(jname + "O5");
        output_pins[6] = SpareParts.RegisterIOpin(jname + "O6");
        output_pins[7] = SpareParts.RegisterIOpin(jname + "O7");
        output_pins[8] = SpareParts.RegisterIOpin(jname + "O8");
        output_pins[9] = SpareParts.RegisterIOpin(jname + "O9");
        output_pins[10] = SpareParts.RegisterIOpin(jname + "OA");
        output_pins[11] = SpareParts.RegisterIOpin(jname + "OB");
        output_pins[12] = SpareParts.RegisterIOpin(jname + "OC");
        output_pins[13] = SpareParts.RegisterIOpin(jname + "OD");
        output_pins[14] = SpareParts.RegisterIOpin(jname + "OE");
        output_pins[15] = SpareParts.RegisterIOpin(jname + "OF");
    }

    jtype = ntype;
}

void cpart_Jumpers::ComboChange(CPWindow* WProp, CCombo* control, std::string value) {
    unsigned char ntype = jtype;

    if (!control->GetName().compare("combo1")) {
        if (!value.compare("F")) {
            ntype |= 0x02;
        } else {
            ntype &= ~0x02;
        }

    } else {
        if (!value.compare("F")) {
            ntype |= 0x01;
        } else {
            ntype &= ~0x01;
        }
    }
    ChangeType(ntype);
}

part_init(PART_JUMPERS_Name, cpart_Jumpers, "Other");
