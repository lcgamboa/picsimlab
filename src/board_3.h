/* ########################################################################

   PICsimLab - PIC laboratory simulator

   ########################################################################

   Copyright (c) : 2010-2015  Luis Claudio Gambôa Lopes

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

#ifndef BOARD_3_H
#define	BOARD_3_H

#include "board_pic.h"

class cboard_3:public board_pic
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
     wxSound buzz;


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

    
  public:
      cboard_3(void);
      ~cboard_3(void);
      void Draw(CDraw *draw,double scale);
      void Run_CPU(void);
      String GetSupportedDevices(void){return wxT("PIC16F777,PIC16F877A,PIC18F452,PIC18F4520,PIC18F4550,PIC18F4620,");};
      String GetPictureFileName(void){return wxT("picsimlab3.png");};
      String GetInputMapFile(void){return wxT("input3.map");};
      String GetOutputMapFile(void){return wxT("output3.map");};
      void Reset(void);
      void MouseButtonPress(uint button, uint x, uint y,uint state);
      void MouseButtonRelease(uint button, uint x, uint y,uint state);
      void KeyPress(uint key, uint x, uint y,uint mask);
      void KeyRelease(uint key, uint x, uint y,uint mask);
      void OnShow(void);
      void RefreshStatus(void);
      void WritePreferences(void);
      void ReadPreferences(char *name,char *value);
      unsigned short get_in_id(char * name);
      unsigned short get_out_id(char * name);
};


#endif	/* BOARD_1_H */

