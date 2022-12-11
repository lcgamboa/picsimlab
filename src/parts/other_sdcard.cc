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

#include "other_sdcard.h"
#include "../picsimlab1.h"
#include "../picsimlab4.h"
#include "../spareparts.h"

/* outputs */
enum { O_P3, O_P4, O_P5, O_P6, O_FILE };

/* inputs */
enum { I_CONN };

/*
const char pin_names[10][10] = {
 "GND",
 "VCC",
 "MISO",
 "MOSI",
 "SCK",
 "CS"
};

const char pin_values[10][10] = {
 "GND",
 "+5V",
 {0},
 {1},
 {2},
 {3}
};
 */

static PCWProp pcwprop[6] = {{PCW_LABEL, "P1-GND ,GND"}, {PCW_LABEL, "P2-VCC,+5V"}, {PCW_COMBO, "P3-MISO"},
                             {PCW_COMBO, "P4-MOSI"},     {PCW_COMBO, "P5-SCK"},     {PCW_COMBO, "P6-CS"}};

cpart_SDCard::cpart_SDCard(const unsigned x, const unsigned y, const char* name, const char* type)
    : part(x, y, name, type), font(8, lxFONTFAMILY_TELETYPE, lxFONTSTYLE_NORMAL, lxFONTWEIGHT_BOLD) {
    sdcard_init(&sd);
    sdcard_rst(&sd);

    input_pins[0] = 0;
    input_pins[1] = 0;
    input_pins[2] = 0;

    output_pins[0] = 0;

    _ret = -1;

    sdcard_fname[0] = '*';
    sdcard_fname[1] = 0;

    SetPCWProperties(pcwprop, 6);
}

cpart_SDCard::~cpart_SDCard(void) {
    delete Bitmap;
    canvas.Destroy();
    sdcard_end(&sd);
}

void cpart_SDCard::Reset(void) {
    sdcard_rst(&sd);
}

void cpart_SDCard::DrawOutput(const unsigned int i) {
    int to;

    switch (output[i].id) {
        case O_FILE:
            canvas.SetColor(49, 61, 99);
            canvas.Rectangle(1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);
            canvas.SetFgColor(255, 255, 255);
            to = strlen(sdcard_fname) + 4;
            if (to < 38) {
                to = 0;
            } else {
                to = to - 38;
            }
            canvas.RotatedText(lxT("Img:") + lxString(sdcard_fname + to), output[i].x1, output[i].y1, 0);
            break;
        default:
            canvas.SetColor(49, 61, 99);
            canvas.Rectangle(1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);

            canvas.SetFgColor(155, 155, 155);

            int pinv = output[i].id - O_P3;
            int pin = 0;
            switch (pinv) {
                case 0:
                case 4:
                    pin = pinv > 1;
                    if (output_pins[pin] == 0)
                        canvas.RotatedText("NC", output[i].x1, output[i].y2, 90.0);
                    else
                        canvas.RotatedText(SpareParts.GetPinName(output_pins[pin]), output[i].x1, output[i].y2, 90.0);
                    break;
                case 1:
                case 2:
                case 3:
                    pinv++;
                case 5:
                    pin = pinv - 2;
                    if (input_pins[pin] == 0)
                        canvas.RotatedText("NC", output[i].x1, output[i].y2, 90.0);
                    else
                        canvas.RotatedText(SpareParts.GetPinName(input_pins[pin]), output[i].x1, output[i].y2, 90.0);
            }
            break;
    }
}

unsigned short cpart_SDCard::GetInputId(char* name) {
    if (strcmp(name, "CN_CONN") == 0)
        return I_CONN;

    printf("Erro input '%s' don't have a valid id! \n", name);
    return -1;
}

unsigned short cpart_SDCard::GetOutputId(char* name) {
    if (strcmp(name, "PN_3") == 0)
        return O_P3;
    if (strcmp(name, "PN_4") == 0)
        return O_P4;
    if (strcmp(name, "PN_5") == 0)
        return O_P5;
    if (strcmp(name, "PN_6") == 0)
        return O_P6;
    if (strcmp(name, "DI_FILE") == 0)
        return O_FILE;

    printf("Erro output '%s' don't have a valid id! \n", name);
    return 1;
}

lxString cpart_SDCard::WritePreferences(void) {
    char prefs[256];

    sprintf(prefs, "%hhu,%hhu,%hhu,%hhu,%s", input_pins[0], input_pins[1], input_pins[2], output_pins[0], sdcard_fname);

    return prefs;
}

void cpart_SDCard::ReadPreferences(lxString value) {
    sscanf(value.c_str(), "%hhu,%hhu,%hhu,%hhu,%s", &input_pins[0], &input_pins[1], &input_pins[2], &output_pins[0],
           sdcard_fname);

    Reset();
    if (sdcard_fname[0] != '*') {
#ifdef _WIN_
        if (!strncmp(sdcard_fname, "/tmp/", 5)) {
            char buff[200];
            strcpy(buff, (const char*)lxGetTempDir("PICSimLab").c_str());
            strcat(buff, sdcard_fname + 4);
            strcpy(sdcard_fname, buff);
        }
#endif
        sdcard_set_filename(&sd, sdcard_fname);
        sdcard_set_card_present(&sd, 1);
    } else {
        sdcard_set_card_present(&sd, 0);
    }
}

void cpart_SDCard::ConfigurePropertiesWindow(CPWindow* WProp) {
    SetPCWComboWithPinNames(WProp, "combo3", output_pins[0]);
    SetPCWComboWithPinNames(WProp, "combo4", input_pins[0]);
    SetPCWComboWithPinNames(WProp, "combo5", input_pins[1]);
    SetPCWComboWithPinNames(WProp, "combo6", input_pins[2]);
}

void cpart_SDCard::ReadPropertiesWindow(CPWindow* WProp) {
    output_pins[0] = GetPWCComboSelectedPin(WProp, "combo3");
    input_pins[0] = GetPWCComboSelectedPin(WProp, "combo4");
    input_pins[1] = GetPWCComboSelectedPin(WProp, "combo5");
    input_pins[2] = GetPWCComboSelectedPin(WProp, "combo6");
}

void cpart_SDCard::PreProcess(void) {}

void cpart_SDCard::Process(void) {
    const picpin* ppins = SpareParts.GetPinsValues();

    unsigned short ret = 0;

    ret =
        sdcard_io(&sd, ppins[input_pins[0] - 1].value, ppins[input_pins[1] - 1].value, ppins[input_pins[2] - 1].value);

    if (!ppins[input_pins[2] - 1].value)  // if SS is active, update output
    {
        if (_ret != ret) {
            SpareParts.SetPin(output_pins[0], ret);
        }
        _ret = ret;
    } else {
        _ret = 0xFF;  // invalid value
    }
}

void cpart_SDCard::OnMouseButtonPress(uint inputId, uint button, uint x, uint y, uint state) {
    switch (inputId) {
        case I_CONN:
            SpareParts.GetFileDialog()->SetType(lxFD_OPEN | lxFD_CHANGE_DIR);
            SpareParts.GetFileDialog()->SetFilter(lxT("SD Card image (*.img)|*.img"));
            if (sdcard_fname[0] == '*') {
                SpareParts.GetFileDialog()->SetFileName(lxT("untitled.img"));
            } else {
                SpareParts.GetFileDialog()->SetFileName(sdcard_fname);
            }
            SpareParts.Setfdtype(id);
            SpareParts.GetFileDialog()->Run();
            break;
    }
}

void cpart_SDCard::filedialog_EvOnClose(int retId) {
    if (retId) {
        if ((SpareParts.GetFileDialog()->GetType() == (lxFD_OPEN | lxFD_CHANGE_DIR))) {
            if (lxFileExists(SpareParts.GetFileDialog()->GetFileName())) {
                strncpy(sdcard_fname, SpareParts.GetFileDialog()->GetFileName().c_str(), 199);
                sdcard_set_filename(&sd, sdcard_fname);
                sdcard_set_card_present(&sd, 1);
            } else {
                sdcard_set_card_present(&sd, 0);
            }
        }
    }
    output_ids[O_FILE]->update = 1;
}

void cpart_SDCard::PostProcess(void) {}

part_init(PART_SDCARD_Name, cpart_SDCard, "Other");
