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
#include"board_Blue_Pill.h"

/* ids of inputs of input map*/
enum
{
 I_ICSP, //ICSP connector
 I_PWR, //Power button
 I_RST, //Reset button
};

/* ids of outputs of output map*/
enum
{
 O_LPWR, //Power LED
 O_LED, //LED on PC13 output
};
//return the input ids numbers of names used in input map

unsigned short
cboard_Blue_Pill::get_in_id(char * name)
{
 if (strcmp (name, "I_ICSP") == 0)return I_ICSP;
 if (strcmp (name, "I_PWR") == 0)return I_PWR;
 if (strcmp (name, "I_RST") == 0)return I_RST;

 printf ("Error input '%s' don't have a valid id! \n", name);
 return -1;
}

//return the output ids numbers of names used in output map

unsigned short
cboard_Blue_Pill::get_out_id(char * name)
{

 if (strcmp (name, "LED") == 0)return O_LED;
 if (strcmp (name, "LPWR") == 0)return O_LPWR;


 printf ("Error output '%s' don't have a valid id! \n", name);
 return 1;
}

//Constructor called once on board creation 

cboard_Blue_Pill::cboard_Blue_Pill(void)
{
 Proc = "stm32f103c8t6"; //default microcontroller if none defined in preferences
 ReadMaps (); //Read input and output board maps
}

//Destructor called once on board destruction 

cboard_Blue_Pill::~cboard_Blue_Pill(void)
{

}

//Reset board status

void
cboard_Blue_Pill::Reset(void)
{
 MReset(1);

 //verify serial port state and refresh status bar  
#ifndef _WIN_
 if (serialfd[0] > 0)
#else
 if (serialfd[0] != INVALID_HANDLE_VALUE)
#endif
  Window1.statusbar1.SetField (2, lxT ("Serial: ") +
                               lxString::FromAscii (SERIALDEVICE) + lxT (":") + itoa (serialbaud) + lxT ("(") +
                               lxString ().Format ("%4.1f", fabs ((100.0 * serialexbaud - 100.0 *
                                                                 serialbaud) / serialexbaud)) + lxT ("%)"));
 else
  Window1.statusbar1.SetField (2, lxT ("Serial: ") +
                               lxString::FromAscii (SERIALDEVICE) + lxT (" (ERROR)"));

 if (use_spare)Window5.Reset ();
}

//Called ever 1s to refresh status

void
cboard_Blue_Pill::RefreshStatus(void)
{
 //verify serial port state and refresh status bar   
#ifndef _WIN_
 if (serialfd[0] > 0)
#else
 if (serialfd[0] != INVALID_HANDLE_VALUE)
#endif
  Window1.statusbar1.SetField (2, lxT ("Serial: ") +
                               lxString::FromAscii (SERIALDEVICE) + lxT (":") + itoa (serialbaud) + lxT ("(") +
                               lxString ().Format ("%4.1f", fabs ((100.0 * serialexbaud - 100.0 *
                                                                 serialbaud) / serialexbaud)) + lxT ("%)"));
 else
  Window1.statusbar1.SetField (2, lxT ("Serial: ") +
                               lxString::FromAscii (SERIALDEVICE) + lxT (" (ERROR)"));

}

//Called to save board preferences in configuration file

void
cboard_Blue_Pill::WritePreferences(void)
{
 //write selected microcontroller of board_x to preferences
 Window1.saveprefs (lxT ("Blue_Pill_proc"), Proc);
 //write microcontroller clock to preferences
 Window1.saveprefs (lxT ("Blue_Pill_clock"), lxString ().Format ("%2.1f", Window1.GetClock())); 
}

//Called whe configuration file load  preferences 

void
cboard_Blue_Pill::ReadPreferences(char *name, char *value)
{

 //read microcontroller of preferences
 if (!strcmp (name, "Blue_Pill_proc"))
  {
   Proc = value;
  }
 //read microcontroller clock
 if (!strcmp (name, "Blue_Pill_clock"))
 {
  Window1.SetClock (atof(value));
 } 
}


//Event on the board

void
cboard_Blue_Pill::EvKeyPress(uint key, uint mask)
{
 
}

//Event on the board

void
cboard_Blue_Pill::EvKeyRelease(uint key, uint mask)
{

}

//Event on the board

void
cboard_Blue_Pill::EvMouseButtonPress(uint button, uint x, uint y, uint state)
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
       /*
       if (Window1.Get_mcupwr () && reset (-1))//if powered
        {
         Window1.Set_mcupwr (0);
         Window1.Set_mcurst (1);
        }
        */ 
       MReset(-1);
       p_MCLR = 0;
       break;
      }
    }
  }

}

//Event on the board

void
cboard_Blue_Pill::EvMouseButtonRelease(uint button, uint x, uint y, uint state)
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
/*
         if (reset (-1))
          {
           Reset ();
          }
 */ 
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
cboard_Blue_Pill::Draw(CDraw *draw, double scale)
{
 int i;

 draw->Canvas.Init (scale, scale); //initialize draw context

 //board_x draw 
 for (i = 0; i < outputc; i++) //run over all outputs
  {
   if (!output[i].r)//if output shape is a rectangle
    {
  
     draw->Canvas.SetFgColor (0, 0, 0); //black

     switch (output[i].id)//search for color of output
      {
      case O_LED: //White using pc13 mean value 
       draw->Canvas.SetColor (pins[1].oavalue, 0 , 0);
       break;
      case O_LPWR: //Blue using mcupwr value
       draw->Canvas.SetColor (225 * Window1.Get_mcupwr () + 30, 0 ,0 );
       break;
      }

     draw->Canvas.Rectangle (1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);
    }

  }

 //end draw
 draw->Canvas.End ();
 draw->Update ();

}

void
cboard_Blue_Pill::Run_CPU(void)
{
 int i;
 int j;
 unsigned char pi;
 unsigned int alm[64];

 int JUMPSTEPS = Window1.GetJUMPSTEPS (); //number of steps skipped
 long int NSTEPJ = Window1.GetNSTEPJ (); //number of steps in 100ms


 //reset pins mean value
 memset (alm, 0, 64* sizeof (unsigned int));


 //Spare parts window pre process
 if (use_spare)Window5.PreProcess ();

 j = JUMPSTEPS; //step counter
 if (Window1.Get_mcupwr ()) //if powered
  for (i = 0; i < Window1.GetNSTEP (); i++) //repeat for number of steps in 100ms
   {

    /*
    if (j >= JUMPSTEPS)//if number of step is bigger than steps to skip 
     {
     }
    */
    //verify if a breakpoint is reached if not run one instruction 
    MStep ();
    //Oscilloscope window process
    if (use_oscope)Window4.SetSample ();
    //Spare parts window process
    if (use_spare)Window5.Process ();

    //increment mean value counter if pin is high 
    if (j < MGetPinCount ())
     alm[j] += pins[j].value;

    if (j >= JUMPSTEPS)//if number of step is bigger than steps to skip 
     {
      j = -1; //reset counter
     }
    
    j++; //counter increment
   }

 //calculate mean value
 for (pi = 0; pi < MGetPinCount (); pi++)
  {
   pins[pi].oavalue = (int) (((225.0 * alm[pi]) / NSTEPJ) + 30);
  }
 
 //Spare parts window pre post process
 if (use_spare)Window5.PostProcess ();

}

//Register the board in PICSimLab
board_init("Blue Pill", cboard_Blue_Pill); 

