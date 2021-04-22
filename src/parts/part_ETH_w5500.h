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

#ifndef PART_ETH_W5500_H
#define	PART_ETH_W5500_H

#include<lxrad.h>
#include"part.h"
#include"../devices/eth_w5500.h"

class cpart_ETH_w5500:public part
{
   public:
      lxString GetName(void){return lxT("ETH w5500");};
      lxString GetHelpURL(void){return lxT("ETH_w5500.html");};
      lxString GetAboutInfo(void){return lxT("L.C. Gamboa \n <lcgamboa@yahoo.com>");};
      cpart_ETH_w5500(unsigned x, unsigned y);
      ~cpart_ETH_w5500(void);
      void Draw(void);
      void PreProcess (void);
      void Process(void); 
      void PostProcess (void);
      lxString GetPictureFileName(void){return lxT("ETH_w5500/w5500.svg");};
      lxString GetInputMapFile(void){return lxT("ETH_w5500/w5500_i.map");};
      lxString GetOutputMapFile(void){return lxT("ETH_w5500/w5500_o.map");};
      lxString GetPropertiesWindowFile(void){return lxT("ETH_w5500/w5500.lxrad");};
      void Reset(void);
      void EvMouseButtonPress(uint button, uint x, uint y,uint state);
      void ConfigurePropertiesWindow(CPWindow *  WProp);
      void ReadPropertiesWindow(CPWindow * WProp);
      lxString WritePreferences(void);
      void ReadPreferences(lxString value);
      unsigned short get_in_id(char * name);
      unsigned short get_out_id(char * name);
   private:
      void RegisterRemoteControl(void);         
      unsigned char input_pins[4];
      unsigned char output_pins[2];
      eth_w5500_t ethw;
      unsigned char link;
      unsigned short _ret; 
      lxFont font;
};




#endif	/* PART_ETH_W5500_H */

