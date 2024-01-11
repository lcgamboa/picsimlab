/* ########################################################################

   PICSimLab - Programmable IC Simulator Laboratory

   ########################################################################

   Copyright (c) : 2019-2023  Luis Claudio Gambôa Lopes <lcgamboa@yahoo.com>

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

#ifndef PART_IO_PCF8574_H
#define PART_IO_PCF8574_H

#include <lxrad.h>
#include "../devices/io_PCF8574.h"
#include "../lib/part.h"

#define PART_IO_PCF8574_Name "IO PCF8574"

class cpart_IO_PCF8574 : public part {
public:
    std::string GetAboutInfo(void) override { return "L.C. Gamboa \n <lcgamboa@yahoo.com>"; };
    cpart_IO_PCF8574(const unsigned x, const unsigned y, const char* name, const char* type, board* pboard_);
    ~cpart_IO_PCF8574(void);
    void DrawOutput(const unsigned int index) override;
    void PreProcess(void) override;
    void Process(void) override;
    void PostProcess(void) override;
    std::string GetPictureFileName(void) override { return "../Common/IC16.svg"; };
    std::string GetMapFile(void) override { return "../Common/IC16.map"; };
    void ConfigurePropertiesWindow(CPWindow* WProp) override;
    void ReadPropertiesWindow(CPWindow* WProp) override;
    std::string WritePreferences(void) override;
    void ReadPreferences(std::string value) override;
    unsigned short GetInputId(char* name) override;
    unsigned short GetOutputId(char* name) override;
    const unsigned char* GetOutputPins(void);

private:
    unsigned char input_pins[5];
    unsigned char output_pins[9];
    unsigned long output_pins_alm[9];
    long mcount;
    int JUMPSTEPS_;
    io_PCF8574_t ioe8;
    unsigned short _ret;
};

#endif /* PART_IO_PCF8574_H */
