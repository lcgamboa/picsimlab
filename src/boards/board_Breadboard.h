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

#ifndef BOARD_Breadboard_H
#define	BOARD_Breadboard_H

#include<lxrad.h>

#include "bsim_picsim.h"
#include "bsim_simavr.h"

#define _PIC 0
#define _AVR 1

//new board class must be derived from board class defined in board.h
class cboard_Breadboard:public bsim_picsim, public bsim_simavr
{
  private:
      int ptype;
      lxBitmap * micbmp;
      lxFont font;
   public:
      void SetScale (double scale);
      lxString GetAboutInfo(void){return lxT("L.C. Gamboa \n <lcgamboa@yahoo.com>");}; 
      int DebugInit(int dtyppe); 
      lxString GetDebugName(void);
      void DebugLoop(void);
      int CpuInitialized(void);
      void MSetSerial(const char * port);
      int MInit(const char * processor, const char * fname, float freq);
      void MEnd(void);
      void MDumpMemory(const char * fname);
      void MEraseFlash(void);
      void MSetFreq(float freq);
      float MGetFreq(void);
      float MGetInstClockFreq(void);
      int MGetPinCount(void);
      lxString MGetPinName(int pin);
      void MSetPin(int pin, unsigned char value);
      void MSetPinDOV(int pin, unsigned char ovalue);
      void MSetAPin(int pin, float value);
      unsigned char MGetPin(int pin);  
      const picpin * MGetPinsValues(void);  
      void MStep(void);
      void MStepResume(void);
      int DBGTestBP(unsigned int bp);
      void MReset(int flags);
      unsigned short * DBGGetProcID_p(void);
      unsigned int  DBGGetPC(void);
      void DBGSetPC(unsigned int pc);
      unsigned char * DBGGetRAM_p(void);
      unsigned char * DBGGetROM_p(void);
      unsigned char * DBGGetCONFIG_p(void);
      unsigned char * DBGGetID_p(void);
      unsigned char * DBGGetEEPROM_p(void);
      unsigned int DBGGetRAMSize(void);
      unsigned int DBGGetROMSize(void);
      unsigned int DBGGetCONFIGSize(void);
      unsigned int DBGGetIDSize(void);
      unsigned int DBGGetEEPROM_Size(void);
      
      //Constructor called once on board creation 
      cboard_Breadboard(void);
      //Destructor called once on board destruction 
      ~cboard_Breadboard(void); 
      //Called ever 100ms to draw board
      void Draw(CDraw *draw);
      void Run_CPU(void);
      //Return a list of board supported microcontrollers
      lxString GetSupportedDevices(void){return lxT("atmega328p,atmega2560,PIC16F18324,PIC16F18855,PIC16F1619,PIC16F1788,PIC16F1789,PIC16F1939,PIC16F628A,PIC16F648A,PIC16F84A,PIC16F777,PIC16F877A,PIC18F452,PIC18F4520,PIC18F4550,PIC18F45K50,PIC18F4620,PIC18F27K40,PIC18F46J50,PIC18F47K40,PIC18F67J94,");};     
      //Return the filename of board picture 
      lxString GetPictureFileName(void){return lxT("Breadboard/board.svg");};
      //Return the filename of board picture input map 
      lxString GetInputMapFile(void){return lxT("Breadboard/input.map");};
      //Return the filename of board picture output map 
      lxString GetOutputMapFile(void){return lxT("Breadboard/output.map");};
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

#endif	/* BOARD_Breadboard_H */

