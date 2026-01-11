/* ########################################################################

   PICSimLab - Programmable IC Simulator Laboratory

   ########################################################################

   Copyright (c) : 2020       El-khadraouy Mohammed <mohammed.el-khadraouy@ecole.ensicaen.fr>
   Copyright (c) : 2015-2026 Luis Claudio Gambôa Lopes <lcgamboa@yahoo.com>

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

#ifndef BOARD_Curiosity_HPC_H
#define BOARD_Curiosity_HPC_H

#include "../sim_backend/bsim_picsim.h"

#define BOARD_Curiosity_HPC_Name "Curiosity HPC"

// New board Class derived from board class defined in board.h
class cboard_Curiosity_HPC : public bsim_picsim {
private:
    unsigned char p_BT[2];  // Button S1 and S2

    unsigned char pot1;
    unsigned char active;

    unsigned char jmp[1];  // jumper

    void RegisterRemoteControl(void) override;

    unsigned char ic28pins;

public:
    // Return the board name
    std::string GetName(void) override { return BOARD_Curiosity_HPC_Name; };
    std::string GetAboutInfo(void) override {
        return "El-khadraouy Mohammed\n<mohammed.el-khadraouy@ecole.ensicaen.fr>\nand L.C. Gamboa \n "
               "<lcgamboa@yahoo.com>";
    };
    // Constructor called once on board creation
    cboard_Curiosity_HPC(void);
    // Destructor called once on board destruction
    ~cboard_Curiosity_HPC(void);
    // Called ever 100ms to draw board
    void Draw(void) override;
    void Run_CPU(void) override;
    // Return a list of board supported microcontrollers
    std::string GetSupportedDevices(void) override { return "PIC18F47K40,"; };
    // Reset board status
    void Reset(void) override;
    // Event on the board
    void EvMouseButtonPress(unsigned int button, unsigned int x, unsigned int y, unsigned int state) override;
    // Event on the board
    void EvMouseButtonRelease(unsigned int button, unsigned int x, unsigned int y, unsigned int state) override;
    // Event on the board
    void EvMouseMove(unsigned int button, unsigned int x, unsigned int y, unsigned int state) override;
    // Event on the board
    void EvKeyPress(unsigned int key, unsigned int mask) override;
    // Event on the board
    void EvKeyRelease(unsigned int key, unsigned int mask) override;
    // Called to save board preferences in configuration file
    void WritePreferences(void) override;
    // Called whe configuration file load  preferences
    void ReadPreferences(char* name, char* value) override;
    // return the input ids numbers of names used in input map
    unsigned short GetInputId(char* name) override;
    // return the output ids numbers of names used in output map
    unsigned short GetOutputId(char* name) override;
    // Set Serial port name
    void MSetSerial(const char* port) override;
};

#endif /* BOARD_Curiosity_HPC_H */