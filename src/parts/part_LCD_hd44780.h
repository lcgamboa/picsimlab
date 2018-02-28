/* ########################################################################

   PICsimLab - PIC laboratory simulator

   ########################################################################

   Copyright (c) : 2010-2015  Luis Claudio Gambôa Lopes

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

#ifndef PART_LCD_HD44780_H
#define	PART_LCD_HD44780_H

#include<lxrad/lxrad.h>
#include"part.h"


class cpart_LCD_hd44780:public part
{
   public:
      String GetName(void){return wxT("LCD hd44780");};
      cpart_LCD_hd44780(unsigned x, unsigned y);
      ~cpart_LCD_hd44780(void);
      void Draw(void);
      void Process(void);
      String GetPictureFileName(void){return wxT("LCD_hd44780.png");};
      String GetPictureFileName_(void){return wxT("LCD_hd44780_.png");};
      String GetInputMapFile(void){return wxT("LCD_hd44780_i.map");};
      String GetOutputMapFile(void){return wxT("LCD_hd44780_o.map");};
      String GetPropertiesWindowFile(void){return wxT("LCD_hd44780.lxrad");};
      void ConfigurePropertiesWindow(CPWindow *  wprop);
      void ReadPropertiesWindow(void);
      String WritePreferences(void);
      void ReadPreferences(String value);
      unsigned short get_in_id(char * name);
      unsigned short get_out_id(char * name);
      void PropButton (CControl * control, uint button, uint x, uint y, uint state);
    private:
      void Reset(void);
      unsigned char input_pins[10]; 
      lcd_t lcd;
      int lcde;
      unsigned char model;
};




#endif	/* PART_LCD_HD44780 */

