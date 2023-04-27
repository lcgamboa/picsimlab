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

#ifndef PART_POT_H
#define PART_POT_H

#include <lxrad.h>
#include "../lib/part.h"

#define PART_POT_Name "Potentiometers"

class cpart_pot : public part {
public:
    lxString GetAboutInfo(void) override { return lxT("L.C. Gamboa \n <lcgamboa@yahoo.com>"); };
    cpart_pot(const unsigned x, const unsigned y, const char* name, const char* type, board* pboard_);
    ~cpart_pot(void);
    void DrawOutput(const unsigned int index) override;
    void PreProcess(void) override;
    void OnMouseButtonPress(uint inputId, uint button, uint x, uint y, uint state) override;
    void OnMouseButtonRelease(uint inputId, uint button, uint x, uint y, uint state) override;
    void OnMouseMove(uint inputId, uint button, uint x, uint y, uint state) override;
    void ConfigurePropertiesWindow(CPWindow* WProp) override;
    void ReadPropertiesWindow(CPWindow* WProp) override;
    void LoadImage(void) override;
    lxString WritePreferences(void) override;
    void ReadPreferences(lxString value) override;
    void Reset(void) override;
    unsigned short GetInputId(char* name) override;
    unsigned short GetOutputId(char* name) override;
    void SpinChange(CPWindow* WProp, CSpin* control, int value) override;

private:
    void RegisterRemoteControl(void) override;
    void ChangeSize(const unsigned int sz);
    unsigned char output_pins[4];
    unsigned char values[4];
    unsigned char active[4];
    lxFont font_p;
    float vmax;
    int OWidth;
    int OHeight;
    int xoff;
    unsigned int Size;
};

#endif /* POT */
