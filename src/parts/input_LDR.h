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

#ifndef PART_LDR_H
#define PART_LDR_H

#include "../lib/part.h"

#define PART_LDR_Name "LDR"

class cpart_LDR : public part {
public:
    std::string GetAboutInfo(void) override { return "L.C. Gamboa \n <lcgamboa@yahoo.com>"; };
    cpart_LDR(const unsigned x, const unsigned y, const char* name, const char* type, board* pboard_);
    ~cpart_LDR(void);
    void DrawOutput(const unsigned int index) override;
    void PostProcess(void) override;
    void OnMouseButtonPress(uint inputId, uint button, uint x, uint y, uint state) override;
    void OnMouseButtonRelease(uint inputId, uint button, uint x, uint y, uint state) override;
    void OnMouseMove(uint inputId, uint button, uint x, uint y, uint state) override;
    void ConfigurePropertiesWindow(CPWindow* WProp) override;
    void ReadPropertiesWindow(CPWindow* WProp) override;
    std::string WritePreferences(void) override;
    void ReadPreferences(std::string value) override;
    void Reset(void) override;
    unsigned short GetInputId(char* name) override;
    unsigned short GetOutputId(char* name) override;

private:
    void RegisterRemoteControl(void) override;
    unsigned char output_pins[2];
    unsigned char value;
    unsigned char active;
    float vthreshold;
    float lux;
    float vout;
    float vmax;
};

#endif /* LDR */
