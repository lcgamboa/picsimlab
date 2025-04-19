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

#ifndef PART_DCMOTOR_H
#define PART_DCMOTOR_H

#include "../lib/part.h"

#define PART_DCMOTOR_Name "DC Motor"

class cpart_dcmotor : public part {
public:
    std::string GetAboutInfo(void) override { return "L.C. Gamboa \n <lcgamboa@yahoo.com>"; };
    cpart_dcmotor(const unsigned x, const unsigned y, const char* name, const char* type, board* pboard_,
                  const int id_);
    ~cpart_dcmotor(void);
    void DrawOutput(const unsigned int index) override;
    void PreProcess(void) override;
    void Process(void) override;
    void PostProcess(void) override;
    void ConfigurePropertiesWindow(void) override;
    void ReadPropertiesWindow(void) override;
    std::string WritePreferences(void) override;
    void ReadPreferences(std::string value) override;
    unsigned short GetInputId(char* name) override;
    unsigned short GetOutputId(char* name) override;

private:
    void RegisterRemoteControl(void) override;
    unsigned char pins[5];
    unsigned char value;
    unsigned char value_old;
    int step;
    int count;
    unsigned char state;
    unsigned char dir;
    unsigned char speed;
    unsigned char* status[3];
};

#endif /* DCMOTOR */
