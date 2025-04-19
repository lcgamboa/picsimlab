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

#ifndef PART_SERVO_H
#define PART_SERVO_H

#include "../lib/part.h"

#define PART_SERVO_Name "Servo Motor"

/**
 * @brief servo motor part class
 *
 * class definition of servo motor class.
 */
class cpart_servo : public part {
public:
    std::string GetAboutInfo(void) override { return "L.C. Gamboa \n <lcgamboa@yahoo.com>"; };

    /**
     * @brief constructor called once on part creation
     *
     */
    cpart_servo(const unsigned x, const unsigned y, const char* name, const char* type, board* pboard_, const int id_);

    /**
     * @brief destructor called once on part destruction
     *
     */
    ~cpart_servo(void);

    void DrawOutput(const unsigned int index) override;
    void Process(void) override;
    void PostProcess(void) override;
    void ConfigurePropertiesWindow(void) override;
    void ReadPropertiesWindow(void) override;
    std::string WritePreferences(void) override;
    void ReadPreferences(std::string value) override;
    void LoadPartImage(void) override;
    unsigned short GetInputId(char* name) override;
    unsigned short GetOutputId(char* name) override;

private:
    void RegisterRemoteControl(void) override;
    unsigned char input_pin;  ///< pulse input pin
    int BackGround;           ///< Background image
    float angle;              ///< angle of shaft
    float angle_;             ///< old angle of shaft
    unsigned char in_[2];     ///< input pin memory
    int time;                 ///< pulse time
};

#endif /* PART_SERVO */
