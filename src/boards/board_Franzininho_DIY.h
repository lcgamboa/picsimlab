/* ########################################################################

   PICSimLab - Programmable IC Simulator Laboratory

   ########################################################################

   Copyright (c) : 2017-2024  Luis Claudio Gambôa Lopes <lcgamboa@yahoo.com>

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

#ifndef BOARD_Franzininho_DIY_H
#define BOARD_Franzininho_DIY_H

#include <lxrad.h>  //FIXME remove lxrad

#include "bsim_simavr.h"

#define BOARD_Franzininho_DIY_Name "Franzininho DIY"

// new board class must be derived from board class defined in board.h
class cboard_Franzininho_DIY : public bsim_simavr {
private:
    void RegisterRemoteControl(void) override;

public:
    // Return the board name
    std::string GetName(void) override { return BOARD_Franzininho_DIY_Name; };
    std::string GetAboutInfo(void) override {
        return "Fábio Souza e Felipe Alvares\n <contato@franzininho.com.br>\n https://franzininho.com.br/";
    };
    // Constructor called once on board creation
    cboard_Franzininho_DIY(void);
    // Destructor called once on board destruction
    ~cboard_Franzininho_DIY(void);
    // Called ever 100ms to draw board
    void Draw(CDraw* draw) override;
    void Run_CPU(void) override;
    // Return a list of board supported microcontrollers
    std::string GetSupportedDevices(void) override { return "attiny85,"; };
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
    unsigned short GetInputId(char* name) override;
    // return the output ids numbers of names used in output map
    unsigned short GetOutputId(char* name) override;
};

#endif /* BOARD_Franzininho_DIY_H */
