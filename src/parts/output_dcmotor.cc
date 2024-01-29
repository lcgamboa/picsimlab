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

#include "output_dcmotor.h"
#include "../lib/oscilloscope.h"
#include "../lib/picsimlab.h"
#include "../lib/spareparts.h"

#define dprintf \
    if (1) {    \
    } else      \
        printf

/* outputs */
enum { O_MT1, O_ST, O_P1, O_P2, O_P3, O_P4, O_P5 };

static PCWProp pcwprop[8] = {
    {PCW_COMBO, "1-Out A"},  {PCW_COMBO, "2-Out B"},   {PCW_COMBO, "3-In A"},    {PCW_COMBO, "4-In B"},
    {PCW_COMBO, "5-In PWM"}, {PCW_LABEL, "6-VCC,+5V"}, {PCW_LABEL, "7-GND,GND"}, {PCW_END, ""}};

cpart_dcmotor::cpart_dcmotor(const unsigned x, const unsigned y, const char* name, const char* type, board* pboard_,
                             const int id_)
    : part(x, y, name, type, pboard_, id_) {
    X = x;
    Y = y;
    always_update = 1;
    ReadMaps();

    LoadPartImage();

    pins[0] = 0;
    pins[1] = 0;

    pins[2] = 0;
    pins[3] = 0;
    pins[4] = 0;

    value = 0;
    value_old = 0;

    state = 0;
    count = 0;

    dir = 0;
    speed = 0;

    SetPCWProperties(pcwprop);

    PinCount = 5;
    Pins = pins;
}

void cpart_dcmotor::RegisterRemoteControl(void) {
    status[0] = &dir;
    status[1] = &speed;
    status[2] = &value;
    output_ids[O_MT1]->status = &status;
}

cpart_dcmotor::~cpart_dcmotor(void) {
    delete Bitmap;
    SpareParts.SetPartOnDraw(id);
    SpareParts.CanvasCmd({CC_DESTROY});
}

void cpart_dcmotor::DrawOutput(const unsigned int i) {
    int x, y, hsp;

    switch (output[i].id) {
        case O_P1:
        case O_P2:
        case O_P3:
        case O_P4:
        case O_P5:
            SpareParts.CanvasCmd({CC_SETCOLOR, .SetColor{49, 61, 99}});
            SpareParts.CanvasCmd({CC_RECTANGLE, .Rectangle{1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1,
                                                           output[i].y2 - output[i].y1}});
            SpareParts.CanvasCmd({CC_SETFGCOLOR, .SetFgColor{255, 255, 255}});
            if (pins[output[i].id - O_P1] == 0)
                SpareParts.CanvasCmd({CC_ROTATEDTEXT, .RotatedText{"NC", output[i].x1 - 3, output[i].y2, 90}});
            else
                SpareParts.CanvasCmd(
                    {CC_ROTATEDTEXT, .RotatedText{SpareParts.GetPinName(pins[output[i].id - O_P1]).c_str(),
                                                  output[i].x1 - 3, output[i].y2, 90}});
            break;
        case O_MT1:
            SpareParts.CanvasCmd({CC_SETCOLOR, .SetColor{153, 153, 153}});
            SpareParts.CanvasCmd({CC_RECTANGLE, .Rectangle{1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1,
                                                           output[i].y2 - output[i].y1}});

            SpareParts.CanvasCmd({CC_SETCOLOR, .SetColor{250, 250, 250}});
            SpareParts.CanvasCmd({CC_CIRCLE, .Circle{1, output[i].cx, output[i].cy, 25}});
            SpareParts.CanvasCmd({CC_SETCOLOR, .SetColor{5, 5, 5}});
            SpareParts.CanvasCmd({CC_CIRCLE, .Circle{1, output[i].cx, output[i].cy, 2}});

            SpareParts.CanvasCmd({CC_SETCOLOR, .SetColor{250, 0, 0}});
            x = -18 * sin((2 * M_PI * (value / 200.0)));
            y = 18 * cos((2 * M_PI * (value / 200.0)));
            SpareParts.CanvasCmd({CC_CIRCLE, .Circle{1, output[i].cx + x, output[i].cy + y, 5}});
            break;
        case O_ST:
            SpareParts.CanvasCmd({CC_SETFGCOLOR, .SetFgColor{0, 0, 0}});
            SpareParts.CanvasCmd({CC_SETBGCOLOR, .SetBgColor{200, 200, 200}});
            SpareParts.CanvasCmd(
                {CC_RECTANGLE, .Rectangle{1, output[i].x1 - 2, output[i].y1 - 2, output[i].x2 - output[i].x1 + 4,
                                          output[i].y2 - output[i].y1 + 4}});

            hsp = (speed / 3);

            if (hsp > 21) {
                SpareParts.CanvasCmd({CC_SETCOLOR, .SetColor{0, 255, 0}});
                if (hsp > 32)
                    hsp = 32;
            } else if (hsp > 10) {
                SpareParts.CanvasCmd({CC_SETCOLOR, .SetColor{255, 255, 0}});
            } else {
                SpareParts.CanvasCmd({CC_SETCOLOR, .SetColor{255, 0, 0}});
            }

            if (dir) {
                SpareParts.CanvasCmd({CC_RECTANGLE, .Rectangle{1, output[i].x1, output[i].y1 + 32,
                                                               output[i].x2 - output[i].x1, (float)hsp}});
            } else {
                SpareParts.CanvasCmd({CC_RECTANGLE, .Rectangle{1, output[i].x1, output[i].y1 + (32 - hsp),
                                                               output[i].x2 - output[i].x1, (float)hsp}});
            }
            break;
    }
}

void cpart_dcmotor::PreProcess(void) {
    const picpin* ppins = SpareParts.GetPinsValues();

    int ia = 0, ib = 0;
    float da;

    if (pins[2]) {
        ia = ppins[pins[2] - 1].value;
    }

    if (pins[3]) {
        ib = ppins[pins[3] - 1].value;
    }

    if (pins[4]) {
        // TODO Add transfer funcion of dc motor
        speed = (ppins[pins[4] - 1].oavalue - 55) / 2;
    }

    if (ia && !ib) {
        dir = 0;
        value += speed / (300 / BASETIMER);
        if (value > 199)
            value -= 200;
    } else if (!ia && ib) {
        dir = 1;
        value -= speed / (300 / BASETIMER);
        if (value > 199)
            value -= 57;
    } else {
        speed = 0;
    }

    da = (value / 2.5) - (value_old / 2.5);

    if (da < -40) {
        da = ((value + 200) / 2.5) - (value_old / 2.5);
    }
    if (da > 40) {
        da = ((value - 200) / 2.5) - (value_old / 2.5);
    }

    if (da == 0) {
        step = 0;
    } else if (fabs(da) < 1.0) {
        state = ((value_old) % 10) / 2.5;

        switch (state) {
            case 0:
                if (pins[0])
                    SpareParts.SetPin(pins[0], 0);
                break;
            case 1:
                if (pins[1])
                    SpareParts.SetPin(pins[1], 1);
                break;
            case 2:
                if (pins[0])
                    SpareParts.SetPin(pins[0], 1);
                break;
            case 3:
                if (pins[1])
                    SpareParts.SetPin(pins[1], 0);
                break;
        }
        step = 0;
        // int new_state = ((value) % 10) / 2.5;

        // FIXME on slow speed output is not 90 degrees
        /*
        ///printf ("s=%i ns=%i \n", state, new_state);
        if (state != new_state)
         {
          printf ("need step \n");
          //step = pboard->MGetInstClock () / ((da > 0) ? da * 10 : -da * 10);
         }
         */
    } else {
        step = pboard->MGetInstClockFreq() / ((da > 0) ? da * 10 : -da * 10);
    }

    dprintf("state=%i da=%f  %3i  %3i  dir=%i  step=%i\n", state, da, value, value_old, dir, step);

    value_old = value;
}

void cpart_dcmotor::Process(void) {
    if (step) {
        count++;

        if (count >= step) {
            count = 0;

            if (dir) {
                state++;
                if (state > 3)
                    state = 0;

            } else {
                state--;
                if (state > 3)
                    state = 3;
            }

            dprintf("state=%i\n", state);
            switch (state) {
                case 0:
                    if (pins[0])
                        SpareParts.SetPin(pins[0], 0);
                    break;
                case 1:
                    if (pins[1])
                        SpareParts.SetPin(pins[1], 1);
                    break;
                case 2:
                    if (pins[0])
                        SpareParts.SetPin(pins[0], 1);
                    break;
                case 3:
                    if (pins[1])
                        SpareParts.SetPin(pins[1], 0);
                    break;
            }
        }
    }
}

void cpart_dcmotor::PostProcess(void) {
    if (output_ids[O_MT1]->value != value) {
        output_ids[O_MT1]->value = value;
        output_ids[O_MT1]->update = 1;
        output_ids[O_ST]->update = 1;
    }
}

unsigned short cpart_dcmotor::GetInputId(char* name) {
    printf("Error input '%s' don't have a valid id! \n", name);
    return INVALID_ID;
}

unsigned short cpart_dcmotor::GetOutputId(char* name) {
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

    if (strcmp(name, "ST_1") == 0)
        return O_ST;
    if (strcmp(name, "MT_1") == 0)
        return O_MT1;

    printf("Error output '%s' don't have a valid id! \n", name);
    return INVALID_ID;
}

std::string cpart_dcmotor::WritePreferences(void) {
    char prefs[256];

    sprintf(prefs, "%hhu,%hhu,%hhu,%hhu,%hhu", pins[0], pins[1], pins[2], pins[3], pins[4]);

    return prefs;
}

void cpart_dcmotor::ReadPreferences(std::string value) {
    sscanf(value.c_str(), "%hhu,%hhu,%hhu,%hhu,%hhu", &pins[0], &pins[1], &pins[2], &pins[3], &pins[4]);
}

void cpart_dcmotor::ConfigurePropertiesWindow(CPWindow* WProp) {
    SetPCWComboWithPinNames(WProp, "combo1", pins[0]);
    SetPCWComboWithPinNames(WProp, "combo2", pins[1]);
    SetPCWComboWithPinNames(WProp, "combo3", pins[2]);
    SetPCWComboWithPinNames(WProp, "combo4", pins[3]);
    SetPCWComboWithPinNames(WProp, "combo5", pins[4]);
}

void cpart_dcmotor::ReadPropertiesWindow(CPWindow* WProp) {
    pins[0] = GetPWCComboSelectedPin(WProp, "combo1");
    pins[1] = GetPWCComboSelectedPin(WProp, "combo2");
    pins[2] = GetPWCComboSelectedPin(WProp, "combo3");
    pins[3] = GetPWCComboSelectedPin(WProp, "combo4");
    pins[4] = GetPWCComboSelectedPin(WProp, "combo5");
}

part_init(PART_DCMOTOR_Name, cpart_dcmotor, "Output");
