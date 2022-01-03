/* ########################################################################

   PICsimLab - PIC laboratory simulator

   ########################################################################

   Copyright (c) : 2010-2021  Luis Claudio Gambôa Lopes

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


#ifndef BOARD_REMOTETCP_H
#define	BOARD_REMOTETCP_H

#include "board.h"
#include <stdint.h>

//===================== Ripes protocol =========================================
typedef struct {
uint32_t msg_type;
uint32_t payload_size;
} cmd_header_t;

enum {VB_PINFO=1, VB_PWRITE, VB_PREAD, VB_PSTATUS, VB_DMAWR, VB_DMARD, VB_QUIT, VB_LAST};
//==============================================================================

class bsim_remote: virtual public board
{
  public:
      bsim_remote(void);
      ~bsim_remote(void);
      int DebugInit(int dtyppe) override; 
      lxString GetDebugName(void) override {return "NONE";};
      void DebugLoop(void) override;
      int CpuInitialized(void) override;
      void MSetSerial(const char * port) override;
      int MInit(const char * processor, const char * fname, float freq) override;
      void MEnd(void) override;
      void MDumpMemory(const char * fname) override;
      void MEraseFlash(void) override;
      void MSetFreq(float freq) override;
      float MGetFreq(void) override;
      float MGetInstClockFreq(void) override;
      int MGetPinCount(void) override;
      lxString MGetPinName(int pin) override;
      void MSetPin(int pin, unsigned char value) override;
      void MSetPinDOV(int pin, unsigned char ovalue) override;      
      void MSetAPin(int pin, float value) override;
      unsigned char MGetPin(int pin) override;  
      const picpin * MGetPinsValues(void) override;  
      void MStepResume(void) override;
      void MReset(int flags) override;
      void EndServers(void) override;
 protected:
      const int TestConnection(void); 
      const int DataAvaliable(void);
      void ConnectionError(const char * s_error);
      void Disconnect(void);
      void pins_reset(void);
//===================== Ripes protocol =========================================        
      int32_t recv_cmd (cmd_header_t * cmd_header);
      int32_t recv_payload (char * buff, const uint32_t payload_size);
      int32_t send_cmd (const uint32_t cmd, const char * payload = NULL, const uint32_t payload_size = 0);
//==============================================================================      
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
      int connected;
      char fname_[300];
      char fname_bak[300];
      unsigned short ADCvalues[16];
      unsigned short Dirs[2]  = {0xFF, 0xFF};
      unsigned short Ports[2] = {0x00, 0x00};
      unsigned short t0CNT = 0;
      unsigned short t0STA = 0;
      unsigned short t0CON = 0x7FFF;
      unsigned short t0PR  = 0xFFFF;
      unsigned short t0iclk = 0; 
};

#endif	/* BOARD_REMOTETCP_H */

