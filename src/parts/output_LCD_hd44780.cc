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

#include "output_LCD_hd44780.h"
#include "../lib/oscilloscope.h"
#include "../lib/picsimlab.h"
#include "../lib/spareparts.h"
#include "other_IO_PCF8574.h"

/* outputs */
enum { O_P1, O_P2, O_P3, O_P4, O_P5, O_P6, O_P7, O_P8, O_P9, O_P10, O_P11, O_F1, O_F2, O_F3, O_LCD };

static PCWProp pcwprop[16] = {
    {PCW_LABEL, "1-Vss,GND"}, {PCW_LABEL, "2-Vcc,+5V"}, {PCW_LABEL, "3-Vee,POT"}, {PCW_COMBO, "4-RS"},
    {PCW_COMBO, "5-RW"},      {PCW_COMBO, "6-EN"},      {PCW_COMBO, "7-D0"},      {PCW_COMBO, "8-D1"},
    {PCW_COMBO, "9-D2"},      {PCW_COMBO, "10-D3"},     {PCW_COMBO, "11-D4"},     {PCW_COMBO, "12-D5"},
    {PCW_COMBO, "13-D6"},     {PCW_COMBO, "14-D7"},     {PCW_COMBO, "Model"},     {PCW_END, ""}};

void cpart_LCD_hd44780::Reset(void) {
    LoadPartImage();

    switch (model) {
        case LCD16x2:
            lcd_end(&lcd);
            lcd_init(&lcd, 16, 2, pboard);
            break;
        case LCD16x4:
            lcd_end(&lcd);
            lcd_init(&lcd, 16, 4, pboard);
            break;
        case LCD20x2:
            lcd_end(&lcd);
            lcd_init(&lcd, 20, 2, pboard);
            break;
        case LCD20x4:
            lcd_end(&lcd);
            lcd_init(&lcd, 20, 4, pboard);
            break;
    }
    lcd_rst(&lcd);
    lcde = 0;

    SetPCWProperties(pcwprop);
}

cpart_LCD_hd44780::cpart_LCD_hd44780(const unsigned x, const unsigned y, const char* name, const char* type,
                                     board* pboard_, const int id_)
    : part(x, y, name, type, pboard_, id_, 8) {
    X = x;
    Y = y;
    ReadMaps();
    BitmapId = -1;

    model = LCD16x2;

    lcd_init(&lcd, 16, 2, pboard);

    Reset();

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

    lcde = 0;

    PinCount = 11;
    Pins = input_pins;
}

cpart_LCD_hd44780::~cpart_LCD_hd44780(void) {
    SpareParts.SetPartOnDraw(id);
    SpareParts.CanvasCmd({.cmd = CC_FREEBITMAP, .FreeBitmap{BitmapId}});
    SpareParts.CanvasCmd({.cmd = CC_DESTROY});
    lcd_end(&lcd);
}

void cpart_LCD_hd44780::DrawOutput(const unsigned int i) {
    int yoff = 0;

    if ((model == LCD16x4) || (model == LCD20x4))
        yoff = 96;

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
            SpareParts.CanvasCmd({.cmd = CC_SETCOLOR, .SetColor{49, 61, 99}});
            SpareParts.CanvasCmd({.cmd = CC_RECTANGLE,
                                  .Rectangle{1, output[i].x1, output[i].y1 + yoff, output[i].x2 - output[i].x1,
                                             output[i].y2 - output[i].y1}});
            SpareParts.CanvasCmd({.cmd = CC_SETFGCOLOR, .SetFgColor{255, 255, 255}});
            if (input_pins[output[i].id - O_P1] == 0)
                SpareParts.CanvasCmd(
                    {.cmd = CC_ROTATEDTEXT, .RotatedText{"NC", output[i].x1, output[i].y2 + yoff, 90.0}});
            else
                SpareParts.CanvasCmd({.cmd = CC_ROTATEDTEXT,
                                      .RotatedText{SpareParts.GetPinName(input_pins[output[i].id - O_P1]).c_str(),
                                                   output[i].x1, output[i].y2 + yoff, 90.0}});
            break;
        case O_F1:
            SpareParts.CanvasCmd({.cmd = CC_SETCOLOR, .SetColor{49, 61, 99}});
            SpareParts.CanvasCmd({.cmd = CC_RECTANGLE,
                                  .Rectangle{1, output[i].x1, output[i].y1 + yoff, output[i].x2 - output[i].x1,
                                             output[i].y2 - output[i].y1}});
            SpareParts.CanvasCmd({.cmd = CC_SETFGCOLOR, .SetFgColor{155, 155, 155}});
            SpareParts.CanvasCmd({.cmd = CC_ROTATEDTEXT, .RotatedText{"GND", output[i].x1, output[i].y2 + yoff, 90.0}});
            break;
        case O_F2:
            SpareParts.CanvasCmd({.cmd = CC_SETCOLOR, .SetColor{49, 61, 99}});
            SpareParts.CanvasCmd({.cmd = CC_RECTANGLE,
                                  .Rectangle{1, output[i].x1, output[i].y1 + yoff, output[i].x2 - output[i].x1,
                                             output[i].y2 - output[i].y1}});
            SpareParts.CanvasCmd({.cmd = CC_SETFGCOLOR, .SetFgColor{155, 155, 155}});
            SpareParts.CanvasCmd({.cmd = CC_ROTATEDTEXT, .RotatedText{"+5V", output[i].x1, output[i].y2 + yoff, 90.0}});
            break;
        case O_F3:
            SpareParts.CanvasCmd({.cmd = CC_SETCOLOR, .SetColor{49, 61, 99}});
            SpareParts.CanvasCmd({.cmd = CC_RECTANGLE,
                                  .Rectangle{1, output[i].x1, output[i].y1 + yoff, output[i].x2 - output[i].x1,
                                             output[i].y2 - output[i].y1}});
            SpareParts.CanvasCmd({.cmd = CC_SETFGCOLOR, .SetFgColor{155, 155, 155}});
            SpareParts.CanvasCmd({.cmd = CC_ROTATEDTEXT, .RotatedText{"POT", output[i].x1, output[i].y2 + yoff, 90.0}});
            break;
        case O_LCD:
            // draw lcd text
            if (lcd.update) {
                SpareParts.CanvasCmd({.cmd = CC_SETCOLOR, .SetColor{0, 90 + 40, 0}});
                lcd_draw(&lcd, SpareParts.CanvasCmd, output[i].x1, output[i].y1, output[i].x2 - output[i].x1,
                         output[i].y2 - output[i].y1, 1);
            }
            /*
            else
            {
               SpareParts.CanvasCmd({.cmd = CC_RECTANGLE, .Rectangle{11, output[i].x1, output[i].y1,
            output[i].x2-output[i].x1,output[i].y2-output[i].y1 );
            }
             */
            break;
    }
}

unsigned short cpart_LCD_hd44780::GetInputId(char* name) {
    printf("Error input '%s' don't have a valid id! \n", name);
    return INVALID_ID;
};

unsigned short cpart_LCD_hd44780::GetOutputId(char* name) {
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

    if (strcmp(name, "PN_F1") == 0)
        return O_F1;
    if (strcmp(name, "PN_F2") == 0)
        return O_F2;
    if (strcmp(name, "PN_F3") == 0)
        return O_F3;

    if (strcmp(name, "DS_LCD") == 0)
        return O_LCD;

    printf("Error output '%s' don't have a valid id! \n", name);
    return INVALID_ID;
};

std::string cpart_LCD_hd44780::WritePreferences(void) {
    char prefs[256];

    sprintf(prefs, "%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu", input_pins[0], input_pins[1],
            input_pins[2], input_pins[3], input_pins[4], input_pins[5], input_pins[6], input_pins[7], input_pins[8],
            input_pins[9], input_pins[10], model);

    return prefs;
}

void cpart_LCD_hd44780::ReadPreferences(std::string value) {
    sscanf(value.c_str(), "%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu", &input_pins[0], &input_pins[1],
           &input_pins[2], &input_pins[3], &input_pins[4], &input_pins[5], &input_pins[6], &input_pins[7],
           &input_pins[8], &input_pins[9], &input_pins[10], &model);
    Reset();
}

void cpart_LCD_hd44780::RegisterRemoteControl(void) {
    output_ids[O_LCD]->status = (void*)&lcd;
}

void cpart_LCD_hd44780::ConfigurePropertiesWindow(void) {
    SetPCWComboWithPinNames("combo4", input_pins[0]);
    SetPCWComboWithPinNames("combo6", input_pins[1]);
    SetPCWComboWithPinNames("combo7", input_pins[2]);
    SetPCWComboWithPinNames("combo8", input_pins[3]);
    SetPCWComboWithPinNames("combo9", input_pins[4]);
    SetPCWComboWithPinNames("combo10", input_pins[5]);
    SetPCWComboWithPinNames("combo11", input_pins[6]);
    SetPCWComboWithPinNames("combo12", input_pins[7]);
    SetPCWComboWithPinNames("combo13", input_pins[8]);
    SetPCWComboWithPinNames("combo14", input_pins[9]);
    SetPCWComboWithPinNames("combo5", input_pins[10]);

    SpareParts.WPropCmd("combo15", PWA_COMBOSETITEMS, "16x2,16x4,20x2,20x4,");
    switch (model) {
        case LCD16x2:
            SpareParts.WPropCmd("combo15", PWA_COMBOSETTEXT, "16x2");
            break;
        case LCD16x4:
            SpareParts.WPropCmd("combo15", PWA_COMBOSETTEXT, "16x4");
            break;
        case LCD20x2:
            SpareParts.WPropCmd("combo15", PWA_COMBOSETTEXT, "20x2");
            break;
        case LCD20x4:
            SpareParts.WPropCmd("combo15", PWA_COMBOSETTEXT, "20x4");
            break;
    }
}

void cpart_LCD_hd44780::ReadPropertiesWindow(void) {
    input_pins[0] = GetPWCComboSelectedPin("combo4");
    input_pins[1] = GetPWCComboSelectedPin("combo6");
    input_pins[2] = GetPWCComboSelectedPin("combo7");
    input_pins[3] = GetPWCComboSelectedPin("combo8");
    input_pins[4] = GetPWCComboSelectedPin("combo9");
    input_pins[5] = GetPWCComboSelectedPin("combo10");
    input_pins[6] = GetPWCComboSelectedPin("combo11");
    input_pins[7] = GetPWCComboSelectedPin("combo12");
    input_pins[8] = GetPWCComboSelectedPin("combo13");
    input_pins[9] = GetPWCComboSelectedPin("combo14");
    input_pins[10] = GetPWCComboSelectedPin("combo5");

    char buff[64];
    SpareParts.WPropCmd("combo15", PWA_COMBOGETTEXT, NULL, buff);

    model = LCD16x2;
    if (!strcmp(buff, "16x4"))
        model = LCD16x4;
    else if (!strcmp(buff, "20x2"))
        model = LCD20x2;
    else if (!strcmp(buff, "20x4"))
        model = LCD20x4;

    Reset();
}

void cpart_LCD_hd44780::Process(void) {
    const picpin* ppins = SpareParts.GetPinsValues();

    if (input_pins[1] > 0)  // EN
    {
        if (input_pins[10] > 0)  // R/W
        {
            if (!ppins[input_pins[10] - 1].value) {
                // Write
                if (lcde && (!ppins[input_pins[1] - 1].value)) {
                    // EN falling edge
                    unsigned char d = 0;
                    if ((input_pins[9] > 0) && (ppins[input_pins[9] - 1].value))
                        d |= 0x80;
                    if ((input_pins[8] > 0) && (ppins[input_pins[8] - 1].value))
                        d |= 0x40;
                    if ((input_pins[7] > 0) && (ppins[input_pins[7] - 1].value))
                        d |= 0x20;
                    if ((input_pins[6] > 0) && (ppins[input_pins[6] - 1].value))
                        d |= 0x10;
                    if ((input_pins[5] > 0) && (ppins[input_pins[5] - 1].value))
                        d |= 0x08;
                    if ((input_pins[4] > 0) && (ppins[input_pins[4] - 1].value))
                        d |= 0x04;
                    if ((input_pins[3] > 0) && (ppins[input_pins[3] - 1].value))
                        d |= 0x02;
                    if ((input_pins[2] > 0) && (ppins[input_pins[2] - 1].value))
                        d |= 0x01;

                    if (!ppins[input_pins[0] - 1].value) {
                        lcd_cmd(&lcd, d);
                    } else {
                        lcd_data(&lcd, d);
                    }

                    if (lcd.update)
                        output_ids[O_LCD]->update = 1;
                }
            } else {
                // Read
                if (!lcde && (ppins[input_pins[1] - 1].value)) {
                    // EN rising edge
                    unsigned char val = 0;
                    if (!ppins[input_pins[0] - 1].value) {
                        val = lcd_read_busyf_acounter(&lcd);
                    } else {
                        val = lcd_read_data(&lcd);
                    }

                    if (input_pins[9] > 0)
                        SpareParts.SetPin(input_pins[9], (val & 0x80) > 0);
                    if (input_pins[8] > 0)
                        SpareParts.SetPin(input_pins[8], (val & 0x40) > 0);
                    if (input_pins[7] > 0)
                        SpareParts.SetPin(input_pins[7], (val & 0x20) > 0);
                    if (input_pins[6] > 0)
                        SpareParts.SetPin(input_pins[6], (val & 0x10) > 0);
                    if (input_pins[5] > 0)
                        SpareParts.SetPin(input_pins[5], (val & 0x08) > 0);
                    if (input_pins[4] > 0)
                        SpareParts.SetPin(input_pins[4], (val & 0x04) > 0);
                    if (input_pins[3] > 0)
                        SpareParts.SetPin(input_pins[3], (val & 0x02) > 0);
                    if (input_pins[2] > 0)
                        SpareParts.SetPin(input_pins[2], (val & 0x01) > 0);
                }
            }
        }
        lcde = ppins[input_pins[1] - 1].value;
    } else {
        lcde = 0;
    }
}

void cpart_LCD_hd44780::PostProcess(void) {
    if (lcd.update)
        output_ids[O_LCD]->update = 1;
}

void cpart_LCD_hd44780::LoadPartImage(void) {
    int bmp = -1;

    if (BitmapId >= 0) {
        SpareParts.CanvasCmd({.cmd = CC_FREEBITMAP, .FreeBitmap{BitmapId}});
    }

    switch (model) {
        case LCD16x2:
            bmp = SpareParts.CanvasCmd(
                {.cmd = CC_LOADIMAGE,
                 .LoadImage{(PICSimLab.GetSharePath() + "parts/Output/" + GetPictureFileName()).c_str(), Scale, 0,
                            Orientation}});
            break;
        case LCD16x4:
            bmp = SpareParts.CanvasCmd(
                {.cmd = CC_LOADIMAGE,
                 .LoadImage{(PICSimLab.GetSharePath() + "parts/Output/" + GetPictureFileName_()).c_str(), Scale, 0,
                            Orientation}});
            break;
        case LCD20x2:
            bmp = SpareParts.CanvasCmd(
                {.cmd = CC_LOADIMAGE,
                 .LoadImage{(PICSimLab.GetSharePath() + "parts/Output/" + GetPictureFileName__()).c_str(), Scale, 0,
                            Orientation}});
            break;
        case LCD20x4:
            bmp = SpareParts.CanvasCmd(
                {.cmd = CC_LOADIMAGE,
                 .LoadImage{(PICSimLab.GetSharePath() + "parts/Output/" + GetPictureFileName___()).c_str(), Scale, 0,
                            Orientation}});
            break;
    }

    SpareParts.SetPartOnDraw(id);
    BitmapId = bmp;
    SpareParts.CanvasCmd({.cmd = CC_DESTROY});
    SpareParts.CanvasCmd({.cmd = CC_CREATE, .Create{BitmapId}});

    for (int i = 0; i < outputc; i++) {
        output[i].update = 1;
    }

    lcd.update = 1;
}

part_init(PART_LCD_HD44780_Name, cpart_LCD_hd44780, "Output");

// Combined hd44780 + IO PCF8574

static part* cpart_LCD_hd44780_i2c_create(const unsigned int x, const unsigned int y, board* pboard_, const int id_) {
    cpart_IO_PCF8574* pcf = (cpart_IO_PCF8574*)SpareParts.AddPart("IO PCF8574", x, y + 365, SpareParts.GetScale());

    const unsigned char* pcf_pins = pcf->GetOutputPins();

    cpart_LCD_hd44780* lcd = new cpart_LCD_hd44780(x, y, PART_LCD_HD44780_Name, "Output", pboard_, id_ + 1);

    // connect lcd to pcf
    lcd->input_pins[0] = pcf_pins[0];
    lcd->input_pins[10] = pcf_pins[1];
    lcd->input_pins[1] = pcf_pins[2];
    lcd->input_pins[6] = pcf_pins[4];
    lcd->input_pins[7] = pcf_pins[5];
    lcd->input_pins[8] = pcf_pins[6];
    lcd->input_pins[9] = pcf_pins[7];

    // find GND
    for (int i = 0; i < 255; i++) {
        if (!SpareParts.GetPinName(i).compare("GND")) {
            lcd->input_pins[2] = i;
            lcd->input_pins[3] = i;
            lcd->input_pins[4] = i;
            lcd->input_pins[5] = i;
            break;
        }
    }

    return lcd;
}

static void __attribute__((constructor)) cpart_LCD_hd44780_i2c_init(void);

static void cpart_LCD_hd44780_i2c_init(void) {
    part_register(PART_LCD_HD44780_Name " I2C", cpart_LCD_hd44780_i2c_create, "Output");
}