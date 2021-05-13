/* ########################################################################

   PICsimLab - PIC laboratory simulator

   ########################################################################

   Copyright (c) : 2015-2021  Luis Claudio Gambôa Lopes

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

//include files
#include"../picsimlab1.h"
#include"../picsimlab4.h" //Oscilloscope
#include"../picsimlab5.h" //Spare Parts
#include"board_Breadboard.h"

/* ids of inputs of input map*/
enum
{
 I_ICSP, //ICSP connector
 I_PWR, //Power button
 I_RST //Reset button
};

/* ids of outputs of output map*/
enum
{
 O_LPWR, //Power LED
 O_MP, //uController name
 O_RST //Reset button    
};
//return the input ids numbers of names used in input map

unsigned short
cboard_Breadboard::get_in_id(char * name)
{
 if (strcmp (name, "PG_ICSP") == 0)return I_ICSP;
 if (strcmp (name, "SW_PWR") == 0)return I_PWR;
 if (strcmp (name, "PB_RST") == 0)return I_RST;

 printf ("Erro input '%s' don't have a valid id! \n", name);
 return -1;
}

//return the output ids numbers of names used in output map

unsigned short
cboard_Breadboard::get_out_id(char * name)
{

 if (strcmp (name, "LD_LPWR") == 0)return O_LPWR;
 if (strcmp (name, "MP_CPU") == 0)return O_MP;
 if (strcmp (name, "PB_RST") == 0)return O_RST;

 printf ("Erro output '%s' don't have a valid id! \n", name);
 return 1;
}

//Constructor called once on board creation 

cboard_Breadboard::cboard_Breadboard(void) :
font(10, lxFONTFAMILY_TELETYPE, lxFONTSTYLE_NORMAL, lxFONTWEIGHT_BOLD)
{
 ptype = _PIC;
 Proc = "PIC18F4550"; //default microcontroller if none defined in preferences
 ReadMaps (); //Read input and output board maps

 lxImage image (&Window1);
 image.LoadFile (Window1.GetSharePath () + lxT ("boards/Common/ic40.svg"), 0, Scale, Scale, 1);
 micbmp = new lxBitmap (&image, &Window1);

}

//Destructor called once on board destruction 

cboard_Breadboard::~cboard_Breadboard(void)
{
 delete micbmp;
 micbmp = NULL;
}

//Reset board status

void
cboard_Breadboard::Reset(void)
{

 switch (ptype)
  {
  case _PIC:

   pic_reset (1);


   //verify serial port state and refresh status bar  
#ifndef _WIN_
   if (pic.serial[0].serialfd > 0)
#else
   if (pic.serial[0].serialfd != INVALID_HANDLE_VALUE)
#endif
    Window1.statusbar1.SetField (2, lxT ("Serial: ") +
                                 lxString::FromAscii (SERIALDEVICE) + lxT (":") + itoa (pic.serial[0].serialbaud) + lxT ("(") +
                                 lxString ().Format ("%4.1f", fabs ((100.0 * pic.serial[0].serialexbaud - 100.0 *
                                                                     pic.serial[0].serialbaud) / pic.serial[0].serialexbaud)) + lxT ("%)"));
   else
    Window1.statusbar1.SetField (2, lxT ("Serial: ") +
                                 lxString::FromAscii (SERIALDEVICE) + lxT (" (ERROR)"));
   break;
  case _AVR:
   avr_reset (avr);
   avr->data[UCSR0B] = 0x00; //FIX the simavr reset TX enabled

   //verify serial port state and refresh status bar  
#ifndef _WIN_
   if (serialfd > 0)
#else
   if (serialfd != INVALID_HANDLE_VALUE)
#endif
    Window1.statusbar1.SetField (2, lxT ("Serial: ") +
                                 lxString::FromAscii (SERIALDEVICE) + lxT (":") + itoa (serialbaud) + lxT ("(") +
                                 lxString ().Format ("%4.1f", fabs ((100.0 * serialexbaud - 100.0 *
                                                                     serialbaud) / serialexbaud)) + lxT ("%)"));
   else
    Window1.statusbar1.SetField (2, lxT ("Serial: ") +
                                 lxString::FromAscii (SERIALDEVICE) + lxT (" (ERROR)"));
   break;

  }

 if (use_spare)Window5.Reset ();
}

//Called ever 1s to refresh status

void
cboard_Breadboard::RefreshStatus(void)
{

 switch (ptype)
  {
  case _PIC:
   //verify serial port state and refresh status bar   
#ifndef _WIN_
   if (pic.serial[0].serialfd > 0)
#else
   if (pic.serial[0].serialfd != INVALID_HANDLE_VALUE)
#endif
    Window1.statusbar1.SetField (2, lxT ("Serial: ") +
                                 lxString::FromAscii (SERIALDEVICE) + lxT (":") + itoa (pic.serial[0].serialbaud) + lxT ("(") +
                                 lxString ().Format ("%4.1f", fabs ((100.0 * pic.serial[0].serialexbaud - 100.0 *
                                                                     pic.serial[0].serialbaud) / pic.serial[0].serialexbaud)) + lxT ("%)"));
   else
    Window1.statusbar1.SetField (2, lxT ("Serial: ") +
                                 lxString::FromAscii (SERIALDEVICE) + lxT (" (ERROR)"));
   break;
  case _AVR:
   //verify serial port state and refresh status bar   
#ifndef _WIN_
   if (serialfd > 0)
#else
   if (serialfd != INVALID_HANDLE_VALUE)
#endif
    Window1.statusbar1.SetField (2, lxT ("Serial: ") +
                                 lxString::FromAscii (SERIALDEVICE) + lxT (":") + itoa (serialbaud) + lxT ("(") +
                                 lxString ().Format ("%4.1f", fabs ((100.0 * serialexbaud - 100.0 *
                                                                     serialbaud) / serialexbaud)) + lxT ("%)"));
   else
    Window1.statusbar1.SetField (2, lxT ("Serial: ") +
                                 lxString::FromAscii (SERIALDEVICE) + lxT (" (ERROR)"));

   switch (avr->state)
    {
    case cpu_Limbo: Window1.SetCpuState (CPU_ERROR);
     break;
    case cpu_Stopped: Window1.SetCpuState (CPU_HALTED);
     break;
    case cpu_Running: Window1.SetCpuState (CPU_RUNNING);
     break;
    case cpu_Sleeping: Window1.SetCpuState (CPU_HALTED);
     break;
    case cpu_Step: Window1.SetCpuState (CPU_STEPPING);
     break;
    case cpu_StepDone: Window1.SetCpuState (CPU_STEPPING);
     break;
    case cpu_Done: Window1.SetCpuState (CPU_HALTED);
     break;
    case cpu_Crashed: Window1.SetCpuState (CPU_ERROR);
     break;
    }
   break;
  }

}

//Called to save board preferences in configuration file

void
cboard_Breadboard::WritePreferences(void)
{
 //write selected microcontroller of board_x to preferences
 Window1.saveprefs (lxT ("Breadboard_proc"), Proc);
 Window1.saveprefs (lxT ("Breadboard_clock"), lxString ().Format ("%2.1f", Window1.GetClock ()));
}

//Called whe configuration file load  preferences 

void
cboard_Breadboard::ReadPreferences(char *name, char *value)
{
 //read microcontroller of preferences
 if (!strcmp (name, "Breadboard_proc"))
  {
   Proc = value;
  }
 if (!strcmp (name, "Breadboard_clock"))
  {
   Window1.SetClock (atof (value));
  }
}


//Event on the board

void
cboard_Breadboard::EvKeyPress(uint key, uint mask) { }

//Event on the board

void
cboard_Breadboard::EvKeyRelease(uint key, uint mask) { }

//Event on the board

void
cboard_Breadboard::EvMouseButtonPress(uint button, uint x, uint y, uint state)
{

 int i;

 //search for the input area which owner the event    
 for (i = 0; i < inputc; i++)
  {
   if (((input[i].x1 <= x)&&(input[i].x2 >= x))&&((input[i].y1 <= y)&&
                                                  (input[i].y2 >= y)))
    {

     switch (input[i].id)
      {
       //if event is over I_ISCP area then load hex file  
      case I_ICSP:
       Window1.menu1_File_LoadHex_EvMenuActive (NULL);
       break;
       //if event is over I_PWR area then toggle board on/off
      case I_PWR:
       if (Window1.Get_mcupwr ()) //if on turn off
        {
         Window1.Set_mcurun (0);
         Window1.Set_mcupwr (0);
         Reset ();
         Window1.statusbar1.SetField (0, lxT ("Stoped"));
        }
       else //if off turn on
        {
         Window1.Set_mcupwr (1);
         Window1.Set_mcurun (1);
         Reset ();
         Window1.statusbar1.SetField (0, lxT ("Running..."));
        }
       output_ids[O_LPWR]->update = 1;
       break;
       //if event is over I_RST area then turn off and reset
      case I_RST:
       if (Window1.Get_mcupwr ())//if powered
        {
         Window1.Set_mcupwr (0);
         Window1.Set_mcurst (1);
        }
       p_RST = 0;
       output_ids[O_RST]->update = 1;
       break;
      }
    }
  }
}

//Event on the board

void
cboard_Breadboard::EvMouseButtonRelease(uint button, uint x, uint y, uint state)
{
 int i;

 //search for the input area which owner the event  
 for (i = 0; i < inputc; i++)
  {
   if (((input[i].x1 <= x)&&(input[i].x2 >= x))&&((input[i].y1 <= y)&&
                                                  (input[i].y2 >= y)))
    {
     switch (input[i].id)
      {
       //if event is over I_RST area then turn on  
      case I_RST:
       if (Window1.Get_mcurst ())//if powered
        {
         Window1.Set_mcupwr (1);
         Window1.Set_mcurst (0);

         Reset ();

        }
       p_RST = 1;
       output_ids[O_RST]->update = 1;
       break;
      }
    }
  }
}


//Called ever 100ms to draw board
//This is the critical code for simulator running speed

void
cboard_Breadboard::Draw(CDraw *draw)
{
 int i;
 lxRect rec;
 lxSize ps;
 int update = 0; //verifiy if updated is needed

 //board_0 draw 
 for (i = 0; i < outputc; i++) //run over all outputs
  {
   if (output[i].update)//only if need update
    {
     output[i].update = 0;

     if (!update)
      {
       draw->Canvas.Init (Scale, Scale);
      }
     update++; //set to update buffer

     draw->Canvas.SetFgColor (0, 0, 0); //black

     switch (output[i].id)//search for color of output
      {
      case O_LPWR: //Blue using mcupwr value
       draw->Canvas.SetColor (0, 0, 200 * Window1.Get_mcupwr () + 55);
       draw->Canvas.Rectangle (1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);
       break;
      case O_MP:
       font.SetPointSize ((MGetPinCount () >= 44) ? 5 : ((MGetPinCount () > 14) ? 12 : 10));
       draw->Canvas.SetFont (font);

       ps = micbmp->GetSize ();
       draw->Canvas.ChangeScale (1.0, 1.0);
       draw->Canvas.PutBitmap (micbmp, output[i].x1*Scale, output[i].y1 * Scale);
       draw->Canvas.ChangeScale (Scale, Scale);
       draw->Canvas.SetFgColor (230, 230, 230);

       rec.x = output[i].x1;
       rec.y = output[i].y1;
       rec.width = ps.GetWidth () / Scale;
       rec.height = ps.GetHeight () / Scale;
       draw->Canvas.TextOnRect (Proc, rec, lxALIGN_CENTER | lxALIGN_CENTER_VERTICAL);
       break;
      case O_RST:
       draw->Canvas.SetColor (100, 100, 100);
       draw->Canvas.Circle (1, output[i].cx, output[i].cy, 11);
       if (p_RST)
        {
         draw->Canvas.SetColor (15, 15, 15);
        }
       else
        {
         draw->Canvas.SetColor (55, 55, 55);
        }
       draw->Canvas.Circle (1, output[i].cx, output[i].cy, 9);
       break;
      }
    }
  }
 
 //end draw
 if (update)
  {
   draw->Canvas.End ();
   draw->Update ();
  }
}

void
cboard_Breadboard::Run_CPU(void)
{
 int i;
 int j;
 unsigned char pi;
 const picpin * pins;
 unsigned int alm[100];
 int JUMPSTEPS = 0;
 long int NSTEP = 0;

 switch (ptype)
  {
  case _PIC:

   JUMPSTEPS = Window1.GetJUMPSTEPS (); //number of steps skipped
   NSTEP = Window1.GetNSTEP () / pic.PINCOUNT; //number of steps in 100ms

   //reset mean value
   memset (alm, 0, 100 * sizeof (unsigned int));


   //read pic.pins to a local variable to speed up 
   pins = MGetPinsValues ();
   if (use_spare)Window5.PreProcess ();

   j = JUMPSTEPS; //step counter
   if (Window1.Get_mcupwr ()) //if powered
    for (i = 0; i < Window1.GetNSTEP (); i++) //repeat for number of steps in 100ms
     {

      if (j >= JUMPSTEPS)//if number of step is bigger than steps to skip 
       {
        pic_set_pin (pic.mclr, p_RST);
       }

      //verify if a breakpoint is reached if not run one instruction 
      if (!mplabxd_testbp ())pic_step ();
      if (use_oscope)Window4.SetSample ();
      if (use_spare)Window5.Process ();

      //increment mean value counter if pin is high
      alm[i % pic.PINCOUNT] += pins[i % pic.PINCOUNT].value;

      if (j >= JUMPSTEPS)//if number of step is bigger than steps to skip 
       {
        j = -1; //reset counter
       }
      j++; //counter increment
     }
   //calculate mean value
   for (pi = 0; pi < MGetPinCount (); pi++)
    {
     bsim_picsim::pic.pins[pi].oavalue = (int) (((200.0 * alm[pi]) / NSTEP) + 55);
    }
   if (use_spare)Window5.PostProcess ();
   break;
  case _AVR:

   int pinc = bsim_simavr::MGetPinCount ();
   JUMPSTEPS = Window1.GetJUMPSTEPS ()*4.0; //number of steps skipped
   NSTEP = 4.0 * Window1.GetNSTEP () / pinc; //number of steps in 100ms

   long long unsigned int cycle_start;
   int twostep = 0;

   //reset mean value

   memset (alm, 0, pinc * sizeof (unsigned int));

   //read pic.pins to a local variable to speed up 

   pins = bsim_simavr::MGetPinsValues ();

   if (use_spare)Window5.PreProcess ();

   j = JUMPSTEPS; //step counter
   if (Window1.Get_mcupwr ()) //if powered
    for (i = 0; i < (Window1.GetNSTEP ()*4); i++) //repeat for number of steps in 100ms
     {

      //verify if a breakpoint is reached if not run one instruction 
      if (avr_debug_type || (!mplabxd_testbp ()))
       {
        if (twostep)
         {
          twostep = 0; //NOP   
         }
        else
         {
          cycle_start = avr->cycle;
          avr_run (avr);
          if ((avr->cycle - cycle_start) > 1)
           {
            twostep = 1;
           }
         }
       }
      bsim_simavr::UpdateHardware ();

      //avr->sleep_usec=0;
      if (use_oscope)Window4.SetSample ();
      if (use_spare)Window5.Process ();

      //increment mean value counter if pin is high
      alm[i % pinc] += pins[i % pinc].value;

      if (j >= JUMPSTEPS)//if number of step is bigger than steps to skip 
       {
        //set analog pin 2 (AN0) with value from scroll  
        //pic_set_apin(2,((5.0*(scroll1->GetPosition()))/
        //  (scroll1->GetRange()-1)));

        j = -1; //reset counter
       }
      j++; //counter increment   
     }
   //calculate mean value
   for (pi = 0; pi < MGetPinCount (); pi++)
    {
     bsim_simavr::pins[pi].oavalue = (int) (((200.0 * alm[pi]) / NSTEP) + 55);
    }
   if (use_spare)Window5.PostProcess ();
   break;
  }



}


//class dependent

int
cboard_Breadboard::DebugInit(int dtyppe)
{
 switch (ptype)
  {
  case _PIC:
   return bsim_picsim::DebugInit (dtyppe);
   break;
  case _AVR:
   return bsim_simavr::DebugInit (dtyppe);
   break;
  }
 return 0;
}

lxString
cboard_Breadboard::GetDebugName(void)
{
 switch (ptype)
  {
  case _PIC:
   return bsim_picsim::GetDebugName ();
   break;
  case _AVR:
   return bsim_simavr::GetDebugName ();
   break;
  }
 return "";
}

void
cboard_Breadboard::DebugLoop(void)
{
 switch (ptype)
  {
  case _PIC:
   bsim_picsim::DebugLoop ();
   break;
  case _AVR:
   bsim_simavr::DebugLoop ();
   break;
  }
}

int
cboard_Breadboard::CpuInitialized(void)
{
 switch (ptype)
  {
  case _PIC:
   return bsim_picsim::CpuInitialized ();
   break;
  case _AVR:
   return bsim_simavr::CpuInitialized ();
   break;
  }
 return 0;
}

void
cboard_Breadboard::MSetSerial(const char * port)
{
 switch (ptype)
  {
  case _PIC:
   return bsim_picsim::MSetSerial (port);
   break;
  case _AVR:
   return bsim_simavr::MSetSerial (port);
   break;
  }
}

int
cboard_Breadboard::MInit(const char * processor, const char * fname, float freq)
{
 int ret = 0;

 if (processor[0] == 'P')
  ptype = _PIC;
 else
  ptype = _AVR;


 switch (ptype)
  {
  case _PIC:
   ret = bsim_picsim::MInit (processor, fname, freq);
   break;
  case _AVR:
   ret = bsim_simavr::MInit (processor, fname, freq);
   break;
  }

 lxImage image (&Window1);


 if (!image.LoadFile (Window1.GetSharePath () + lxT ("boards/Common/ic") + itoa (MGetPinCount ()) + lxT (".svg"), 0, Scale, Scale, 1))
  {
   image.LoadFile (Window1.GetSharePath () + lxT ("boards/Common/ic6.svg"), 0, Scale, Scale, 1);
   printf ("picsimlab: IC package with %i pins not found!\n", MGetPinCount ());
   printf ("picsimlab: %s not found!\n", (const char *) (Window1.GetSharePath () + lxT ("boards/Common/ic") + itoa (MGetPinCount ()) + lxT (".svg")).c_str ());
  }

 if (micbmp) delete micbmp;
 micbmp = new lxBitmap (&image, &Window1);


 return ret;
}

void
cboard_Breadboard::MEnd(void)
{
 switch (ptype)
  {
  case _PIC:
   return bsim_picsim::MEnd ();
   break;
  case _AVR:
   return bsim_simavr::MEnd ();
   break;
  }
}

void
cboard_Breadboard::MDumpMemory(const char * fname)
{
 switch (ptype)
  {
  case _PIC:
   return bsim_picsim::MDumpMemory (fname);
   break;
  case _AVR:
   return bsim_simavr::MDumpMemory (fname);
   break;
  }
}

void
cboard_Breadboard::MEraseFlash(void)
{
 switch (ptype)
  {
  case _PIC:
   return bsim_picsim::MEraseFlash ();
   break;
  case _AVR:
   return bsim_simavr::MEraseFlash ();
   break;
  }
}

void
cboard_Breadboard::MSetFreq(float freq)
{
 switch (ptype)
  {
  case _PIC:
   return bsim_picsim::MSetFreq (freq);
   break;
  case _AVR:
   return bsim_simavr::MSetFreq (freq);
   break;
  }

}

float
cboard_Breadboard::MGetFreq(void)
{
 switch (ptype)
  {
  case _PIC:
   return bsim_picsim::MGetFreq ();
   break;
  case _AVR:
   return bsim_simavr::MGetFreq ();
   break;
  }
 return 0;
}

float
cboard_Breadboard::MGetInstClockFreq(void)
{
 switch (ptype)
  {
  case _PIC:
   return bsim_picsim::MGetInstClockFreq ();
   break;
  case _AVR:
   return bsim_simavr::MGetInstClockFreq ();
   break;
  }
 return 0;
}

int
cboard_Breadboard::MGetPinCount(void)
{
 switch (ptype)
  {
  case _PIC:
   return bsim_picsim::MGetPinCount ();
   break;
  case _AVR:
   return bsim_simavr::MGetPinCount ();
   break;
  }
 return 0;
}

lxString
cboard_Breadboard::MGetPinName(int pin)
{
 switch (ptype)
  {
  case _PIC:
   return bsim_picsim::MGetPinName (pin);
   break;
  case _AVR:
   return bsim_simavr::MGetPinName (pin);
   break;
  }
 return "";
}

void
cboard_Breadboard::MSetPin(int pin, unsigned char value)
{
 switch (ptype)
  {
  case _PIC:
   return bsim_picsim::MSetPin (pin, value);
   break;
  case _AVR:
   return bsim_simavr::MSetPin (pin, value);
   break;
  }
}

void
cboard_Breadboard::MSetPinDOV(int pin, unsigned char ovalue)
{
 switch (ptype)
  {
  case _PIC:
   return bsim_picsim::MSetPinDOV (pin, ovalue);
   break;
  case _AVR:
   return bsim_simavr::MSetPinDOV (pin, ovalue);
   break;
  }
}

void
cboard_Breadboard::MSetAPin(int pin, float value)
{
 switch (ptype)
  {
  case _PIC:
   return bsim_picsim::MSetAPin (pin, value);
   break;
  case _AVR:
   return bsim_simavr::MSetAPin (pin, value);
   break;
  }
}

unsigned char
cboard_Breadboard::MGetPin(int pin)
{
 switch (ptype)
  {
  case _PIC:
   return bsim_picsim::MGetPin (pin);
   break;
  case _AVR:
   return bsim_simavr::MGetPin (pin);
   break;
  }
 return 0;
}

const picpin *
cboard_Breadboard::MGetPinsValues(void)
{
 switch (ptype)
  {
  case _PIC:
   return bsim_picsim::MGetPinsValues ();
   break;
  case _AVR:
   return bsim_simavr::MGetPinsValues ();
   break;
  }
 return NULL;
}

void
cboard_Breadboard::MStep(void)
{
 switch (ptype)
  {
  case _PIC:
   return bsim_picsim::MStep ();
   break;
  case _AVR:
   return bsim_simavr::MStep ();
   break;
  }
}

void
cboard_Breadboard::MStepResume(void)
{
 switch (ptype)
  {
  case _PIC:
   return bsim_picsim::MStepResume ();
   break;
  case _AVR:
   return bsim_simavr::MStepResume ();
   break;
  }
}

int
cboard_Breadboard::DBGTestBP(unsigned int bp)
{
 switch (ptype)
  {
  case _PIC:
   return bsim_picsim::DBGTestBP (bp);
   break;
  case _AVR:
   return bsim_simavr::DBGTestBP (bp);
   break;
  }
 return 0;
}

void
cboard_Breadboard::MReset(int flags)
{
 switch (ptype)
  {
  case _PIC:
   return bsim_picsim::MReset (flags);
   break;
  case _AVR:
   return bsim_simavr::MReset (flags);
   break;
  }
}

unsigned short *
cboard_Breadboard::DBGGetProcID_p(void)
{
 switch (ptype)
  {
  case _PIC:
   return bsim_picsim::DBGGetProcID_p ();
   break;
  case _AVR:
   return bsim_simavr::DBGGetProcID_p ();
   break;
  }
 return NULL;
}

unsigned int
cboard_Breadboard::DBGGetPC(void)
{
 switch (ptype)
  {
  case _PIC:
   return bsim_picsim::DBGGetPC ();
   break;
  case _AVR:
   return bsim_simavr::DBGGetPC ();
   break;
  }
 return 0;
}

void
cboard_Breadboard::DBGSetPC(unsigned int pc)
{
 switch (ptype)
  {
  case _PIC:
   bsim_picsim::DBGSetPC (pc);
   return;
   break;
  case _AVR:
   bsim_simavr::DBGSetPC (pc);
   return;
   break;
  }
 return;
}

unsigned char *
cboard_Breadboard::DBGGetRAM_p(void)
{
 switch (ptype)
  {
  case _PIC:
   return bsim_picsim::DBGGetRAM_p ();
   break;
  case _AVR:
   return bsim_simavr::DBGGetRAM_p ();
   break;
  }
 return NULL;
}

unsigned char *
cboard_Breadboard::DBGGetROM_p(void)
{
 switch (ptype)
  {
  case _PIC:
   return bsim_picsim::DBGGetROM_p ();
   break;
  case _AVR:
   return bsim_simavr::DBGGetROM_p ();
   break;
  }
 return NULL;
}

unsigned char *
cboard_Breadboard::DBGGetCONFIG_p(void)
{
 switch (ptype)
  {
  case _PIC:
   return bsim_picsim::DBGGetCONFIG_p ();
   break;
  case _AVR:
   return bsim_simavr::DBGGetCONFIG_p ();
   break;
  }
 return NULL;
}

unsigned char *
cboard_Breadboard::DBGGetID_p(void)
{
 switch (ptype)
  {
  case _PIC:
   return bsim_picsim::DBGGetID_p ();
   break;
  case _AVR:
   return bsim_simavr::DBGGetID_p ();
   break;
  }
 return NULL;
}

unsigned char *
cboard_Breadboard::DBGGetEEPROM_p(void)
{
 switch (ptype)
  {
  case _PIC:
   return bsim_picsim::DBGGetEEPROM_p ();
   break;
  case _AVR:
   return bsim_simavr::DBGGetEEPROM_p ();
   break;
  }
 return NULL;
}

unsigned int
cboard_Breadboard::DBGGetRAMSize(void)
{
 switch (ptype)
  {
  case _PIC:
   return bsim_picsim::DBGGetRAMSize ();
   break;
  case _AVR:
   return bsim_simavr::DBGGetRAMSize ();
   break;
  }
 return 0;
}

unsigned int
cboard_Breadboard::DBGGetROMSize(void)
{
 switch (ptype)
  {
  case _PIC:
   return bsim_picsim::DBGGetROMSize ();
   break;
  case _AVR:
   return bsim_simavr::DBGGetROMSize ();
   break;
  }
 return 0;
}

unsigned int
cboard_Breadboard::DBGGetCONFIGSize(void)
{
 switch (ptype)
  {
  case _PIC:
   return bsim_picsim::DBGGetCONFIGSize ();
   break;
  case _AVR:
   return bsim_simavr::DBGGetCONFIGSize ();
   break;
  }
 return 0;
}

unsigned int
cboard_Breadboard::DBGGetIDSize(void)
{
 switch (ptype)
  {
  case _PIC:
   return bsim_picsim::DBGGetIDSize ();
   break;
  case _AVR:
   return bsim_simavr::DBGGetIDSize ();
   break;
  }
 return 0;
}

unsigned int
cboard_Breadboard::DBGGetEEPROM_Size(void)
{
 switch (ptype)
  {
  case _PIC:
   return bsim_picsim::DBGGetEEPROM_Size ();
   break;
  case _AVR:
   return bsim_simavr::DBGGetEEPROM_Size ();
   break;
  }
 return 0;
}

void
cboard_Breadboard::SetScale(double scale)
{

 if (Scale == scale)return;

 Scale = scale;


 lxImage image (&Window1);

 if (!image.LoadFile (Window1.GetSharePath () + lxT ("boards/Common/ic") + itoa (MGetPinCount ()) + lxT (".svg"), 0, Scale, Scale, 1))
  {
   image.LoadFile (Window1.GetSharePath () + lxT ("boards/Common/ic6.svg"), 0, Scale, Scale, 1);
   printf ("picsimlab: IC package with %i pins not found!\n", MGetPinCount ());
   printf ("picsimlab: %s not found!\n", (const char *) (Window1.GetSharePath () + lxT ("boards/Common/ic") + itoa (MGetPinCount ()) + lxT (".svg")).c_str ());
  }

 if (micbmp) delete micbmp;
 micbmp = new lxBitmap (&image, &Window1);

}

board_init("Breadboard", cboard_Breadboard);

