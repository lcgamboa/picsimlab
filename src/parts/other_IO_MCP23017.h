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

#ifndef PART_IO_MCP23017_H
#define PART_IO_MCP23017_H

#include "../devices/io_MCP23X17.h"
#include "../lib/part.h"

#define PART_IO_MCP23017_Name "IO MCP23017"

class cpart_IO_MCP23017 : public part {
public:
    std::string GetAboutInfo(void) override { return "L.C. Gamboa \n <lcgamboa@yahoo.com>"; };
    cpart_IO_MCP23017(const unsigned x, const unsigned y, const char* name, const char* type, board* pboard_,
                      const int id_);
    ~cpart_IO_MCP23017(void);
    void DrawOutput(const unsigned int index) override;
    void PreProcess(void) override;
    void Process(void) override;
    void PostProcess(void) override;
    void Reset(void) override;
    std::string GetPictureFileName(void) override { return "../Common/IC28.svg"; };
    std::string GetMapFile(void) override { return "../Common/IC28.map"; };
    void ConfigurePropertiesWindow(void) override;
    void ReadPropertiesWindow(void) override;
    std::string WritePreferences(void) override;
    void ReadPreferences(std::string value) override;
    unsigned short GetInputId(char* name) override;
    unsigned short GetOutputId(char* name) override;

private:
    unsigned char input_pins[8];
    unsigned char output_pins[16];
    unsigned long output_pins_alm[16];
    long mcount;
    int JUMPSTEPS_;
    io_MCP23X17_t mcp;
    unsigned char _PA;
    unsigned char _PB;
    unsigned char _PA_INT;
    unsigned char _PB_INT;
    unsigned char _DIRA;
    unsigned char _DIRB;
};

#endif /* PART_IO_MCP23017_H */
