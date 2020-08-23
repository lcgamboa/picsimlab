
#include"bridge_gpsim.h"

#include <gpsim/sim_context.h>

// GpSim includes
#include <gpsim/cod.h>
#include <gpsim/interface.h>
#include <gpsim/gpsim_classes.h>
#include <gpsim/pic-processor.h>
#include <gpsim/uart.h>
#include <gpsim/pir.h>
#include <gpsim/eeprom.h>
#include <gpsim/packages.h>
#include <gpsim/stimuli.h>

void simulation_cleanup(void);


static pic_processor *gpic;

int
bridge_gpsim_init(const char * processor, const char *fileName, float freq)
{
 //initialize_gpsim_core ();
 //initialization_is_complete ();
 Processor *tempProc = 0l;
 CSimulationContext::GetContext ()->LoadProgram (fileName, processor, &tempProc, NULL);
 gpic = dynamic_cast<pic_processor*> (tempProc);

 gpic->set_frequency (freq);

 return (gpic != NULL) - 1;
}

void
bridge_gpsim_reset(void)
{
 if (gpic->is_sleeping ()) gpic->exit_sleep ();
 gpic->reset (POR_RESET);
}

unsigned char
bridge_gpsim_get_pin_count(void)
{
 return gpic->get_pin_count ();
}

const char *
bridge_gpsim_get_pin_name(int pin)
{
 return gpic->get_pin_name (pin).c_str ();
}

unsigned char
bridge_gpsim_get_pin_value(int pin)
{
 return gpic->get_pin_state (pin);
}

unsigned char
bridge_gpsim_get_pin_dir(int pin)
{
 IOPIN * iopin = gpic->get_pin (pin);

 if (iopin)
  {
   return (iopin->get_direction () == IOPIN::DIR_INPUT);
  }

 return 0;
}

void
bridge_gpsim_set_pin_value(int pin, unsigned char value)
{
 if (value)
  {
   gpic->package->get_pin (1)->setDrivingState (true);
  }
 else
  {
   gpic->package->get_pin (1)->setDrivingState (false);
  }
}

void
bridge_gpsim_step(void)
{
 gpic->step_cycle ();
}

void
bridge_gpsim_end(void)
{
 CSimulationContext::GetContext ()->Clear ();
 //simulation_cleanup ();
 //Ugly hack to permit gpsim restart
 //gpsim don't support restart yet
 //https://sourceforge.net/p/gpsim/bugs/163/
 for(int i=0;i< 1000;i++)
  {
    get_interface().remove_interface (i);
  }
}

char *
bridge_gpsim_get_processor_list(char * buff, unsigned int size)
{
 std::list <ProcessorConstructor *>::iterator processor_iterator;
 ProcessorConstructorList *pl = ProcessorConstructor::GetList ();
 ProcessorConstructor *p;

 buff[0] = 0;
 // Print the name of each processor.

 for (processor_iterator = pl->begin (); processor_iterator != pl->end (); processor_iterator++)
  {

   p = *processor_iterator;

   strncat (buff, p->names[1], size);
   strncat (buff, ",", size);
  }

 return buff;
}

static int write_hex(unsigned char * mem, unsigned int size, const char * fname);

int
bridge_gpsim_dump_memory(const char * fname)
{
 unsigned char * mem;
 unsigned short val;
 unsigned int memsize = gpic->program_memory_size ()*2;

 mem = (unsigned char *) malloc (memsize);

 for (unsigned int i = 0; i < memsize; i += 2)
  {
   val = gpic->program_memory[i / 2]->get_value ();
   mem[i + 1] = (val & 0xFF00) >> 8;
   mem[i    ] = (val & 0x00FF);
  }

 int ret = write_hex (mem, memsize, fname);

 free (mem);
 return ret;
}

static int
write_hex(unsigned char * mem, unsigned int size, const char * fname)
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
   for (i = 0; i < size; i++)
    {
     if (i == 0x00000)fprintf (fout, ":020000040000FA\n");
     if (i == 0x10000)fprintf (fout, ":020000040001F9\n");
     if (i == 0x20000)fprintf (fout, ":020000040002F8\n");
     if (i == 0x30000)fprintf (fout, ":020000040003F7\n");

     if (nb == 0)
      {
       iaddr = i & 0xFFFF;
       snprintf (values, 99, "%02X", mem[i]);
      }
     else
      {
       snprintf (tmp, 199, "%s%02X", values, mem[i]);
       strcpy (values, tmp);
      }

     nb++;
     sum += mem[i];

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
   printf ("ERRO: File not found!(%s)\n", fname);
   return -1;
  }
 return 0; //no error
}