/* ########################################################################

   PICsimLab - PIC laboratory simulator

   ########################################################################

   Copyright (c) : 2015-2020  Luis Claudio Gambôa Lopes

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
#include "../picsimlab1.h"
#include "../picsimlab4.h"  //Oscilloscope
#include "../picsimlab5.h"  //Spare Parts

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

unsigned short cboard_Franzininho_DIY::get_in_id(char* name) {
    if (strcmp(name, "PG_ICSP") == 0)
        return I_ICSP;
    if (strcmp(name, "SW_PWR") == 0)
        return I_PWR;
    if (strcmp(name, "PB_RST_I") == 0)
        return I_RST;

    printf("Erro input '%s' don't have a valid id! \n", name);
    return -1;
}

// return the output ids numbers of names used in output map

unsigned short cboard_Franzininho_DIY::get_out_id(char* name) {
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

    printf("Erro output '%s' don't have a valid id! \n", name);
    return 1;
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

    Window1.statusbar1.SetField(2, "");

    if (use_spare)
        Window5.Reset();

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
                Window1.SetCpuState(CPU_ERROR);
                break;
            case cpu_Stopped:
                Window1.SetCpuState(CPU_HALTED);
                break;
            case cpu_Running:
                Window1.SetCpuState(CPU_RUNNING);
                break;
            case cpu_Sleeping:
                Window1.SetCpuState(CPU_HALTED);
                break;
            case cpu_Step:
                Window1.SetCpuState(CPU_STEPPING);
                break;
            case cpu_StepDone:
                Window1.SetCpuState(CPU_STEPPING);
                break;
            case cpu_Done:
                Window1.SetCpuState(CPU_HALTED);
                break;
            case cpu_Crashed:
                Window1.SetCpuState(CPU_ERROR);
                break;
        }
    }
}

// Called to save board preferences in configuration file

void cboard_Franzininho_DIY::WritePreferences(void) {
    // write selected microcontroller of board_x to preferences
    Window1.saveprefs(lxT("Franzininho_DIY_proc"), Proc);
    Window1.saveprefs(lxT("Franzininho_DIY_clock"), lxString().Format("%2.1f", Window1.GetClock()));
}

// Called whe configuration file load  preferences

void cboard_Franzininho_DIY::ReadPreferences(char* name, char* value) {
    // read microcontroller of preferences
    if (!strcmp(name, "Franzininho_DIY_proc")) {
        Proc = value;
    }

    if (!strcmp(name, "Franzininho_DIY_clock")) {
        Window1.SetClock(atof(value));
    }
}

// Event on the board

void cboard_Franzininho_DIY::EvKeyPress(uint key, uint mask) {}

// Event on the board

void cboard_Franzininho_DIY::EvKeyRelease(uint key, uint mask) {}

// Event on the board

void cboard_Franzininho_DIY::EvMouseButtonPress(uint button, uint x, uint y, uint state) {
    int i;

    // search for the input area which owner the event
    for (i = 0; i < inputc; i++) {
        if (((input[i].x1 <= x) && (input[i].x2 >= x)) && ((input[i].y1 <= y) && (input[i].y2 >= y))) {
            switch (input[i].id) {
                    // if event is over I_ISCP area then load hex file
                case I_ICSP:
                    Window1.menu1_File_LoadHex_EvMenuActive(NULL);
                    break;
                    // if event is over I_PWR area then toggle board on/off
                case I_PWR:
                    if (Window1.Get_mcupwr())  // if on turn off
                    {
                        Window1.Set_mcupwr(0);
                        Reset();
                    } else  // if off turn on
                    {
                        Window1.Set_mcupwr(1);
                        Reset();
                    }
                    output_ids[O_ON]->update = 1;
                    break;
                    // if event is over I_RST area then turn off and reset
                case I_RST:
                    if (Window1.Get_mcupwr())  // if powered
                    {
                        Window1.Set_mcupwr(0);
                        Window1.Set_mcurst(1);
                    }
                    p_RST = 0;
                    output_ids[O_RST]->update = 1;
                    break;
            }
        }
    }
}

// Event on the board

void cboard_Franzininho_DIY::EvMouseButtonRelease(uint button, uint x, uint y, uint state) {
    int i;

    // search for the input area which owner the event
    for (i = 0; i < inputc; i++) {
        if (((input[i].x1 <= x) && (input[i].x2 >= x)) && ((input[i].y1 <= y) && (input[i].y2 >= y))) {
            switch (input[i].id) {
                    // if event is over I_RST area then turn on
                case I_RST:
                    if (Window1.Get_mcurst())  // if powered
                    {
                        Window1.Set_mcupwr(1);
                        Window1.Set_mcurst(0);

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

void cboard_Franzininho_DIY::Draw(CDraw* draw) {
    int i;
    int update = 0;  // verifiy if updated is needed

    // board  draw
    for (i = 0; i < outputc; i++)  // run over all outputs
    {
        if (output[i].update)  // only if need update
        {
            output[i].update = 0;

            if (!update) {
                draw->Canvas.Init(Scale, Scale);
            }
            update++;  // set to update buffer

            if (!output[i].r)  // if output shape is a rectangle
            {
                switch (output[i].id) {
                    case O_RST:
                        draw->Canvas.SetColor(100, 100, 100);
                        break;
                    case O_PB0:
                    case O_PPB0:
                        draw->Canvas.SetColor(pins[4].oavalue, 0, 0);
                        break;
                    case O_PB1:
                    case O_PPB1:
                        draw->Canvas.SetColor(pins[5].oavalue, 0, 0);
                        break;
                    case O_PB2:
                    case O_PPB2:
                        draw->Canvas.SetColor(pins[6].oavalue, 0, 0);
                        break;
                    case O_PB3:
                    case O_PPB3:
                        draw->Canvas.SetColor(pins[1].oavalue, 0, 0);
                        break;
                    case O_PB4:
                    case O_PPB4:
                        draw->Canvas.SetColor(pins[2].oavalue, 0, 0);
                        break;
                    case O_PB5:
                    case O_PPB5:
                        draw->Canvas.SetColor(pins[0].oavalue, 0, 0);
                        break;
                    default:
                        draw->Canvas.SetColor(0, 0, 0);
                        break;
                }

                if (output[i].id == O_RST) {
                    draw->Canvas.Circle(1, output[i].cx, output[i].cy, 11);
                    if (p_RST) {
                        draw->Canvas.SetColor(15, 15, 15);
                    } else {
                        draw->Canvas.SetColor(55, 55, 55);
                    }
                    draw->Canvas.Circle(1, output[i].cx, output[i].cy, 9);
                } else {
                    draw->Canvas.Rectangle(1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1,
                                           output[i].y2 - output[i].y1);
                }
            }

            else  // circle
            {
                draw->Canvas.SetFgColor(0, 0, 0);
                switch (output[i].id) {
                    case O_ON:
                        draw->Canvas.SetBgColor(0, 200 * Window1.Get_mcupwr() + 55, 0);
                        break;
                    case O_L:
                        draw->Canvas.SetBgColor(pins[5].oavalue, pins[5].oavalue, 0);
                        break;
                    default:
                        draw->Canvas.SetBgColor(0, 0, 0);
                        break;
                }

                // draw a LED
                color1 = draw->Canvas.GetBgColor();
                int r = color1.Red() - 120;
                int g = color1.Green() - 120;
                int b = color1.Blue() - 120;
                if (r < 0)
                    r = 0;
                if (g < 0)
                    g = 0;
                if (b < 0)
                    b = 0;
                color2.Set(r, g, b);
                draw->Canvas.SetBgColor(color2);
                draw->Canvas.Circle(1, output[i].x1, output[i].y1, output[i].r + 1);
                draw->Canvas.SetBgColor(color1);
                draw->Canvas.Circle(1, output[i].x1, output[i].y1, output[i].r - 2);
            }
        }
    }

    // end draw
    if (update) {
        draw->Canvas.End();
        draw->Update();
    }
}

void cboard_Franzininho_DIY::Run_CPU(void) {
    int i;
    unsigned char pi;
    const picpin* pins;
    unsigned int alm[40];

    const int pinc = MGetPinCount();
    const long int NSTEP = 4.0 * Window1.GetNSTEP();  // number of steps in 100ms
    const float RNSTEP = 200.0 * pinc / NSTEP;

    long long unsigned int cycle_start;
    int twostep = 0;

    // reset mean value

    memset(alm, 0, pinc * sizeof(unsigned int));

    // read pic.pins to a local variable to speed up

    pins = MGetPinsValues();

    if (use_spare)
        Window5.PreProcess();

    pi = 0;
    if (Window1.Get_mcupwr())        // if powered
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

            UpdateHardware();

            if (use_oscope)
                Window4.SetSample();
            if (use_spare)
                Window5.Process();
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
        Window5.PostProcess();

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
