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

#ifndef PART_7S_DISPLAY_H
#define PART_7S_DISPLAY_H

#include<lxrad.h>
#include"part.h"

class cpart_7s_display : public part {
public:

    lxString GetName(void) {return lxT("7 Segments Display"); };
    lxString GetHelpURL(void){return lxT("Segments_Display.html");};
    lxString GetAboutInfo(void){return lxT("L.C. Gamboa \n <lcgamboa@yahoo.com>");};
    cpart_7s_display(unsigned x, unsigned y);
    ~cpart_7s_display(void);
    void Draw(void);
    void PreProcess (void);
    void Process(void); 
    void PostProcess (void);
    lxString GetPictureFileName(void) {return lxT("7s_display/7sdisplay.svg"); };
    lxString GetInputMapFile(void) {return lxT("7s_display/7sdisplay_i.map"); };
    lxString GetOutputMapFile(void) {return lxT("7s_display/7sdisplay_o.map");};
    lxString GetPropertiesWindowFile(void) {return lxT("7s_display/7sdisplay.lxrad");};
    void ConfigurePropertiesWindow(CPWindow * wprop);
    void ReadPropertiesWindow(CPWindow * WProp);
    lxString WritePreferences(void);
    void ReadPreferences(lxString value);
    unsigned short get_in_id(char * name);
    unsigned short get_out_id(char * name);
private:
    void RegisterRemoteControl(void);     
    unsigned char active;     
    unsigned char input_pins[12];
    unsigned int lm1[8]; //luminosidade media display
    unsigned int lm2[8]; //luminosidade media display
    unsigned int lm3[8]; //luminosidade media display
    unsigned int lm4[8]; //luminosidade media display

    unsigned int alm1[8];
    unsigned int alm2[8];
    unsigned int alm3[8];
    unsigned int alm4[8];
    long int mcount;
    int JUMPSTEPS_;
    lxFont font;
};




#endif /* PART_7S_DISPLAY */

