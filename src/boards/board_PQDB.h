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

#ifndef BOARD_PQDB_H
#define BOARD_PQDB_H

#include "../sim_backend/bsim_picsim.h"

#include "../devices/io_74xx595.h"
#include "../devices/lcd_hd44780.h"
#include "../devices/rtc_ds1307.h"

#define BOARD_PQDB_Name "PQDB"

// Pin definitions
// Arduino style
/*
#define SDA_PIN      18
#define SCL_PIN      19
#define KEYPAD_1_PIN 13
#define KEYPAD_2_PIN 12
#define SO_CLK_PIN   11
#define SO_EN_PIN    10
#define PWM_PIN       9
#define SO_DATA_PIN   8
#define LCD_RS_PIN    7
#define LCD_EN_PIN    6
#define DISP_4_PIN    5
#define LED_BLUE_PIN  4
#define DISP_3_PIN    4
#define LED_GREEN_PIN 3
#define DISP_2_PIN    3
#define LED_RED_PIN   2
#define DISP_1_PIN    2
#define TX_PIN        1
#define RX_PIN        0
 */

// PIC18f4550 style
// zero index, pin 1 ->0; pin 18 -> 17

#define RA0 1
#define RA1 2
#define RA2 3

#define RB0 32
#define RB1 33
#define RB2 34
#define RB3 35
#define RB4 36
#define RB5 37
#define RB6 38
#define RB7 39

#define RC1 15

#define RD0 18
#define RD1 19
#define RD2 20
#define RD3 21
#define RD4 26
#define RD5 27
#define RD6 28
#define RD7 29

#define PSRD0 40
#define PSRD1 41
#define PSRD2 42
#define PSRD3 43
#define PSRD4 44
#define PSRD5 45
#define PSRD6 46
#define PSRD7 47

#define SRD0 0x01
#define SRD1 0x02
#define SRD2 0x04
#define SRD3 0x08
#define SRD4 0x10
#define SRD5 0x20
#define SRD6 0x40
#define SRD7 0x80

#define SDA_PIN RB4
#define SCL_PIN RB3
#define KEYPAD_1_PIN RB2
#define KEYPAD_2_PIN RB1
#define SO_CLK_PIN RB0
#define SO_EN_PIN RD7
#define PWM_PIN RC1
#define SO_DATA_PIN RD6
#define LCD_RS_PIN RD5
#define LCD_EN_PIN RD4
#define DISP_4_PIN RD3
#define DISP_3_PIN RD2
#define DISP_2_PIN RD1
#define DISP_1_PIN RD0
#define TX_PIN 1
#define RX_PIN 0

#define LED_BLUE_PIN DISP_3_PIN
#define LED_GREEN_PIN DISP_2_PIN
#define LED_RED_PIN DISP_1_PIN

#define AN2_PIN RA2
#define AN1_PIN RA1
#define AN0_PIN RA0

#define LM_PIN AN2_PIN
#define LDR_PIN AN1_PIN
#define POT_PIN AN0_PIN

class cboard_PQDB : public bsim_picsim {
private:
    unsigned char p_KEY[10];

    unsigned char pot;
    unsigned char active;

    int vtc;
    int vt;

    // external peripherals
    lcd_t lcd;
    unsigned char d;

    rtc_ds1307_t rtc2;
    unsigned char sda, sck;

    io_74xx595_t shiftReg;
    unsigned char srDATA, srCLK, srLAT;
    unsigned long shiftReg_alm[8];
    unsigned short _srret;

    int lcde;

    int sound_on;

    float vPOT;
    float vLDR;
    float vLM;

    float vp2[2];
    float temp[2];
    float ref;

    int rpmstp;
    int rpmc;

    int lm7seg[32];  // luminosidade media display

    int buzzerId;

    void RegisterRemoteControl(void) override;

    unsigned char scroll1_old;
    unsigned char scroll2_old;

public:
    // Return the board name
    std::string GetName(void) override { return BOARD_PQDB_Name; };
    std::string GetAboutInfo(void) override { return "R.M.A. Almeida	  \n <rodrigomax@unifei.edu.br>"; };
    cboard_PQDB(void);
    ~cboard_PQDB(void);
    void Draw(void) override;
    void Run_CPU(void) override;

    std::string GetSupportedDevices(void) override { return "PIC18F4520,PIC18F4550,PIC18F4620,"; };

    void Reset(void) override;
    void EvMouseMove(unsigned int button, unsigned int x, unsigned int y, unsigned int state) override;
    void EvMouseButtonPress(unsigned int button, unsigned int x, unsigned int y, unsigned int state) override;
    void EvMouseButtonRelease(unsigned int button, unsigned int x, unsigned int y, unsigned int state) override;
    void EvKeyPress(unsigned int key, unsigned int mask) override;
    void EvKeyRelease(unsigned int key, unsigned int mask) override;
    void EvOnShow(void) override;
    void WritePreferences(void) override;
    void ReadPreferences(char* name, char* value) override;
    unsigned short GetInputId(char* name) override;
    unsigned short GetOutputId(char* name) override;
    std::string MGetPinName(int pin) override;
    int MGetPinCount(void) override;
};

#endif /* BOARD_4_H */
