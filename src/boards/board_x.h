/* ########################################################################

   PICsimLab - PIC laboratory simulator

   ########################################################################

   Copyright (c) : 2015-2021  Luis Claudio Gambôa Lopes

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

#ifndef BOARD_x_H
#define	BOARD_x_H

#include<lxrad.h>

#include "bsim_picsim.h"

#define	BOARD_x_Name "X"

//new board class must be derived from board class defined in board.h
class cboard_x:public bsim_picsim
{
  private:
     unsigned char p_BT1;           //first board push button in RD0
     unsigned char p_BT2;           //second board switch in RD1  
     
     //value of potentiometer
     unsigned char pot1;
     //flag to control if potentiometer is active
     unsigned char active;
     
     //controls to be added in simulator window 
     CGauge *gauge1;   //gauge to show mean value of RB0
     CGauge *gauge2;   //gauge to show mean value of RB1
     CLabel *label2;   //label of gauge RB0
     CLabel *label3;   //label of gauge RB1
     
     //Register controls for remote interface called once on board creation 
     void RegisterRemoteControl(void);  
     
     lxColor color1;//LEDs color 1
     lxColor color2;//LEDs color 2   
     lxFont font;
   public:
      //Constructor called once on board creation 
      cboard_x(void);
      //Destructor called once on board destruction 
      ~cboard_x(void); 
      //Return the board name
      lxString GetName(void) {return lxT(BOARD_x_Name); }; 
      //Return the about info of board
      lxString GetAboutInfo(void){return lxT("L.C. Gamboa \n <lcgamboa@yahoo.com>");};
      //Called ever 100ms to draw board
      void Draw(CDraw *draw);
      void Run_CPU(void);
      //Return a list of board supported microcontrollers
      lxString GetSupportedDevices(void){return lxT("PIC16F877A,PIC18F4550,PIC18F4620,");};
      //Reset board status
      void Reset(void);
      //Event on the board
      void EvMouseButtonPress(uint button, uint x, uint y,uint state);
      //Event on the board
      void EvMouseButtonRelease(uint button, uint x, uint y,uint state);
      //Event on the board
      void EvMouseMove(uint button, uint x, uint y, uint state);
      //Event on the board
      void EvKeyPress(uint key,uint mask);
      //Event on the board
      void EvKeyRelease(uint key,uint mask);
      //Called ever 1s to refresh status
      void RefreshStatus(void);
      //Called to save board preferences in configuration file
      void WritePreferences(void);
      //Called whe configuration file load  preferences 
      void ReadPreferences(char *name,char *value);
      //return the input ids numbers of names used in input map
      unsigned short get_in_id(char * name);
      //return the output ids numbers of names used in output map
      unsigned short get_out_id(char * name);
};

#endif	/* BOARD_x_H */

