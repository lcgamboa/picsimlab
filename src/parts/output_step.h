/* ########################################################################

   PICSimLab - Programmable IC Simulator Laboratory

   ########################################################################

   Copyright (c) : 2010-2023  Luis Claudio Gambôa Lopes <lcgamboa@yahoo.com>

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

#ifndef PART_STEP_H
#define PART_STEP_H

#include <lxrad.h>
#include "../lib/part.h"

#define PART_STEP_Name "Step Motor"

/**
 * @brief step motor part class
 *
 * class definition of step motor class.
 */
class cpart_step : public part {
public:
    lxString GetAboutInfo(void) override { return lxT("L.C. Gamboa \n <lcgamboa@yahoo.com>"); };

    /**
     * @brief constructor called once on part creation
     *
     */
    cpart_step(const unsigned x, const unsigned y, const char* name, const char* type);

    /**
     * @brief destructor called once on part destruction
     *
     */
    ~cpart_step(void);

    void DrawOutput(const unsigned int index) override;
    void PreProcess(void) override;
    void Process(void) override;
    void PostProcess(void) override;
    void ConfigurePropertiesWindow(CPWindow* WProp) override;
    void ReadPropertiesWindow(CPWindow* WProp) override;
    lxString WritePreferences(void) override;
    void ReadPreferences(lxString value) override;
    unsigned short GetInputId(char* name) override;
    unsigned short GetOutputId(char* name) override;

private:
    void RegisterRemoteControl(void) override;
    unsigned char input_pins[4];   ///< pulse input pind
    unsigned char output_pins[1];  ///< pulse output pin
    float steps;
    float angle;
    int input[2];
    int turns;
    lxColor color1;
    lxColor color2;
    lxFont font;
};

#endif /* PART_STEP */
