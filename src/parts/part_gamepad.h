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

#ifndef PART_GAMEPAD_H
#define	PART_GAMEPAD_H

#include<lxrad.h>
#include"part.h"

#define	PART_GAMEPAD_Name "Gamepad"

class cpart_gamepad:public part
{
   public:
      lxString GetName(void) override {return lxT(PART_GAMEPAD_Name);};
      lxString GetAboutInfo(void) override {return lxT("L.C. Gamboa \n <lcgamboa@yahoo.com>");};            
      cpart_gamepad(unsigned x, unsigned y);
      ~cpart_gamepad(void);
      void Draw(void) override;
      void PreProcess(void) override;
      void EvMouseButtonPress(uint button, uint x, uint y,uint state) override;
      void EvMouseButtonRelease(uint button, uint x, uint y,uint state) override;
      void EvMouseMove(uint button, uint x, uint y,uint state) override;
      void EvKeyPress(uint key,uint mask) override;
      void EvKeyRelease(uint key,uint mask) override;
      void ConfigurePropertiesWindow(CPWindow *  WProp) override;
      void ReadPropertiesWindow(CPWindow * WProp) override;
      lxString WritePreferences(void) override;
      void ReadPreferences(lxString value) override;
      unsigned short get_in_id(char * name) override;
      unsigned short get_out_id(char * name) override;
    private:
      void RegisterRemoteControl(void) override;          
      unsigned char output_pins[8];    
      unsigned char output_value[6];
      unsigned char value[2];
      unsigned char active;
      unsigned int jr;
      lxFont font;
};

#endif	/* PART_GAMEPAD_H */

