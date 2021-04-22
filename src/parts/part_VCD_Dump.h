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

#ifndef PART_VCD_DUMP_H
#define	PART_VCD_DUMP_H

#include<lxrad.h>
#include"part.h"

class cpart_VCD_Dump:public part
{
   public:
      lxString GetName(void){return lxT("VCD Dump");};
      lxString GetHelpURL(void){return lxT("VCD_dump.html");};
      lxString GetAboutInfo(void){return lxT("L.C. Gamboa \n <lcgamboa@yahoo.com>");};            
      cpart_VCD_Dump(unsigned x, unsigned y);
      ~cpart_VCD_Dump(void);
      void Draw(void);
      void Process(void);
      lxString GetPictureFileName(void){return lxT("VCD_Dump/VCD_Dump.svg");};
      lxString GetInputMapFile(void){return lxT("VCD_Dump/VCD_Dump_i.map");};
      lxString GetOutputMapFile(void){return lxT("VCD_Dump/VCD_Dump_o.map");};
      lxString GetPropertiesWindowFile(void){return lxT("VCD_Dump/VCD_Dump.lxrad");};
      void EvMouseButtonPress(uint button, uint x, uint y,uint state);
      void ConfigurePropertiesWindow(CPWindow *  WProp);
      void ReadPropertiesWindow(CPWindow * WProp);
      lxString WritePreferences(void);
      void ReadPreferences(lxString value);
      unsigned short get_in_id(char * name);
      unsigned short get_out_id(char * name);
     private:
      void RegisterRemoteControl(void);           
      unsigned char input_pins[8];    
      unsigned char old_value_pins[8];   
      char   f_vcd_name[200];
      FILE * f_vcd;
      unsigned long vcd_count;
      unsigned char rec;
      lxFont font;
      lxColor color1;
      lxColor color2;         
};




#endif	/* PART_VCD_DUMP_H */

