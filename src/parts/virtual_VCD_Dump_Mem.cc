/* ########################################################################

   PICSimLab - Programmable IC Simulator Laboratory

   ########################################################################

   Copyright (c) : 2024-2025  Luis Claudio Gambôa Lopes <lcgamboa@yahoo.com>

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

#include "virtual_VCD_Dump_Mem.h"
#include <unistd.h>
#include "../lib/oscilloscope.h"
#include "../lib/picsimlab.h"
#include "../lib/spareparts.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

static const char markers[] = "!$%&[()]+*{}<>'^";

/* outputs */
enum { O_P1, O_P2, O_P3, O_P4, O_P5, O_P6, O_P7, O_P8, O_L1, O_L2, O_L3, O_L4, O_L5, O_L6, O_L7, O_L8, O_NAME, O_REC };

/*inputs*/
enum { I_START, I_VIEW };

static PCWProp pcwprop[3] = {{PCW_SPIN, "Addr"}, {PCW_COMBO, "Size"}, {PCW_END, ""}};

cpart_VCD_Dump_Mem::cpart_VCD_Dump_Mem(const unsigned x, const unsigned y, const char* name, const char* type,
                                       board* pboard_, const int id_)
    : part(x, y, name, type, pboard_, id_) {
    always_update = 1;

    output_bits[0] = 2;
    output_bits[1] = 2;
    output_bits[2] = 2;
    output_bits[3] = 2;
    output_bits[4] = 2;
    output_bits[5] = 2;
    output_bits[6] = 2;
    output_bits[7] = 2;

    mem_addr = 0;
    mem_size = 8;

    mcount = 0;
    memset(output_bits_alm, 0, 8 * sizeof(unsigned long));

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

    PinCount = 0;
    Pins = NULL;
}

void cpart_VCD_Dump_Mem::RegisterRemoteControl(void) {
    input_ids[I_START]->status = &rec;
    input_ids[I_START]->update = &output_ids[O_REC]->update;
}

cpart_VCD_Dump_Mem::~cpart_VCD_Dump_Mem(void) {
    SpareParts.SetPartOnDraw(id);
    SpareParts.CanvasCmd({.cmd = CC_FREEBITMAP, .FreeBitmap{BitmapId}});
    SpareParts.CanvasCmd({.cmd = CC_DESTROY});

    if (f_vcd) {
        fclose(f_vcd);
        f_vcd = NULL;
    }
    unlink(f_vcd_name);
}

void cpart_VCD_Dump_Mem::DrawOutput(const unsigned int i) {
    int to;
    char vname[50];

    switch (output[i].id) {
        case O_P1:
            SpareParts.CanvasCmd({.cmd = CC_SETCOLOR, .SetColor{49, 61, 99}});
            SpareParts.CanvasCmd({.cmd = CC_RECTANGLE, .Rectangle{1, output[i].x1, output[i].y1, 200, 30}});
            SpareParts.CanvasCmd({.cmd = CC_SETFGCOLOR, .SetFgColor{255, 255, 255}});
            sprintf(vname, "Mem[%d]", mem_addr);
            SpareParts.CanvasCmd({.cmd = CC_SETFONTSIZE, .SetFontSize{12}});
            SpareParts.CanvasCmd({.cmd = CC_ROTATEDTEXT, .RotatedText{vname, output[i].x1, output[i].y1, 0}});
            SpareParts.CanvasCmd({.cmd = CC_SETFONTSIZE, .SetFontSize{8}});
            break;
        case O_NAME:
            SpareParts.CanvasCmd({.cmd = CC_SETCOLOR, .SetColor{49, 61, 99}});
            SpareParts.CanvasCmd(
                {.cmd = CC_RECTANGLE,
                 .Rectangle{1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1}});
            SpareParts.CanvasCmd({.cmd = CC_SETFGCOLOR, .SetFgColor{255, 255, 255}});
            to = strlen(f_vcd_name);
            if (to < 48) {
                to = 0;
            } else {
                to = to - 48;
            }
            SpareParts.CanvasCmd({.cmd = CC_ROTATEDTEXT, .RotatedText{f_vcd_name + to, output[i].x1, output[i].y1, 0}});
            break;
        case O_L1:
        case O_L2:
        case O_L3:
        case O_L4:
        case O_L5:
        case O_L6:
        case O_L7:
        case O_L8:
            SpareParts.CanvasCmd({.cmd = CC_SETCOLOR, .SetColor{(unsigned int)output_bits[output[i].id - O_L1], 0, 0}});

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

unsigned short cpart_VCD_Dump_Mem::GetInputId(char* name) {
    if (strcmp(name, "PB_START") == 0)
        return I_START;
    if (strcmp(name, "PB_VIEW") == 0)
        return I_VIEW;

    printf("Error input '%s' don't have a valid id! \n", name);
    return INVALID_ID;
};

unsigned short cpart_VCD_Dump_Mem::GetOutputId(char* name) {
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

std::string cpart_VCD_Dump_Mem::WritePreferences(void) {
    char prefs[256];
    sprintf(prefs, "%hu,%hhu,%hhu", mem_addr, mem_size, rec);
    return prefs;
}

void cpart_VCD_Dump_Mem::ReadPreferences(std::string value) {
    sscanf(value.c_str(), "%hu,%hhu,%hhu", &mem_addr, &mem_size, &rec);
    rec &= 0x01;
    if (mem_size % 8) {
        mem_size = 8;
    }
}

void cpart_VCD_Dump_Mem::ConfigurePropertiesWindow(void) {
    SpareParts.WPropCmd("spin1", PWA_SPINSETMIN, "0");
    SpareParts.WPropCmd("spin1", PWA_SPINSETMAX, std::to_string(pboard->DBGGetRAMSize()).c_str());
    SpareParts.WPropCmd("spin1", PWA_SPINSETVALUE, std::to_string(mem_addr).c_str());

    SpareParts.WPropCmd("combo2", PWA_COMBOSETITEMS, "8, 16,");
    SpareParts.WPropCmd("combo2", PWA_COMBOSETTEXT, std::to_string(mem_size).c_str());
}

void cpart_VCD_Dump_Mem::ReadPropertiesWindow(void) {
    int value;
    SpareParts.WPropCmd("spin1", PWA_SPINGETVALUE, NULL, &value);
    mem_addr = value;

    char buff[64];
    SpareParts.WPropCmd("combo2", PWA_COMBOGETTEXT, NULL, buff);
    sscanf(buff, "%hhu", &mem_size);
}

void cpart_VCD_Dump_Mem::PreProcess(void) {
    memset(output_bits_alm, 0, 8 * sizeof(unsigned long));
    JUMPSTEPS_ = PICSimLab.GetJUMPSTEPS() * 4.0 / PICSimLab.GetBoard()->MGetClocksPerInstructions();
    mcount = JUMPSTEPS_;

    if ((pboard->DBGGetRAM_p() != NULL) && (mem_addr < pboard->DBGGetRAMSize())) {
        mem_ptr = (unsigned short*)&(pboard->DBGGetRAM_p()[mem_addr]);
        mem_old = (*mem_ptr) ^ 0xFF;
    } else {
        mem_ptr = NULL;
        mem_old = 0;
    }

    if (rec && (f_vcd == NULL)) {
        float tscale = 1.0e12 / pboard->MGetInstClockFreq();  // ps step

        f_vcd = fopen_UTF8(f_vcd_name, "w");
        vcd_count = 0;

        fprintf(f_vcd,
                "$version Generated by PICSimLab $end\n"
                "$timescale %ips $end\n"
                "$scope module logic $end\n",
                (int)tscale);

        fprintf(f_vcd, "$var wire 1 !  mem[%d][0] $end\n", mem_addr);
        fprintf(f_vcd, "$var wire 1 $  mem[%d][1] $end\n", mem_addr);
        fprintf(f_vcd, "$var wire 1 %%  mem[%d][2] $end\n", mem_addr);
        fprintf(f_vcd, "$var wire 1 &  mem[%d][3] $end\n", mem_addr);
        fprintf(f_vcd, "$var wire 1 [  mem[%d][4] $end\n", mem_addr);
        fprintf(f_vcd, "$var wire 1 (  mem[%d][5] $end\n", mem_addr);
        fprintf(f_vcd, "$var wire 1 )  mem[%d][6] $end\n", mem_addr);
        fprintf(f_vcd, "$var wire 1 ]  mem[%d][7] $end\n", mem_addr);

        if (mem_size == 16) {
            fprintf(f_vcd, "$var wire 1 +  mem[%d][8] $end\n", mem_addr);
            fprintf(f_vcd, "$var wire 1 *  mem[%d][9] $end\n", mem_addr);
            fprintf(f_vcd, "$var wire 1 {  mem[%d][10] $end\n", mem_addr);
            fprintf(f_vcd, "$var wire 1 }  mem[%d][11] $end\n", mem_addr);
            fprintf(f_vcd, "$var wire 1 <  mem[%d][12] $end\n", mem_addr);
            fprintf(f_vcd, "$var wire 1 >  mem[%d][13] $end\n", mem_addr);
            fprintf(f_vcd, "$var wire 1 '  mem[%d][14] $end\n", mem_addr);
            fprintf(f_vcd, "$var wire 1 ^  mem[%d][15] $end\n", mem_addr);
        }

        fprintf(f_vcd,
                "$upscope $end\n"
                "$enddefinitions $end\n"
                "$dumpvars\n");

        fprintf(f_vcd, "x!\n");
        fprintf(f_vcd, "x$\n");
        fprintf(f_vcd, "x%%\n");
        fprintf(f_vcd, "x&\n");
        fprintf(f_vcd, "x[\n");
        fprintf(f_vcd, "x(\n");
        fprintf(f_vcd, "x)\n");
        fprintf(f_vcd, "x[\n");
        if (mem_size == 16) {
            fprintf(f_vcd, "x+\n");
            fprintf(f_vcd, "x*\n");
            fprintf(f_vcd, "x{\n");
            fprintf(f_vcd, "x}\n");
            fprintf(f_vcd, "x<\n");
            fprintf(f_vcd, "x>\n");
            fprintf(f_vcd, "x'\n");
            fprintf(f_vcd, "x^\n");
        }
        fprintf(f_vcd, "$end\n");
    } else if (!rec && (f_vcd != NULL)) {
        if (f_vcd) {
            fclose(f_vcd);
            f_vcd = NULL;
        }
    }
}

void cpart_VCD_Dump_Mem::Process(void) {
    if (rec && f_vcd) {
        vcd_count++;
        int tprint = 0;

        if ((mem_ptr != NULL) && ((*mem_ptr) ^ mem_old)) {
            for (int i = 0; i < mem_size; i++) {
                if (((*mem_ptr) & (1 << i)) != (mem_old & (1 << i))) {
                    if (!tprint) {
                        tprint = 1;
                        fprintf(f_vcd, "#%li\n", vcd_count);
                    }
                    int val = ((*mem_ptr) & (1 << i)) > 0;
                    if (val) {
                        mem_old |= (1 << i);
                    } else {
                        mem_old &= ~(1 << i);
                    }
                    fprintf(f_vcd, "%i%c\n", val, markers[i]);
                    fflush(f_vcd);
                }
            }
        }
    } else {
        if (mem_ptr != NULL) {
            mem_old = (*mem_ptr);
        } else {
            mem_old = 0;
        }
    }

    mcount++;
    if (mcount >= JUMPSTEPS_) {
        if (mem_old & 0x01)
            output_bits_alm[0]++;
        if (mem_old & 0x02)
            output_bits_alm[1]++;
        if (mem_old & 0x04)
            output_bits_alm[2]++;
        if (mem_old & 0x08)
            output_bits_alm[3]++;
        if (mem_old & 0x10)
            output_bits_alm[4]++;
        if (mem_old & 0x20)
            output_bits_alm[5]++;
        if (mem_old & 0x40)
            output_bits_alm[6]++;
        if (mem_old & 0x80)
            output_bits_alm[7]++;

        mcount = -1;
    }
}

void cpart_VCD_Dump_Mem::PostProcess(void) {
    long int NSTEPJ = PICSimLab.GetNSTEPJ();

    output_bits[0] = (((output_bits_alm[0] * 200.0) / NSTEPJ) + 55);
    output_bits[1] = (((output_bits_alm[1] * 200.0) / NSTEPJ) + 55);
    output_bits[2] = (((output_bits_alm[2] * 200.0) / NSTEPJ) + 55);
    output_bits[3] = (((output_bits_alm[3] * 200.0) / NSTEPJ) + 55);
    output_bits[4] = (((output_bits_alm[4] * 200.0) / NSTEPJ) + 55);
    output_bits[5] = (((output_bits_alm[5] * 200.0) / NSTEPJ) + 55);
    output_bits[6] = (((output_bits_alm[6] * 200.0) / NSTEPJ) + 55);
    output_bits[7] = (((output_bits_alm[7] * 200.0) / NSTEPJ) + 55);

    for (int i = 0; i < 8; i++) {
        if ((output_ids[O_L1 + i]->value != output_bits[i])) {
            output_ids[O_L1 + i]->value = output_bits[i];
            output_ids[O_L1 + i]->update = 1;
        }
    }
}

void cpart_VCD_Dump_Mem::OnMouseButtonPress(unsigned int inputId, unsigned int button, unsigned int x, unsigned int y,
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

part_init(PART_VCD_DUMP_MEM_Name, cpart_VCD_Dump_Mem, "Virtual");
