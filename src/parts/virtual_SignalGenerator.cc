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

#define _USE_MATH_DEFINES

#include "virtual_SignalGenerator.h"
#include "../lib/oscilloscope.h"
#include "../lib/picsimlab.h"
#include "../lib/spareparts.h"
#include "math.h"

/* outputs */
enum { O_P1, O_P2, O_P3, O_PO1, O_PO2, O_PO3, O_TP, O_AMPL, O_OFFS, O_FREQ, O_MF };

/* inputs */
enum { I_PO1, I_PO2, I_PO3, I_TP, I_MF };

static PCWProp pcwprop[4] = {{PCW_LABEL, "P4 - GND,GND"},
                             {PCW_COMBO, "P2 - Out"},
                             {PCW_COMBO, "P3 - Out"},
                             {PCW_END, ""}};

cpart_SignalGenerator::cpart_SignalGenerator(const unsigned x, const unsigned y, const char* name, const char* type,
                                             board* pboard_, const int id_)
    : part(x, y, name, type, pboard_, id_) {
    always_update = 1;

    input_pins[0] = 0;
    input_pins[1] = 0;

    values[0] = 0;
    values[1] = 0;
    values[2] = 0;

    active[0] = 0;
    active[1] = 0;
    active[2] = 0;

    type = 0;
    ts = 0;
    maxfreq = 1;
    lastd = 2;

    SetPCWProperties(pcwprop);

    PinCount = 2;
    Pins = input_pins;
}

cpart_SignalGenerator::~cpart_SignalGenerator(void) {
    SpareParts.SetPartOnDraw(id);
    SpareParts.CanvasCmd({CC_FREEBITMAP, .FreeBitmap{BitmapId}});
    SpareParts.CanvasCmd({CC_DESTROY});
}

void cpart_SignalGenerator::DrawOutput(const unsigned int i) {
    int j;
    char temp[100];
    float v[2];
    float tsi;
    int sizex;
    int sizey;

    switch (output[i].id) {
        case O_P2:
        case O_P3:
            SpareParts.CanvasCmd({CC_SETCOLOR, .SetColor{49, 61, 99}});
            SpareParts.CanvasCmd({CC_RECTANGLE, .Rectangle{1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1,
                                                           output[i].y2 - output[i].y1}});
            SpareParts.CanvasCmd({CC_SETFGCOLOR, .SetFgColor{255, 255, 255}});
            if (input_pins[output[i].id - O_P2] == 0)
                SpareParts.CanvasCmd({CC_ROTATEDTEXT, .RotatedText{"NC", output[i].x1, output[i].y1, 0}});
            else
                SpareParts.CanvasCmd(
                    {CC_ROTATEDTEXT, .RotatedText{SpareParts.GetPinName(input_pins[output[i].id - O_P2]).c_str(),
                                                  output[i].x1, output[i].y1, 0}});
            break;
        case O_P1:
            SpareParts.CanvasCmd({CC_SETCOLOR, .SetColor{49, 61, 99}});
            SpareParts.CanvasCmd({CC_RECTANGLE, .Rectangle{1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1,
                                                           output[i].y2 - output[i].y1}});
            SpareParts.CanvasCmd({CC_SETFGCOLOR, .SetFgColor{255, 255, 255}});
            SpareParts.CanvasCmd({CC_ROTATEDTEXT, .RotatedText{"GND", output[i].x1, output[i].y1, 0}});
            break;
        case O_PO1:
        case O_PO2:
        case O_PO3:
            DrawPotentiometer(SpareParts.CanvasCmd, &output[i], 200 - values[output[i].id - O_PO1], "", 9);
            break;
        case O_TP:
            SpareParts.CanvasCmd({CC_SETCOLOR, .SetColor{255, 255, 255}});
            SpareParts.CanvasCmd({CC_RECTANGLE, .Rectangle{1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1,
                                                           output[i].y2 - output[i].y1}});
            SpareParts.CanvasCmd({CC_SETFGCOLOR, .SetFgColor{0, 0, 0}});

            v[0] = 0;
            tsi = 0;
            sizex = output[i].x2 - output[i].x1;
            sizey = output[i].y2 - output[i].y1;

            for (j = 1; j < sizex; j++) {
                v[1] = v[0];
                switch (type) {
                    case 0:
                        v[0] = (sin(tsi));
                        break;
                    case 1:
                        v[0] = ((sin(tsi) > 0) - 0.5) * 2;
                        break;
                    case 2:
                        v[0] = ((acos(sin(tsi)) / 1.5708) - 1);
                        break;
                }
                tsi += 3 * 6.28 / sizex;
                if (j > 0) {
                    SpareParts.CanvasCmd(
                        {CC_LINE, .Line{output[i].x1 + j - 1, output[i].y1 + ((v[1] + 2.0f) * sizey / 4.0f),
                                        output[i].x1 + j, output[i].y1 + ((v[0] + 2.0f) * sizey / 4.0f)}});
                }
            }

            break;
        case O_AMPL:
            snprintf(temp, 100, "%5.2f", ampl);
            SpareParts.CanvasCmd({CC_SETCOLOR, .SetColor{49, 61, 99}});
            SpareParts.CanvasCmd({CC_RECTANGLE, .Rectangle{1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1,
                                                           output[i].y2 - output[i].y1}});
            SpareParts.CanvasCmd({CC_SETFGCOLOR, .SetFgColor{255, 255, 255}});
            SpareParts.CanvasCmd({CC_ROTATEDTEXT, .RotatedText{temp, output[i].x1, output[i].y1, 0}});
            break;
        case O_OFFS:
            snprintf(temp, 100, "%5.2f", offs);
            SpareParts.CanvasCmd({CC_SETCOLOR, .SetColor{49, 61, 99}});
            SpareParts.CanvasCmd({CC_RECTANGLE, .Rectangle{1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1,
                                                           output[i].y2 - output[i].y1}});
            SpareParts.CanvasCmd({CC_SETFGCOLOR, .SetFgColor{255, 255, 255}});
            SpareParts.CanvasCmd({CC_ROTATEDTEXT, .RotatedText{temp, output[i].x1, output[i].y1, 0}});
            break;
        case O_FREQ:
            snprintf(temp, 100, "%5.2f", freq);
            SpareParts.CanvasCmd({CC_SETCOLOR, .SetColor{49, 61, 99}});
            SpareParts.CanvasCmd({CC_RECTANGLE, .Rectangle{1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1,
                                                           output[i].y2 - output[i].y1}});
            SpareParts.CanvasCmd({CC_SETFGCOLOR, .SetFgColor{255, 255, 255}});
            SpareParts.CanvasCmd({CC_ROTATEDTEXT, .RotatedText{temp, output[i].x1, output[i].y1, 0}});
            break;
        case O_MF:
            SpareParts.CanvasCmd({CC_SETCOLOR, .SetColor{255, 255, 255}});
            SpareParts.CanvasCmd({CC_RECTANGLE, .Rectangle{1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1,
                                                           output[i].y2 - output[i].y1}});
            SpareParts.CanvasCmd({CC_SETCOLOR, .SetColor{0, 0, 0}});
            snprintf(temp, 100, "F x %i", maxfreq);
            SpareParts.CanvasCmd({CC_ROTATEDTEXT, .RotatedText{temp, output[i].x1, output[i].y1, 0}});
            break;
    }
}

void cpart_SignalGenerator::PreProcess(void) {
    JUMPSTEPS_ = (pboard->MGetInstClockFreq() / 250000);
    mcount = JUMPSTEPS_;

    freq = (maxfreq * values[1] / 200.0);
    ampl = (5.0 * values[0] / 200.0);
    offs = (5.0 * values[2] / 200.0);
}

void cpart_SignalGenerator::Process(void) {
    mcount++;

    if (mcount > JUMPSTEPS_) {
        float v = 0;
        float wt = freq * 2.0 * M_PI * ts;

        switch (type) {
            case 0:
                v = (ampl * sin(wt)) + offs;
                break;
            case 1:
                v = ((sin(wt) > 0) - 0.5) * 2 * ampl + offs;
                break;
            case 2:
                v = ((acos(sin(wt)) / 1.5708) - 1) * ampl + offs;
                break;
        }
        ts += 4e-6;

        if (wt >= 2.0 * M_PI) {
            ts = ts - (1.0 / freq);
        }

        SpareParts.SetAPin(input_pins[0], v);
        SpareParts.SetAPin(input_pins[1], v);

        unsigned char vald = v > offs;
        if (vald != lastd) {
            lastd = vald;
            SpareParts.SetPin(input_pins[0], vald);
            SpareParts.SetPin(input_pins[1], vald);
        }
        mcount = 1;
    }
}

void cpart_SignalGenerator::OnMouseButtonPress(unsigned int inputId, unsigned int button, unsigned int x,
                                               unsigned int y, unsigned int state) {
    switch (inputId) {
        case I_PO1:
            values[0] = (y - input_ids[I_PO1]->y1) * 1.66;
            if (values[0] > 200)
                values[0] = 200;
            active[0] = 1;
            output_ids[O_PO1]->update = 1;
            output_ids[O_AMPL]->update = 1;
            break;
        case I_PO2:
            values[1] = (y - input_ids[I_PO2]->y1) * 1.66;
            if (values[1] > 200)
                values[1] = 200;
            active[1] = 1;
            output_ids[O_PO2]->update = 1;
            output_ids[O_FREQ]->update = 1;
            break;
        case I_PO3:
            values[2] = (y - input_ids[I_PO3]->y1) * 1.66;
            if (values[2] > 200)
                values[2] = 200;
            active[2] = 1;
            output_ids[O_PO3]->update = 1;
            output_ids[O_OFFS]->update = 1;
            break;
        case I_TP:
            type++;
            if (type > 2)
                type = 0;
            output_ids[O_TP]->update = 1;
            break;
        case I_MF:
            maxfreq *= 10;
            if (maxfreq > 10000)
                maxfreq = 1;
            output_ids[O_MF]->update = 1;
            break;
    }
}

void cpart_SignalGenerator::OnMouseButtonRelease(unsigned int inputId, unsigned int button, unsigned int x,
                                                 unsigned int y, unsigned int state) {
    switch (inputId) {
        case I_PO1:
            active[0] = 0;
            output_ids[O_PO1]->update = 1;
            output_ids[O_AMPL]->update = 1;
            break;
        case I_PO2:
            active[1] = 0;
            output_ids[O_PO2]->update = 1;
            output_ids[O_FREQ]->update = 1;
            break;
        case I_PO3:
            active[2] = 0;
            output_ids[O_PO3]->update = 1;
            output_ids[O_OFFS]->update = 1;
            break;
    }
}

void cpart_SignalGenerator::OnMouseMove(unsigned int inputId, unsigned int button, unsigned int x, unsigned int y,
                                        unsigned int state) {
    switch (inputId) {
        case I_PO1:
            if (active[0]) {
                values[0] = (y - input_ids[I_PO1]->y1) * 1.66;
                if (values[0] > 200)
                    values[0] = 200;
                output_ids[O_PO1]->update = 1;
                output_ids[O_AMPL]->update = 1;
            }
            break;
        case I_PO2:
            if (active[1]) {
                values[1] = (y - input_ids[I_PO2]->y1) * 1.66;
                if (values[1] > 200)
                    values[1] = 200;
                output_ids[O_PO2]->update = 1;
                output_ids[O_FREQ]->update = 1;
            }
            break;
        case I_PO3:
            if (active[2]) {
                values[2] = (y - input_ids[I_PO3]->y1) * 1.66;
                if (values[2] > 200)
                    values[2] = 200;
                output_ids[O_PO3]->update = 1;
                output_ids[O_OFFS]->update = 1;
            }
            break;
        default:
            active[0] = 0;
            active[1] = 0;
            active[2] = 0;
            break;
    }
}

unsigned short cpart_SignalGenerator::GetInputId(char* name) {
    if (strcmp(name, "PO_1") == 0)
        return I_PO1;
    if (strcmp(name, "PO_2") == 0)
        return I_PO2;
    if (strcmp(name, "PO_3") == 0)
        return I_PO3;
    if (strcmp(name, "PB_TP") == 0)
        return I_TP;
    if (strcmp(name, "PB_MF") == 0)
        return I_MF;

    printf("Error input '%s' don't have a valid id! \n", name);
    return INVALID_ID;
}

unsigned short cpart_SignalGenerator::GetOutputId(char* name) {
    if (strcmp(name, "PN_1") == 0)
        return O_P1;
    if (strcmp(name, "PN_2") == 0)
        return O_P2;
    if (strcmp(name, "PN_3") == 0)
        return O_P3;

    if (strcmp(name, "PO_1") == 0)
        return O_PO1;
    if (strcmp(name, "PO_2") == 0)
        return O_PO2;
    if (strcmp(name, "PO_3") == 0)
        return O_PO3;
    if (strcmp(name, "PB_TP") == 0)
        return O_TP;
    if (strcmp(name, "DI_AMPL") == 0)
        return O_AMPL;
    if (strcmp(name, "DI_OFFS") == 0)
        return O_OFFS;
    if (strcmp(name, "DI_FREQ") == 0)
        return O_FREQ;
    if (strcmp(name, "PB_MF") == 0)
        return O_MF;

    printf("Error output '%s' don't have a valid id! \n", name);
    return INVALID_ID;
}

std::string cpart_SignalGenerator::WritePreferences(void) {
    char prefs[256];

    sprintf(prefs, "%hhu,%hhu,%hhu,%hhu,%u,%hhu,%hhu", input_pins[0], values[0], values[1], type, maxfreq,
            input_pins[1], values[2]);

    return prefs;
}

void cpart_SignalGenerator::ReadPreferences(std::string value) {
    sscanf(value.c_str(), "%hhu,%hhu,%hhu,%hhu,%u,%hhu,%hhu", &input_pins[0], &values[0], &values[1], &type, &maxfreq,
           &input_pins[1], &values[2]);
}

void cpart_SignalGenerator::RegisterRemoteControl(void) {
    output_ids[O_AMPL]->status = (void*)&ampl;
    output_ids[O_FREQ]->status = (void*)&freq;
    output_ids[O_OFFS]->status = (void*)&offs;
}

void cpart_SignalGenerator::ConfigurePropertiesWindow(void) {
    SetPCWComboWithPinNames("combo2", input_pins[0]);
    SetPCWComboWithPinNames("combo3", input_pins[1]);
}

void cpart_SignalGenerator::ReadPropertiesWindow(void) {
    input_pins[0] = GetPWCComboSelectedPin("combo2");
    input_pins[1] = GetPWCComboSelectedPin("combo3");
}

part_init(PART_SIGNALGENERATOR_Name, cpart_SignalGenerator, "Virtual");
