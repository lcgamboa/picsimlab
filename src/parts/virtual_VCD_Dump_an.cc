/* ########################################################################

   PICSimLab - Programmable IC Simulator Laboratory

   ########################################################################

   Copyright (c) : 2010-2024  Luis Claudio Gambôa Lopes <lcgamboa@yahoo.com>

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

#include "virtual_VCD_Dump_an.h"
#include <unistd.h>
#include "../lib/oscilloscope.h"
#include "../lib/picsimlab.h"
#include "../lib/spareparts.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

static const char markers[] = "!$%&[()]";

/* outputs */
enum { O_P1, O_P2, O_P3, O_P4, O_P5, O_P6, O_P7, O_P8, O_L1, O_L2, O_L3, O_L4, O_L5, O_L6, O_L7, O_L8, O_NAME, O_REC };

/*inputs*/
enum { I_START, I_VIEW };

static PCWProp pcwprop[9] = {{PCW_COMBO, "Pin 1"}, {PCW_COMBO, "Pin 2"}, {PCW_COMBO, "Pin 3"},
                             {PCW_COMBO, "Pin 4"}, {PCW_COMBO, "Pin 5"}, {PCW_COMBO, "Pin 6"},
                             {PCW_COMBO, "Pin 7"}, {PCW_COMBO, "Pin 8"}, {PCW_END, ""}};

cpart_VCD_Dump_an::cpart_VCD_Dump_an(const unsigned x, const unsigned y, const char* name, const char* type,
                                     board* pboard_, const int id_)
    : part(x, y, name, type, pboard_, id_) {
    always_update = 1;

    input_pins[0] = 0;
    input_pins[1] = 0;
    input_pins[2] = 0;
    input_pins[3] = 0;
    input_pins[4] = 0;
    input_pins[5] = 0;
    input_pins[6] = 0;
    input_pins[7] = 0;

    old_value_pins[0] = 2;
    old_value_pins[1] = 2;
    old_value_pins[2] = 2;
    old_value_pins[3] = 2;
    old_value_pins[4] = 2;
    old_value_pins[5] = 2;
    old_value_pins[6] = 2;
    old_value_pins[7] = 2;

    char tname[128];
    PICSimLab.SystemCmd(PSC_GETTEMPDIR, NULL, tname);
    snprintf(f_vcd_name, 200, "%s/picsimlab-XXXXXX", tname);
    close(mkstemp(f_vcd_name));
    unlink(f_vcd_name);

    strncat(f_vcd_name, ".vcd", 200);

    f_vcd = fopen_UTF8(f_vcd_name, "w");
    fclose(f_vcd);
    f_vcd = NULL;

    rec = 0;
    vcd_count = 0;

    SetPCWProperties(pcwprop);

    PinCount = 8;
    Pins = input_pins;
}

void cpart_VCD_Dump_an::RegisterRemoteControl(void) {
    input_ids[I_START]->status = &rec;
    input_ids[I_START]->update = &output_ids[O_REC]->update;
}

cpart_VCD_Dump_an::~cpart_VCD_Dump_an(void) {
    SpareParts.SetPartOnDraw(id);
    SpareParts.CanvasCmd({.cmd = CC_FREEBITMAP, .FreeBitmap{BitmapId}});
    SpareParts.CanvasCmd({.cmd = CC_DESTROY});

    if (f_vcd) {
        fclose(f_vcd);
        f_vcd = NULL;
    }
    unlink(f_vcd_name);
}

void cpart_VCD_Dump_an::DrawOutput(const unsigned int i) {
    int to;

    const picpin* ppins = SpareParts.GetPinsValues();

    switch (output[i].id) {
        case O_P1:
        case O_P2:
        case O_P3:
        case O_P4:
        case O_P5:
        case O_P6:
        case O_P7:
        case O_P8:
            SpareParts.CanvasCmd({.cmd = CC_SETCOLOR, .SetColor{49, 61, 99}});
            SpareParts.CanvasCmd(
                {.cmd = CC_RECTANGLE,
                 .Rectangle{1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1}});
            SpareParts.CanvasCmd({.cmd = CC_SETFGCOLOR, .SetFgColor{255, 255, 255}});
            if (input_pins[output[i].id - O_P1] == 0)
                SpareParts.CanvasCmd({.cmd = CC_ROTATEDTEXT, .RotatedText{"NC", output[i].x1, output[i].y1, 0}});
            else
                SpareParts.CanvasCmd({.cmd = CC_ROTATEDTEXT,
                                      .RotatedText{SpareParts.GetPinName(input_pins[output[i].id - O_P1]).c_str(),
                                                   output[i].x1, output[i].y1, 0}});
            break;
        case O_NAME:
            SpareParts.CanvasCmd({.cmd = CC_SETCOLOR, .SetColor{49, 61, 99}});
            SpareParts.CanvasCmd(
                {.cmd = CC_RECTANGLE,
                 .Rectangle{1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1}});
            SpareParts.CanvasCmd({.cmd = CC_SETFGCOLOR, .SetFgColor{255, 255, 255}});
            to = strlen(f_vcd_name) + 5;
            if (to < 48) {
                to = 0;
            } else {
                to = to - 48;
            }
            SpareParts.CanvasCmd(
                {.cmd = CC_ROTATEDTEXT,
                 .RotatedText{((f_vcd_name + to) + std::string(" (An)")).c_str(), output[i].x1, output[i].y1, 0}});
            break;
        case O_L1:
        case O_L2:
        case O_L3:
        case O_L4:
        case O_L5:
        case O_L6:
        case O_L7:
        case O_L8:
            if (input_pins[output[i].id - O_L1] > 0) {
                if (ppins[input_pins[output[i].id - O_L1] - 1].dir == PD_IN) {
                    SpareParts.CanvasCmd(
                        {.cmd = CC_SETCOLOR,
                         .SetColor{(unsigned int)(ppins[input_pins[output[i].id - O_L1] - 1].avalue * 51), 0, 0}});
                } else {
                    SpareParts.CanvasCmd(
                        {.cmd = CC_SETCOLOR,
                         .SetColor{(unsigned int)ppins[input_pins[output[i].id - O_L1] - 1].oavalue, 0, 0}});
                }
            } else {
                SpareParts.CanvasCmd({.cmd = CC_SETCOLOR, .SetColor{30, 0, 0}});
            }
            SpareParts.CanvasCmd({.cmd = CC_SETFGCOLOR, .SetFgColor{0, 0, 0}});
            DrawLED(SpareParts.CanvasCmd, &output[i]);
            break;
        case O_REC:
            if (rec > 0) {
                SpareParts.CanvasCmd({.cmd = CC_SETCOLOR, .SetColor{0, 255, 0}});
            } else {
                SpareParts.CanvasCmd({.cmd = CC_SETCOLOR, .SetColor{255, 0, 0}});
            }
            SpareParts.CanvasCmd({.cmd = CC_SETFGCOLOR, .SetFgColor{0, 0, 0}});
            DrawLED(SpareParts.CanvasCmd, &output[i]);
            break;
    }
}

unsigned short cpart_VCD_Dump_an::GetInputId(char* name) {
    if (strcmp(name, "PB_START") == 0)
        return I_START;
    if (strcmp(name, "PB_VIEW") == 0)
        return I_VIEW;

    printf("Error input '%s' don't have a valid id! \n", name);
    return INVALID_ID;
};

unsigned short cpart_VCD_Dump_an::GetOutputId(char* name) {
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

    if (strcmp(name, "LD_1") == 0)
        return O_L1;
    if (strcmp(name, "LD_2") == 0)
        return O_L2;
    if (strcmp(name, "LD_3") == 0)
        return O_L3;
    if (strcmp(name, "LD_4") == 0)
        return O_L4;
    if (strcmp(name, "LD_5") == 0)
        return O_L5;
    if (strcmp(name, "LD_6") == 0)
        return O_L6;
    if (strcmp(name, "LD_7") == 0)
        return O_L7;
    if (strcmp(name, "LD_8") == 0)
        return O_L8;

    if (strcmp(name, "DI_NAME") == 0)
        return O_NAME;
    if (strcmp(name, "LD_REC") == 0)
        return O_REC;

    printf("Error output '%s' don't have a valid id! \n", name);
    return INVALID_ID;
}

std::string cpart_VCD_Dump_an::WritePreferences(void) {
    char prefs[256];

    sprintf(prefs, "%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu", input_pins[0], input_pins[1], input_pins[2],
            input_pins[3], input_pins[4], input_pins[5], input_pins[6], input_pins[7], rec);

    return prefs;
}

void cpart_VCD_Dump_an::ReadPreferences(std::string value) {
    sscanf(value.c_str(), "%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu", &input_pins[0], &input_pins[1],
           &input_pins[2], &input_pins[3], &input_pins[4], &input_pins[5], &input_pins[6], &input_pins[7], &rec);
}

void cpart_VCD_Dump_an::ConfigurePropertiesWindow(void) {
    SetPCWComboWithPinNames("combo1", input_pins[0]);
    SetPCWComboWithPinNames("combo2", input_pins[1]);
    SetPCWComboWithPinNames("combo3", input_pins[2]);
    SetPCWComboWithPinNames("combo4", input_pins[3]);
    SetPCWComboWithPinNames("combo5", input_pins[4]);
    SetPCWComboWithPinNames("combo6", input_pins[5]);
    SetPCWComboWithPinNames("combo7", input_pins[6]);
    SetPCWComboWithPinNames("combo8", input_pins[7]);
}

void cpart_VCD_Dump_an::ReadPropertiesWindow(void) {
    input_pins[0] = GetPWCComboSelectedPin("combo1");
    input_pins[1] = GetPWCComboSelectedPin("combo2");
    input_pins[2] = GetPWCComboSelectedPin("combo3");
    input_pins[3] = GetPWCComboSelectedPin("combo4");
    input_pins[4] = GetPWCComboSelectedPin("combo5");
    input_pins[5] = GetPWCComboSelectedPin("combo6");
    input_pins[6] = GetPWCComboSelectedPin("combo7");
    input_pins[7] = GetPWCComboSelectedPin("combo8");
}

void cpart_VCD_Dump_an::PreProcess(void) {
    if (rec && (f_vcd == NULL)) {
        float tscale = 1.0e12 / pboard->MGetInstClockFreq();  // ns step

        f_vcd = fopen_UTF8(f_vcd_name, "w");
        vcd_count = 0;

        fprintf(f_vcd,
                "$version Generated by PICSimLab $end\n"
                "$timescale %ips $end\n"
                "$scope module analogic $end\n",
                (int)tscale);

        if (input_pins[0])
            fprintf(f_vcd, "$var real 32 !  1-%s $end\n", (const char*)SpareParts.GetPinName(input_pins[0]).c_str());
        if (input_pins[1])
            fprintf(f_vcd, "$var real 32 $  2-%s $end\n", (const char*)SpareParts.GetPinName(input_pins[1]).c_str());
        if (input_pins[2])
            fprintf(f_vcd, "$var real 32 %%  3-%s $end\n", (const char*)SpareParts.GetPinName(input_pins[2]).c_str());
        if (input_pins[3])
            fprintf(f_vcd, "$var real 32 &  4-%s $end\n", (const char*)SpareParts.GetPinName(input_pins[3]).c_str());
        if (input_pins[4])
            fprintf(f_vcd, "$var real 32 [  5-%s $end\n", (const char*)SpareParts.GetPinName(input_pins[4]).c_str());
        if (input_pins[5])
            fprintf(f_vcd, "$var real 32 (  6-%s $end\n", (const char*)SpareParts.GetPinName(input_pins[5]).c_str());
        if (input_pins[6])
            fprintf(f_vcd, "$var real 32 )  7-%s $end\n", (const char*)SpareParts.GetPinName(input_pins[6]).c_str());
        if (input_pins[7])
            fprintf(f_vcd, "$var real 32 ]  8-%s $end\n", (const char*)SpareParts.GetPinName(input_pins[7]).c_str());

        fprintf(f_vcd,
                "$upscope $end\n"
                "$enddefinitions $end\n");
    } else if (!rec && (f_vcd != NULL)) {
        if (f_vcd) {
            fclose(f_vcd);
            f_vcd = NULL;
        }
    }
}

void cpart_VCD_Dump_an::Process(void) {
    if (rec && f_vcd) {
        const picpin* ppins = SpareParts.GetPinsValues();

        vcd_count++;
        int tprint = 0;

        for (int i = 0; i < 8; i++) {
            if (input_pins[i] != 0) {
                if (ppins[input_pins[i] - 1].dir == PD_IN) {
                    if (ppins[input_pins[i] - 1].avalue != old_value_pins[i]) {
                        if (!tprint) {
                            tprint = 1;
                            fprintf(f_vcd, "#%li\n", vcd_count);
                        }
                        old_value_pins[i] = ppins[input_pins[i] - 1].avalue;
                        fprintf(f_vcd, "r%f %c\n", old_value_pins[i], markers[i]);
                        fflush(f_vcd);
                    }
                } else  // out
                {
                    if (ppins[input_pins[i] - 1].oavalue != old_value_pins[i]) {
                        if (!tprint) {
                            tprint = 1;
                            fprintf(f_vcd, "#%li\n", vcd_count);
                        }
                        old_value_pins[i] = ppins[input_pins[i] - 1].oavalue;
                        fprintf(f_vcd, "r%f %c\n", old_value_pins[i] / 51, markers[i]);
                        fflush(f_vcd);
                    }
                }
            }
        }
    }
}

void cpart_VCD_Dump_an::PostProcess(void) {
    const picpin* ppins = SpareParts.GetPinsValues();

    for (int i = 0; i < 8; i++) {
        if (input_pins[i] && (output_ids[O_L1 + i]->value != ppins[input_pins[i] - 1].oavalue)) {
            output_ids[O_L1 + i]->value = ppins[input_pins[i] - 1].oavalue;
            output_ids[O_L1 + i]->update = 1;
        }
    }
}

void cpart_VCD_Dump_an::OnMouseButtonPress(unsigned int inputId, unsigned int button, unsigned int x, unsigned int y,
                                           unsigned int state) {
    switch (inputId) {
        case I_START:
            rec ^= 1;
            output_ids[O_REC]->update = 1;
            break;
        case I_VIEW:
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
                f_vcd_name);
#else
            PICSimLab.SystemCmd(PSC_LAUNCHDEFAULAPPLICATION, f_vcd_name);
#endif
            break;
    }
}

part_init(PART_VCD_DUMP_AN_Name, cpart_VCD_Dump_an, "Virtual");
