/* ########################################################################

   PICSimLab - Programmable IC Simulator Laboratory

   ########################################################################

   Copyright (c) : 2019-2023  Luis Claudio Gambôa Lopes <lcgamboa@yahoo.com>

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

#include "virtual_VCD_Play.h"
#include "../lib/oscilloscope.h"
#include "../lib/picsimlab.h"
#include "../lib/spareparts.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

/* outputs */
enum { O_P1, O_P2, O_P3, O_P4, O_P5, O_P6, O_P7, O_P8, O_L1, O_L2, O_L3, O_L4, O_L5, O_L6, O_L7, O_L8, O_NAME, O_PLAY };

/*inputs*/
enum { I_PLAY, I_VIEW, I_LOAD };

static PCWProp pcwprop[9] = {{PCW_COMBO, "Pin 1"}, {PCW_COMBO, "Pin 2"}, {PCW_COMBO, "Pin 3"},
                             {PCW_COMBO, "Pin 4"}, {PCW_COMBO, "Pin 5"}, {PCW_COMBO, "Pin 6"},
                             {PCW_COMBO, "Pin 7"}, {PCW_COMBO, "Pin 8"}, {PCW_END, ""}};

cpart_VCD_Play::cpart_VCD_Play(const unsigned x, const unsigned y, const char* name, const char* type, board* pboard_)
    : part(x, y, name, type, pboard_), font(9, lxFONTFAMILY_TELETYPE, lxFONTSTYLE_NORMAL, lxFONTWEIGHT_BOLD) {
    always_update = 1;

    output_pins[0] = 0;
    output_pins[1] = 0;
    output_pins[2] = 0;
    output_pins[3] = 0;
    output_pins[4] = 0;
    output_pins[5] = 0;
    output_pins[6] = 0;
    output_pins[7] = 0;

    f_vcd_name[0] = '*';
    f_vcd_name[1] = 0;

    play = 0;

    vcd_data = NULL;
    vcd_data_count = 0;
    vcd_count = 0;
    vcd_ptr = 0;
    vcd_step = 0;

    SetPCWProperties(pcwprop);

    PinCount = 8;
    Pins = output_pins;
}

void cpart_VCD_Play::RegisterRemoteControl(void) {
    input_ids[I_PLAY]->status = &play;
    input_ids[I_PLAY]->update = &output_ids[O_PLAY]->update;
}

cpart_VCD_Play::~cpart_VCD_Play(void) {
    delete Bitmap;
    canvas.Destroy();
    if (vcd_data) {
        free(vcd_data);
        vcd_data = NULL;
    }
}

void cpart_VCD_Play::DrawOutput(const unsigned int i) {
    int to;
    int r, g, b;

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
            canvas.SetColor(49, 61, 99);
            canvas.Rectangle(1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);
            canvas.SetFgColor(255, 255, 255);
            if (output_pins[output[i].id - O_P1] == 0)
                canvas.RotatedText("NC", output[i].x1, output[i].y1, 0);
            else
                canvas.RotatedText(SpareParts.GetPinName(output_pins[output[i].id - O_P1]), output[i].x1, output[i].y1,
                                   0);
            break;
        case O_NAME:
            canvas.SetColor(49, 61, 99);
            canvas.Rectangle(1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);
            canvas.SetFgColor(255, 255, 255);
            to = strlen(f_vcd_name);
            if (to < 48) {
                to = 0;
            } else {
                to = to - 48;
            }
            canvas.RotatedText(f_vcd_name + to, output[i].x1, output[i].y1, 0);
            break;
        case O_L1:
        case O_L2:
        case O_L3:
        case O_L4:
        case O_L5:
        case O_L6:
        case O_L7:
        case O_L8:
            if (output_pins[output[i].id - O_L1] > 0) {
                canvas.SetColor(ppins[output_pins[output[i].id - O_L1] - 1].oavalue, 0, 0);
            } else {
                canvas.SetColor(30, 0, 0);
            }
            canvas.SetFgColor(0, 0, 0);
            // draw a circle
            color1 = canvas.GetBgColor();
            r = color1.Red() - 120;
            g = color1.Green() - 120;
            b = color1.Blue() - 120;
            if (r < 0)
                r = 0;
            if (g < 0)
                g = 0;
            if (b < 0)
                b = 0;
            color2.Set(r, g, b);
            canvas.SetBgColor(color2);
            canvas.Circle(1, output[i].x1, output[i].y1, output[i].r);
            canvas.SetBgColor(color1);
            canvas.Circle(1, output[i].x1, output[i].y1, output[i].r - 3);
            break;
        case O_PLAY:
            if (play > 0) {
                canvas.SetColor(0, 255, 0);
            } else {
                canvas.SetColor(255, 0, 0);
            }
            canvas.SetFgColor(0, 0, 0);
            // draw a circle
            color1 = canvas.GetBgColor();
            r = color1.Red() - 120;
            g = color1.Green() - 120;
            b = color1.Blue() - 120;
            if (r < 0)
                r = 0;
            if (g < 0)
                g = 0;
            if (b < 0)
                b = 0;
            color2.Set(r, g, b);
            canvas.SetBgColor(color2);
            canvas.Circle(1, output[i].x1, output[i].y1, output[i].r);
            canvas.SetBgColor(color1);
            canvas.Circle(1, output[i].x1, output[i].y1, output[i].r - 3);
            break;
    }
}

unsigned short cpart_VCD_Play::GetInputId(char* name) {
    if (strcmp(name, "PB_PLAY") == 0)
        return I_PLAY;
    if (strcmp(name, "PB_VIEW") == 0)
        return I_VIEW;
    if (strcmp(name, "PB_LOAD") == 0)
        return I_LOAD;

    printf("Error input '%s' don't have a valid id! \n", name);
    return INVALID_ID;
}

unsigned short cpart_VCD_Play::GetOutputId(char* name) {
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
        return O_PLAY;

    printf("Error output '%s' don't have a valid id! \n", name);
    return INVALID_ID;
}

lxString cpart_VCD_Play::WritePreferences(void) {
    char prefs[256];
    sprintf(prefs, "%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%s", output_pins[0], output_pins[1], output_pins[2],
            output_pins[3], output_pins[4], output_pins[5], output_pins[6], output_pins[7], play, f_vcd_name);

    return prefs;
}

void cpart_VCD_Play::ReadPreferences(lxString value) {
    sscanf(value.c_str(), "%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%s", &output_pins[0], &output_pins[1],
           &output_pins[2], &output_pins[3], &output_pins[4], &output_pins[5], &output_pins[6], &output_pins[7], &play,
           f_vcd_name);

    if (f_vcd_name[0] != '*') {
#ifdef _WIN_
        if (!strncmp(f_vcd_name, "/tmp/", 5)) {
            char buff[200];
            strcpy(buff, (const char*)lxGetTempDir("PICSimLab").c_str());
            strcat(buff, f_vcd_name + 4);
            strcpy(f_vcd_name, buff);
        }
#endif
        if (lxFileExists(f_vcd_name)) {
            LoadVCD(f_vcd_name);
        } else {
            printf("PICSimLab: VCD_Play Error loading from file: %s \n", f_vcd_name);
            PICSimLab.RegisterError(lxString("PICSimLab: VCD_Play Error loading from file: \n") + f_vcd_name);
            f_vcd_name[0] = '*';
            f_vcd_name[1] = 0;
            play = 0;
        }
    } else {
        play = 0;
    }
    Reset();
}

void cpart_VCD_Play::ConfigurePropertiesWindow(CPWindow* WProp) {
    SetPCWComboWithPinNames(WProp, "combo1", output_pins[0]);
    SetPCWComboWithPinNames(WProp, "combo2", output_pins[1]);
    SetPCWComboWithPinNames(WProp, "combo3", output_pins[2]);
    SetPCWComboWithPinNames(WProp, "combo4", output_pins[3]);
    SetPCWComboWithPinNames(WProp, "combo5", output_pins[4]);
    SetPCWComboWithPinNames(WProp, "combo6", output_pins[5]);
    SetPCWComboWithPinNames(WProp, "combo7", output_pins[6]);
    SetPCWComboWithPinNames(WProp, "combo8", output_pins[7]);
}

void cpart_VCD_Play::ReadPropertiesWindow(CPWindow* WProp) {
    output_pins[0] = GetPWCComboSelectedPin(WProp, "combo1");
    output_pins[1] = GetPWCComboSelectedPin(WProp, "combo2");
    output_pins[2] = GetPWCComboSelectedPin(WProp, "combo3");
    output_pins[3] = GetPWCComboSelectedPin(WProp, "combo4");
    output_pins[4] = GetPWCComboSelectedPin(WProp, "combo5");
    output_pins[5] = GetPWCComboSelectedPin(WProp, "combo6");
    output_pins[6] = GetPWCComboSelectedPin(WProp, "combo7");
    output_pins[7] = GetPWCComboSelectedPin(WProp, "combo8");
}

void cpart_VCD_Play::PreProcess(void) {
    vcd_inc = 1.0 / ((timescale * 1e-12) * pboard->MGetInstClockFreq());
}

void cpart_VCD_Play::Process(void) {
    if (play) {
        if (vcd_data[vcd_ptr].count <= vcd_count) {
            SpareParts.SetPin(output_pins[0], (vcd_data[vcd_ptr].data & 0x01) > 0);
            SpareParts.SetPin(output_pins[1], (vcd_data[vcd_ptr].data & 0x02) > 0);
            SpareParts.SetPin(output_pins[2], (vcd_data[vcd_ptr].data & 0x04) > 0);
            SpareParts.SetPin(output_pins[3], (vcd_data[vcd_ptr].data & 0x08) > 0);
            SpareParts.SetPin(output_pins[4], (vcd_data[vcd_ptr].data & 0x10) > 0);
            SpareParts.SetPin(output_pins[5], (vcd_data[vcd_ptr].data & 0x20) > 0);
            SpareParts.SetPin(output_pins[6], (vcd_data[vcd_ptr].data & 0x40) > 0);
            SpareParts.SetPin(output_pins[7], (vcd_data[vcd_ptr].data & 0x80) > 0);
            vcd_ptr++;
            if (vcd_ptr >= vcd_data_count) {
                vcd_ptr = 0;
                vcd_count = 0;
            }
        }

        vcd_step += vcd_inc;
        if (vcd_step >= 1) {
            vcd_count += (int)(vcd_step + 0.5);
            vcd_step = 0;
        }
    } else {
        if (vcd_count) {
            SpareParts.SetPin(output_pins[0], 0);
            SpareParts.SetPin(output_pins[1], 0);
            SpareParts.SetPin(output_pins[2], 0);
            SpareParts.SetPin(output_pins[3], 0);
            SpareParts.SetPin(output_pins[4], 0);
            SpareParts.SetPin(output_pins[5], 0);
            SpareParts.SetPin(output_pins[6], 0);
            SpareParts.SetPin(output_pins[7], 0);
        }

        vcd_count = 0;
        vcd_ptr = 0;
        vcd_step = 0;
    }
}

void cpart_VCD_Play::PostProcess(void) {
    const picpin* ppins = SpareParts.GetPinsValues();

    for (int i = 0; i < 8; i++) {
        if (output_pins[i] && (output_ids[O_L1 + i]->value != ppins[output_pins[i] - 1].oavalue)) {
            output_ids[O_L1 + i]->value = ppins[output_pins[i] - 1].oavalue;
            output_ids[O_L1 + i]->update = 1;
        }
    }
}

void cpart_VCD_Play::OnMouseButtonPress(uint inputId, uint button, uint x, uint y, uint state) {
    switch (inputId) {
        case I_LOAD:
            SpareParts.GetFileDialog()->SetType(lxFD_OPEN | lxFD_CHANGE_DIR);
            SpareParts.GetFileDialog()->SetFilter(lxT("Value change dump (*.vcd)|*.vcd"));
            if (f_vcd_name[0] == '*') {
                SpareParts.GetFileDialog()->SetFileName(lxT("untitled.vcd"));
            } else {
                SpareParts.GetFileDialog()->SetFileName(f_vcd_name);
            }
            SpareParts.Setfdtype(id);
            SpareParts.GetFileDialog()->Run();
            break;
        case I_PLAY:
            if (f_vcd_name[0] != '*') {
                play ^= 1;
            }
            output_ids[O_PLAY]->update = 1;
            break;
        case I_VIEW:
            if (f_vcd_name[0] == '*')
                break;
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
            lxLaunchDefaultApplication(f_vcd_name);
#endif
            break;
    }
}

void cpart_VCD_Play::filedialog_EvOnClose(int retId) {
    if (retId) {
        if ((SpareParts.GetFileDialog()->GetType() == (lxFD_OPEN | lxFD_CHANGE_DIR))) {
            strncpy(f_vcd_name, SpareParts.GetFileDialog()->GetFileName().c_str(), 199);
            LoadVCD(f_vcd_name);
        }
        output_ids[O_NAME]->update = 1;
    }
}

int cpart_VCD_Play::LoadVCD(lxString fname) {
    FILE* fvcd;
    fvcd = fopen(fname.c_str(), "r");
    char buff[256];
    int data = 0;
    char* id;
    char* value;
    char signal[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    int signal_count = 0;
    unsigned char old_play = play;

    play = 0;
    vcd_data_count = 0;

    unsigned int vcd_count_ = -1;

    if (vcd_data) {
        free(vcd_data);
        vcd_data = NULL;
    }

    if (fvcd) {
        while (fgets(buff, 255, fvcd)) {
            if (buff[0] == '#') {
                vcd_data_count++;
            }

            if (!data)  // option read
            {
                id = strtok(buff, " \n\r");
                if (!strcmp(id, "$timescale")) {
                    int itimescale;
                    value = strtok(NULL, " ");
                    sscanf(value, "%ips", &itimescale);
                    timescale = itimescale;
                } else if (!strcmp(id, "$var")) {
                    value = strtok(NULL, " ");  // wire
                    value = strtok(NULL, " ");  // 1
                    value = strtok(NULL, " ");  // const
                    signal[signal_count++] = value[0];
                } else if (!strcmp(id, "$end") || (id[0] == '#')) {
                    data = 1;
                }
            }
        }
        vcd_data_count--;

        vcd_data = (vcd_reg_t*)malloc((vcd_data_count + 1) * sizeof(vcd_reg_t));

        if (!vcd_data) {
            printf("vcd_play: malloc error \n");
            fclose(fvcd);
            return 0;
        }

        rewind(fvcd);

        data = 0;
        while (fgets(buff, 255, fvcd)) {
            if (!data)  // option read
            {
                id = strtok(buff, " \n\r");
                if (!strcmp(id, "$end") || (id[0] == '#')) {
                    data = 1;
                }
            } else  // data read
            {
                if (buff[0] == '#') {
                    vcd_count_++;
                    sscanf(buff + 1, "%lu", &vcd_data[vcd_count_].count);
                } else {
                    for (int i = 0; i < 8; i++) {
                        if (signal[i] == buff[1]) {
                            if (buff[0] == '1') {
                                vcd_data[vcd_count_].data |= (1 << i);
                                break;
                            } else {
                                vcd_data[vcd_count_].data &= ~(1 << i);
                                break;
                            }
                        }
                    }
                }
            }
        }
        fclose(fvcd);
        vcd_count = 0;
        vcd_ptr = 0;
        vcd_step = 0;
        play = old_play;
    } else {
        printf("vcd play: Error open file %s\n", (const char*)fname.c_str());
    }
    return 0;
}

part_init(PART_VCD_Play_Name, cpart_VCD_Play, "Virtual");
