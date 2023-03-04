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

#include "virtual_d_transfer_function.h"
#include "../lib/oscilloscope.h"
#include "../lib/picsimlab.h"
#include "../lib/spareparts.h"

/* outputs */
enum { O_P1, O_P2, O_IG, O_IO, O_OG, O_OO, O_NUM, O_DEN, O_TS };

static PCWProp pcwprop[9] = {{PCW_COMBO, "Input"},  {PCW_COMBO, "Output"},  {PCW_EDIT, "Num."},
                             {PCW_EDIT, "Den."},    {PCW_EDIT, "Sample"},   {PCW_EDIT, "In Gain"},
                             {PCW_EDIT, "In Off."}, {PCW_EDIT, "Out Gain"}, {PCW_EDIT, "Out Off."}};

cpart_dtfunc::cpart_dtfunc(const unsigned x, const unsigned y, const char* name, const char* type)
    : part(x, y, name, type), font(7, lxFONTFAMILY_TELETYPE, lxFONTSTYLE_NORMAL, lxFONTWEIGHT_BOLD) {
    always_update = 1;
    pins[0] = 0;
    pins[1] = 0;

    num[0] = 0.1445;
    den[0] = 1.0;
    den[1] = -0.997;
    ordern = 1;
    orderd = 2;
    sample = 0.1;
    in_gain = 0.2;
    in_off = 0;
    out_gain = 0.01;
    out_off = 0.27;

    nsamples = sample * PICSimLab.GetBoard()->MGetInstClockFreq();

    refresh = 0;

    SetPCWProperties(pcwprop, 9);

    PinCount = 2;
    Pins = pins;
}

cpart_dtfunc::~cpart_dtfunc(void) {
    delete Bitmap;
    canvas.Destroy();
}

void cpart_dtfunc::DrawOutput(const unsigned int i) {
    char buff[20];
    char eq[100];

    switch (output[i].id) {
        case O_P1:
        case O_P2:
            canvas.SetColor(249, 249, 249);
            canvas.Rectangle(1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);
            canvas.SetFgColor(0, 0, 0);
            if (output[i].id == O_P1) {
                if (pins[0] == 0)
                    canvas.RotatedText("NC", output[i].x1, output[i].y1, 0);
                else
                    canvas.RotatedText(SpareParts.GetPinName(pins[0]), output[i].x1, output[i].y1, 0);
            }
            if (output[i].id == O_P2) {
                if (pins[1] == 0)
                    canvas.RotatedText("NC", output[i].x1, output[i].y1, 0);
                else
                    canvas.RotatedText(SpareParts.GetPinName(pins[1]), output[i].x1, output[i].y1, 0);
            }
            break;
        case O_IG:
        case O_IO:
        case O_OG:
        case O_OO:
        case O_TS:
            canvas.SetColor(220, 220, 220);
            canvas.Rectangle(1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);
            canvas.SetFgColor(0, 0, 0);
            if (output[i].id == O_IG)
                snprintf(buff, 19, "%5.2f", in_gain);
            if (output[i].id == O_IO)
                snprintf(buff, 19, "%5.2f", in_off);
            if (output[i].id == O_OG)
                snprintf(buff, 19, "%5.2f", out_gain);
            if (output[i].id == O_OO)
                snprintf(buff, 19, "%5.2f", out_off);
            if (output[i].id == O_TS)
                snprintf(buff, 19, "%5.2f", sample);
            canvas.RotatedText(buff, output[i].x1, output[i].y1, 0);
            break;
        case O_NUM:
            canvas.SetColor(220, 220, 220);
            canvas.Rectangle(1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);

            strncpy(eq, "[", 99);
            for (int i = 0; i < ordern; i++) {
                snprintf(buff, 19, "%+6.3f ", num[i]);
                strncat(eq, buff, 99);
            }
            strncat(eq, "]", 99);
            canvas.SetFgColor(0, 0, 0);
            canvas.RotatedText(eq, output[i].x1, output[i].y1, 0);
            break;
        case O_DEN:
            canvas.SetColor(220, 220, 220);
            canvas.Rectangle(1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);

            strncpy(eq, "[", 99);
            for (int i = 0; i < orderd; i++) {
                snprintf(buff, 19, "%+6.3f ", den[i]);
                strncat(eq, buff, 99);
            }
            strncat(eq, "]", 99);
            canvas.SetFgColor(0, 0, 0);
            canvas.RotatedText(eq, output[i].x1, output[i].y1, 0);
            break;
    }
}

void cpart_dtfunc::PreProcess(void) {
    nsamples = sample * PICSimLab.GetBoard()->MGetInstClockFreq();
}

void cpart_dtfunc::Process(void) {
    if (refresh > nsamples) {
        refresh = 0;

        const picpin* ppins = SpareParts.GetPinsValues();

        float in, out, pinv;

        if (pins[0] == 0)
            return;
        pinv = (ppins[pins[0] - 1].oavalue - 30) * 0.022502250225;
        // pinv = (ppins[pins[0] - 1].value) *5.0;

        in = pinv * in_gain + in_off;

        v[3] = v[2];
        v[2] = v[1];
        v[1] = v[0];
        v[0] = in - den[1] * v[1] - den[2] * v[2] - den[3] * v[3];
        out = v[0] * num[0] + v[1] * num[1] + v[2] * num[2] + v[3] * num[3];

        out = out * out_gain + out_off;

        if (out < 0.0)
            out = 0.0;
        if (out > 5.0)
            out = 5.0;

        SpareParts.SetAPin(pins[1], out);
    }
    refresh++;
}

void cpart_dtfunc::Reset(void) {
    for (int i = 0; i < 4; i++)
        v[i] = 0;
}

void
cpart_dtfunc::EvKeyPress(uint key, uint mask) {
 /*
 if(key == '1')
 {
   p_BT1=0; 
 }
 if(key == '2')
 {
   p_BT2=0; 
 }
 if(key == '3')
 {
   p_BT3=0; 
 }
 if(key == '4')
 {
   p_BT4=0; 
 }
  */ };

void
cpart_dtfunc::EvKeyRelease(uint key, uint mask) {
 /*
 if(key == '1')
 {
   p_BT1=1; 
 }
  
 if(key == '2')
 {
   p_BT2=1; 
 }
  
 if(key == '3')
 {
   p_BT3=1; 
 }
  
 if(key == '4')
 {
   p_BT4=1; 
 }
  */ };

unsigned short cpart_dtfunc::GetInputId(char* name) {
    printf("Erro input '%s' don't have a valid id! \n", name);
    return -1;
};

unsigned short cpart_dtfunc::GetOutputId(char* name) {
    if (strcmp(name, "PN_1") == 0)
        return O_P1;
    if (strcmp(name, "PN_2") == 0)
        return O_P2;
    if (strcmp(name, "DI_IG") == 0)
        return O_IG;
    if (strcmp(name, "DI_IO") == 0)
        return O_IO;
    if (strcmp(name, "DI_OG") == 0)
        return O_OG;
    if (strcmp(name, "DI_OO") == 0)
        return O_OO;
    if (strcmp(name, "DI_TS") == 0)
        return O_TS;
    if (strcmp(name, "DI_NUM") == 0)
        return O_NUM;
    if (strcmp(name, "DI_DEN") == 0)
        return O_DEN;

    printf("Erro output '%s' don't have a valid id! \n", name);
    return 1;
};

lxString cpart_dtfunc::WritePreferences(void) {
    char prefs[256];

    snprintf(prefs, 255, "%hhu,%hhu,%f,%f,%f,%f,%f,%i,%i,%f,%f,%f,%f,%f,%f,%f,%f", pins[0], pins[1], sample, in_gain,
             in_off, out_gain, out_off, ordern, orderd, num[0], num[1], num[2], num[3], den[0], den[1], den[2], den[3]);

    return prefs;
}

void cpart_dtfunc::ReadPreferences(lxString value) {
    sscanf(value.c_str(), "%hhu,%hhu,%f,%f,%f,%f,%f,%i,%i,%f,%f,%f,%f,%f,%f,%f,%f", &pins[0], &pins[1], &sample,
           &in_gain, &in_off, &out_gain, &out_off, &ordern, &orderd, &num[0], &num[1], &num[2], &num[3], &den[0],
           &den[1], &den[2], &den[3]);
}

void cpart_dtfunc::ConfigurePropertiesWindow(CPWindow* WProp) {
    lxString Items = SpareParts.GetPinsNames();
    lxString spin;
    char buff[20];
    char eq[200];

    SetPCWComboWithPinNames(WProp, "combo1", pins[0]);
    SetPCWComboWithPinNames(WProp, "combo2", pins[1]);

    eq[0] = 0;
    for (int i = 0; i < ordern; i++) {
        snprintf(buff, 19, "%+f ", num[i]);
        strncat(eq, buff, 199);
    }
    ((CEdit*)WProp->GetChildByName("edit3"))->SetText(eq);

    eq[0] = 0;
    for (int i = 0; i < orderd; i++) {
        snprintf(buff, 19, "%+f ", den[i]);
        strncat(eq, buff, 199);
    }
    ((CEdit*)WProp->GetChildByName("edit4"))->SetText(eq);

    ((CEdit*)WProp->GetChildByName("edit5"))->SetText(ftoa(sample));
    ((CEdit*)WProp->GetChildByName("edit6"))->SetText(ftoa(in_gain));
    ((CEdit*)WProp->GetChildByName("edit7"))->SetText(ftoa(in_off));
    ((CEdit*)WProp->GetChildByName("edit8"))->SetText(ftoa(out_gain));
    ((CEdit*)WProp->GetChildByName("edit9"))->SetText(ftoa(out_off));
}

void cpart_dtfunc::ReadPropertiesWindow(CPWindow* WProp) {
    char line[256];
    char* T[4];

    pins[0] = GetPWCComboSelectedPin(WProp, "combo1");
    pins[1] = GetPWCComboSelectedPin(WProp, "combo2");

    strncpy(line, ((CEdit*)WProp->GetChildByName("edit3"))->GetText().c_str(), 255);
    T[0] = strtok(line, " ");
    T[1] = strtok(NULL, " ");
    T[2] = strtok(NULL, " ");
    T[3] = strtok(NULL, " ");

    ordern = 0;
    for (int i = 0; i < 4; i++) {
        if (T[i]) {
            sscanf(T[i], "%f", &num[i]);
            ordern++;
        } else {
            num[i] = 0;
        }
    }

    strncpy(line, ((CEdit*)WProp->GetChildByName("edit4"))->GetText().c_str(), 255);
    T[0] = strtok(line, " ");
    T[1] = strtok(NULL, " ");
    T[2] = strtok(NULL, " ");
    T[3] = strtok(NULL, " ");

    orderd = 0;
    for (int i = 0; i < 4; i++) {
        if (T[i]) {
            sscanf(T[i], "%f", &den[i]);
            orderd++;
        } else {
            den[i] = 0;
        }
    }

    if (den[0] != 1.0) {
        for (int i = 0; i < ordern; i++) {
            num[i] /= den[0];
        }

        for (int i = orderd - 1; i >= 0; i--) {
            den[i] /= den[0];
        }
    }

    sample = atof(((CEdit*)WProp->GetChildByName("edit5"))->GetText());
    in_gain = atof(((CEdit*)WProp->GetChildByName("edit6"))->GetText());
    in_off = atof(((CEdit*)WProp->GetChildByName("edit7"))->GetText());
    out_gain = atof(((CEdit*)WProp->GetChildByName("edit8"))->GetText());
    out_off = atof(((CEdit*)WProp->GetChildByName("edit9"))->GetText());

    Reset();
}

part_init(PART_DTRANSFERF_Name, cpart_dtfunc, "Virtual");
