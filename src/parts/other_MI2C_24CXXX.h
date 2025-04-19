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

#ifndef PART_MI2C_24CXXX_H
#define PART_MI2C_24CXXX_H

#include "../lib/part.h"

#include "../devices/mi2c_24CXXX.h"

#define PART_MI2C_24CXXX_Name "MEM 24CXXX"

class cpart_MI2C_24CXXX : public part {
public:
    std::string GetAboutInfo(void) override { return "L.C. Gamboa \n <lcgamboa@yahoo.com>"; };
    cpart_MI2C_24CXXX(const unsigned x, const unsigned y, const char* name, const char* type, board* pboard_,
                      const int id_);
    ~cpart_MI2C_24CXXX(void);
    void DrawOutput(const unsigned int index) override;
    void PreProcess(void) override;
    void Process(void) override;
    void OnMouseButtonPress(unsigned int inputId, unsigned int button, unsigned int x, unsigned int y,
                            unsigned int state) override;
    void ConfigurePropertiesWindow(void) override;
    void filedialog_EvOnClose(int retId) override;
    void ReadPropertiesWindow(void) override;
    std::string WritePreferences(void) override;
    void ReadPreferences(std::string value) override;
    unsigned short GetInputId(char* name) override;
    unsigned short GetOutputId(char* name) override;

private:
    unsigned char input_pins[5];
    mi2c_t mi2c;
    int kbits;
    char f_mi2c_name[200];
    char f_mi2c_tmp_name[200];
    FILE* f_mi2c;
};

#endif /* PART_MI2C_24CXXX_H */
