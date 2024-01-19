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

#ifndef PART_LCD_iLI9341_H
#define PART_LCD_iLI9341_H

#include <lxrad.h>
#include "../devices/lcd_ili9341.h"
#include "../devices/tsc_xpt2046.h"
#include "../lib/part.h"

#define PART_LCD_iLI9341_Name "LCD ili9341"

class cpart_LCD_ili9341 : public part {
public:
    std::string GetAboutInfo(void) override { return "L.C. Gamboa \n <lcgamboa@yahoo.com>"; };
    cpart_LCD_ili9341(const unsigned x, const unsigned y, const char* name, const char* type, board* pboard_);
    ~cpart_LCD_ili9341(void);
    void DrawOutput(const unsigned int index) override;
    void PreProcess(void) override;
    void Process(void) override;
    void PostProcess(void) override;
    std::string GetPictureFileName(void) override;
    std::string GetMapFile(void) override;
    void OnMouseButtonPress(uint inputId, uint button, uint x, uint y, uint state) override;
    void OnMouseButtonRelease(uint inputId, uint button, uint x, uint y, uint state) override;
    void OnMouseMove(uint inputId, uint button, uint x, uint y, uint state) override;
    void Reset(void) override;
    void ConfigurePropertiesWindow(CPWindow* WProp) override;
    void ReadPropertiesWindow(CPWindow* WProp) override;
    std::string WritePreferences(void) override;
    void LoadImage(void) override;
    void ReadPreferences(std::string value) override;
    unsigned short GetInputId(char* name) override;
    unsigned short GetOutputId(char* name) override;
    void ComboChange(CPWindow* WProp, CCombo* control, std::string value) override;

private:
    void ChangeType(unsigned char tp);
    unsigned char input_pins[13];
    unsigned char touch_pins[5];
    lcd_ili9341_t lcd;
    tsc_XPT2046_t touch;
    unsigned char type_com;
    unsigned char active[1];
    lxFont font;
    int valid_lcd_pins;
    int valid_touch_pins;
    const unsigned char* pins_value[13];
    const unsigned char* tpins_value[5];
    const unsigned char* tpins_int_value;
    unsigned short lret;
    unsigned short tret;
};

#endif /* PART_LCD_iLI9341_H */
