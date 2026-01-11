/* ########################################################################

   PICSimLab - Programmable IC Simulator Laboratory

   ########################################################################

   Copyright (c) : 2010-2026  Luis Claudio Gambôa Lopes <lcgamboa@yahoo.com>

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

#define _USE_MATH_DEFINES

#include "output_servo.h"
#include "../lib/oscilloscope.h"
#include "../lib/picsimlab.h"
#include "../lib/spareparts.h"
#include "math.h"

/* outputs */
enum { O_P1, O_AXIS };

static PCWProp pcwprop[2] = {{PCW_COMBO, "Pin 1"}, {PCW_END, ""}};

cpart_servo::cpart_servo(const unsigned x, const unsigned y, const char* name, const char* type, board* pboard_,
                         const int id_)
    : part(x, y, name, type, pboard_, id_) {
    always_update = 1;
    input_pin = 0;
    angle = 0;
    angle_ = 0;
    in_[0] = 0;
    in_[1] = 0;
    time = 0;
    BackGround = -1;
    SetPCWProperties(pcwprop);
    PinCount = 1;
    Pins = &input_pin;
}

cpart_servo::~cpart_servo(void) {
    SpareParts.SetPartOnDraw(id);
    SpareParts.CanvasCmd({.cmd = CC_FREEBITMAP, .FreeBitmap{BitmapId}});
    SpareParts.CanvasCmd({.cmd = CC_FREEBITMAP, .FreeBitmap{BackGround}});
    SpareParts.CanvasCmd({.cmd = CC_DESTROY});
}

void cpart_servo::DrawOutput(const unsigned int i) {
    if (Update == 1) {
        SpareParts.CanvasCmd({.cmd = CC_END});
        SpareParts.CanvasCmd(
            {.cmd = CC_SETBITMAP, .SetBitmap{BackGround, 1.0, 1.0}});  // /FIXME draw servo error on scale or rotate
        SpareParts.CanvasCmd({.cmd = CC_INIT, .Init{Scale, Scale, Orientation}});
    }

    switch (output[i].id) {
        case O_P1:
            SpareParts.CanvasCmd({.cmd = CC_SETFGCOLOR, .SetFgColor{255, 255, 255}});
            if (input_pin == 0)
                SpareParts.CanvasCmd({.cmd = CC_ROTATEDTEXT, .RotatedText{"NC", output[i].x1, output[i].y1, 0}});
            else
                SpareParts.CanvasCmd(
                    {.cmd = CC_ROTATEDTEXT,
                     .RotatedText{SpareParts.GetPinName(input_pin).c_str(), output[i].x1, output[i].y1, 0}});
            break;
        case O_AXIS:
            float x2 = output[i].x1 + output[i].r * sin(angle);
            float y2 = output[i].y1 - output[i].r * cos(angle);
            SpareParts.CanvasCmd({.cmd = CC_SETFGCOLOR, .SetFgColor{0, 0, 0}});
            SpareParts.CanvasCmd({.cmd = CC_SETLINEWIDTH, .SetLineWidth{20}});
            SpareParts.CanvasCmd({.cmd = CC_LINE, .Line{output[i].x1, output[i].y1, x2, y2}});
            SpareParts.CanvasCmd({.cmd = CC_SETFGCOLOR, .SetFgColor{255, 255, 255}});
            SpareParts.CanvasCmd({.cmd = CC_SETLINEWIDTH, .SetLineWidth{18}});
            SpareParts.CanvasCmd({.cmd = CC_LINE, .Line{output[i].x1, output[i].y1, x2, y2}});
            break;
    }
}

void cpart_servo::Process(void) {
    const picpin* ppins = SpareParts.GetPinsValues();

    if (input_pin == 0)
        return;

    in_[1] = in_[0];
    in_[0] = ppins[input_pin - 1].value;

    if ((in_[0] == 1) && (in_[1] == 0))  // rise
    {
        time = 0;
    }

    if ((in_[0] == 0) && (in_[1] == 1))  // low
    {
        angle_ = ((time / pboard->MGetInstClockFreq()) - 0.0015) * 3141.59265359;

        if (angle_ > M_PI / 2.0)
            angle_ = M_PI / 2.0;
        if (angle_ < -M_PI / 2.0)
            angle_ = -M_PI / 2.0;
    }

    time++;
}

void cpart_servo::PostProcess(void) {
    if (angle > angle_) {
        angle -= 0.2;
        if (angle < angle_)
            angle = angle_;
    }

    if (angle < angle_) {
        angle += 0.2;
        if (angle > angle_)
            angle = angle_;
    }

    if (output_ids[O_AXIS]->value_f != angle) {
        output_ids[O_AXIS]->value_f = angle;
        output_ids[O_AXIS]->update = 1;
        output_ids[O_P1]->update = 1;
    }
}

unsigned short cpart_servo::GetInputId(char* name) {
    printf("Error input '%s' don't have a valid id! \n", name);
    return INVALID_ID;
}

unsigned short cpart_servo::GetOutputId(char* name) {
    if (strcmp(name, "PN_1") == 0)
        return O_P1;
    if (strcmp(name, "DG_AXIS") == 0)
        return O_AXIS;

    printf("Error output '%s' don't have a valid id! \n", name);
    return INVALID_ID;
}

std::string cpart_servo::WritePreferences(void) {
    char prefs[256];

    sprintf(prefs, "%hhu", input_pin);

    return prefs;
}

void cpart_servo::ReadPreferences(std::string value) {
    sscanf(value.c_str(), "%hhu", &input_pin);
}

void cpart_servo::RegisterRemoteControl(void) {
    output_ids[O_AXIS]->status = (void*)&angle;
}

void cpart_servo::ConfigurePropertiesWindow(void) {
    SetPCWComboWithPinNames("combo1", input_pin);
}

void cpart_servo::ReadPropertiesWindow(void) {
    input_pin = GetPWCComboSelectedPin("combo1");
}

void cpart_servo::LoadPartImage(void) {
    SpareParts.SetPartOnDraw(id);
    if (BitmapId >= 0) {
        SpareParts.CanvasCmd({.cmd = CC_FREEBITMAP, .FreeBitmap{BitmapId}});
    }
    BitmapId = SpareParts.CanvasCmd(
        {.cmd = CC_LOADIMAGE,
         .LoadImage{(PICSimLab.GetSharePath() + "parts/" + Type + "/" + GetPictureFileName()).c_str(), Scale, 0,
                    Orientation}});

    int bmp = SpareParts.CanvasCmd(
        {.cmd = CC_LOADIMAGE,
         .LoadImage{(PICSimLab.GetSharePath() + "parts/" + Type + "/" + GetPictureFileName()).c_str(), Scale, 0,
                    Orientation}});

    if (BackGround >= 0) {
        SpareParts.CanvasCmd({.cmd = CC_FREEBITMAP, .FreeBitmap{BackGround}});
    }
    BackGround = bmp;

    SpareParts.CanvasCmd({.cmd = CC_DESTROY});
    SpareParts.CanvasCmd({.cmd = CC_CREATE, .Create{BitmapId}});
}

// Register the part in PICSimLab spare parts list
part_init(PART_SERVO_Name, cpart_servo, "Output");
