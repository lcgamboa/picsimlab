/* ########################################################################

   PICsimLab - PIC laboratory simulator

   ########################################################################

   Copyright (c) : 2010-2021  Luis Claudio Gambôa Lopes

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
#define	BOARD_PICGenios_H

#include "bsim_picsim.h"


class cboard_PICGenios:public bsim_picsim
{
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
    unsigned char sda,sck;

    int jmp[1];
    int dip[20];

    unsigned int lm1[40]; //7s display mean value
    unsigned int lm2[40];
    unsigned int lm3[40];
    unsigned int lm4[40];
    
    lxBitmap * vent[2];
    lxBitmap * lcdbmp[2];
    
    CGauge *gauge1;
    CGauge *gauge2;

    CLabel *label3;
    CLabel *label4;
    CLabel *label5;
    CLabel *label6;
    CCombo *combo1; 
    
    lxaudio buzzer;
    
    char mi2c_tmp_name[200]; 
    
    void RegisterRemoteControl(void);  
    lxColor color1;
    lxColor color2;    
    lxFont font;
  public:
      lxString GetAboutInfo(void){return lxT("L.C. Gamboa \n <lcgamboa@yahoo.com>");};
      cboard_PICGenios(void);
      ~cboard_PICGenios(void);
      void Draw(CDraw *draw);
      void Run_CPU(void);
      lxString GetSupportedDevices(void){return lxT("PIC16F1789,PIC16F1939,PIC16F777,PIC16F877A,PIC18F452,PIC18F4520,PIC18F4550,PIC18F45K50,PIC18F4620,PIC18F47K40,");};
      lxString GetPictureFileName(void){return lxT("PICGenios/board.svg");};
      lxString GetInputMapFile(void){return lxT("PICGenios/input.map");};
      lxString GetOutputMapFile(void){return lxT("PICGenios/output.map");};
      void Reset(void);
      void MDumpMemory(const char * mfname);
      void EvMouseMove(uint button, uint x, uint y, uint state);
      void EvMouseButtonPress(uint button, uint x, uint y,uint state);
      void EvMouseButtonRelease(uint button, uint x, uint y,uint state);
      void EvKeyPress(uint key, uint mask);
      void EvKeyRelease(uint key, uint mask);
      void EvOnShow(void);
      void RefreshStatus(void);
      void WritePreferences(void);
      void ReadPreferences(char *name,char *value);
      unsigned short get_in_id(char * name);
      unsigned short get_out_id(char * name);
      void board_Event(CControl * control);
};


#endif	/* BOARD_4_H */

