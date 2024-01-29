/* ########################################################################

   PICSimLab - Programmable IC Simulator Laboratory

   ########################################################################

   Copyright (c) : 2020-2024  Luis Claudio Gambôa Lopes <lcgamboa@yahoo.com>

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

#include "../devices/vterm.h"
#include "../lib/part.h"

#define PART_vterm_Name "IO Virtual Term"

class cpart_vterm : public part {
public:
    std::string GetAboutInfo(void) override { return "L.C. Gamboa \n <lcgamboa@yahoo.com>"; };
    cpart_vterm(const unsigned x, const unsigned y, const char* name, const char* type, board* pboard_, const int id_);
    ~cpart_vterm(void);
    void DrawOutput(const unsigned int index) override;
    void PreProcess(void) override;
    void Process(void) override;
    void PostProcess(void) override;
    void Reset(void) override;
    void OnMouseButtonPress(uint inputId, uint button, uint x, uint y, uint state) override;
    void ConfigurePropertiesWindow(CPWindow* WProp) override;
    void ReadPropertiesWindow(CPWindow* WProp) override;
    void ButtonEvent(CControl* control, uint button, uint x, uint y, uint state) override;
    void KeyEvent(CControl* control, uint keysym, uint ukeysym, uint state) override;
    void Event(CControl* control) override;
    std::string WritePreferences(void) override;
    void ReadPreferences(std::string value) override;
    unsigned short GetInputId(char* name) override;
    unsigned short GetOutputId(char* name) override;

    static int count;

private:
    void RegisterRemoteControl(void) override;
    unsigned char pins[2];
    vterm_t vt;
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
    std::string text_to_send;
    unsigned char show;
};

#endif /* PART_vterm_H */
