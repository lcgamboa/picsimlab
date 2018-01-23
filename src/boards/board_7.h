/* ########################################################################

   PICsimLab - PIC laboratory simulator

   ########################################################################

   Copyright (c) : 2015-2018  Luis Claudio Gambôa Lopes

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

#ifndef BOARD_7_H
#define	BOARD_7_H

#include<lxrad/lxrad.h>

#include "board_pic.h"

//new board class must be derived from board class defined in board.h
class cboard_7:public board_pic
{
  private:
     int p_BT1;           //S1 switch

     int jmp[1];             //jmp
          
     //controls to be added in simulator window
     CScroll *scroll1; //scroll for analog input AN0    
     CGauge *gauge1;   //gauge to show mean value of RA5
     CGauge *gauge2;   //gauge to show mean value of RA1
     CGauge *gauge3;   //gauge to show mean value of RA2
     CGauge *gauge4;   //gauge to show mean value of RC5
     CLabel *label1;   //label of scroll AN4
     CLabel *label2;   //label of gauge RA5
     CLabel *label3;   //label of gauge RA1
     CLabel *label4;   //label of gauge RA2
     CLabel *label5;   //label of gauge RC5
     
 
   public:
      //Constructor called once on board creation 
      cboard_7(void);
      //Destructor called once on board destruction 
      ~cboard_7(void); 
      //Called ever 100ms to draw board
      void Draw(CDraw *draw,double scale);
      void Run_CPU(void);
      //Return a list of board supported microcontrollers
      String GetSupportedDevices(void){return wxT("PIC16F1619,");};
      //Return the filename of board picture 
      String GetPictureFileName(void){return wxT("picsimlab7.png");};
      //Return the filename of board picture input map 
      String GetInputMapFile(void){return wxT("input7.map");};
      //Return the filename of board picture output map 
      String GetOutputMapFile(void){return wxT("output7.map");};
      //Reset board status
      void Reset(void);
      //Event on the board
      void MouseButtonPress(uint button, uint x, uint y,uint state);
      //Event on the board
      void MouseButtonRelease(uint button, uint x, uint y,uint state);
      //Event on the board
      void KeyPress(uint key, uint x, uint y,uint mask);
      //Event on the board
      void KeyRelease(uint key, uint x, uint y,uint mask);
      void OnShow(void){};
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

#endif	/* BOARD_7_H */

