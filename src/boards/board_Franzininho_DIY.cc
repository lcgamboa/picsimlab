/* ########################################################################

   PICSimLab - Programmable IC Simulator Laboratory

   ########################################################################

   Copyright (c) : 2015-2025  Luis Claudio Gambôa Lopes <lcgamboa@yahoo.com>

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

// include files
#include "board_Franzininho_DIY.h"
#include "../lib/oscilloscope.h"
#include "../lib/picsimlab.h"
#include "../lib/spareparts.h"

/* ids of inputs of input map*/
enum {
    I_ICSP,  // ICSP connector
    I_PWR,   // Power button
    I_RST    // Reset button
};

/* ids of outputs of output map*/
enum {
    O_L,     // LED
    O_ON,    // Power LED
    O_RST,   // Reset button
    O_PB0,   // PIN PB0
    O_PB1,   // PIN PB1
    O_PB2,   // PIN PB2
    O_PB3,   // PIN PB3
    O_PB4,   // PIN PB4
    O_PB5,   // PIN PB5
    O_PPB0,  // PIN PB0
    O_PPB1,  // PIN PB1
    O_PPB2,  // PIN PB2
    O_PPB3,  // PIN PB3
    O_PPB4,  // PIN PB4
    O_PPB5   // PIN PB5
};
// return the input ids numbers of names used in input map

unsigned short cboard_Franzininho_DIY::GetInputId(char* name) {
    if (strcmp(name, "PG_ICSP") == 0)
        return I_ICSP;
    if (strcmp(name, "SW_PWR") == 0)
        return I_PWR;
    if (strcmp(name, "PB_RST_I") == 0)
        return I_RST;

    printf("Error input '%s' don't have a valid id! \n", name);
    return INVALID_ID;
}

// return the output ids numbers of names used in output map

unsigned short cboard_Franzininho_DIY::GetOutputId(char* name) {
    if (strcmp(name, "LD_L") == 0)
        return O_L;
    if (strcmp(name, "LD_ON") == 0)
        return O_ON;
    if (strcmp(name, "PB_RST_O") == 0)
        return O_RST;
    if (strcmp(name, "P_PB0") == 0)
        return O_PPB0;
    if (strcmp(name, "P_PB1") == 0)
        return O_PPB1;
    if (strcmp(name, "P_PB2") == 0)
        return O_PPB2;
    if (strcmp(name, "P_PB3") == 0)
        return O_PPB3;
    if (strcmp(name, "P_PB4") == 0)
        return O_PPB4;
    if (strcmp(name, "P_PB5") == 0)
        return O_PPB5;
    if (strcmp(name, "BP_0") == 0)
        return O_PB0;
    if (strcmp(name, "BP_1") == 0)
        return O_PB1;
    if (strcmp(name, "BP_2") == 0)
        return O_PB2;
    if (strcmp(name, "BP_3") == 0)
        return O_PB3;
    if (strcmp(name, "BP_4") == 0)
        return O_PB5;
    if (strcmp(name, "BP_5") == 0)
        return O_PB4;

    printf("Error output '%s' don't have a valid id! \n", name);
    return INVALID_ID;
}

// Constructor called once on board creation

cboard_Franzininho_DIY::cboard_Franzininho_DIY(void) {
    Proc = "attiny85";  // default microcontroller if none defined in preferences
    ReadMaps();         // Read input and output board maps
}

// Destructor called once on board destruction

cboard_Franzininho_DIY::~cboard_Franzininho_DIY(void) {}

// Reset board status

void cboard_Franzininho_DIY::Reset(void) {
    MReset(0);

    PICSimLab.UpdateStatus(PS_SERIAL, "");

    if (use_spare)
        SpareParts.Reset();

    RegisterRemoteControl();
}

void cboard_Franzininho_DIY::RegisterRemoteControl(void) {
    output_ids[O_L]->status = &pins[5].oavalue;
}

// Called ever 1s to refresh status

void cboard_Franzininho_DIY::RefreshStatus(void) {
    if (avr) {
        switch (avr->state) {
            case cpu_Limbo:
                PICSimLab.SetCpuState(CPU_ERROR);
                break;
            case cpu_Stopped:
                PICSimLab.SetCpuState(CPU_HALTED);
                break;
            case cpu_Running:
                PICSimLab.SetCpuState(CPU_RUNNING);
                break;
            case cpu_Sleeping:
                PICSimLab.SetCpuState(CPU_HALTED);
                break;
            case cpu_Step:
                PICSimLab.SetCpuState(CPU_STEPPING);
                break;
            case cpu_StepDone:
                PICSimLab.SetCpuState(CPU_STEPPING);
                break;
            case cpu_Done:
                PICSimLab.SetCpuState(CPU_HALTED);
                break;
            case cpu_Crashed:
                PICSimLab.SetCpuState(CPU_ERROR);
                break;
        }
    }
}

// Called to save board preferences in configuration file

void cboard_Franzininho_DIY::WritePreferences(void) {
    // write selected microcontroller of board_x to preferences
    PICSimLab.SavePrefs("Franzininho_DIY_proc", Proc);
    PICSimLab.SavePrefs("Franzininho_DIY_clock", FloatStrFormat("%2.1f", PICSimLab.GetClock()));
}

// Called whe configuration file load  preferences

void cboard_Franzininho_DIY::ReadPreferences(char* name, char* value) {
    // read microcontroller of preferences
    if (!strcmp(name, "Franzininho_DIY_proc")) {
        Proc = value;
    }

    if (!strcmp(name, "Franzininho_DIY_clock")) {
        PICSimLab.SetClock(atof(value));
    }
}

// Event on the board

void cboard_Franzininho_DIY::EvKeyPress(unsigned int key, unsigned int mask) {}

// Event on the board

void cboard_Franzininho_DIY::EvKeyRelease(unsigned int key, unsigned int mask) {}

// Event on the board

void cboard_Franzininho_DIY::EvMouseButtonPress(unsigned int button, unsigned int x, unsigned int y,
                                                unsigned int state) {
    int i;

    // search for the input area which owner the event
    for (i = 0; i < inputc; i++) {
        if (((input[i].x1 <= x) && (input[i].x2 >= x)) && ((input[i].y1 <= y) && (input[i].y2 >= y))) {
            switch (input[i].id) {
                    // if event is over I_ISCP area then load hex file
                case I_ICSP:
                    PICSimLab.OpenLoadHexFileDialog();
                    break;
                    // if event is over I_PWR area then toggle board on/off
                case I_PWR:
                    if (PICSimLab.GetMcuPwr())  // if on turn off
                    {
                        PICSimLab.SetMcuPwr(0);
                        Reset();
                    } else  // if off turn on
                    {
                        PICSimLab.SetMcuPwr(1);
                        Reset();
                    }
                    output_ids[O_ON]->update = 1;
                    break;
                    // if event is over I_RST area then turn off and reset
                case I_RST:
                    if (PICSimLab.GetMcuPwr())  // if powered
                    {
                        PICSimLab.SetMcuPwr(0);
                        PICSimLab.SetMcuRst(1);
                    }
                    p_RST = 0;
                    output_ids[O_RST]->update = 1;
                    break;
            }
        }
    }
}

// Event on the board

void cboard_Franzininho_DIY::EvMouseButtonRelease(unsigned int button, unsigned int x, unsigned int y,
                                                  unsigned int state) {
    int i;

    // search for the input area which owner the event
    for (i = 0; i < inputc; i++) {
        if (((input[i].x1 <= x) && (input[i].x2 >= x)) && ((input[i].y1 <= y) && (input[i].y2 >= y))) {
            switch (input[i].id) {
                    // if event is over I_RST area then turn on
                case I_RST:
                    if (PICSimLab.GetMcuRst())  // if powered
                    {
                        PICSimLab.SetMcuPwr(1);
                        PICSimLab.SetMcuRst(0);

                        Reset();
                    }
                    p_RST = 1;
                    output_ids[O_RST]->update = 1;
                    break;
            }
        }
    }
}

// Called ever 100ms to draw board
// This is the critical code for simulator running speed

void cboard_Franzininho_DIY::Draw(void) {
    int i;
    int update = 0;  // verifiy if updated is needed

    // board  draw
    for (i = 0; i < outputc; i++)  // run over all outputs
    {
        if (output[i].update)  // only if need update
        {
            output[i].update = 0;

            if (!update) {
                PICSimLab.CanvasCmd({.cmd = CC_INIT, .Init{Scale, Scale, 0}});
                PICSimLab.CanvasCmd({.cmd = CC_SETFONTWEIGHT, .SetFontWeight{CC_FONTWEIGHT_BOLD}});
            }
            update++;  // set to update buffer

            if (!output[i].r)  // if output shape is a rectangle
            {
                switch (output[i].id) {
                    case O_RST:
                        PICSimLab.CanvasCmd({.cmd = CC_SETCOLOR, .SetColor{100, 100, 100}});
                        break;
                    case O_PB0:
                    case O_PPB0:
                        PICSimLab.CanvasCmd({.cmd = CC_SETCOLOR, .SetColor{(unsigned int)pins[4].oavalue, 0, 0}});
                        break;
                    case O_PB1:
                    case O_PPB1:
                        PICSimLab.CanvasCmd({.cmd = CC_SETCOLOR, .SetColor{(unsigned int)pins[5].oavalue, 0, 0}});
                        break;
                    case O_PB2:
                    case O_PPB2:
                        PICSimLab.CanvasCmd({.cmd = CC_SETCOLOR, .SetColor{(unsigned int)pins[6].oavalue, 0, 0}});
                        break;
                    case O_PB3:
                    case O_PPB3:
                        PICSimLab.CanvasCmd({.cmd = CC_SETCOLOR, .SetColor{(unsigned int)pins[1].oavalue, 0, 0}});
                        break;
                    case O_PB4:
                    case O_PPB4:
                        PICSimLab.CanvasCmd({.cmd = CC_SETCOLOR, .SetColor{(unsigned int)pins[2].oavalue, 0, 0}});
                        break;
                    case O_PB5:
                    case O_PPB5:
                        PICSimLab.CanvasCmd({.cmd = CC_SETCOLOR, .SetColor{(unsigned int)pins[0].oavalue, 0, 0}});
                        break;
                    default:
                        PICSimLab.CanvasCmd({.cmd = CC_SETCOLOR, .SetColor{0, 0, 0}});
                        break;
                }

                if (output[i].id == O_RST) {
                    PICSimLab.CanvasCmd({.cmd = CC_CIRCLE, .Circle{1, output[i].cx, output[i].cy, 11}});
                    if (p_RST) {
                        PICSimLab.CanvasCmd({.cmd = CC_SETCOLOR, .SetColor{15, 15, 15}});
                    } else {
                        PICSimLab.CanvasCmd({.cmd = CC_SETCOLOR, .SetColor{55, 55, 55}});
                    }
                    PICSimLab.CanvasCmd({.cmd = CC_CIRCLE, .Circle{1, output[i].cx, output[i].cy, 9}});
                } else {
                    PICSimLab.CanvasCmd({.cmd = CC_RECTANGLE,
                                         .Rectangle{1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1,
                                                    output[i].y2 - output[i].y1}});
                }
            }

            else  // circle
            {
                PICSimLab.CanvasCmd({.cmd = CC_SETFGCOLOR, .SetFgColor{0, 0, 0}});
                switch (output[i].id) {
                    case O_ON:
                        PICSimLab.CanvasCmd({.cmd = CC_SETBGCOLOR,
                                             .SetBgColor{0, (unsigned int)(200 * PICSimLab.GetMcuPwr() + 55), 0}});
                        break;
                    case O_L:
                        PICSimLab.CanvasCmd(
                            {.cmd = CC_SETBGCOLOR,
                             .SetBgColor{(unsigned int)pins[5].oavalue, (unsigned int)pins[5].oavalue, 0}});
                        break;
                    default:
                        PICSimLab.CanvasCmd({.cmd = CC_SETBGCOLOR, .SetBgColor{0, 0, 0}});
                        break;
                }

                DrawLED(PICSimLab.CanvasCmd, &output[i]);
            }
        }
    }

    // end draw
    if (update) {
        PICSimLab.CanvasCmd({.cmd = CC_END});
    }
}

void cboard_Franzininho_DIY::Run_CPU(void) {
    int i;
    unsigned char pi;
    const picpin* pins;
    unsigned int alm[40];

    const int pinc = MGetPinCount();
    const long int NSTEP = 4.0 * PICSimLab.GetNSTEP();  // number of steps in 100ms
    const float RNSTEP = 200.0 * pinc / NSTEP;

    long long unsigned int cycle_start;
    int twostep = 0;

    // reset mean value

    memset(alm, 0, pinc * sizeof(unsigned int));

    // read pic.pins to a local variable to speed up

    pins = MGetPinsValues();

    if (use_spare)
        SpareParts.PreProcess();

    pi = 0;
    if (PICSimLab.GetMcuPwr())       // if powered
        for (i = 0; i < NSTEP; i++)  // repeat for number of steps in 100ms
        {
            // verify if a breakpoint is reached if not run one instruction
            if (avr_debug_type || (!mplabxd_testbp())) {
                if (twostep) {
                    twostep = 0;  // NOP
                } else {
                    cycle_start = avr->cycle;
                    avr_run(avr);
                    if ((avr->cycle - cycle_start) > 1) {
                        twostep = 1;
                    }
                    // TinyDebug support
                    if (avr->data[TDDR]) {
                        printf("%c", avr->data[TDDR]);
                        serial_port_send(serialfd, avr->data[TDDR]);
                        avr->data[TDDR] = 0;
                    }
                }
            }
            InstCounterInc();
            UpdateHardware();

            if (use_oscope)
                Oscilloscope.SetSample();
            if (use_spare)
                SpareParts.Process();
            ioupdated = 0;

            // increment mean value counter if pin is high
            alm[pi] += pins[pi].value;
            pi++;
            if (pi == pinc)
                pi = 0;
        }

    // calculate mean value
    for (pi = 0; pi < MGetPinCount(); pi++) {
        cboard_Franzininho_DIY::pins[pi].oavalue = (int)((alm[pi] * RNSTEP) + 55);
    }

    if (use_spare)
        SpareParts.PostProcess();

    // verifiy if LEDS need update
    if (output_ids[O_PB0]->value != pins[4].oavalue) {
        output_ids[O_PB0]->value = pins[4].oavalue;
        output_ids[O_PB0]->update = 1;
        output_ids[O_PPB0]->update = 1;
    }

    if (output_ids[O_PB1]->value != pins[5].oavalue) {
        output_ids[O_PB1]->value = pins[5].oavalue;
        output_ids[O_PB1]->update = 1;
        output_ids[O_PPB1]->update = 1;
        output_ids[O_L]->update = 1;
    }

    if (output_ids[O_PB2]->value != pins[6].oavalue) {
        output_ids[O_PB2]->value = pins[6].oavalue;
        output_ids[O_PB2]->update = 1;
        output_ids[O_PPB2]->update = 1;
    }

    if (output_ids[O_PB3]->value != pins[1].oavalue) {
        output_ids[O_PB3]->value = pins[1].oavalue;
        output_ids[O_PB3]->update = 1;
        output_ids[O_PPB3]->update = 1;
    }

    if (output_ids[O_PB4]->value != pins[2].oavalue) {
        output_ids[O_PB4]->value = pins[2].oavalue;
        output_ids[O_PB4]->update = 1;
        output_ids[O_PPB4]->update = 1;
    }

    if (output_ids[O_PB5]->value != pins[0].oavalue) {
        output_ids[O_PB5]->value = pins[0].oavalue;
        output_ids[O_PB5]->update = 1;
        output_ids[O_PPB5]->update = 1;
    }
}

board_init(BOARD_Franzininho_DIY_Name, cboard_Franzininho_DIY);
