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

#ifndef PART_LOGIC_BLOCK_H
#define PART_LOGIC_BLOCK_H

#include "../lib/part.h"

#define PART_LOGIC_BLOCK_Name "Logic Block"

enum { LG_NOT, LG_BUFFER, LG_AND, LG_NAND, LG_OR, LG_NOR, LG_XOR, LG_XNOR };
const char* logicgate_name[] = {"NOT", "BUFFER", "AND", "NAND", "OR", "NOR", "XOR", "XNOR"};

class cpart_lblock : public part {
public:
    std::string GetAboutInfo(void) override { return "L.C. Gamboa \n <lcgamboa@yahoo.com>"; };
    cpart_lblock(const unsigned x, const unsigned y, const char* name, const char* type, board* pboard_, const int id_);
    ~cpart_lblock(void);
    void DrawOutput(const unsigned int index) override;
    void PreProcess(void) override;
    void Process(void) override;
    void PostProcess(void) override;
    void Reset(void) override;
    void ConfigurePropertiesWindow(void) override;
    void ReadPropertiesWindow(void) override;
    void LoadPartImage(void) override;
    std::string WritePreferences(void) override;
    void ReadPreferences(std::string value) override;
    unsigned short GetInputId(char* name) override;
    unsigned short GetOutputId(char* name) override;
    void SpinChange(const char* controlname, int value) override;
    void ComboChange(const char* controlname, std::string value) override;

private:
    void ChangeSize(const unsigned int sz);
    unsigned char gatetype;
    unsigned char input_pins[8];
    unsigned char output_pins[2];
    unsigned long output_pins_alm;
    unsigned char output_value;
    unsigned char output_value_prev;
    int OWidth;
    int OHeight;
    int xoff;
    unsigned int Size;
    long mcount;
    int JUMPSTEPS_;
};

#endif /* PART_LOGIC_BLOCK_H */
