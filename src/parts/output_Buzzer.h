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

#ifndef PART_BUZZER_H
#define PART_BUZZER_H

#include <lxrad.h>
#include "../lib/part.h"

#define PART_BUZZER_Name "Buzzer"

class cpart_Buzzer : public part {
public:
    std::string GetAboutInfo(void) override { return "L.C. Gamboa \n <lcgamboa@yahoo.com>"; };
    cpart_Buzzer(const unsigned x, const unsigned y, const char* name, const char* type, board* pboard_);
    ~cpart_Buzzer(void);
    void DrawOutput(const unsigned int index) override;
    void PreProcess(void) override;
    void Process(void) override;
    void PostProcess(void) override;
    void Stop(void) override;
    void ConfigurePropertiesWindow(CPWindow* WProp) override;
    void ReadPropertiesWindow(CPWindow* WProp) override;
    std::string WritePreferences(void) override;
    void ReadPreferences(std::string value) override;
    unsigned short GetInputId(char* name) override;
    unsigned short GetOutputId(char* name) override;

private:
    void RegisterRemoteControl(void) override;
    void ChangeType(unsigned char tp);
    unsigned char active;
    unsigned char input_pins[1];
    lxaudio buzzer;
    long int mcount;
    int JUMPSTEPS_;
    unsigned char btype;
    unsigned int samplerate;
    short* buffer;
    unsigned int buffersize;
    unsigned int buffercount;
    unsigned int maxv;
    unsigned char optone;
    unsigned int ctone;
    float ftone;
    float oftone;
    float in[3];
    float out[3];
    lxFont font;
    lxColor color1;
    lxColor color2;
    CTimer* timer;
};

#endif /* PART_BUZZER */
