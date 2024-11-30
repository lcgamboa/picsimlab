/* ########################################################################

   PICSimLab - Programmable IC Simulator Laboratory

   ########################################################################

   Copyright (c) : 2020-2024  Luis Claudio Gambôa Lopes <lcgamboa@yahoo.com>

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

#include "output_LCD_ili9341.h"
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
    O_P11,
    O_P12,
    O_P13,
    O_F1,
    O_F2,
    O_LCD,
    O_T1,
    O_T2,
    O_T3,
    O_T4,
    O_T5
};

/* inputs */
enum { I_LCD };

/* types */
enum { TC_SPI = 0, TC_8BITS, TC_SPI_TOUCH, TC_8BITS_TOUCH };

cpart_LCD_ili9341::cpart_LCD_ili9341(const unsigned x, const unsigned y, const char* name, const char* type,
                                     board* pboard_, const int id_)
    : part(x, y, name, type, pboard_, id_) {
    X = x;
    Y = y;

    BitmapId = -1;
    type_com = -1;
    ChangeType(TC_SPI);

    lcd_ili9341_init(&lcd);
    lcd_ili9341_rst(&lcd);

    tsc_XPT2046_init(&touch, 240, 320);
    tsc_XPT2046_rst(&touch);

    input_pins[0] = 0;
    input_pins[1] = 0;
    input_pins[2] = 0;
    input_pins[3] = 0;
    input_pins[4] = 0;

    input_pins[5] = 0;
    input_pins[6] = 0;
    input_pins[7] = 0;
    input_pins[8] = 0;
    input_pins[9] = 0;
    input_pins[10] = 0;
    input_pins[11] = 0;
    input_pins[12] = 0;

    touch_pins[0] = 0;
    touch_pins[1] = 0;
    touch_pins[2] = 0;
    touch_pins[3] = 0;
    touch_pins[4] = 0;

    active[0] = 0;

    lret = -1;
    tret = -1;

    valid_lcd_pins = 0;
    valid_touch_pins = 0;

    PinCount = 13;
    Pins = input_pins;
}

std::string cpart_LCD_ili9341::GetPictureFileName(void) {
    switch (type_com) {
        case TC_SPI:
            return "LCD ili9341/LCD_ili9341.svg";
            break;
        case TC_SPI_TOUCH:
            return "LCD ili9341/LCD_ili9341_t.svg";
            break;
        case TC_8BITS:
            return "LCD ili9341/LCD_ili9341_8.svg";
            break;
        case TC_8BITS_TOUCH:
            return "LCD ili9341/LCD_ili9341_8_t.svg";
            break;
    }
    return "LCD ili9341/LCD_ili9341.svg";
}

std::string cpart_LCD_ili9341::GetMapFile(void) {
    switch (type_com) {
        case TC_SPI:
            return "LCD ili9341/LCD_ili9341.map";
            break;
        case TC_SPI_TOUCH:
            return "LCD ili9341/LCD_ili9341_t.map";
            break;
        case TC_8BITS:
            return "LCD ili9341/LCD_ili9341_8.map";
            break;
        case TC_8BITS_TOUCH:
            return "LCD ili9341/LCD_ili9341_8_t.map";
            break;
    }
    return "LCD ili9341/LCD_ili9341.map";
}

cpart_LCD_ili9341::~cpart_LCD_ili9341(void) {
    SpareParts.SetPartOnDraw(id);
    SpareParts.CanvasCmd({.cmd = CC_FREEBITMAP, .FreeBitmap{BitmapId}});
    SpareParts.CanvasCmd({.cmd = CC_DESTROY});
}

void cpart_LCD_ili9341::DrawOutput(const unsigned int i) {
    switch (output[i].id) {
        case O_P1:
        case O_P2:
        case O_P3:
        case O_P4:
        case O_P5:
        case O_P6:
        case O_P7:
        case O_P8:
        case O_P9:
        case O_P10:
        case O_P11:
        case O_P12:
        case O_P13:
            SpareParts.CanvasCmd({.cmd = CC_SETFONTSIZE, .SetFontSize{8}});
            SpareParts.CanvasCmd({.cmd = CC_SETCOLOR, .SetColor{49, 61, 99}});
            SpareParts.CanvasCmd(
                {.cmd = CC_RECTANGLE,
                 .Rectangle{1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1}});
            SpareParts.CanvasCmd({.cmd = CC_SETFGCOLOR, .SetFgColor{255, 255, 255}});
            if (input_pins[output[i].id - O_P1] == 0)
                SpareParts.CanvasCmd({.cmd = CC_ROTATEDTEXT, .RotatedText{"NC", output[i].x1, output[i].y2, 90.0}});
            else
                SpareParts.CanvasCmd({.cmd = CC_ROTATEDTEXT,
                                      .RotatedText{SpareParts.GetPinName(input_pins[output[i].id - O_P1]).c_str(),
                                                   output[i].x1, output[i].y2, 90.0}});
            break;
        case O_T1:
        case O_T2:
        case O_T3:
        case O_T4:
        case O_T5:
            SpareParts.CanvasCmd({.cmd = CC_SETCOLOR, .SetColor{49, 61, 99}});
            SpareParts.CanvasCmd(
                {.cmd = CC_RECTANGLE,
                 .Rectangle{1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1}});
            SpareParts.CanvasCmd({.cmd = CC_SETFGCOLOR, .SetFgColor{255, 255, 255}});
            if (touch_pins[output[i].id - O_T1] == 0)
                SpareParts.CanvasCmd({.cmd = CC_ROTATEDTEXT, .RotatedText{"NC", output[i].x1, output[i].y2, 90.0}});
            else
                SpareParts.CanvasCmd({.cmd = CC_ROTATEDTEXT,
                                      .RotatedText{SpareParts.GetPinName(touch_pins[output[i].id - O_T1]).c_str(),
                                                   output[i].x1, output[i].y2, 90.0}});
            break;
        case O_F2:
            SpareParts.CanvasCmd({.cmd = CC_SETCOLOR, .SetColor{49, 61, 99}});
            SpareParts.CanvasCmd(
                {.cmd = CC_RECTANGLE,
                 .Rectangle{1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1}});
            SpareParts.CanvasCmd({.cmd = CC_SETFGCOLOR, .SetFgColor{155, 155, 155}});
            SpareParts.CanvasCmd({.cmd = CC_ROTATEDTEXT, .RotatedText{"3.3V", output[i].x1, output[i].y2, 90.0}});
            break;
        case O_F1:
            SpareParts.CanvasCmd({.cmd = CC_SETCOLOR, .SetColor{49, 61, 99}});
            SpareParts.CanvasCmd(
                {.cmd = CC_RECTANGLE,
                 .Rectangle{1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1}});
            SpareParts.CanvasCmd({.cmd = CC_SETFGCOLOR, .SetFgColor{155, 155, 155}});
            SpareParts.CanvasCmd({.cmd = CC_ROTATEDTEXT, .RotatedText{"GND", output[i].x1, output[i].y2, 90.0}});
            break;
        case O_LCD:
            // draw lcd text
            if (lcd.update) {
                SpareParts.CanvasCmd({.cmd = CC_SETCOLOR, .SetColor{0, 90 + 40, 0}});
                lcd_ili9341_draw(&lcd, SpareParts.CanvasCmd, output[i].x1, output[i].y1, output[i].x2 - output[i].x1,
                                 output[i].y2 - output[i].y1, 1);
            }
            break;
    }
}

unsigned short cpart_LCD_ili9341::GetInputId(char* name) {
    if (strcmp(name, "DS_LCD") == 0)
        return I_LCD;

    printf("Error input '%s' don't have a valid id! \n", name);
    return INVALID_ID;
};

unsigned short cpart_LCD_ili9341::GetOutputId(char* name) {
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
    if (strcmp(name, "PN_11") == 0)
        return O_P11;
    if (strcmp(name, "PN_12") == 0)
        return O_P12;
    if (strcmp(name, "PN_13") == 0)
        return O_P13;

    if (strcmp(name, "PN_F1") == 0)
        return O_F1;
    if (strcmp(name, "PN_F2") == 0)
        return O_F2;

    if (strcmp(name, "DS_LCD") == 0)
        return O_LCD;

    if (strcmp(name, "PN_T1") == 0)
        return O_T1;
    if (strcmp(name, "PN_T2") == 0)
        return O_T2;
    if (strcmp(name, "PN_T3") == 0)
        return O_T3;
    if (strcmp(name, "PN_T4") == 0)
        return O_T4;
    if (strcmp(name, "PN_T5") == 0)
        return O_T5;

    printf("Error output '%s' don't have a valid id! \n", name);
    return INVALID_ID;
};

std::string cpart_LCD_ili9341::WritePreferences(void) {
    char prefs[256];

    sprintf(prefs,
            "%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%"
            "hhu,%hhu,%hhu,%hhu,%hhu",
            input_pins[0], input_pins[1], input_pins[2], input_pins[3], input_pins[4], type_com, input_pins[5],
            input_pins[6], input_pins[7], input_pins[8], input_pins[9], input_pins[10], input_pins[11], input_pins[12],
            touch_pins[0], touch_pins[1], touch_pins[2], touch_pins[3], touch_pins[4]);
    return prefs;
}

void cpart_LCD_ili9341::ReadPreferences(std::string value) {
    unsigned char tp;
    sscanf(value.c_str(),
           "%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,"
           "%hhu,%hhu,%hhu,%hhu",
           &input_pins[0], &input_pins[1], &input_pins[2], &input_pins[3], &input_pins[4], &tp, &input_pins[5],
           &input_pins[6], &input_pins[7], &input_pins[8], &input_pins[9], &input_pins[10], &input_pins[11],
           &input_pins[12], &touch_pins[0], &touch_pins[1], &touch_pins[2], &touch_pins[3], &touch_pins[4]);
    Reset();
    ChangeType(tp);
}

void cpart_LCD_ili9341::ConfigurePropertiesWindow(void) {
    std::string Items = SpareParts.GetPinsNames();
    std::string spin;
    char name[100];

    SpareParts.WPropCmd("combo1", PWA_COMBOSETITEMS, Items.c_str());
    if (input_pins[0] == 0)
        SpareParts.WPropCmd("combo1", PWA_COMBOSETTEXT, "0  NC");
    else {
        spin = SpareParts.GetPinName(input_pins[0]);
        SpareParts.WPropCmd("combo1", PWA_COMBOSETTEXT, (std::to_string(input_pins[0]) + "  " + spin).c_str());
    }

    SpareParts.WPropCmd("combo2", PWA_COMBOSETITEMS, Items.c_str());
    if (input_pins[1] == 0)
        SpareParts.WPropCmd("combo2", PWA_COMBOSETTEXT, "0  NC");
    else {
        spin = SpareParts.GetPinName(input_pins[1]);
        SpareParts.WPropCmd("combo2", PWA_COMBOSETTEXT, (std::to_string(input_pins[1]) + "  " + spin).c_str());
    }

    SpareParts.WPropCmd("combo3", PWA_COMBOSETITEMS, Items.c_str());
    if (input_pins[2] == 0)
        SpareParts.WPropCmd("combo3", PWA_COMBOSETTEXT, "0  NC");
    else {
        spin = SpareParts.GetPinName(input_pins[2]);
        SpareParts.WPropCmd("combo3", PWA_COMBOSETTEXT, (std::to_string(input_pins[2]) + "  " + spin).c_str());
    }

    SpareParts.WPropCmd("combo4", PWA_COMBOSETITEMS, Items.c_str());
    if (input_pins[3] == 0)
        SpareParts.WPropCmd("combo4", PWA_COMBOSETTEXT, "0  NC");
    else {
        spin = SpareParts.GetPinName(input_pins[3]);
        SpareParts.WPropCmd("combo4", PWA_COMBOSETTEXT, (std::to_string(input_pins[3]) + "  " + spin).c_str());
    }

    SpareParts.WPropCmd("combo5", PWA_COMBOSETITEMS, Items.c_str());
    if (input_pins[4] == 0)
        SpareParts.WPropCmd("combo5", PWA_COMBOSETTEXT, "0  NC");
    else {
        spin = SpareParts.GetPinName(input_pins[4]);
        SpareParts.WPropCmd("combo5", PWA_COMBOSETTEXT, (std::to_string(input_pins[4]) + "  " + spin).c_str());
    }

    switch (type_com) {
        case TC_SPI:

            SpareParts.WPropCmd(NULL, PWA_SETWIDTH, "389");
            SpareParts.WPropCmd(NULL, PWA_SETHEIGHT, "370");

            SpareParts.WPropCmd("button1", PWA_SETX, "104");
            SpareParts.WPropCmd("button2", PWA_SETX, "185");
            SpareParts.WPropCmd("button1", PWA_SETY, "286");
            SpareParts.WPropCmd("button2", PWA_SETY, "286");

            SpareParts.WPropCmd("combo6", PWA_COMBOSETTEXT, "SPI");

            SpareParts.WPropCmd("combo7", PWA_SETVISIBLE, "0");
            SpareParts.WPropCmd("combo8", PWA_SETVISIBLE, "0");
            SpareParts.WPropCmd("combo9", PWA_SETVISIBLE, "0");
            SpareParts.WPropCmd("combo10", PWA_SETVISIBLE, "0");
            SpareParts.WPropCmd("combo11", PWA_SETVISIBLE, "0");
            SpareParts.WPropCmd("combo12", PWA_SETVISIBLE, "0");
            SpareParts.WPropCmd("combo13", PWA_SETVISIBLE, "0");
            SpareParts.WPropCmd("combo14", PWA_SETVISIBLE, "0");

            SpareParts.WPropCmd("combo15", PWA_SETVISIBLE, "0");
            SpareParts.WPropCmd("combo16", PWA_SETVISIBLE, "0");
            SpareParts.WPropCmd("combo17", PWA_SETVISIBLE, "0");
            SpareParts.WPropCmd("combo18", PWA_SETVISIBLE, "0");
            SpareParts.WPropCmd("combo19", PWA_SETVISIBLE, "0");

            SpareParts.WPropCmd("label9", PWA_SETVISIBLE, "0");
            SpareParts.WPropCmd("label10", PWA_SETVISIBLE, "0");
            SpareParts.WPropCmd("label11", PWA_SETVISIBLE, "0");
            SpareParts.WPropCmd("label12", PWA_SETVISIBLE, "0");
            SpareParts.WPropCmd("label13", PWA_SETVISIBLE, "0");
            SpareParts.WPropCmd("label14", PWA_SETVISIBLE, "0");
            SpareParts.WPropCmd("label15", PWA_SETVISIBLE, "0");
            SpareParts.WPropCmd("label16", PWA_SETVISIBLE, "0");

            SpareParts.WPropCmd("label17", PWA_SETVISIBLE, "0");
            SpareParts.WPropCmd("label18", PWA_SETVISIBLE, "0");
            SpareParts.WPropCmd("label19", PWA_SETVISIBLE, "0");
            SpareParts.WPropCmd("label20", PWA_SETVISIBLE, "0");
            SpareParts.WPropCmd("label21", PWA_SETVISIBLE, "0");

            SpareParts.WPropCmd("label5", PWA_LABELSETTEXT, "Pin 3 - CLK");
            SpareParts.WPropCmd("label4", PWA_LABELSETTEXT, "Pin 4 - DIN");
            break;
        case TC_8BITS:

            SpareParts.WPropCmd(NULL, PWA_SETWIDTH, "740");
            SpareParts.WPropCmd(NULL, PWA_SETHEIGHT, "370");

            SpareParts.WPropCmd("button1", PWA_SETX, "154");
            SpareParts.WPropCmd("button2", PWA_SETX, "235");
            SpareParts.WPropCmd("button1", PWA_SETY, "286");
            SpareParts.WPropCmd("button2", PWA_SETY, "286");

            SpareParts.WPropCmd("combo6", PWA_COMBOSETTEXT, "8Bits");

            SpareParts.WPropCmd("combo7", PWA_SETVISIBLE, "1");
            SpareParts.WPropCmd("combo8", PWA_SETVISIBLE, "1");
            SpareParts.WPropCmd("combo9", PWA_SETVISIBLE, "1");
            SpareParts.WPropCmd("combo10", PWA_SETVISIBLE, "1");
            SpareParts.WPropCmd("combo11", PWA_SETVISIBLE, "1");
            SpareParts.WPropCmd("combo12", PWA_SETVISIBLE, "1");
            SpareParts.WPropCmd("combo13", PWA_SETVISIBLE, "1");
            SpareParts.WPropCmd("combo14", PWA_SETVISIBLE, "1");

            SpareParts.WPropCmd("combo15", PWA_SETVISIBLE, "0");
            SpareParts.WPropCmd("combo16", PWA_SETVISIBLE, "0");
            SpareParts.WPropCmd("combo17", PWA_SETVISIBLE, "0");
            SpareParts.WPropCmd("combo18", PWA_SETVISIBLE, "0");
            SpareParts.WPropCmd("combo19", PWA_SETVISIBLE, "0");

            SpareParts.WPropCmd("label9", PWA_SETVISIBLE, "1");
            SpareParts.WPropCmd("label10", PWA_SETVISIBLE, "1");
            SpareParts.WPropCmd("label11", PWA_SETVISIBLE, "1");
            SpareParts.WPropCmd("label12", PWA_SETVISIBLE, "1");
            SpareParts.WPropCmd("label13", PWA_SETVISIBLE, "1");
            SpareParts.WPropCmd("label14", PWA_SETVISIBLE, "1");
            SpareParts.WPropCmd("label15", PWA_SETVISIBLE, "1");
            SpareParts.WPropCmd("label16", PWA_SETVISIBLE, "1");

            SpareParts.WPropCmd("label17", PWA_SETVISIBLE, "0");
            SpareParts.WPropCmd("label18", PWA_SETVISIBLE, "0");
            SpareParts.WPropCmd("label19", PWA_SETVISIBLE, "0");
            SpareParts.WPropCmd("label20", PWA_SETVISIBLE, "0");
            SpareParts.WPropCmd("label21", PWA_SETVISIBLE, "0");

            SpareParts.WPropCmd("label5", PWA_LABELSETTEXT, "Pin 3 - /WR");
            SpareParts.WPropCmd("label4", PWA_LABELSETTEXT, "Pin 4 - /RD");

            for (int i = 0; i < 8; i++) {
                snprintf(name, 100, "combo%i", i + 7);
                SpareParts.WPropCmd(name, PWA_COMBOSETITEMS, Items.c_str());
                if (input_pins[5 + i] == 0)
                    SpareParts.WPropCmd(name, PWA_COMBOSETTEXT, "0  NC");
                else {
                    spin = SpareParts.GetPinName(input_pins[5 + i]);
                    SpareParts.WPropCmd(name, PWA_COMBOSETTEXT,
                                        (std::to_string(input_pins[5 + i]) + "  " + spin).c_str());
                }
            }
            break;
        case TC_SPI_TOUCH:

            SpareParts.WPropCmd(NULL, PWA_SETWIDTH, "389");
            SpareParts.WPropCmd(NULL, PWA_SETHEIGHT, "570");

            SpareParts.WPropCmd("button1", PWA_SETX, "104");
            SpareParts.WPropCmd("button2", PWA_SETX, "185");
            SpareParts.WPropCmd("button1", PWA_SETY, "486");
            SpareParts.WPropCmd("button2", PWA_SETY, "486");

            SpareParts.WPropCmd("combo6", PWA_COMBOSETTEXT, "SPI+Touch");

            SpareParts.WPropCmd("combo7", PWA_SETVISIBLE, "0");
            SpareParts.WPropCmd("combo8", PWA_SETVISIBLE, "0");
            SpareParts.WPropCmd("combo9", PWA_SETVISIBLE, "0");
            SpareParts.WPropCmd("combo10", PWA_SETVISIBLE, "0");
            SpareParts.WPropCmd("combo11", PWA_SETVISIBLE, "0");
            SpareParts.WPropCmd("combo12", PWA_SETVISIBLE, "0");
            SpareParts.WPropCmd("combo13", PWA_SETVISIBLE, "0");
            SpareParts.WPropCmd("combo14", PWA_SETVISIBLE, "0");

            SpareParts.WPropCmd("combo15", PWA_SETVISIBLE, "1");
            SpareParts.WPropCmd("combo16", PWA_SETVISIBLE, "1");
            SpareParts.WPropCmd("combo17", PWA_SETVISIBLE, "1");
            SpareParts.WPropCmd("combo18", PWA_SETVISIBLE, "1");
            SpareParts.WPropCmd("combo19", PWA_SETVISIBLE, "1");

            SpareParts.WPropCmd("label9", PWA_SETVISIBLE, "0");
            SpareParts.WPropCmd("label10", PWA_SETVISIBLE, "0");
            SpareParts.WPropCmd("label11", PWA_SETVISIBLE, "0");
            SpareParts.WPropCmd("label12", PWA_SETVISIBLE, "0");
            SpareParts.WPropCmd("label13", PWA_SETVISIBLE, "0");
            SpareParts.WPropCmd("label14", PWA_SETVISIBLE, "0");
            SpareParts.WPropCmd("label15", PWA_SETVISIBLE, "0");
            SpareParts.WPropCmd("label16", PWA_SETVISIBLE, "0");

            SpareParts.WPropCmd("label17", PWA_SETVISIBLE, "1");
            SpareParts.WPropCmd("label18", PWA_SETVISIBLE, "1");
            SpareParts.WPropCmd("label19", PWA_SETVISIBLE, "1");
            SpareParts.WPropCmd("label20", PWA_SETVISIBLE, "1");
            SpareParts.WPropCmd("label21", PWA_SETVISIBLE, "1");

            SpareParts.WPropCmd("label5", PWA_LABELSETTEXT, "Pin 3 - CLK");
            SpareParts.WPropCmd("label4", PWA_LABELSETTEXT, "Pin 4 - DIN");

            for (int i = 0; i < 5; i++) {
                snprintf(name, 100, "combo%i", i + 15);
                SpareParts.WPropCmd(name, PWA_COMBOSETITEMS, Items.c_str());
                if (touch_pins[i] == 0)
                    SpareParts.WPropCmd(name, PWA_COMBOSETTEXT, "0  NC");
                else {
                    spin = SpareParts.GetPinName(touch_pins[i]);
                    SpareParts.WPropCmd(name, PWA_COMBOSETTEXT, (std::to_string(touch_pins[i]) + "  " + spin).c_str());
                }
            }

            break;
        case TC_8BITS_TOUCH:

            SpareParts.WPropCmd(NULL, PWA_SETWIDTH, "740");
            SpareParts.WPropCmd(NULL, PWA_SETHEIGHT, "570");

            SpareParts.WPropCmd("button1", PWA_SETX, "154");
            SpareParts.WPropCmd("button2", PWA_SETX, "235");
            SpareParts.WPropCmd("button1", PWA_SETY, "486");
            SpareParts.WPropCmd("button2", PWA_SETY, "486");

            SpareParts.WPropCmd("combo6", PWA_COMBOSETTEXT, "8Bits+Touch");

            SpareParts.WPropCmd("combo7", PWA_SETVISIBLE, "1");
            SpareParts.WPropCmd("combo8", PWA_SETVISIBLE, "1");
            SpareParts.WPropCmd("combo9", PWA_SETVISIBLE, "1");
            SpareParts.WPropCmd("combo10", PWA_SETVISIBLE, "1");
            SpareParts.WPropCmd("combo11", PWA_SETVISIBLE, "1");
            SpareParts.WPropCmd("combo12", PWA_SETVISIBLE, "1");
            SpareParts.WPropCmd("combo13", PWA_SETVISIBLE, "1");
            SpareParts.WPropCmd("combo14", PWA_SETVISIBLE, "1");

            SpareParts.WPropCmd("combo15", PWA_SETVISIBLE, "1");
            SpareParts.WPropCmd("combo16", PWA_SETVISIBLE, "1");
            SpareParts.WPropCmd("combo17", PWA_SETVISIBLE, "1");
            SpareParts.WPropCmd("combo18", PWA_SETVISIBLE, "1");
            SpareParts.WPropCmd("combo19", PWA_SETVISIBLE, "1");

            SpareParts.WPropCmd("label9", PWA_SETVISIBLE, "1");
            SpareParts.WPropCmd("label10", PWA_SETVISIBLE, "1");
            SpareParts.WPropCmd("label11", PWA_SETVISIBLE, "1");
            SpareParts.WPropCmd("label12", PWA_SETVISIBLE, "1");
            SpareParts.WPropCmd("label13", PWA_SETVISIBLE, "1");
            SpareParts.WPropCmd("label14", PWA_SETVISIBLE, "1");
            SpareParts.WPropCmd("label15", PWA_SETVISIBLE, "1");
            SpareParts.WPropCmd("label16", PWA_SETVISIBLE, "1");

            SpareParts.WPropCmd("label17", PWA_SETVISIBLE, "1");
            SpareParts.WPropCmd("label18", PWA_SETVISIBLE, "1");
            SpareParts.WPropCmd("label19", PWA_SETVISIBLE, "1");
            SpareParts.WPropCmd("label20", PWA_SETVISIBLE, "1");
            SpareParts.WPropCmd("label21", PWA_SETVISIBLE, "1");

            SpareParts.WPropCmd("label5", PWA_LABELSETTEXT, "Pin 3 - /WR");
            SpareParts.WPropCmd("label4", PWA_LABELSETTEXT, "Pin 4 - /RD");

            for (int i = 0; i < 8; i++) {
                snprintf(name, 100, "combo%i", i + 7);
                SpareParts.WPropCmd(name, PWA_COMBOSETITEMS, Items.c_str());
                if (input_pins[5 + i] == 0)
                    SpareParts.WPropCmd(name, PWA_COMBOSETTEXT, "0  NC");
                else {
                    spin = SpareParts.GetPinName(input_pins[5 + i]);
                    SpareParts.WPropCmd(name, PWA_COMBOSETTEXT,
                                        (std::to_string(input_pins[5 + i]) + "  " + spin).c_str());
                }
            }

            for (int i = 0; i < 5; i++) {
                snprintf(name, 100, "combo%i", i + 15);
                SpareParts.WPropCmd(name, PWA_COMBOSETITEMS, Items.c_str());
                if (touch_pins[i] == 0)
                    SpareParts.WPropCmd(name, PWA_COMBOSETTEXT, "0  NC");
                else {
                    spin = SpareParts.GetPinName(touch_pins[i]);
                    SpareParts.WPropCmd(name, PWA_COMBOSETTEXT, (std::to_string(touch_pins[i]) + "  " + spin).c_str());
                }
            }
            break;
    }

    SpareParts.WPropCmd("combo6", PWA_COMBOPROPEV, "1");
}

void cpart_LCD_ili9341::ReadPropertiesWindow(void) {
    char buff[64];
    SpareParts.WPropCmd("combo1", PWA_COMBOGETTEXT, NULL, buff);
    input_pins[0] = std::stoi(buff);
    SpareParts.WPropCmd("combo2", PWA_COMBOGETTEXT, NULL, buff);
    input_pins[1] = std::stoi(buff);
    SpareParts.WPropCmd("combo3", PWA_COMBOGETTEXT, NULL, buff);
    input_pins[2] = std::stoi(buff);
    SpareParts.WPropCmd("combo4", PWA_COMBOGETTEXT, NULL, buff);
    input_pins[3] = std::stoi(buff);
    SpareParts.WPropCmd("combo5", PWA_COMBOGETTEXT, NULL, buff);
    input_pins[4] = std::stoi(buff);

    if ((type_com == TC_8BITS) || (type_com == TC_8BITS_TOUCH)) {
        SpareParts.WPropCmd("combo7", PWA_COMBOGETTEXT, NULL, buff);
        input_pins[5] = std::stoi(buff);
        SpareParts.WPropCmd("combo8", PWA_COMBOGETTEXT, NULL, buff);
        input_pins[6] = std::stoi(buff);
        SpareParts.WPropCmd("combo9", PWA_COMBOGETTEXT, NULL, buff);
        input_pins[7] = std::stoi(buff);
        SpareParts.WPropCmd("combo10", PWA_COMBOGETTEXT, NULL, buff);
        input_pins[8] = std::stoi(buff);
        SpareParts.WPropCmd("combo11", PWA_COMBOGETTEXT, NULL, buff);
        input_pins[9] = std::stoi(buff);
        SpareParts.WPropCmd("combo12", PWA_COMBOGETTEXT, NULL, buff);
        input_pins[10] = std::stoi(buff);
        SpareParts.WPropCmd("combo13", PWA_COMBOGETTEXT, NULL, buff);
        input_pins[11] = std::stoi(buff);
        SpareParts.WPropCmd("combo14", PWA_COMBOGETTEXT, NULL, buff);
        input_pins[12] = std::stoi(buff);
    }

    if ((type_com == TC_8BITS_TOUCH) || (type_com == TC_SPI_TOUCH)) {
        SpareParts.WPropCmd("combo15", PWA_COMBOGETTEXT, NULL, buff);
        touch_pins[0] = std::stoi(buff);
        SpareParts.WPropCmd("combo16", PWA_COMBOGETTEXT, NULL, buff);
        touch_pins[1] = std::stoi(buff);
        SpareParts.WPropCmd("combo17", PWA_COMBOGETTEXT, NULL, buff);
        touch_pins[2] = std::stoi(buff);
        SpareParts.WPropCmd("combo18", PWA_COMBOGETTEXT, NULL, buff);
        touch_pins[3] = std::stoi(buff);
        SpareParts.WPropCmd("combo19", PWA_COMBOGETTEXT, NULL, buff);
        touch_pins[4] = std::stoi(buff);
    }
}

void cpart_LCD_ili9341::PreProcess(void) {
    const picpin* ppins = SpareParts.GetPinsValues();

    valid_lcd_pins = 0;
    valid_touch_pins = 0;

    switch (type_com) {
        case TC_SPI:
        case TC_SPI_TOUCH:
            if ((input_pins[0] > 0) && (input_pins[1] > 0) && (input_pins[2] > 0) && (input_pins[3] > 0) &&
                (input_pins[4] > 0)) {
                pins_value[0] = &ppins[input_pins[0] - 1].value;
                pins_value[1] = &ppins[input_pins[1] - 1].value;
                pins_value[2] = &ppins[input_pins[4] - 1].value;
                pins_value[3] = &ppins[input_pins[3] - 1].value;
                pins_value[4] = &ppins[input_pins[2] - 1].value;
                valid_lcd_pins = 1;
            }
            break;
        case TC_8BITS:
        case TC_8BITS_TOUCH:
            if ((input_pins[0] > 0) && (input_pins[1] > 0) && (input_pins[2] > 0) && (input_pins[3] > 0) &&
                (input_pins[4] > 0) && (input_pins[5] > 0) && (input_pins[6] > 0) && (input_pins[7] > 0) &&
                (input_pins[8] > 0) && (input_pins[9] > 0) && (input_pins[10] > 0) && (input_pins[11] > 0) &&
                (input_pins[12] > 0)) {
                for (int i = 0; i < 13; i++) {
                    pins_value[i] = &ppins[input_pins[i] - 1].value;
                }
                valid_lcd_pins = 1;
            }
            break;
    }

    // Touch constroller
    switch (type_com) {
        case TC_SPI_TOUCH:
        case TC_8BITS_TOUCH:
            if ((touch_pins[0] > 0) && (touch_pins[1] > 0) && (touch_pins[2] > 0) && (touch_pins[3] > 0)) {
                tpins_value[0] = &ppins[touch_pins[0] - 1].value;
                tpins_value[1] = &ppins[touch_pins[2] - 1].value;
                tpins_value[2] = &ppins[touch_pins[1] - 1].value;

                valid_touch_pins = 1;
            }
            if (touch_pins[4] > 0) {
                tpins_int_value = &ppins[touch_pins[4] - 1].value;
            } else {
                tpins_int_value = NULL;
            }
            break;
    }
}

void cpart_LCD_ili9341::Process(void) {
    unsigned short ret;

    switch (type_com) {
        case TC_SPI:
        case TC_SPI_TOUCH:
            if (valid_lcd_pins) {
                lcd_ili9341_SPI_io(&lcd, pins_value);
            }
            break;
        case TC_8BITS:
        case TC_8BITS_TOUCH:
            if (valid_lcd_pins) {
                ret = lcd_ili9341_8_io(&lcd, pins_value);
                if ((ret != lret) && (ret & 0x0100)) {
                    SpareParts.SetPin(input_pins[5], (ret & 0x01));
                    SpareParts.SetPin(input_pins[6], (ret & 0x02) >> 1);
                    SpareParts.SetPin(input_pins[7], (ret & 0x04) >> 2);
                    SpareParts.SetPin(input_pins[8], (ret & 0x08) >> 3);
                    SpareParts.SetPin(input_pins[9], (ret & 0x10) >> 4);
                    SpareParts.SetPin(input_pins[10], (ret & 0x20) >> 5);
                    SpareParts.SetPin(input_pins[11], (ret & 0x40) >> 6);
                    SpareParts.SetPin(input_pins[12], (ret & 0x80) >> 7);
                    lret = ret;
                }
            }
            break;
    }

    // Touch constroller
    if (valid_touch_pins) {
        ret = tsc_XPT2046_SPI_io(&touch, tpins_value);

        if (ret != tret) {
            SpareParts.SetPin(touch_pins[3], (ret & 0x01));
            tret = ret;
        }
        if (tpins_int_value) {
            const unsigned char value = (ret & 0x02) >> 1;
            if (*tpins_int_value != value) {
                SpareParts.SetPin(touch_pins[4], value);
            }
        }
    }
}

void cpart_LCD_ili9341::PostProcess(void) {
    if (lcd.update)
        output_ids[O_LCD]->update = 1;
}

void cpart_LCD_ili9341::ComboChange(const char* controlname, std::string value) {
    if (!value.compare("SPI")) {
        ChangeType(TC_SPI);
    } else if (!value.compare("8Bits")) {
        ChangeType(TC_8BITS);
    } else if (!value.compare("SPI+Touch")) {
        ChangeType(TC_SPI_TOUCH);
    } else if (!value.compare("8Bits+Touch")) {
        ChangeType(TC_8BITS_TOUCH);
    }
}

void cpart_LCD_ili9341::ChangeType(unsigned char tp) {
    if (tp > 3)
        tp = TC_SPI;

    // if same
    if (tp == type_com)
        return;

    if (BitmapId >= 0) {
        SpareParts.SetPartOnDraw(id);
        SpareParts.CanvasCmd({.cmd = CC_FREEBITMAP, .FreeBitmap{BitmapId}});
        BitmapId = -1;
    }

    type_com = tp;

    ReadMaps();

    LoadPartImage();
}

void cpart_LCD_ili9341::OnMouseButtonPress(unsigned int inputId, unsigned int button, unsigned int x, unsigned int y,
                                           unsigned int state) {
    switch (inputId) {
        case I_LCD:
            tsc_XPT2046_set_pos(&touch, x - input_ids[I_LCD]->x1, y - input_ids[I_LCD]->y1);
            active[0] = 1;
            break;
    }
}

void cpart_LCD_ili9341::OnMouseButtonRelease(unsigned int inputId, unsigned int button, unsigned int x, unsigned int y,
                                             unsigned int state) {
    switch (inputId) {
        case I_LCD:
            tsc_XPT2046_set_pos(&touch, -1, -1);
            active[0] = 0;
            break;
    }
}

void cpart_LCD_ili9341::OnMouseMove(unsigned int inputId, unsigned int button, unsigned int x, unsigned int y,
                                    unsigned int state) {
    switch (inputId) {
        case I_LCD:
            if (active[0]) {
                tsc_XPT2046_set_pos(&touch, x - input_ids[I_LCD]->x1, y - input_ids[I_LCD]->y1);
            }
            break;
        default:
            active[0] = 0;
            break;
    }
}

void cpart_LCD_ili9341::Reset(void) {
    lcd_ili9341_rst(&lcd);
    tsc_XPT2046_rst(&touch);
}

void cpart_LCD_ili9341::LoadPartImage(void) {
    SpareParts.SetPartOnDraw(id);
    part::LoadPartImage();
    lcd_ili9341_update(&lcd);
}

part_init(PART_LCD_iLI9341_Name, cpart_LCD_ili9341, "Output");
