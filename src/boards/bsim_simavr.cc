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

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include "bsim_simavr.h"
#include"../picsimlab1.h"
#include "simavr/avr_eeprom.h"

//gdb debug hack stuff
extern "C"
{
#define WATCH_LIMIT (32)

 typedef struct
 {
  uint32_t len; /**< How many points are taken (points[0] .. points[len - 1]). */

  struct
  {
   uint32_t addr; /**< Which address is watched. */
   uint32_t size; /**< How large is the watched segment. */
   uint32_t kind; /**< Bitmask of enum avr_gdb_watch_type values. */
  } points[WATCH_LIMIT];
 } avr_gdb_watchpoints_t;

 typedef struct avr_gdb_t
 {
  avr_t * avr;
  int listen; // listen socket
  int s; // current gdb connection

  avr_gdb_watchpoints_t breakpoints;
  avr_gdb_watchpoints_t watchpoints;
 } avr_gdb_t;


 void gdb_send_quick_status(avr_gdb_t * g, uint8_t signal);
 int gdb_network_handler(avr_gdb_t * g, uint32_t dosleep);
 int gdb_watch_find(const avr_gdb_watchpoints_t * w, uint32_t addr);
};

bsim_simavr::bsim_simavr(void)
{
 avr = NULL;
 serial_irq = NULL;
 avr_debug_type = 0;
 eeprom = NULL;
}

void
bsim_simavr::MSetSerial(const char * port) { }

enum
{
 IRQ_UART_BYTE_IN = 0,
 IRQ_UART_BYTE_OUT,
 IRQ_UART_COUNT
};

static const char * irq_names_uart[IRQ_UART_COUNT] = {
 /*[IRQ_UART_BYTE_IN] =*/ "8<uart.in",
 /*[IRQ_UART_BYTE_OUT] =*/ "8>uart.out",
};


/*
 * called when a byte is send via the uart on the AVR
 */
static void uart_in_hook(struct avr_irq_t * irq, uint32_t value, void * param);

void
bsim_simavr::pins_reset(void)
{
 for (int p = 0; p < MGetPinCount (); p++)
  {
   pins[p].avalue = 0;
   pins[p].lvalue = 0;
   pins[p].value = 0;
   pins[p].ptype = PT_DIGITAL;
   pins[p].dir = PD_IN;
   pins[p].ovalue = 0;
   pins[p].oavalue = 0;
  }

 //VCC pins
 if (lxString (avr->mmcu).compare (lxT ("atmega2560")) == 0)
  {
   //VCC
   pins[9].value = 1;
   pins[9].ptype = PT_POWER;
   pins[30].value = 1;
   pins[30].ptype = PT_POWER;
   pins[60].value = 1;
   pins[60].ptype = PT_POWER;
   pins[79].value = 1;
   pins[79].ptype = PT_POWER;
   pins[99].value = 1;
   pins[99].ptype = PT_POWER;
   //GND
   pins[10].ptype = PT_POWER;
   pins[31].ptype = PT_POWER;
   pins[61].ptype = PT_POWER;
   pins[80].ptype = PT_POWER;
   pins[98].ptype = PT_POWER;
  }
 else if (lxString (avr->mmcu).compare (lxT ("attiny85")) == 0)
  {
   //VCC
   pins[7].value = 1;
   pins[7].ptype = PT_POWER;
   //GND
   pins[3].ptype = PT_POWER;
  }
 else//atmega328p
  {
   //VCC
   pins[6].value = 1;
   pins[6].ptype = PT_POWER;
   pins[19].value = 1;
   pins[19].ptype = PT_POWER;
   //GND
   pins[7].ptype = PT_POWER;
   pins[21].ptype = PT_POWER;
   //AREF
   pins[20].ptype = PT_ANAREF;
  }
}

void
avr_callback_sleep_raw_(avr_t *avr, avr_cycle_count_t how_long) { }

static const unsigned char AVR_PORTS[12] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L'};

int
bsim_simavr::MInit(const char * processor, const char * fname, float freq)
{
 int ret;
 lxString sproc = GetSupportedDevices ();
 //avr_ioport_external_t p;

 avr = NULL;
 if (sproc.Contains (processor))
  {
   avr = avr_make_mcu_by_name (processor);
  }

 if (!avr)
  {
   printf ("PICSimLab: Unknown processor %s ! Loading Default\n", processor);
   avr = avr_make_mcu_by_name ("atmega328p");
   Proc = "atmega328p";
  }


 avr_init (avr);

 avr->sleep = avr_callback_sleep_raw_;

 //using ee =0 ioctl return the pointer to internal eeprom data instead load values in the pointer 
 avr_eeprom_desc_t epromd;
 epromd.ee = 0;
 epromd.offset = 0;
 epromd.size = avr->e2end + 1;

 if (avr_ioctl (avr, AVR_IOCTL_EEPROM_GET, &epromd))
  {
   eeprom = epromd.ee;
  }
 else
  {
   eeprom = NULL;
  }

 ret = read_ihx_avr (fname, 1);

 avr->frequency = freq;

 if (lxString (avr->mmcu).compare (lxT ("atmega2560")) == 0)
  {
   avr->reset_pc = 0x3E000;
  }
 else if (((lxString (avr->mmcu).compare (lxT ("atmega328p")) == 0)) || ((lxString (avr->mmcu).compare (lxT ("atmega328")) == 0)))
  {
   avr->reset_pc = 0x07000; // bootloader 0x3800
  }
 else
  {
   avr->reset_pc = 0x0000;
  }

 avr->avcc = 5000;

 //avr->log= LOG_DEBUG;

 avr_reset (avr);
 avr->data[UCSR0B] = 0x00; //FIX the simavr reset TX enabled
 pins_reset ();

 /*
 //external pull-up for i2c
 p.mask =  0x30;
 p.value = 0x30;
 avr_ioctl(avr, AVR_IOCTL_IOPORT_SET_EXTERNAL('C'), &p);
  */

 for (int p = 0; p < MGetPinCount (); p++)
  {
   char pname[20];
   strncpy (pname, (const char *) MGetPinName (p + 1).c_str (), 19);
   if (pname[0] == 'P')
    {
     pins[p].port = (unsigned char *) &AVR_PORTS[pname[1] - 'A'];
     pins[p].pord = pname[2] - '0';

     avr_irq_t* stateIrq = avr_io_getirq (avr, AVR_IOCTL_IOPORT_GETIRQ (*pins[p].port), pins[p].pord);
     avr_irq_register_notify (stateIrq, out_hook, &pins[p]);

     avr_irq_t* directionIrq = avr_io_getirq (avr, AVR_IOCTL_IOPORT_GETIRQ (*pins[p].port), IOPORT_IRQ_DIRECTION_ALL);
     avr_irq_register_notify (directionIrq, ddr_hook, &pins[p]);

     const char* name[1];
     name[0] = pname;
     Write_stat_irq[p] = avr_alloc_irq (&avr->irq_pool, 0, 1, name);

     avr_irq_t* writeIrq = avr_io_getirq (avr, AVR_IOCTL_IOPORT_GETIRQ (*pins[p].port), pins[p].pord);
     avr_connect_irq (Write_stat_irq[p], writeIrq);

    }
   else
    {
     pins[p].port = 0;
     pins[p].pord = -1;
     Write_stat_irq[p] = NULL;
    }
  }



 // disable the uart stdio 
 uint32_t f = 0;
 avr_ioctl (avr, AVR_IOCTL_UART_GET_FLAGS ('0'), &f);
 f &= ~AVR_UART_FLAG_STDIO;
 f &= ~AVR_UART_FLAG_POLL_SLEEP;
 avr_ioctl (avr, AVR_IOCTL_UART_SET_FLAGS ('0'), &f);

 serial_irq = avr_alloc_irq (&avr->irq_pool, 0, IRQ_UART_COUNT, irq_names_uart);
 avr_irq_register_notify (serial_irq + IRQ_UART_BYTE_IN, uart_in_hook, this);

 avr_irq_t * src = avr_io_getirq (avr, AVR_IOCTL_UART_GETIRQ ('0'), UART_IRQ_OUTPUT);
 avr_irq_t * dst = avr_io_getirq (avr, AVR_IOCTL_UART_GETIRQ ('0'), UART_IRQ_INPUT);
 if (src && dst)
  {
   avr_connect_irq (src, serial_irq + IRQ_UART_BYTE_IN);
   avr_connect_irq (serial_irq + IRQ_UART_BYTE_OUT, dst);
  }

 serial_port_open (&serialfd, SERIALDEVICE);

 serialexbaud = 9600;
 serialbaud = serial_port_cfg (serialfd, serialexbaud);

 bitbang_uart_init (&bb_uart);

 bitbang_uart_set_speed (&bb_uart, serialexbaud);
 bitbang_uart_set_clk_freq (&bb_uart, freq);

 pin_rx = 2; //PD0
 pin_tx = 3; //PD1  
 uart_config = 0;
 return ret;
}

void
bsim_simavr::MEnd(void)
{
 if (avr_debug_type)
  {
   avr_deinit_gdb (avr);
  }
 else
  {
   mplabxd_end ();
  }

 serial_port_close (serialfd);

 avr_terminate (avr);


 for (int i = 0; i < MGetPinCount (); i++)
  avr_free_irq (Write_stat_irq[i], 1);

 avr_free_irq (serial_irq, IRQ_UART_COUNT);


 for (int i = 0; i < avr->irq_pool.count; i++)
  {
   if (avr->irq_pool.irq[i])
    {
     if (avr->irq_pool.irq[i]->name)
      {
       free ((void*) avr->irq_pool.irq[i]->name);
       avr->irq_pool.irq[i]->name = NULL;
      }
     if (avr->irq_pool.irq[i]->hook)
      {
       free ((void*) avr->irq_pool.irq[i]->hook);
       avr->irq_pool.irq[i]->hook = NULL;
      }
    }
  }

 for (int i = 0; i < 100; i++)
  {
   if (avr->io[i].irq)
    {
     free (avr->io[i].irq);
     avr->io[i].irq = NULL;
    }

  }

 free ((void*) avr->irq_pool.irq);
 free (avr);
 avr = NULL;

 bitbang_uart_end (&bb_uart);
}

void
bsim_simavr::MEraseFlash(void)
{
 //pic_erase_flash();
 memset (avr->flash, 0xFF, avr->flashend);
}

void
bsim_simavr::MSetFreq(float freq)
{
 if (avr)
  {
   avr->frequency = freq;
   bitbang_uart_set_clk_freq (&bb_uart, freq);
  }
}

float
bsim_simavr::MGetFreq(void)
{
 return avr->frequency;
}

float
bsim_simavr::MGetInstClockFreq(void)
{
 return avr->frequency;
}

void
bsim_simavr::MDumpMemory(const char * fname)
{
 write_ihx_avr (fname);
}

void
avr_callback_run_gdb_(avr_t * avr)
{
 avr_gdb_t * g = avr->gdb;

 if (avr->state == cpu_Running &&
     gdb_watch_find (&g->breakpoints, avr->pc) != -1)
  {
   gdb_send_quick_status (g, 0);
   avr->state = cpu_Stopped;
  }
 else if (avr->state == cpu_StepDone)
  {
   gdb_send_quick_status (g, 0);
   avr->state = cpu_Stopped;
  }

 // this also sleeps for a bit
 //gdb_network_handler(g, 0);

 if (avr->state == cpu_Stopped)
  {
   gdb_network_handler (g, 0);
   return;
  }

 // if we are stepping one instruction, we "run" for one..
 int step = avr->state == cpu_Step;
 if (step)
  avr->state = cpu_Running;

 avr_callback_run_raw (avr);


 // if we were stepping, use this state to inform remote gdb
 if (step)
  avr->state = cpu_StepDone;

}

int
bsim_simavr::DebugInit(int dtyppe)
{
 avr_debug_type = dtyppe;

 if (avr_debug_type)
  {
   avr->gdb_port = Window1.Get_debug_port ();
   if (avr_gdb_init (avr))
    {
     return -1;
    }
   else
    {
     avr->run = avr_callback_run_gdb_;
     avr->sleep = avr_callback_sleep_raw_;
     return 1;
    }
  }
 else
  {
   return !mplabxd_init (this, Window1.Get_debug_port ()) - 1;
  }
}

lxString
bsim_simavr::GetDebugName(void)
{
 if (avr_debug_type)
  {
   return "GDB";
  }
 else
  {
   return "MDB";
  }
}

void
bsim_simavr::DebugLoop(void)
{
 if (Window1.Get_mcupwr ())
  {
   if ((avr_debug_type)&&(avr->gdb))
    {
     // this also sleeps for a bit
     gdb_network_handler (avr->gdb, 0);
    }
   else
    {
     mplabxd_loop ();
    }
  }
}

int
bsim_simavr::CpuInitialized(void)
{
 return (avr != NULL);
}

int
bsim_simavr::MGetPinCount(void)
{
 if (avr == NULL) return 0;
 if (lxString (avr->mmcu).compare (lxT ("atmega328")) == 0)return 28;
 if (lxString (avr->mmcu).compare (lxT ("atmega328p")) == 0)return 28;
 if (lxString (avr->mmcu).compare (lxT ("atmega2560")) == 0)return 100;
 if (lxString (avr->mmcu).compare (lxT ("attiny85")) == 0)return 8;
 return 0;
}

lxString
bsim_simavr::MGetPinName(int pin)
{
 if (pin <= 0 || pin > MGetPinCount ())return "error";

 if (lxString (avr->mmcu).compare (lxT ("atmega2560")) == 0)
  {
   switch (pin)
    {
    case 1:
     return "PG5/~4 ";
     break;
    case 2:
     return "PE0/~0";
     break;
    case 3:
     return "PE1/~1";
     break;
    case 4:
     return "PE2";
     break;
    case 5:
     return "PE3/~5";
     break;
    case 6:
     return "PE4/~2";
     break;
    case 7:
     return "PE5/~3";
     break;
    case 8:
     return "PE6";
     break;
    case 9:
     return "PE7";
     break;
    case 10:
     return "+5V";
     break;
    case 11:
     return "GND";
     break;
    case 12:
     return "PH0/17";
     break;
    case 13:
     return "PH1/16";
     break;
    case 14:
     return "PH2";
     break;
    case 15:
     return "PH3/~6";
     break;
    case 16:
     return "PH4/~7";
     break;
    case 17:
     return "PH5/~8";
     break;
    case 18:
     return "PH6/~9";
     break;
    case 19:
     return "PB0/53";
     break;
    case 20:
     return "PB1/52";
     break;
    case 21:
     return "PB2/51";
     break;
    case 22:
     return "PB3/50";
     break;
    case 23:
     return "PB4/~10";
     break;
    case 24:
     return "PB5/~11";
     break;
    case 25:
     return "PB6/~12";
     break;
    case 26:
     return "PB7/~13";
     break;
    case 27:
     return "PH7";
     break;
    case 28:
     return "PG3";
     break;
    case 29:
     return "PG4";
     break;
     //case 30:
     //return "RESET";
     //break;
    case 31:
     return "+5V";
     break;
    case 32:
     return "GND";
     break;
    case 33:
     return "XTAL2";
     break;
    case 34:
     return "XTAL1";
     break;
    case 35:
     return "PL0/49";
     break;
    case 36:
     return "PL1/48";
     break;
    case 37:
     return "PL2/47";
     break;
    case 38:
     return "PL3/46";
     break;
    case 39:
     return "PL4/45";
     break;
    case 40:
     return "PL5/44";
     break;
    case 41:
     return "PL6/43";
     break;
    case 42:
     return "PL7/42";
     break;
    case 43:
     return "PD0/21";
     break;
    case 44:
     return "PD1/20";
     break;
    case 45:
     return "PD2/19";
     break;
    case 46:
     return "PD3/18";
     break;
    case 47:
     return "PD4";
     break;
    case 48:
     return "PD5";
     break;
    case 49:
     return "PD6";
     break;
    case 50:
     return "PD7/38";
     break;
    case 51:
     return "PG0/41";
     break;
    case 52:
     return "PG1/40";
     break;
    case 53:
     return "PC0/37";
     break;
    case 54:
     return "PC1/36";
     break;
    case 55:
     return "PC2/35";
     break;
    case 56:
     return "PC3/34";
     break;
    case 57:
     return "PC4/33";
     break;
    case 58:
     return "PC5/32";
     break;
    case 59:
     return "PC6/31";
     break;
    case 60:
     return "PC7/30";
     break;
    case 61:
     return "+5V";
     break;
    case 62:
     return "GND";
     break;
    case 63:
     return "PJ0/15";
     break;
    case 64:
     return "PJ1/14";
     break;
    case 65:
     return "PJ2";
     break;
    case 66:
     return "PJ3";
     break;
    case 67:
     return "PJ4";
     break;
    case 68:
     return "PJ5";
     break;
    case 69:
     return "PJ6";
     break;
    case 70:
     return "PG2/39";
     break;
    case 71:
     return "PA7/29";
     break;
    case 72:
     return "PA6/28";
     break;
    case 73:
     return "PA5/27";
     break;
    case 74:
     return "PA4/26";
     break;
    case 75:
     return "PA3/25";
     break;
    case 76:
     return "PA2/24";
     break;
    case 77:
     return "PA1/23";
     break;
    case 78:
     return "PA0/22";
     break;
    case 79:
     return "PJ7";
     break;
    case 80:
     return "+5V";
     break;
    case 81:
     return "GND";
     break;
    case 82:
     return "PK7/A15";
     break;
    case 83:
     return "PK6/A14";
     break;
    case 84:
     return "PK5/A13";
     break;
    case 85:
     return "PK4/A12";
     break;
    case 86:
     return "PK3/A11";
     break;
    case 87:
     return "PK2/A10";
     break;
    case 88:
     return "PK1/A9";
     break;
    case 89:
     return "PK0/A8";
     break;
    case 90:
     return "PF7/A7";
     break;
    case 91:
     return "PF6/A6";
     break;
    case 92:
     return "PF5/A5";
     break;
    case 93:
     return "PF4/A4";
     break;
    case 94:
     return "PF3/A3";
     break;
    case 95:
     return "PF2/A2";
     break;
    case 96:
     return "PF1/A1";
     break;
    case 97:
     return "PF0/A0";
     break;
    case 98:
     return "AREF";
     break;
    case 99:
     return "GND";
     break;
    case 100:
     return "+5V";
     break;
    }
  }
 else if (lxString (avr->mmcu).compare (lxT ("attiny85")) == 0)
  {
   switch (pin)
    {
    case 1:
     return "PB5/4";
     break;
    case 2:
     return "PB3/3";
     break;
    case 3:
     return "PB4/5";
     break;
    case 4:
     return "GND";
     break;
    case 5:
     return "PB0/0";
     break;
    case 6:
     return "PB1/1";
     break;
    case 7:
     return "PB2/2";
     break;
    case 8:
     return "+5V";
     break;
    }
  }
 else
  {
   switch (pin)
    {
    case 1:
     return "PC6/RST";
     break;
    case 2:
     return "PD0/0";
     break;
    case 3:
     return "PD1/1";
     break;
    case 4:
     return "PD2/2";
     break;
    case 5:
     return "PD3/~3";
     break;
    case 6:
     return "PD4/4";
     break;
    case 7:
     return "+5V";
     break;
    case 8:
     return "GND";
     break;
    case 9:
     return "PB6/X1";
     break;
    case 10:
     return "PB7/X2";
     break;
    case 11:
     return "PD5/~5";
     break;
    case 12:
     return "PD6/~6";
     break;
    case 13:
     return "PD7/7";
     break;
    case 14:
     return "PB0/8";
     break;
    case 15:
     return "PB1/~9";
     break;
    case 16:
     return "PB2/~10";
     break;
    case 17:
     return "PB3/~11";
     break;
    case 18:
     return "PB4/12";
     break;
    case 19:
     return "PB5/13";
     break;
    case 20:
     return "+5V";
     break;
    case 21:
     return "AREF";
     break;
    case 22:
     return "GND";
     break;
    case 23:
     return "PC0/A0";
     break;
    case 24:
     return "PC1/A1";
     break;
    case 25:
     return "PC2/A2";
     break;
    case 26:
     return "PC3/A3";
     break;
    case 27:
     return "PC4/A4";
     break;
    case 28:
     return "PC5/A5";
     break;
    }
  }
 return "error";
}

void
bsim_simavr::MSetPin(int pin, unsigned char value)
{
 if (pin <= 0 || pin > MGetPinCount ())return;
 if (avr == NULL) return;
 if (!pins[pin - 1].dir)return;
 if (pins[pin - 1].value == value)return;
 if (Write_stat_irq[pin - 1] == NULL) return;
 avr_raise_irq (Write_stat_irq[pin - 1], value);
}

void
bsim_simavr::MSetPinDOV(int pin, unsigned char ovalue)
{
 if (pin <= 0 || pin > MGetPinCount ())return;
 if (avr == NULL) return;
 if (!pins[pin - 1].dir)return;
 if (pins[pin - 1].ovalue == ovalue)return;
 //TODO default output value is not used yet (DOV)
 pins[pin - 1].ovalue = ovalue;
}

void
bsim_simavr::MSetAPin(int pin, float value)
{
 if (pin <= 0 || pin > MGetPinCount ())return;
 if (!pins[pin - 1].dir)return;
 if (pins[pin - 1].avalue == value)return;

 pins[pin - 1].avalue = value;
 if (avr == NULL) return;

 if (lxString (avr->mmcu).compare (lxT ("atmega2560")) == 0)
  {
   switch (pin)
    {
    case 82:
     pins[pin - 1].ptype = PT_ANALOG;
     avr_raise_irq (avr_io_getirq (avr, AVR_IOCTL_ADC_GETIRQ, 15), (int) (value * 1000));
     break;
    case 83:
     pins[pin - 1].ptype = PT_ANALOG;
     avr_raise_irq (avr_io_getirq (avr, AVR_IOCTL_ADC_GETIRQ, 14), (int) (value * 1000));
     break;
    case 84:
     pins[pin - 1].ptype = PT_ANALOG;
     avr_raise_irq (avr_io_getirq (avr, AVR_IOCTL_ADC_GETIRQ, 13), (int) (value * 1000));
     break;
    case 85:
     pins[pin - 1].ptype = PT_ANALOG;
     avr_raise_irq (avr_io_getirq (avr, AVR_IOCTL_ADC_GETIRQ, 12), (int) (value * 1000));
     break;
    case 86:
     pins[pin - 1].ptype = PT_ANALOG;
     avr_raise_irq (avr_io_getirq (avr, AVR_IOCTL_ADC_GETIRQ, 11), (int) (value * 1000));
     break;
    case 87:
     pins[pin - 1].ptype = PT_ANALOG;
     avr_raise_irq (avr_io_getirq (avr, AVR_IOCTL_ADC_GETIRQ, 10), (int) (value * 1000));
     break;
    case 88:
     pins[pin - 1].ptype = PT_ANALOG;
     avr_raise_irq (avr_io_getirq (avr, AVR_IOCTL_ADC_GETIRQ, 9), (int) (value * 1000));
     break;
    case 89:
     pins[pin - 1].ptype = PT_ANALOG;
     avr_raise_irq (avr_io_getirq (avr, AVR_IOCTL_ADC_GETIRQ, 8), (int) (value * 1000));
     break;
    case 90:
     pins[pin - 1].ptype = PT_ANALOG;
     avr_raise_irq (avr_io_getirq (avr, AVR_IOCTL_ADC_GETIRQ, 7), (int) (value * 1000));
     break;
    case 91:
     pins[pin - 1].ptype = PT_ANALOG;
     avr_raise_irq (avr_io_getirq (avr, AVR_IOCTL_ADC_GETIRQ, 6), (int) (value * 1000));
     break;
    case 92:
     pins[pin - 1].ptype = PT_ANALOG;
     avr_raise_irq (avr_io_getirq (avr, AVR_IOCTL_ADC_GETIRQ, 5), (int) (value * 1000));
     break;
    case 93:
     pins[pin - 1].ptype = PT_ANALOG;
     avr_raise_irq (avr_io_getirq (avr, AVR_IOCTL_ADC_GETIRQ, 4), (int) (value * 1000));
     break;
    case 94:
     pins[pin - 1].ptype = PT_ANALOG;
     avr_raise_irq (avr_io_getirq (avr, AVR_IOCTL_ADC_GETIRQ, 3), (int) (value * 1000));
     break;
    case 95:
     pins[pin - 1].ptype = PT_ANALOG;
     avr_raise_irq (avr_io_getirq (avr, AVR_IOCTL_ADC_GETIRQ, 2), (int) (value * 1000));
     break;
    case 96:
     pins[pin - 1].ptype = PT_ANALOG;
     avr_raise_irq (avr_io_getirq (avr, AVR_IOCTL_ADC_GETIRQ, 1), (int) (value * 1000));
     break;
    case 97:
     pins[pin - 1].ptype = PT_ANALOG;
     avr_raise_irq (avr_io_getirq (avr, AVR_IOCTL_ADC_GETIRQ, 0), (int) (value * 1000));
     break;
    }
  }
 else
  {
   switch (pin)
    {
    case 23:
     pins[pin - 1].ptype = PT_ANALOG;
     avr_raise_irq (avr_io_getirq (avr, AVR_IOCTL_ADC_GETIRQ, 0), (int) (value * 1000));
     break;
    case 24:
     pins[pin - 1].ptype = PT_ANALOG;
     avr_raise_irq (avr_io_getirq (avr, AVR_IOCTL_ADC_GETIRQ, 1), (int) (value * 1000));
     break;
    case 25:
     pins[pin - 1].ptype = PT_ANALOG;
     avr_raise_irq (avr_io_getirq (avr, AVR_IOCTL_ADC_GETIRQ, 2), (int) (value * 1000));
     break;
    case 26:
     pins[pin - 1].ptype = PT_ANALOG;
     avr_raise_irq (avr_io_getirq (avr, AVR_IOCTL_ADC_GETIRQ, 3), (int) (value * 1000));
     break;
    case 27:
     pins[pin - 1].ptype = PT_ANALOG;
     avr_raise_irq (avr_io_getirq (avr, AVR_IOCTL_ADC_GETIRQ, 4), (int) (value * 1000));
     break;
    case 28:
     pins[pin - 1].ptype = PT_ANALOG;
     avr_raise_irq (avr_io_getirq (avr, AVR_IOCTL_ADC_GETIRQ, 5), (int) (value * 1000));
     break;
    }
  }
}

unsigned char
bsim_simavr::MGetPin(int pin)
{
 if (pin <= 0 || pin > MGetPinCount ())return -1;

 return pins[pin - 1].value;
}

const picpin *
bsim_simavr::MGetPinsValues(void)
{
 return pins;
}

void
bsim_simavr::SerialSend(unsigned char value)
{
 serial_port_send (serialfd, value);
 bitbang_uart_send (&bb_uart, value);
}

/*
 * called when a byte is send via the uart on the AVR
 */
static void
uart_in_hook(struct avr_irq_t * irq, uint32_t value, void * param)
{
 ((bsim_simavr *) param)->SerialSend (value);
}

void
bsim_simavr::UpdateHardware(void)
{
 static int cont = 0;
 static int aux = 1;
 unsigned char c;
 cont++;

 if (cont > 1000)
  {
   cont = 0;

   if (serial_port_get_dsr (serialfd))
    {
     if (aux)
      {
       MReset (0);
       aux = 0;
      }
    }
   else
    {
     aux = 1;
    }

   if (avr->data[UCSR0B] & 0x10)//RXEN
    {
     if (serial_port_rec (serialfd, &c))
      {
       avr_raise_irq (serial_irq + IRQ_UART_BYTE_OUT, c);
      }

     if (bitbang_uart_data_available (&bb_uart))
      {
       unsigned char data = bitbang_uart_recv (&bb_uart);
       //printf ("data recv:%02X  %c\n", data,data);
       avr_raise_irq (serial_irq + IRQ_UART_BYTE_OUT, data);
      }

    }
  }

 if (avr->data[UCSR0B] & 0x18)//RXEN TXEN
  {
   if (!uart_config)
    {
     uart_config = 1;

     if (avr->data[UCSR0A] & 0x02) //U2Xn
      {
       serialexbaud = avr->frequency / (8 * (((avr->data[UBRR0H] << 8) | avr->data[UBRR0L]) + 1));
      }
     else
      {
       serialexbaud = avr->frequency / (16 * (((avr->data[UBRR0H] << 8) | avr->data[UBRR0L]) + 1));
      }


     serialbaud = serial_port_cfg (serialfd, serialexbaud);

     //printf ("baud=%i %f\n", serialbaud, serialexbaud);

     bitbang_uart_init (&bb_uart);

     bitbang_uart_set_speed (&bb_uart, serialexbaud);
     bitbang_uart_set_clk_freq (&bb_uart, avr->frequency);

     pins[pin_rx - 1 ].dir = PD_IN;
     pins[pin_tx - 1 ].dir = PD_OUT;
    }

   pins[pin_tx - 1 ].value = bitbang_uart_io (&bb_uart, pins[pin_rx - 1 ].value);
  }
 else
  {
   uart_config = 0;
  }
}

void
bsim_simavr::MStep(void)
{
 avr_run (avr);
}

void
bsim_simavr::MStepResume(void) { }

int
bsim_simavr::DBGTestBP(unsigned int bp)
{
 return (bp == avr->pc >> 1);
}

void
bsim_simavr::MReset(int flags)
{
 avr_reset (avr);
 avr->data[UCSR0B] = 0x00; //FIX the simavr reset TX enabled
 bitbang_uart_rst (&bb_uart);
}

unsigned short *
bsim_simavr::DBGGetProcID_p(void)
{
 return 0;
}

unsigned int
bsim_simavr::DBGGetPC(void)
{
 return avr->pc >> 1;
}

void
bsim_simavr::DBGSetPC(unsigned int pc)
{
 avr->pc = pc << 1;
}

unsigned char *
bsim_simavr::DBGGetRAM_p(void)
{
 return avr->data;
}

unsigned char *
bsim_simavr::DBGGetROM_p(void)
{
 return avr->flash;
}

unsigned char *
bsim_simavr::DBGGetCONFIG_p(void)
{
 return avr->fuse;
}

unsigned char *
bsim_simavr::DBGGetID_p(void)
{
 //TODO avr ID pointer
 return NULL;
}

unsigned char *
bsim_simavr::DBGGetEEPROM_p(void)
{
 return eeprom;
}

unsigned int
bsim_simavr::DBGGetRAMSize(void)
{
 return avr->ramend + 1;
}

unsigned int
bsim_simavr::DBGGetROMSize(void)
{
 return avr->flashend + 1;
}

unsigned int
bsim_simavr::DBGGetCONFIGSize(void)
{
 //FIXME avr CONFIG size
 return 3;
}

unsigned int
bsim_simavr::DBGGetIDSize(void)
{
 //TODO avr ID size
 return 0;
}

unsigned int
bsim_simavr::DBGGetEEPROM_Size(void)
{
 return avr->e2end + 1;
}

void
bsim_simavr::EndServers(void)
{
 mplabxd_server_end ();
}

//hexfile support ============================================================

int
bsim_simavr::parse_hex(char *line, int bytes)
{
 char snum[200];
 int num;

 strncpy (snum, line, bytes);
 snum[bytes] = '\0';
 sscanf (snum, "%X", &num);

 return num;
}

unsigned char
bsim_simavr::checksum(char* str)
{
 unsigned int i;
 unsigned char acum = 0;

 for (i = 0; i < ((strlen (str) - 1) / 2); i++)
  {
   acum += parse_hex (str + 1 + (i * 2), 2);
  }

 return acum;
}

int
bsim_simavr::read_ihx_avr(const char * fname, int leeprom)
{
 FILE* fin;
 int lc = 0;
 unsigned int bc;
 char line[256];
 unsigned int nbytes, addr, type;
 unsigned int addrx;
 unsigned short addrh = 0;
 //unsigned short addrl=0;
 char *mptr;

 fin = fopen (fname, "r");

 if (fin)
  {
   do
    {
     fgets (line, 256, fin);
     lc++;

     if (strlen (line) == 0) continue;

     /*for dos file*/
     if (line[strlen (line) - 2] == '\r')
      {
       line[strlen (line) - 2] = '\n';
       line[strlen (line) - 1] = 0;
      }

     if (checksum (line) == 0)
      {
       nbytes = parse_hex (line + 1, 2);
       addr = parse_hex (line + 3, 4);
       type = parse_hex (line + 7, 2);

       switch (type)
        {
        case 0:
         //addrl=addr/2;
         /*
                     if(addrh == 0x0030 )
                 {
                       //config
                       mptr=(char*)pic->config;
                       for(bc=0;bc < nbytes;bc++)
                   {
                     addrx=addr+bc;
                         if((addrx/2) < pic->CONFIGSIZE)
                       mptr[addrx]=parse_hex(line+9+(bc*2),2);
                       }
                 }
                     else
                 {
                       if(addrh == 0x00F0 )
                       { 
                         //EEPROM
                         if(leeprom == 1) 
                         for(bc=0;bc < nbytes;bc++)
                     {
                           addrx= addr+bc;
                           if(addrx < avr->flashend)
                         avr->flash[addrx]= parse_hex(line+9+(bc*2),2);
                         }
                       }
                       else
                       {
                         if(addrh == 0x0020 )
                         {
                           //IDS
                           mptr=(char*)pic->id;
                           for(bc=0;bc < nbytes;bc++)
                       {
                         addrx=addr+bc;
                             if((addrx/2) < pic->IDSIZE)
                           mptr[addrx]=parse_hex(line+9+(bc*2),2);
                           }
                         } 
                         else
          */
         if (addrh == 0x0081)
          {
           //EEPROM
           if (leeprom == 1)
            {
             for (bc = 0; bc < nbytes; bc++)
              {
               addrx = addr + bc;
               if (addrx <= avr->e2end)
                {
                 eeprom[addrx] = parse_hex (line + 9 + (bc * 2), 2);
                }
              }
            }
          }
         else
          {
           //prog mem
           mptr = (char*) avr->flash;
           for (bc = 0; bc < nbytes; bc++)
            {
             addrx = (addrh << 16) | (addr + bc);
             if (addrx <= avr->flashend)
              {
               mptr[addrx] = parse_hex (line + 9 + (bc * 2), 2);
              }
            }
          }
         //}
         //}
         break;
        case 1:
         fclose (fin);
         return 0; //no error
         break;
        case 2:
         addrh = (((parse_hex (line + 9, 2) << 8) | parse_hex (line + 11, 2))) >> 12;
         break;
        case 4:
         addrh = ((parse_hex (line + 9, 2) << 8) | parse_hex (line + 11, 2));
         break;
        }
      }
     else
      {
       printf ("ERRO: Picsim->File bad checksum line %i!(%s)\n", lc, fname);
       fclose (fin);
       return HEX_CHKSUM;
      }
    }
   while (!feof (fin));
   fclose (fin);
  }
 else
  {
   printf ("ERRO: Picsim->File not found!(%s)\n", fname);
   return HEX_NFOUND;
  }
 return 0; //no error
}

int
bsim_simavr::write_ihx_avr(const char * fname)
{

 FILE * fout;
 unsigned char sum;
 unsigned char nb;
 unsigned int iaddr = 0;
 unsigned int i;
 char values[100];
 char tmp[200];

 fout = fopen (fname, "w");

 if (fout)
  {
   //program memory  
   nb = 0;
   sum = 0;
   for (i = 0; i <= avr->flashend; i++)
    {
     if (i == 0x10000)fprintf (fout, ":020000040001F9\n");
     if (i == 0x20000)fprintf (fout, ":020000040002F8\n");
     if (i == 0x30000)fprintf (fout, ":020000040003F7\n");

     if (nb == 0)
      {
       iaddr = i & 0xFFFF;
       snprintf (values, 99, "%02X", avr->flash[i]);
      }
     else
      {
       snprintf (tmp, 199, "%s%02X", values, avr->flash[i]);
       strcpy (values, tmp);
      }

     nb++;
     sum += avr->flash[i];

     if (nb == 16)
      {
       sum += nb;
       sum += (iaddr & 0x00FF);
       sum += ((iaddr & 0xFF00) >> 8);
       //printf("sum=%02X %02X %02X\n",sum,~sum,(~sum)+1);
       sum = (~sum) + 1;
       fprintf (fout, ":%02X%04X00%s%02X\n", nb, iaddr, values, sum);
       nb = 0;
       sum = 0;
      }
    }
   if (nb)
    {
     sum += nb;
     sum += (iaddr & 0x00FF);
     sum += ((iaddr & 0xFF00) >> 8);
     sum = (~sum) + 1;
     fprintf (fout, ":%02X%04X00%s%02X\n", nb, iaddr, values, sum);
    }
   /*
   //ids
       nb=0;
       sum=0;
       fprintf(fout,":020000040020DA\n");
       for(i=0;i<pic->IDSIZE;i++)
       {
    
         if(nb==0)
         {
           iaddr=(i*2);
           sprintf(values,"%02X%02X",pic->id[i]&0x00FF ,(pic->id[i]&0xFF00)>>8);
         }
         else
         {
           sprintf(tmp,"%s%02X%02X",values,pic->id[i]&0x00FF,(pic->id[i]&0xFF00)>>8);
           strcpy(values,tmp);
         }

         nb+=2;
         sum+=pic->id[i]&0x00FF;
         sum+=(pic->id[i]&0xFF00)>>8;
         if(nb==16)
         {
           sum+=nb;
           sum+=(iaddr&0x00FF);
           sum+=((iaddr&0xFF00)>>8);
         //printf("sum=%02X %02X %02X\n",sum,~sum,(~sum)+1);
           sum=(~sum)+1; 
           fprintf(fout,":%02X%04X00%s%02X\n",nb,iaddr,values,sum);
           nb=0;
           sum=0;
         }
       }
       if(nb)
       {
           sum+=nb;
           sum+=(iaddr&0x00FF);
           sum+=((iaddr&0xFF00)>>8);
           sum=(~sum)+1; 
           fprintf(fout,":%02X%04X00%s%02X\n",nb,iaddr,values,sum);
       }
    */
   /*    
   //config
       nb=0;
       sum=0;
       fprintf(fout,":020000040030CA\n");
       for(i=0;i<pic->CONFIGSIZE;i++)
       {
    
         if(nb==0)
         {
           iaddr=(i*2);
           sprintf(values,"%02X%02X",pic->config[i]&0x00FF ,(pic->config[i]&0xFF00)>>8);
         }
         else
         {
           sprintf(tmp,"%s%02X%02X",values,pic->config[i]&0x00FF,(pic->config[i]&0xFF00)>>8);
           strcpy(values,tmp);
         }

         nb+=2;
         sum+=pic->config[i]&0x00FF;
         sum+=(pic->config[i]&0xFF00)>>8;
         if(nb==16)
         {
           sum+=nb;
           sum+=(iaddr&0x00FF);
           sum+=((iaddr&0xFF00)>>8);
         //printf("sum=%02X %02X %02X\n",sum,~sum,(~sum)+1);
           sum=(~sum)+1; 
           fprintf(fout,":%02X%04X00%s%02X\n",nb,iaddr,values,sum);
           nb=0;
           sum=0;
         }
       }
       if(nb)
       {
           sum+=nb;
           sum+=(iaddr&0x00FF);
           sum+=((iaddr&0xFF00)>>8);
           sum=(~sum)+1; 
           fprintf(fout,":%02X%04X00%s%02X\n",nb,iaddr,values,sum);
       }
    */
   //eeprom
   nb = 0;
   sum = 0;
   fprintf (fout, ":02000004008179\n");
   for (i = 0; i <= avr->e2end; i++)
    {

     if (nb == 0)
      {
       iaddr = i;
       sprintf (values, "%02X", eeprom[i]);
      }
     else
      {
       sprintf (tmp, "%s%02X", values, eeprom[i]);
       strcpy (values, tmp);
      }

     nb++;
     sum += eeprom[i];
     if (nb == 16)
      {
       sum += nb;
       sum += (iaddr & 0x00FF);
       sum += ((iaddr & 0xFF00) >> 8);
       //printf("sum=%02X %02X %02X\n",sum,~sum,(~sum)+1);
       sum = (~sum) + 1;
       fprintf (fout, ":%02X%04X00%s%02X\n", nb, iaddr, values, sum);
       nb = 0;
       sum = 0;
      }
    }
   if (nb)
    {
     sum += nb;
     sum += (iaddr & 0x00FF);
     sum += ((iaddr & 0xFF00) >> 8);
     sum = (~sum) + 1;
     fprintf (fout, ":%02X%04X00%s%02X\n", nb, iaddr, values, sum);
    }

   //end
   fprintf (fout, ":00000001FF\n");
   fclose (fout);
   return 0; //no error
  }
 else
  {
   printf ("ERRO: Picsim->File not found!(%s)\n", fname);
   return HEX_NWRITE;
  }
 return 0; //no error
}
