/* ########################################################################

   PICSimLab - Programmable IC Simulator Laboratory

   ########################################################################

   Copyright (c) : 2010-2024  Luis Claudio Gambôa Lopes <lcgamboa@yahoo.com>

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

#include "bridge_gpsim.h"

#include "../lib/picsimlab.h"
#include "bsim_gpsim.h"

static const unsigned char GPSIM_PORTS[7] = {0, 1, 2, 3, 4, 5, 0xFF};

bsim_gpsim::bsim_gpsim(void) {
    char list[2000];
    supported_devices = bridge_gpsim_get_processor_list(list, 1999);
    PICSimLab.SetNeedReboot();
}

void bsim_gpsim::MSetSerial(const char* port) {
 /*
 set_serial (&pic,0, port, 0, 0, 0);
 set_serial (&pic,1, "", 0, 0, 0);
  */ }

 int bsim_gpsim::MInit(const char* processor, const char* fname, float freq) {
     int ret = -1;

     std::string sproc = GetSupportedDevices();

     if (sproc.find(processor) != std::string::npos) {
         if (!lxFileExists(fname)) {
             // create a empty memory
             FILE* fout;
             fout = fopen_UTF8(fname, "w");
             if (fout) {
                 fprintf(fout, ":020000040000FA\n:00000001FF\n");
                 fclose(fout);
             } else {
                 printf("PICSimlab: Erro creating file %s \n", fname);
                 exit(-1);
             }
         }
         ret = bridge_gpsim_init(processor, fname, freq);
         pins_reset();
     }

     return ret;
 }

 void bsim_gpsim::MEnd(void) {
     // bridge_gpsim_end(); //Not needed when SetNeedReboot is used
 }

 int bsim_gpsim::MGetArchitecture(void) {
     return ARCH_UNKNOWN;
 }

 void bsim_gpsim::MEraseFlash(void) {
     // erase_flash ();
 }

 void bsim_gpsim::MSetFreq(float freq_) {
     freq = freq_;
     bridge_gpsim_set_frequency(freq);
     TimerUpdateFrequency(freq);
 }

 float bsim_gpsim::MGetFreq(void) {
     return freq;
 }

 void bsim_gpsim::MSetVCC(float vcc) {
     printf("gpsim: Incomplete!!!!\n");
 }

 float bsim_gpsim::MGetVCC(void) {
     return 5.0;
 }

 float bsim_gpsim::MGetInstClockFreq(void) {
     return freq;
 }

 int bsim_gpsim::CpuInitialized(void) {
     return 1;
 }

 void bsim_gpsim::DebugLoop(void) {}

 std::string bsim_gpsim::MGetPinName(int pin) {
     std::string pinname = "error";

     if ((pin) && (pin <= MGetPinCount())) {
         pinname = bridge_gpsim_get_pin_name(pin);

         if (pinname.find("port") != std::string::npos) {
             pinname = "R" + pinname.substr(4, 2);
             pinname = uppercase(pinname);
         }
         if (pinname.length() == 0) {
             pinname = "nd";
         }
     }
     return pinname;
 }

 int bsim_gpsim::MDumpMemory(const char* fname) {
     return bridge_gpsim_dump_memory(fname);
 }

 int bsim_gpsim::DebugInit(int dtyppe)  // argument not used in picm only mplabx
 {
     return 0;  //! mplabxd_init (this, Window1.Get_debug_port ()) - 1;
 }

 int bsim_gpsim::MGetPinCount(void) {
     return bridge_gpsim_get_pin_count();
 }

 void bsim_gpsim::pins_reset(void) {
     for (int p = 0; p < MGetPinCount(); p++) {
         std::string pname = MGetPinName(p + 1);
         if (pname[0] == 'R') {
             pins[p].port = (unsigned char*)&GPSIM_PORTS[pname[1] - 'A'];
             pins[p].pord = pname[2] - '0';
             pins[p].ptype = PT_DIGITAL;
             pins[p].dir = PD_IN;
             pins[p].value = 0;
         } else if (pname[0] == 'M') {  // MCLR
             pins[p].port = (unsigned char*)&GPSIM_PORTS[6];
             pins[p].pord = -1;
             pins[p].ptype = PT_DIGITAL;
             pins[p].dir = PD_IN;
             pins[p].value = 1;
         } else {
             pins[p].port = (unsigned char*)&GPSIM_PORTS[6];
             pins[p].pord = -1;
             pins[p].ptype = PT_POWER;
             pins[p].dir = PD_OUT;
             pins[p].value = 0;
         }
         pins[p].avalue = 0;
         pins[p].lvalue = 0;
         pins[p].ovalue = 0;
         pins[p].oavalue = 55;
     }
     // TODO add VCC and GND pins
     //  pins[7 - 1].value = 0;
     //  pins[7 - 1].dir = PD_OUT;
     //  pins[9 - 1].value = 1;
     //  pins[9 - 1].dir = PD_OUT;
 }

 void bsim_gpsim::MSetPin(int pin, unsigned char value) {
     bridge_gpsim_set_pin_value(pin, value);
 }

 void bsim_gpsim::MSetPinDOV(int pin, unsigned char ovalue) {
     // set_pin_DOV (pin, ovalue);
 }

 void bsim_gpsim::MSetAPin(int pin, float value) {
     if (pins[pin - 1].dir == PD_IN) {
         pins[pin - 1].ptype = PT_ANALOG;
         pins[pin - 1].avalue = value;
         bridge_gpsim_set_apin_value(pin, value);
     }
 }

 unsigned char bsim_gpsim::MGetPin(int pin) {
     return bridge_gpsim_get_pin_value(pin);
 }

 void bsim_gpsim::MReset(int flags) {
     bridge_gpsim_reset();
 }

 const picpin* bsim_gpsim::MGetPinsValues(void) {
     return pins;
 }

 void bsim_gpsim::MStep(void) {
     bridge_gpsim_step();

     for (int i = 0; i < MGetPinCount(); i++) {
         pins[i].value = bridge_gpsim_get_pin_value(i + 1);
         pins[i].dir = bridge_gpsim_get_pin_dir(i + 1);
     }
 }

 void bsim_gpsim::MStepResume(void) {
     // if (pic.s2 == 1)step ();
 }
