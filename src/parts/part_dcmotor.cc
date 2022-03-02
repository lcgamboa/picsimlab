/* ########################################################################

   PICsimLab - PIC laboratory simulator

   ########################################################################

   Copyright (c) : 2010-2022  Luis Claudio Gambôa Lopes

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

#include "part_dcmotor.h"
#include "../picsimlab1.h"
#include "../picsimlab4.h"
#include "../picsimlab5.h"

#define dprintf \
    if (1) {    \
    } else      \
        printf

/* outputs */
enum { O_MT1, O_ST, O_P1, O_P2, O_P3, O_P4, O_P5 };

cpart_dcmotor::cpart_dcmotor(unsigned x, unsigned y)
    : font(9, lxFONTFAMILY_TELETYPE, lxFONTSTYLE_NORMAL, lxFONTWEIGHT_BOLD) {
    X = x;
    Y = y;
    aways_update = 1;
    ReadMaps();

    LoadImage();

    output_pins[0] = 0;
    output_pins[1] = 0;

    input_pins[0] = 0;
    input_pins[1] = 0;
    input_pins[2] = 0;

    value = 0;
    value_old = 0;

    state = 0;
    count = 0;

    RegisterRemoteControl();

    dir = 0;
    speed = 0;
}

void cpart_dcmotor::RegisterRemoteControl(void) {
    for (int i = 0; i < outputc; i++) {
        switch (output[i].id) {
            case O_MT1:
                status[0] = &dir;
                status[1] = &speed;
                status[2] = &value;
                output[i].status = &status;
                break;
        }
    }
}

cpart_dcmotor::~cpart_dcmotor(void) {
    delete Bitmap;
    canvas.Destroy();
}

void cpart_dcmotor::Draw(void) {
    int i, x, y, hsp;
    // char val[10];

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
                case O_P1:
                case O_P2:
                    canvas.SetColor(49, 61, 99);
                    canvas.Rectangle(1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1,
                                     output[i].y2 - output[i].y1);
                    canvas.SetFgColor(255, 255, 255);
                    if (output_pins[output[i].id - O_P1] == 0)
                        canvas.RotatedText("NC", output[i].x1 - 3, output[i].y2, 90);
                    else
                        canvas.RotatedText(Window5.GetPinName(output_pins[output[i].id - O_P1]), output[i].x1 - 3,
                                           output[i].y2, 90);
                    break;
                case O_P3:
                case O_P4:
                case O_P5:
                    canvas.SetColor(49, 61, 99);
                    canvas.Rectangle(1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1,
                                     output[i].y2 - output[i].y1);
                    canvas.SetFgColor(255, 255, 255);
                    if (input_pins[output[i].id - O_P3] == 0)
                        canvas.RotatedText("NC", output[i].x1 - 3, output[i].y2, 90);
                    else
                        canvas.RotatedText(Window5.GetPinName(input_pins[output[i].id - O_P3]), output[i].x1 - 3,
                                           output[i].y2, 90);
                    break;
                case O_MT1:
                    canvas.SetColor(153, 153, 153);
                    canvas.Rectangle(1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1,
                                     output[i].y2 - output[i].y1);

                    canvas.SetColor(250, 250, 250);
                    canvas.Circle(1, output[i].cx, output[i].cy, 25);
                    canvas.SetColor(5, 5, 5);
                    canvas.Circle(1, output[i].cx, output[i].cy, 2);

                    canvas.SetColor(250, 0, 0);
                    x = -18 * sin((2 * M_PI * (value / 200.0)));
                    y = 18 * cos((2 * M_PI * (value / 200.0)));
                    canvas.Circle(1, output[i].cx + x, output[i].cy + y, 5);
                    break;
                case O_ST:
                    canvas.SetFgColor(0, 0, 0);
                    canvas.SetBgColor(200, 200, 200);
                    canvas.Rectangle(1, output[i].x1 - 2, output[i].y1 - 2, output[i].x2 - output[i].x1 + 4,
                                     output[i].y2 - output[i].y1 + 4);

                    hsp = (speed / 3);

                    if (hsp > 21) {
                        canvas.SetColor(0, 255, 0);
                        if (hsp > 32)
                            hsp = 32;
                    } else if (hsp > 10) {
                        canvas.SetColor(255, 255, 0);
                    } else {
                        canvas.SetColor(255, 0, 0);
                    }

                    if (dir) {
                        canvas.Rectangle(1, output[i].x1, output[i].y1 + 32, output[i].x2 - output[i].x1, hsp);
                    } else {
                        canvas.Rectangle(1, output[i].x1, output[i].y1 + (32 - hsp), output[i].x2 - output[i].x1, hsp);
                    }
                    break;
            }
        }
    }

    if (Update) {
        canvas.End();
    }
}

void cpart_dcmotor::PreProcess(void) {
    const picpin* ppins = Window5.GetPinsValues();

    int ia = 0, ib = 0;
    float da;

    if (input_pins[0]) {
        ia = ppins[input_pins[0] - 1].value;
    }

    if (input_pins[1]) {
        ib = ppins[input_pins[1] - 1].value;
    }

    if (input_pins[2]) {
        // TODO Add transfer funcion of dc motor
        speed = (ppins[input_pins[2] - 1].oavalue - 55) / 2;
    }

    if (ia && !ib) {
        dir = 0;
        value += speed / 3;
        if (value > 199)
            value -= 200;
    } else if (!ia && ib) {
        dir = 1;
        value -= speed / 3;
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
                if (output_pins[0])
                    Window5.SetPin(output_pins[0], 0);
                break;
            case 1:
                if (output_pins[1])
                    Window5.SetPin(output_pins[1], 1);
                break;
            case 2:
                if (output_pins[0])
                    Window5.SetPin(output_pins[0], 1);
                break;
            case 3:
                if (output_pins[1])
                    Window5.SetPin(output_pins[1], 0);
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
          //step = Window1.GetBoard ()->MGetInstClock () / ((da > 0) ? da * 10 : -da * 10);
         }
         */
    } else {
        step = Window1.GetBoard()->MGetInstClockFreq() / ((da > 0) ? da * 10 : -da * 10);
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
                    if (output_pins[0])
                        Window5.SetPin(output_pins[0], 0);
                    break;
                case 1:
                    if (output_pins[1])
                        Window5.SetPin(output_pins[1], 1);
                    break;
                case 2:
                    if (output_pins[0])
                        Window5.SetPin(output_pins[0], 1);
                    break;
                case 3:
                    if (output_pins[1])
                        Window5.SetPin(output_pins[1], 0);
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

unsigned short cpart_dcmotor::get_in_id(char* name) {
    printf("Erro input '%s' don't have a valid id! \n", name);
    return -1;
}

unsigned short cpart_dcmotor::get_out_id(char* name) {
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

    printf("Erro output '%s' don't have a valid id! \n", name);
    return 1;
}

lxString cpart_dcmotor::WritePreferences(void) {
    char prefs[256];

    sprintf(prefs, "%hhu,%hhu,%hhu,%hhu,%hhu", output_pins[0], output_pins[1], input_pins[0], input_pins[1],
            input_pins[2]);

    return prefs;
}

void cpart_dcmotor::ReadPreferences(lxString value) {
    sscanf(value.c_str(), "%hhu,%hhu,%hhu,%hhu,%hhu", &output_pins[0], &output_pins[1], &input_pins[0], &input_pins[1],
           &input_pins[2]);
}

void cpart_dcmotor::ConfigurePropertiesWindow(CPWindow* WProp) {
    lxString Items = Window5.GetPinsNames();
    lxString spin;

    ((CCombo*)WProp->GetChildByName("combo1"))->SetItems(Items);
    if (output_pins[0] == 0)
        ((CCombo*)WProp->GetChildByName("combo1"))->SetText("0  NC");
    else {
        spin = Window5.GetPinName(output_pins[0]);
        ((CCombo*)WProp->GetChildByName("combo1"))->SetText(itoa(output_pins[0]) + "  " + spin);
    }

    ((CCombo*)WProp->GetChildByName("combo2"))->SetItems(Items);
    if (output_pins[1] == 0)
        ((CCombo*)WProp->GetChildByName("combo2"))->SetText("0  NC");
    else {
        spin = Window5.GetPinName(output_pins[1]);
        ((CCombo*)WProp->GetChildByName("combo2"))->SetText(itoa(output_pins[1]) + "  " + spin);
    }

    ((CCombo*)WProp->GetChildByName("combo3"))->SetItems(Items);
    if (input_pins[0] == 0)
        ((CCombo*)WProp->GetChildByName("combo3"))->SetText("0  NC");
    else {
        spin = Window5.GetPinName(input_pins[0]);
        ((CCombo*)WProp->GetChildByName("combo3"))->SetText(itoa(input_pins[0]) + "  " + spin);
    }

    ((CCombo*)WProp->GetChildByName("combo4"))->SetItems(Items);
    if (input_pins[1] == 0)
        ((CCombo*)WProp->GetChildByName("combo4"))->SetText("0  NC");
    else {
        spin = Window5.GetPinName(input_pins[1]);
        ((CCombo*)WProp->GetChildByName("combo4"))->SetText(itoa(input_pins[1]) + "  " + spin);
    }

    ((CCombo*)WProp->GetChildByName("combo5"))->SetItems(Items);
    if (input_pins[2] == 0)
        ((CCombo*)WProp->GetChildByName("combo5"))->SetText("0  NC");
    else {
        spin = Window5.GetPinName(input_pins[2]);
        ((CCombo*)WProp->GetChildByName("combo5"))->SetText(itoa(input_pins[2]) + "  " + spin);
    }

    ((CButton*)WProp->GetChildByName("button1"))->EvMouseButtonRelease =
        EVMOUSEBUTTONRELEASE & CPWindow5::PropButtonRelease;
    ((CButton*)WProp->GetChildByName("button1"))->SetTag(1);

    ((CButton*)WProp->GetChildByName("button2"))->EvMouseButtonRelease =
        EVMOUSEBUTTONRELEASE & CPWindow5::PropButtonRelease;
}

void cpart_dcmotor::ReadPropertiesWindow(CPWindow* WProp) {
    output_pins[0] = atoi(((CCombo*)WProp->GetChildByName("combo1"))->GetText());
    output_pins[1] = atoi(((CCombo*)WProp->GetChildByName("combo2"))->GetText());
    input_pins[0] = atoi(((CCombo*)WProp->GetChildByName("combo3"))->GetText());
    input_pins[1] = atoi(((CCombo*)WProp->GetChildByName("combo4"))->GetText());
    input_pins[2] = atoi(((CCombo*)WProp->GetChildByName("combo5"))->GetText());
}

part_init(PART_DCMOTOR_Name, cpart_dcmotor, "Output");
