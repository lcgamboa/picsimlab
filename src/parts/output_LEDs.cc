/* ########################################################################

   PICSimLab - Programmable IC Simulator Laboratory

   ########################################################################

   Copyright (c) : 2010-2026  Luis Claudio Gambôa Lopes <lcgamboa@yahoo.com>

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

#include "output_LEDs.h"
#include "../lib/oscilloscope.h"
#include "../lib/picsimlab.h"
#include "../lib/spareparts.h"

/* outputs */
enum { O_P1, O_P2, O_P3, O_P4, O_P5, O_P6, O_P7, O_P8, O_L1, O_L2, O_L3, O_L4, O_L5, O_L6, O_L7, O_L8 };

enum { C_RED = 0, C_GREEN, C_BLUE, C_YELLOW, C_WHITE, C_END };

static const char Colorname[C_END][7] = {"Red", "Green", "Blue", "Yellow", "White"};

typedef struct {
    unsigned char r, g, b;
} colorval_t;

static const colorval_t colortable[C_END] = {{1, 0, 0}, {0, 1, 0}, {0, 0, 1}, {1, 1, 0}, {1, 1, 1}};

static PCWProp pcwprop[11] = {{PCW_DCOMBO, "1-D1"},  {PCW_DCOMBO, "2-D2"}, {PCW_DCOMBO, "3-D3"}, {PCW_DCOMBO, "4-D4"},
                              {PCW_DCOMBO, "5-D5"},  {PCW_DCOMBO, "6-D6"}, {PCW_DCOMBO, "7-D7"}, {PCW_DCOMBO, "8-D8"},
                              {PCW_COMBO, "Active"}, {PCW_SPIN, "Size"},   {PCW_END, ""}};

cpart_leds::cpart_leds(const unsigned x, const unsigned y, const char* name, const char* type, board* pboard_,
                       const int id_)
    : part(x, y, name, type, pboard_, id_, 9) {
    X = x;
    Y = y;
    active = 1;
    Size = 0;
    BitmapId = -1;

    ReadMaps();
    OWidth = Width;
    OHeight = Height;

    input_pins[0] = 0;
    input_pins[1] = 0;
    input_pins[2] = 0;
    input_pins[3] = 0;
    input_pins[4] = 0;
    input_pins[5] = 0;
    input_pins[6] = 0;
    input_pins[7] = 0;

    ChangeSize(8);

    colors[0] = 0;
    colors[1] = 0;
    colors[2] = 0;
    colors[3] = 0;
    colors[4] = 0;
    colors[5] = 0;
    colors[6] = 0;
    colors[7] = 0;

    SetPCWProperties(pcwprop);

    PinCount = 8;
    Pins = input_pins;
}

cpart_leds::~cpart_leds(void) {
    SpareParts.SetPartOnDraw(id);
    SpareParts.CanvasCmd({.cmd = CC_FREEBITMAP, .FreeBitmap{BitmapId}});
    SpareParts.CanvasCmd({.cmd = CC_DESTROY});
}

void cpart_leds::DrawOutput(const unsigned int i) {
    colorval_t col;

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
        case O_L1:
        case O_L2:
        case O_L3:
        case O_L4:
        case O_L5:
        case O_L6:
        case O_L7:
        case O_L8:
            SpareParts.CanvasCmd({.cmd = CC_SETCOLOR, .SetColor{49, 61, 99}});
            SpareParts.CanvasCmd({.cmd = CC_RECTANGLE, .Rectangle{1, output[i].x1 - 20, output[i].y1 - 20, 40, 40}});
            col = colortable[colors[output[i].id - O_L1]];
            if (input_pins[output[i].id - O_L1] > 0) {
                if (active) {
                    SpareParts.CanvasCmd(
                        {.cmd = CC_SETBGCOLOR,
                         .SetBgColor{(unsigned int)(col.r * ppins[input_pins[output[i].id - O_L1] - 1].oavalue),
                                     (unsigned int)(col.g * ppins[input_pins[output[i].id - O_L1] - 1].oavalue),
                                     (unsigned int)(col.b * ppins[input_pins[output[i].id - O_L1] - 1].oavalue)}});
                } else {
                    SpareParts.CanvasCmd(
                        {.cmd = CC_SETBGCOLOR,
                         .SetBgColor{
                             (unsigned int)(col.r * (310 - ppins[input_pins[output[i].id - O_L1] - 1].oavalue)),
                             (unsigned int)(col.g * (310 - ppins[input_pins[output[i].id - O_L1] - 1].oavalue)),
                             (unsigned int)(col.b * (310 - ppins[input_pins[output[i].id - O_L1] - 1].oavalue))}});
                }
            } else {
                SpareParts.CanvasCmd(
                    {.cmd = CC_SETBGCOLOR,
                     .SetBgColor{(unsigned int)(col.r * 55), (unsigned int)(col.g * 55), (unsigned int)(col.b * 55)}});
            }
            SpareParts.CanvasCmd({.cmd = CC_SETFGCOLOR, .SetFgColor{0, 0, 0}});
            DrawLED(SpareParts.CanvasCmd, &output[i]);
            break;
    }
}

void cpart_leds::PostProcess(void) {
    const picpin* ppins = SpareParts.GetPinsValues();

    for (unsigned int i = 0; i < Size; i++) {
        if (input_pins[i] && (output_ids[O_L1 + i]->value != ppins[input_pins[i] - 1].oavalue)) {
            output_ids[O_L1 + i]->value = ppins[input_pins[i] - 1].oavalue;
            output_ids[O_L1 + i]->update = 1;
        }
    }
}

unsigned short cpart_leds::GetInputId(char* name) {
    printf("Error input '%s' don't have a valid id! \n", name);
    return INVALID_ID;
};

unsigned short cpart_leds::GetOutputId(char* name) {
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

    printf("Error output '%s' don't have a valid id! \n", name);
    return INVALID_ID;
};

std::string cpart_leds::WritePreferences(void) {
    char prefs[256];

    sprintf(prefs, "%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%u",
            input_pins[0], input_pins[1], input_pins[2], input_pins[3], input_pins[4], input_pins[5], input_pins[6],
            input_pins[7], active, colors[0], colors[1], colors[2], colors[3], colors[4], colors[5], colors[6],
            colors[7], Size);

    return prefs;
}

void cpart_leds::ReadPreferences(std::string value) {
    unsigned int sz;
    sscanf(value.c_str(), "%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%u",
           &input_pins[0], &input_pins[1], &input_pins[2], &input_pins[3], &input_pins[4], &input_pins[5],
           &input_pins[6], &input_pins[7], &active, &colors[0], &colors[1], &colors[2], &colors[3], &colors[4],
           &colors[5], &colors[6], &colors[7], &sz);

    ChangeSize(sz);
}

void cpart_leds::RegisterRemoteControl(void) {
    const picpin* ppins = SpareParts.GetPinsValues();

    for (int i = 0; i < 8; i++) {
        if (input_pins[i]) {
            output_ids[O_L1 + i]->status = (void*)&ppins[input_pins[i] - 1].oavalue;
        } else {
            output_ids[O_L1 + i]->status = NULL;
        }
    }
}

void cpart_leds::ConfigurePropertiesWindow(void) {
    std::string Colors = "";

    for (int i = 0; i < C_END; i++) {
        Colors += Colorname[i];
        Colors += ",";
    }

    SetPCWComboWithPinNames("combo1", input_pins[0]);
    SetPCWComboWithPinNames("combo2", input_pins[1]);
    SetPCWComboWithPinNames("combo3", input_pins[2]);
    SetPCWComboWithPinNames("combo4", input_pins[3]);
    SetPCWComboWithPinNames("combo5", input_pins[4]);
    SetPCWComboWithPinNames("combo6", input_pins[5]);
    SetPCWComboWithPinNames("combo7", input_pins[6]);
    SetPCWComboWithPinNames("combo8", input_pins[7]);

    SpareParts.WPropCmd("combo9", PWA_COMBOSETITEMS, "HIGH,LOW,");
    if (active)
        SpareParts.WPropCmd("combo9", PWA_COMBOSETTEXT, "HIGH");
    else
        SpareParts.WPropCmd("combo9", PWA_COMBOSETTEXT, "LOW ");

    SpareParts.WPropCmd("spin10", PWA_SPINSETMAX, "8");
    SpareParts.WPropCmd("spin10", PWA_SPINSETMIN, "1");
    SpareParts.WPropCmd("spin10", PWA_SPINSETVALUE, std::to_string(Size).c_str());
    SpareParts.WPropCmd("spin10", PWA_SPINPROPEV, "1");

    SpareParts.WPropCmd("combo_1", PWA_COMBOSETITEMS, Colors.c_str());
    SpareParts.WPropCmd("combo_1", PWA_COMBOSETTEXT, Colorname[colors[0]]);
    SpareParts.WPropCmd("combo_2", PWA_COMBOSETITEMS, Colors.c_str());
    SpareParts.WPropCmd("combo_2", PWA_COMBOSETTEXT, Colorname[colors[1]]);
    SpareParts.WPropCmd("combo_3", PWA_COMBOSETITEMS, Colors.c_str());
    SpareParts.WPropCmd("combo_3", PWA_COMBOSETTEXT, Colorname[colors[2]]);
    SpareParts.WPropCmd("combo_4", PWA_COMBOSETITEMS, Colors.c_str());
    SpareParts.WPropCmd("combo_4", PWA_COMBOSETTEXT, Colorname[colors[3]]);
    SpareParts.WPropCmd("combo_5", PWA_COMBOSETITEMS, Colors.c_str());
    SpareParts.WPropCmd("combo_5", PWA_COMBOSETTEXT, Colorname[colors[4]]);
    SpareParts.WPropCmd("combo_6", PWA_COMBOSETITEMS, Colors.c_str());
    SpareParts.WPropCmd("combo_6", PWA_COMBOSETTEXT, Colorname[colors[5]]);
    SpareParts.WPropCmd("combo_7", PWA_COMBOSETITEMS, Colors.c_str());
    SpareParts.WPropCmd("combo_7", PWA_COMBOSETTEXT, Colorname[colors[6]]);
    SpareParts.WPropCmd("combo_8", PWA_COMBOSETITEMS, Colors.c_str());
    SpareParts.WPropCmd("combo_8", PWA_COMBOSETTEXT, Colorname[colors[7]]);

    SpinChange(NULL, Size);
}

void cpart_leds::ReadPropertiesWindow(void) {
    input_pins[0] = GetPWCComboSelectedPin("combo1");
    input_pins[1] = GetPWCComboSelectedPin("combo2");
    input_pins[2] = GetPWCComboSelectedPin("combo3");
    input_pins[3] = GetPWCComboSelectedPin("combo4");
    input_pins[4] = GetPWCComboSelectedPin("combo5");
    input_pins[5] = GetPWCComboSelectedPin("combo6");
    input_pins[6] = GetPWCComboSelectedPin("combo7");
    input_pins[7] = GetPWCComboSelectedPin("combo8");

    char buff[64];
    SpareParts.WPropCmd("combo9", PWA_COMBOGETTEXT, NULL, buff);

    active = (strcmp(buff, "HIGH") == 0);

    for (int i = 0; i < 8; i++) {
        char cname[100];
        snprintf(cname, 100, "combo_%i", 1 + i);

        char buff[64];
        SpareParts.WPropCmd(cname, PWA_COMBOGETTEXT, NULL, buff);
        std::string val = buff;
        for (int j = 0; j < C_END; j++) {
            if (!val.compare(Colorname[j])) {
                colors[i] = j;
                break;
            }
        }
    }

    int size;
    SpareParts.WPropCmd("spin10", PWA_SPINGETVALUE, NULL, &size);
    ChangeSize(size);
}

void cpart_leds::SpinChange(const char* controlname, int value) {
    for (int i = 0; i < 8; i++) {
        char name[20];
        sprintf(name, "combo%i", i + 1);
        SpareParts.WPropCmd(name, PWA_SETENABLE, std::to_string(i < value).c_str());
    }
}

void cpart_leds::ChangeSize(const unsigned int sz) {
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
        outputc = Size * 2;
        LoadPartImage();
    }
    RegisterRemoteControl();
}

void cpart_leds::LoadPartImage(void) {
    if (Size < 8) {
        xoff = (8 - Size) * 62;

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

part_init(PART_LEDS_Name, cpart_leds, "Output");
