/* ########################################################################

   PICsimLab - PIC laboratory simulator

   ########################################################################

   Copyright (c) : 2020-2022  Luis Claudio Gambôa Lopes

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

#define	PART_ETH_W5500_Name "ETH w5500"

class cpart_ETH_w5500:public part
{
   public:
      lxString GetName(void) override {return lxT(PART_ETH_W5500_Name);};
      lxString GetAboutInfo(void) override {return lxT("L.C. Gamboa \n <lcgamboa@yahoo.com>");};
      cpart_ETH_w5500(unsigned x, unsigned y);
      ~cpart_ETH_w5500(void);
      void Draw(void) override;
      void PreProcess (void) override;
      void Process(void) override; 
      void PostProcess (void) override;
      void Reset(void) override;
      void EvMouseButtonPress(uint button, uint x, uint y,uint state) override;
      void ConfigurePropertiesWindow(CPWindow *  WProp) override;
      void ReadPropertiesWindow(CPWindow * WProp) override;
      lxString WritePreferences(void) override;
      void ReadPreferences(lxString value) override;
      unsigned short get_in_id(char * name) override;
      unsigned short get_out_id(char * name) override;
   private:
      void RegisterRemoteControl(void) override;         
      unsigned char input_pins[4];
      unsigned char output_pins[2];
      eth_w5500_t ethw;
      unsigned char link;
      unsigned short _ret; 
      lxFont font;
      unsigned int sts[8][4];
};




#endif	/* PART_ETH_W5500_H */

