/* ########################################################################

   PICSimLab - Programmable IC Simulator Laboratory

   ########################################################################

   Copyright (c) : 2019-2024  Luis Claudio Gambôa Lopes <lcgamboa@yahoo.com>

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

#include "other_MI2C_24CXXX.h"
#include <unistd.h>
#include "../lib/oscilloscope.h"
#include "../lib/picsimlab.h"
#include "../lib/spareparts.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

/* inputs */
enum { I_LOAD, I_SAVE, I_VIEW };

/* outputs */
enum { O_P1, O_P2, O_P3, O_P4, O_P5, O_P6, O_P7, O_P8, O_IC };

const char pin_names[8][10] = {"A0", "A1", "A2", "VSS", "SDA", "SCL", "WP", "VCC"};
const char pin_values[8][10] = {{0}, {1}, {2}, "GND", {3}, {4}, "GND", "+5V"};

static PCWProp pcwprop[10] = {{PCW_COMBO, "P1 - A0"},
                              {PCW_COMBO, "P2 - A1"},
                              {PCW_COMBO, "P3 - A1"},
                              {PCW_LABEL, "P4 - VSS,GND"},
                              {PCW_COMBO, "P5 - SDA"},
                              {PCW_COMBO, "P6 - SCL"},
                              {PCW_LABEL, "P7 - WP      GND"},
                              {PCW_LABEL, "P8 - VCC,+5V"},
                              {PCW_COMBO, "kbits"},
                              {PCW_END, ""}};

cpart_MI2C_24CXXX::cpart_MI2C_24CXXX(const unsigned x, const unsigned y, const char* name, const char* type,
                                     board* pboard_, const int id_)
    : part(x, y, name, type, pboard_, id_, 8) {
    kbits = 4;

    mi2c_init(&mi2c, kbits);
    mi2c_rst(&mi2c);

    input_pins[0] = 0;
    input_pins[1] = 0;
    input_pins[2] = 0;
    input_pins[3] = 0;
    input_pins[4] = 0;

    f_mi2c_name[0] = '*';
    f_mi2c_name[1] = 0;
    f_mi2c = NULL;

    char tname[128];
    PICSimLab.SystemCmd(PSC_GETTEMPDIR, NULL, tname);
    snprintf(f_mi2c_tmp_name, 200, "%s/picsimlab-XXXXXX", tname);
    close(mkstemp(f_mi2c_tmp_name));
    unlink(f_mi2c_tmp_name);

    strncat(f_mi2c_tmp_name, ".txt", 200);

    SetPCWProperties(pcwprop);

    PinCount = 5;
    Pins = input_pins;
}

cpart_MI2C_24CXXX::~cpart_MI2C_24CXXX(void) {
    mi2c_end(&mi2c);
    SpareParts.SetPartOnDraw(id);
    SpareParts.CanvasCmd({.cmd = CC_FREEBITMAP, .FreeBitmap{BitmapId}});
    SpareParts.CanvasCmd({.cmd = CC_DESTROY});
    unlink(f_mi2c_tmp_name);
}

void cpart_MI2C_24CXXX::DrawOutput(const unsigned int i) {
    switch (output[i].id) {
        case O_IC:
            char buff[10];
            snprintf(buff, 9, "24C%02i", kbits);
            SpareParts.CanvasCmd({.cmd = CC_SETCOLOR, .SetColor{26, 26, 26}});
            SpareParts.CanvasCmd(
                {.cmd = CC_RECTANGLE,
                 .Rectangle{1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1}});
            SpareParts.CanvasCmd({.cmd = CC_SETFGCOLOR, .SetFgColor{255, 255, 255}});
            SpareParts.CanvasCmd({.cmd = CC_ROTATEDTEXT, .RotatedText{buff, output[i].x1, output[i].y2 - 15, 0}});
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
                if (input_pins[pinv] == 0)
                    SpareParts.CanvasCmd(
                        {.cmd = CC_ROTATEDTEXT, .RotatedText{"NC", output[i].x1, output[i].y2 - 30, 90.0}});
                else
                    SpareParts.CanvasCmd({.cmd = CC_ROTATEDTEXT,
                                          .RotatedText{SpareParts.GetPinName(input_pins[pinv]).c_str(), output[i].x1,
                                                       output[i].y2 - 30, 90.0}});
            }
            break;
    }
}

unsigned short cpart_MI2C_24CXXX::GetInputId(char* name) {
    if (strcmp(name, "PB_LOAD") == 0)
        return I_LOAD;
    if (strcmp(name, "PB_SAVE") == 0)
        return I_SAVE;
    if (strcmp(name, "PB_VIEW") == 0)
        return I_VIEW;

    printf("Error input '%s' don't have a valid id! \n", name);
    return INVALID_ID;
}

unsigned short cpart_MI2C_24CXXX::GetOutputId(char* name) {
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

    if (strcmp(name, "IC_8") == 0)
        return O_IC;

    printf("Error output '%s' don't have a valid id! \n", name);
    return INVALID_ID;
}

std::string cpart_MI2C_24CXXX::WritePreferences(void) {
    char prefs[256];

    sprintf(prefs, "%hhu,%hhu,%hhu,%hhu,%hhu,%u,%s", input_pins[0], input_pins[1], input_pins[2], input_pins[3],
            input_pins[4], kbits, f_mi2c_name);

    if (f_mi2c_name[0] != '*') {
        FILE* fout;
        fout = fopen_UTF8(f_mi2c_name, "wb");
        if (fout) {
            fwrite(mi2c.data, mi2c.SIZE, 1, fout);
            fclose(fout);
        } else {
            printf("Error saving to file: %s \n", f_mi2c_name);
        }
    }

    return prefs;
}

void cpart_MI2C_24CXXX::ReadPreferences(std::string value) {
    sscanf(value.c_str(), "%hhu,%hhu,%hhu,%hhu,%hhu,%u,%s", &input_pins[0], &input_pins[1], &input_pins[2],
           &input_pins[3], &input_pins[4], &kbits, f_mi2c_name);

    mi2c_end(&mi2c);
    mi2c_init(&mi2c, kbits);
    mi2c_rst(&mi2c);

    if (f_mi2c_name[0] != '*') {
        FILE* fout;
        fout = fopen_UTF8(f_mi2c_name, "rb");
        if (fout) {
            fread(mi2c.data, mi2c.SIZE, 1, fout);
            fclose(fout);
        } else {
            printf("Error loading from file: %s \n", f_mi2c_name);
        }
    }
    Reset();
}

void cpart_MI2C_24CXXX::ConfigurePropertiesWindow(void) {
    SetPCWComboWithPinNames("combo1", input_pins[0]);
    SetPCWComboWithPinNames("combo2", input_pins[1]);
    SetPCWComboWithPinNames("combo3", input_pins[2]);
    SetPCWComboWithPinNames("combo5", input_pins[3]);
    SetPCWComboWithPinNames("combo6", input_pins[4]);

    SpareParts.WPropCmd("combo9", PWA_COMBOSETITEMS, "4,512,");
    SpareParts.WPropCmd("combo9", PWA_COMBOSETTEXT, std::to_string(kbits).c_str());
}

void cpart_MI2C_24CXXX::ReadPropertiesWindow(void) {
    input_pins[0] = GetPWCComboSelectedPin("combo1");
    input_pins[1] = GetPWCComboSelectedPin("combo2");
    input_pins[2] = GetPWCComboSelectedPin("combo3");
    input_pins[3] = GetPWCComboSelectedPin("combo5");
    input_pins[4] = GetPWCComboSelectedPin("combo6");

    char buff[64];
    SpareParts.WPropCmd("combo9", PWA_COMBOGETTEXT, NULL, buff);

    int nkbits = std::stoi(buff);

    if (nkbits != kbits) {
        kbits = nkbits;
        mi2c_end(&mi2c);
        mi2c_init(&mi2c, kbits);
        mi2c_rst(&mi2c);
        f_mi2c_name[0] = '*';
        f_mi2c_name[1] = 0;
    }
}

void cpart_MI2C_24CXXX::PreProcess(void) {
    const picpin* ppins = SpareParts.GetPinsValues();
    unsigned char addr = 0x50;

    if (input_pins[0]) {
        if (ppins[input_pins[0] - 1].value)
            addr |= 0x01;
    }
    if (input_pins[1]) {
        if (ppins[input_pins[1] - 1].value)
            addr |= 0x02;
    }
    if (input_pins[2]) {
        if (ppins[input_pins[2] - 1].value)
            addr |= 0x04;
    }

    mi2c_set_addr(&mi2c, addr);

    if (input_pins[3] > 0) {
        SpareParts.ResetPullupBus(input_pins[3] - 1);
    }
}

void cpart_MI2C_24CXXX::Process(void) {
    const picpin* ppins = SpareParts.GetPinsValues();

    if ((input_pins[3] > 0) && (input_pins[4] > 0))
        SpareParts.SetPullupBus(input_pins[3] - 1,
                                mi2c_io(&mi2c, ppins[input_pins[4] - 1].value, ppins[input_pins[3] - 1].value));
}

void cpart_MI2C_24CXXX::OnMouseButtonPress(unsigned int inputId, unsigned int button, unsigned int x, unsigned int y,
                                           unsigned int state) {
    switch (inputId) {
        case I_LOAD:
            SpareParts.WindowCmd(PW_MAIN, "filedialog1", PWA_FILEDIALOGSETTYPE,
                                 std::to_string(PFD_OPEN | PFD_CHANGE_DIR).c_str());
            SpareParts.WindowCmd(PW_MAIN, "filedialog1", PWA_FILEDIALOGSETFILTER,
                                 "PICSimLab Binary File (*.bin)|*.bin");
            SpareParts.WindowCmd(PW_MAIN, "filedialog1", PWA_FILEDIALOGSETFNAME, "untitled.bin");
            SpareParts.Setfdtype(id);
            SpareParts.WindowCmd(PW_MAIN, "filedialog1", PWA_FILEDIALOGRUN, NULL);
            break;
        case I_SAVE:
            SpareParts.WindowCmd(PW_MAIN, "filedialog1", PWA_FILEDIALOGSETTYPE,
                                 std::to_string(PFD_SAVE | PFD_CHANGE_DIR).c_str());
            SpareParts.WindowCmd(PW_MAIN, "filedialog1", PWA_FILEDIALOGSETFILTER,
                                 "PICSimLab Binary File (*.bin)|*.bin");
            SpareParts.WindowCmd(PW_MAIN, "filedialog1", PWA_FILEDIALOGSETFNAME, "untitled.bin");
            SpareParts.Setfdtype(id);
            SpareParts.WindowCmd(PW_MAIN, "filedialog1", PWA_FILEDIALOGRUN, NULL);
            break;
        case I_VIEW:
            FILE* fout;
            fout = fopen_UTF8(f_mi2c_tmp_name, "w");
            if (fout) {
                for (unsigned int i = 0; i < mi2c.SIZE; i += 16) {
                    fprintf(fout, "%04X: ", i);
                    for (int j = 0; j < 16; j++) {
                        fprintf(fout, "%02X ", mi2c.data[j + i]);
                    }
                    fprintf(fout, "\r\n");
                }
                fclose(fout);
#ifdef __EMSCRIPTEN__
                EM_ASM_(
                    {
                        var filename = UTF8ToString($0);
                        var buf = FS.readFile(filename);
                        var blob = new Blob([buf], { "type" : "application/octet-stream" });
                        var text = URL.createObjectURL(blob);

                        var element = document.createElement('a');
                        element.setAttribute('href', text);
                        element.setAttribute('download', filename);

                        element.style.display = 'none';
                        document.body.appendChild(element);

                        element.click();

                        document.body.removeChild(element);
                        URL.revokeObjectURL(text);
                    },
                    f_mi2c_tmp_name);
#else
                PICSimLab.SystemCmd(PSC_LAUNCHDEFAULAPPLICATION, f_mi2c_tmp_name);
#endif
            } else {
                printf("Error saving to file: %s \n", f_mi2c_tmp_name);
            }
            break;
    }
}

void cpart_MI2C_24CXXX::filedialog_EvOnClose(int retId) {
    if (retId) {
        int type;
        SpareParts.WindowCmd(PW_MAIN, "filedialog1", PWA_FILEDIALOGGETTYPE, NULL, &type);
        if ((type == (PFD_OPEN | PFD_CHANGE_DIR))) {
            char buff[200];
            SpareParts.WindowCmd(PW_MAIN, "filedialog1", PWA_FILEDIALOGGETFNAME, NULL, buff);
            if (PICSimLab.SystemCmd(PSC_FILEEXISTS, buff)) {
                char fname[128];
                PICSimLab.SystemCmd(PSC_BASENAME, buff, fname);
                if (!PICSimLab.SystemCmd(PSC_SHOWDIALOG, (std::string("Overwriting file: ") + fname + " ?").c_str()))
                    return;
            }

            FILE* fout;
            fout = fopen_UTF8(buff, "wb");
            if (fout) {
                fwrite(mi2c.data, mi2c.SIZE, 1, fout);
                fclose(fout);
                strncpy(f_mi2c_name, buff, 200);
            } else {
                printf("Error saving to file: %s \n", (const char*)buff);
            }
        }

        if (type == (PFD_OPEN | PFD_CHANGE_DIR)) {
            FILE* fout;
            char buff[256];
            SpareParts.WindowCmd(PW_MAIN, "filedialog1", PWA_FILEDIALOGGETFNAME, NULL, buff);
            fout = fopen_UTF8(buff, "rb");
            if (fout) {
                fread(mi2c.data, mi2c.SIZE, 1, fout);
                fclose(fout);
            } else {
                printf("Error loading from file: %s \n", buff);
            }
        }
    }
}

part_init(PART_MI2C_24CXXX_Name, cpart_MI2C_24CXXX, "Other");
