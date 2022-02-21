/* ########################################################################

   PICsimLab - PIC laboratory simulator

   ########################################################################

   Copyright (c) : 2010-2022  Luis Claudio Gambôa Lopes

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

#ifndef BOARD_McLab2_H
#define	BOARD_McLab2_H

#include "bsim_picsim.h"
#include"../devices/swbounce.h"

#define	BOARD_McLab2_Name "McLab2"

class cboard_McLab2:public bsim_picsim
{
   private:
     unsigned char p_BT[4]; 
     
     unsigned char pot1;
     unsigned char active;
     
     lcd_t lcd;

     mi2c_t mi2c;

     int vtc;
     int vt;
     
     int lcde;

     int sound_on;


     float vp2in;
     float vp2[2];
     float temp[2];
     float ref;

     int rpmstp;
     int rpmc;

     unsigned char d;
     unsigned char sda,sck;
 
    unsigned char jmp[8];
    unsigned int lm1[40]; //luminosidade media display
    unsigned int lm2[40]; //luminosidade media display
    unsigned int lm3[40]; //luminosidade media display
    unsigned int lm4[40]; //luminosidade media display     
    
    CGauge *gauge1;
    CGauge *gauge2;
    CLabel *label2;
    CLabel *label3;
    CLabel *label4;

    lxaudio buzzer;
    
    lxBitmap * vent[2];
    
    char mi2c_tmp_name[200]; 
    
    void RegisterRemoteControl(void) override;  
    lxColor color1;
    lxColor color2;
    lxFont font;
    SWBounce_t bounce;
  public:
      //Return the board name
      lxString GetName(void) override {return lxT(BOARD_McLab2_Name); };
      lxString GetAboutInfo(void) override {return lxT("L.C. Gamboa \n <lcgamboa@yahoo.com>");};
      cboard_McLab2(void);
      ~cboard_McLab2(void);
      void Draw(CDraw *draw) override;
      void Run_CPU(void) override;
      lxString GetSupportedDevices(void) override {return lxT("PIC16F1789,PIC16F1939,PIC16F777,PIC16F877A,PIC16F887,PIC18F452,PIC18F4520,PIC18F4550,PIC18F45K50,PIC18F4620,PIC18F47K40,");};
      void Reset(void) override;
      void MDumpMemory(const char * mfname) override;
      void EvMouseButtonPress(uint button, uint x, uint y,uint state) override;
      void EvMouseButtonRelease(uint button, uint x, uint y,uint state) override;
      void EvMouseMove(uint button, uint x, uint y, uint state) override;
      void EvKeyPress(uint key, uint mask) override;
      void EvKeyRelease(uint key, uint mask) override;
      void EvOnShow(void) override;
      void RefreshStatus(void) override;
      void WritePreferences(void) override;
      void ReadPreferences(char *name,char *value) override;
      void SetScale(double scale) override;
      unsigned short get_in_id(char * name) override;
      unsigned short get_out_id(char * name) override;
};


#endif	/* BOARD_McLab2_H */

