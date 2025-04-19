/* ########################################################################

   PICSimLab - Programmable IC Simulator Laboratory

   ########################################################################

   Copyright (c) : 2020-2025  Luis Claudio Gambôa Lopes <lcgamboa@yahoo.com>

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

// FIXME remove lxrad

#include "input_MPU6050.h"
#include "../lib/oscilloscope.h"
#include "../lib/picsimlab.h"
#include "../lib/spareparts.h"

/* inputs */
enum { I_VS1, I_VS2, I_VS3, I_VS4, I_VS5, I_VS6 };

/* outputs */
enum { O_P1, O_P2, O_P3, O_P4, O_P5, O_P6, O_P7, O_P8, O_VS1, O_VS2, O_VS3, O_VS4, O_VS5, O_VS6 };

const char pin_names[8][10] = {"VCC", "GND", "SCL", "SDA", "XDA", "XCL", "AD0", "INT"};
const char pin_values[8][10] = {"+5V", "GND", {0}, {1}, {2}, {3}, {4}, {5}};

static PCWProp pcwprop[9] = {{PCW_LABEL, "1 - VCC,+5V"}, {PCW_LABEL, "2 - GND ,GND"}, {PCW_COMBO, "3 - SCL"},
                             {PCW_COMBO, "4 - SDA"},     {PCW_COMBO, "5 - XDA"},      {PCW_COMBO, "6 - XCL"},
                             {PCW_COMBO, "7 - AD0"},     {PCW_COMBO, "8 - INT"},      {PCW_END, ""}};

cpart_MPU6050::cpart_MPU6050(const unsigned x, const unsigned y, const char* name, const char* type, board* pboard_,
                             const int id_)
    : part(x, y, name, type, pboard_, id_) {
    mpu6050_init(&mpu);
    mpu6050_rst(&mpu);

    mpu_pins[0] = 0;
    mpu_pins[1] = 0;
    mpu_pins[2] = 0;
    mpu_pins[3] = 0;
    mpu_pins[4] = 0;
    mpu_pins[5] = 0;

    active[0] = 0;
    active[1] = 0;
    active[2] = 0;
    active[3] = 0;
    active[4] = 0;
    active[5] = 0;

    asens = 16383 / 50.0;
    gsens = 131 * 2.5;

    mpu6050_set_temp(&mpu, 27.8);
    mpu6050_set_accel(&mpu, 0, 0, 1.0);
    mpu6050_set_gyro(&mpu, 0, 0, 0);

    SetPCWProperties(pcwprop);

    PinCount = 6;
    Pins = mpu_pins;
}

void cpart_MPU6050::RegisterRemoteControl(void) {
    input_ids[I_VS1]->status = &mpu.regs[ACCEL_XOUT_H];
    input_ids[I_VS1]->update = &output_ids[O_VS1]->update;

    input_ids[I_VS2]->status = &mpu.regs[ACCEL_YOUT_H];
    input_ids[I_VS2]->update = &output_ids[O_VS2]->update;

    input_ids[I_VS3]->status = &mpu.regs[ACCEL_ZOUT_H];
    input_ids[I_VS3]->update = &output_ids[O_VS3]->update;

    input_ids[I_VS4]->status = &mpu.regs[GYRO_XOUT_H];
    input_ids[I_VS4]->update = &output_ids[O_VS4]->update;

    input_ids[I_VS5]->status = &mpu.regs[GYRO_YOUT_H];
    input_ids[I_VS5]->update = &output_ids[O_VS5]->update;

    input_ids[I_VS6]->status = &mpu.regs[GYRO_ZOUT_H];
    input_ids[I_VS6]->update = &output_ids[O_VS6]->update;
}

cpart_MPU6050::~cpart_MPU6050(void) {
    mpu6050_end(&mpu);
    SpareParts.SetPartOnDraw(id);
    SpareParts.CanvasCmd({.cmd = CC_FREEBITMAP, .FreeBitmap{BitmapId}});
    SpareParts.CanvasCmd({.cmd = CC_DESTROY});
}

void cpart_MPU6050::DrawOutput(const unsigned int i) {
    char val[10];

    switch (output[i].id) {
        case O_VS1:
        case O_VS2:
        case O_VS3:
            switch ((mpu.regs[ACCEL_CONFIG] & 0x18) >> 3) {
                case 0:  // 2g
                    snprintf(val, 10, "%4.2f", 2 * ((getValues(output[i].id - O_VS1)) - 100) / 100.0);
                    break;
                case 1:  // 4g
                    snprintf(val, 10, "%4.2f", 4 * ((getValues(output[i].id - O_VS1)) - 100) / 100.0);
                    break;
                case 2:  // 8g
                    snprintf(val, 10, "%4.2f", 8 * ((getValues(output[i].id - O_VS1)) - 100) / 100.0);
                    break;
                case 3:  // 16g
                    snprintf(val, 10, "%4.2f", 16 * ((getValues(output[i].id - O_VS1)) - 100) / 100.0);
                    break;
            }
            DrawSlider(SpareParts.CanvasCmd, &output[i], 200 - getValues(output[i].id - O_VS1), val, 7);
            SpareParts.CanvasCmd({.cmd = CC_SETFONTSIZE, .SetFontSize{9}});
            break;
        case O_VS4:
        case O_VS5:
        case O_VS6:
            switch ((mpu.regs[GYRO_CONFIG] & 0x18) >> 3) {
                case 0:  // 250g/s
                    snprintf(val, 10, "%4.0f", 250 * ((getValues(output[i].id - O_VS1)) - 100) / 100.0);
                    break;
                case 1:  // 500g/s
                    snprintf(val, 10, "%4.0f", 500 * ((getValues(output[i].id - O_VS1)) - 100) / 100.0);
                    break;
                case 2:  // 1000g/s
                    snprintf(val, 10, "%4.0f", 1000 * ((getValues(output[i].id - O_VS1)) - 100) / 100.0);
                    break;
                case 3:  // 2000g/s
                    snprintf(val, 10, "%4.0f", 2000 * ((getValues(output[i].id - O_VS1)) - 100) / 100.0);
                    break;
            }
            DrawSlider(SpareParts.CanvasCmd, &output[i], 200 - getValues(output[i].id - O_VS1), val, 7);
            SpareParts.CanvasCmd({.cmd = CC_SETFONTSIZE, .SetFontSize{9}});
            break;
        default:
            SpareParts.CanvasCmd({.cmd = CC_SETCOLOR, .SetColor{49, 61, 99}});
            SpareParts.CanvasCmd(
                {.cmd = CC_RECTANGLE,
                 .Rectangle{1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1}});

            SpareParts.CanvasCmd({.cmd = CC_SETFGCOLOR, .SetFgColor{255, 255, 255}});
            SpareParts.CanvasCmd({.cmd = CC_ROTATEDTEXT,
                                  .RotatedText{pin_names[output[i].id - O_P1], output[i].x1, output[i].y2, 90.0}});

            int pinv = pin_values[output[i].id - O_P1][0];
            if (pinv > 10) {
                SpareParts.CanvasCmd({.cmd = CC_SETFGCOLOR, .SetFgColor{155, 155, 155}});
                SpareParts.CanvasCmd(
                    {.cmd = CC_ROTATEDTEXT,
                     .RotatedText{pin_values[output[i].id - O_P1], output[i].x1, output[i].y2 - 30, 90.0}});
            } else {
                if (mpu_pins[pinv] == 0)
                    SpareParts.CanvasCmd(
                        {.cmd = CC_ROTATEDTEXT, .RotatedText{"NC", output[i].x1, output[i].y2 - 30, 90.0}});
                else
                    SpareParts.CanvasCmd({.cmd = CC_ROTATEDTEXT,
                                          .RotatedText{SpareParts.GetPinName(mpu_pins[pinv]).c_str(), output[i].x1,
                                                       output[i].y2 - 30, 90.0}});
            }
            break;
    }
}

unsigned short cpart_MPU6050::GetInputId(char* name) {
    if (strcmp(name, "VS_1") == 0)
        return I_VS1;
    if (strcmp(name, "VS_2") == 0)
        return I_VS2;
    if (strcmp(name, "VS_3") == 0)
        return I_VS3;
    if (strcmp(name, "VS_4") == 0)
        return I_VS4;
    if (strcmp(name, "VS_5") == 0)
        return I_VS5;
    if (strcmp(name, "VS_6") == 0)
        return I_VS6;

    printf("Error input '%s' don't have a valid id! \n", name);
    return INVALID_ID;
}

unsigned short cpart_MPU6050::GetOutputId(char* name) {
    if (strcmp(name, "PN_1") == 0)
        return O_P1;
    if (strcmp(name, "PN_2") == 0)
        return O_P2;
    if (strcmp(name, "PN_3") == 0)
        return O_P3;
    if (strcmp(name, "PN_4") == 0)
        return O_P4;
    if (strcmp(name, "PN_5") == 0)
        return O_P5;
    if (strcmp(name, "PN_6") == 0)
        return O_P6;
    if (strcmp(name, "PN_7") == 0)
        return O_P7;
    if (strcmp(name, "PN_8") == 0)
        return O_P8;

    if (strcmp(name, "VS_1") == 0)
        return O_VS1;
    if (strcmp(name, "VS_2") == 0)
        return O_VS2;
    if (strcmp(name, "VS_3") == 0)
        return O_VS3;
    if (strcmp(name, "VS_4") == 0)
        return O_VS4;
    if (strcmp(name, "VS_5") == 0)
        return O_VS5;
    if (strcmp(name, "VS_6") == 0)
        return O_VS6;

    printf("Error output '%s' don't have a valid id! \n", name);
    return INVALID_ID;
}

std::string cpart_MPU6050::WritePreferences(void) {
    char prefs[256];

    sprintf(prefs, "%hhu,%hhu,%hhu,%hhu,%hhu,%hhu", mpu_pins[0], mpu_pins[1], mpu_pins[2], mpu_pins[3], mpu_pins[4],
            mpu_pins[5]);

    return prefs;
}

void cpart_MPU6050::ReadPreferences(std::string value) {
    sscanf(value.c_str(), "%hhu,%hhu,%hhu,%hhu,%hhu,%hhu", &mpu_pins[0], &mpu_pins[1], &mpu_pins[2], &mpu_pins[3],
           &mpu_pins[4], &mpu_pins[5]);

    Reset();
}

void cpart_MPU6050::ConfigurePropertiesWindow(void) {
    SetPCWComboWithPinNames("combo3", mpu_pins[0]);
    SetPCWComboWithPinNames("combo4", mpu_pins[1]);
    SetPCWComboWithPinNames("combo5", mpu_pins[2]);
    SetPCWComboWithPinNames("combo6", mpu_pins[3]);
    SetPCWComboWithPinNames("combo7", mpu_pins[4]);
    SetPCWComboWithPinNames("combo8", mpu_pins[5]);
}

void cpart_MPU6050::ReadPropertiesWindow(void) {
    mpu_pins[0] = GetPWCComboSelectedPin("combo3");
    mpu_pins[1] = GetPWCComboSelectedPin("combo4");
    mpu_pins[2] = GetPWCComboSelectedPin("combo5");
    mpu_pins[3] = GetPWCComboSelectedPin("combo6");
    mpu_pins[4] = GetPWCComboSelectedPin("combo7");
    mpu_pins[5] = GetPWCComboSelectedPin("combo8");
}

void cpart_MPU6050::PreProcess(void) {
    const picpin* ppins = SpareParts.GetPinsValues();
    unsigned char addr = 0x68;

    if (mpu_pins[4]) {
        if (ppins[mpu_pins[4] - 1].value)
            addr |= 0x01;
    }

    mpu6050_set_addr(&mpu, addr);
    /*
     float sens = 2 << ((mpu.regs[ACCEL_CONFIG]&0x18) >> 3);

     mpu6050_set_accel (&mpu, sens * ((values[0] - 100) / 100.0), sens * ((values[1] - 100) / 100.0), sens * ((values[2]
     - 100) / 100.0));

     sens = 250 << ((mpu.regs[ACCEL_CONFIG]&0x18) >> 3);

     mpu6050_set_gyro (&mpu, sens * ((values[3] - 100) / 100.0), sens * ((values[4] - 100) / 100.0), sens * ((values[5]
     - 100) / 100.0));
     */

    if (mpu_pins[1] > 0) {
        SpareParts.ResetPullupBus(mpu_pins[1] - 1);
    }
}

void cpart_MPU6050::Process(void) {
    const picpin* ppins = SpareParts.GetPinsValues();

    if ((mpu_pins[0] > 0) && (mpu_pins[1] > 0))
        SpareParts.SetPullupBus(mpu_pins[1] - 1,
                                mpu6050_io_I2C(&mpu, ppins[mpu_pins[0] - 1].value, ppins[mpu_pins[1] - 1].value));
}

void cpart_MPU6050::PostProcess(void) {
    if (mpu.update) {
        mpu.update = 0;
        for (int i = 0; i < outputc; i++) {
            output[i].update = 1;
        }
    }
}

void cpart_MPU6050::setMpuReg(unsigned char addr, unsigned char val) {
    unsigned char reg = 0;
    short sv = 0;

    switch (addr) {
        case 0:
            reg = ACCEL_XOUT_H;
            sv = (val - 100) * asens;
            break;
        case 1:
            reg = ACCEL_YOUT_H;
            sv = (val - 100) * asens;
            break;
        case 2:
            reg = ACCEL_ZOUT_H;
            sv = (val - 100) * asens;
            break;
        case 3:
            reg = GYRO_XOUT_H;
            sv = (val - 100) * gsens;
            break;
        case 4:
            reg = GYRO_YOUT_H;
            sv = (val - 100) * gsens;
            break;
        case 5:
            reg = GYRO_ZOUT_H;
            sv = (val - 100) * gsens;
            break;
    }

    mpu.regs[reg] = (sv & 0xFF00) >> 8;
    mpu.regs[reg + 1] = sv & 0x00FF;
}

unsigned char cpart_MPU6050::getValues(unsigned char addr) {
    short temp;
    unsigned char value = 0;

    switch (addr) {
        case 0:
            temp = ((mpu.regs[ACCEL_XOUT_H] << 8) + mpu.regs[ACCEL_XOUT_L]);
            value = (temp / asens) + 100;
            break;
        case 1:
            temp = ((mpu.regs[ACCEL_YOUT_H] << 8) + mpu.regs[ACCEL_YOUT_L]);
            value = (temp / asens) + 100;
            break;
        case 2:
            temp = ((mpu.regs[ACCEL_ZOUT_H] << 8) + mpu.regs[ACCEL_ZOUT_L]);
            value = (temp / asens) + 100;
            break;
        case 3:
            temp = ((mpu.regs[GYRO_XOUT_H] << 8) + mpu.regs[GYRO_XOUT_L]);
            value = (temp / gsens) + 100;
            break;
        case 4:
            temp = ((mpu.regs[GYRO_YOUT_H] << 8) + mpu.regs[GYRO_YOUT_L]);
            value = (temp / gsens) + 100;
            break;
        case 5:
            temp = ((mpu.regs[GYRO_ZOUT_H] << 8) + mpu.regs[GYRO_ZOUT_L]);
            value = (temp / gsens) + 100;
            break;
    }

    if (value > 200)
        value = 200;

    return value;
}

void cpart_MPU6050::OnMouseButtonPress(unsigned int inputId, unsigned int button, unsigned int x, unsigned int y,
                                       unsigned int state) {
    unsigned char value;

    switch (inputId) {
        case I_VS1:
            value = 200 - ((y - input_ids[I_VS1]->y1) * 1.66);
            if (value > 200)
                value = 0;
            setMpuReg(0, value);
            active[0] = 1;
            output_ids[O_VS1]->update = 1;
            break;
        case I_VS2:
            value = 200 - ((y - input_ids[I_VS2]->y1) * 1.66);
            if (value > 200)
                value = 0;
            setMpuReg(1, value);
            active[1] = 1;
            output_ids[O_VS2]->update = 1;
            break;
        case I_VS3:
            value = 200 - ((y - input_ids[I_VS3]->y1) * 1.66);
            if (value > 200)
                value = 0;
            setMpuReg(2, value);
            active[2] = 1;
            output_ids[O_VS3]->update = 1;
            break;
        case I_VS4:
            value = 200 - ((y - input_ids[I_VS4]->y1) * 1.66);
            if (value > 200)
                value = 0;
            setMpuReg(3, value);
            active[3] = 1;
            output_ids[O_VS4]->update = 1;
            break;
        case I_VS5:
            value = 200 - ((y - input_ids[I_VS5]->y1) * 1.66);
            if (value > 200)
                value = 0;
            setMpuReg(4, value);
            active[4] = 1;
            output_ids[O_VS5]->update = 1;
            break;
        case I_VS6:
            value = 200 - ((y - input_ids[I_VS6]->y1) * 1.66);
            if (value > 200)
                value = 0;
            setMpuReg(5, value);
            active[5] = 1;
            output_ids[O_VS6]->update = 1;
            break;
    }
}

void cpart_MPU6050::OnMouseButtonRelease(unsigned int inputId, unsigned int button, unsigned int x, unsigned int y,
                                         unsigned int state) {
    switch (inputId) {
        case I_VS1:
            active[0] = 0;
            output_ids[O_VS1]->update = 1;
            break;
        case I_VS2:
            active[1] = 0;
            output_ids[O_VS2]->update = 1;
            break;
        case I_VS3:
            active[2] = 0;
            output_ids[O_VS3]->update = 1;
            break;
        case I_VS4:
            active[3] = 0;
            output_ids[O_VS4]->update = 1;
            break;
        case I_VS5:
            active[4] = 0;
            output_ids[O_VS5]->update = 1;
            break;
        case I_VS6:
            active[5] = 0;
            output_ids[O_VS6]->update = 1;
            break;
    }
}

void cpart_MPU6050::OnMouseMove(unsigned int inputId, unsigned int button, unsigned int x, unsigned int y,
                                unsigned int state) {
    unsigned char value;

    switch (inputId) {
        case I_VS1 ... I_VS6:
            if (active[inputId - I_VS1]) {
                value = 200 - ((y - input_ids[inputId]->y1) * 1.66);
                if (value > 200)
                    value = 0;

                output_ids[O_VS1 + inputId - I_VS1]->update = 1;
                setMpuReg(inputId - I_VS1, value);
            }
            break;
        default:
            active[0] = 0;
            active[1] = 0;
            active[2] = 0;
            active[3] = 0;
            active[4] = 0;
            active[5] = 0;
            break;
    }
}

part_init(PART_MPU6050_Name, cpart_MPU6050, "Input");
