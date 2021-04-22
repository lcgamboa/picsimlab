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

#ifndef PART_JUMPERS_H
#define PART_JUMPERS_H

#include<lxrad.h>
#include"part.h"

class cpart_Jumpers : public part {
    int ID;
    static int countID;
public:
    
    lxString GetName(void) {
        return lxT("Jumper Wires");
    };

    lxString GetHelpURL(void) {
        return lxT("Jumper_Wires.html");
    };

    lxString GetAboutInfo(void) {
        return lxT("L.C. Gamboa \n <lcgamboa@yahoo.com>");
    };
    cpart_Jumpers(unsigned x, unsigned y);
    ~cpart_Jumpers(void);
    void Draw(void);
    void PreProcess(void);
    void Process(void);
    void PostProcess(void);

    lxString GetPictureFileName(void) {
        return lxT("jumpers/jumper.svg");
    };

    lxString GetInputMapFile(void) {
        return lxT("jumpers/jumper_i.map");
    };

    lxString GetOutputMapFile(void) {
        return lxT("jumpers/jumper_o.map");
    };

    lxString GetPropertiesWindowFile(void) {
        return lxT("jumpers/jumper.lxrad");
    };
    void ConfigurePropertiesWindow(CPWindow * WProp);
    void ReadPropertiesWindow(CPWindow * WProp);
    lxString WritePreferences(void);
    void ReadPreferences(lxString value);
    unsigned short get_in_id(char * name);
    unsigned short get_out_id(char * name);
    void ComboChange(CCombo * control, lxString value);
    void ChangeType(unsigned char type); 
private:
    void RegisterRemoteControl(void);      
    unsigned char input_pins[16];
    unsigned char output_pins[16];
    unsigned long output_pins_alm[16];
    long mcount;
    int JUMPSTEPS_;
    lxString jname;
    unsigned char type; 
    lxFont font;
};

// input/output   0-M 1-F
#define JWT_MM 0x00
#define JWT_MF 0x01
#define JWT_FM 0x02
#define JWT_FF 0x03



#endif /* PART_IJUMPERS_H */

