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
#include "board_Arduino_Mega.h"

#include "../lib/picsimlab.h"

// Constructor called once on board creation

cboard_Arduino_Mega::cboard_Arduino_Mega(void) {
    Proc = "atmega2560";
    LED_pin = 25;

    PWM_pins[0] = 6;
    PWM_pins[1] = 4;
    PWM_pins[2] = 14;
    PWM_pins[3] = 17;
    PWM_pins[4] = 22;
    PWM_pins[5] = 23;

    ReadMaps();
}

// Called to save board preferences in configuration file

void cboard_Arduino_Mega::WritePreferences(void) {
    // write selected microcontroller of board_x to preferences
    PICSimLab.SavePrefs("Arduino_Mega_proc", Proc);
    PICSimLab.SavePrefs("Arduino_Mega_clock", FloatStrFormat("%2.1f", PICSimLab.GetClock()));
}

// Called whe configuration file load  preferences

void cboard_Arduino_Mega::ReadPreferences(char* name, char* value) {
    // read microcontroller of preferences
    if (!strcmp(name, "Arduino_Mega_proc")) {
        Proc = value;
    }

    if (!strcmp(name, "Arduino_Mega_clock")) {
        PICSimLab.SetClock(atof(value));
    }
}

// TODO cboard_Arduino_Mega: add suport to analog inputs A6 and A7

board_init(BOARD_Arduino_Mega_Name, cboard_Arduino_Mega);
