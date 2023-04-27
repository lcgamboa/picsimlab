/* ########################################################################

   PICSimLab - Programmable IC Simulator Laboratory

   ########################################################################

   Copyright (c) : 2021-2023  Luis Claudio Gambôa Lopes <lcgamboa@yahoo.com>

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
#include "../devices/sen_hx711.h"
#include "../lib/part.h"

#define PART_BMP280_Name "HX711 (Load Cell Amp)"

class cpart_hx711 : public part {
public:
    lxString GetAboutInfo(void) override { return lxT("L.C. Gamboa \n <lcgamboa@yahoo.com>"); };
    cpart_hx711(const unsigned x, const unsigned y, const char* name, const char* type, board* pboard_);
    ~cpart_hx711(void);
    void DrawOutput(const unsigned int index) override;
    void PreProcess(void) override;
    void Process(void) override;
    void Reset(void) override;
    void OnMouseButtonPress(uint inputId, uint button, uint x, uint y, uint state) override;
    void OnMouseButtonRelease(uint inputId, uint button, uint x, uint y, uint state) override;
    void OnMouseMove(uint inputId, uint button, uint x, uint y, uint state) override;
    void ConfigurePropertiesWindow(CPWindow* WProp) override;
    void ReadPropertiesWindow(CPWindow* WProp) override;
    lxString WritePreferences(void) override;
    void ReadPreferences(lxString value) override;
    unsigned short GetInputId(char* name) override;
    unsigned short GetOutputId(char* name) override;

private:
    unsigned char pins[2];
    sen_hx711_t hx711;
    unsigned char values[1];
    unsigned char active[1];
    lxFont font;
    lxFont font_p;
    unsigned char ret_;
};

#endif /* PART_v_H */
