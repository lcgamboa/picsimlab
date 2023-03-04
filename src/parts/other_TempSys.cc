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

#include "other_TempSys.h"
#include "../lib/oscilloscope.h"
#include "../lib/picsimlab.h"
#include "../lib/spareparts.h"

/* outputs */
enum { O_HT, O_CO, O_TE, O_TA, O_F1, O_F2, O_OTA, O_OTE, O_VT };

/* inputs */
enum { I_PO1, I_PO2, I_PO3, I_PO4 };

static PCWProp pcwprop[6] = {{PCW_COMBO, "Heater"}, {PCW_COMBO, "Cooler"},   {PCW_COMBO, "Temp."},
                             {PCW_COMBO, "Tach"},   {PCW_LABEL, "VCC,+12V"}, {PCW_LABEL, "GND,GND"}};

cpart_tempsys::cpart_tempsys(const unsigned x, const unsigned y, const char* name, const char* type)
    : part(x, y, name, type), font(9, lxFONTFAMILY_TELETYPE, lxFONTSTYLE_NORMAL, lxFONTWEIGHT_BOLD) {
    always_update = 1;

    vtc = 0;
    vt = 0;

    lxImage image(SpareParts.GetWindow());
    image.LoadFile(lxGetLocalFile(PICSimLab.GetSharePath() + lxT("boards/Common/VT1.svg")), Orientation, Scale * 0.867,
                   Scale * 0.867);
    vent[0] = new lxBitmap(&image, SpareParts.GetWindow());
    image.Destroy();

    image.LoadFile(lxGetLocalFile(PICSimLab.GetSharePath() + lxT("boards/Common/VT2.svg")), Orientation, Scale * 0.867,
                   Scale * 0.867);
    vent[1] = new lxBitmap(&image, SpareParts.GetWindow());
    image.Destroy();

    input_pins[0] = 0;
    input_pins[1] = 0;
    input_pins[2] = 0;
    input_pins[3] = 0;

    temp[0] = 27.5;
    temp[1] = 27.5;
    ref = 0;
    rpmstp = 0;  //(NSTEP*2)/100;
    rpmc = 0;

    refresh = 0;

    SetPCWProperties(pcwprop, 6);

    PinCount = 4;
    Pins = input_pins;
}

cpart_tempsys::~cpart_tempsys(void) {
    delete Bitmap;

    delete vent[0];
    delete vent[1];
    vent[0] = NULL;
    vent[1] = NULL;

    canvas.Destroy();
}

void cpart_tempsys::DrawOutput(const unsigned int i) {
    lxString str;

    switch (output[i].id) {
        case O_HT:
        case O_CO:
        case O_TA:
        case O_TE:
            canvas.SetColor(49, 61, 99);
            canvas.Rectangle(1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);
            canvas.SetFgColor(255, 255, 255);
            if (input_pins[output[i].id - O_HT] == 0)
                canvas.RotatedText("NC", output[i].x1, output[i].y1, 0);
            else
                canvas.RotatedText(SpareParts.GetPinName(input_pins[output[i].id - O_HT]), output[i].x1, output[i].y1,
                                   0);
            break;
        case O_F1:
            canvas.SetColor(49, 61, 99);
            canvas.Rectangle(1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);
            canvas.SetFgColor(155, 155, 155);
            canvas.RotatedText("12V", output[i].x1, output[i].y1, 0);
            break;
        case O_F2:
            canvas.SetColor(49, 61, 99);
            canvas.Rectangle(1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);
            canvas.SetFgColor(155, 155, 155);
            canvas.RotatedText("GND", output[i].x1, output[i].y1, 0);
            break;
        case O_OTA:
            canvas.SetColor(49, 61, 99);
            canvas.Rectangle(1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);
            canvas.SetFgColor(255, 255, 255);
            canvas.RotatedText("Ambient=27.5C", output[i].x1, output[i].y1, 0);
            break;
        case O_OTE:
            str.Printf(lxT("Temp.=%5.2fC"), temp[0]);
            canvas.SetColor(49, 61, 99);
            canvas.Rectangle(1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);
            canvas.SetFgColor(255, 255, 255);
            canvas.RotatedText(str, output[i].x1, output[i].y1, 0);
            break;
        case O_VT:
            canvas.PutBitmap(vent[vt], output[i].x1, output[i].y1);
            break;
    }
}

void cpart_tempsys::Process(void) {
    if (!refresh) {
        refresh = PICSimLab.GetJUMPSTEPS();

        const picpin* ppins = SpareParts.GetPinsValues();

        if ((input_pins[1] > 0) && (input_pins[3] > 0)) {
            if (ppins[input_pins[1] - 1].oavalue > 55) {
                rpmc++;
                if (rpmc > rpmstp) {
                    rpmc = 0;
                    SpareParts.SetPin(input_pins[3], !ppins[input_pins[3] - 1].value);
                }
            } else
                SpareParts.SetPin(input_pins[3], 0);
        }
    }
    refresh--;
}

void cpart_tempsys::PostProcess(void) {
    const picpin* ppins = SpareParts.GetPinsValues();

    // sensor ventilador
    if (input_pins[1] > 0) {
        rpmstp = ((float)PICSimLab.GetNSTEPJ()) / (0.7196 * (ppins[input_pins[1] - 1].oavalue - 54));

        if (ppins[input_pins[1] - 1].oavalue > 55)
            vtc++;

        if (vtc > (4 - 0.04 * ppins[input_pins[1] - 1].oavalue)) {
            vtc = 0;
            vt ^= 1;
        }
    }

    // temperatura
    if (input_pins[0] > 0)
        ref = (0.25 * (ppins[input_pins[0] - 1].oavalue - 55));
    if (input_pins[1] > 0)
        ref -= (0.25 * (ppins[input_pins[1] - 1].oavalue - 55));

    temp[1] = temp[0];
    temp[0] = ((27.5 + ref) * 0.003) + temp[1] * (0.997);

    if (temp[0] < 27.5)
        temp[0] = 27.5;

    SpareParts.SetAPin(input_pins[2], temp[0] / 100.0);

    if (output_ids[O_OTE]->value_f != temp[0]) {
        output_ids[O_OTE]->value_f = temp[0];
        output_ids[O_OTE]->update = 1;
    }

    if (output_ids[O_VT]->value != vt) {
        output_ids[O_VT]->value = vt;
        output_ids[O_VT]->update = 1;
    }
}

unsigned short cpart_tempsys::GetInputId(char* name) {
    printf("Erro input '%s' don't have a valid id! \n", name);
    return -1;
};

unsigned short cpart_tempsys::GetOutputId(char* name) {
    if (strcmp(name, "MC_VT") == 0)
        return O_VT;
    if (strcmp(name, "PN_HT") == 0)
        return O_HT;
    if (strcmp(name, "PN_CO") == 0)
        return O_CO;
    if (strcmp(name, "PN_TE") == 0)
        return O_TE;
    if (strcmp(name, "PN_TA") == 0)
        return O_TA;
    if (strcmp(name, "PN_F1") == 0)
        return O_F1;
    if (strcmp(name, "PN_F2") == 0)
        return O_F2;
    if (strcmp(name, "DI_OTA") == 0)
        return O_OTA;
    if (strcmp(name, "DI_OTE") == 0)
        return O_OTE;

    printf("Erro output '%s' don't have a valid id! \n", name);
    return 1;
};

lxString cpart_tempsys::WritePreferences(void) {
    char prefs[256];

    sprintf(prefs, "%hhu,%hhu,%hhu,%hhu", input_pins[0], input_pins[1], input_pins[2], input_pins[3]);

    return prefs;
}

void cpart_tempsys::ReadPreferences(lxString value) {
    sscanf(value.c_str(), "%hhu,%hhu,%hhu,%hhu", &input_pins[0], &input_pins[1], &input_pins[2], &input_pins[3]);
}

void cpart_tempsys::ConfigurePropertiesWindow(CPWindow* WProp) {
    SetPCWComboWithPinNames(WProp, "combo1", input_pins[0]);
    SetPCWComboWithPinNames(WProp, "combo2", input_pins[1]);
    SetPCWComboWithPinNames(WProp, "combo3", input_pins[2]);
    SetPCWComboWithPinNames(WProp, "combo4", input_pins[3]);
}

void cpart_tempsys::ReadPropertiesWindow(CPWindow* WProp) {
    input_pins[0] = GetPWCComboSelectedPin(WProp, "combo1");
    input_pins[1] = GetPWCComboSelectedPin(WProp, "combo2");
    input_pins[2] = GetPWCComboSelectedPin(WProp, "combo3");
    input_pins[3] = GetPWCComboSelectedPin(WProp, "combo4");
}

part_init(PART_TEMPSYS_Name, cpart_tempsys, "Other");
