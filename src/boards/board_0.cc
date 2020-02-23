/* ########################################################################

   PICsimLab - PIC laboratory simulator

   ########################################################################

   Copyright (c) : 2015-2020  Luis Claudio Gambôa Lopes

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
#include"board_0.h"

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
 O_MP
};
//return the input ids numbers of names used in input map

unsigned short
cboard_0::get_in_id(char * name)
{
 if (strcmp (name, "I_ICSP") == 0)return I_ICSP;
 if (strcmp (name, "I_PWR") == 0)return I_PWR;
 if (strcmp (name, "I_RST") == 0)return I_RST;

 printf ("Erro input '%s' don't have a valid id! \n", name);
 return -1;
};

//return the output ids numbers of names used in output map

unsigned short
cboard_0::get_out_id(char * name)
{

 if (strcmp (name, "O_LPWR") == 0)return O_LPWR;
 if (strcmp (name, "O_MP") == 0)return O_MP;

 printf ("Erro output '%s' don't have a valid id! \n", name);
 return 1;
};

//Constructor called once on board creation 

cboard_0::cboard_0(void)
{
 ptype = _PIC;
 proc = "PIC18F4550"; //default microcontroller if none defined in preferences
 ReadMaps (); //Read input and output board maps

 lxImage image;
 image.LoadFile (Window1.GetSharePath () + lxT ("boards/ic40.png"));
 micbmp = new lxBitmap (image, &Window1);

}

//Destructor called once on board destruction 

cboard_0::~cboard_0(void)
{
 delete micbmp;
 micbmp = NULL;
}

//Reset board status

void
cboard_0::Reset(void)
{

 switch (ptype)
  {
  case _PIC:

   pic_reset (1);


   //verify serial port state and refresh status bar  
#ifndef _WIN_
   if (pic.serialfd > 0)
#else
   if (pic.serialfd != INVALID_HANDLE_VALUE)
#endif
    Window1.statusbar1.SetField (2, lxT ("Serial: ") +
                                 String::FromAscii (SERIALDEVICE) + lxT (":") + itoa (pic.serialbaud) + lxT ("(") +
                                 String ().Format ("%4.1f", fabs ((100.0 * pic.serialexbaud - 100.0 *
                                                                   pic.serialbaud) / pic.serialexbaud)) + lxT ("%)"));
   else
    Window1.statusbar1.SetField (2, lxT ("Serial: ") +
                                 String::FromAscii (SERIALDEVICE) + lxT (" (ERROR)"));
   break;
  case _AVR:
   avr_reset (avr);


   //verify serial port state and refresh status bar  
#ifndef _WIN_
   if (avr_serial_get_fd () > 0)
#else
   if (avr_serial_get_fd () != INVALID_HANDLE_VALUE)
#endif
    Window1.statusbar1.SetField (2, lxT ("Serial: ") +
                                 String::FromAscii (SERIALDEVICE) + lxT (":") + itoa (serialbaud) + lxT ("(") +
                                 String ().Format ("%4.1f", fabs ((100.0 * serialexbaud - 100.0 *
                                                                   serialbaud) / serialexbaud)) + lxT ("%)"));
   else
    Window1.statusbar1.SetField (2, lxT ("Serial: ") +
                                 String::FromAscii (SERIALDEVICE) + lxT (" (ERROR)"));
   break;

  }
}

//Called ever 1s to refresh status

void
cboard_0::RefreshStatus(void)
{

 switch (ptype)
  {
  case _PIC:
   //verify serial port state and refresh status bar   
#ifndef _WIN_
   if (pic.serialfd > 0)
#else
   if (pic.serialfd != INVALID_HANDLE_VALUE)
#endif
    Window1.statusbar1.SetField (2, lxT ("Serial: ") +
                                 String::FromAscii (SERIALDEVICE) + lxT (":") + itoa (pic.serialbaud) + lxT ("(") +
                                 String ().Format ("%4.1f", fabs ((100.0 * pic.serialexbaud - 100.0 *
                                                                   pic.serialbaud) / pic.serialexbaud)) + lxT ("%)"));
   else
    Window1.statusbar1.SetField (2, lxT ("Serial: ") +
                                 String::FromAscii (SERIALDEVICE) + lxT (" (ERROR)"));
   break;
  case _AVR:
   //verify serial port state and refresh status bar   
#ifndef _WIN_
   if (avr_serial_get_fd () > 0)
#else
   if (avr_serial_get_fd () != INVALID_HANDLE_VALUE)
#endif
    Window1.statusbar1.SetField (2, lxT ("Serial: ") +
                                 String::FromAscii (SERIALDEVICE) + lxT (":") + itoa (serialbaud) + lxT ("(") +
                                 String ().Format ("%4.1f", fabs ((100.0 * serialexbaud - 100.0 *
                                                                   serialbaud) / serialexbaud)) + lxT ("%)"));
   else
    Window1.statusbar1.SetField (2, lxT ("Serial: ") +
                                 String::FromAscii (SERIALDEVICE) + lxT (" (ERROR)"));

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

};

//Called to save board preferences in configuration file

void
cboard_0::WritePreferences(void)
{
 //write selected microcontroller of board_x to preferences
 Window1.saveprefs (lxT ("p0_proc"), proc);
}

//Called whe configuration file load  preferences 

void
cboard_0::ReadPreferences(char *name, char *value)
{
 //read microcontroller of preferences
 if (!strcmp (name, "p0_proc"))
  {
   proc = value;
  }
};


//Event on the board

void
cboard_0::EvKeyPress(uint key, uint mask) { }

//Event on the board

void
cboard_0::EvKeyRelease(uint key, uint mask) { }

//Event on the board

void
cboard_0::EvMouseButtonPress(uint button, uint x, uint y, uint state)
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
       break;
       //if event is over I_RST area then turn off and reset
      case I_RST:
       if (Window1.Get_mcupwr ())//if powered
        {
         Window1.Set_mcupwr (0);
         Window1.Set_mcurst (1);
        }
       p_MCLR = 0;
       break;
      }
    }
  }
}

//Event on the board

void
cboard_0::EvMouseButtonRelease(uint button, uint x, uint y, uint state)
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
       p_MCLR = 1;
       break;
      }
    }
  }
}


//Called ever 100ms to draw board
//This is the critical code for simulator running speed

void
cboard_0::Draw(CDraw *draw, double scale)
{
 int i;

 draw->Canvas.Init (scale, scale); //initialize draw context

 //board_0 draw 
 for (i = 0; i < outputc; i++) //run over all outputs
  {

   draw->Canvas.SetFgColor (0, 0, 0); //black

   switch (output[i].id)//search for color of output
    {
    case O_LPWR: //Blue using mcupwr value
     draw->Canvas.SetColor (0, 0, 225 * Window1.Get_mcupwr () + 30);
     draw->Canvas.Rectangle (1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);
     break;
    case O_MP:
     lxFont font (12, lxFONTFAMILY_TELETYPE, lxFONTSTYLE_NORMAL, lxFONTWEIGHT_NORMAL);
     draw->Canvas.SetFont (font);

     lxSize ps = micbmp->GetSize ();

     draw->Canvas.PutBitmap (micbmp, output[i].x1, output[i].y1);
     draw->Canvas.SetFgColor (255, 255, 255);
     draw->Canvas.Text (proc, (ps.x - strlen (proc)*11) / 2 + output[i].x1, (ps.y / 2) + output[i].y1 - 9);
     break;
    }

  }

 //end draw
 draw->Canvas.End ();
 draw->Update ();
}

void
cboard_0::Run_CPU(void)
{
 int i;
 int j;
 unsigned char pi;
 const picpin * pins;
 unsigned int alm[40];
 int JUMPSTEPS = 0;
 long int NSTEPJ = 0;

 switch (ptype)
  {
  case _PIC:

   JUMPSTEPS = Window1.GetJUMPSTEPS (); //number of steps skipped
   NSTEPJ = Window1.GetNSTEPJ (); //number of steps in 100ms

   //reset mean value
   memset (alm, 0, 40 * sizeof (unsigned int));


   //read pic.pins to a local variable to speed up 
   pins = MGetPinsValues ();
   if (use_spare)Window5.PreProcess ();

   j = JUMPSTEPS; //step counter
   if (Window1.Get_mcupwr ()) //if powered
    for (i = 0; i < Window1.GetNSTEP (); i++) //repeat for number of steps in 100ms
     {

      if (j >= JUMPSTEPS)//if number of step is bigger than steps to skip 
       {
        pic_set_pin (pic.mclr, p_MCLR);
       }

      //verify if a breakpoint is reached if not run one instruction 
      if (!mplabxd_testbp ())pic_step ();
      if (use_oscope)Window4.SetSample ();
      if (use_spare)Window5.Process ();

      //increment mean value counter if pin is high 
      if (j < pic.PINCOUNT)
       alm[j] += pins[j].value;

      if (j >= JUMPSTEPS)//if number of step is bigger than steps to skip 
       {
        /*  
        //increment mean value counter if pin is high  
        for(pi=0;pi < pic.PINCOUNT;pi++)
        {
         alm[pi]+=pins[pi].value;
        }
         */

        j = -1; //reset counter
       }
      j++; //counter increment
     }
   //calculate mean value
   for (pi = 0; pi < MGetPinCount (); pi++)
    {
     board_pic::pic.pins[pi].oavalue = (int) (((225.0 * alm[pi]) / NSTEPJ) + 30);
    }
   if (use_spare)Window5.PostProcess ();
   break;
  case _AVR:

   JUMPSTEPS = Window1.GetJUMPSTEPS ()*4.0; //number of steps skipped
   NSTEPJ = Window1.GetNSTEPJ (); //number of steps in 100ms

   long long unsigned int cycle_start;
   int twostep = 0;

   int pinc = board_avr::MGetPinCount ();
   //reset mean value

   memset (alm, 0, pinc * sizeof (unsigned int));

   //read pic.pins to a local variable to speed up 
   //FIXME pins = pic.pins;
   pins = board_avr::MGetPinsValues ();

   if (use_spare)Window5.PreProcess ();

   j = JUMPSTEPS; //step counter
   if (Window1.Get_mcupwr ()) //if powered
    for (i = 0; i < (Window1.GetNSTEP ()*4); i++) //repeat for number of steps in 100ms
     {


      /*
             if(j > JUMPSTEPS)//if number of step is bigger than steps to skip 
             {  
             //FIXME ATMEGA RST
             //pic_set_pin(pic.mclr, p_MCLR);
               pic_set_pin(19,p_BT1);//Set pin 19 (RD0) with button state 
               pic_set_pin(20,p_BT2);//Set pin 20 (RD1) with switch state 
             } 
       */
      //verify if a breakpoint is reached if not run one instruction 
#ifndef AVR_USE_GDB
      if (!mplabxd_testbp ())
#endif 
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
      board_avr::UpdateHardware ();

      //avr->sleep_usec=0;
      if (use_oscope)Window4.SetSample ();
      if (use_spare)Window5.Process ();

      //increment mean value counter if pin is high
      if (j < pinc)
       alm[j] += pins[j].value;

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
     board_avr::pins[pi].oavalue = (int) (((225.0 * alm[pi]) / NSTEPJ) + 30);
    }
   if (use_spare)Window5.PostProcess ();
   break;
  }



}


//class dependent

int
cboard_0::DebugInit(void)
{
 switch (ptype)
  {
  case _PIC:
   return board_pic::DebugInit ();
   break;
  case _AVR:
   return board_avr::DebugInit ();
   break;
  }
 return 0;
}

void
cboard_0::DebugLoop(void)
{
 switch (ptype)
  {
  case _PIC:
   board_pic::DebugLoop ();
   break;
  case _AVR:
   board_avr::DebugLoop ();
   break;
  }
}

int
cboard_0::CpuInitialized(void)
{
 switch (ptype)
  {
  case _PIC:
   return board_pic::CpuInitialized ();
   break;
  case _AVR:
   return board_avr::CpuInitialized ();
   break;
  }
 return 0;
}

void
cboard_0::MSetSerial(const char * port)
{
 switch (ptype)
  {
  case _PIC:
   return board_pic::MSetSerial (port);
   break;
  case _AVR:
   return board_avr::MSetSerial (port);
   break;
  }
}

int
cboard_0::MInit(const char * processor, const char * fname, float freq)
{
 int ret = 0;

 if (processor[0] == 'P')
  ptype = _PIC;
 else
  ptype = _AVR;


 switch (ptype)
  {
  case _PIC:
   ret = board_pic::MInit (processor, fname, freq);
   break;
  case _AVR:
   ret = board_avr::MInit (processor, fname, freq);
   break;
  }

 if (!ret)
  {
   lxImage image;

   switch (MGetPinCount ())
    {
    case 8:
     image.LoadFile (Window1.GetSharePath () + lxT ("boards/ic08.png"));
     break;
    case 18:
     image.LoadFile (Window1.GetSharePath () + lxT ("boards/ic18.png"));
     break;
    case 20:
     image.LoadFile (Window1.GetSharePath () + lxT ("boards/ic20.png"));
     break;
    case 28:
     image.LoadFile (Window1.GetSharePath () + lxT ("boards/ic28.png"));
     break;
    case 40:
     image.LoadFile (Window1.GetSharePath () + lxT ("boards/ic40.png"));
     break;
    default:
     image.LoadFile (Window1.GetSharePath () + lxT ("boards/ic40.png"));
     printf ("IC package with %i pins not found!\n", MGetPinCount ());
     break;
    }

   if (micbmp) delete micbmp;
   micbmp = new lxBitmap (image, &Window1);
  }

 return ret;
}

void
cboard_0::MEnd(void)
{
 switch (ptype)
  {
  case _PIC:
   return board_pic::MEnd ();
   break;
  case _AVR:
   return board_avr::MEnd ();
   break;
  }
}

void
cboard_0::MDumpMemory(const char * fname)
{
 switch (ptype)
  {
  case _PIC:
   return board_pic::MDumpMemory (fname);
   break;
  case _AVR:
   return board_avr::MDumpMemory (fname);
   break;
  }
}

void
cboard_0::MEraseFlash(void)
{
 switch (ptype)
  {
  case _PIC:
   return board_pic::MEraseFlash ();
   break;
  case _AVR:
   return board_avr::MEraseFlash ();
   break;
  }
}

void
cboard_0::MSetFreq(float freq)
{
 switch (ptype)
  {
  case _PIC:
   return board_pic::MSetFreq (freq);
   break;
  case _AVR:
   return board_avr::MSetFreq (freq);
   break;
  }

}

float
cboard_0::MGetFreq(void)
{
 switch (ptype)
  {
  case _PIC:
   return board_pic::MGetFreq ();
   break;
  case _AVR:
   return board_avr::MGetFreq ();
   break;
  }
 return 0;
}

float
cboard_0::MGetInstClock(void)
{
 switch (ptype)
  {
  case _PIC:
   return board_pic::MGetInstClock ();
   break;
  case _AVR:
   return board_avr::MGetInstClock ();
   break;
  }
 return 0;
}

int
cboard_0::MGetPinCount(void)
{
 switch (ptype)
  {
  case _PIC:
   return board_pic::MGetPinCount ();
   break;
  case _AVR:
   return board_avr::MGetPinCount ();
   break;
  }
 return 0;
}

String
cboard_0::MGetPinName(int pin)
{
 switch (ptype)
  {
  case _PIC:
   return board_pic::MGetPinName (pin);
   break;
  case _AVR:
   return board_avr::MGetPinName (pin);
   break;
  }
 return "";
}

void
cboard_0::MSetPin(int pin, unsigned char value)
{
 switch (ptype)
  {
  case _PIC:
   return board_pic::MSetPin (pin, value);
   break;
  case _AVR:
   return board_avr::MSetPin (pin, value);
   break;
  }
}

void
cboard_0::MSetAPin(int pin, float value)
{
 switch (ptype)
  {
  case _PIC:
   return board_pic::MSetAPin (pin, value);
   break;
  case _AVR:
   return board_avr::MSetAPin (pin, value);
   break;
  }
}

unsigned char
cboard_0::MGetPin(int pin)
{
 switch (ptype)
  {
  case _PIC:
   return board_pic::MGetPin (pin);
   break;
  case _AVR:
   return board_avr::MGetPin (pin);
   break;
  }
 return 0;
}

const picpin *
cboard_0::MGetPinsValues(void)
{
 switch (ptype)
  {
  case _PIC:
   return board_pic::MGetPinsValues ();
   break;
  case _AVR:
   return board_avr::MGetPinsValues ();
   break;
  }
 return NULL;
}

void
cboard_0::MStep(void)
{
 switch (ptype)
  {
  case _PIC:
   return board_pic::MStep ();
   break;
  case _AVR:
   return board_avr::MStep ();
   break;
  }
}

void
cboard_0::MStepResume(void)
{
 switch (ptype)
  {
  case _PIC:
   return board_pic::MStepResume ();
   break;
  case _AVR:
   return board_avr::MStepResume ();
   break;
  }
}

int
cboard_0::MTestBP(unsigned short bp)
{
 switch (ptype)
  {
  case _PIC:
   return board_pic::MTestBP (bp);
   break;
  case _AVR:
   return board_avr::MTestBP (bp);
   break;
  }
 return 0;
}

void
cboard_0::MReset(int flags)
{
 switch (ptype)
  {
  case _PIC:
   return board_pic::MReset (flags);
   break;
  case _AVR:
   return board_avr::MReset (flags);
   break;
  }
}

unsigned short *
cboard_0::MGetProcID_p(void)
{
 switch (ptype)
  {
  case _PIC:
   return board_pic::MGetProcID_p ();
   break;
  case _AVR:
   return board_avr::MGetProcID_p ();
   break;
  }
 return NULL;
}

unsigned short 
cboard_0::MGetPC(void)
{
 switch (ptype)
  {
  case _PIC:
   return board_pic::MGetPC();
   break;
  case _AVR:
   return board_avr::MGetPC();
   break;
  }
 return 0;
}

void 
cboard_0::MSetPC(unsigned short pc)
{
 switch (ptype)
  {
  case _PIC:
   board_pic::MSetPC(pc);
   return;
   break;
  case _AVR:
   board_avr::MSetPC(pc);
   return;
   break;
  }
 return;
}

unsigned char *
cboard_0::MGetRAM_p(void)
{
 switch (ptype)
  {
  case _PIC:
   return board_pic::MGetRAM_p ();
   break;
  case _AVR:
   return board_avr::MGetRAM_p ();
   break;
  }
 return NULL;
}

unsigned char *
cboard_0::MGetROM_p(void)
{
 switch (ptype)
  {
  case _PIC:
   return board_pic::MGetROM_p ();
   break;
  case _AVR:
   return board_avr::MGetROM_p ();
   break;
  }
 return NULL;
}

unsigned char *
cboard_0::MGetCONFIG_p(void)
{
 switch (ptype)
  {
  case _PIC:
   return board_pic::MGetCONFIG_p ();
   break;
  case _AVR:
   return board_avr::MGetCONFIG_p ();
   break;
  }
 return NULL;
}

unsigned char *
cboard_0::MGetID_p(void)
{
 switch (ptype)
  {
  case _PIC:
   return board_pic::MGetID_p ();
   break;
  case _AVR:
   return board_avr::MGetID_p ();
   break;
  }
 return NULL;
}

unsigned char *
cboard_0::MGetEEPROM_p(void)
{
 switch (ptype)
  {
  case _PIC:
   return board_pic::MGetEEPROM_p ();
   break;
  case _AVR:
   return board_avr::MGetEEPROM_p ();
   break;
  }
 return NULL;
}

unsigned int
cboard_0::MGetRAMSize(void)
{
 switch (ptype)
  {
  case _PIC:
   return board_pic::MGetRAMSize ();
   break;
  case _AVR:
   return board_avr::MGetRAMSize ();
   break;
  }
 return 0;
}

unsigned int
cboard_0::MGetROMSize(void)
{
 switch (ptype)
  {
  case _PIC:
   return board_pic::MGetROMSize ();
   break;
  case _AVR:
   return board_avr::MGetROMSize ();
   break;
  }
 return 0;
}

unsigned int
cboard_0::MGetCONFIGSize(void)
{
 switch (ptype)
  {
  case _PIC:
   return board_pic::MGetCONFIGSize ();
   break;
  case _AVR:
   return board_avr::MGetCONFIGSize ();
   break;
  }
 return 0;
}

unsigned int
cboard_0::MGetIDSize(void)
{
 switch (ptype)
  {
  case _PIC:
   return board_pic::MGetIDSize ();
   break;
  case _AVR:
   return board_avr::MGetIDSize ();
   break;
  }
 return 0;
}

unsigned int
cboard_0::MGetEEPROM_Size(void)
{
 switch (ptype)
  {
  case _PIC:
   return board_pic::MGetEEPROM_Size ();
   break;
  case _AVR:
   return board_avr::MGetEEPROM_Size ();
   break;
  }
 return 0;
}
