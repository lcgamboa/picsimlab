/* ########################################################################

   PICsimLab - PIC laboratory simulator

   ########################################################################

   Copyright (c) : 2010-2022  Luis Claudio Gambôa Lopes

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

#ifndef PART_DHT22_H
#define PART_DHT22_H

#include <lxrad.h>
#include "../devices/sen_ds18b20.h"
#include "part.h"

#define PART_DHT22_Name "DS18B20 (Temperature)"

class cpart_ds18b20 : public part {
public:
    lxString GetAboutInfo(void) override { return lxT("L.C. Gamboa \n <lcgamboa@yahoo.com>"); };
    cpart_ds18b20(const unsigned x, const unsigned y, const char* name, const char* type);
    ~cpart_ds18b20(void);
    void DrawOutput(const unsigned int index) override;
    void Reset(void) override;
    void PreProcess(void) override;
    void Process(void) override;
    void OnMouseButtonPress(uint inputId, uint button, uint x, uint y, uint state) override;
    void OnMouseButtonRelease(uint inputId, uint button, uint x, uint y, uint state) override;
    void OnMouseMove(uint inputId, uint button, uint x, uint y, uint state) override;
    void ConfigurePropertiesWindow(CPWindow* WProp) override;
    void ReadPropertiesWindow(CPWindow* WProp) override;
    lxString WritePreferences(void) override;
    void ReadPreferences(lxString value) override;
    unsigned short GetInputId(char* name) override;
    unsigned short GetOutputId(char* name) override;

private:
    sen_ds18b20_t ds18b20;
    void RegisterRemoteControl(void) override;
    unsigned char output_pins[1];
    unsigned char values[1];
    unsigned char active[1];
    lxFont font;
    lxFont font_p;
};

#endif /* DHT22 */
