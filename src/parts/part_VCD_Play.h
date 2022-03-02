/* ########################################################################

   PICsimLab - PIC laboratory simulator

   ########################################################################

   Copyright (c) : 2019-2022  Luis Claudio Gambôa Lopes

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

#include <lxrad.h>
#include "part.h"

#define PART_VCD_Play_Name "VCD Play"

typedef struct {
    unsigned long count;
    unsigned char data;
} vcd_reg_t;

class cpart_VCD_Play : public part {
public:
    lxString GetName(void) override { return lxT(PART_VCD_Play_Name); };
    lxString GetAboutInfo(void) override { return lxT("L.C. Gamboa \n <lcgamboa@yahoo.com>"); };
    cpart_VCD_Play(unsigned x, unsigned y);
    ~cpart_VCD_Play(void);
    void Draw(void) override;
    void PreProcess(void) override;
    void Process(void) override;
    void PostProcess(void) override;
    void EvMouseButtonPress(uint button, uint x, uint y, uint state) override;
    void ConfigurePropertiesWindow(CPWindow* WProp) override;
    void filedialog_EvOnClose(int retId) override;
    void ReadPropertiesWindow(CPWindow* WProp) override;
    lxString WritePreferences(void) override;
    void ReadPreferences(lxString value) override;
    unsigned short get_in_id(char* name) override;
    unsigned short get_out_id(char* name) override;
    int LoadVCD(lxString fname);

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
    lxFont font;
    lxColor color1;
    lxColor color2;
};

#endif /* PART_VCD_Play_H */
