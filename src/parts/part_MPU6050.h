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

#ifndef PART_MPU6050_H
#define PART_MPU6050_H

#include <lxrad.h>
#include "part.h"

#include "../devices/sen_mpu6050.h"

#define PART_MPU6050_Name "MPU6050"

class cpart_MPU6050 : public part {
public:
    lxString GetName(void) override { return lxT(PART_MPU6050_Name); };
    lxString GetAboutInfo(void) override { return lxT("L.C. Gamboa \n <lcgamboa@yahoo.com>"); };
    cpart_MPU6050(unsigned x, unsigned y);
    ~cpart_MPU6050(void);
    void Draw(void) override;
    void PreProcess(void) override;
    void Process(void) override;
    void PostProcess(void) override;
    void EvMouseButtonPress(uint button, uint x, uint y, uint state) override;
    void EvMouseButtonRelease(uint button, uint x, uint y, uint state) override;
    void EvMouseMove(uint button, uint x, uint y, uint state) override;
    void ConfigurePropertiesWindow(CPWindow* WProp) override;
    void ReadPropertiesWindow(CPWindow* WProp) override;
    lxString WritePreferences(void) override;
    void ReadPreferences(lxString value) override;
    unsigned short get_in_id(char* name) override;
    unsigned short get_out_id(char* name) override;
    void setMpuReg(unsigned char addr, unsigned char val);
    unsigned char getValues(unsigned char addr);

private:
    void RegisterRemoteControl(void) override;
    unsigned char mpu_pins[6];
    mpu6050_t mpu;
    lxFont font;
    lxFont font_p;
    unsigned char active[6];
    double asens;
    double gsens;
};

#endif /* PART_MPU6050_H */
