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

#ifndef PART_vterm_H
#define PART_vterm_H

#include <lxrad.h>
#include "../devices/vterm.h"
#include "part.h"

#define PART_vterm_Name "IO Virtual Term"

class cpart_vterm : public part {
public:
    lxString GetName(void) override { return lxT(PART_vterm_Name); };
    lxString GetAboutInfo(void) override { return lxT("L.C. Gamboa \n <lcgamboa@yahoo.com>"); };
    cpart_vterm(unsigned x, unsigned y);
    ~cpart_vterm(void);
    void Draw(void) override;
    void PreProcess(void) override;
    void Process(void) override;
    void PostProcess(void) override;
    void Reset(void) override;
    void EvMouseButtonPress(uint button, uint x, uint y, uint state) override;
    void ConfigurePropertiesWindow(CPWindow* WProp) override;
    void ReadPropertiesWindow(CPWindow* WProp) override;
    void ButtonEvent(CControl* control, uint button, uint x, uint y, uint state) override;
    void KeyEvent(CControl* control, uint keysym, uint ukeysym, uint state) override;
    void Event(CControl* control) override;
    lxString WritePreferences(void) override;
    void ReadPreferences(lxString value) override;
    unsigned short get_in_id(char* name) override;
    unsigned short get_out_id(char* name) override;
    void SetId(int _id) override;

private:
    unsigned char input_pins[1];
    unsigned char output_pins[1];
    vterm_t vt;
    int _ret;
    unsigned int vterm_speed;
    CPWindow* wvterm;
    CText* vttext;
    CEdit* vtedit;
    CButton* vtbtn_send;
    CButton* vtbtn_clear;
    CCombo* vtcmb_speed;
    CCombo* vtcmb_ending;
    char send_text;
    unsigned char lending;
    lxFont font;
};

#endif /* PART_vterm_H */
