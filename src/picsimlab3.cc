/* ########################################################################

   PICsimLab - PIC laboratory simulator

   ########################################################################

   Copyright (c) : 2010-2018  Luis Claudio Gambôa Lopes

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

//Configuration window

#include"picsimlab3.h"
#include"picsimlab3_d.cc"

#include"picsimlab1.h"
#include"serial_port.h" 

CPWindow3 Window3;

#ifdef _USE_PICSTARTP_
extern char PROGDEVICE[100];
#endif

extern char SERIALDEVICE[100];


//Implementation

void
CPWindow3::_EvOnCreate(CControl * control)
{
 combo1.SetText (lxString::FromAscii (SERIALDEVICE));
#ifdef _USE_PICSTARTP_  
 combo2.SetText (lxString::FromAscii (PROGDEVICE));
 combo2.SetVisible (true);
 label2.SetVisible (true);
#else  
 combo2.SetVisible (false);
 label2.SetVisible (false);
#endif

}

void
CPWindow3::button1_EvMouseButtonClick(CControl * control, uint button, uint x, uint y, uint state)
{
 int osc_on = Window1.GetBoard ()->GetUseOscilloscope ();
 int spare_on = Window1.GetBoard ()->GetUseSpareParts ();

 Window1.Set_debug_type (combo3.GetText ().compare ("GDB") == 0);
 
 Window1.Set_debug_port (spin1.GetValue ());

 Window1.Set_remotec_port (spin2.GetValue ());
 
#ifdef _USE_PICSTARTP_ 
 if (combo1.GetText () == combo2.GetText ())
  {
   Message (lxT ("Use diferent ports!"));
   return;
  }
#endif
 strcpy (SERIALDEVICE, (char*) combo1.GetText ().char_str ());
#ifdef _USE_PICSTARTP_
 strcpy (PROGDEVICE, (char*) combo2.GetText ().char_str ());
#endif
 Window1.EndSimulation ();
 Window1.Configure (Window1.GetHOME());

 if (osc_on) Window1.menu1_Modules_Oscilloscope_EvMenuActive (this);
 if (spare_on) Window1.menu1_Modules_Spareparts_EvMenuActive (this);

 WDestroy ();
}
void
CPWindow3::button2_EvMouseButtonClick(CControl * control, uint button, uint x, uint y, uint state)
{
 WDestroy ();
}

void
CPWindow3::_EvOnShow(CControl * control)
{

 char * resp = serial_port_list ();

 if (resp)
  {
   lxString temp;
   temp = combo1.GetText ();
   combo1.SetItems (resp);
   combo1.SetText (temp);

   temp = combo2.GetText ();
   combo2.SetItems (resp);
   combo2.SetText (temp);

   free (resp);
  }
 else
  {
   printf ("No serial ports found!\n");
  }

 if (Window1.Get_debug_type ())
  {
   combo3.SetText ("GDB");
  }
 else
  {
   combo3.SetText ("MDB");
  }
 
  spin1.SetValue (Window1.Get_debug_port ());
  
  spin2.SetValue (Window1.Get_remotec_port ());
}



