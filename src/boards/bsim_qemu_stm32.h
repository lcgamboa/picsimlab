/* ########################################################################

   PICsimLab - PIC laboratory simulator

   ########################################################################

   Copyright (c) : 2010-2020  Luis Claudio Gambôa Lopes

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


#ifndef BOARD_STM32_H
#define	BOARD_STM32_H

#include "board.h"

class bsim_qemu_stm32: virtual public board
{
  public:
      bsim_qemu_stm32(void);
      ~bsim_qemu_stm32(void);
      int DebugInit(int dtyppe); 
      lxString GetDebugName(void){return "GDB";};
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
      void MReset(int flags);
      void pins_reset(void);
 protected:
      int qemu_cmd(const char * cmd, int raw =0);
#ifdef _WIN_
      HANDLE serialfd[4];
#else
      int serialfd[4];
#endif      
      int procid; 
      picpin pins[256];
      unsigned int serialbaud; 
      float serialexbaud;
      float freq;
      int sockfd;
      int sockmon;
      int connected;
      char fname_[300];
      char fname_bak[300];
      unsigned short ADCvalues[16];
};

#endif	/* BOARD_STM32_H */

