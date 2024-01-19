/* ########################################################################

   PICSimLab - Programmable IC Simulator Laboratory

   ########################################################################

   Copyright (c) : 2021-2024  Luis Claudio Gambôa Lopes <lcgamboa@yahoo.com>

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

#ifndef PART_IO_MM74C922_H
#define PART_IO_MM74C922_H

#include <lxrad.h>
#include "../devices/io_MM74C922.h"
#include "../lib/part.h"

#define PART_IO_MM74C922_Name "IO MM74C922"

class cpart_IO_MM74C922 : public part {
public:
    std::string GetAboutInfo(void) override { return "L.C. Gamboa \n <lcgamboa@yahoo.com>"; };
    cpart_IO_MM74C922(const unsigned x, const unsigned y, const char* name, const char* type, board* pboard_);
    ~cpart_IO_MM74C922(void);
    void DrawOutput(const unsigned int index) override;
    void PreProcess(void) override;
    void Process(void) override;
    void PostProcess(void) override;
    std::string GetPictureFileName(void) override { return "../Common/IC18.svg"; };
    std::string GetMapFile(void) override { return "../Common/IC18.map"; };
    void ConfigurePropertiesWindow(CPWindow* WProp) override;
    void ReadPropertiesWindow(CPWindow* WProp) override;
    std::string WritePreferences(void) override;
    void ReadPreferences(std::string value) override;
    unsigned short GetInputId(char* name) override;
    unsigned short GetOutputId(char* name) override;

private:
    unsigned char input_pins[4];
    unsigned char output_pins[9];
    unsigned long pins_alm[8];
    long mcount;
    int JUMPSTEPS_;
    io_MM74C922_t kc;
    unsigned short _ret;
    lxFont font;
};

#endif /* PART_IO_MM74C922_H */
