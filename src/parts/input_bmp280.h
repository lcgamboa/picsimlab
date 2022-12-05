/* ########################################################################

   PICsimLab - PIC laboratory simulator

   ########################################################################

   Copyright (c) : 2021-2022  Luis Claudio Gambôa Lopes

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

#ifndef PART_RTC_BMP280_H
#define PART_RTC_BMP280_H

#include <lxrad.h>
#include "../devices/sen_bmp280.h"
#include "part.h"

#define PART_BMP280_Name "BMP280 (Pressure I2C)"

class cpart_bmp280 : public part {
public:
    lxString GetAboutInfo(void) override { return lxT("L.C. Gamboa \n <lcgamboa@yahoo.com>"); };
    cpart_bmp280(const unsigned x, const unsigned y, const char* name, const char* type);
    ~cpart_bmp280(void);
    void Draw(void) override;
    void PreProcess(void) override;
    void Process(void) override;
    void EvMouseButtonPress(uint button, uint x, uint y, uint state) override;
    void EvMouseButtonRelease(uint button, uint x, uint y, uint state) override;
    void EvMouseMove(uint button, uint x, uint y, uint state) override;
    void ConfigurePropertiesWindow(CPWindow* WProp) override;
    void ReadPropertiesWindow(CPWindow* WProp) override;
    lxString WritePreferences(void) override;
    void ReadPreferences(lxString value) override;
    unsigned short GetInputId(char* name) override;
    unsigned short GetOutputId(char* name) override;

private:
    unsigned char input_pins[2];
    sen_bmp280_t bmp280;
    unsigned char values[2];
    unsigned char active[2];
    lxFont font;
    lxFont font_p;
};

#endif /* PART_v_H */
