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
#include"part_LCD_hd44780.h"
#include"part_LCD_pcf8833.h"
#include"part_LCD_pcd8544.h"
#include"part_gamepad.h"
#include"part_LED_matrix.h"
#include"part_7s_Display.h"
#include"part_TempSys.h"
#include"part_keypad.h"
#include"part_MI2C_24CXXX.h"
#include"part_RTC_ds1307.h"
#include"part_RTC_pfc8563.h"
#include"part_IO_74xx595.h"
#include"part_VCD_Dump.h"
#include"part_VCD_Dump_an.h"
#include"part_IO_PCF8574.h"
#include"part_Buzzer.h"
#include"part_SignalGenerator.h" 
#include"part_push_buttons_an.h"
#include"part_IO_MCP23S17.h"

const char parts_list[NUM_PARTS][30]={"7 Segments Display","Buzzer","D. Transfer function","Gamepad","IO 74xx595","IO MCP23S17","IO PCF8574", "Keypad","LCD hd44780", "LCD pcf8833", "LCD pcd8544", "LED Matrix", "LEDs", "MEM 24CXXX", "Potentiometers", "Push buttons", "Push buttons (Analogic)", "RGB LED", "RTC ds1307", "RTC pfc8563", "Servo motor","Signal Generator","Step motor","Switchs","Temperature System","VCD Dump","VCD Dump (Analogic)",};


//boards object creation
part * create_part(String name, unsigned int x, unsigned int y)
{
   part * part_=NULL; 
 
   if(name.compare(lxT("Servo motor")) == 0 )part_= new cpart_servo(x,y);

   if(name.compare(lxT("Step motor")) == 0 )part_= new cpart_step(x,y);
  
   if(name.compare(lxT("D. Transfer function")) == 0 )part_= new cpart_dtfunc(x,y);

   if(name.compare(lxT("Push buttons")) == 0 )part_= new cpart_pbuttons(x,y);
   
   if(name.compare(lxT("Switchs")) == 0 )part_= new cpart_switchs(x,y);
   
   if(name.compare(lxT("LED Matrix")) == 0 )part_= new cpart_led_matrix(x,y);
   
   if(name.compare(lxT("LEDs")) == 0 )part_= new cpart_leds(x,y);
   
   if(name.compare(lxT("Potentiometers")) == 0 )part_= new cpart_pot(x,y);
   
   if(name.compare(lxT("RGB LED")) == 0 )part_= new cpart_rgb_led(x,y);
   
   if(name.compare(lxT("LCD hd44780")) == 0 )part_= new cpart_LCD_hd44780(x,y);
   
   if(name.compare(lxT("LCD pcf8833")) == 0 )part_= new cpart_LCD_pcf8833(x,y);
  
   if(name.compare(lxT("LCD pcd8544")) == 0 )part_= new cpart_LCD_pcd8544(x,y);
 
   if(name.compare(lxT("Gamepad")) == 0 )part_= new cpart_gamepad(x,y);
 
   if(name.compare(lxT("7 Segments Display")) == 0 )part_= new cpart_7s_display(x,y);
   
   if(name.compare(lxT("Temperature System")) == 0 )part_= new cpart_tempsys(x,y);

   if(name.compare(lxT("Keypad")) == 0 )part_= new cpart_keypad(x,y);
  
   if(name.compare(lxT("MEM 24CXXX")) == 0 )part_= new cpart_MI2C_24CXXX(x,y);
   
   if(name.compare(lxT("RTC ds1307")) == 0 )part_= new cpart_RTC_ds1307(x,y);
   
   if(name.compare(lxT("RTC pfc8563")) == 0 )part_= new cpart_RTC_pfc8563(x,y);

   if(name.compare(lxT("IO 74xx595")) == 0 )part_= new cpart_IO_74xx595(x,y);

   if(name.compare(lxT("VCD Dump")) == 0 )part_= new cpart_VCD_Dump(x,y);

   if(name.compare(lxT("VCD Dump (Analogic)")) == 0 )part_= new cpart_VCD_Dump_an(x,y);   

   if(name.compare(lxT("IO PCF8574")) == 0 )part_= new cpart_IO_PCF8574(x,y);
   
   if(name.compare(lxT("Buzzer")) == 0 )part_= new cpart_Buzzer(x,y);
   
   if(name.compare(lxT("Signal Generator")) == 0 )part_= new cpart_SignalGenerator(x,y);
   
   if(name.compare(lxT("Push buttons (Analogic)")) == 0 )part_= new cpart_pbuttons_an(x,y);
   
   if(name.compare(lxT("IO MCP23S17")) == 0 )part_= new cpart_IO_MCP23S17(x,y);
   
   return part_; 
}
