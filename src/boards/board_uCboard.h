/* ########################################################################

   PICsimLab - PIC laboratory simulator

   ########################################################################

   Copyright (c) : 2015-2020  Luis Claudio Gambôa Lopes

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

#ifndef BOARD_uCboard_H
#define BOARD_uCboard_H

#include <lxrad.h>

#include "bsim_ucsim.h"

#define BOARD_uCboard_Name "uCboard"

// new board class must be derived from board class defined in board.h
class cboard_uCboard : public bsim_ucsim {
private:
    lxBitmap* micbmp;
    lxFont font;

public:
    void SetScale(double scale) override;
    // Return the board name
    lxString GetName(void) override { return lxT(BOARD_uCboard_Name); };
    lxString GetAboutInfo(void) override { return lxT("L.C. Gamboa \n <lcgamboa@yahoo.com>"); };
    // Constructor called once on board creation
    cboard_uCboard(void);
    // Destructor called once on board destruction
    ~cboard_uCboard(void);
    // Called ever 100ms to draw board
    void Draw(CDraw* draw) override;
    void Run_CPU(void) override;
    int MInit(const char* processor, const char* fname, float freq) override;
    // Return a list of board supported microcontrollers
    lxString GetSupportedDevices(void) override { return lxT("C51,STM8S103,Z80,"); };
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

#endif /* BOARD_uCboard_H */
