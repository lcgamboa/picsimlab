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
#include"board_Arduino_Uno.h"

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
 O_L, //LED 
 O_RX, //LED on PD0
 O_TX, //LED on PD1
 O_ON, //Power LED
 O_RST //Reset button
};
//return the input ids numbers of names used in input map

unsigned short
cboard_Arduino_Uno::get_in_id(char * name)
{
 if (strcmp (name, "PG_ICSP") == 0)return I_ICSP;
 if (strcmp (name, "SW_PWR") == 0)return I_PWR;
 if (strcmp (name, "PB_RST") == 0)return I_RST;

 printf ("Erro input '%s' don't have a valid id! \n", name);
 return -1;
}

//return the output ids numbers of names used in output map

unsigned short
cboard_Arduino_Uno::get_out_id(char * name)
{

 if (strcmp (name, "LD_L") == 0)return O_L;
 if (strcmp (name, "LD_TX") == 0)return O_TX;
 if (strcmp (name, "LD_RX") == 0)return O_RX;
 if (strcmp (name, "LD_ON") == 0)return O_ON;
 if (strcmp (name, "PB_RST") == 0)return O_RST;

 printf ("Erro output '%s' don't have a valid id! \n", name);
 return 1;
}

//Constructor called once on board creation 

cboard_Arduino_Uno::cboard_Arduino_Uno(void)
{

 Proc = "atmega328p"; //default microcontroller if none defined in preferences
 ReadMaps (); //Read input and output board maps

 //controls properties and creation

 //gauge1
 gauge1 = new CGauge ();
 gauge1->SetFOwner (&Window1);
 gauge1->SetName (lxT ("gauge1_p5"));
 gauge1->SetX (35);
 gauge1->SetY (74);
 gauge1->SetWidth (110);
 gauge1->SetHeight (20);
 gauge1->SetEnable (1);
 gauge1->SetVisible (1);
 gauge1->SetRange (100);
 gauge1->SetValue (0);
 gauge1->SetType (4);
 Window1.CreateChild (gauge1);
 //gauge2
 gauge2 = new CGauge ();
 gauge2->SetFOwner (&Window1);
 gauge2->SetName (lxT ("gauge2_p5"));
 gauge2->SetX (35);
 gauge2->SetY (100);
 gauge2->SetWidth (110);
 gauge2->SetHeight (20);
 gauge2->SetEnable (1);
 gauge2->SetVisible (1);
 gauge2->SetRange (100);
 gauge2->SetValue (0);
 gauge2->SetType (4);
 Window1.CreateChild (gauge2);
 //gauge3
 gauge3 = new CGauge ();
 gauge3->SetFOwner (&Window1);
 gauge3->SetName (lxT ("gauge3_p5"));
 gauge3->SetX (35);
 gauge3->SetY (125);
 gauge3->SetWidth (110);
 gauge3->SetHeight (20);
 gauge3->SetEnable (1);
 gauge3->SetVisible (1);
 gauge3->SetRange (100);
 gauge3->SetValue (0);
 gauge3->SetType (4);
 Window1.CreateChild (gauge3);
 //gauge4
 gauge4 = new CGauge ();
 gauge4->SetFOwner (&Window1);
 gauge4->SetName (lxT ("gauge4_p5"));
 gauge4->SetX (35);
 gauge4->SetY (150);
 gauge4->SetWidth (110);
 gauge4->SetHeight (20);
 gauge4->SetEnable (1);
 gauge4->SetVisible (1);
 gauge4->SetRange (100);
 gauge4->SetValue (0);
 gauge4->SetType (4);
 Window1.CreateChild (gauge4);
 //gauge5
 gauge5 = new CGauge ();
 gauge5->SetFOwner (&Window1);
 gauge5->SetName (lxT ("gauge5_p5"));
 gauge5->SetX (35);
 gauge5->SetY (175);
 gauge5->SetWidth (110);
 gauge5->SetHeight (20);
 gauge5->SetEnable (1);
 gauge5->SetVisible (1);
 gauge5->SetRange (100);
 gauge5->SetValue (0);
 gauge5->SetType (4);
 Window1.CreateChild (gauge5);
 //gauge6
 gauge6 = new CGauge ();
 gauge6->SetFOwner (&Window1);
 gauge6->SetName (lxT ("gauge6_p5"));
 gauge6->SetX (35);
 gauge6->SetY (200);
 gauge6->SetWidth (110);
 gauge6->SetHeight (20);
 gauge6->SetEnable (1);
 gauge6->SetVisible (1);
 gauge6->SetRange (100);
 gauge6->SetValue (0);
 gauge6->SetType (4);
 Window1.CreateChild (gauge6);

 //label1
 label1 = new CLabel ();
 label1->SetFOwner (&Window1);
 label1->SetName (lxT ("label1_p5"));
 label1->SetX (12);
 label1->SetY (75);
 label1->SetWidth (20);
 label1->SetHeight (20);
 label1->SetEnable (1);
 label1->SetVisible (1);
 label1->SetText (lxT ("3"));
 label1->SetAlign (1);
 Window1.CreateChild (label1);
 //label2
 label2 = new CLabel ();
 label2->SetFOwner (&Window1);
 label2->SetName (lxT ("label2_p5"));
 label2->SetX (12);
 label2->SetY (100);
 label2->SetWidth (20);
 label2->SetHeight (20);
 label2->SetEnable (1);
 label2->SetVisible (1);
 label2->SetText (lxT ("5"));
 label2->SetAlign (1);
 Window1.CreateChild (label2);
 //label3
 label3 = new CLabel ();
 label3->SetFOwner (&Window1);
 label3->SetName (lxT ("label3_p5"));
 label3->SetX (13);
 label3->SetY (125);
 label3->SetWidth (20);
 label3->SetHeight (20);
 label3->SetEnable (1);
 label3->SetVisible (1);
 label3->SetText (lxT ("6"));
 label3->SetAlign (1);
 Window1.CreateChild (label3);
 //label4
 label4 = new CLabel ();
 label4->SetFOwner (&Window1);
 label4->SetName (lxT ("label4_p5"));
 label4->SetX (13);
 label4->SetY (150);
 label4->SetWidth (20);
 label4->SetHeight (20);
 label4->SetEnable (1);
 label4->SetVisible (1);
 label4->SetText (lxT ("9"));
 label4->SetAlign (1);
 Window1.CreateChild (label4);
 //label5
 label5 = new CLabel ();
 label5->SetFOwner (&Window1);
 label5->SetName (lxT ("label5_p5"));
 label5->SetX (13);
 label5->SetY (175);
 label5->SetWidth (20);
 label5->SetHeight (20);
 label5->SetEnable (1);
 label5->SetVisible (1);
 label5->SetText (lxT ("10"));
 label5->SetAlign (1);
 Window1.CreateChild (label5);
 //label6
 label6 = new CLabel ();
 label6->SetFOwner (&Window1);
 label6->SetName (lxT ("label6_p5"));
 label6->SetX (13);
 label6->SetY (200);
 label6->SetWidth (20);
 label6->SetHeight (20);
 label6->SetEnable (1);
 label6->SetVisible (1);
 label6->SetText (lxT ("11"));
 label6->SetAlign (1);
 Window1.CreateChild (label6);
}

//Destructor called once on board destruction 

cboard_Arduino_Uno::~cboard_Arduino_Uno(void)
{
 //controls destruction 
 Window1.DestroyChild (gauge1);
 Window1.DestroyChild (gauge2);
 Window1.DestroyChild (gauge3);
 Window1.DestroyChild (gauge4);
 Window1.DestroyChild (gauge5);
 Window1.DestroyChild (gauge6);

 Window1.DestroyChild (label1);
 Window1.DestroyChild (label2);
 Window1.DestroyChild (label3);
 Window1.DestroyChild (label4);
 Window1.DestroyChild (label5);
 Window1.DestroyChild (label6);

}

//Reset board status

void
cboard_Arduino_Uno::Reset(void)
{

 //write button state to pic pin 19 (RD0)
 //pic_set_pin(19,p_BT1); 
 //write switch state to pic pin 20 (RD1)
 //pic_set_pin(20,p_BT2); 
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

 /*
   //reset mean value
   for(int pi=0;pi < pic.PINCOUNT;pi++)
   {
     lm[pi]=0;
   }
  */
 if (use_spare)Window5.Reset ();

 RegisterRemoteControl ();
}

void
cboard_Arduino_Uno::RegisterRemoteControl(void)
{
 output_ids[O_L]->status = &pins[18].oavalue;
}

//Called ever 1s to refresh status

void
cboard_Arduino_Uno::RefreshStatus(void)
{
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

 if (avr)
  {
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
  }
}

//Called to save board preferences in configuration file

void
cboard_Arduino_Uno::WritePreferences(void)
{
 //write selected microcontroller of board_x to preferences
 Window1.saveprefs (lxT ("Arduino_Uno_proc"), Proc);
 Window1.saveprefs (lxT ("Arduino_Uno_clock"), lxString ().Format ("%2.1f", Window1.GetClock ()));
}

//Called whe configuration file load  preferences 

void
cboard_Arduino_Uno::ReadPreferences(char *name, char *value)
{
 //read microcontroller of preferences
 if (!strcmp (name, "Arduino_Uno_proc"))
  {
   Proc = value;
  }

 if (!strcmp (name, "Arduino_Uno_clock"))
  {
   Window1.SetClock (atof (value));
  }
}


//Event on the board

void
cboard_Arduino_Uno::EvKeyPress(uint key, uint mask) { }

//Event on the board

void
cboard_Arduino_Uno::EvKeyRelease(uint key, uint mask) { }

//Event on the board

void
cboard_Arduino_Uno::EvMouseButtonPress(uint button, uint x, uint y, uint state)
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
       output_ids[O_ON]->update = 1;
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
cboard_Arduino_Uno::EvMouseButtonRelease(uint button, uint x, uint y, uint state)
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
cboard_Arduino_Uno::Draw(CDraw *draw)
{
 int i;
 int update = 0; //verifiy if updated is needed


 //board  draw 
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
     if (!output[i].r)//if output shape is a rectangle
      {
       switch (output[i].id)
        {
        case O_ON:
         draw->Canvas.SetColor (0, 200 * Window1.Get_mcupwr () + 55, 0);
         break;
        case O_RX:
         draw->Canvas.SetColor (0, (255 - pins[1].oavalue) * Window1.Get_mcupwr (), 0);
         break;
        case O_TX:
         draw->Canvas.SetColor (0, (255 - ((unsigned char) pins[2].oavalue * 10)) * Window1.Get_mcupwr (), 0);
         break;
        case O_L:
         draw->Canvas.SetColor (0, pins[18].oavalue, 0);
         break;
        case O_RST:
         draw->Canvas.SetColor (100, 100, 100);
         break;
        default:
         draw->Canvas.SetColor (0, 0, 0);
         break;
        }

       if (output[i].id == O_RST)
        {
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
        }
       else
        {
         draw->Canvas.Rectangle (1, output[i].x1, output[i].y1,
                                 output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);

        }
      }
    }
  }

 //end draw
 if (update)
  {
   draw->Canvas.End ();
   draw->Update ();
  }

 gauge1->SetValue ((pins[4].oavalue - 55) / 2);
 gauge2->SetValue ((pins[10].oavalue - 55) / 2);
 gauge3->SetValue ((pins[11].oavalue - 55) / 2);
 gauge4->SetValue ((pins[14].oavalue - 55) / 2);
 gauge5->SetValue ((pins[15].oavalue - 55) / 2);
 gauge6->SetValue ((pins[16].oavalue - 55) / 2);

}

void
cboard_Arduino_Uno::Run_CPU(void)
{

 int i;
 //int j;
 unsigned char pi;
 const picpin *pins;
 unsigned int alm[40];

 //int JUMPSTEPS = Window1.GetJUMPSTEPS ()*4.0; //number of steps skipped
 const int pinc = MGetPinCount ();
 const long int NSTEP = 4.0 * Window1.GetNSTEP (); //number of steps in 100ms
 const float RNSTEP = 200.0 * pinc / NSTEP;

 long long unsigned int cycle_start;
 int twostep = 0;


 //reset mean value

 memset (alm, 0, pinc * sizeof (unsigned int));

 //read pic.pins to a local variable to speed up 

 pins = MGetPinsValues ();

 if (use_spare)Window5.PreProcess ();

 //j = JUMPSTEPS; //step counter
 pi = 0;
 if (Window1.Get_mcupwr ()) //if powered
  for (i = 0; i < NSTEP; i++) //repeat for number of steps in 100ms
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


    UpdateHardware ();

    //avr->sleep_usec=0;
    if (use_oscope)Window4.SetSample ();
    if (use_spare)Window5.Process ();

    //increment mean value counter if pin is high
    alm[pi] += pins[pi].value;
    pi++;
    if (pi == pinc)pi = 0;
    /*
        if (j >= JUMPSTEPS)//if number of step is bigger than steps to skip 
         {
          //set analog pin 2 (AN0) with value from scroll  
          //pic_set_apin(2,((5.0*(scroll1->GetPosition()))/
          //  (scroll1->GetRange()-1)));

          j = -1; //reset counter
         }
        j++; //counter increment   
     */
   }

 //calculate mean value
 for (pi = 0; pi < MGetPinCount (); pi++)
  {
   cboard_Arduino_Uno::pins[pi].oavalue = (int) ((alm[pi] * RNSTEP) + 55);
  }

 if (use_spare)Window5.PostProcess ();

 //verifiy if LEDS need update 
 if (output_ids[O_RX]->value != pins[1].oavalue)
  {
   output_ids[O_RX]->value = pins[1].oavalue;
   output_ids[O_RX]->update = 1;
  }
 if (output_ids[O_TX]->value != pins[2].oavalue)
  {
   output_ids[O_TX]->value = pins[2].oavalue;
   output_ids[O_TX]->update = 1;
  }
 if (output_ids[O_L]->value != pins[18].oavalue)
  {
   output_ids[O_L]->value = pins[18].oavalue;
   output_ids[O_L]->update = 1;
  }
}


board_init(BOARD_Arduino_Uno_Name, cboard_Arduino_Uno);

