/* ########################################################################

   PICSimLab - Programmable IC Simulator Laboratory

   ########################################################################

   Copyright (c) : 2022-2024  Luis Claudio Gambôa Lopes <lcgamboa@yahoo.com>

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

#include "output_LED_WS2812B.h"
#include "../lib/oscilloscope.h"
#include "../lib/picsimlab.h"
#include "../lib/spareparts.h"

/* outputs */
enum { O_P1, O_P2, O_F1, O_F2, O_LED };

static PCWProp pcwprop[8] = {
    {PCW_LABEL, "1-VDD,+5V"}, {PCW_LABEL, "2-DOUT,NC"}, {PCW_LABEL, "3-VSS,GND"}, {PCW_COMBO, "4-DIN"},
    {PCW_SPIN, "Rows"},       {PCW_SPIN, "Cols"},       {PCW_COMBO, "Diffuser"},  {PCW_END, ""}};

cpart_led_ws2812b::cpart_led_ws2812b(const unsigned x, const unsigned y, const char* name, const char* type,
                                     board* pboard_, const int id_)
    : part(x, y, name, type, pboard_, id_) {
    X = x;
    Y = y;
    ReadMaps();
    OWidth = Width;
    OHeight = Height;

    BitmapId = -1;

    always_update = 1;

    led_ws2812b_init(&led, 1, 1, 1);

    ChangeType(1, 1, 0);

    led_ws2812b_rst(&led);

    input_pins[0] = 0;

    output_pins[0] = SpareParts.RegisterIOpin("DOUT");

    SetPCWProperties(pcwprop);

    PinCount = 1;
    Pins = input_pins;
}

cpart_led_ws2812b::~cpart_led_ws2812b(void) {
    SpareParts.UnregisterIOpin(output_pins[0]);
    SpareParts.SetPartOnDraw(id);
    SpareParts.CanvasCmd({CC_FREEBITMAP, .FreeBitmap{BitmapId}});
    SpareParts.CanvasCmd({CC_DESTROY});
    led_ws2812b_end(&led);
}

void cpart_led_ws2812b::LoadPartImage(void) {
    if (led.nleds > 1) {
        xoff = (led.ncols - 1) * 40;
        yoff = (led.nrows - 1) * 40;
        Width = OWidth + xoff;
        Height = OHeight + yoff;

        if (SpareParts.GetWindow()) {
            SpareParts.SetPartOnDraw(id);
            BitmapId = SpareParts.CanvasCmd({CC_CREATEIMAGE, .CreateImage{Width, Height, Scale, 0, Orientation}});

            SpareParts.CanvasCmd({CC_DESTROY});
            SpareParts.CanvasCmd({CC_CREATE, .Create{BitmapId}});

            int BackBitmap = SpareParts.CanvasCmd(
                {CC_LOADIMAGE,
                 .LoadImage{(PICSimLab.GetSharePath() + "parts/" + Type + "/" + GetPictureFileName()).c_str(), Scale, 0,
                            Orientation}});

            int LEDBitmap = SpareParts.CanvasCmd(
                {CC_LOADIMAGE,
                 .LoadImage{(PICSimLab.GetSharePath() + "parts/" + Type + "/" + GetName() + "/LED.svg").c_str(), Scale,
                            0, Orientation}});

            SpareParts.CanvasCmd({CC_INIT, .Init{Scale, Scale, Orientation}});
            SpareParts.CanvasCmd({CC_SETCOLOR, .SetColor{0x31, 0x3d, 0x63}});
            SpareParts.CanvasCmd({CC_RECTANGLE, .Rectangle{1, 0, 0, (float)Width, (float)Height}});

            SpareParts.CanvasCmd({CC_CHANGESCALE, .ChangeScale{1.0, 1.0}});
            SpareParts.CanvasCmd({CC_PUTBITMAP, .PutBitmap{BackBitmap, 0, yoff * Scale}});

            if (!led.diffuser) {
                for (unsigned int r = 0; r < led.nrows; r++)
                    for (unsigned int c = 0; c < led.ncols; c++) {
                        SpareParts.CanvasCmd(
                            {CC_PUTBITMAP, .PutBitmap{LEDBitmap, (output_ids[O_LED]->x1 + (40 * c)) * Scale,
                                                      (output_ids[O_LED]->y1 - (40 * r) + yoff) * Scale}});
                    }
            }
            SpareParts.CanvasCmd({CC_CHANGESCALE, .ChangeScale{Scale, Scale}});
            SpareParts.CanvasCmd({CC_END});

            SpareParts.CanvasCmd({CC_FREEBITMAP, .FreeBitmap{BackBitmap}});
            SpareParts.CanvasCmd({CC_FREEBITMAP, .FreeBitmap{LEDBitmap}});
        }
    } else {
        Width = OWidth;
        Height = OHeight;
        xoff = 0;
        yoff = 0;
        part::LoadPartImage();
    }
}

void cpart_led_ws2812b::DrawOutput(const unsigned int i) {
    switch (output[i].id) {
        case O_P1:
            SpareParts.CanvasCmd({CC_SETFONTSIZE, .SetFontSize{8}});
            SpareParts.CanvasCmd({CC_SETCOLOR, .SetColor{49, 61, 99}});
            SpareParts.CanvasCmd({CC_RECTANGLE, .Rectangle{1, output[i].x1, output[i].y1 + yoff,
                                                           output[i].x2 - output[i].x1, output[i].y2 - output[i].y1}});
            SpareParts.CanvasCmd({CC_SETFGCOLOR, .SetFgColor{255, 255, 255}});
            if (input_pins[output[i].id - O_P1] == 0)
                SpareParts.CanvasCmd({CC_ROTATEDTEXT, .RotatedText{"NC", output[i].x1, output[i].y2 + yoff, 90.0}});
            else
                SpareParts.CanvasCmd(
                    {CC_ROTATEDTEXT, .RotatedText{SpareParts.GetPinName(input_pins[output[i].id - O_P1]).c_str(),
                                                  output[i].x1, output[i].y2 + yoff, 90.0}});
            break;
        case O_P2:
            SpareParts.CanvasCmd({CC_SETCOLOR, .SetColor{49, 61, 99}});
            SpareParts.CanvasCmd({CC_RECTANGLE, .Rectangle{1, output[i].x1, output[i].y1 + yoff,
                                                           output[i].x2 - output[i].x1, output[i].y2 - output[i].y1}});
            SpareParts.CanvasCmd({CC_SETFGCOLOR, .SetFgColor{255, 255, 255}});
            if (output_pins[output[i].id - O_P2] == 0)
                SpareParts.CanvasCmd({CC_ROTATEDTEXT, .RotatedText{"NC", output[i].x1, output[i].y2 + yoff, 90.0}});
            else
                SpareParts.CanvasCmd(
                    {CC_ROTATEDTEXT, .RotatedText{std::to_string(output_pins[output[i].id - O_P2]).c_str(),
                                                  output[i].x1, output[i].y2 + yoff, 90.0}});
            break;
            break;
        case O_F1:
            SpareParts.CanvasCmd({CC_SETCOLOR, .SetColor{49, 61, 99}});
            SpareParts.CanvasCmd({CC_RECTANGLE, .Rectangle{1, output[i].x1, output[i].y1 + yoff,
                                                           output[i].x2 - output[i].x1, output[i].y2 - output[i].y1}});
            SpareParts.CanvasCmd({CC_SETFGCOLOR, .SetFgColor{155, 155, 155}});
            SpareParts.CanvasCmd({CC_ROTATEDTEXT, .RotatedText{"5V", output[i].x1, output[i].y2 + yoff, 90.0}});
            break;
        case O_F2:
            SpareParts.CanvasCmd({CC_SETCOLOR, .SetColor{49, 61, 99}});
            SpareParts.CanvasCmd({CC_RECTANGLE, .Rectangle{1, output[i].x1, output[i].y1 + yoff,
                                                           output[i].x2 - output[i].x1, output[i].y2 - output[i].y1}});
            SpareParts.CanvasCmd({CC_SETFGCOLOR, .SetFgColor{155, 155, 155}});
            SpareParts.CanvasCmd({CC_ROTATEDTEXT, .RotatedText{"GND", output[i].x1, output[i].y2 + yoff, 90.0}});
            break;
        case O_LED:
            // draw led text
            if (led.update) {
                SpareParts.CanvasCmd({CC_SETCOLOR, .SetColor{0, 90 + 40, 0}});
                led_ws2812b_draw(&led, SpareParts.CanvasCmd, output[i].x1, output[i].y1 + yoff,
                                 output[i].x2 - output[i].x1, output[i].y2 - output[i].y1, 1);
            }
            break;
    }
}

unsigned short cpart_led_ws2812b::GetInputId(char* name) {
    printf("Error input '%s' don't have a valid id! \n", name);
    return INVALID_ID;
}

unsigned short cpart_led_ws2812b::GetOutputId(char* name) {
    if (strcmp(name, "PN_1") == 0)
        return O_P1;
    if (strcmp(name, "PN_2") == 0)
        return O_P2;

    if (strcmp(name, "PN_F1") == 0)
        return O_F1;
    if (strcmp(name, "PN_F2") == 0)
        return O_F2;

    if (strcmp(name, "LR_LED") == 0)
        return O_LED;

    printf("Error output '%s' don't have a valid id! \n", name);
    return INVALID_ID;
}

std::string cpart_led_ws2812b::WritePreferences(void) {
    char prefs[256];

    sprintf(prefs, "%hhu,%hhu,%u,%u,%hhu", input_pins[0], output_pins[0], led.nrows, led.ncols, led.diffuser);

    return prefs;
}

void cpart_led_ws2812b::ReadPreferences(std::string value) {
    unsigned char outp;
    unsigned int rows, cols;
    unsigned char diffuser;
    sscanf(value.c_str(), "%hhu,%hhu,%u,%u,%hhu", &input_pins[0], &outp, &rows, &cols, &diffuser);

    SpareParts.UnregisterIOpin(output_pins[0]);
    output_pins[0] = SpareParts.RegisterIOpin("DOUT", outp);

    ChangeType(rows, cols, diffuser);

    Reset();
}

void cpart_led_ws2812b::ChangeType(const unsigned int rows, const unsigned int cols, const unsigned char diffuser) {
    if ((led.nrows != rows) || (led.ncols != cols) || (led.diffuser != diffuser)) {
        if (BitmapId >= 0) {
            SpareParts.SetPartOnDraw(id);
            SpareParts.CanvasCmd({CC_FREEBITMAP, .FreeBitmap{BitmapId}});
        }

        unsigned int rows_ = rows;
        unsigned int cols_ = cols;
        led_ws2812b_end(&led);
        if (rows > MAXROWS)
            rows_ = MAXROWS;
        if (cols > MAXCOLS)
            cols_ = MAXCOLS;
        led_ws2812b_init(&led, rows_, cols_, diffuser);

        LoadPartImage();
    }
}

void cpart_led_ws2812b::RegisterRemoteControl(void) {
    output_ids[O_LED]->status = (void*)&led;
}

void cpart_led_ws2812b::ConfigurePropertiesWindow(void) {
    SetPCWComboWithPinNames("combo4", input_pins[0]);

    SpareParts.WPropCmd("label_2", PWA_LABELSETTEXT, std::to_string(output_pins[0]).c_str());

    SpareParts.WPropCmd("spin5", PWA_SPINSETMAX, std::to_string(MAXROWS).c_str());
    SpareParts.WPropCmd("spin5", PWA_SPINSETMIN, "1");
    SpareParts.WPropCmd("spin5", PWA_SPINSETVALUE, std::to_string(led.nrows).c_str());

    SpareParts.WPropCmd("spin6", PWA_SPINSETMAX, std::to_string(MAXCOLS).c_str());
    SpareParts.WPropCmd("spin6", PWA_SPINSETMIN, "1");
    SpareParts.WPropCmd("spin6", PWA_SPINSETVALUE, std::to_string(led.ncols).c_str());

    SpareParts.WPropCmd("combo7", PWA_COMBOSETITEMS, "On,Off,");
    if (led.diffuser) {
        SpareParts.WPropCmd("combo7", PWA_COMBOSETTEXT, "On");
    } else {
        SpareParts.WPropCmd("combo7", PWA_COMBOSETTEXT, "Off");
    }
}

void cpart_led_ws2812b::ReadPropertiesWindow(void) {
    unsigned int rows, cols;
    unsigned char diffuser;

    input_pins[0] = GetPWCComboSelectedPin("combo4");

    SpareParts.WPropCmd("spin5", PWA_SPINGETVALUE, NULL, &rows);
    SpareParts.WPropCmd("spin6", PWA_SPINGETVALUE, NULL, &cols);

    char buff[64];
    SpareParts.WPropCmd("combo7", PWA_COMBOGETTEXT, NULL, buff);

    if (!strcmp(buff, "On")) {
        diffuser = 1;
    } else {
        diffuser = 0;
    }

    ChangeType(rows, cols, diffuser);
}

void cpart_led_ws2812b::PreProcess(void) {
    led_ws2812b_prepare(&led, pboard->MGetInstClockFreq());
}

void cpart_led_ws2812b::Process(void) {
    const picpin* ppins = SpareParts.GetPinsValues();

    if (input_pins[0] > 0) {
        unsigned char out;
        out = led_ws2812b_io(&led, ppins[input_pins[0] - 1].value);

        if (out != ppins[output_pins[0] - 1].value) {
            SpareParts.WritePin(output_pins[0], out);
        }
    }
}

void cpart_led_ws2812b::PostProcess(void) {
    if (led.update)
        output_ids[O_LED]->update = 1;
}

part_init(PART_LED_WS2812B_Name, cpart_led_ws2812b, "Output");
