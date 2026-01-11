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

#ifndef PART_TEMPSYS_H
#define PART_TEMPSYS_H

#include "../lib/part.h"

#define PART_TEMPSYS_Name "Temperature System"

class cpart_tempsys : public part {
public:
    std::string GetAboutInfo(void) override { return "L.C. Gamboa \n <lcgamboa@yahoo.com>"; };
    cpart_tempsys(const unsigned x, const unsigned y, const char* name, const char* type, board* pboard_,
                  const int id_);
    ~cpart_tempsys(void);
    void DrawOutput(const unsigned int index) override;
    void Process(void) override;
    void ConfigurePropertiesWindow(void) override;
    void ReadPropertiesWindow(void) override;
    std::string WritePreferences(void) override;
    void ReadPreferences(std::string value) override;
    unsigned short GetInputId(char* name) override;
    unsigned short GetOutputId(char* name) override;
    void OnTime(void);

private:
    unsigned char input_pins[4];
    int vent[2];
    float temp[2];
    float ref;
    float ambient;
    float tvoff;
    int vtc;
    int vt;
    int rpmstp;
    int rpmc;
    int TimerID;
};

#endif /* PART_TEMPSYS */
