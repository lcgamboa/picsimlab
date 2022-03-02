/* ########################################################################

   PICsimLab - PIC laboratory simulator

   ########################################################################

   Copyright (c) : 2017-2021  Luis Claudio Gambôa Lopes

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

#ifndef BOARD_Arduino_Uno_H
#define BOARD_Arduino_Uno_H

#include <lxrad.h>

#include "bsim_simavr.h"

#define BOARD_Arduino_Uno_Name "Arduino Uno"

// new board class must be derived from board class defined in board.h
class cboard_Arduino_Uno : public bsim_simavr {
private:
    // controls to be added in simulator window
    CGauge* gauge1;  // gauge to show mean value of 3
    CGauge* gauge2;  // gauge to show mean value of 5
    CGauge* gauge3;  // gauge to show mean value of 6
    CGauge* gauge4;  // gauge to show mean value of 9
    CGauge* gauge5;  // gauge to show mean value of 10
    CGauge* gauge6;  // gauge to show mean value of 11

    CLabel* label1;  // label of gauge 3
    CLabel* label2;  // label of gauge 5
    CLabel* label3;  // label of gauge 6
    CLabel* label4;  // label of gauge 9
    CLabel* label5;  // label of gauge 10
    CLabel* label6;  // label of gauge 11

    void RegisterRemoteControl(void) override;

public:
    lxString GetName(void) override { return lxT(BOARD_Arduino_Uno_Name); };
    lxString GetAboutInfo(void) override { return lxT("L.C. Gamboa \n <lcgamboa@yahoo.com>"); };
    // Constructor called once on board creation
    cboard_Arduino_Uno(void);
    // Destructor called once on board destruction
    ~cboard_Arduino_Uno(void);
    // Called ever 100ms to draw board
    void Draw(CDraw* draw) override;
    void Run_CPU(void) override;
    // Return a list of board supported microcontrollers
    lxString GetSupportedDevices(void) override { return lxT("atmega328p,"); };
    // Reset board status
    void Reset(void) override;
    // Event on the board
    void EvMouseButtonPress(uint button, uint x, uint y, uint state) override;
    // Event on the board
    void EvMouseButtonRelease(uint button, uint x, uint y, uint state) override;
    // Event on the board
    void EvKeyPress(uint key, uint mask) override;
    // Event on the board
    void EvKeyRelease(uint key, uint mask) override;
    // Called ever 1s to refresh status
    void RefreshStatus(void) override;
    // Called to save board preferences in configuration file
    void WritePreferences(void) override;
    // Called whe configuration file load  preferences
    void ReadPreferences(char* name, char* value) override;
    // return the input ids numbers of names used in input map
    unsigned short get_in_id(char* name) override;
    // return the output ids numbers of names used in output map
    unsigned short get_out_id(char* name) override;
};

#endif /* BOARD_Arduino_Uno_H */
