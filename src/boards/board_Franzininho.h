/* ########################################################################

   PICsimLab - PIC laboratory simulator

   ########################################################################

   Copyright (c) : 2017-2021  Luis Claudio Gambôa Lopes

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

#ifndef BOARD_Franzininho_H
#define	BOARD_Franzininho_H

#include<lxrad.h>

#include "bsim_simavr.h"

#define	BOARD_Franzininho_Name "Franzininho"

//new board class must be derived from board class defined in board.h
class cboard_Franzininho:public bsim_simavr
{
  private:
/*
     //controls to be added in simulator window
     CGauge *gauge1;   //gauge to show mean value of 3
     CGauge *gauge2;   //gauge to show mean value of 5
     CGauge *gauge3;   //gauge to show mean value of 6
     CGauge *gauge4;   //gauge to show mean value of 9
     CGauge *gauge5;   //gauge to show mean value of 10
     CGauge *gauge6;   //gauge to show mean value of 11


     CLabel *label1;   //label of gauge 3
     CLabel *label2;   //label of gauge 5
     CLabel *label3;   //label of gauge 6
     CLabel *label4;   //label of gauge 9
     CLabel *label5;   //label of gauge 10
     CLabel *label6;   //label of gauge 11
 */
     void RegisterRemoteControl(void);
     lxColor color1;
     lxColor color2;
   public:
      //Return the board name
      lxString GetName(void) {return lxT(BOARD_Franzininho_Name); };
      lxString GetAboutInfo(void){return lxT("Fábio Souza e Felipe Alvares\n <contato@franzininho.com.br>\n https://franzininho.com.br/");};
      //Constructor called once on board creation
      cboard_Franzininho(void);
      //Destructor called once on board destruction
      ~cboard_Franzininho(void);
      //Called ever 100ms to draw board
      void Draw(CDraw *draw);
      void Run_CPU(void);
      //Return a list of board supported microcontrollers
      lxString GetSupportedDevices(void){return lxT("attiny85,");};
      //Reset board status
      void Reset(void);
      //Event on the board
      void EvMouseButtonPress(uint button, uint x, uint y,uint state);
      //Event on the board
      void EvMouseButtonRelease(uint button, uint x, uint y,uint state);
      //Event on the board
      void EvKeyPress(uint key, uint mask);
      //Event on the board
      void EvKeyRelease(uint key, uint mask);
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

#endif	/* BOARD_Franzininho_H */

