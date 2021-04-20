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

#ifndef PART_DTRANSFERF_H
#define	PART_DTRANSFERF_H

#include<lxrad.h>
#include"part.h"


class cpart_dtfunc:public part
{
   public:
      lxString GetName(void){return lxT("D. Transfer function");};
      lxString GetHelpURL(void){return lxT("D_Transfer_Function.html");};
      lxString GetAboutInfo(void){return lxT("L.C. Gamboa \n <lcgamboa@yahoo.com>");};            
      cpart_dtfunc(unsigned x, unsigned y);
      ~cpart_dtfunc(void);
      void Draw(void);
      void Process(void);
      lxString GetPictureFileName(void){return lxT("dtfunc/d_transfer_function.svg");};
      lxString GetInputMapFile(void){return lxT("dtfunc/d_transfer_function_i.map");};
      lxString GetOutputMapFile(void){return lxT("dtfunc/d_transfer_function_o.map");};
      lxString GetPropertiesWindowFile(void){return lxT("dtfunc/d_transfer_function.lxrad");};
      void Reset(void);
      void EvMouseButtonPress(uint button, uint x, uint y,uint state);
      void EvMouseButtonRelease(uint button, uint x, uint y,uint state);
      void EvKeyPress(uint key, uint mask);
      void EvKeyRelease(uint key, uint mask);
      void ConfigurePropertiesWindow(CPWindow *  WProp);
      void ReadPropertiesWindow(CPWindow * WProp);
      lxString WritePreferences(void);
      void ReadPreferences(lxString value);
      unsigned short get_in_id(char * name);
      unsigned short get_out_id(char * name);
   private:
      unsigned char input_pin;
      unsigned char output_pin;
      int ordern;
      int orderd;
      float num[4];
      float den[4];
      float v[4];
      float sample;
      float in_gain;
      float in_off;
      float out_gain;
      float out_off;
      long unsigned int nsamples;
      lxFont font;
};




#endif	/* PART_DTRANSFERF_H */

