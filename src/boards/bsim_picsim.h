/* ########################################################################

   PICsimLab - PIC laboratory simulator

   ########################################################################

   Copyright (c) : 2010-2017  Luis Claudio Gambôa Lopes

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

#ifndef BOARD_PIC_H
#define BOARD_PIC_H

#include "board.h"

#include "../devices/mplabxd.h"

class bsim_picsim : virtual public board {
public:
    bsim_picsim(void);
    int DebugInit(int dtyppe) override;
    lxString GetDebugName(void) override { return "MDB"; };
    void DebugLoop(void) override;
    int CpuInitialized(void) override;
    void MSetSerial(const char* port) override;
    int MInit(const char* processor, const char* fname, float freq) override;
    void MEnd(void) override;
    int MGetArchitecture(void) override;
    void MDumpMemory(const char* fname) override;
    void MEraseFlash(void) override;
    void MSetFreq(float freq) override;
    float MGetFreq(void) override;
    void MSetVCC(float vcc) override;
    float MGetVCC(void) override;
    float MGetInstClockFreq(void) override;
    int MGetPinCount(void) override;
    lxString MGetPinName(int pin) override;
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
    unsigned int DBGGetRAMLAWR(void) override;
    unsigned int DBGGetRAMLARD(void) override;
    void EndServers(void) override;

protected:
    _pic pic;
};

#endif /* BOARD_PIC_H */
