/* ########################################################################

   PICsimLab - PIC laboratory simulator

   ########################################################################

   Copyright (c) : 2010-2020  Luis Claudio Gambôa Lopes

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

#include "board_picsim.h"

class cboard_McLab2:public board_picsim
{
   private:
     int p_BT1; 
     int p_BT2;  
     int p_BT3; 
     int p_BT4; 
     lcd_t lcd;

     mi2c_t mi2c;

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
 
    int jmp[8];
    unsigned int lm1[40]; //luminosidade media display
    unsigned int lm2[40]; //luminosidade media display
    unsigned int lm3[40]; //luminosidade media display
    unsigned int lm4[40]; //luminosidade media display     
    
    CGauge *gauge1;
    CGauge *gauge2;
    CScroll *scroll1;
    CLabel *label1;
    CLabel *label2;
    CLabel *label3;
    CLabel *label4;

    lxaudio buzzer;
    
    char mi2c_tmp_name[200]; 
  public:
      lxString GetAboutInfo(void){return lxT("L.C. Gamboa \n <lcgamboa@yahoo.com>");};
      cboard_McLab2(void);
      ~cboard_McLab2(void);
      void Draw(CDraw *draw,double scale);
      void Run_CPU(void);
      lxString GetSupportedDevices(void){return lxT("PIC16F777,PIC16F877A,PIC18F452,PIC18F4520,PIC18F4550,PIC18F4620,");};
      lxString GetPictureFileName(void){return lxT("McLab2/board.png");};
      lxString GetInputMapFile(void){return lxT("McLab2/input.map");};
      lxString GetOutputMapFile(void){return lxT("McLab2/output.map");};
      void Reset(void);
      void MDumpMemory(const char * mfname);
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
};


#endif	/* BOARD_McLab2_H */

