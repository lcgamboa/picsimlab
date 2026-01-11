/* ########################################################################

   PICSimLab - Programmable IC Simulator Laboratory

   ########################################################################

   Copyright (c) : 2010-2026 Luis Claudio Gambôa Lopes <lcgamboa@yahoo.com>

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

#ifndef PART_7S_DISPLAY_H
#define PART_7S_DISPLAY_H

#include "../lib/part.h"

#define PART_7S_DISPLAY_Name "7 Segments Display"

class cpart_7s_display : public part {
public:
    std::string GetAboutInfo(void) override { return "L.C. Gamboa \n <lcgamboa@yahoo.com>"; };
    cpart_7s_display(const unsigned x, const unsigned y, const char* name, const char* type, board* pboard_,
                     const int id_);
    ~cpart_7s_display(void);
    void DrawOutput(const unsigned int index) override;
    void PreProcess(void) override;
    void Process(void) override;
    void PostProcess(void) override;
    std::string GetPictureFileName(void) override;
    std::string GetMapFile(void) override;
    void ConfigurePropertiesWindow(void) override;
    void ReadPropertiesWindow(void) override;
    std::string WritePreferences(void) override;
    void ReadPreferences(std::string value) override;
    unsigned short GetInputId(char* name) override;
    unsigned short GetOutputId(char* name) override;
    void ComboChange(const char* controlname, std::string value) override;

private:
    void RegisterRemoteControl(void) override;
    void ChangeType(unsigned char tp);
    unsigned char active;
    unsigned char input_pins[12];
    unsigned int lm1[8];  // luminosidade media display
    unsigned int lm2[8];  // luminosidade media display
    unsigned int lm3[8];  // luminosidade media display
    unsigned int lm4[8];  // luminosidade media display

    unsigned int alm1[8];
    unsigned int alm2[8];
    unsigned int alm3[8];
    unsigned int alm4[8];
    long int mcount;
    int JUMPSTEPS_;
    unsigned char dtype;
};

#endif /* PART_7S_DISPLAY */
