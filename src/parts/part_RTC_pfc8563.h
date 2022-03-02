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

#ifndef PART_RTC_PFC8563_H
#define PART_RTC_PFC8563_H

#include <lxrad.h>
#include "part.h"

#define PART_RTC_PFC8563_Name "RTC pfc8563"

class cpart_RTC_pfc8563 : public part {
public:
    lxString GetName(void) override { return lxT(PART_RTC_PFC8563_Name); };
    lxString GetAboutInfo(void) override { return lxT("L.C. Gamboa \n <lcgamboa@yahoo.com>"); };
    cpart_RTC_pfc8563(unsigned x, unsigned y);
    ~cpart_RTC_pfc8563(void);
    void Draw(void) override;
    void PreProcess(void) override;
    void Process(void) override;
    lxString GetPictureFileName(void) override { return lxT("Common/IC8.svg"); };
    lxString GetMapFile(void) override { return lxT("Common/IC8.map"); };
    void ConfigurePropertiesWindow(CPWindow* WProp) override;
    void ReadPropertiesWindow(CPWindow* WProp) override;
    lxString WritePreferences(void) override;
    void ReadPreferences(lxString value) override;
    unsigned short get_in_id(char* name) override;
    unsigned short get_out_id(char* name) override;

private:
    unsigned char input_pins[4];
    rtc_pfc8563_t rtc;
    lxFont font;
    lxFont font_p;
};

#endif /* PART_RTC_PFC8563_H */
