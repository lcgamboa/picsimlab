/* ########################################################################

   PICSimLab - Programmable IC Simulator Laboratory

   ########################################################################

   Copyright (c) : 2010-2024  Luis Claudio Gambôa Lopes <lcgamboa@yahoo.com>

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

#include "input_ds18b20.h"
#include "../lib/oscilloscope.h"
#include "../lib/picsimlab.h"
#include "../lib/spareparts.h"

/* outputs */
enum { O_P1, O_F1, O_F2, O_PO1 };

/* inputs */
enum { I_PO1 };

static PCWProp pcwprop[4] = {{PCW_LABEL, "1 - GND,GND"},
                             {PCW_COMBO, "2 - DQ"},
                             {PCW_LABEL, "3 - VCC,+5V"},
                             {PCW_END, ""}};

cpart_ds18b20::cpart_ds18b20(const unsigned x, const unsigned y, const char* name, const char* type, board* pboard_,
                             const int id_)
    : part(x, y, name, type, pboard_, id_) {
    output_pins[0] = 0;

    values[0] = 0;

    active[0] = 0;

    sen_ds18b20_init(&ds18b20, pboard);

    SetPCWProperties(pcwprop);

    PinCount = 1;
    Pins = output_pins;
}

void cpart_ds18b20::RegisterRemoteControl(void) {
    input_ids[I_PO1]->status = &values[0];
    input_ids[I_PO1]->update = &output_ids[O_PO1]->update;
}

cpart_ds18b20::~cpart_ds18b20(void) {
    sen_ds18b20_end(&ds18b20);
    SpareParts.SetPartOnDraw(id);
    SpareParts.CanvasCmd({CC_FREEBITMAP, .FreeBitmap{BitmapId}});
    SpareParts.CanvasCmd({CC_DESTROY});
}

void cpart_ds18b20::DrawOutput(const unsigned int i) {
    char val[10];

    switch (output[i].id) {
        case O_P1:
            SpareParts.CanvasCmd({CC_SETCOLOR, .SetColor{49, 61, 99}});
            SpareParts.CanvasCmd({CC_RECTANGLE, .Rectangle{1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1,
                                                           output[i].y2 - output[i].y1}});
            SpareParts.CanvasCmd({CC_SETFGCOLOR, .SetFgColor{255, 255, 255}});
            if (output_pins[output[i].id - O_P1] == 0)
                SpareParts.CanvasCmd({CC_ROTATEDTEXT, .RotatedText{"NC", output[i].x1, output[i].y2, 90}});
            else
                SpareParts.CanvasCmd(
                    {CC_ROTATEDTEXT, .RotatedText{SpareParts.GetPinName(output_pins[output[i].id - O_P1]).c_str(),
                                                  output[i].x1, output[i].y2, 90}});
            break;
        case O_F1:
            SpareParts.CanvasCmd({CC_SETCOLOR, .SetColor{49, 61, 99}});
            SpareParts.CanvasCmd({CC_RECTANGLE, .Rectangle{1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1,
                                                           output[i].y2 - output[i].y1}});
            SpareParts.CanvasCmd({CC_SETFGCOLOR, .SetFgColor{155, 155, 155}});
            SpareParts.CanvasCmd({CC_ROTATEDTEXT, .RotatedText{"GND", output[i].x1, output[i].y2, 90}});
            break;
        case O_F2:
            SpareParts.CanvasCmd({CC_SETCOLOR, .SetColor{49, 61, 99}});
            SpareParts.CanvasCmd({CC_RECTANGLE, .Rectangle{1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1,
                                                           output[i].y2 - output[i].y1}});
            SpareParts.CanvasCmd({CC_SETFGCOLOR, .SetFgColor{155, 155, 155}});
            SpareParts.CanvasCmd({CC_ROTATEDTEXT, .RotatedText{"+5V", output[i].x1, output[i].y2, 90}});
            break;
        case O_PO1:
            snprintf(val, 10, "%5.1f", (0.6 * (200 - values[0]) - 40));
            DrawSlider(SpareParts.CanvasCmd, &output[i], values[0], val, 7);
            SpareParts.CanvasCmd({CC_SETFONTSIZE, .SetFontSize{9}});
            break;
    }
}

void cpart_ds18b20::Reset(void) {
    sen_ds18b20_rst(&ds18b20);
    if (output_pins[0]) {
        SpareParts.SetPin(output_pins[0], 0);
    }
}

void cpart_ds18b20::PreProcess(void) {
    if (output_pins[0]) {
        sen_ds18b20_setTemp(&ds18b20, (0.6 * (200 - values[0]) - 40));
        SpareParts.ResetPullupBus(output_pins[0] - 1);
    }
}

void cpart_ds18b20::Process(void) {
    if (output_pins[0]) {
        const picpin* ppins = SpareParts.GetPinsValues();

        if ((ppins[output_pins[0] - 1].dir) && (ppins[output_pins[0] - 1].value != ds18b20.out)) {
            SpareParts.SetPin(output_pins[0], ds18b20.out);
        }

        SpareParts.SetPullupBus(output_pins[0] - 1, sen_ds18b20_io(&ds18b20, ppins[output_pins[0] - 1].value));
    }
}

void cpart_ds18b20::OnMouseButtonPress(unsigned int inputId, unsigned int button, unsigned int x, unsigned int y,
                                       unsigned int state) {
    switch (inputId) {
        case I_PO1:
            values[0] = (y - input_ids[I_PO1]->y1) * 1.66;
            if (values[0] > 200)
                values[0] = 200;
            active[0] = 1;
            output_ids[O_PO1]->update = 1;
            break;
    }
}

void cpart_ds18b20::OnMouseButtonRelease(unsigned int inputId, unsigned int button, unsigned int x, unsigned int y,
                                         unsigned int state) {
    switch (inputId) {
        case I_PO1:
            active[0] = 0;
            output_ids[O_PO1]->update = 1;
            break;
    }
}

void cpart_ds18b20::OnMouseMove(unsigned int inputId, unsigned int button, unsigned int x, unsigned int y,
                                unsigned int state) {
    switch (inputId) {
        case I_PO1:
            if (active[0]) {
                values[0] = (y - input_ids[I_PO1]->y1) * 1.66;
                if (values[0] > 200)
                    values[0] = 200;
                output_ids[O_PO1]->update = 1;
            }
            break;
        default:
            active[0] = 0;
            break;
    }
}

unsigned short cpart_ds18b20::GetInputId(char* name) {
    if (strcmp(name, "PO_1") == 0)
        return I_PO1;

    printf("Error input '%s' don't have a valid id! \n", name);
    return INVALID_ID;
}

unsigned short cpart_ds18b20::GetOutputId(char* name) {
    if (strcmp(name, "PN_1") == 0)
        return O_P1;

    if (strcmp(name, "PN_F1") == 0)
        return O_F1;
    if (strcmp(name, "PN_F2") == 0)
        return O_F2;

    if (strcmp(name, "PO_1") == 0)
        return O_PO1;

    printf("Error output '%s' don't have a valid id! \n", name);
    return INVALID_ID;
}

std::string cpart_ds18b20::WritePreferences(void) {
    char prefs[256];

    sprintf(prefs, "%hhu,%hhu,%" SCNu64, output_pins[0], values[0], sen_ds18b20_get_addr(&ds18b20));

    return prefs;
}

void cpart_ds18b20::ReadPreferences(std::string value_) {
    uint64_t addr;
    sscanf(value_.c_str(), "%hhu,%hhu,%" SCNu64, &output_pins[0], &values[0], &addr);
    sen_ds18b20_set_addr(&ds18b20, addr);
}

void cpart_ds18b20::ConfigurePropertiesWindow(void) {
    SetPCWComboWithPinNames("combo2", output_pins[0]);
}

void cpart_ds18b20::ReadPropertiesWindow(void) {
    output_pins[0] = GetPWCComboSelectedPin("combo2");
}

part_init(PART_DS1820B_Name, cpart_ds18b20, "Input");
