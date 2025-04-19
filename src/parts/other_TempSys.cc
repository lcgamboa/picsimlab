/* ########################################################################

   PICSimLab - Programmable IC Simulator Laboratory

   ########################################################################

   Copyright (c) : 2010-2025  Luis Claudio Gambôa Lopes <lcgamboa@yahoo.com>

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

static PCWProp pcwprop[9] = {{PCW_COMBO, "Heater"},   {PCW_COMBO, "Cooler"},   {PCW_COMBO, "Temp."},
                             {PCW_COMBO, "Tach"},     {PCW_LABEL, "VCC,+12V"}, {PCW_LABEL, "GND,GND"},
                             {PCW_COMBO, "T. Voff."}, {PCW_SPIND, "Amb. T."},  {PCW_END, ""}};

static void cpart_tempsys_callback(void* arg) {
    cpart_tempsys* tempsys = (cpart_tempsys*)arg;
    tempsys->OnTime();
}

cpart_tempsys::cpart_tempsys(const unsigned x, const unsigned y, const char* name, const char* type, board* pboard_,
                             const int id_)
    : part(x, y, name, type, pboard_, id_) {
    always_update = 1;

    vtc = 0;
    vt = 0;

    vent[0] = SpareParts.CanvasCmd(
        {.cmd = CC_LOADIMAGE,
         .LoadImage{(PICSimLab.GetSharePath() + "boards/Common/VT1.svg").c_str(), Scale * 0.867f, 0, Orientation}});
    vent[1] = SpareParts.CanvasCmd(
        {.cmd = CC_LOADIMAGE,
         .LoadImage{(PICSimLab.GetSharePath() + "boards/Common/VT2.svg").c_str(), Scale * 0.867f, 0, Orientation}});

    input_pins[0] = 0;
    input_pins[1] = 0;
    input_pins[2] = 0;
    input_pins[3] = 0;

    ambient = 27.5;
    tvoff = 0;

    temp[0] = ambient;
    temp[1] = ambient;
    ref = 0;
    rpmstp = 0;  //(NSTEP*2)/100;
    rpmc = 0;

    refresh = 0;

    SetPCWProperties(pcwprop);

    PinCount = 4;
    Pins = input_pins;

    TimerID = pboard->TimerRegister_ms(100, cpart_tempsys_callback, this);
}

cpart_tempsys::~cpart_tempsys(void) {
    SpareParts.SetPartOnDraw(id);
    SpareParts.CanvasCmd({.cmd = CC_FREEBITMAP, .FreeBitmap{BitmapId}});
    SpareParts.CanvasCmd({.cmd = CC_FREEBITMAP, .FreeBitmap{vent[0]}});
    SpareParts.CanvasCmd({.cmd = CC_FREEBITMAP, .FreeBitmap{vent[1]}});
    vent[0] = -1;
    vent[1] = -1;

    SpareParts.CanvasCmd({.cmd = CC_DESTROY});

    pboard->TimerUnregister(TimerID);
}

void cpart_tempsys::DrawOutput(const unsigned int i) {
    char str[100];

    switch (output[i].id) {
        case O_HT:
        case O_CO:
        case O_TA:
        case O_TE:
            SpareParts.CanvasCmd({.cmd = CC_SETCOLOR, .SetColor{49, 61, 99}});
            SpareParts.CanvasCmd(
                {.cmd = CC_RECTANGLE,
                 .Rectangle{1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1}});
            SpareParts.CanvasCmd({.cmd = CC_SETFGCOLOR, .SetFgColor{255, 255, 255}});
            if (input_pins[output[i].id - O_HT] == 0)
                SpareParts.CanvasCmd({.cmd = CC_ROTATEDTEXT, .RotatedText{"NC", output[i].x1, output[i].y1, 0}});
            else
                SpareParts.CanvasCmd({.cmd = CC_ROTATEDTEXT,
                                      .RotatedText{SpareParts.GetPinName(input_pins[output[i].id - O_HT]).c_str(),
                                                   output[i].x1, output[i].y1, 0}});
            break;
        case O_F1:
            SpareParts.CanvasCmd({.cmd = CC_SETCOLOR, .SetColor{49, 61, 99}});
            SpareParts.CanvasCmd(
                {.cmd = CC_RECTANGLE,
                 .Rectangle{1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1}});
            SpareParts.CanvasCmd({.cmd = CC_SETFGCOLOR, .SetFgColor{155, 155, 155}});
            SpareParts.CanvasCmd({.cmd = CC_ROTATEDTEXT, .RotatedText{"12V", output[i].x1, output[i].y1, 0}});
            break;
        case O_F2:
            SpareParts.CanvasCmd({.cmd = CC_SETCOLOR, .SetColor{49, 61, 99}});
            SpareParts.CanvasCmd(
                {.cmd = CC_RECTANGLE,
                 .Rectangle{1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1}});
            SpareParts.CanvasCmd({.cmd = CC_SETFGCOLOR, .SetFgColor{155, 155, 155}});
            SpareParts.CanvasCmd({.cmd = CC_ROTATEDTEXT, .RotatedText{"GND", output[i].x1, output[i].y1, 0}});
            break;
        case O_OTA:
            SpareParts.CanvasCmd({.cmd = CC_SETCOLOR, .SetColor{49, 61, 99}});
            SpareParts.CanvasCmd(
                {.cmd = CC_RECTANGLE,
                 .Rectangle{1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1}});
            SpareParts.CanvasCmd({.cmd = CC_SETFGCOLOR, .SetFgColor{255, 255, 255}});
            snprintf(str, 100, "Ambient.=%5.2fC", ambient);
            SpareParts.CanvasCmd({.cmd = CC_ROTATEDTEXT, .RotatedText{str, output[i].x1, output[i].y1, 0}});
            break;
        case O_OTE:
            snprintf(str, 100, "Temp.=%5.2fC", temp[0]);
            SpareParts.CanvasCmd({.cmd = CC_SETCOLOR, .SetColor{49, 61, 99}});
            SpareParts.CanvasCmd(
                {.cmd = CC_RECTANGLE,
                 .Rectangle{1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1}});
            SpareParts.CanvasCmd({.cmd = CC_SETFGCOLOR, .SetFgColor{255, 255, 255}});
            SpareParts.CanvasCmd({.cmd = CC_ROTATEDTEXT, .RotatedText{str, output[i].x1, output[i].y1, 0}});
            break;
        case O_VT:
            SpareParts.CanvasCmd({.cmd = CC_PUTBITMAP, .PutBitmap{vent[vt], output[i].x1, output[i].y1}});
            break;
    }
}

void cpart_tempsys::Process(void) {
    if (!refresh) {
        refresh = PICSimLab.GetJUMPSTEPS() + 1;

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

void cpart_tempsys::OnTime(void) {
    const picpin* ppins = SpareParts.GetPinsValues();

    // sensor ventilador
    if (input_pins[1] > 0) {
        rpmstp = 2000.0 / (0.7196 * (ppins[input_pins[1] - 1].oavalue - 54));

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
    temp[0] = ((ambient + ref) * 0.003) + temp[1] * (0.997);

    if (temp[0] < ambient)
        temp[0] = ambient;

    float vsensor = temp[0] / 100.0;

    if (vsensor > 1.5) {
        vsensor = 1.5;
    }

    if (tvoff == 0) {
        if (vsensor < 0) {
            vsensor = 0;
        }
    } else {
        if (vsensor < -0.55) {
            vsensor = -0.55;
        }
    }
    SpareParts.SetAPin(input_pins[2], vsensor + tvoff);

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
    printf("Error input '%s' don't have a valid id! \n", name);
    return INVALID_ID;
}

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

    printf("Error output '%s' don't have a valid id! \n", name);
    return INVALID_ID;
}

std::string cpart_tempsys::WritePreferences(void) {
    char prefs[256];

    sprintf(prefs, "%hhu,%hhu,%hhu,%hhu,%f,%f", input_pins[0], input_pins[1], input_pins[2], input_pins[3], ambient,
            tvoff);

    return prefs;
}

void cpart_tempsys::ReadPreferences(std::string value) {
    sscanf(value.c_str(), "%hhu,%hhu,%hhu,%hhu,%f,%f", &input_pins[0], &input_pins[1], &input_pins[2], &input_pins[3],
           &ambient, &tvoff);
    temp[0] = ambient;
    temp[1] = ambient;
}

void cpart_tempsys::ConfigurePropertiesWindow(void) {
    SetPCWComboWithPinNames("combo1", input_pins[0]);
    SetPCWComboWithPinNames("combo2", input_pins[1]);
    SetPCWComboWithPinNames("combo3", input_pins[2]);
    SetPCWComboWithPinNames("combo4", input_pins[3]);

    SpareParts.WPropCmd("combo7", PWA_COMBOSETITEMS, "0V,1.2V,");

    if (tvoff == 0) {
        SpareParts.WPropCmd("combo7", PWA_COMBOSETTEXT, "0V");
    } else {
        SpareParts.WPropCmd("combo7", PWA_COMBOSETTEXT, "1.2V");
    }

    SpareParts.WPropCmd("spind8", PWA_SPINDSETMAX, "150");
    SpareParts.WPropCmd("spind8", PWA_SPINDSETMIN, "-55");
    SpareParts.WPropCmd("spind8", PWA_SPINDSETVALUE, std::to_string(ambient).c_str());
}

void cpart_tempsys::ReadPropertiesWindow(void) {
    input_pins[0] = GetPWCComboSelectedPin("combo1");
    input_pins[1] = GetPWCComboSelectedPin("combo2");
    input_pins[2] = GetPWCComboSelectedPin("combo3");
    input_pins[3] = GetPWCComboSelectedPin("combo4");

    char buff[64];
    SpareParts.WPropCmd("combo7", PWA_COMBOGETTEXT, NULL, buff);
    if (strcmp(buff, "0V") == 0) {
        tvoff = 0;
    } else {
        tvoff = 1.2;
    }

    SpareParts.WPropCmd("spind8", PWA_SPINDGETVALUE, NULL, &ambient);
    temp[0] = ambient;
    temp[1] = ambient;
}

part_init(PART_TEMPSYS_Name, cpart_tempsys, "Other");
