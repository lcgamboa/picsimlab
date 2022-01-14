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

#ifndef BOARD_AVR_H
#define BOARD_AVR_H

#include "board.h"

#include"../devices/mplabxd.h"
#include"../devices/bitbang_uart.h"
#include"../serial_port.h"

#include <simavr/sim_avr.h>
#include <simavr/avr_ioport.h>
#include <simavr/sim_elf.h>
#include <simavr/sim_hex.h>
#include <simavr/sim_gdb.h>
#include <simavr/avr_adc.h>
#include <simavr/avr_uart.h>
#include <simavr/sim_hex.h>
#include <simavr/avr_twi.h>

typedef struct {
    unsigned char sdao;
    unsigned char sclo;
    picpin * sda;
    picpin * scl;
    avr_irq_t * sda_irq;
    avr_irq_t * scl_irq;   
    unsigned char out;
} usi_t;

class bsim_simavr : virtual public board {
public:
    bsim_simavr(void); //Called once on board creation
    int DebugInit(int dtyppe) override;
    lxString GetDebugName(void) override;
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
    void MStep(void) override;
    void MStepResume(void) override;
    void MReset(int flags) override;
    unsigned short * DBGGetProcID_p(void) override;
    unsigned int DBGGetPC(void) override;
    void DBGSetPC(unsigned int pc) override;
    unsigned char * DBGGetRAM_p(void) override;
    unsigned char * DBGGetROM_p(void) override;
    unsigned char * DBGGetCONFIG_p(void) override;
    unsigned char * DBGGetID_p(void) override;
    unsigned char * DBGGetEEPROM_p(void) override;
    unsigned int DBGGetRAMSize(void) override;
    unsigned int DBGGetROMSize(void) override;
    unsigned int DBGGetCONFIGSize(void) override;
    unsigned int DBGGetIDSize(void) override;
    unsigned int DBGGetEEPROM_Size(void) override;
    void EndServers(void) override;

    virtual void UpdateHardware(void);
    
    static void out_hook(struct avr_irq_t* irq, uint32_t value, void* param) {
        picpin * p = (picpin *) param;
        p->value = value;
        ioupdated = 1;
    }

    static void ddr_hook(struct avr_irq_t* irq, uint32_t value, void* param) {
        picpin * p = (picpin *) param;
        p->dir = !(value & (1 << p->pord));
        ioupdated = 1;
    }

    void SerialSend(unsigned char value);
    
    usi_t USI;    
protected:    
    avr_t * avr;
    avr_irq_t * serial_irq;
    picpin pins[256];
    avr_irq_t * Write_stat_irq[100];
    unsigned int serialbaud;
    float serialexbaud;
    void pins_reset(void);
    int avr_debug_type;
    serialfd_t serialfd;
    bitbang_uart_t bb_uart;
    unsigned char * eeprom;
    unsigned char uart_config;
    unsigned char has_usart;
private:
    int parse_hex(char *line, int bytes);
    unsigned char checksum(char* str);
    int read_ihx_avr(const char * fname, int leeprom);
    int write_ihx_avr(const char * fname);

    unsigned char pin_rx;
    unsigned char pin_tx;
};

#define UCSR0A 0XC0
#define UCSR0B 0XC1
#define UCSR0C 0XC2
#define UBRR0L 0xC4
#define UBRR0H 0xC5

#define USICR   0x2D
#define USISR   0x2E
#define USIDR   0x2F
#define USIBR   0x30 

//TinyDebug
#define TDDR 0x3A
#define TDCR 0x3B

#endif /* BOARD_AVR_H */

