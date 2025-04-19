/* ########################################################################

   PICSimLab - Programmable IC Simulator Laboratory

   ########################################################################

   Copyright (c) : 2010-2025  Luis Claudio Gambôa Lopes <lcgamboa@yahoo.com>

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

#include "bsim_picsim.h"
#include "math.h"

#include "../lib/picsimlab.h"

bsim_picsim::bsim_picsim(void) {
    pic.PINCOUNT = 0;
}

void bsim_picsim::MSetSerial(const char* port) {
    pic_set_serial(&pic, 0, port, 0, 0, 0);
    pic_set_serial(&pic, 1, "", 0, 0, 0);
}

int bsim_picsim::MInit(const char* processor, const char* fname, float freq) {
    std::string sproc = GetSupportedDevices();
    int procn = 0;

    if (sproc.find(processor) != std::string::npos) {
        procn = getprocbyname(processor);
    }

    if (procn == 0) {
        printf("PICSimLab: Unknown processor %s ! Loading Default\n", processor);
        int i = sproc.find("PIC");
        sproc = sproc.substr(i, sproc.length());
        i = sproc.find(",");
        Proc = sproc.substr(0, i);
        procn = getprocbyname(Proc.c_str());
    }

    int ret = pic_init(&pic, procn, fname, 1, freq);

    pic.disable_debug(&pic);

    pic.pins = (picpin*)realloc(pic.pins, sizeof(picpin) * MAX_PIN_COUNT);

    return ret;
}

void bsim_picsim::MEnd(void) {
    pic_end(&pic);
    // prog_end();
    mplabxd_end();
}

int bsim_picsim::MGetArchitecture(void) {
    switch (getfprocbyname((const char*)Proc.c_str())) {
        case P16:
            return ARCH_P16;
            break;
        case P16E:
            return ARCH_P16E;
            break;
        case P18:
            return ARCH_P18;
            break;
    }
    return ARCH_UNKNOWN;
}

void bsim_picsim::MEraseFlash(void) {
    pic_erase_flash(&pic);
}

void bsim_picsim::MSetFreq(float freq) {
    pic.freq = freq;
    TimerUpdateFrequency(freq / 4.0);
}

float bsim_picsim::MGetFreq(void) {
    return pic.freq;
}

void bsim_picsim::MSetVCC(float vcc) {
    pic.vcc = vcc;
}

float bsim_picsim::MGetVCC(void) {
    return pic.vcc;
}

float bsim_picsim::MGetInstClockFreq(void) {
    return pic.freq / 4.0;
}

int bsim_picsim::CpuInitialized(void) {
    return 1;
}

int bsim_picsim::MDumpMemory(const char* fname) {
    switch (getfprocbyname((const char*)Proc.c_str())) {
        case P16:
            return write_ihx(&pic, fname);
            break;
        case P16E:
            return write_ihx16e(&pic, fname);
            break;
        case P18:
            return write_ihx18(&pic, fname);
            break;
    }
    return 1;
}

int bsim_picsim::DebugInit(int dtyppe)  // argument not used in pic, it only use mplabx
{
    int ret = !mplabxd_init(this, PICSimLab.GetDebugPort()) - 1;

    if (ret < 0) {
        PICSimLab.RegisterError("Error starting MPLABX debugger support !");
    }

    return ret;
}

void bsim_picsim::DebugLoop(void) {
    if (PICSimLab.GetMcuPwr()) {
        // prog_loop(&pic);
        mplabxd_loop();
    }
}

int bsim_picsim::MGetPinCount(void) {
    return pic.PINCOUNT;
}

std::string bsim_picsim::MGetPinName(int pin) {
    char cbuf[10];
    std::string pinname;

    pinname = getPinName(&pic, pin, cbuf);

    return pinname;
}

void bsim_picsim::MSetPin(int pin, unsigned char value) {
    pic_set_pin(&pic, pin, value);
}

void bsim_picsim::MSetPinDOV(int pin, unsigned char ovalue) {
    pic_set_pin_DOV(&pic, pin, ovalue);
}

void bsim_picsim::MSetPinOAV(int pin, float value) {
    pic.pins[pin - 1].oavalue = value;
}

void bsim_picsim::MSetAPin(int pin, float value) {
    pic_set_apin(&pic, pin, value);
}

unsigned char bsim_picsim::MGetPin(int pin) {
    return pic_get_pin(&pic, pin);
}

const picpin* bsim_picsim::MGetPinsValues(void) {
    return pic.pins;
}

float* bsim_picsim::MGetPinOAVPtr(int pin) {
    return &pic.pins[pin - 1].oavalue;
}

void bsim_picsim::MStep(void) {
    pic_step(&pic);
}

void bsim_picsim::MStepResume(void) {
    if (pic.s2 == 1)
        pic_step(&pic);
}

int bsim_picsim::MReset(int flags) {
    return pic_reset(&pic, flags);
}

int bsim_picsim::MGetResetPin(void) {
    return pic.mclr;
}

int bsim_picsim::MGetIOUpdated(void) {
    return pic.ioupdated;
}

void bsim_picsim::MClearIOUpdated(void) {
    pic.ioupdated = 0;
}

unsigned short* bsim_picsim::DBGGetProcID_p(void) {
    return (unsigned short*)&pic.processor;
}

unsigned int bsim_picsim::DBGGetPC(void) {
    return pic.pc;
}

void bsim_picsim::DBGSetPC(unsigned int pc) {
    pic.pc = pc;
}

unsigned char* bsim_picsim::DBGGetRAM_p(void) {
    return pic.ram;
}

unsigned char* bsim_picsim::DBGGetROM_p(void) {
    return (unsigned char*)pic.prog;
}

unsigned char* bsim_picsim::DBGGetCONFIG_p(void) {
    return (unsigned char*)pic.config;
}

unsigned char* bsim_picsim::DBGGetID_p(void) {
    return (unsigned char*)pic.id;
}

unsigned char* bsim_picsim::DBGGetEEPROM_p(void) {
    return pic.eeprom;
}

unsigned int bsim_picsim::DBGGetRAMSize(void) {
    return pic.RAMSIZE;
}

unsigned int bsim_picsim::DBGGetROMSize(void) {
    if (pic.processor == getprocbyname("PIC18F46J50")) {
        return (pic.ROMSIZE - 4) * 2;  // anomalous size!!!
    } else if (pic.processor == getprocbyname("PIC18F67J94")) {
        return (pic.ROMSIZE - 8) * 2;  // anomalous size!!!
    } else {
        return pic.ROMSIZE * 2;
    }
}

unsigned int bsim_picsim::DBGGetCONFIGSize(void) {
    return pic.CONFIGSIZE * 2;
}

unsigned int bsim_picsim::DBGGetIDSize(void) {
    return pic.IDSIZE * 2;
}

unsigned int bsim_picsim::DBGGetEEPROM_Size(void) {
    return pic.EEPROMSIZE;
}

unsigned int bsim_picsim::DBGGetRAMLAWR(void) {
    return pic.lram;
}

unsigned int bsim_picsim::DBGGetRAMLARD(void) {
    return pic.rram;
}

void bsim_picsim::EndServers(void) {
    mplabxd_server_end();
}

int bsim_picsim::GetUARTRX(const int uart_num) {
    if (uart_num < pic.USARTCOUNT) {
        return pic.usart_rx[uart_num];
    }
    return 0;
}

int bsim_picsim::GetUARTTX(const int uart_num) {
    if (uart_num < pic.USARTCOUNT) {
        return pic.usart_tx[uart_num];
    }
    return 0;
}

std::string bsim_picsim::GetUARTStrStatus(const int uart_num) {
    if (pic.serial[uart_num].serialfd != INVALID_SERIAL)
        return "Serial: " + std::string(SERIALDEVICE) + ":" + std::to_string(pic.serial[uart_num].serialbaud) + "(" +
               FloatStrFormat(
                   "%4.1f", fabs((100.0 * pic.serial[uart_num].serialexbaud - 100.0 * pic.serial[uart_num].serialbaud) /
                                 pic.serial[uart_num].serialexbaud)) +
               "%)";
    else
        return "Serial: " + std::string(SERIALDEVICE) + " (ERROR)";
}