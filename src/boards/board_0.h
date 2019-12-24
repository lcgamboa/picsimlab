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

#ifndef BOARD_0_H
#define	BOARD_0_H

#include<lxrad.h>

#include "board_pic.h"
#include "board_avr.h"

#define _PIC 0
#define _AVR 1

//new board class must be derived from board class defined in board.h
class cboard_0:public board_pic, public board_avr
{
  private:
      int ptype;
      lxBitmap * micbmp;
   public:
      int DebugInit(void); 
      void DebugLoop(void);
      int CpuInitialized(void);
      void MSetSerial(const char * port);
      int MInit(const char * processor, const char * fname, float freq);
      void MEnd(void);
      void MDumpMemory(const char * fname);
      void MEraseFlash(void);
      void MSetFreq(float freq);
      float MGetFreq(void);
      float MGetInstClock(void);
      int MGetPinCount(void);
      String MGetPinName(int pin);
      void MSetPin(int pin, unsigned char value);
      void MSetAPin(int pin, float value);
      unsigned char MGetPin(int pin);  
      const picpin * MGetPinsValues(void);  
      
      //Constructor called once on board creation 
      cboard_0(void);
      //Destructor called once on board destruction 
      ~cboard_0(void); 
      //Called ever 100ms to draw board
      void Draw(CDraw *draw,double scale);
      void Run_CPU(void);
      //Return a list of board supported microcontrollers
#ifdef _EXPERIMENTAL_      
      String GetSupportedDevices(void){return lxT("atmega328,PIC16F18855,PIC16F1619,PIC16F628A,PIC16F648A,PIC16F84A,PIC16F777,PIC16F877A,PIC18F452,PIC18F4520,PIC18F4550,PIC18F45K50,PIC18F4620,");};
#else
      String GetSupportedDevices(void){return lxT("atmega328,PIC16F628A,PIC16F648A,PIC16F84A,PIC16F777,PIC16F877A,PIC18F452,PIC18F4550,PIC18F4620,");};
#endif      
      //Return the filename of board picture 
      String GetPictureFileName(void){return lxT("picsimlab0.png");};
      //Return the filename of board picture input map 
      String GetInputMapFile(void){return lxT("input0.map");};
      //Return the filename of board picture output map 
      String GetOutputMapFile(void){return lxT("output0.map");};
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

#endif	/* BOARD_x_H */

