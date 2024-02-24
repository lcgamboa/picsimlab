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

#include "output_Buzzer.h"
#include <math.h>
#include "../lib/oscilloscope.h"
#include "../lib/picsimlab.h"
#include "../lib/spareparts.h"

/* outputs */
enum { O_P1, O_P2, O_L1 };

enum { ACTIVE = 0, PASSIVE, TONE };

static PCWProp pcwprop[5] = {{PCW_COMBO, "Pin 1"},
                             {PCW_LABEL, "Pin2,GND"},
                             {PCW_COMBO, "Type"},
                             {PCW_COMBO, "Active"},
                             {PCW_END, ""}};

cpart_Buzzer::cpart_Buzzer(const unsigned x, const unsigned y, const char* name, const char* type, board* pboard_,
                           const int id_)
    : part(x, y, name, type, pboard_, id_) {
    X = x;
    Y = y;
    active = 1;
    always_update = 1;

    ReadMaps();

    LoadPartImage();

    input_pins[0] = 0;

    mcount = 0;

    buzzerId = PICSimLab.SystemCmd(PSC_AUDIOCHCREATE, NULL);
    btype = ACTIVE;

    PICSimLab.SystemCmd(PSC_AUDIOCHGETSAMPLERATE, (const char*)&buzzerId, &samplerate);
    buffersize = samplerate / 5;  // 0.1 seconds
    buffer = NULL;
    PICSimLab.SystemCmd(PSC_AUDIOCHGETMAX, (const char*)&buzzerId, &maxv);
    buffercount = 0;
    ctone = 0;
    ftone = 0;
    optone = 0;
    oftone = 0;

    in[0] = 0;
    in[1] = 0;
    in[2] = 0;

    out[0] = 0;
    out[1] = 0;
    out[2] = 0;

    SetPCWProperties(pcwprop);

    PinCount = 1;
    Pins = input_pins;
}

cpart_Buzzer::~cpart_Buzzer(void) {
    SpareParts.SetPartOnDraw(id);
    SpareParts.CanvasCmd({CC_FREEBITMAP, .FreeBitmap{BitmapId}});
    if (buffer) {
        delete[] buffer;
    }
    SpareParts.CanvasCmd({CC_DESTROY});
    PICSimLab.SystemCmd(PSC_AUDIOCHDESTROY, (const char*)&buzzerId);
}

void cpart_Buzzer::DrawOutput(const unsigned int i) {
    const picpin* ppins = SpareParts.GetPinsValues();

    SpareParts.CanvasCmd({CC_SETFGCOLOR, .SetFgColor{30, 0, 0}});

    switch (output[i].id) {
        case O_P1:
            SpareParts.CanvasCmd({CC_SETCOLOR, .SetColor{49, 61, 99}});
            SpareParts.CanvasCmd({CC_RECTANGLE, .Rectangle{1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1,
                                                           output[i].y2 - output[i].y1}});
            SpareParts.CanvasCmd({CC_SETFGCOLOR, .SetFgColor{255, 255, 255}});
            if (input_pins[output[i].id - O_P1] == 0)
                SpareParts.CanvasCmd({CC_ROTATEDTEXT, .RotatedText{"NC", output[i].x1, output[i].y1, 0}});
            else
                SpareParts.CanvasCmd(
                    {CC_ROTATEDTEXT, .RotatedText{SpareParts.GetPinName(input_pins[output[i].id - O_P1]).c_str(),
                                                  output[i].x1, output[i].y1, 0}});
            break;
        case O_P2:
            SpareParts.CanvasCmd({CC_SETCOLOR, .SetColor{49, 61, 99}});
            SpareParts.CanvasCmd({CC_RECTANGLE, .Rectangle{1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1,
                                                           output[i].y2 - output[i].y1}});
            SpareParts.CanvasCmd({CC_SETFGCOLOR, .SetFgColor{255, 255, 255}});
            SpareParts.CanvasCmd({CC_ROTATEDTEXT, .RotatedText{"GND", output[i].x1, output[i].y1, 0}});
            break;
        case O_L1:
            SpareParts.CanvasCmd({CC_SETFGCOLOR, .SetFgColor{0, 0, 0}});

            if (input_pins[0] > 0) {
                if (active) {
                    SpareParts.CanvasCmd(
                        {CC_SETBGCOLOR, .SetBgColor{(unsigned int)ppins[input_pins[0] - 1].oavalue, 0, 0}});
                } else {
                    SpareParts.CanvasCmd(
                        {CC_SETBGCOLOR, .SetBgColor{(unsigned int)(310 - ppins[input_pins[0] - 1].oavalue), 0, 0}});
                }
            } else {
                SpareParts.CanvasCmd({CC_SETBGCOLOR, .SetBgColor{55, 0, 0}});
            }
            DrawLED(SpareParts.CanvasCmd, &output[i]);
            break;
    }
}

unsigned short cpart_Buzzer::GetInputId(char* name) {
    printf("Error input '%s' don't have a valid id! \n", name);
    return INVALID_ID;
}

unsigned short cpart_Buzzer::GetOutputId(char* name) {
    if (strcmp(name, "PN_1") == 0)
        return O_P1;
    if (strcmp(name, "PN_2") == 0)
        return O_P2;
    if (strcmp(name, "LD_1") == 0)
        return O_L1;

    printf("Error output '%s' don't have a valid id! \n", name);
    return INVALID_ID;
};

std::string cpart_Buzzer::WritePreferences(void) {
    char prefs[256];

    sprintf(prefs, "%hhu,%hhu,%hhu", input_pins[0], btype, active);
    return prefs;
}

void cpart_Buzzer::ReadPreferences(std::string value) {
    unsigned char tp;
    sscanf(value.c_str(), "%hhu,%hhu,%hhu", &input_pins[0], &tp, &active);
    ChangeType(tp);
}

void cpart_Buzzer::RegisterRemoteControl(void) {
    const picpin* ppins = SpareParts.GetPinsValues();

    output_ids[O_L1]->status = (void*)&ppins[input_pins[0] - 1].oavalue;
}

void cpart_Buzzer::ConfigurePropertiesWindow(void) {
    SetPCWComboWithPinNames("combo1", input_pins[0]);

    SpareParts.WPropCmd("combo3", PWA_COMBOSETITEMS, "Active,Passive,Tone,");
    if (btype == ACTIVE)
        SpareParts.WPropCmd("combo3", PWA_COMBOSETTEXT, "Active");
    else if (btype == PASSIVE) {
        SpareParts.WPropCmd("combo3", PWA_COMBOSETTEXT, "Passive");
    } else  // TONE
    {
        SpareParts.WPropCmd("combo3", PWA_COMBOSETTEXT, "Tone");
    }

    SpareParts.WPropCmd("combo4", PWA_COMBOSETITEMS, "HIGH,LOW,");
    if (active)
        SpareParts.WPropCmd("combo4", PWA_COMBOSETTEXT, "HIGH");
    else
        SpareParts.WPropCmd("combo4", PWA_COMBOSETTEXT, "LOW ");
}

void cpart_Buzzer::ReadPropertiesWindow(void) {
    input_pins[0] = GetPWCComboSelectedPin("combo1");

    unsigned char tp = 0;

    char buff[64];
    SpareParts.WPropCmd("combo3", PWA_COMBOGETTEXT, NULL, buff);

    std::string mode = buff;
    if (mode.compare("Active") == 0) {
        tp = ACTIVE;
    } else if (mode.compare("Passive") == 0) {
        tp = PASSIVE;
    } else {  // TONE
        tp = TONE;
    }

    SpareParts.WPropCmd("combo4", PWA_COMBOGETTEXT, NULL, buff);

    active = (strcmp(buff, "HIGH") == 0);

    ChangeType(tp);
}

void cpart_Buzzer::PreProcess(void) {
    if (btype == PASSIVE) {
        int time;
        PICSimLab.WindowCmd(PW_MAIN, "timer1", PWA_TIMERGETTIME, NULL, &time);
        JUMPSTEPS_ = (pboard->MGetInstClockFreq() / samplerate);
        JUMPSTEPS_ *= ((float)BASETIMER) / time;  // Adjust to sample at the same time to the timer
        mcount = JUMPSTEPS_;
    } else if (btype == TONE) {
        JUMPSTEPS_ = (pboard->MGetInstClockFreq() / samplerate);
        mcount = JUMPSTEPS_;
        ctone = 0;
        optone = 0;
        ftone = 0;
    }
}

void cpart_Buzzer::Stop(void) {
    if ((btype == ACTIVE) || (btype == TONE)) {
        PICSimLab.SystemCmd(PSC_AUDIOCHBEEPSTOP, (const char*)&buzzerId);
    }
}

void cpart_Buzzer::Process(void) {
    if (btype == PASSIVE) {
        mcount++;
        if (mcount > JUMPSTEPS_) {
            if ((input_pins[0]) && (buffercount < buffersize)) {
                const picpin* ppins = SpareParts.GetPinsValues();

                /*
                   0.7837 z-1 - 0.7837 z-2
             y1:  ----------------------
                  1 - 1.196 z-1 + 0.2068 z-2
                 */
                in[2] = in[1];
                in[1] = in[0];
                if (active) {
                    in[0] = ((2.0 * ppins[input_pins[0] - 1].value) - 1.0) * maxv * 0.5;
                } else {
                    in[0] = ((2.0 * (ppins[input_pins[0] - 1].value == 0)) - 1.0) * maxv * 0.5;
                }
                out[2] = out[1];
                out[1] = out[0];
                out[0] = 0.7837 * in[1] - 0.7837 * in[2] + 1.196 * out[1] - 0.2068 * out[2];

                buffer[buffercount++] = out[0];
            }

            mcount = 0;
        }
    } else if (btype == TONE) {
        mcount++;
        if (mcount > JUMPSTEPS_) {
            if (input_pins[0]) {
                const picpin* ppins = SpareParts.GetPinsValues();

                if ((!optone) && (ppins[input_pins[0] - 1].value)) {
                    ftone = (ftone + ctone) / 2.0;
                    ctone = 0;
                }
                optone = ppins[input_pins[0] - 1].value;
                ctone++;
            }
            mcount = 0;
        }
    }
}

void cpart_Buzzer::PostProcess(void) {
    const picpin* ppins = SpareParts.GetPinsValues();

    if (btype == ACTIVE) {
        if (active) {
            if (ppins[input_pins[0] - 1].oavalue > 65) {
                PICSimLab.SystemCmd(PSC_AUDIOCHBEEPSTART, (const char*)&buzzerId);
            } else {
                PICSimLab.SystemCmd(PSC_AUDIOCHBEEPSTOP, (const char*)&buzzerId);
            }
        } else {
            if ((310 - ppins[input_pins[0] - 1].oavalue) > 215) {
                PICSimLab.SystemCmd(PSC_AUDIOCHBEEPSTART, (const char*)&buzzerId);
            } else {
                PICSimLab.SystemCmd(PSC_AUDIOCHBEEPSTOP, (const char*)&buzzerId);
            }
        }
    } else if (btype == PASSIVE) {
        buffer[buffercount - 1] = 0;
        short* ptr[2];
        ptr[0] = (short int*)&buffercount;
        ptr[1] = buffer;
        /*int ret =*/PICSimLab.SystemCmd(PSC_AUDIOCHSOUNDPLAY, (const char*)&buzzerId, ptr);
        // printf("ret=%i buffercount=%i sample=%i time=%f timer=%i\n", ret, buffercount, samplerate,
        //        ((float)(buffercount)) / samplerate, timer->GetTime());
        buffercount = 0;
    } else  // TONE
    {
        float freq;

        if (ftone > 5) {
            freq = samplerate / ftone;
        } else {
            freq = 0;
        }

        if (freq > 100) {
            if (fabs(oftone - freq) > 10.0) {
                PICSimLab.SystemCmd(PSC_AUDIOCHBEEPSTOP, (const char*)&buzzerId);
                float args[3] = {freq, 0.5, 1};
                PICSimLab.SystemCmd(PSC_AUDIOCHBEEPSTARTF, (const char*)&buzzerId, args);
                oftone = freq;
            }
        } else {
            PICSimLab.SystemCmd(PSC_AUDIOCHBEEPSTOP, (const char*)&buzzerId);
            oftone = 0;
        }
    }

    if (input_pins[0] && (output_ids[O_L1]->value != ppins[input_pins[0] - 1].oavalue)) {
        output_ids[O_L1]->value = ppins[input_pins[0] - 1].oavalue;
        output_ids[O_L1]->update = 1;
    }
}

void cpart_Buzzer::ChangeType(unsigned char tp) {
    if (btype == tp)
        return;

    if ((btype == ACTIVE) || (btype == TONE)) {
        PICSimLab.SystemCmd(PSC_AUDIOCHBEEPSTOP, (const char*)&buzzerId);
        if (!buffer) {
            buffer = new short[buffersize];
        }
        btype = tp;
    } else if (btype == PASSIVE) {
        if (buffer) {
            delete[] buffer;
            buffer = NULL;
        }
        btype = tp;
    }
}

part_init(PART_BUZZER_Name, cpart_Buzzer, "Output");
