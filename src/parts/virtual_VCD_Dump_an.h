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

#ifndef PART_VCD_DUMP_AN_H
#define PART_VCD_DUMP_AN_H

#include <lxrad.h>
#include "../lib/part.h"

#define PART_VCD_DUMP_AN_Name "VCD Dump (Analogic)"

class cpart_VCD_Dump_an : public part {
public:
    std::string GetAboutInfo(void) override { return "L.C. Gamboa \n <lcgamboa@yahoo.com>"; };
    cpart_VCD_Dump_an(const unsigned x, const unsigned y, const char* name, const char* type, board* pboard_);
    ~cpart_VCD_Dump_an(void);
    void DrawOutput(const unsigned int index) override;
    void PreProcess(void) override;
    void Process(void) override;
    void PostProcess(void) override;
    std::string GetPictureFileName(void) override { return "VCD Dump/part.svg"; };
    std::string GetMapFile(void) override { return "VCD Dump/part.map"; };
    std::string GetPropertiesWindowFile(void) override { return "VCD Dump/part.lxrad"; };
    void OnMouseButtonPress(uint inputId, uint button, uint x, uint y, uint state) override;
    void ConfigurePropertiesWindow(CPWindow* WProp) override;
    void ReadPropertiesWindow(CPWindow* WProp) override;
    std::string WritePreferences(void) override;
    void ReadPreferences(std::string value) override;
    unsigned short GetInputId(char* name) override;
    unsigned short GetOutputId(char* name) override;

private:
    void RegisterRemoteControl(void) override;
    unsigned char input_pins[8];
    float old_value_pins[8];
    char f_vcd_name[200];
    FILE* f_vcd;
    unsigned long vcd_count;
    unsigned char rec;
};

#endif /* PART_VCD_DUMP_AN_H */
