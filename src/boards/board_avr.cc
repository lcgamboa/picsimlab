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


#include "board_avr.h"

#include"../picsimlab1.h"


unsigned long avr_serial_send(unsigned char c);
unsigned long avr_serial_rec(unsigned char * c);
int avr_serial_get_dsr(void);
int avr_serial_open(char * SERIALDEVICE);
int avr_serial_cfg(float serialexbaud);
int avr_serial_close(void);

board_avr::board_avr(void)
{
 avr = NULL;
 serial_irq = NULL;
}

void
board_avr::MSetSerial(const char * port) {
 //TODO
 //pic_set_serial(&pic,port,0,0,0);
}

enum
{
 IRQ_UART_BYTE_IN = 0,
 IRQ_UART_BYTE_OUT,
 IRQ_UART_COUNT
};

static const char * irq_names_uart[IRQ_UART_COUNT] = {
 [IRQ_UART_BYTE_IN] = "8<uart.in",
 [IRQ_UART_BYTE_OUT] = "8>uart.out",
};


/*
 * called when a byte is send via the uart on the AVR
 */
static void uart_in_hook(struct avr_irq_t * irq, uint32_t value, void * param);

void
board_avr::pins_reset(void)
{
 for (int p = 0; p < MGetPinCount (); p++)
  {
   pins[p].avalue = 0;
   pins[p].lvalue = 0;
   pins[p].value = 0;
   pins[p].ptype = PT_CMOS;
   pins[p].dir = PD_IN;
   pins[p].ovalue = 0;
   pins[p].oavalue = 0;
  }

 //VCC pins
 pins[6].value = 1;
 pins[19].value = 1;
}

void
avr_callback_sleep_raw_(avr_t *avr, avr_cycle_count_t how_long) { }

int
board_avr::MInit(const char * processor, const char * fname, float freq)
{
 int ret;
 avr = avr_make_mcu_by_name (processor);

 if (!avr)
  {
   fprintf (stderr, "Error creating the AVR core\n");
   avr = avr_make_mcu_by_name ("atmega328");
  }


 avr_init (avr);

 avr->sleep = avr_callback_sleep_raw_;

 ret = read_ihx_avr (fname, 1);

 avr->frequency = freq;

 avr->reset_pc = 0x07000; // bootloader 0x3800

 avr->avcc = 5000;

 //avr->log= LOG_DEBUG;

 avr_reset (avr);

 pins_reset ();


 for (int p = 0; p < MGetPinCount (); p++)
  {
   char pname[20];
   strncpy (pname, (const char *) MGetPinName (p + 1).c_str (), 19);
   if (pname[0] == 'P')
    {
     pins[p].port = pname[1];
     pins[p].pord = pname[2] - '0';

     avr_irq_t* stateIrq = avr_io_getirq (avr, AVR_IOCTL_IOPORT_GETIRQ (pins[p].port), pins[p].pord);
     avr_irq_register_notify (stateIrq, out_hook, &pins[p]);

     avr_irq_t* directionIrq = avr_io_getirq (avr, AVR_IOCTL_IOPORT_GETIRQ (pins[p].port), IOPORT_IRQ_DIRECTION_ALL);
     avr_irq_register_notify (directionIrq, ddr_hook, &pins[p]);

     const char* name[1];
     name[0] = pname;
     Write_stat_irq[p] = avr_alloc_irq (&avr->irq_pool, 0, 1, name);

     avr_irq_t* writeIrq = avr_io_getirq (avr, AVR_IOCTL_IOPORT_GETIRQ (pins[p].port), pins[p].pord);
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
 avr_irq_register_notify (serial_irq + IRQ_UART_BYTE_IN, uart_in_hook, NULL);

 avr_irq_t * src = avr_io_getirq (avr, AVR_IOCTL_UART_GETIRQ ('0'), UART_IRQ_OUTPUT);
 avr_irq_t * dst = avr_io_getirq (avr, AVR_IOCTL_UART_GETIRQ ('0'), UART_IRQ_INPUT);
 if (src && dst)
  {
   avr_connect_irq (src, serial_irq + IRQ_UART_BYTE_IN);
   avr_connect_irq (serial_irq + IRQ_UART_BYTE_OUT, dst);
  }

 avr_serial_open (SERIALDEVICE);

 serialexbaud = 57600; //FIXME read value from avr 
 serialbaud = avr_serial_cfg (serialexbaud);

 return ret;
}

void
board_avr::MEnd(void)
{
 avr_serial_close ();

 avr_terminate (avr);


 for (int i = 0; i < 28; i++)
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

#ifndef AVR_USE_GDB
 mplabxd_end ();
#endif 
}

void
board_avr::MEraseFlash(void)
{
 //pic_erase_flash();
 memset (avr->flash, 0xFF, avr->flashend);
}

void
board_avr::MSetFreq(float freq)
{
 if (avr)
  avr->frequency = freq;
}

float
board_avr::MGetFreq(void)
{
 return avr->frequency;
}

float
board_avr::MGetInstClock(void)
{
 return avr->frequency;
}

void
board_avr::MDumpMemory(const char * fname)
{
 write_ihx_avr (fname);
}

int
board_avr::DebugInit(void)
{
#ifdef AVR_USE_GDB
 avr->gdb_port = 1234;
 //avr->state = cpu_Stopped;
 return avr_gdb_init (avr);
#else
 return mplabxd_init (this);
#endif 
}

void
board_avr::DebugLoop(void) {
#ifndef AVR_USE_GDB
 if (Window1.Get_mcupwr ())
  {
   //prog_loop(&pic);
   mplabxd_loop ();
  }
#endif 
 }

int
board_avr::CpuInitialized(void)
{
 return (avr != NULL);
}

int
board_avr::MGetPinCount(void)
{
 if (avr == NULL) return 0;
 if (String (avr->mmcu).compare (lxT ("atmega328")) == 0)return 28;
 return 0;
}

String
board_avr::MGetPinName(int pin)
{
 if (pin <= 0 || pin > MGetPinCount ())return "error";

 switch (pin)
  {
   //case 1:
   //  return "PC6 (RST)";break;
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
   return "PB6 (OSC1)";
   break;
  case 10:
   return "PB7 (OSC2)";
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
   //case 21:
   //  return "AREF";break;
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

 return "error";
}

void
board_avr::MSetPin(int pin, unsigned char value)
{
 if (pin <= 0 || pin > MGetPinCount ())return;
 if (avr == NULL) return;
 if (!pins[pin - 1].dir)return;
 if (pins[pin - 1].value == value)return;
 if (Write_stat_irq[pin - 1] == NULL) return;
 avr_raise_irq (Write_stat_irq[pin - 1], value);
}

void
board_avr::MSetAPin(int pin, float value)
{
 if (pin <= 0 || pin > MGetPinCount ())return;
 if (!pins[pin - 1].dir)return;
 if (pins[pin - 1].avalue == value)return;

 pins[pin - 1].avalue = value;
 if (avr == NULL) return;

 //FIXME ptype == PT_ANALOG;
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

unsigned char
board_avr::MGetPin(int pin)
{
 if (pin <= 0 || pin > MGetPinCount ())return -1;

 return pins[pin - 1].value;
}

const picpin *
board_avr::MGetPinsValues(void)
{
 return pins;
}



//hexfile support ============================================================


#include<stdio.h>
#include<stdlib.h>
#include<string.h>

int
board_avr::parse_hex(char *line, int bytes)
{
 char snum[200];
 int num;

 strncpy (snum, line, bytes);
 snum[bytes] = '\0';
 sscanf (snum, "%X", &num);

 return num;
};

unsigned char
board_avr::checksum(char* str)
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
board_avr::read_ihx_avr(const char * fname, int leeprom)
{
 FILE* fin;
 int lc = 0;
 unsigned int bc;
 char line[256];
 unsigned int nbytes, addr, type;
 unsigned int addrx;
 //unsigned short addrh=0;
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
         //{
         //prog mem
         mptr = (char*) avr->flash;
         for (bc = 0; bc < nbytes; bc++)
          {
           addrx = addr + bc;
           if (addrx <= avr->flashend)
            mptr[addrx] = parse_hex (line + 9 + (bc * 2), 2);
          }
         //}
         //}
         //}
         break;
        case 1:
         fclose (fin);
         return 0; //no error
         break;
        case 4:
         //addrh=((parse_hex(line+9,2)<<8)|parse_hex(line+11,2));
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
};

int
board_avr::write_ihx_avr(const char * fname)
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
   //program memory  //TODO P18 only address < 64K bytes  
   nb = 0;
   sum = 0;
   //fprintf(fout,":020000040000FA\n");
   for (i = 0; i <= avr->flashend; i++)
    {

     if (nb == 0)
      {
       iaddr = i;
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
   /*  
 //eeprom
     nb=0;
     sum=0;
     fprintf(fout,":0200000400F00A\n");
     for(i=0;i<pic->EEPROMSIZE;i++)
     {
    
       if(nb==0)
       {
         iaddr=i;
         sprintf(values,"%02X",pic->eeprom[i]);
       }
       else
       {
         sprintf(tmp,"%s%02X",values,pic->eeprom[i]);
         strcpy(values,tmp);
       }

       nb++;
       sum+=pic->eeprom[i];
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

/*
 * called when a byte is send via the uart on the AVR
 */
static void
uart_in_hook(struct avr_irq_t * irq, uint32_t value, void * param)
{
 avr_serial_send (value);
}

int cont = 0;
int aux = 1;

void
board_avr::UpdateHardware(void)
{

 unsigned char c;
 cont++;

 if (cont > 1000)//FIXME : correct the baud rate
  {
   cont = 0;
   if (avr_serial_rec (&c))
    {
     avr_raise_irq (serial_irq + IRQ_UART_BYTE_OUT, c);
    }

   if (avr_serial_get_dsr ())
    {
     if (aux)
      {
       avr_reset (avr);
       pins_reset ();
       aux = 0;
      }
    }
   else
    {
     aux = 1;
    }
  }

}


void 
board_avr::MStep(void)
{
      avr_run (avr);
}

void 
board_avr::MStepResume(void)
{
}

int 
board_avr::MTestBP(unsigned short bp)
{
 return (bp== avr->pc>>1);
}

void 
board_avr::MReset(int flags)
{
 avr_reset (avr);
}

unsigned short * 
board_avr::MGetProcID_p(void)
{
 return 0;
}

unsigned short  
board_avr::MGetPC(void)
{
 return avr->pc>>1;
}

void  
board_avr::MSetPC(unsigned short pc)
{
 avr->pc= pc << 1;
}


unsigned char * 
board_avr::MGetRAM_p(void)
{
 return avr->data;
}

unsigned char * 
board_avr::MGetROM_p(void)
{
 return avr->flash;
}

unsigned char * 
board_avr::MGetCONFIG_p(void)
{
 return avr->fuse;
}

unsigned char * 
board_avr::MGetID_p(void)
{
 //TODO
 return NULL; 
}

unsigned char * 
board_avr::MGetEEPROM_p(void)
{
 //TODO
 return NULL;
}

unsigned int 
board_avr::MGetRAMSize(void)
{
 return avr->ramend+1;
}

unsigned int 
board_avr::MGetROMSize(void)
{
 return avr->flashend+1;
}

unsigned int 
board_avr::MGetCONFIGSize(void)
{
 return 3; //FIXME
}

unsigned int 
board_avr::MGetIDSize(void)
{
 //TODO
 return 0;
}

unsigned int 
board_avr::MGetEEPROM_Size(void)
{
 //TODO
 return 0;
}