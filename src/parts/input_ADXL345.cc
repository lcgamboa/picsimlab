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

#include "input_ADXL345.h"
#include "../lib/oscilloscope.h"
#include "../lib/picsimlab.h"
#include "../lib/spareparts.h"

/* inputs */
enum { I_VS1, I_VS2, I_VS3 };

/* outputs */
enum { O_P1, O_P2, O_P3, O_P4, O_P5, O_P6, O_P7, O_P8, O_VS1, O_VS2, O_VS3 };

const char pin_names[8][10] = {"GND", "VCC", "CS", "INT1", "INT2", "SDO", "SDA", "SCL"};
const char pin_values[8][10] = {"GND", "+5V", {0}, {1}, {2}, {3}, {4}, {5}};

static PCWProp pcwprop[9] = {{PCW_LABEL, "1 - GND ,GND"}, {PCW_LABEL, "2 - VCC,+5V"}, {PCW_COMBO, "3 - CS"},
                             {PCW_COMBO, "4 - INT1"},     {PCW_COMBO, "5 - INT2"},    {PCW_COMBO, "6 - SDO"},
                             {PCW_COMBO, "7 - SDA"},      {PCW_COMBO, "8 - SCL"},     {PCW_END, ""}};

cpart_ADXL345::cpart_ADXL345(const unsigned x, const unsigned y, const char* name, const char* type, board* pboard_,
                             const int id_)
    : part(x, y, name, type, pboard_, id_) {
    adxl345_init(&adxl);
    adxl345_rst(&adxl);

    adxl_pins[0] = 0;
    adxl_pins[1] = 0;
    adxl_pins[2] = 0;
    adxl_pins[3] = 0;
    adxl_pins[4] = 0;
    adxl_pins[5] = 0;

    active[0] = 0;
    active[1] = 0;
    active[2] = 0;

    ret_ = -1;

    adxl345_set_accel(&adxl, 0, 0, 1.0);

    SetPCWProperties(pcwprop);

    PinCount = 6;
    Pins = adxl_pins;
}

void cpart_ADXL345::RegisterRemoteControl(void) {
    input_ids[I_VS1]->status = &adxl.regs[DATAX1];
    input_ids[I_VS1]->update = &output_ids[O_VS1]->update;

    input_ids[I_VS2]->status = &adxl.regs[DATAY1];
    input_ids[I_VS2]->update = &output_ids[O_VS2]->update;

    input_ids[I_VS3]->status = &adxl.regs[DATAZ1];
    input_ids[I_VS3]->update = &output_ids[O_VS3]->update;
}

cpart_ADXL345::~cpart_ADXL345(void) {
    adxl345_end(&adxl);
    SpareParts.SetPartOnDraw(id);
    SpareParts.CanvasCmd({.cmd = CC_FREEBITMAP, .FreeBitmap{BitmapId}});
    SpareParts.CanvasCmd({.cmd = CC_DESTROY});
}

void cpart_ADXL345::Reset(void) {
    adxl345_rst(&adxl);
    adxl345_set_accel(&adxl, 0, 0, 1.0);
}

void cpart_ADXL345::DrawOutput(const unsigned int i) {
    char val[10];

    switch (output[i].id) {
        case O_VS1:
        case O_VS2:
        case O_VS3:
            switch (adxl.regs[DATA_FORMAT] & 0x03) {
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
            SpareParts.CanvasCmd({.cmd = CC_SETFONTSIZE, .SetFontSize{8}});
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
                if (adxl_pins[pinv] == 0)
                    SpareParts.CanvasCmd(
                        {.cmd = CC_ROTATEDTEXT, .RotatedText{"NC", output[i].x1, output[i].y2 - 30, 90.0}});
                else
                    SpareParts.CanvasCmd({.cmd = CC_ROTATEDTEXT,
                                          .RotatedText{SpareParts.GetPinName(adxl_pins[pinv]).c_str(), output[i].x1,
                                                       output[i].y2 - 30, 90.0}});
            }
            break;
    }
}

unsigned short cpart_ADXL345::GetInputId(char* name) {
    if (strcmp(name, "VS_1") == 0)
        return I_VS1;
    if (strcmp(name, "VS_2") == 0)
        return I_VS2;
    if (strcmp(name, "VS_3") == 0)
        return I_VS3;

    printf("Error input '%s' don't have a valid id! \n", name);
    return INVALID_ID;
}

unsigned short cpart_ADXL345::GetOutputId(char* name) {
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

    printf("Error output '%s' don't have a valid id! \n", name);
    return INVALID_ID;
}

std::string cpart_ADXL345::WritePreferences(void) {
    char prefs[256];

    sprintf(prefs, "%hhu,%hhu,%hhu,%hhu,%hhu,%hhu", adxl_pins[0], adxl_pins[1], adxl_pins[2], adxl_pins[3],
            adxl_pins[4], adxl_pins[5]);

    return prefs;
}

void cpart_ADXL345::ReadPreferences(std::string value) {
    sscanf(value.c_str(), "%hhu,%hhu,%hhu,%hhu,%hhu,%hhu", &adxl_pins[0], &adxl_pins[1], &adxl_pins[2], &adxl_pins[3],
           &adxl_pins[4], &adxl_pins[5]);

    Reset();
}

void cpart_ADXL345::ConfigurePropertiesWindow(void) {
    SetPCWComboWithPinNames("combo3", adxl_pins[0]);
    SetPCWComboWithPinNames("combo4", adxl_pins[1]);
    SetPCWComboWithPinNames("combo5", adxl_pins[2]);
    SetPCWComboWithPinNames("combo6", adxl_pins[3]);
    SetPCWComboWithPinNames("combo7", adxl_pins[4]);
    SetPCWComboWithPinNames("combo8", adxl_pins[5]);
}

void cpart_ADXL345::ReadPropertiesWindow(void) {
    adxl_pins[0] = GetPWCComboSelectedPin("combo3");
    adxl_pins[1] = GetPWCComboSelectedPin("combo4");
    adxl_pins[2] = GetPWCComboSelectedPin("combo5");
    adxl_pins[3] = GetPWCComboSelectedPin("combo6");
    adxl_pins[4] = GetPWCComboSelectedPin("combo7");
    adxl_pins[5] = GetPWCComboSelectedPin("combo8");
}

void cpart_ADXL345::PreProcess(void) {
    const picpin* ppins = SpareParts.GetPinsValues();

    if ((adxl.i2c_mode) && (adxl_pins[0]) && (ppins[adxl_pins[0] - 1].value)) {
        unsigned char addr = 0x1D;

        if (adxl_pins[3]) {
            if (ppins[adxl_pins[3] - 1].value == 0)
                addr = 0x53;
        }

        adxl345_set_addr(&adxl, addr);

        if (adxl_pins[4]) {
            SpareParts.ResetPullupBus(adxl_pins[4] - 1);
        }
    }
}

void cpart_ADXL345::Process(void) {
    const picpin* ppins = SpareParts.GetPinsValues();

    if ((adxl_pins[5]) && (adxl_pins[4]) && (adxl_pins[0])) {
        if ((adxl.i2c_mode) && (ppins[adxl_pins[0] - 1].value)) {  // I2C mode
            SpareParts.SetPullupBus(
                adxl_pins[4] - 1, adxl345_io_I2C(&adxl, ppins[adxl_pins[5] - 1].value, ppins[adxl_pins[4] - 1].value));
        } else {  // SPI mode
            unsigned char ret = adxl345_io_SPI(&adxl, ppins[adxl_pins[4] - 1].value, ppins[adxl_pins[5] - 1].value,
                                               ppins[adxl_pins[0] - 1].value);

            if ((adxl_pins[3])) {
                if (!ppins[adxl_pins[0] - 1].value)  // if SS is active, update output
                {
                    if (ret_ != ret) {
                        SpareParts.SetPin(adxl_pins[3], ret);
                    }
                    ret_ = ret;
                } else {
                    ret_ = 0xFF;  // invalid value
                }
            }
        }
    }
}

void cpart_ADXL345::PostProcess(void) {
    if (adxl.update) {
        if (adxl.regs[DATA_FORMAT] & 0x8) {  // FULL_RES
            switch (adxl.regs[DATA_FORMAT] & 0x03) {
                case 0:  // 2g
                    asens = 256 / 50.0;
                    break;
                case 1:  // 4g
                    asens = 512 / 50.0;
                    break;
                case 2:  // 8g
                    asens = 1023 / 50.0;
                    break;
                case 3:  // 16g
                    asens = 2048 / 50.0;
                    break;
            }
        } else {
            asens = 256 / 50.0;
        }

        adxl.update = 0;
        for (int i = 0; i < outputc; i++) {
            output[i].update = 1;
        }
    }
}

void cpart_ADXL345::setAdxlReg(unsigned char addr, unsigned char val) {
    unsigned char reg = 0;
    short sv = 0;
    switch (addr) {
        case 0:
            reg = DATAX1;
            sv = (val - 100) * asens;
            break;
        case 1:
            reg = DATAY1;
            sv = (val - 100) * asens;
            break;
        case 2:
            reg = DATAZ1;
            sv = (val - 100) * asens;
            break;
    }

    adxl.regs[reg] = (sv & 0xFF00) >> 8;
    adxl.regs[reg - 1] = sv & 0x00FF;
}

unsigned char cpart_ADXL345::getValues(unsigned char addr) {
    short temp;
    unsigned char value = 0;
    switch (addr) {
        case 0:
            temp = ((adxl.regs[DATAX1] << 8) + adxl.regs[DATAX0]);
            value = (temp / asens) + 100;
            break;
        case 1:
            temp = ((adxl.regs[DATAY1] << 8) + adxl.regs[DATAY0]);
            value = (temp / asens) + 100;
            break;
        case 2:
            temp = ((adxl.regs[DATAZ1] << 8) + adxl.regs[DATAZ0]);
            value = (temp / asens) + 100;
            break;
    }
    if (value > 200)
        value = 200;

    return value;
}

void cpart_ADXL345::OnMouseButtonPress(unsigned int inputId, unsigned int button, unsigned int x, unsigned int y,
                                       unsigned int state) {
    unsigned char value;

    switch (inputId) {
        case I_VS1:
            value = 200 - ((y - input_ids[I_VS1]->y1) * 1.66);
            if (value > 200)
                value = 0;
            setAdxlReg(0, value);
            active[0] = 1;
            output_ids[O_VS1]->update = 1;
            break;
        case I_VS2:
            value = 200 - ((y - input_ids[I_VS2]->y1) * 1.66);
            if (value > 200)
                value = 0;
            setAdxlReg(1, value);
            active[1] = 1;
            output_ids[O_VS2]->update = 1;
            break;
        case I_VS3:
            value = 200 - ((y - input_ids[I_VS3]->y1) * 1.66);
            if (value > 200)
                value = 0;
            setAdxlReg(2, value);
            active[2] = 1;
            output_ids[O_VS3]->update = 1;
            break;
    }
}

void cpart_ADXL345::OnMouseButtonRelease(unsigned int inputId, unsigned int button, unsigned int x, unsigned int y,
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
    }
}

void cpart_ADXL345::OnMouseMove(unsigned int inputId, unsigned int button, unsigned int x, unsigned int y,
                                unsigned int state) {
    unsigned char value;

    switch (inputId) {
        case I_VS1 ... I_VS3:
            if (active[inputId - I_VS1]) {
                value = 200 - ((y - input_ids[inputId]->y1) * 1.66);
                if (value > 200)
                    value = 0;

                output_ids[O_VS1 + inputId - I_VS1]->update = 1;
                setAdxlReg(inputId - I_VS1, value);
            }
            break;
        default:
            active[0] = 0;
            active[1] = 0;
            active[2] = 0;
            break;
    }
}

part_init(PART_ADXL345_Name, cpart_ADXL345, "Input");
