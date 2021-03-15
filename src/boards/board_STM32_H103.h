/* ########################################################################

   PICsimLab - PIC laboratory simulator

   ########################################################################

   Copyright (c) : 2015-2020  Luis Claudio Gambôa Lopes

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

#ifndef BOARD_STM32_H103_H
#define	BOARD_STM32_H103_H

#include<lxrad.h>

#include "bsim_qemu_stm32.h"

//new board class must be derived from board class defined in board.h
class cboard_STM32_H103:public bsim_qemu_stm32
{
   private:
     unsigned char p_BUT;

     void RegisterRemoteControl(void);        
   public:
      lxString GetAboutInfo(void){return lxT("L.C. Gamboa \n <lcgamboa@yahoo.com>");}; 
      //Constructor called once on board creation 
      cboard_STM32_H103(void);
      //Destructor called once on board destruction 
      ~cboard_STM32_H103(void); 
      //Called ever 100ms to draw board
      void Draw(CDraw *draw);
      void Run_CPU(void);
      //Return a list of board supported microcontrollers
      lxString GetSupportedDevices(void){return lxT("stm32f103rbt6,");};
      //Return the filename of board picture 
      lxString GetPictureFileName(void){return lxT("STM32_H103/board.png");};
      //Return the filename of board picture input map 
      lxString GetInputMapFile(void){return lxT("STM32_H103/input.map");};
      //Return the filename of board picture output map 
      lxString GetOutputMapFile(void){return lxT("STM32_H103/output.map");};
      //Reset board status
      void Reset(void);
      //Event on the board
      void EvMouseButtonPress(uint button, uint x, uint y,uint state);
      //Event on the board
      void EvMouseButtonRelease(uint button, uint x, uint y,uint state);
      //Event on the board
      void EvKeyPress(uint key,uint mask);
      //Event on the board
      void EvKeyRelease(uint key,uint mask);
      void EvOnShow(void){};
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

#endif	/* BOARD_STM32_H103_H */

