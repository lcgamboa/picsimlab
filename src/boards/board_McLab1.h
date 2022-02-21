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

#ifndef BOARD_McLab1_H
#define	BOARD_McLab1_H

#include<lxrad.h>
#include "bsim_picsim.h"
#include"../devices/swbounce.h"

#define	BOARD_McLab1_Name "McLab1"

class cboard_McLab1:public bsim_picsim
{
  private:
     unsigned char p_BT[4]; 
     unsigned char jmp[1];
     unsigned int lm1[18]; //luminosidade media display
     unsigned int lm2[18]; //luminosidade media display
     
     CGauge *gauge1;
     CLabel *label1;
     void RegisterRemoteControl(void) override; 
     lxColor color1;
     lxColor color2;
     lxFont font;   
     SWBounce_t bounce;  
   public:
      //Return the board name
      lxString GetName(void) override {return lxT(BOARD_McLab1_Name); };
      lxString GetAboutInfo(void) override {return lxT("L.C. Gamboa \n <lcgamboa@yahoo.com>");}; 
      cboard_McLab1(void);
      ~cboard_McLab1(void);
      void Draw(CDraw *draw) override;
      void Run_CPU(void) override;
      lxString GetSupportedDevices(void) override {return lxT("PIC16F628A,PIC16F648A,PIC16F84A,");};
      void Reset(void) override;
      void EvMouseButtonPress(uint button, uint x, uint y,uint state) override;
      void EvMouseButtonRelease(uint button, uint x, uint y,uint state) override;
      void EvKeyPress(uint key, uint mask) override;
      void EvKeyRelease(uint key, uint mask) override;
      void WritePreferences(void) override;
      void ReadPreferences(char *name,char *value) override;
      unsigned short get_in_id(char * name) override;
      unsigned short get_out_id(char * name) override;
};


#endif	/* BOARD_McLab1_H */

