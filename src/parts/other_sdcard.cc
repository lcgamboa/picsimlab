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

#include "other_sdcard.h"
#include "../lib/oscilloscope.h"
#include "../lib/picsimlab.h"
#include "../lib/spareparts.h"

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

static PCWProp pcwprop[7] = {
    {PCW_LABEL, "P1-GND ,GND"}, {PCW_LABEL, "P2-VCC,+5V"}, {PCW_COMBO, "P3-MISO"}, {PCW_COMBO, "P4-MOSI"},
    {PCW_COMBO, "P5-SCK"},      {PCW_COMBO, "P6-CS"},      {PCW_END, ""}};

cpart_SDCard::cpart_SDCard(const unsigned x, const unsigned y, const char* name, const char* type, board* pboard_,
                           const int id_)
    : part(x, y, name, type, pboard_, id_) {
    sdcard_init(&sd);
    sdcard_rst(&sd);

    pins[0] = 0;
    pins[1] = 0;
    pins[2] = 0;
    pins[3] = 0;

    _ret = -1;

    sdcard_fname[0] = '*';
    sdcard_fname[1] = 0;

    SetPCWProperties(pcwprop);

    PinCount = 4;
    Pins = pins;
}

cpart_SDCard::~cpart_SDCard(void) {
    SpareParts.SetPartOnDraw(id);
    SpareParts.CanvasCmd({.cmd = CC_FREEBITMAP, .FreeBitmap{BitmapId}});
    SpareParts.CanvasCmd({.cmd = CC_DESTROY});
    sdcard_end(&sd);
}

void cpart_SDCard::Reset(void) {
    sdcard_rst(&sd);
}

void cpart_SDCard::DrawOutput(const unsigned int i) {
    int to;

    switch (output[i].id) {
        case O_FILE:
            SpareParts.CanvasCmd({.cmd = CC_SETFONTSIZE, .SetFontSize{8}});
            SpareParts.CanvasCmd({.cmd = CC_SETCOLOR, .SetColor{49, 61, 99}});
            SpareParts.CanvasCmd(
                {.cmd = CC_RECTANGLE,
                 .Rectangle{1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1}});
            SpareParts.CanvasCmd({.cmd = CC_SETFGCOLOR, .SetFgColor{255, 255, 255}});
            to = strlen(sdcard_fname) + 4;
            if (to < 38) {
                to = 0;
            } else {
                to = to - 38;
            }
            SpareParts.CanvasCmd(
                {.cmd = CC_ROTATEDTEXT,
                 .RotatedText{("Img:" + std::string(sdcard_fname + to)).c_str(), output[i].x1, output[i].y1, 0}});
            break;
        default:
            SpareParts.CanvasCmd({.cmd = CC_SETCOLOR, .SetColor{49, 61, 99}});
            SpareParts.CanvasCmd(
                {.cmd = CC_RECTANGLE,
                 .Rectangle{1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1}});

            SpareParts.CanvasCmd({.cmd = CC_SETFGCOLOR, .SetFgColor{155, 155, 155}});

            int pinv = output[i].id - O_P3;
            int pin = 0;
            switch (pinv) {
                case 0:
                case 4:
                    pin = 3;
                    if (pins[pin] == 0)
                        SpareParts.CanvasCmd(
                            {.cmd = CC_ROTATEDTEXT, .RotatedText{"NC", output[i].x1, output[i].y2, 90.0}});
                    else
                        SpareParts.CanvasCmd(
                            {.cmd = CC_ROTATEDTEXT,
                             .RotatedText{SpareParts.GetPinName(pins[pin]).c_str(), output[i].x1, output[i].y2, 90.0}});
                    break;
                case 1:
                case 2:
                case 3:
                    pinv++;
                case 5:
                    pin = pinv - 2;
                    if (pins[pin] == 0)
                        SpareParts.CanvasCmd(
                            {.cmd = CC_ROTATEDTEXT, .RotatedText{"NC", output[i].x1, output[i].y2, 90.0}});
                    else
                        SpareParts.CanvasCmd(
                            {.cmd = CC_ROTATEDTEXT,
                             .RotatedText{SpareParts.GetPinName(pins[pin]).c_str(), output[i].x1, output[i].y2, 90.0}});
            }
            break;
    }
}

unsigned short cpart_SDCard::GetInputId(char* name) {
    if (strcmp(name, "CN_CONN") == 0)
        return I_CONN;

    printf("Error input '%s' don't have a valid id! \n", name);
    return INVALID_ID;
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

    printf("Error output '%s' don't have a valid id! \n", name);
    return INVALID_ID;
}

std::string cpart_SDCard::WritePreferences(void) {
    char prefs[256];

    sprintf(prefs, "%hhu,%hhu,%hhu,%hhu,%s", pins[0], pins[1], pins[2], pins[3], sdcard_fname);

    return prefs;
}

void cpart_SDCard::ReadPreferences(std::string value) {
    sscanf(value.c_str(), "%hhu,%hhu,%hhu,%hhu,%s", &pins[0], &pins[1], &pins[2], &pins[3], sdcard_fname);

    Reset();

    if (sdcard_fname[0] != '*') {
        if (!strncmp(sdcard_fname, "/tmp/picsimlab_workspace/", 25)) {
            char buff[256];
            strcpy(buff, PICSimLab.GetPzwTmpdir());
            strcat(buff, sdcard_fname + 4);
            strcpy(sdcard_fname, buff);
        }

        sdcard_set_filename(&sd, sdcard_fname);
        sdcard_set_card_present(&sd, 1);
    } else {
        sdcard_set_card_present(&sd, 0);
    }
}

void cpart_SDCard::ConfigurePropertiesWindow(void) {
    SetPCWComboWithPinNames("combo3", pins[3]);
    SetPCWComboWithPinNames("combo4", pins[0]);
    SetPCWComboWithPinNames("combo5", pins[1]);
    SetPCWComboWithPinNames("combo6", pins[2]);
}

void cpart_SDCard::ReadPropertiesWindow(void) {
    pins[3] = GetPWCComboSelectedPin("combo3");
    pins[0] = GetPWCComboSelectedPin("combo4");
    pins[1] = GetPWCComboSelectedPin("combo5");
    pins[2] = GetPWCComboSelectedPin("combo6");
}

void cpart_SDCard::Process(void) {
    const picpin* ppins = SpareParts.GetPinsValues();

    unsigned short ret = 0;

    ret = sdcard_io(&sd, ppins[pins[0] - 1].value, ppins[pins[1] - 1].value, ppins[pins[2] - 1].value);

    if (!ppins[pins[2] - 1].value)  // if SS is active, update output
    {
        if (_ret != ret) {
            SpareParts.SetPin(pins[3], ret);
        }
        _ret = ret;
    } else {
        _ret = 0xFF;  // invalid value
    }
}

void cpart_SDCard::OnMouseButtonPress(unsigned int inputId, unsigned int button, unsigned int x, unsigned int y,
                                      unsigned int state) {
    switch (inputId) {
        case I_CONN:
            SpareParts.WindowCmd(PW_MAIN, "filedialog1", PWA_FILEDIALOGSETTYPE,
                                 std::to_string(PFD_OPEN | PFD_CHANGE_DIR).c_str());
            SpareParts.WindowCmd(PW_MAIN, "filedialog1", PWA_FILEDIALOGSETFILTER, "SD Card image (*.img)|*.img");
            if (sdcard_fname[0] == '*') {
                SpareParts.WindowCmd(PW_MAIN, "filedialog1", PWA_FILEDIALOGSETFNAME, "untitled.img");
            } else {
                SpareParts.WindowCmd(PW_MAIN, "filedialog1", PWA_FILEDIALOGSETFNAME, sdcard_fname);
            }
            SpareParts.Setfdtype(id);
            SpareParts.WindowCmd(PW_MAIN, "filedialog1", PWA_FILEDIALOGRUN, NULL);
            break;
    }
}

void cpart_SDCard::filedialog_EvOnClose(int retId) {
    if (retId) {
        int type;
        SpareParts.WindowCmd(PW_MAIN, "filedialog1", PWA_FILEDIALOGGETTYPE, NULL, &type);
        if ((type == (PFD_OPEN | PFD_CHANGE_DIR))) {
            char buff[256];
            SpareParts.WindowCmd(PW_MAIN, "filedialog1", PWA_FILEDIALOGGETFNAME, NULL, buff);
            if (PICSimLab.SystemCmd(PSC_FILEEXISTS, buff)) {
                strcpy(sdcard_fname, buff);
                sdcard_set_filename(&sd, sdcard_fname);
                sdcard_set_card_present(&sd, 1);
            } else {
                sdcard_set_card_present(&sd, 0);
            }
        }
    }
    output_ids[O_FILE]->update = 1;
}

part_init(PART_SDCARD_Name, cpart_SDCard, "Other");
