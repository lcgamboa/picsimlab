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

#ifndef PART_DCMOTOR_H
#define	PART_DCMOTOR_H

#include<lxrad.h>
#include"part.h"

#define	PART_DCMOTOR_Name "DC Motor"

class cpart_dcmotor:public part
{
    public:
      lxString GetName(void){return lxT(PART_DCMOTOR_Name);};
      lxString GetAboutInfo(void){return lxT("L.C. Gamboa \n <lcgamboa@yahoo.com>");};            
      cpart_dcmotor(unsigned x, unsigned y);
      ~cpart_dcmotor(void); 
      void Draw(void);
      void PreProcess(void);
      void Process(void);
      void PostProcess(void);
      void ConfigurePropertiesWindow(CPWindow *  WProp);
      void ReadPropertiesWindow(CPWindow * WProp);
      lxString WritePreferences(void);
      void ReadPreferences(lxString value);
      unsigned short get_in_id(char * name);
      unsigned short get_out_id(char * name);
    private:
      void RegisterRemoteControl(void);      
      unsigned char input_pins[3];
      unsigned char output_pins[2];
      unsigned char value;
      unsigned char value_old;
      int step;
      int count;
      unsigned char state;
      unsigned char dir;
      unsigned char speed;
      unsigned char * status[3];
      lxFont font;
};




#endif	/* DCMOTOR */

