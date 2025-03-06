/* ########################################################################

   PICSimLab - Programmable IC Simulator Laboratory

   ########################################################################

   Copyright (c) : 2015-2024  Luis Claudio Gambôa Lopes <lcgamboa@yahoo.com>

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

#ifndef BOARD_Breadboard_H
#define BOARD_Breadboard_H

#include "../sim_backend/bsim_picsim.h"
#include "../sim_backend/bsim_simavr.h"

#define BOARD_Breadboard_Name "Breadboard"

#define _PIC 0
#define _AVR 1

// new board class must be derived from board class defined in board.h
class cboard_Breadboard : public bsim_picsim, public bsim_simavr {
private:
    int ptype;
    int micbmp;
    unsigned char jmp[1];  // jumper
public:
    void SetScale(double scale) override;
    // Return the board name
    std::string GetName(void) override { return BOARD_Breadboard_Name; };
    std::string GetAboutInfo(void) override { return "L.C. Gamboa \n <lcgamboa@yahoo.com>"; };
    int DebugInit(int dtyppe) override;
    std::string GetDebugName(void) override;
    void DebugLoop(void) override;
    int CpuInitialized(void) override;
    void MSetSerial(const char* port) override;
    int MInit(const char* processor, const char* fname, float freq) override;
    void MEnd(void) override;
    int MGetArchitecture(void) override;
    int MDumpMemory(const char* fname) override;
    void MEraseFlash(void) override;
    void MSetFreq(float freq) override;
    float MGetFreq(void) override;
    void MSetVCC(float vcc) override;
    float MGetVCC(void) override;
    float MGetInstClockFreq(void) override;
    int MGetPinCount(void) override;
    std::string MGetPinName(int pin) override;
    void MSetPin(int pin, unsigned char value) override;
    void MSetPinDOV(int pin, unsigned char ovalue) override;
    void MSetAPin(int pin, float value) override;
    unsigned char MGetPin(int pin) override;
    const picpin* MGetPinsValues(void) override;
    void MStep(void) override;
    void MStepResume(void) override;
    void MReset(int flags) override;
    unsigned short* DBGGetProcID_p(void) override;
    unsigned int DBGGetPC(void) override;
    void DBGSetPC(unsigned int pc) override;
    unsigned char* DBGGetRAM_p(void) override;
    unsigned char* DBGGetROM_p(void) override;
    unsigned char* DBGGetCONFIG_p(void) override;
    unsigned char* DBGGetID_p(void) override;
    unsigned char* DBGGetEEPROM_p(void) override;
    unsigned int DBGGetRAMSize(void) override;
    unsigned int DBGGetROMSize(void) override;
    unsigned int DBGGetCONFIGSize(void) override;
    unsigned int DBGGetIDSize(void) override;
    unsigned int DBGGetEEPROM_Size(void) override;
    int GetUARTRX(const int uart_num) override;
    int GetUARTTX(const int uart_num) override;

    // Constructor called once on board creation
    cboard_Breadboard(void);
    // Destructor called once on board destruction
    ~cboard_Breadboard(void);
    // Called ever 100ms to draw board
    void Draw(void) override;
    void Run_CPU(void) override;
    // Return a list of board supported microcontrollers
    std::string GetSupportedDevices(void) override {
        return "atmega328p,atmega2560,attiny85,PIC16F1516,PIC16F18324,PIC16F18855,PIC16F1619,PIC16F1788,PIC16F1789,"
               "PIC16F1827,PIC16F1829,PIC16F1847,PIC16F1939,PIC16F1947,PIC16F628A,PIC16F648A,PIC16F688,PIC16F84A,"
               "PIC16F777,PIC16F819,PIC16F877A,PIC16F886,PIC16F887,PIC18F24Q10,PIC18F26K80,PIC18F27K40,PIC18F452,"
               "PIC18F4520,PIC18F4550,PIC18F45K50,PIC18F4580,PIC18F4620,PIC18F46J50,PIC18F47K40,PIC18F67J60,"
               "PIC18F67J94,";
    };
    // Reset board status
    void Reset(void) override;
    // Event on the board
    void EvMouseButtonPress(unsigned int button, unsigned int x, unsigned int y, unsigned int state) override;
    // Event on the board
    void EvMouseButtonRelease(unsigned int button, unsigned int x, unsigned int y, unsigned int state) override;
    // Event on the board
    void EvKeyPress(unsigned int key, unsigned int mask) override;
    // Event on the board
    void EvKeyRelease(unsigned int key, unsigned int mask) override;
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
    void EndServers(void) override;
    int GetDefaultClock(void) override;
};

#endif /* BOARD_Breadboard_H */
