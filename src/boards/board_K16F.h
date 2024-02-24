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

#ifndef BOARD_K16F_H
#define BOARD_K16F_H

#include "bsim_picsim.h"

#include "../devices/lcd_hd44780.h"
#include "../devices/mi2c_24CXXX.h"
#include "../devices/rtc_pfc8563.h"

#define BOARD_K16F_Name "K16F"

class cboard_K16F : public bsim_picsim {
private:
    unsigned char p_KEY[12];

    lcd_t lcd;

    mi2c_t mi2c;
    rtc_pfc8563_t rtc;

    int lcde;

    unsigned char clko;
    unsigned char d;
    unsigned char sda, sck;
    char mi2c_tmp_name[200];

    void RegisterRemoteControl(void) override;

public:
    // Return the board name
    std::string GetName(void) override { return BOARD_K16F_Name; };
    std::string GetAboutInfo(void) override { return "L.C. Gamboa \n <lcgamboa@yahoo.com>"; };
    cboard_K16F(void);
    ~cboard_K16F(void);
    void Draw(void) override;
    void Run_CPU(void) override;
    std::string GetSupportedDevices(void) override { return "PIC16F628A,PIC16F648A,PIC16F84A,"; };
    int MInit(const char* processor, const char* fname, float freq) override;
    void Reset(void) override;
    int MDumpMemory(const char* mfname) override;
    void EvMouseButtonPress(unsigned int button, unsigned int x, unsigned int y, unsigned int state) override;
    void EvMouseButtonRelease(unsigned int button, unsigned int x, unsigned int y, unsigned int state) override;
    void EvKeyPress(unsigned int key, unsigned int mask) override;
    void EvKeyRelease(unsigned int key, unsigned int mask) override;
    void EvOnShow(void) override;
    void RefreshStatus(void) override;
    void WritePreferences(void) override;
    void ReadPreferences(char* name, char* value) override;
    unsigned short GetInputId(char* name) override;
    unsigned short GetOutputId(char* name) override;
};

#endif /* BOARD_K16F_H */
