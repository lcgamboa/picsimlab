/* ########################################################################

   PICsimLab - PIC laboratory simulator

   ########################################################################

   Copyright (c) : 2015  Luis Claudio Gambôa Lopes

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

#include "board_pic.h"

//new board class must be derived from board class defined in board.h
class cboard_x:public board_pic
{
  private:
     int p_BT1;           //first board switch in RD0
     int p_BT2;           //second board switch in RD1  

     //controls to be added in simulator window
     CScroll *scroll1; //scroll for analog input AN0    
     CGauge *gauge1;   //gauge to show mean value of RB0
     CGauge *gauge2;   //gauge to show mean value of RB1
     CLabel *label1;   //label of scroll AN0
     CLabel *label2;   //label of gauge RB0
     CLabel *label3;   //label of gauge RB1
 
   public:
      //Constructor called once on board creation 
      cboard_x(void);
      //Destructor called once on board destruction 
      ~cboard_x(void); 
      //Called ever 100ms to draw board
      void Draw(CDraw *draw,double scale);
      void Run_CPU(void);
      //Return a list of board supported microcontrollers
      String GetSupportedDevices(void){return lxT("PIC18F4550,PIC16F877A,");};
      //Return the filename of board picture 
      String GetPictureFileName(void){return lxT("board_x.png");};
      //Return the filename of board picture input map 
      String GetInputMapFile(void){return lxT("input_boardx.map");};
      //Return the filename of board picture output map 
      String GetOutputMapFile(void){return lxT("output_boardx.map");};
      //Reset board status
      void Reset(void);
      //Event on the board
      void EvMouseButtonPress(uint button, uint x, uint y,uint state);
      //Event on the board
      void EvMouseButtonRelease(uint button, uint x, uint y,uint state);
      //Event on the board
      void EvKeyPress(uint key, uint x, uint y,uint mask);
      //Event on the board
      void EvKeyRelease(uint key, uint x, uint y,uint mask);
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

#endif	/* BOARD_x_H */

