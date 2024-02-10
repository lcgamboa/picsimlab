/* ########################################################################

   PICSimLab - Programmable IC Simulator Laboratory

   ########################################################################

   Copyright (c) : 2019-2024  Luis Claudio Gambôa Lopes <lcgamboa@yahoo.com>

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

#ifndef PART_KEYPAD_H
#define PART_KEYPAD_H

#include "../lib/part.h"

#define PART_KEYPAD_Name "Keypad"

class cpart_keypad : public part {
public:
    std::string GetAboutInfo(void) override { return "L.C. Gamboa \n <lcgamboa@yahoo.com>"; };
    cpart_keypad(const unsigned x, const unsigned y, const char* name, const char* type, board* pboard_, const int id_);
    ~cpart_keypad(void);
    void DrawOutput(const unsigned int index) override;
    void Process(void) override;
    std::string GetPictureFileName(void) override;
    std::string GetMapFile(void) override;
    void OnMouseButtonPress(unsigned int inputId, unsigned int button, unsigned int x, unsigned int y,
                            unsigned int state) override;
    void OnMouseButtonRelease(unsigned int inputId, unsigned int button, unsigned int x, unsigned int y,
                              unsigned int state) override;
    void ConfigurePropertiesWindow(void) override;
    void ReadPropertiesWindow(void) override;
    std::string WritePreferences(void) override;
    void ReadPreferences(std::string value) override;
    unsigned short GetInputId(char* name) override;
    unsigned short GetOutputId(char* name) override;
    void ComboChange(const char* controlname, std::string value) override;

private:
    void ChangeType(unsigned char tp);
    void RegisterRemoteControl(void) override;
    unsigned char type;
    unsigned char pull;
    unsigned char output_pins[8];
    unsigned char keys[4][4];
    unsigned char keys2[2][5];
};

#endif /* PART_KEYPAD_H */
