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

#include "other_logic_block.h"
#include "../lib/oscilloscope.h"
#include "../lib/picsimlab.h"
#include "../lib/spareparts.h"

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
    O_IN1,
    O_IN2,
    O_IN3,
    O_IN4,
    O_IN5,
    O_IN6,
    O_IN7,
    O_IN8,
    O_OUT,
    O_TYPE
};

static PCWProp pcwprop[14] = {{PCW_COMBO, "1 - In 1"},
                              {PCW_COMBO, "2 - In 2"},
                              {PCW_COMBO, "3 - In 3"},
                              {PCW_COMBO, "4 - In 4"},
                              {PCW_COMBO, "5 - In 5"},
                              {PCW_COMBO, "6 - In 6"},
                              {PCW_COMBO, "7 - In 7"},
                              {PCW_COMBO, "8 - In 8"},
                              {PCW_COMBO, "9 - Out"},
                              {PCW_LABEL, "10 - Out,NC"},
                              {PCW_COMBO, "Type"},
                              {PCW_SPIN, "Size"},
                              {PCW_END, ""}};

cpart_lblock::cpart_lblock(const unsigned x, const unsigned y, const char* name, const char* type, board* pboard_,
                           const int id_)
    : part(x, y, name, type, pboard_, id_) {
    gatetype = LG_NOT;
    Size = 0;
    BitmapId = -1;
    always_update = 1;

    ReadMaps();
    OWidth = Width;
    OHeight = Height;

    ChangeSize(1);

    input_pins[0] = 0;
    input_pins[1] = 0;
    input_pins[2] = 0;
    input_pins[3] = 0;
    input_pins[4] = 0;
    input_pins[5] = 0;
    input_pins[6] = 0;
    input_pins[7] = 0;

    output_value = 0;
    output_value_prev = !output_value;

    output_pins[0] = 0;
    output_pins[1] = SpareParts.RegisterIOpin("LB0");

    output_pins_alm = 0;

    SetPCWProperties(pcwprop);

    PinCount = 8;
    Pins = input_pins;
}

void cpart_lblock::Reset(void) {
    // release all
    output_value = 0;

    // force pin update
    SpareParts.SetPin(output_pins[0], output_value);
    SpareParts.SetPin(output_pins[1], output_value);
}

cpart_lblock::~cpart_lblock(void) {
    SpareParts.UnregisterIOpin(output_pins[1]);
    SpareParts.SetPartOnDraw(id);
    SpareParts.CanvasCmd({.cmd = CC_FREEBITMAP, .FreeBitmap{BitmapId}});
    SpareParts.CanvasCmd({.cmd = CC_DESTROY});
}

void cpart_lblock::DrawOutput(const unsigned int i) {
    unsigned char color;
    const picpin* ppins = SpareParts.GetPinsValues();
    switch (output[i].id) {
        case O_P1:
        case O_P2:
        case O_P3:
        case O_P4:
        case O_P5:
        case O_P6:
        case O_P7:
        case O_P8:
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
        case O_P9:
        case O_P10:
            SpareParts.CanvasCmd({.cmd = CC_SETCOLOR, .SetColor{49, 61, 99}});
            SpareParts.CanvasCmd({.cmd = CC_RECTANGLE,
                                  .Rectangle{1, output[i].x1 - xoff, output[i].y1, output[i].x2 - output[i].x1,
                                             output[i].y2 - output[i].y1}});
            SpareParts.CanvasCmd({.cmd = CC_SETFGCOLOR, .SetFgColor{255, 255, 255}});
            if (output_pins[output[i].id - O_P9] == 0)
                SpareParts.CanvasCmd({.cmd = CC_ROTATEDTEXT, .RotatedText{"NC", output[i].x1 - xoff, output[i].y1, 0}});
            else
                SpareParts.CanvasCmd({.cmd = CC_ROTATEDTEXT,
                                      .RotatedText{SpareParts.GetPinName(output_pins[output[i].id - O_P9]).c_str(),
                                                   output[i].x1 - xoff, output[i].y1, 0}});
            break;
        case O_IN1:
        case O_IN2:
        case O_IN3:
        case O_IN4:
        case O_IN5:
        case O_IN6:
        case O_IN7:
        case O_IN8:
            if (input_pins[output[i].id - O_IN1]) {
                color = ppins[input_pins[output[i].id - O_IN1] - 1].oavalue;
            } else {
                color = 0;
            }
            SpareParts.CanvasCmd({.cmd = CC_SETCOLOR, .SetBgColor{(unsigned int)color, 0, 0}});
            SpareParts.CanvasCmd(
                {.cmd = CC_RECTANGLE,
                 .Rectangle{1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1}});
            break;
        case O_OUT:
            SpareParts.CanvasCmd(
                {.cmd = CC_SETCOLOR, .SetBgColor{(unsigned int)ppins[output_pins[1] - 1].oavalue, 0, 0}});
            SpareParts.CanvasCmd({.cmd = CC_RECTANGLE,
                                  .Rectangle{1, output[i].x1 - xoff, output[i].y1, output[i].x2 - output[i].x1,
                                             output[i].y2 - output[i].y1}});
            break;
        case O_TYPE:
            SpareParts.CanvasCmd({.cmd = CC_SETCOLOR, .SetColor{255, 255, 255}});
            SpareParts.CanvasCmd({.cmd = CC_RECTANGLE,
                                  .Rectangle{1, output[i].x1 - xoff, output[i].y1, output[i].x2 - output[i].x1,
                                             output[i].y2 - output[i].y1}});

            SpareParts.CanvasCmd({.cmd = CC_SETFGCOLOR, .SetFgColor{0, 0, 0}});
            SpareParts.CanvasCmd({.cmd = CC_SETBGCOLOR, .SetBgColor{250, 250, 180}});

            switch (gatetype) {
                case LG_NOT:
                    SpareParts.CanvasCmd({.cmd = CC_CIRCLE, .Circle{1, output[i].cx + 16 - xoff, output[i].cy + 0, 3}});
                case LG_BUFFER: {
                    Point_t pts[] = {{-10, -13}, {-10, 13}, {13, 0}};
                    for (int p = 0; p < 3; p++) {
                        // offset
                        pts[p].x += output[i].cx - xoff;
                        pts[p].y += output[i].cy;
                    }
                    SpareParts.CanvasCmd({.cmd = CC_POLYGON, .Polygon{1, pts, 3}});

                } break;

                case LG_NAND:
                    SpareParts.CanvasCmd({.cmd = CC_CIRCLE, .Circle{1, output[i].cx + 19 - xoff, output[i].cy + 0, 3}});
                case LG_AND: {
                    Point_t pts[] = {{0, -16}, {-21, -16}, {-21, 16}, {0, 16}};
                    for (int p = 0; p < 4; p++) {
                        // offset
                        pts[p].x += output[i].cx - xoff;
                        pts[p].y += output[i].cy;
                    }
                    SpareParts.CanvasCmd({.cmd = CC_SETCOLOR, .SetColor{250, 250, 180}});
                    SpareParts.CanvasCmd(
                        {.cmd = CC_RECTANGLE, .Rectangle{1, (float)pts[0].x, (float)pts[0].y, -21, 32}});
                    SpareParts.CanvasCmd({.cmd = CC_SETFGCOLOR, .SetFgColor{0, 0, 0}});
                    SpareParts.CanvasCmd({.cmd = CC_LINES, .Lines{pts, 4}});
                    SpareParts.CanvasCmd(
                        {.cmd = CC_ELLIPTICARC,
                         .EllipticArc{1, output[i].cx + 0 - 16 - xoff, output[i].cy + 0 - 16, 32, 32, -90, 90}});
                } break;
                case LG_NOR:
                    SpareParts.CanvasCmd({.cmd = CC_CIRCLE, .Circle{1, output[i].cx + 25 - xoff, output[i].cy + 0, 3}});
                case LG_OR: {
                    Point_t pts[] = {{-16, 16}, {-6, 16}, {-6, -16}, {-16, -16}};
                    for (int p = 0; p < 4; p++) {
                        // offset
                        pts[p].x += output[i].cx - xoff;
                        pts[p].y += output[i].cy;
                    }
                    SpareParts.CanvasCmd({.cmd = CC_SETFGCOLOR, .SetFgColor{250, 255, 0}});
                    SpareParts.CanvasCmd(
                        {.cmd = CC_RECTANGLE, .Rectangle{1, (float)pts[0].x, (float)pts[0].y, 14, -32}});
                    SpareParts.CanvasCmd({.cmd = CC_SETFGCOLOR, .SetFgColor{0, 0, 0}});
                    SpareParts.CanvasCmd({.cmd = CC_LINES, .Lines{pts, 2}});

                    SpareParts.CanvasCmd({.cmd = CC_LINES, .Lines{&pts[2], 2}});

                    SpareParts.CanvasCmd(
                        {.cmd = CC_ELLIPTICARC,
                         .EllipticArc{1, output[i].cx - 6 - 32 - xoff, output[i].cy + 16 - 32, 64, 64, 30, 90}});
                    SpareParts.CanvasCmd(
                        {.cmd = CC_ELLIPTICARC,
                         .EllipticArc{1, output[i].cx - 6 - 32 - xoff, output[i].cy - 16 - 32, 64, 64, 270, 330}});
                    SpareParts.CanvasCmd({.cmd = CC_SETBGCOLOR, .SetBgColor{255, 255, 255}});
                    SpareParts.CanvasCmd(
                        {.cmd = CC_ELLIPTICARC,
                         .EllipticArc{1, output[i].cx - 37 - 26 - xoff, output[i].cy - 0 - 26, 52, 52, -37, 37}});

                } break;
                case LG_XNOR:
                    SpareParts.CanvasCmd({.cmd = CC_CIRCLE, .Circle{1, output[i].cx + 25 - xoff, output[i].cy + 0, 3}});
                case LG_XOR: {
                    Point_t pts[] = {{-16, 16}, {-6, 16}, {-6, -16}, {-16, -16}};
                    for (int p = 0; p < 4; p++) {
                        // offset
                        pts[p].x += output[i].cx - xoff;
                        pts[p].y += output[i].cy;
                    }
                    SpareParts.CanvasCmd({.cmd = CC_SETFGCOLOR, .SetFgColor{250, 255, 0}});
                    SpareParts.CanvasCmd(
                        {.cmd = CC_RECTANGLE, .Rectangle{1, (float)pts[0].x, (float)pts[0].y, 14, -32}});
                    SpareParts.CanvasCmd({.cmd = CC_SETFGCOLOR, .SetFgColor{0, 0, 0}});
                    SpareParts.CanvasCmd({.cmd = CC_LINES, .Lines{pts, 2}});
                    SpareParts.CanvasCmd({.cmd = CC_LINES, .Lines{&pts[2], 2}});
                    SpareParts.CanvasCmd(
                        {.cmd = CC_ELLIPTICARC,
                         .EllipticArc{1, output[i].cx - 6 - 32 - xoff, output[i].cy + 16 - 32, 64, 64, 30, 90}});
                    SpareParts.CanvasCmd(
                        {.cmd = CC_ELLIPTICARC,
                         .EllipticArc{1, output[i].cx - 6 - 32 - xoff, output[i].cy - 16 - 32, 64, 64, 270, 330}});
                    SpareParts.CanvasCmd({.cmd = CC_SETBGCOLOR, .SetBgColor{255, 255, 255}});
                    SpareParts.CanvasCmd(
                        {.cmd = CC_ELLIPTICARC,
                         .EllipticArc{1, output[i].cx - 37 - 26 - xoff, output[i].cy - 0 - 26, 52, 52, -37, 37}});
                    SpareParts.CanvasCmd(
                        {.cmd = CC_ELLIPTICARC,
                         .EllipticArc{1, output[i].cx - 42 - 26 - xoff, output[i].cy - 0 - 26, 52, 52, -37, 37}});

                } break;
                default:
                    break;
            }
            break;
    }
}

void cpart_lblock::PreProcess(void) {
    output_pins_alm = 0;
    JUMPSTEPS_ = PICSimLab.GetJUMPSTEPS() * 4.0 / PICSimLab.GetBoard()->MGetClocksPerInstructions();
    mcount = JUMPSTEPS_;
}

void cpart_lblock::Process(void) {
    const picpin* ppins = SpareParts.GetPinsValues();

    // Add one clock pulse delay to output
    if (output_value_prev != output_value) {
        SpareParts.SetPin(output_pins[0], output_value);
        SpareParts.WritePin(output_pins[1], output_value);
        output_value_prev = output_value;
        ioupdated = 1;
    }

    if (ioupdated) {
        switch (gatetype) {
            case LG_NOT:
                if (input_pins[0]) {
                    output_value = !ppins[input_pins[0] - 1].value;
                }
                break;
            case LG_BUFFER:
                if (input_pins[0]) {
                    output_value = ppins[input_pins[0] - 1].value;
                }
                break;
            case LG_AND:
                if (input_pins[0]) {
                    output_value = ppins[input_pins[0] - 1].value;
                }
                for (unsigned int i = 1; i < Size; i++) {
                    if (input_pins[i]) {
                        output_value &= ppins[input_pins[i] - 1].value;
                    }
                }
                break;
            case LG_NAND:
                if (input_pins[0]) {
                    output_value = ppins[input_pins[0] - 1].value;
                }
                for (unsigned int i = 1; i < Size; i++) {
                    if (input_pins[i]) {
                        output_value &= ppins[input_pins[i] - 1].value;
                    }
                }
                output_value = !output_value;
                break;
            case LG_OR:
                if (input_pins[0]) {
                    output_value = ppins[input_pins[0] - 1].value;
                }
                for (unsigned int i = 1; i < Size; i++) {
                    if (input_pins[i]) {
                        output_value |= ppins[input_pins[i] - 1].value;
                    }
                }
                break;
            case LG_NOR:
                if (input_pins[0]) {
                    output_value = ppins[input_pins[0] - 1].value;
                }
                for (unsigned int i = 1; i < Size; i++) {
                    if (input_pins[i]) {
                        output_value |= ppins[input_pins[i] - 1].value;
                    }
                }
                output_value = !output_value;
                break;
            case LG_XOR:
                if (input_pins[0]) {
                    output_value = ppins[input_pins[0] - 1].value;
                }
                for (unsigned int i = 1; i < Size; i++) {
                    if (input_pins[i]) {
                        output_value ^= ppins[input_pins[i] - 1].value;
                    }
                }
                break;
            case LG_XNOR:
                if (input_pins[0]) {
                    output_value = ppins[input_pins[0] - 1].value;
                }
                for (unsigned int i = 1; i < Size; i++) {
                    if (input_pins[i]) {
                        output_value ^= ppins[input_pins[i] - 1].value;
                    }
                }
                output_value = !output_value;
                break;
        }
    }

    mcount++;
    if (mcount >= JUMPSTEPS_) {
        if (output_pins[1]) {
            if (ppins[output_pins[1] - 1].value) {
                output_pins_alm++;
            }
        }
        mcount = -1;
    }
}

void cpart_lblock::PostProcess(void) {
    const long int NSTEPJ = PICSimLab.GetNSTEPJ();
    const picpin* ppins = SpareParts.GetPinsValues();

    SpareParts.WritePinOA(output_pins[1], ((output_pins_alm * 200.0) / NSTEPJ) + 55);

    for (unsigned int i = 0; i < Size; i++) {
        if (input_pins[i] && (output_ids[O_IN1 + i]->value != ppins[input_pins[i] - 1].oavalue)) {
            output_ids[O_IN1 + i]->value = ppins[input_pins[i] - 1].oavalue;
            output_ids[O_IN1 + i]->update = 1;
        }
    }

    if ((output_ids[O_OUT]->value != ppins[output_pins[1] - 1].oavalue)) {
        output_ids[O_OUT]->value = ppins[output_pins[1] - 1].oavalue;
        output_ids[O_OUT]->update = 1;
    }
}

unsigned short cpart_lblock::GetInputId(char* name) {
    printf("Error input '%s' don't have a valid id! \n", name);
    return INVALID_ID;
}

unsigned short cpart_lblock::GetOutputId(char* name) {
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

    if (strcmp(name, "IN_1") == 0)
        return O_IN1;
    if (strcmp(name, "IN_2") == 0)
        return O_IN2;
    if (strcmp(name, "IN_3") == 0)
        return O_IN3;
    if (strcmp(name, "IN_4") == 0)
        return O_IN4;
    if (strcmp(name, "IN_5") == 0)
        return O_IN5;
    if (strcmp(name, "IN_6") == 0)
        return O_IN6;
    if (strcmp(name, "IN_7") == 0)
        return O_IN7;
    if (strcmp(name, "IN_8") == 0)
        return O_IN8;

    if (strcmp(name, "OUT") == 0)
        return O_OUT;
    if (strcmp(name, "TYPE") == 0)
        return O_TYPE;

    printf("Error output '%s' don't have a valid id! \n", name);
    return INVALID_ID;
}

std::string cpart_lblock::WritePreferences(void) {
    char prefs[256];

    sprintf(prefs, "%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%u", input_pins[0], input_pins[1],
            input_pins[2], input_pins[3], input_pins[4], input_pins[5], input_pins[6], input_pins[7], output_pins[0],
            output_pins[1], gatetype, Size);

    return prefs;
}

void cpart_lblock::ReadPreferences(std::string value) {
    unsigned int sz;
    unsigned char outp;
    sscanf(value.c_str(), "%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%u", &input_pins[0], &input_pins[1],
           &input_pins[2], &input_pins[3], &input_pins[4], &input_pins[5], &input_pins[6], &input_pins[7],
           &output_pins[0], &outp, &gatetype, &sz);

    SpareParts.UnregisterIOpin(output_pins[1]);
    output_pins[1] = SpareParts.RegisterIOpin("LB0", outp);

    ChangeSize(sz);
}

void cpart_lblock::ConfigurePropertiesWindow(void) {
    SetPCWComboWithPinNames("combo1", input_pins[0]);
    SetPCWComboWithPinNames("combo2", input_pins[1]);
    SetPCWComboWithPinNames("combo3", input_pins[2]);
    SetPCWComboWithPinNames("combo4", input_pins[3]);
    SetPCWComboWithPinNames("combo5", input_pins[4]);
    SetPCWComboWithPinNames("combo6", input_pins[5]);
    SetPCWComboWithPinNames("combo7", input_pins[6]);
    SetPCWComboWithPinNames("combo8", input_pins[7]);
    SetPCWComboWithPinNames("combo9", output_pins[0]);

    SpareParts.WPropCmd("label_10", PWA_LABELSETTEXT, std::to_string(output_pins[1]).c_str());

    SpareParts.WPropCmd("combo11", PWA_COMBOSETITEMS, "NOT,BUFFER,AND,NAND,OR,NOR,XOR,XNOR,");
    SpareParts.WPropCmd("combo11", PWA_COMBOPROPEV, "1");
    switch (gatetype) {
        case LG_NOT:
            SpareParts.WPropCmd("combo11", PWA_COMBOSETTEXT, "NOT");
            SpareParts.WPropCmd("spin12", PWA_SPINSETMAX, "1");
            SpareParts.WPropCmd("spin12", PWA_SPINSETMIN, "1");
            break;
        case LG_BUFFER:
            SpareParts.WPropCmd("combo11", PWA_COMBOSETTEXT, "BUFFER");
            SpareParts.WPropCmd("spin12", PWA_SPINSETMAX, "1");
            SpareParts.WPropCmd("spin12", PWA_SPINSETMIN, "1");
            break;
        case LG_AND:
            SpareParts.WPropCmd("combo11", PWA_COMBOSETTEXT, "AND");
            SpareParts.WPropCmd("spin12", PWA_SPINSETMAX, "8");
            SpareParts.WPropCmd("spin12", PWA_SPINSETMIN, "2");
            break;
        case LG_NAND:
            SpareParts.WPropCmd("combo11", PWA_COMBOSETTEXT, "NAND");
            SpareParts.WPropCmd("spin12", PWA_SPINSETMAX, "8");
            SpareParts.WPropCmd("spin12", PWA_SPINSETMIN, "2");
            break;
        case LG_OR:
            SpareParts.WPropCmd("combo11", PWA_COMBOSETTEXT, "OR");
            SpareParts.WPropCmd("spin12", PWA_SPINSETMAX, "8");
            SpareParts.WPropCmd("spin12", PWA_SPINSETMIN, "2");
            break;
        case LG_NOR:
            SpareParts.WPropCmd("combo11", PWA_COMBOSETTEXT, "NOR");
            SpareParts.WPropCmd("spin12", PWA_SPINSETMAX, "8");
            SpareParts.WPropCmd("spin12", PWA_SPINSETMIN, "2");
            break;
        case LG_XOR:
            SpareParts.WPropCmd("combo11", PWA_COMBOSETTEXT, "XOR");
            SpareParts.WPropCmd("spin12", PWA_SPINSETMAX, "8");
            SpareParts.WPropCmd("spin12", PWA_SPINSETMIN, "2");
            break;
        case LG_XNOR:
            SpareParts.WPropCmd("combo11", PWA_COMBOSETTEXT, "XNOR");
            SpareParts.WPropCmd("spin12", PWA_SPINSETMAX, "8");
            SpareParts.WPropCmd("spin12", PWA_SPINSETMIN, "2");
            break;
    }

    SpareParts.WPropCmd("spin12", PWA_SPINSETVALUE, std::to_string(Size).c_str());
    SpareParts.WPropCmd("spin12", PWA_SPINPROPEV, "1");

    SpinChange(NULL, Size);
}

void cpart_lblock::ReadPropertiesWindow(void) {
    input_pins[0] = GetPWCComboSelectedPin("combo1");
    input_pins[1] = GetPWCComboSelectedPin("combo2");
    input_pins[2] = GetPWCComboSelectedPin("combo3");
    input_pins[3] = GetPWCComboSelectedPin("combo4");
    input_pins[4] = GetPWCComboSelectedPin("combo5");
    input_pins[5] = GetPWCComboSelectedPin("combo6");
    input_pins[6] = GetPWCComboSelectedPin("combo7");
    input_pins[7] = GetPWCComboSelectedPin("combo8");
    output_pins[0] = GetPWCComboSelectedPin("combo9");

    char buff[64];
    SpareParts.WPropCmd("combo11", PWA_COMBOGETTEXT, NULL, buff);

    for (int i = 0; i < 8; i++) {
        if (!strcmp(buff, logicgate_name[i])) {
            gatetype = i;
            break;
        }
    }

    int size;
    SpareParts.WPropCmd("spin12", PWA_SPINGETVALUE, NULL, &size);
    ChangeSize(size);
}

void cpart_lblock::SpinChange(const char* controlname, int value) {
    for (int i = 0; i < 8; i++) {
        char name[20];
        sprintf(name, "combo%i", i + 1);
        SpareParts.WPropCmd(name, PWA_SETENABLE, std::to_string(i < value).c_str());
    }
}

void cpart_lblock::ComboChange(const char* controlname, std::string value) {
    char buff[64];
    SpareParts.WPropCmd("combo11", PWA_COMBOGETTEXT, NULL, buff);
    for (int i = 0; i < 8; i++) {
        if (!strcmp(buff, logicgate_name[i])) {
            gatetype = i;
            break;
        }
    }
    switch (gatetype) {
        case LG_NOT:
        case LG_BUFFER:
            ChangeSize(1);
            break;
        case LG_AND:
        case LG_NAND:
        case LG_OR:
        case LG_NOR:
        case LG_XOR:
        case LG_XNOR:
            ChangeSize(2);
            break;
    }
    ConfigurePropertiesWindow();
}

void cpart_lblock::ChangeSize(const unsigned int sz) {
    if (Size != sz) {
        if (BitmapId >= 0) {
            SpareParts.SetPartOnDraw(id);
            SpareParts.CanvasCmd({.cmd = CC_FREEBITMAP, .FreeBitmap{BitmapId}});
            BitmapId = -1;
        }
        Size = sz;
        if (Size > 8) {
            Size = 8;
        }
        outputc = (Size * 2) + 4;
        LoadPartImage();
    }
}

void cpart_lblock::LoadPartImage(void) {
    if (Size < 8) {
        xoff = (8 - Size) * 62;
        outputc = (Size * 2) + 4;

        Width = OWidth - xoff;
        Height = OHeight;

        SpareParts.SetPartOnDraw(id);
        if (BitmapId >= 0) {
            SpareParts.CanvasCmd({.cmd = CC_FREEBITMAP, .FreeBitmap{BitmapId}});
        }
        BitmapId = SpareParts.CanvasCmd({.cmd = CC_CREATEIMAGE, .CreateImage{Width, Height, Scale, 0, Orientation}});

        SpareParts.CanvasCmd({.cmd = CC_DESTROY});
        SpareParts.CanvasCmd({.cmd = CC_CREATE, .Create{BitmapId}});

        int BackBitmap = SpareParts.CanvasCmd(
            {.cmd = CC_LOADIMAGE,
             .LoadImage{(PICSimLab.GetSharePath() + "parts/" + Type + "/" + GetPictureFileName()).c_str(), Scale, 0,
                        Orientation}});

        SpareParts.CanvasCmd({.cmd = CC_INIT, .Init{Scale, Scale, Orientation}});
        SpareParts.CanvasCmd({.cmd = CC_SETCOLOR, .SetColor{0x31, 0x3d, 0x63}});
        SpareParts.CanvasCmd({.cmd = CC_RECTANGLE, .Rectangle{1, 0, 0, (float)Width, (float)Height}});

        SpareParts.CanvasCmd({.cmd = CC_CHANGESCALE, .ChangeScale{1.0, 1.0}});
        SpareParts.CanvasCmd({.cmd = CC_PUTBITMAP, .PutBitmap{BackBitmap, -xoff * Scale, 0}});

        SpareParts.CanvasCmd({.cmd = CC_CHANGESCALE, .ChangeScale{Scale, Scale}});
        SpareParts.CanvasCmd({.cmd = CC_END});

        SpareParts.CanvasCmd({.cmd = CC_FREEBITMAP, .FreeBitmap{BackBitmap}});

    } else {
        Width = OWidth;
        Height = OHeight;
        xoff = 0;
        part::LoadPartImage();
    }
}

part_init(PART_LOGIC_BLOCK_Name, cpart_lblock, "Other");
