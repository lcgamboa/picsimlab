/* ########################################################################

   PICsimLab - PIC laboratory simulator

   ########################################################################

   Copyright (c) : 2020-2021  Luis Claudio Gambôa Lopes

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

#ifndef PART_LCD_iLI9341_H
#define	PART_LCD_iLI9341_H

#include<lxrad.h>
#include"part.h"
#include"../devices/lcd_ili9341.h"
#include"../devices/tsc_xpt2046.h"

class cpart_LCD_ili9341:public part
{
   public:
      lxString GetName(void){return lxT("LCD ili9341");};
      lxString GetHelpURL(void){return lxT("LCD_ili9341.html");};
      lxString GetAboutInfo(void){return lxT("L.C. Gamboa \n <lcgamboa@yahoo.com>");};            
      cpart_LCD_ili9341(unsigned x, unsigned y);
      ~cpart_LCD_ili9341(void);
      void Draw(void);
      void Process(void);
      lxString GetPictureFileName(void);
      lxString GetInputMapFile(void);
      lxString GetOutputMapFile(void);
      lxString GetPropertiesWindowFile(void){return lxT("LCD_ili9341/LCD_ili9341.lxrad");};
      void EvMouseButtonPress(uint button, uint x, uint y,uint state);
      void EvMouseButtonRelease(uint button, uint x, uint y,uint state);
      void EvMouseMove(uint button, uint x, uint y,uint state);
      void Reset(void);
      void ConfigurePropertiesWindow(CPWindow *  WProp);
      void ReadPropertiesWindow(CPWindow * WProp);
      lxString WritePreferences(void);
      void SetOrientation(int orientation);
      void SetScale(double scale);
      void ReadPreferences(lxString value);
      unsigned short get_in_id(char * name);
      unsigned short get_out_id(char * name);
      void ComboChange(CCombo * control, lxString value);
      void ChangeType(unsigned char tp); 
   private:
      unsigned char input_pins[13]; 
      unsigned char touch_pins[5]; 
      lcd_ili9341_t lcd;
      tsc_XPT2046_t  touch;
      unsigned char type_com;
      unsigned char active[1];
      lxFont font;
};



#endif	/* PART_LCD_iLI9341_H */

