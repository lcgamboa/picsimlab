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

#include "output_LCD_pcf8833.h"
#include "../lib/oscilloscope.h"
#include "../lib/picsimlab.h"
#include "../lib/spareparts.h"

/* outputs */
enum { O_P1, O_P2, O_P3, O_P4, O_F1, O_F2, O_F3, O_F4, O_F5, O_LCD };

static PCWProp pcwprop[11] = {
    {PCW_LABEL, "1-VDDI,+3.3V"}, {PCW_COMBO, "2-/RST"},       {PCW_COMBO, "3-DAT"},   {PCW_COMBO, "4-CLK"},
    {PCW_COMBO, "5-/CS"},        {PCW_LABEL, "6-VDD,+3.3V"},  {PCW_LABEL, "7-NC,NC"}, {PCW_LABEL, "8-GND,GND"},
    {PCW_LABEL, "9-VLED-,GND"},  {PCW_LABEL, "10-VLED+,+6V"}, {PCW_END, ""}};

cpart_LCD_pcf8833::cpart_LCD_pcf8833(const unsigned x, const unsigned y, const char* name, const char* type,
                                     board* pboard_)
    : part(x, y, name, type, pboard_) {
    X = x;
    Y = y;
    ReadMaps();
    Bitmap = NULL;

    LoadImage();

    lcd_pcf8833_init(&lcd);
    lcd_pcf8833_rst(&lcd);

    input_pins[0] = 0;
    input_pins[1] = 0;
    input_pins[2] = 0;
    input_pins[3] = 0;

    SetPCWProperties(pcwprop);

    PinCount = 4;
    Pins = input_pins;
}

cpart_LCD_pcf8833::~cpart_LCD_pcf8833(void) {
    delete Bitmap;
    canvas.Destroy();
}

void cpart_LCD_pcf8833::DrawOutput(const unsigned int i) {
    switch (output[i].id) {
        case O_P1:
        case O_P2:
        case O_P3:
        case O_P4:
            canvas.SetFontSize(8);
            canvas.SetColor(49, 61, 99);
            canvas.Rectangle(1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);
            canvas.SetFgColor(255, 255, 255);
            if (input_pins[output[i].id - O_P1] == 0)
                canvas.RotatedText("NC", output[i].x1, output[i].y2, 90.0);
            else
                canvas.RotatedText(SpareParts.GetPinName(input_pins[output[i].id - O_P1]), output[i].x1, output[i].y2,
                                   90.0);
            break;
        case O_F1:
            canvas.SetColor(49, 61, 99);
            canvas.Rectangle(1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);
            canvas.SetFgColor(155, 155, 155);
            canvas.RotatedText("3.3V", output[i].x1, output[i].y2, 90.0);
            break;
        case O_F2:
            canvas.SetColor(49, 61, 99);
            canvas.Rectangle(1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);
            canvas.SetFgColor(155, 155, 155);
            canvas.RotatedText("3.3V", output[i].x1, output[i].y2, 90.0);
            break;
        case O_F3:
            canvas.SetColor(49, 61, 99);
            canvas.Rectangle(1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);
            canvas.SetFgColor(155, 155, 155);
            canvas.RotatedText("GND", output[i].x1, output[i].y2, 90.0);
            break;
        case O_F4:
            canvas.SetColor(49, 61, 99);
            canvas.Rectangle(1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);
            canvas.SetFgColor(155, 155, 155);
            canvas.RotatedText("GND", output[i].x1, output[i].y2, 90.0);
            break;
        case O_F5:
            canvas.SetColor(49, 61, 99);
            canvas.Rectangle(1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);
            canvas.SetFgColor(155, 155, 155);
            canvas.RotatedText("6V", output[i].x1, output[i].y2, 90.0);
            break;
        case O_LCD:
            // draw lcd text
            if (lcd.update) {
                canvas.SetColor(0, 90 + 40, 0);
                lcd_pcf8833_draw(&lcd, &canvas, output[i].x1, output[i].y1, output[i].x2 - output[i].x1,
                                 output[i].y2 - output[i].y1, 1);
            }
            /*
            else
            {
               canvas.Rectangle (1, output[i].x1, output[i].y1,
            output[i].x2-output[i].x1,output[i].y2-output[i].y1 );
            }
             */
            break;
    }
}

unsigned short cpart_LCD_pcf8833::GetInputId(char* name) {
    printf("Error input '%s' don't have a valid id! \n", name);
    return INVALID_ID;
};

unsigned short cpart_LCD_pcf8833::GetOutputId(char* name) {
    if (strcmp(name, "PN_1") == 0)
        return O_P1;
    if (strcmp(name, "PN_2") == 0)
        return O_P2;
    if (strcmp(name, "PN_3") == 0)
        return O_P3;
    if (strcmp(name, "PN_4") == 0)
        return O_P4;

    if (strcmp(name, "PN_F1") == 0)
        return O_F1;
    if (strcmp(name, "PN_F2") == 0)
        return O_F2;
    if (strcmp(name, "PN_F3") == 0)
        return O_F3;
    if (strcmp(name, "PN_F4") == 0)
        return O_F4;
    if (strcmp(name, "PN_F5") == 0)
        return O_F5;

    if (strcmp(name, "DS_LCD") == 0)
        return O_LCD;

    printf("Error output '%s' don't have a valid id! \n", name);
    return INVALID_ID;
};

std::string cpart_LCD_pcf8833::WritePreferences(void) {
    char prefs[256];

    sprintf(prefs, "%hhu,%hhu,%hhu,%hhu", input_pins[0], input_pins[1], input_pins[2], input_pins[3]);

    return prefs;
}

void cpart_LCD_pcf8833::ReadPreferences(std::string value) {
    sscanf(value.c_str(), "%hhu,%hhu,%hhu,%hhu", &input_pins[0], &input_pins[1], &input_pins[2], &input_pins[3]);
    Reset();
}

void cpart_LCD_pcf8833::ConfigurePropertiesWindow(CPWindow* WProp) {
    SetPCWComboWithPinNames(WProp, "combo2", input_pins[0]);
    SetPCWComboWithPinNames(WProp, "combo3", input_pins[1]);
    SetPCWComboWithPinNames(WProp, "combo4", input_pins[2]);
    SetPCWComboWithPinNames(WProp, "combo5", input_pins[3]);
}

void cpart_LCD_pcf8833::ReadPropertiesWindow(CPWindow* WProp) {
    input_pins[0] = GetPWCComboSelectedPin(WProp, "combo2");
    input_pins[1] = GetPWCComboSelectedPin(WProp, "combo3");
    input_pins[2] = GetPWCComboSelectedPin(WProp, "combo4");
    input_pins[3] = GetPWCComboSelectedPin(WProp, "combo5");
}

void cpart_LCD_pcf8833::Process(void) {
    const picpin* ppins = SpareParts.GetPinsValues();

    if ((input_pins[0]) && (input_pins[1]) && (input_pins[2]) && (input_pins[3])) {
        lcd_pcf8833_io(&lcd, ppins[input_pins[1] - 1].value, ppins[input_pins[2] - 1].value,
                       ppins[input_pins[3] - 1].value, ppins[input_pins[0] - 1].value);
    }
}

void cpart_LCD_pcf8833::PostProcess(void) {
    if (lcd.update)
        output_ids[O_LCD]->update = 1;
}

void cpart_LCD_pcf8833::LoadImage(void) {
    part::LoadImage();
    lcd_pcf8833_update(&lcd);
}

part_init(PART_LCD_PCF8833_Name, cpart_LCD_pcf8833, "Output");
