/* ########################################################################

   PICSimLab - Programmable IC Simulator Laboratory

   ########################################################################

   Copyright (c) : 2010-2025  Luis Claudio Gambôa Lopes <lcgamboa@yahoo.com>

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

#ifndef PART_SWITCHES_H
#define PART_SWITCHES_H

#include "../devices/swbounce.h"
#include "../lib/part.h"

#define PART_SWITCHES_Name "Switches"

class cpart_switches : public part {
public:
    std::string GetAboutInfo(void) override { return "L.C. Gamboa \n <lcgamboa@yahoo.com>"; };
    cpart_switches(const unsigned x, const unsigned y, const char* name, const char* type, board* pboard_,
                   const int id_);
    ~cpart_switches(void);
    void DrawOutput(const unsigned int index) override;
    void PreProcess(void) override;
    void Process(void) override;
    void Reset(void) override;
    void OnMouseButtonPress(unsigned int inputId, unsigned int button, unsigned int x, unsigned int y,
                            unsigned int state) override;
    void ConfigurePropertiesWindow(void) override;
    void ReadPropertiesWindow(void) override;
    void LoadPartImage(void) override;
    std::string WritePreferences(void) override;
    void ReadPreferences(std::string value) override;
    unsigned short GetInputId(char* name) override;
    unsigned short GetOutputId(char* name) override;
    void SpinChange(const char* controlname, int value) override;

private:
    void RegisterRemoteControl(void) override;
    void ChangeSize(const unsigned int sz);
    unsigned char output_pins[8];
    unsigned char output_value[8];
    unsigned char mode;
    SWBounce_t bounce;
    unsigned char active;
    int OWidth;
    int OHeight;
    int xoff;
    unsigned int Size;
};

#endif /* SWITCHES */
