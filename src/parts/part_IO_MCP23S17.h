/* ########################################################################

   PICsimLab - PIC laboratory simulator

   ########################################################################

   Copyright (c) : 2019-2021  Luis Claudio Gambôa Lopes

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

#ifndef PART_IO_MCP23S17_H
#define	PART_IO_MCP23S17_H

#include<lxrad.h>
#include"part.h"
#include"../devices/io_MCP23X17.h"

#define	PART_IO_MCP23S17_Name "IO MCP23S17"

class cpart_IO_MCP23S17:public part
{
   public:
      lxString GetName(void) override {return lxT(PART_IO_MCP23S17_Name);};
      lxString GetAboutInfo(void) override {return lxT("L.C. Gamboa \n <lcgamboa@yahoo.com>");};
      cpart_IO_MCP23S17(unsigned x, unsigned y);
      ~cpart_IO_MCP23S17(void);
      void Draw(void) override;
      void PreProcess (void) override;
      void Process(void) override; 
      void PostProcess (void) override;
      lxString GetPictureFileName(void) override{return lxT("Common/IC28.svg");};
      lxString GetMapFile(void) override{return lxT("Common/IC28.map");};
      void ConfigurePropertiesWindow(CPWindow *  WProp) override;
      void ReadPropertiesWindow(CPWindow * WProp) override;
      lxString WritePreferences(void) override;
      void ReadPreferences(lxString value) override;
      unsigned short get_in_id(char * name) override;
      unsigned short get_out_id(char * name) override;
   private:
      unsigned char input_pins[10];
      unsigned char output_pins[16];
      unsigned long output_pins_alm[16];
      long mcount;
      int JUMPSTEPS_;
      io_MCP23X17_t mcp;
      unsigned char _PA;
      unsigned char _PB;
      lxFont font;
};

#endif	/* PART_IO_MCP23S17_H */

