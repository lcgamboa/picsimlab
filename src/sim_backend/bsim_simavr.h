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

#ifndef BOARD_AVR_H
#define BOARD_AVR_H

#include "../lib/board.h"

#include "../devices/bitbang_uart.h"
#include "../devices/mplabxd.h"
#include "../lib/serial_port.h"

#include <simavr/avr_adc.h>
#include <simavr/avr_ioport.h>
#include <simavr/avr_twi.h>
#include <simavr/avr_uart.h>
#include <simavr/sim_avr.h>
#include <simavr/sim_elf.h>
#include <simavr/sim_gdb.h>
#include <simavr/sim_hex.h>

#define MAX_UART_COUNT 4

typedef struct {
    unsigned char sdao;
    unsigned char sclo;
    picpin* sda;
    picpin* scl;
    avr_irq_t* sda_irq;
    avr_irq_t* scl_irq;
    unsigned char out;
} usi_t;

class bsim_simavr : virtual public board {
public:
    bsim_simavr(void);  // Called once on board creation
    int DebugInit(int dtyppe) override;
    std::string GetDebugName(void) override;
    void DebugLoop(void) override;
    int CpuInitialized(void) override;
    void MSetSerial(const char* port) override;
    int MInit(const char* processor, const char* fname, float freq) override;
    void MEnd(void) override;
    int MGetArchitecture(void) override;
    int MDumpMemory(const char* fname) override;
    void MEraseFlash(void) override;
    void MSetFreq(float freq) override;
    float MGetFreq(void) override;
    void MSetVCC(float vcc) override;
    float MGetVCC(void) override;
    float MGetInstClockFreq(void) override;
    int MGetPinCount(void) override;
    std::string MGetPinName(int pin) override;
    void MSetPin(int pin, unsigned char value) override;
    void MSetPinDOV(int pin, unsigned char ovalue) override;
    void MSetPinOAV(int pin, float value) override;
    void MSetAPin(int pin, float value) override;
    unsigned char MGetPin(int pin) override;
    const picpin* MGetPinsValues(void) override;
    float* MGetPinOAVPtr(int pin) override;
    void MStep(void) override;
    void MStepResume(void) override;
    int MReset(int flags) override;
    int MGetResetPin(void) override;
    int MGetIOUpdated(void) override;
    void MClearIOUpdated(void) override;
    unsigned short* DBGGetProcID_p(void) override;
    unsigned int DBGGetPC(void) override;
    void DBGSetPC(unsigned int pc) override;
    unsigned char* DBGGetRAM_p(void) override;
    unsigned char* DBGGetROM_p(void) override;
    unsigned char* DBGGetCONFIG_p(void) override;
    unsigned char* DBGGetID_p(void) override;
    unsigned char* DBGGetEEPROM_p(void) override;
    unsigned int DBGGetRAMSize(void) override;
    unsigned int DBGGetROMSize(void) override;
    unsigned int DBGGetCONFIGSize(void) override;
    unsigned int DBGGetIDSize(void) override;
    unsigned int DBGGetEEPROM_Size(void) override;
    void EndServers(void) override;
    int GetDefaultClock(void) override { return 16; };
    int GetUARTRX(const int uart_num) override;
    int GetUARTTX(const int uart_num) override;
    std::string GetUARTStrStatus(const int uart_num) override;
    virtual void UpdateHardware(void);
    void RefreshStatus(void) override;

    static void out_hook(struct avr_irq_t* irq, uint32_t value, void* param) {
        picpin* p = (picpin*)param;
        p->value = value;
        ioupdated = 1;
    }

    static void ddr_hook(struct avr_irq_t* irq, uint32_t value, void* param) {
        picpin* p = (picpin*)param;
        p->dir = !(value & (1 << p->pord));
        ioupdated = 1;
    }

    void SerialSend(bitbang_uart_t* _bb_uart, const unsigned char value);

    usi_t USI;

protected:
    avr_t* avr;
    avr_irq_t* serial_irq[MAX_UART_COUNT];
    picpin pins[MAX_PIN_COUNT];
    avr_irq_t* Write_stat_irq[100];
    unsigned int serialbaud[MAX_UART_COUNT];
    float serialexbaud[MAX_UART_COUNT];
    void pins_reset(void);
    int avr_debug_type;
    serialfd_t serialfd;
    bitbang_uart_t bb_uart[MAX_UART_COUNT];
    unsigned char* eeprom;
    unsigned char uart_config[MAX_UART_COUNT];
    unsigned char usart_count;
    unsigned int UCSR_base[MAX_UART_COUNT];

private:
    int parse_hex(const char* line, int bytes);
    unsigned char checksum(char* str);
    int read_ihx_avr(const char* fname, int leeprom);
    int write_ihx_avr(const char* fname);

protected:
    int pkg;
};

#define EIMSK 0x3D
#define UCSR0A 0XC0
#define UCSR0B 0XC1
#define UCSR0C 0XC2
#define UBRR0L 0xC4
#define UBRR0H 0xC5

#define UCSR1A 0xC8
#define UCSR1B 0XC9
#define UBRR1L 0xCC
#define UBRR1H 0xCD

#define UCSR2A 0xD0
#define UCSR2B 0XD1
#define UBRR2L 0xD4
#define UBRR2H 0xD5

#define UCSR3A 0x130
#define UCSR3B 0X131
#define UBRR3L 0x134
#define UBRR3H 0x135

#define GIMSK 0x3B
#define USICR 0x2D
#define USISR 0x2E
#define USIDR 0x2F
#define USIBR 0x30

// TinyDebug
#define TDDR 0x3A
#define TDCR 0x3B

#endif /* BOARD_AVR_H */
