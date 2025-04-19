/* ########################################################################

   PICSimLab - Programmable IC Simulator Laboratory

   ########################################################################

   Copyright (c) : 2019-2025  Luis Claudio Gambôa Lopes <lcgamboa@yahoo.com>

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

#ifndef PART_JUMPERS_H
#define PART_JUMPERS_H

#include "../lib/part.h"

#define PART_JUMPERS_Name "Jumper Wires"

class cpart_Jumpers : public part {
    int ID;
    static int countID;

public:
    std::string GetAboutInfo(void) override { return "L.C. Gamboa \n <lcgamboa@yahoo.com>"; };
    cpart_Jumpers(const unsigned x, const unsigned y, const char* name, const char* type, board* pboard_,
                  const int id_);
    ~cpart_Jumpers(void);
    void DrawOutput(const unsigned int index) override;
    void PreProcess(void) override;
    void Process(void) override;
    void PostProcess(void) override;
    void ConfigurePropertiesWindow(void) override;
    void ReadPropertiesWindow(void) override;
    std::string WritePreferences(void) override;
    void ReadPreferences(std::string value) override;
    unsigned short GetInputId(char* name) override;
    unsigned short GetOutputId(char* name) override;
    void ComboChange(const char* controlname, std::string value) override;

private:
    void ChangeType(unsigned char type);
    void RegisterRemoteControl(void) override;
    unsigned char input_pins[16];
    unsigned char output_pins[16];
    unsigned long output_pins_alm[16];
    long mcount;
    int JUMPSTEPS_;
    std::string jname;
    unsigned char jtype;
};

// input/output   0-M 1-F
#define JWT_MM 0x00
#define JWT_MF 0x01
#define JWT_FM 0x02
#define JWT_FF 0x03

#endif /* PART_IJUMPERS_H */
