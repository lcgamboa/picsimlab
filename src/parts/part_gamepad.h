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


class cpart_gamepad:public part
{
   public:
      lxString GetName(void){return lxT("Gamepad");};
      lxString GetHelpURL(void){return lxT("Gamepad.html");};
      lxString GetAboutInfo(void){return lxT("L.C. Gamboa \n <lcgamboa@yahoo.com>");};            
      cpart_gamepad(unsigned x, unsigned y);
      ~cpart_gamepad(void);
      void Draw(void);
      void PreProcess(void);
      lxString GetPictureFileName(void){return lxT("gamepad/gamepad.svg");};
      lxString GetInputMapFile(void){return lxT("gamepad/gamepad_i.map");};
      lxString GetOutputMapFile(void){return lxT("gamepad/gamepad_o.map");};
      lxString GetPropertiesWindowFile(void){return lxT("gamepad/gamepad.lxrad");};
      void EvMouseButtonPress(uint button, uint x, uint y,uint state);
      void EvMouseButtonRelease(uint button, uint x, uint y,uint state);
      void EvMouseMove(uint button, uint x, uint y,uint state);
      void EvKeyPress(uint key,uint mask);
      void EvKeyRelease(uint key,uint mask);
      void ConfigurePropertiesWindow(CPWindow *  WProp);
      void ReadPropertiesWindow(CPWindow * WProp);
      lxString WritePreferences(void);
      void ReadPreferences(lxString value);
      unsigned short get_in_id(char * name);
      unsigned short get_out_id(char * name);
    private:
      void RegisterRemoteControl(void);          
      unsigned char output_pins[8];    
      unsigned char output_value[6];
      unsigned char value[2];
      unsigned char active;
      unsigned int jr;
      lxFont font;
};


#endif	/* PART_GAMEPAD_H */

