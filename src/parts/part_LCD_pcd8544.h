/* ########################################################################

   PICsimLab - PIC laboratory simulator

   ########################################################################

   Copyright (c) : 2010-2021  Luis Claudio Gambôa Lopes

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

#ifndef PART_LCD_PCD8544_H
#define	PART_LCD_PCD8544_H

#include<lxrad.h>
#include"part.h"
#include"../devices/lcd_pcd8544.h"

#define	PART_LCD_PCD8544_Name "LCD pcd8544"

class cpart_LCD_pcd8544:public part
{
   public:
      lxString GetName(void) override {return lxT(PART_LCD_PCD8544_Name);};
      lxString GetAboutInfo(void) override {return lxT("L.C. Gamboa \n <lcgamboa@yahoo.com>");};            
      cpart_LCD_pcd8544(unsigned x, unsigned y);
      ~cpart_LCD_pcd8544(void);
      void Draw(void) override;
      void Process(void) override;
      void PostProcess(void) override;
      void ConfigurePropertiesWindow(CPWindow *  WProp) override;
      void ReadPropertiesWindow(CPWindow * WProp) override;
      lxString WritePreferences(void) override;
      void SetOrientation(int orientation) override;
      void SetScale(double scale) override;      
      void ReadPreferences(lxString value) override;
      unsigned short get_in_id(char * name) override;
      unsigned short get_out_id(char * name) override;
   private:
      unsigned char input_pins[5]; 
      lcd_pcd8544_t lcd;
      lxFont font;
};

#endif	/* PART_LCD_PCD8544_H */

