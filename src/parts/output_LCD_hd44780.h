/* ########################################################################

   PICsimLab - PIC laboratory simulator

   ########################################################################

   Copyright (c) : 2010-2023  Luis Claudio Gambôa Lopes

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

#ifndef PART_LCD_HD44780_H
#define PART_LCD_HD44780_H

#include <lxrad.h>
#include "../devices/lcd_hd44780.h"
#include "part.h"

#define PART_LCD_HD44780_Name "LCD hd44780"

#define LCD16x2 1
#define LCD16x4 2
#define LCD20x2 4
#define LCD20x4 8

class cpart_LCD_hd44780 : public part {
public:
    lxString GetAboutInfo(void) override { return lxT("L.C. Gamboa \n <lcgamboa@yahoo.com>"); };
    cpart_LCD_hd44780(const unsigned x, const unsigned y, const char* name, const char* type);
    ~cpart_LCD_hd44780(void);
    void DrawOutput(const unsigned int index) override;
    void Process(void) override;
    void PostProcess(void) override;
    lxString GetPictureFileName(void) override { return lxT("LCD hd44780/LCD_hd44780.svg"); };
    lxString GetPictureFileName_(void) { return lxT("LCD hd44780/LCD_hd44780_.svg"); };
    lxString GetPictureFileName__(void) { return lxT("LCD hd44780/LCD_hd44780__.svg"); };
    lxString GetPictureFileName___(void) { return lxT("LCD hd44780/LCD_hd44780___.svg"); };
    void Reset(void) override;
    void ConfigurePropertiesWindow(CPWindow* WProp) override;
    void ReadPropertiesWindow(CPWindow* WProp) override;
    lxString WritePreferences(void) override;
    void LoadImage(void) override;
    void ReadPreferences(lxString value) override;
    unsigned short GetInputId(char* name) override;
    unsigned short GetOutputId(char* name) override;
    unsigned char input_pins[11];

private:
    void InitGraphics(void);
    void RegisterRemoteControl(void) override;
    lcd_t lcd;
    int lcde;
    unsigned char model;
};

#endif /* PART_LCD_HD44780 */
