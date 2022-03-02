/* ########################################################################

   PICsimLab - PIC laboratory simulator

   ########################################################################

   Copyright (c) : 2022-2022  Luis Claudio Gambôa Lopes

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

#include "part_LED_WS2812B.h"
#include "../picsimlab1.h"
#include "../picsimlab4.h"
#include "../picsimlab5.h"

/* outputs */
enum { O_P1, O_P2, O_F1, O_F2, O_LED };

cpart_led_ws2812b::cpart_led_ws2812b(unsigned x, unsigned y)
    : font(8, lxFONTFAMILY_TELETYPE, lxFONTSTYLE_NORMAL, lxFONTWEIGHT_BOLD) {
    X = x;
    Y = y;
    ReadMaps();
    OWidth = Width;
    OHeight = Height;

    Bitmap = NULL;

    aways_update = 1;

    led_ws2812b_init(&led, 1, 1, 1);

    ChangeType(1, 1, 0);

    led_ws2812b_rst(&led);

    input_pins[0] = 0;

    output_pins[0] = Window5.RegisterIOpin(lxT("DOUT"));
}

cpart_led_ws2812b::~cpart_led_ws2812b(void) {
    Window5.UnregisterIOpin(output_pins[0]);
    delete Bitmap;
    canvas.Destroy();
    led_ws2812b_end(&led);
}

void cpart_led_ws2812b::LoadImage(void) {
    if (led.nleds > 1) {
        lxString ifname;
        FILE* fimg;

        xoff = (led.ncols - 1) * 40;
        yoff = (led.nrows - 1) * 40;
        Width = OWidth + xoff;
        Height = OHeight + yoff;

        ifname = lxGetTempDir(lxT("picsimlab")) + lxT("/LEDRGB.svg");

        fimg = fopen((const char*)ifname.c_str(), "w");

        if (fimg) {
            fprintf(fimg,
                    "<svg width=\"%i\" height=\"%i\" version=\"1.1\" viewBox=\"0 0 100 "
                    "100\" xmlns=\"http://www.w3.org/2000/svg\">"
                    "\n <rect x =\"0\" y=\"0\" width=\"100\" height=\"100\"  />\n</svg>",
                    Width, Height);
            fclose(fimg);
        } else {
            printf("PICSimLab: Erro open file %s\n", (const char*)ifname.c_str());
        }

        lxImage image(&Window5);

        image.LoadFile(lxGetLocalFile(ifname), Orientation, Scale, Scale);
        Bitmap = new lxBitmap(&image, &Window5);
        image.Destroy();

        canvas.Destroy();
        canvas.Create(Window5.GetWWidget(), Bitmap);

        image.LoadFile(lxGetLocalFile(Window1.GetSharePath() + lxT("parts/") + GetPictureFileName()), Orientation,
                       Scale, Scale);
        lxBitmap* BackBitmap = new lxBitmap(&image, &Window5);
        image.Destroy();

        image.LoadFile(lxGetLocalFile(Window1.GetSharePath() + lxT("parts/") + GetName() + lxT("/LED.svg")),
                       Orientation, Scale * 1.3, Scale * 1.3);
        lxBitmap* LEDBitmap = new lxBitmap(&image, &Window5);
        image.Destroy();

        canvas.Init(Scale, Scale, Orientation);
        canvas.SetColor(0x31, 0x3d, 0x63);
        canvas.Rectangle(1, 0, 0, Width, Height);

        canvas.ChangeScale(1.0, 1.0);
        canvas.PutBitmap(BackBitmap, 0, yoff * Scale);

        if (!led.diffuser) {
            for (unsigned int r = 0; r < led.nrows; r++)
                for (unsigned int c = 0; c < led.ncols; c++) {
                    canvas.PutBitmap(LEDBitmap, (output_ids[O_LED]->x1 + (40 * c)) * Scale,
                                     (output_ids[O_LED]->y1 - (40 * r) + yoff) * Scale);
                }
        }
        canvas.ChangeScale(Scale, Scale);
        canvas.End();

        delete BackBitmap;
        delete LEDBitmap;
        lxRemoveFile(ifname);
    } else {
        Width = OWidth;
        Height = OHeight;
        xoff = 0;
        yoff = 0;
        part::LoadImage();
    }
}

void cpart_led_ws2812b::Draw(void) {
    int i;

    Update = 0;

    for (i = 0; i < outputc; i++) {
        if (output[i].update)  // only if need update
        {
            output[i].update = 0;

            if (!Update) {
                canvas.Init(Scale, Scale, Orientation);
                canvas.SetFont(font);
            }
            Update++;  // set to update buffer

            switch (output[i].id) {
                case O_P1:
                    canvas.SetColor(49, 61, 99);
                    canvas.Rectangle(1, output[i].x1, output[i].y1 + yoff, output[i].x2 - output[i].x1,
                                     output[i].y2 - output[i].y1);
                    canvas.SetFgColor(255, 255, 255);
                    if (input_pins[output[i].id - O_P1] == 0)
                        canvas.RotatedText("NC", output[i].x1, output[i].y2 + yoff, 90.0);
                    else
                        canvas.RotatedText(Window5.GetPinName(input_pins[output[i].id - O_P1]), output[i].x1,
                                           output[i].y2 + yoff, 90.0);
                    break;
                case O_P2:
                    canvas.SetColor(49, 61, 99);
                    canvas.Rectangle(1, output[i].x1, output[i].y1 + yoff, output[i].x2 - output[i].x1,
                                     output[i].y2 - output[i].y1);
                    canvas.SetFgColor(255, 255, 255);
                    if (output_pins[output[i].id - O_P2] == 0)
                        canvas.RotatedText("NC", output[i].x1, output[i].y2 + yoff, 90.0);
                    else
                        canvas.RotatedText(itoa(output_pins[output[i].id - O_P2]), output[i].x1, output[i].y2 + yoff,
                                           90.0);
                    break;
                    break;
                case O_F1:
                    canvas.SetColor(49, 61, 99);
                    canvas.Rectangle(1, output[i].x1, output[i].y1 + yoff, output[i].x2 - output[i].x1,
                                     output[i].y2 - output[i].y1);
                    canvas.SetFgColor(155, 155, 155);
                    canvas.RotatedText("5V", output[i].x1, output[i].y2 + yoff, 90.0);
                    break;
                case O_F2:
                    canvas.SetColor(49, 61, 99);
                    canvas.Rectangle(1, output[i].x1, output[i].y1 + yoff, output[i].x2 - output[i].x1,
                                     output[i].y2 - output[i].y1);
                    canvas.SetFgColor(155, 155, 155);
                    canvas.RotatedText("GND", output[i].x1, output[i].y2 + yoff, 90.0);
                    break;
                case O_LED:
                    // draw led text
                    if (led.update) {
                        canvas.SetColor(0, 90 + 40, 0);
                        led_ws2812b_draw(&led, &canvas, output[i].x1, output[i].y1 + yoff, output[i].x2 - output[i].x1,
                                         output[i].y2 - output[i].y1, 1);
                    }
                    break;
            }
        }
    }

    if (Update) {
        canvas.End();
    }
}

unsigned short cpart_led_ws2812b::get_in_id(char* name) {
    printf("Erro input '%s' don't have a valid id! \n", name);
    return -1;
}

unsigned short cpart_led_ws2812b::get_out_id(char* name) {
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

    printf("Erro output '%s' don't have a valid id! \n", name);
    return 1;
}

lxString cpart_led_ws2812b::WritePreferences(void) {
    char prefs[256];

    sprintf(prefs, "%hhu,%hhu,%u,%u,%hhu", input_pins[0], output_pins[0], led.nrows, led.ncols, led.diffuser);

    return prefs;
}

void cpart_led_ws2812b::ReadPreferences(lxString value) {
    unsigned char outp;
    unsigned int rows, cols;
    unsigned char diffuser;
    sscanf(value.c_str(), "%hhu,%hhu,%u,%u,%hhu", &input_pins[0], &outp, &rows, &cols, &diffuser);

    Window5.UnregisterIOpin(output_pins[0]);
    output_pins[0] = Window5.RegisterIOpin(lxT("DOUT"), outp);

    ChangeType(rows, cols, diffuser);

    Reset();

    RegisterRemoteControl();
}

void cpart_led_ws2812b::ChangeType(const unsigned int rows, const unsigned int cols, const unsigned char diffuser) {
    if ((led.nrows != rows) || (led.ncols != cols) || (led.diffuser != diffuser)) {
        if (Bitmap) {
            delete Bitmap;
        }

        unsigned int rows_ = rows;
        unsigned int cols_ = cols;
        led_ws2812b_end(&led);
        if (rows > MAXROWS)
            rows_ = MAXROWS;
        if (cols > MAXCOLS)
            cols_ = MAXCOLS;
        led_ws2812b_init(&led, rows_, cols_, diffuser);

        LoadImage();
    }
}

void cpart_led_ws2812b::RegisterRemoteControl(void) {
    for (int i = 0; i < outputc; i++) {
        switch (output[i].id) {
            case O_LED:
                output[i].status = (void*)&led;
                break;
        }
    }
}

void cpart_led_ws2812b::ConfigurePropertiesWindow(CPWindow* WProp) {
    lxString Items = Window5.GetPinsNames();
    lxString spin;

    ((CCombo*)WProp->GetChildByName("combo1"))->SetItems(Items);
    if (input_pins[0] == 0)
        ((CCombo*)WProp->GetChildByName("combo1"))->SetText("0  NC");
    else {
        spin = Window5.GetPinName(input_pins[0]);
        ((CCombo*)WProp->GetChildByName("combo1"))->SetText(itoa(input_pins[0]) + "  " + spin);
    }

    ((CLabel*)WProp->GetChildByName("label3"))->SetText("Pin 2 - DOUT     " + itoa(output_pins[0]));

    ((CSpin*)WProp->GetChildByName("spin1"))->SetMax(MAXROWS);
    ((CSpin*)WProp->GetChildByName("spin1"))->SetValue(led.nrows);

    ((CSpin*)WProp->GetChildByName("spin2"))->SetMax(MAXCOLS);
    ((CSpin*)WProp->GetChildByName("spin2"))->SetValue(led.ncols);

    if (led.diffuser) {
        ((CCombo*)WProp->GetChildByName("combo2"))->SetText("On");
    } else {
        ((CCombo*)WProp->GetChildByName("combo2"))->SetText("Off");
    }

    ((CButton*)WProp->GetChildByName("button1"))->EvMouseButtonRelease =
        EVMOUSEBUTTONRELEASE & CPWindow5::PropButtonRelease;
    ((CButton*)WProp->GetChildByName("button1"))->SetTag(1);

    ((CButton*)WProp->GetChildByName("button2"))->EvMouseButtonRelease =
        EVMOUSEBUTTONRELEASE & CPWindow5::PropButtonRelease;
}

void cpart_led_ws2812b::ReadPropertiesWindow(CPWindow* WProp) {
    unsigned int rows, cols;
    unsigned char diffuser;

    input_pins[0] = atoi(((CCombo*)WProp->GetChildByName("combo1"))->GetText());

    rows = ((CSpin*)WProp->GetChildByName("spin1"))->GetValue();
    cols = ((CSpin*)WProp->GetChildByName("spin2"))->GetValue();

    if (!((CCombo*)WProp->GetChildByName("combo2"))->GetText().compare("On")) {
        diffuser = 1;
    } else {
        diffuser = 0;
    }

    ChangeType(rows, cols, diffuser);

    RegisterRemoteControl();
}

void cpart_led_ws2812b::PreProcess(void) {
    led_ws2812b_prepare(&led, Window1.GetBoard()->MGetInstClockFreq());
}

void cpart_led_ws2812b::Process(void) {
    const picpin* ppins = Window5.GetPinsValues();

    if (input_pins[0] > 0) {
        unsigned char out;
        out = led_ws2812b_io(&led, ppins[input_pins[0] - 1].value);

        if (out != ppins[output_pins[0] - 1].value) {
            Window5.WritePin(output_pins[0], out);
        }
    }
}

void cpart_led_ws2812b::PostProcess(void) {
    if (led.update)
        output_ids[O_LED]->update = 1;
}

part_init(PART_LED_WS2812B_Name, cpart_led_ws2812b, "Output");
