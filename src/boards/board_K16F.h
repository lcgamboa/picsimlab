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

#ifndef BOARD_K16F_H
#define	BOARD_K16F_H

#include "bsim_picsim.h"

#define	BOARD_K16F_Name "K16F"

class cboard_K16F:public bsim_picsim
{
  private:
      
    unsigned char p_KEY1; 
    unsigned char p_KEY2; 
    unsigned char p_KEY3; 
    unsigned char p_KEY4; 
    unsigned char p_KEY5; 
    unsigned char p_KEY6; 
    unsigned char p_KEY7; 
    unsigned char p_KEY8; 
    unsigned char p_KEY9; 
    unsigned char p_KEY10; 
    unsigned char p_KEY11; 
    unsigned char p_KEY12; 

     lcd_t lcd;

     mi2c_t mi2c;
     rtc_pfc8563_t rtc;
     
     int lcde;

     unsigned char clko;
     unsigned char d;
     unsigned char sda,sck;
     char mi2c_tmp_name[200];  
     
     void RegisterRemoteControl(void);    
     lxColor color1;
     lxColor color2;
     lxFont font;
  public:
      //Return the board name
      lxString GetName(void) {return lxT(BOARD_K16F_Name); };
      lxString GetAboutInfo(void){return lxT("L.C. Gamboa \n <lcgamboa@yahoo.com>");};
      cboard_K16F(void);
      ~cboard_K16F(void);
      void Draw(CDraw *draw);
      void Run_CPU(void);
      lxString GetSupportedDevices(void){return lxT("PIC16F628A,PIC16F648A,PIC16F84A,");};
      void Reset(void);
      void MDumpMemory(const char * mfname);
      void EvMouseButtonPress(uint button, uint x, uint y,uint state);
      void EvMouseButtonRelease(uint button, uint x, uint y,uint state);
      void EvKeyPress(uint key, uint mask);
      void EvKeyRelease(uint key,uint mask);      
      void EvOnShow(void);
      void RefreshStatus(void);
      void WritePreferences(void);
      void ReadPreferences(char *name,char *value);
      unsigned short get_in_id(char * name);
      unsigned short get_out_id(char * name);
};



#endif	/* BOARD_K16F_H */

