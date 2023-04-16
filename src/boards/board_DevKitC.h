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

#ifndef BOARD_DevKitC_H
#define BOARD_DevKitC_H

#include <lxrad.h>

#include "bsim_qemu.h"

#define BOARD_DevKitC_Name "ESP32-DevKitC"

// new board class must be derived from board class defined in board.h
class cboard_DevKitC : public bsim_qemu {
private:
    CLabel* label1;
    CCombo* combo1;
    CButton* button1;
    unsigned char p_BOOT;
    void RegisterRemoteControl(void) override;
    CPWindow* wconfig;
    int ConfEnableWifi;
    int ConfDisableWdt;
    int ConfEnableEthernet;

protected:
    void BoardOptions(int* argc, char** argv) override;

public:
    // Return the board name
    lxString GetName(void) override { return lxT(BOARD_DevKitC_Name); };
    lxString GetAboutInfo(void) override { return lxT("L.C. Gamboa \n <lcgamboa@yahoo.com>"); };
    // Constructor called once on board creation
    cboard_DevKitC(void);
    // Destructor called once on board destruction
    ~cboard_DevKitC(void);
    // Called ever 100ms to draw board
    void Draw(CDraw* draw) override;
    void Run_CPU(void) override;
    void Run_CPU_ns(uint64_t time) override;
    // Return a list of board supported microcontrollers
    lxString GetSupportedDevices(void) override { return lxT("ESP32,"); };
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
    // board combo events
    void board_Event(CControl* control) override;
    void board_ButtonEvent(CControl* control, uint button, uint x, uint y, uint state) override;
    void MSetAPin(int pin, float value) override;
    lxString MGetPinName(int pin) override;
    int MGetPinCount(void) override;
    void PinsExtraConfig(int cfg) override;
};

#endif /* BOARD_DevKitC_H */
