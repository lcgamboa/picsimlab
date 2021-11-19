/* ########################################################################

   PICsimLab - PIC laboratory simulator

   ########################################################################

   Copyright (c) : 2020       El-khadraouy Mohammed <mohammed.el-khadraouy@ecole.ensicaen.fr>
   Copyright (c) : 2015-2019  Luis Claudio Gambôa Lopes

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
#include"../picsimlab4.h"   //Oscilloscope 
#include"../picsimlab5.h"   //Spare Parts
#include"../serial_port.h"
#include"exp_board_Curiosity_HPC.h"

/* ids of inputs of input map*/
enum
{
 I_POT1, //potentiometer
 I_ICSP, //ICSP connector
 I_PWR, //Power button
 I_RST, //Reset button
 I_S1, //S1 push button
 I_S2, //S2 push button
 I_JMP //JMP
};

/* ids of outputs of output map*/
enum
{
 O_POT1, //potentiometer
 O_JMP, //JMP
 O_D2, //LED D2 
 O_D3, //LED D3 
 O_D4, //LED D4 
 O_D5, //LED D5 
 O_D6, //LED D6 
 O_D7, //LED D7 
 O_RST, //Reset button
 O_S1, //S1 push button
 O_S2 //S2 push button 
};

//return the input ids numbers of names used in input map

unsigned short
cboard_Curiosity_HPC::get_in_id(char * name)
{
 if (strcmp (name, "PG_ICSP") == 0)return I_ICSP;
 if (strcmp (name, "SW_PWR") == 0)return I_PWR;
 if (strcmp (name, "PB_RST") == 0)return I_RST;
 if (strcmp (name, "PB_S1") == 0)return I_S1;
 if (strcmp (name, "PB_S2") == 0)return I_S2;
 if (strcmp (name, "JP_1") == 0)return I_JMP;
 if (strcmp (name, "PO_1") == 0)return I_POT1;

 printf ("Error input '%s' don't have a valid id! \n", name);
 return -1;
}

//return the output ids numbers of names used in output map

unsigned short
cboard_Curiosity_HPC::get_out_id(char * name)
{

 if (strcmp (name, "JP_1") == 0)return O_JMP;
 if (strcmp (name, "LD_D2") == 0)return O_D2;
 if (strcmp (name, "LD_D3") == 0)return O_D3;
 if (strcmp (name, "LD_D4") == 0)return O_D4;
 if (strcmp (name, "LD_D5") == 0)return O_D5;
 if (strcmp (name, "LD_D6") == 0)return O_D6;
 if (strcmp (name, "LD_D7") == 0)return O_D7;

 if (strcmp (name, "PB_S1") == 0)return O_S1;
 if (strcmp (name, "PB_S2") == 0)return O_S2;
 if (strcmp (name, "PO_1") == 0)return O_POT1;
 if (strcmp (name, "PB_RST") == 0)return O_RST;

 printf ("Error output '%s' don't have a valid id! \n", name);
 return 1;
}

//Constructor called once on board creation 

cboard_Curiosity_HPC::cboard_Curiosity_HPC(void)
{
 Proc = "PIC18F27K40"; //default microcontroller if none defined in preferences
 ReadMaps (); //Read input and output board maps
 jmp[0] = 0;

 pot1 = 100;

 active = 0;

 //controls properties and creation
 //gauge1
 gauge1 = new CGauge ();
 gauge1->SetFOwner (&Window1);
 gauge1->SetName (lxT ("gauge1_p8"));
 gauge1->SetX (48);
 gauge1->SetY (230 - 120);
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
 gauge2->SetName (lxT ("gauge2_p8"));
 gauge2->SetX (48);
 gauge2->SetY (255 - 120);
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
 gauge3->SetName (lxT ("gauge3_p8"));
 gauge3->SetX (48);
 gauge3->SetY (280 - 120);
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
 gauge4->SetName (lxT ("gauge4_p8"));
 gauge4->SetX (48);
 gauge4->SetY (305 - 120);
 gauge4->SetWidth (110);
 gauge4->SetHeight (20);
 gauge4->SetEnable (1);
 gauge4->SetVisible (1);
 gauge4->SetRange (100);
 gauge4->SetValue (0);
 gauge4->SetType (4);
 Window1.CreateChild (gauge4);
 //label2
 label2 = new CLabel ();
 label2->SetFOwner (&Window1);
 label2->SetName (lxT ("label2_p8"));
 label2->SetX (12);
 label2->SetY (230 - 120);
 label2->SetWidth (60);
 label2->SetHeight (20);
 label2->SetEnable (1);
 label2->SetVisible (1);
 label2->SetText (lxT ("RA4"));
 label2->SetAlign (1);
 Window1.CreateChild (label2);
 //label3
 label3 = new CLabel ();
 label3->SetFOwner (&Window1);
 label3->SetName (lxT ("label3_p8"));
 label3->SetX (13);
 label3->SetY (255 - 120);
 label3->SetWidth (60);
 label3->SetHeight (20);
 label3->SetEnable (1);
 label3->SetVisible (1);
 label3->SetText (lxT ("RA5"));
 label3->SetAlign (1);
 Window1.CreateChild (label3);
 //label4
 label4 = new CLabel ();
 label4->SetFOwner (&Window1);
 label4->SetName (lxT ("label4_p8"));
 label4->SetX (13);
 label4->SetY (280 - 120);
 label4->SetWidth (60);
 label4->SetHeight (20);
 label4->SetEnable (1);
 label4->SetVisible (1);
 label4->SetText (lxT ("RA6"));
 label4->SetAlign (1);
 Window1.CreateChild (label4);
 //label5
 label5 = new CLabel ();
 label5->SetFOwner (&Window1);
 label5->SetName (lxT ("label5_p8"));
 label5->SetX (13);
 label5->SetY (305 - 120);
 label5->SetWidth (60);
 label5->SetHeight (20);
 label5->SetEnable (1);
 label5->SetVisible (1);
 label5->SetText (lxT ("RA7"));
 label5->SetAlign (1);
 Window1.CreateChild (label5);
 //label6
 label6 = new CLabel ();
 label6->SetFOwner (&Window1);
 label6->SetName (lxT ("label6_p8"));
 label6->SetX (13);
 label6->SetY (305 + 30 - 120);
 label6->SetWidth (150);
 label6->SetHeight (20);
 label6->SetEnable (1);
 label6->SetVisible (1);
 label6->SetText (lxT ("Second serial port:"));
 label6->SetAlign (1);
 Window1.CreateChild (label6);
 //combo1
 combo1 = new CCombo ();
 combo1->SetFOwner (&Window1);
 combo1->SetName (lxT ("combo1_p8"));
 combo1->SetX (13);
 combo1->SetY (305 + 55 - 120);
 combo1->SetWidth (150);
 combo1->SetHeight (25);
 combo1->SetEnable (1);
 combo1->SetVisible (1);
#ifndef _WIN_   
 combo1->SetText (lxT ("/dev/tnt4"));
#else    
 combo1->SetText (lxT ("COM2"));
#endif  
 char * resp = serial_port_list ();
 if (resp)
  {
   combo1->SetItems (resp);
   free (resp);
  }

 Window1.CreateChild (combo1);
}

//Destructor called once on board destruction 

cboard_Curiosity_HPC::~cboard_Curiosity_HPC(void)
{
 //controls destruction 
 Window1.DestroyChild (gauge1);
 Window1.DestroyChild (gauge2);
 Window1.DestroyChild (gauge3);
 Window1.DestroyChild (gauge4);
 Window1.DestroyChild (label2);
 Window1.DestroyChild (label3);
 Window1.DestroyChild (label4);
 Window1.DestroyChild (label5);
 Window1.DestroyChild (label6);
 Window1.DestroyChild (combo1);
}

void
cboard_Curiosity_HPC::MSetSerial(const char * port)
{
 pic_set_serial (&pic, 0, port, 0, 0, 0);
 pic_set_serial (&pic, 1, combo1->GetText (), 0, 0, 0);
}


//Reset board status

void
cboard_Curiosity_HPC::Reset(void)
{
 pic_reset (1);

 p_BT[0] = 1; //set push button  in default state (high) 
 p_BT[1] = 1; //set push button  in default state (high) 

 //write button state to pic pin 25 (RB4)
 pic_set_pin (25, p_BT[0]);
 //write button state to pic pin 16 (RC5)
 pic_set_pin (16, p_BT[1]);


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

 if (use_spare)Window5.Reset ();

 RegisterRemoteControl ();
}

void
cboard_Curiosity_HPC::RegisterRemoteControl(void)
{
 for (int i = 0; i < inputc; i++)
  {
   switch (input[i].id)
    {
    case I_S1:
     input[i].status = &p_BT[0];
     break;
    case I_S2:
     input[i].status = &p_BT[1];
     break;
    case I_JMP:
     input[i].status = &jmp[0];
     break;
    case I_POT1:
     input[i].status = &pot1;
     break;
    }
  }

 for (int i = 0; i < outputc; i++)
  {
   switch (output[i].id)
    {
    case O_D2:
     output[i].status = &pic.pins[5].oavalue;
     break;
    case O_D3:
     output[i].status = &pic.pins[6].oavalue;
     break;
    case O_D4:
     output[i].status = &pic.pins[9].oavalue;
     break;
    case O_D5:
     output[i].status = &pic.pins[8].oavalue;
     break;
    }
  }
}


//Called ever 1s to refresh status

void
cboard_Curiosity_HPC::RefreshStatus(void)
{
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

}

//Called to save board preferences in configuration file

void
cboard_Curiosity_HPC::WritePreferences(void)
{
 //write selected microcontroller of board_Curiosity_HPC to preferences
 Window1.saveprefs (lxT ("Curiosity_HPC_proc"), Proc);
 Window1.saveprefs (lxT ("Curiosity_HPC_jmp"), lxString ().Format ("%i", jmp[0]));
 Window1.saveprefs (lxT ("Curiosity_HPC_clock"), lxString ().Format ("%2.1f", Window1.GetClock ()));
 Window1.saveprefs (lxT ("Curiosity_HPC_serial2"), combo1->GetText ());
 Window1.saveprefs (lxT ("Curiosity_HPC_pot1"), lxString ().Format ("%i", pot1));
}

//Called whe configuration file load  preferences 

void
cboard_Curiosity_HPC::ReadPreferences(char *name, char *value)
{
 //read microcontroller of preferences
 if (!strcmp (name, "Curiosity_HPC_proc"))
  {
   Proc = value;
  }

 if (!strcmp (name, "Curiosity_HPC_jmp"))
  {
   int i;
   for (i = 0; i < 1; i++)
    {
     if (value[i] == '0')
      jmp[i] = 0;
     else
      jmp[i] = 1;
    }
  }
 if (!strcmp (name, "Curiosity_HPC_clock"))
  {
   Window1.SetClock (atof (value));
  }
 if (!strcmp (name, "Curiosity_HPC_serial2"))
  {
   combo1->SetText (value);
   pic_set_serial (&pic, 1, value, 0, 0, 0);
  }

 if (!strcmp (name, "Curiosity_HPC_pot1"))
  {
   pot1 = atoi (value);
  }

}

//Event on the board

void
cboard_Curiosity_HPC::EvKeyPress(uint key, uint mask)
{
 //if keyboard key 1 is pressed then activate button (state=0)   
 if (key == '1')
  {
   p_BT[0] = 0;
  }
 if (key == '2')
  {
   p_BT[1] = 0;
  }

}

//Event on the board

void
cboard_Curiosity_HPC::EvKeyRelease(uint key, uint mask)
{
 //if keyboard key 1 is pressed then desactivate button (state=1)     
 if (key == '1')
  {
   p_BT[0] = 1;
  }
 if (key == '2')
  {
   p_BT[1] = 1;
  }
}

//Event on the board

void
cboard_Curiosity_HPC::EvMouseButtonPress(uint button, uint x, uint y, uint state)
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
         Window1.Set_mcupwr (0);
         Reset ();
         p_BT[0] = 1;
         p_BT[1] = 1;
        }
       else //if off turn on
        {
         Window1.Set_mcupwr (1);
         Reset ();
        }
       break;
       //if event is over I_RST area then turn off and reset
      case I_RST:
       if (Window1.Get_mcupwr () && pic_reset (-1))//if powered
        {
         Window1.Set_mcupwr (0);
         Window1.Set_mcurst (1);
        }
       p_RST = 0;
       break;
       //if event is over I_S1 area then activate button (state=0) 
      case I_S1:
       p_BT[0] = 0;
       break;
       //if event is over I_S2 area then activate button (state=0) 
      case I_S2:
       p_BT[1] = 0;
       break;
      case I_JMP:
       jmp[0] ^= 0x01;
       break;
      case I_POT1:
       {
        active = 1;
        pot1 = CalcAngle (i, x, y);
       }
       break;
      }
    }
  }

}

void
cboard_Curiosity_HPC::EvMouseMove(uint button, uint x, uint y, uint state)
{
 int i;

 for (i = 0; i < inputc; i++)
  {
   switch (input[i].id)
    {
    case I_POT1:
     if (((input[i].x1 <= x)&&(input[i].x2 >= x))&&((input[i].y1 <= y)&&(input[i].y2 >= y)))
      {
       if (active)
        {
         pot1 = CalcAngle (i, x, y);
        }
      }
     else
      {
       active = 0;
      }
     break;
    }
  }
}

//Event on the board

void
cboard_Curiosity_HPC::EvMouseButtonRelease(uint button, uint x, uint y, uint state)
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

         if (pic_reset (-1))
          {
           Reset ();
          }
        }
       p_RST = 1;
       break;
       //if event is over I_S1 area then deactivate button (state=1) 
      case I_S1:
       p_BT[0] = 1;
       break;
       //if event is over I_S2 area then deactivate button (state=1) 
      case I_S2:
       p_BT[1] = 1;
       break;
      case I_POT1:
       {
        active = 0;
       }
       break;
      }
    }
  }

}

//Called ever 100ms to draw board
//This is the critical code for simulator running speed

void
cboard_Curiosity_HPC::Draw(CDraw *draw)
{
 int i;

 draw->Canvas.Init (Scale, Scale); //initialize draw context

 //board_8 draw 
 for (i = 0; i < outputc; i++) //run over all outputs
  {
   if (!output[i].r)//if output shape is a rectangle
    {
     draw->Canvas.SetFgColor (0, 0, 0); //black

     switch (output[i].id)//search for color of output
      {
      case O_D6: //green using picpwr value
       draw->Canvas.SetColor (0, 200 * Window1.Get_mcupwr () + 55, 0);
       break;
      case O_D2: //Red using pin 6 mean  value (RA4) 
       draw->Canvas.SetColor (pic.pins[5].oavalue, 0, 0);
       break;
      case O_D3: //Red using pin 7 mean value (RA5)
       draw->Canvas.SetColor (pic.pins[6].oavalue, 0, 0);
       break;
      case O_D4: //Red using pin 10 mean value (RA6)
       draw->Canvas.SetColor (pic.pins[9].oavalue, 0, 0);
       break;
      case O_D5: //Red using pin 9 mean value (RA7)
       draw->Canvas.SetColor (pic.pins[8].oavalue, 0, 0);
       break;
      case O_JMP:
       draw->Canvas.SetColor (150, 150, 150);
       break;
      case O_S1:
      case O_S2:
      case O_RST:
       draw->Canvas.SetColor (100, 100, 100);
       break;
      case O_POT1:
       draw->Canvas.SetColor (100, 100, 100);
       break;
      }

     //draw a rectangle
     draw->Canvas.Rectangle (1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);

     if ((output[i].id == O_S1) || (output[i].id == O_S2))
      {
       if (p_BT[output[i].id - O_S1])
        {
         draw->Canvas.SetColor (15, 15, 15);
        }
       else
        {
         draw->Canvas.SetColor (55, 55, 55);
        }
       draw->Canvas.Circle (1, output[i].cx, output[i].cy, 13);
      }
     else if (output[i].id == O_RST)
      {
       if (p_RST)
        {
         draw->Canvas.SetColor (15, 15, 15);
        }
       else
        {
         draw->Canvas.SetColor (55, 55, 55);
        }
       draw->Canvas.Circle (1, output[i].cx, output[i].cy, 13);
      }
     else if (output[i].id == O_POT1)
      {

       draw->Canvas.SetColor (80, 80, 80);
       draw->Canvas.Circle (1, output[i].cx, output[i].cy, 25);

       draw->Canvas.SetColor (150, 150, 150);
       int x = -20 * sin ((5.585 * (pot1 / 200.0)) + 0.349);
       int y = 20 * cos ((5.585 * (pot1 / 200.0)) + 0.349);
       draw->Canvas.Circle (1, output[i].cx + x, output[i].cy + y, 3);

      }
     else if (output[i].id == O_JMP)
      {
       if (!jmp[0])
        {
         draw->Canvas.SetColor (70, 70, 70);
         draw->Canvas.Rectangle (1, output[i].x1, output[i].y1, (int) ((output[i].x2 - output[i].x1)*0.65), output[i].y2 - output[i].y1);
         draw->Canvas.SetColor (220, 220, 0);
         draw->Canvas.Circle (1, output[i].x1 + (int) ((output[i].x2 - output[i].x1)*0.80), output[i].y1 + ((output[i].y2 - output[i].y1) / 2), 3);
        }
       else
        {
         draw->Canvas.SetColor (70, 70, 70);
         draw->Canvas.Rectangle (1, output[i].x1 + ((int) ((output[i].x2 - output[i].x1)*0.35)), output[i].y1, (int) ((output[i].x2 - output[i].x1)*0.65), output[i].y2 - output[i].y1);
         draw->Canvas.SetColor (220, 220, 0);
         draw->Canvas.Circle (1, output[i].x1 + (int) ((output[i].x2 - output[i].x1)*0.20), output[i].y1 + ((output[i].y2 - output[i].y1) / 2), 3);
        }
      }

    }
   else //if output shape is a circle
    {

    }

  }

 //end draw
 draw->Canvas.End ();
 draw->Update ();



 //RA4 mean value to gauge1
 gauge1->SetValue ((pic.pins[5].oavalue - 55) / 2);
 //RA5 mean value to gauge2
 gauge2->SetValue ((pic.pins[6].oavalue - 55) / 2);
 //RA6 mean value to gauge3
 gauge3->SetValue ((pic.pins[9].oavalue - 55) / 2);
 //RA7 mean value to gauge4
 gauge4->SetValue ((pic.pins[8].oavalue - 55) / 2);


}

void
cboard_Curiosity_HPC::Run_CPU(void)
{
 int i;
 int j;
 unsigned char pi;
 const picpin * pins;
 unsigned int alm[28];

 const int JUMPSTEPS = Window1.GetJUMPSTEPS (); //number of steps skipped
 const long int NSTEP = Window1.GetNSTEP (); //number of steps in 100ms
 const float RNSTEP = 200.0 * pic.PINCOUNT / NSTEP;

 //reset mean value
 memset (alm, 0, 20 * sizeof (unsigned int));


 //read pic.pins to a local variable to speed up 
 pins = pic.pins;

 if (use_spare)Window5.PreProcess ();

 j = JUMPSTEPS; //step counter
 pi = 0;
 if (Window1.Get_mcupwr ()) //if powered
  for (i = 0; i < NSTEP; i++) //repeat for number of steps in 100ms
   {


    if (j >= JUMPSTEPS)//if number of step is bigger than steps to skip 
     {
      pic_set_pin (pic.mclr, p_RST);
      pic_set_pin (25, p_BT[0]); //Set pin 25 (RB4) with button state 
      pic_set_pin (16, p_BT[1]); //Set pin 16 (RC5) with button state
     }

    //verify if a breakpoint is reached if not run one instruction 
    if (!mplabxd_testbp ())pic_step ();
    ioupdated = pic.ioupdated;
    if (use_oscope)Window4.SetSample ();
    if (use_spare)Window5.Process ();

    //increment mean value counter if pin is high
    alm[pi] += pins[pi].value;
    pi++;
    if (pi == pic.PINCOUNT)pi = 0;

    if (j >= JUMPSTEPS)//if number of step is bigger than steps to skip 
     {
      //set analog pin 2 (RA0 AN4) with value from scroll  
      pic_set_apin (2, (5.0 * pot1 / 199));
      j = -1; //reset counter
     }

    j++; //counter increment
    pic.ioupdated = 0;
   }

 //calculate mean value
 for (pi = 0; pi < pic.PINCOUNT; pi++)
  {
   pic.pins[pi].oavalue = (int) ((alm[pi] * RNSTEP) + 55);
  }
 if (use_spare)Window5.PostProcess ();
}


board_init(BOARD_Curiosity_HPC_Name, cboard_Curiosity_HPC);