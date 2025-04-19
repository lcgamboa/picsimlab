/* ########################################################################

   PICSimLab - Programmable IC Simulator Laboratory

   ########################################################################

   Copyright (c) : 2017-2025  Luis Claudio Gambôa Lopes <lcgamboa@yahoo.com>

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

#ifndef BOARD_Arduino_Mega_H
#define BOARD_Arduino_Mega_H

#include "board_Arduino_Uno.h"

#define BOARD_Arduino_Mega_Name "Arduino Mega"

// new board class must be derived from board class defined in board.h
class cboard_Arduino_Mega : public cboard_Arduino_Uno {
public:
    std::string GetName(void) override { return BOARD_Arduino_Mega_Name; };
    std::string GetAboutInfo(void) override { return "L.C. Gamboa \n <lcgamboa@yahoo.com>"; };
    // Constructor called once on board creation
    cboard_Arduino_Mega(void);
    std::string GetSupportedDevices(void) override { return "atmega2560,"; };
    // Called to save board preferences in configuration file
    void WritePreferences(void) override;
    // Called whe configuration file load  preferences
    void ReadPreferences(char* name, char* value) override;
};

#endif /* BOARD_Arduino_Mega_H */
