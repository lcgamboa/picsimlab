/* ########################################################################

   PICSimLab - Programmable IC Simulator Laboratory

   ########################################################################

   Copyright (c) : 2015-2023  Luis Claudio Gambôa Lopes <lcgamboa@yahoo.com>

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

// include files
#include "board_Arduino_Nano.h"

#include "../lib/picsimlab.h"

// Constructor called once on board creation

cboard_Arduino_Nano::cboard_Arduino_Nano(void) {
    pkg = QFN;

    LED_pin = 16;

    PWM_pins[0] = 0;
    PWM_pins[1] = 8;
    PWM_pins[2] = 9;
    PWM_pins[3] = 12;
    PWM_pins[4] = 13;
    PWM_pins[5] = 14;

    ReadMaps();
}

// Called to save board preferences in configuration file

void cboard_Arduino_Nano::WritePreferences(void) {
    // write selected microcontroller of board_x to preferences
    PICSimLab.SavePrefs(lxT("Arduino_Nano_proc"), Proc);
    PICSimLab.SavePrefs(lxT("Arduino_Nano_clock"), lxString().Format("%2.1f", PICSimLab.GetClock()));
}

// Called whe configuration file load  preferences

void cboard_Arduino_Nano::ReadPreferences(char* name, char* value) {
    // read microcontroller of preferences
    if (!strcmp(name, "Arduino_Nano_proc")) {
        Proc = value;
    }

    if (!strcmp(name, "Arduino_Nano_clock")) {
        PICSimLab.SetClock(atof(value));
    }
}

// TODO cboard_Arduino_Nano: add suport to analog inputs A6 and A7

board_init(BOARD_Arduino_Nano_Name, cboard_Arduino_Nano);
