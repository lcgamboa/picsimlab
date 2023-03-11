/* ########################################################################

   PICSimLab - Programmable IC Simulator Laboratory

   ########################################################################

   Copyright (c) : 2010-2023  Luis Claudio Gambôa Lopes <lcgamboa@yahoo.com>

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

#ifndef PART_LEDS_H
#define PART_LEDS_H

#include <lxrad.h>
#include "../lib/part.h"

#define PART_LEDS_Name "LEDs"

class cpart_leds : public part {
public:
    lxString GetAboutInfo(void) override { return lxT("L.C. Gamboa \n <lcgamboa@yahoo.com>"); };
    cpart_leds(const unsigned x, const unsigned y, const char* name, const char* type);
    ~cpart_leds(void);
    void DrawOutput(const unsigned int index) override;
    void PostProcess(void) override;
    void ConfigurePropertiesWindow(CPWindow* WProp) override;
    void ReadPropertiesWindow(CPWindow* WProp) override;
    lxString WritePreferences(void) override;
    void ReadPreferences(lxString value) override;
    void LoadImage(void) override;
    unsigned short GetInputId(char* name) override;
    unsigned short GetOutputId(char* name) override;
    void SpinChange(CPWindow* WProp, CSpin* control, int value) override;

private:
    void ChangeSize(const unsigned int sz);
    void RegisterRemoteControl(void) override;
    unsigned char active;
    unsigned char input_pins[8];
    unsigned char colors[8];
    lxColor color1;
    lxColor color2;
    int OWidth;
    int OHeight;
    int xoff;
    unsigned int Size;
};

#endif /* PART_LEDS */
