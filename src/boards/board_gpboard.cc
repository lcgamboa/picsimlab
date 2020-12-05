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
#include"board_gpboard.h"

#ifndef _WIN_
#define INVALID_HANDLE_VALUE -1;
#endif

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
 O_MP,
};

//return the input ids numbers of names used in input map

unsigned short
cboard_gpboard::get_in_id(char * name)
{
 if (strcmp (name, "I_ICSP") == 0)return I_ICSP;
 if (strcmp (name, "I_PWR") == 0)return I_PWR;
 if (strcmp (name, "I_RST") == 0)return I_RST;

 printf ("Error input '%s' don't have a valid id! \n", name);
 return -1;
}

//return the output ids numbers of names used in output map

unsigned short
cboard_gpboard::get_out_id(char * name)
{

 if (strcmp (name, "O_MP") == 0)return O_MP;
 if (strcmp (name, "O_LPWR") == 0)return O_LPWR;


 printf ("Error output '%s' don't have a valid id! \n", name);
 return 1;
}

//Constructor called once on board creation 

cboard_gpboard::cboard_gpboard(void)
{
 Proc = "pic16f628a"; //default microcontroller if none defined in preferences
 ReadMaps (); //Read input and output board maps
 lxImage image;
 image.LoadFile (Window1.GetSharePath () + lxT ("boards/Common/ic40.png"));
 micbmp = new lxBitmap (image, &Window1);
 serialfd = INVALID_HANDLE_VALUE;
}

//Destructor called once on board destruction 

cboard_gpboard::~cboard_gpboard(void)
{
 delete micbmp;
 micbmp = NULL;
}

//Reset board status

void
cboard_gpboard::Reset(void)
{
 MReset (1);


 Window1.statusbar1.SetField (2, lxT ("Serial: ") + lxString::FromAscii (SERIALDEVICE));

 if (use_spare)Window5.Reset ();
}

//Called ever 1s to refresh status

void
cboard_gpboard::RefreshStatus(void)
{

 Window1.statusbar1.SetField (2, lxT ("Serial: ") + lxString::FromAscii (SERIALDEVICE));

}

//Called to save board preferences in configuration file

void
cboard_gpboard::WritePreferences(void)
{
 //write selected microcontroller of board_x to preferences
 Window1.saveprefs (lxT ("gpboard_proc"), Proc);
 //write microcontroller clock to preferences
 Window1.saveprefs (lxT ("gpboard_clock"), lxString ().Format ("%2.1f", Window1.GetClock ()));
}

//Called whe configuration file load  preferences 

void
cboard_gpboard::ReadPreferences(char *name, char *value)
{

 //read microcontroller of preferences
 if (!strcmp (name, "gpboard_proc"))
  {
   Proc = value;
  }
 //read microcontroller clock
 if (!strcmp (name, "gpboard_clock"))
  {
   Window1.SetClock (atof (value));
  }
}


//Event on the board

void
cboard_gpboard::EvKeyPress(uint key, uint mask) { }

//Event on the board

void
cboard_gpboard::EvKeyRelease(uint key, uint mask) { }

//Event on the board

void
cboard_gpboard::EvMouseButtonPress(uint button, uint x, uint y, uint state)
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
       MReset (-1);
       p_MCLR = 0;
       break;
      }
    }
  }

}

//Event on the board

void
cboard_gpboard::EvMouseButtonRelease(uint button, uint x, uint y, uint state)
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
cboard_gpboard::Draw(CDraw *draw, double scale)
{
 int i;
 lxRect rec;
 lxSize ps;

 draw->Canvas.Init (scale, scale); //initialize draw context

 //board_x draw 
 for (i = 0; i < outputc; i++) //run over all outputs
  {
   if (!output[i].r)//if output shape is a rectangle
    {

     draw->Canvas.SetFgColor (0, 0, 0); //black

     switch (output[i].id)//search for color of output
      {
      case O_LPWR: //Blue using mcupwr value
       draw->Canvas.SetColor (225 * Window1.Get_mcupwr () + 30, 0, 0);
       draw->Canvas.Rectangle (1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);
       break;
      case O_MP:
       lxFont font (
                    (MGetPinCount () >= 100) ? 9 : ((MGetPinCount () > 14) ? 12 : 10)
                    , lxFONTFAMILY_TELETYPE, lxFONTSTYLE_NORMAL, lxFONTWEIGHT_NORMAL);

       draw->Canvas.SetFont (font);

       ps = micbmp->GetSize ();
       draw->Canvas.PutBitmap (micbmp, output[i].x1, output[i].y1);
       draw->Canvas.SetFgColor (255, 255, 255);

       rec.x = output[i].x1;
       rec.y = output[i].y1;
       rec.width = ps.GetWidth ();
       rec.height = ps.GetHeight ();
       draw->Canvas.TextOnRect (Proc, rec, lxALIGN_CENTER | lxALIGN_CENTER_VERTICAL);
       break;
      }
    }

  }

 //end draw
 draw->Canvas.End ();
 draw->Update ();

}

void
cboard_gpboard::Run_CPU(void)
{
 int i;
 int j;
 unsigned char pi;
 unsigned int alm[64];

 int JUMPSTEPS = Window1.GetJUMPSTEPS (); //number of steps skipped
 long int NSTEPJ = Window1.GetNSTEPJ (); //number of steps in 100ms


 //reset pins mean value
 memset (alm, 0, 64 * sizeof (unsigned int));


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

int
cboard_gpboard::MInit(const char * processor, const char * fname, float freq)
{

 int ret = board_gpsim::MInit (processor, fname, freq);

 if (ret == -1)
  {
   printf ("PICSimLab: Unknown processor %s, loading default !\n", processor);
   board_gpsim::MInit ("pic16f628a", fname, freq);
   Proc = "pic16f628a";
  }

 lxImage image;

 if (!image.LoadFile (Window1.GetSharePath () + lxT ("boards/Common/ic") + itoa (MGetPinCount ()) + lxT (".png")))
  {
   image.LoadFile (Window1.GetSharePath () + lxT ("boards/Common/ic6.png"));
   printf ("picsimlab: IC package with %i pins not found!\n", MGetPinCount ());
   printf ("picsimlab: %s not found!\n", (const char *) (Window1.GetSharePath () + lxT ("boards/Common/ic") + itoa (MGetPinCount ()) + lxT (".png")).c_str ());
  }

 if (micbmp) delete micbmp;
 micbmp = new lxBitmap (image, &Window1);


 return ret;
}

//Register the board in PICSimLab
board_init("gpboard", cboard_gpboard);

