/* ########################################################################

   PICSimLab - Programmable IC Simulator Laboratory

   ########################################################################

   Copyright (c) : 2010-2026 Luis Claudio Gambôa Lopes <lcgamboa@yahoo.com>

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

#ifndef BOARD_GPSIM_H
#define BOARD_GPSIM_H

#include "../lib/board.h"
#include "../lib/serial_port.h"

class bsim_gpsim : virtual public board {
public:
    bsim_gpsim(void);
    int DebugInit(int dtyppe) override;
    std::string GetDebugName(void) override { return "none"; };
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
    void MSetPinOAV(int pin, float value) override;
    void MSetAPin(int pin, float value) override;
    unsigned char MGetPin(int pin) override;
    const picpin* MGetPinsValues(void) override;
    float* MGetPinOAVPtr(int pin) override;
    void MStep(void) override;
    void MStepResume(void) override;
    int MReset(int flags) override;
    int MGetResetPin(void) override;
    int MGetIOUpdated(void) override;
    void MClearIOUpdated(void) override;
    int GetDefaultClock(void) override { return 8; };
    // Called ever 1s to refresh status
    void RefreshStatus(void) override;

protected:
    void pins_reset(void);
    picpin pins[MAX_PIN_COUNT];
    unsigned int serialbaud;
    float serialexbaud;
    float freq;
#ifdef _WIN_
    HANDLE serialfd;
#else
    int serialfd;
#endif
    std::string supported_devices;
};

#endif /* BOARD_GPSIM_H */
