/* ########################################################################

   PICSimLab - Programmable IC Simulator Laboratory

   ########################################################################

   Copyright (c) : 2024-2024  Luis Claudio Gambôa Lopes <lcgamboa@yahoo.com>

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

#ifndef PART_VCD_DUMP_MEM_H
#define PART_VCD_DUMP_MEM_H

#include "../lib/part.h"

#define PART_VCD_DUMP_MEM_Name "VCD Dump Memory"

class cpart_VCD_Dump_Mem : public part {
public:
    std::string GetAboutInfo(void) override { return "L.C. Gamboa \n <lcgamboa@yahoo.com>"; };
    cpart_VCD_Dump_Mem(const unsigned x, const unsigned y, const char* name, const char* type, board* pboard_,
                       const int id_);
    ~cpart_VCD_Dump_Mem(void);
    void DrawOutput(const unsigned int index) override;
    void PreProcess(void) override;
    void Process(void) override;
    std::string GetPictureFileName(void) override { return "VCD Dump/part.svg"; };
    std::string GetMapFile(void) override { return "VCD Dump/part.map"; };
    void PostProcess(void) override;
    void OnMouseButtonPress(unsigned int inputId, unsigned int button, unsigned int x, unsigned int y,
                            unsigned int state) override;
    void ConfigurePropertiesWindow(void) override;
    void ReadPropertiesWindow(void) override;
    std::string WritePreferences(void) override;
    void ReadPreferences(std::string value) override;
    unsigned short GetInputId(char* name) override;
    unsigned short GetOutputId(char* name) override;

private:
    void RegisterRemoteControl(void) override;

    unsigned long output_bits_alm[8];
    unsigned char output_bits[8];
    long mcount;
    int JUMPSTEPS_;
    char f_vcd_name[200];
    FILE* f_vcd;
    unsigned long vcd_count;
    unsigned char rec;
    unsigned char mem_size;
    unsigned short mem_addr;
    unsigned short* mem_ptr;
    unsigned short mem_old;
};

#endif /* PART_VCD_DUMP_MEM_H */
