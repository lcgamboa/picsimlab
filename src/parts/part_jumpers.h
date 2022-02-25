/* ########################################################################

   PICsimLab - PIC laboratory simulator

   ########################################################################

   Copyright (c) : 2019-2022  Luis Claudio Gambôa Lopes

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

#define PART_JUMPERS_Name "Jumper Wires"

class cpart_Jumpers : public part {
    int ID;
    static int countID;
public:
    
    lxString GetName(void) override {
        return lxT(PART_JUMPERS_Name);
    };

    lxString GetAboutInfo(void) override {
        return lxT("L.C. Gamboa \n <lcgamboa@yahoo.com>");
    };
    cpart_Jumpers(unsigned x, unsigned y);
    ~cpart_Jumpers(void);
    void Draw(void) override;
    void PreProcess(void) override;
    void Process(void) override;
    void PostProcess(void) override;
    void ConfigurePropertiesWindow(CPWindow * WProp) override;
    void ReadPropertiesWindow(CPWindow * WProp) override;
    lxString WritePreferences(void) override;
    void ReadPreferences(lxString value) override;
    unsigned short get_in_id(char * name) override;
    unsigned short get_out_id(char * name) override;
    void ComboChange(CCombo * control, lxString value) override; 
private:
    void ChangeType(unsigned char type);
    void RegisterRemoteControl(void) override;      
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

