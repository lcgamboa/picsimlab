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

#ifndef PART_VCD_Play_H
#define PART_VCD_Play_H

#include "../lib/part.h"

#define PART_VCD_Play_Name "VCD Play"

typedef struct {
    unsigned long count;
    unsigned char data;
} vcd_reg_t;

class cpart_VCD_Play : public part {
public:
    std::string GetAboutInfo(void) override { return "L.C. Gamboa \n <lcgamboa@yahoo.com>"; };
    cpart_VCD_Play(const unsigned x, const unsigned y, const char* name, const char* type, board* pboard_,
                   const int id_);
    ~cpart_VCD_Play(void);
    void DrawOutput(const unsigned int index) override;
    void PreProcess(void) override;
    void Process(void) override;
    void PostProcess(void) override;
    void OnMouseButtonPress(unsigned int inputId, unsigned int button, unsigned int x, unsigned int y,
                            unsigned int state) override;
    void ConfigurePropertiesWindow(void) override;
    void filedialog_EvOnClose(int retId) override;
    void ReadPropertiesWindow(void) override;
    std::string WritePreferences(void) override;
    void ReadPreferences(std::string value) override;
    unsigned short GetInputId(char* name) override;
    unsigned short GetOutputId(char* name) override;
    int LoadVCD(std::string fname);

private:
    void RegisterRemoteControl(void) override;
    unsigned char output_pins[8];
    char f_vcd_name[200];
    unsigned char play;
    float timescale;
    unsigned long vcd_count;
    float vcd_inc;
    float vcd_step;
    vcd_reg_t* vcd_data;
    int vcd_data_count;
    int vcd_ptr;
};

#endif /* PART_VCD_Play_H */
