/* ########################################################################

   PICsimLab - PIC laboratory simulator

   ########################################################################

   Copyright (c) : 2015-2016  Luis Claudio Gambôa Lopes

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

#ifndef BOARD_Xpress_H
#define	BOARD_Xpress_H

#include<lxrad.h>

#include "bsim_picsim.h"

#define BOARD_Xpress_Name "Xpress"

//new board class must be derived from board class defined in board.h
class cboard_Xpress:public bsim_picsim
{
  private:
     unsigned char p_BT1;           //S1 switch
     
     unsigned char pot1;
     unsigned char active;
     
     //controls to be added in simulator window
     CGauge *gauge1;   //gauge to show mean value of RA5
     CGauge *gauge2;   //gauge to show mean value of RA1
     CGauge *gauge3;   //gauge to show mean value of RA2
     CGauge *gauge4;   //gauge to show mean value of RC5
     CLabel *label2;   //label of gauge RA5
     CLabel *label3;   //label of gauge RA1
     CLabel *label4;   //label of gauge RA2
     CLabel *label5;   //label of gauge RC5
     
     void RegisterRemoteControl(void) override; 
   public:
      //Return the board name 
      lxString GetName(void) override {return lxT(BOARD_Xpress_Name); }; 
      lxString GetAboutInfo(void) override {return lxT("L.C. Gamboa \n <lcgamboa@yahoo.com>");}; 
      //Constructor called once on board creation 
      cboard_Xpress(void);
      //Destructor called once on board destruction 
      ~cboard_Xpress(void); 
      //Called ever 100ms to draw board
      void Draw(CDraw *draw) override;
      void Run_CPU(void) override;
      //Return a list of board supported microcontrollers
      lxString GetSupportedDevices(void) override {return lxT("PIC16F18855,");};
      //Return the filename of board picture 
      lxString GetPictureFileName(void) override {return lxT("Xpress/board.png");};
      //Reset board status
      void Reset(void) override;
      //Event on the board
      void EvMouseButtonPress(uint button, uint x, uint y,uint state) override;
      //Event on the board
      void EvMouseButtonRelease(uint button, uint x, uint y,uint state) override;
      //Event on the board
      void EvMouseMove(uint button, uint x, uint y, uint state) override;
      //Event on the board
      void EvKeyPress(uint key, uint mask) override;
      //Event on the board
      void EvKeyRelease(uint key, uint mask) override;
      //Called ever 1s to refresh status
      void RefreshStatus(void) override;
      //Called to save board preferences in configuration file
      void WritePreferences(void) override;
      //Called whe configuration file load  preferences 
      void ReadPreferences(char *name,char *value) override;
      //return the input ids numbers of names used in input map
      unsigned short get_in_id(char * name) override;
      //return the output ids numbers of names used in output map
      unsigned short get_out_id(char * name) override;
};

#endif	/* BOARD_Xpress_H */

