/* ########################################################################

   PICSimLab - Programmable IC Simulator Laboratory

   ########################################################################

   Copyright (c) : 2010-2024  Luis Claudio Gambôa Lopes <lcgamboa@yahoo.com>

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

#ifndef BOARD_PICGenios_H
#define BOARD_PICGenios_H

#include <lxrad.h>  //FIXME remove lxrad

#include "../devices/lcd_hd44780.h"
#include "../devices/mi2c_24CXXX.h"
#include "../devices/rtc_ds1307.h"
#include "../devices/swbounce.h"
#include "bsim_picsim.h"

#define BOARD_PICGenios_Name "PICGenios"

class cboard_PICGenios : public bsim_picsim {
private:
    unsigned char p_BT[7];

    unsigned char p_KEY[12];

    unsigned char pot[2];
    unsigned char active[2];

    int vtc;
    int vt;

    lcd_t lcd;

    mi2c_t mi2c;
    rtc_ds1307_t rtc2;

    int lcde;

    int sound_on;

    float vp1in;
    float vp2in;
    float vp2[2];
    float temp[2];
    float ref;

    int rpmstp;
    int rpmc;

    unsigned char d;
    unsigned char sda, sck;

    int jmp[1];
    int dip[20];

    unsigned int lm1[40];  // 7s display mean value
    unsigned int lm2[40];
    unsigned int lm3[40];
    unsigned int lm4[40];

    lxBitmap* vent[2];
    lxBitmap* lcdbmp[2];

    lxaudio buzzer;

    char mi2c_tmp_name[200];

    void RegisterRemoteControl(void) override;
    SWBounce_t bounce;

    int heater_pwr;
    int cooler_pwr;

    int TimerID;

public:
    // Return the board name
    std::string GetName(void) override { return BOARD_PICGenios_Name; };
    std::string GetAboutInfo(void) override { return "L.C. Gamboa \n <lcgamboa@yahoo.com>"; };
    cboard_PICGenios(void);
    ~cboard_PICGenios(void);
    void Draw(CCanvas* Canvas) override;
    void Run_CPU(void) override;
    std::string GetSupportedDevices(void) override {
        return "PIC16F1789,PIC16F1939,PIC16F777,PIC16F877A,PIC16F887,PIC18F452,PIC18F4520,PIC18F4550,PIC18F45K50,"
               "PIC18F4580,PIC18F4620,PIC18F47K40,";
    };
    int MInit(const char* processor, const char* fname, float freq) override;
    void Reset(void) override;
    int MDumpMemory(const char* mfname) override;
    void EvMouseMove(uint button, uint x, uint y, uint state) override;
    void EvMouseButtonPress(uint button, uint x, uint y, uint state) override;
    void EvMouseButtonRelease(uint button, uint x, uint y, uint state) override;
    void EvKeyPress(uint key, uint mask) override;
    void EvKeyRelease(uint key, uint mask) override;
    void EvOnShow(void) override;
    void RefreshStatus(void) override;
    void WritePreferences(void) override;
    void ReadPreferences(char* name, char* value) override;
    unsigned short GetInputId(char* name) override;
    unsigned short GetOutputId(char* name) override;
    void board_Event(CControl* control) override;
    void SetScale(double scale) override;
    void OnTime(void);
};

#endif /* BOARD_4_H */
