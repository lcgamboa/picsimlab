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

#ifndef BOARD_K16F_H
#define	BOARD_K16F_H

#include "board_picsim.h"


class cboard_K16F:public board_picsim
{
  private:
    int p_CL1; 
    int p_CL2; 
    int p_CL3;    

     lcd_t lcd;

     mi2c_t mi2c;
     rtc_t rtc;
     
     int lcde;

     unsigned char clko;
     unsigned char d;
     unsigned char sda,sck;
     char mi2c_tmp_name[200];  
    
  public:
      lxString GetAboutInfo(void){return lxT("L.C. Gamboa \n <lcgamboa@yahoo.com>");};
      cboard_K16F(void);
      ~cboard_K16F(void);
      void Draw(CDraw *draw,double scale);
      void Run_CPU(void);
      lxString GetSupportedDevices(void){return lxT("PIC16F628A,PIC16F648A,PIC16F84A,");};
      lxString GetPictureFileName(void){return lxT("K16F/board.png");};
      lxString GetInputMapFile(void){return lxT("K16F/input.map");};
      lxString GetOutputMapFile(void){return lxT("K16F/output.map");};
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

