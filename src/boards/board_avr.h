/* ########################################################################

   PICsimLab - PIC laboratory simulator

   ########################################################################

   Copyright (c) : 2010-2017  Luis Claudio Gambôa Lopes

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

#ifndef BOARD_AVR_H
#define	BOARD_AVR_H

#include "board.h"

#include "sim_avr.h"
#include "avr_ioport.h"
#include "sim_elf.h"
#include "sim_hex.h"
#include "sim_gdb.h"
#include "avr_adc.h"
#include "../picsimlab1.h"



class board_avr: public board
{
  public:
      board_avr(void);//Called once on board creation
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
      
      static void out_hook( struct avr_irq_t* irq, uint32_t value, void* param )
      {      
         picpin * p  = (picpin *)param;
         p->value=value;
      }

      static void ddr_hook( struct avr_irq_t* irq, uint32_t value, void* param )
      {
         picpin * p = (picpin *)param;
         p->dir=!(value & (1<<p->pord));
      }

      void adc_read(int ch,float val)
      {
        avr_raise_irq( avr_io_getirq( avr, AVR_IOCTL_ADC_GETIRQ, ch),(int)(val*1000) );
      }
            
      static void adc_hook( struct avr_irq_t* irq, uint32_t value, void* param )   
      {
            picpin * p = (picpin *)param;
            ((board_avr *)Window1.GetBoard())->adc_read(value,p->avalue); 
      } 
      

        
 protected:
      avr_t *  avr;
      picpin pins[28];//FIXME only for atmega328!
      avr_irq_t * Write_stat_irq[28];//FIXME only for atmega328!
 private:
      int parse_hex(char *line,int bytes);
      unsigned char checksum(char* str);
      int read_ihx_avr(const char * fname, int leeprom);
      int write_ihx_avr(const char * fname);  
};

#endif	/* BOARD_AVR_H */

