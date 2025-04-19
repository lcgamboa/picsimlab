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

#define _USE_MATH_DEFINES

#include "output_step.h"
#include "../lib/oscilloscope.h"
#include "../lib/picsimlab.h"
#include "../lib/spareparts.h"
#include "math.h"

/* outputs */
enum { O_P1, O_P2, O_P3, O_P4, O_P5, O_L1, O_L2, O_L3, O_L4, O_ROT, O_STEPS };

static PCWProp pcwprop[6] = {{PCW_COMBO, "Pin 1"}, {PCW_COMBO, "Pin 2"}, {PCW_COMBO, "Pin 3"},
                             {PCW_COMBO, "Pin 4"}, {PCW_COMBO, "Home"},  {PCW_END, ""}};

cpart_step::cpart_step(const unsigned x, const unsigned y, const char* name, const char* type, board* pboard_,
                       const int id_)
    : part(x, y, name, type, pboard_, id_) {
    angle = 0;

    steps = 0;

    input_pins[0] = 0;
    input_pins[1] = 0;
    input_pins[2] = 0;
    input_pins[3] = 0;

    output_pins[0] = 0;

    input[0] = 0;
    input[1] = 0;

    turns = 0;

    SetPCWProperties(pcwprop);

    PinCount = 4;
    Pins = input_pins;
}

cpart_step::~cpart_step(void) {
    SpareParts.SetPartOnDraw(id);
    SpareParts.CanvasCmd({.cmd = CC_FREEBITMAP, .FreeBitmap{BitmapId}});
    SpareParts.CanvasCmd({.cmd = CC_DESTROY});
}

void cpart_step::DrawOutput(const unsigned int i) {
    float x2, y2;
    const picpin* ppins = SpareParts.GetPinsValues();

    switch (output[i].id) {
        case O_P1:
        case O_P2:
        case O_P3:
        case O_P4:
            SpareParts.CanvasCmd({.cmd = CC_SETCOLOR, .SetColor{49, 61, 99}});
            SpareParts.CanvasCmd(
                {.cmd = CC_RECTANGLE,
                 .Rectangle{1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1}});
            SpareParts.CanvasCmd({.cmd = CC_SETFGCOLOR, .SetFgColor{255, 255, 255}});
            if (input_pins[output[i].id - O_P1] == 0)
                SpareParts.CanvasCmd({.cmd = CC_ROTATEDTEXT, .RotatedText{"NC", output[i].x1, output[i].y1, 0}});
            else
                SpareParts.CanvasCmd({.cmd = CC_ROTATEDTEXT,
                                      .RotatedText{SpareParts.GetPinName(input_pins[output[i].id - O_P1]).c_str(),
                                                   output[i].x1, output[i].y1, 0}});
            break;
        case O_P5:
            SpareParts.CanvasCmd({.cmd = CC_SETCOLOR, .SetColor{49, 61, 99}});
            SpareParts.CanvasCmd(
                {.cmd = CC_RECTANGLE,
                 .Rectangle{1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1}});
            SpareParts.CanvasCmd({.cmd = CC_SETFGCOLOR, .SetFgColor{255, 255, 255}});
            if (output_pins[output[i].id - O_P5] == 0)
                SpareParts.CanvasCmd({.cmd = CC_ROTATEDTEXT, .RotatedText{"NC", output[i].x1, output[i].y1, 0}});
            else
                SpareParts.CanvasCmd({.cmd = CC_ROTATEDTEXT,
                                      .RotatedText{SpareParts.GetPinName(output_pins[output[i].id - O_P5]).c_str(),
                                                   output[i].x1, output[i].y1, 0}});
            break;
        case O_STEPS: {
            SpareParts.CanvasCmd({.cmd = CC_SETCOLOR, .SetColor{49, 61, 99}});
            SpareParts.CanvasCmd(
                {.cmd = CC_RECTANGLE,
                 .Rectangle{1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1}});
            SpareParts.CanvasCmd({.cmd = CC_SETFGCOLOR, .SetFgColor{255, 255, 255}});
            char text[100];
            snprintf(text, 100, "S=%5.1f", steps);
            SpareParts.CanvasCmd({.cmd = CC_ROTATEDTEXT, .RotatedText{text, output[i].x1, output[i].y1, 0}});
            snprintf(text, 100, "T=%3i", turns);
            SpareParts.CanvasCmd({.cmd = CC_ROTATEDTEXT, .RotatedText{text, output[i].x1, output[i].y1 + 12, 0}});
        } break;
        case O_ROT:
            SpareParts.CanvasCmd({.cmd = CC_SETCOLOR, .SetColor{77, 77, 77}});
            SpareParts.CanvasCmd({.cmd = CC_CIRCLE, .Circle{1, output[i].x1, output[i].y1, output[i].r + 10}});
            SpareParts.CanvasCmd({.cmd = CC_SETFGCOLOR, .SetFgColor{0, 0, 0}});
            SpareParts.CanvasCmd({.cmd = CC_SETBGCOLOR, .SetBgColor{250, 250, 250}});
            SpareParts.CanvasCmd({.cmd = CC_CIRCLE, .Circle{1, output[i].x1, output[i].y1, output[i].r}});
            SpareParts.CanvasCmd({.cmd = CC_SETBGCOLOR, .SetBgColor{55, 55, 55}});
            SpareParts.CanvasCmd({.cmd = CC_CIRCLE, .Circle{1, output[i].x1, output[i].y1, output[i].r / 3}});
            SpareParts.CanvasCmd({.cmd = CC_SETLINEWIDTH, .SetLineWidth{8}});
            x2 = output[i].x1 + (output[i].r - 2) * sin(angle);
            y2 = output[i].y1 + (output[i].r - 2) * cos(angle);
            SpareParts.CanvasCmd({.cmd = CC_LINE, .Line{output[i].x1, output[i].y1, x2, y2}});
            SpareParts.CanvasCmd({.cmd = CC_SETLINEWIDTH, .SetLineWidth{6}});
            SpareParts.CanvasCmd({.cmd = CC_SETFGCOLOR, .SetFgColor{77, 77, 77}});
            SpareParts.CanvasCmd({.cmd = CC_LINE, .Line{output[i].x1, output[i].y1, x2, y2}});
            SpareParts.CanvasCmd({.cmd = CC_SETLINEWIDTH, .SetLineWidth{1}});
            break;
        case O_L1:
        case O_L2:
        case O_L3:
        case O_L4:
            SpareParts.CanvasCmd({.cmd = CC_SETFGCOLOR, .SetFgColor{0, 0, 0}});
            if (input_pins[output[i].id - O_L1] > 0) {
                SpareParts.CanvasCmd(
                    {.cmd = CC_SETBGCOLOR,
                     .SetBgColor{(unsigned int)ppins[input_pins[output[i].id - O_L1] - 1].oavalue, 0, 0}});
            } else {
                SpareParts.CanvasCmd({.cmd = CC_SETBGCOLOR, .SetBgColor{55, 0, 0}});
            }
            DrawLED(SpareParts.CanvasCmd, &output[i]);
            break;
    }
}

void cpart_step::PreProcess(void) {
    if (output_pins[0] > 0) {
        if ((steps == 0) && (turns == 0)) {
            SpareParts.SetPin(output_pins[0], 0);
        } else {
            SpareParts.SetPin(output_pins[0], 1);
        }
    }
}
#define STEP (1.8 * M_PI / 180.0)
#define HSTEP (STEP / 2.0)

#define NSTEP 200  // 360/1.8

#define S1 1
#define S12 3
#define S2 2
#define S23 6
#define S3 4
#define S34 12
#define S4 8
#define S41 9

void cpart_step::Process(void) {
    const picpin* ppins = SpareParts.GetPinsValues();

    if ((input_pins[0] > 0) && (input_pins[1] > 0) && (input_pins[2] > 0) && (input_pins[3] > 0)) {
        input[1] = input[0];
        input[0] = (ppins[input_pins[0] - 1].value << 3) | (ppins[input_pins[1] - 1].value << 2) |
                   (ppins[input_pins[2] - 1].value << 1) | ppins[input_pins[3] - 1].value;

        if (input[0] ^ input[1]) {
            switch (input[0]) {
                case S1:
                    switch (input[1]) {
                        case S4:  // foward full step
                            steps += 1;
                            break;
                        case S2:  // backward full step
                            steps -= 1;
                            break;
                        case S41:  // foward half step
                            steps += 0.5;
                            break;
                        case S12:  // backward half step
                            steps -= 0.5;
                            break;
                    }
                    break;
                case S2:
                    switch (input[1]) {
                        case S1:  // foward full step
                            steps += 1;
                            break;
                        case S3:  // backward full step
                            steps -= 1;
                            break;
                        case S12:  // foward half step
                            steps += 0.5;
                            break;
                        case S23:  // backward half step
                            steps -= 0.5;
                            break;
                    }
                    break;
                case S3:
                    switch (input[1]) {
                        case S2:  // foward full step
                            steps += 1;
                            break;
                        case S4:  // backward full step
                            steps -= 1;
                            break;
                        case S23:  // foward half step
                            steps += 0.5;
                            break;
                        case S34:  // backward half step
                            steps -= 0.5;
                            break;
                    }
                    break;
                case S4:
                    switch (input[1]) {
                        case S3:  // foward full step
                            steps += 1;
                            break;
                        case S1:  // backward full step
                            steps -= 1;
                            break;
                        case S34:  // foward half step
                            steps += 0.5;
                            break;
                        case S41:  // backward half step
                            steps -= 0.5;
                            break;
                    }
                    break;
                case S12:
                    switch (input[1]) {
                        case S41:  // foward full step double phase
                            steps += 1;
                            break;
                        case S23:  // backward full step double phase
                            steps -= 1;
                            break;
                        case S1:  // foward half step
                            steps += 0.5;
                            break;
                        case S2:  // backward half step
                            steps -= 0.5;
                            break;
                    }
                    break;
                case S23:
                    switch (input[1]) {
                        case S12:  // foward full step double phase
                            steps += 1;
                            break;
                        case S34:  // backward full step double phase
                            steps -= 1;
                            break;
                        case S2:  // foward half step
                            steps += 0.5;
                            break;
                        case S3:  // backward half step
                            steps -= 0.5;
                            break;
                    }
                    break;
                case S34:
                    switch (input[1]) {
                        case S23:  // foward full step double phase
                            steps += 1;
                            break;
                        case S41:  // backward full step double phase
                            steps -= 1;
                            break;
                        case S3:  // foward half step
                            steps += 0.5;
                            break;
                        case S4:  // backward half step
                            steps -= 0.5;
                            break;
                    }
                    break;
                case S41:
                    switch (input[1]) {
                        case S34:  // foward full step double phase
                            steps += 1;
                            break;
                        case S12:  // backward full step double phase
                            steps -= 1;
                            break;
                        case S4:  // foward half step
                            steps += 0.5;
                            break;
                        case S1:  // backward half step
                            steps -= 0.5;
                            break;
                    }
                    break;
                default:  // invalid
                    input[0] = input[1];
                    return;
                    break;
            }

            if (steps >= NSTEP) {
                steps -= NSTEP;
                turns++;
            }

            if (steps < 0) {
                steps += NSTEP;
                turns--;
            }

            angle = STEP * steps;

            if (output_pins[0] > 0) {
                if ((steps == 0) && (turns == 0)) {
                    SpareParts.SetPin(output_pins[0], 0);
                } else {
                    SpareParts.SetPin(output_pins[0], 1);
                }
            }
        }
    }
}

void cpart_step::PostProcess(void) {
    const picpin* ppins = SpareParts.GetPinsValues();

    for (int i = 0; i < 4; i++) {
        if (input_pins[i] && (output_ids[O_L1 + i]->value != ppins[input_pins[i] - 1].oavalue)) {
            output_ids[O_L1 + i]->value = ppins[input_pins[i] - 1].oavalue;
            output_ids[O_L1 + i]->update = 1;
        }
    }

    if (output_ids[O_ROT]->value_f != angle) {
        output_ids[O_ROT]->value_f = angle;
        output_ids[O_ROT]->update = 1;
        output_ids[O_STEPS]->update = 1;
    }
}

unsigned short cpart_step::GetInputId(char* name) {
    printf("Error input '%s' don't have a valid id! \n", name);
    return INVALID_ID;
}

unsigned short cpart_step::GetOutputId(char* name) {
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
    if (strcmp(name, "LD_1") == 0)
        return O_L1;
    if (strcmp(name, "LD_2") == 0)
        return O_L2;
    if (strcmp(name, "LD_3") == 0)
        return O_L3;
    if (strcmp(name, "LD_4") == 0)
        return O_L4;
    if (strcmp(name, "DG_ROT") == 0)
        return O_ROT;
    if (strcmp(name, "DI_STEPS") == 0)
        return O_STEPS;

    printf("Error output '%s' don't have a valid id! \n", name);
    return INVALID_ID;
}

std::string cpart_step::WritePreferences(void) {
    char prefs[256];

    sprintf(prefs, "%hhu,%hhu,%hhu,%hhu,%hhu", input_pins[0], input_pins[1], input_pins[2], input_pins[3],
            output_pins[0]);

    return prefs;
}

void cpart_step::ReadPreferences(std::string value) {
    sscanf(value.c_str(), "%hhu,%hhu,%hhu,%hhu,%hhu", &input_pins[0], &input_pins[1], &input_pins[2], &input_pins[3],
           &output_pins[0]);
}

void cpart_step::RegisterRemoteControl(void) {
    output_ids[O_ROT]->status = (void*)&angle;
}

void cpart_step::ConfigurePropertiesWindow(void) {
    SetPCWComboWithPinNames("combo1", input_pins[0]);
    SetPCWComboWithPinNames("combo2", input_pins[1]);
    SetPCWComboWithPinNames("combo3", input_pins[2]);
    SetPCWComboWithPinNames("combo4", input_pins[3]);
    SetPCWComboWithPinNames("combo5", output_pins[0]);
}

void cpart_step::ReadPropertiesWindow(void) {
    input_pins[0] = GetPWCComboSelectedPin("combo1");
    input_pins[1] = GetPWCComboSelectedPin("combo2");
    input_pins[2] = GetPWCComboSelectedPin("combo3");
    input_pins[3] = GetPWCComboSelectedPin("combo4");
    output_pins[0] = GetPWCComboSelectedPin("combo5");
}

part_init(PART_STEP_Name, cpart_step, "Output");
