/* ########################################################################

   PICsimLab - PIC laboratory simulator

   ########################################################################

   Copyright (c) : 2010-2017  Luis Claudio Gambôa Lopes

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


#include"parts_defs.h" 

//includes of parts
#include"part_servo.h" 
#include"part_step.h" 
#include"part_d_transfer_function.h"
#include"part_push_buttons.h"
#include"part_switchs.h"
#include"part_LEDs.h"
#include"part_pot.h"
#include"part_RGB_LED.h"

#ifdef _EXPERIMENTAL_
const char parts_list[NUM_PARTS][30]={"Servo motor","Step motor","D. Transfer function","Push buttons", "Switchs","LEDs","Potentiometers","RGB LED",};
#else
const char parts_list[NUM_PARTS][30]={"Servo motor","Step motor","Push buttons", "Switchs","LEDs","Potentiometers","RGB LED",};
#endif

//boards object creation
part * create_part(String name, unsigned int x, unsigned int y)
{
   part * part_=NULL; 
 
   if(name.compare(wxT("Servo motor")) == 0 )part_= new cpart_servo(x,y);

   if(name.compare(wxT("Step motor")) == 0 )part_= new cpart_step(x,y);
#ifdef _EXPERIMENTAL_   
   if(name.compare(wxT("D. Transfer function")) == 0 )part_= new cpart_dtfunc(x,y);
#endif
   if(name.compare(wxT("Push buttons")) == 0 )part_= new cpart_pbuttons(x,y);
   
   if(name.compare(wxT("Switchs")) == 0 )part_= new cpart_switchs(x,y);
   
   if(name.compare(wxT("LEDs")) == 0 )part_= new cpart_leds(x,y);
   
   if(name.compare(wxT("Potentiometers")) == 0 )part_= new cpart_pot(x,y);
   
   if(name.compare(wxT("RGB LED")) == 0 )part_= new cpart_rgb_led(x,y);
   
   return part_; 
}
