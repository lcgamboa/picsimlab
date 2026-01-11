/* ########################################################################

   PICSimLab - Programmable IC Simulator Laboratory

   ########################################################################

   Copyright (c) : 2015-2026  Luis Claudio Gambôa Lopes <lcgamboa@yahoo.com>

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
#include "board_Arduino_Uno.h"
#include <math.h>
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
    O_L,   // LED
    O_RX,  // LED on PD0
    O_TX,  // LED on PD1
    O_ON,  // Power LED
    O_RST  // Reset button
};

enum { PWM0 = 0, PWM1, PWM2, PWM3, PWM4, PWM5 };

// return the input ids numbers of names used in input map

unsigned short cboard_Arduino_Uno::GetInputId(char* name) {
    if (strcmp(name, "PG_ICSP") == 0)
        return I_ICSP;
    if (strcmp(name, "SW_PWR") == 0)
        return I_PWR;
    if (strcmp(name, "PB_RST") == 0)
        return I_RST;

    printf("Error input '%s' don't have a valid id! \n", name);
    return INVALID_ID;
}

// return the output ids numbers of names used in output map

unsigned short cboard_Arduino_Uno::GetOutputId(char* name) {
    if (strcmp(name, "LD_L") == 0)
        return O_L;
    if (strcmp(name, "LD_TX") == 0)
        return O_TX;
    if (strcmp(name, "LD_RX") == 0)
        return O_RX;
    if (strcmp(name, "LD_ON") == 0)
        return O_ON;
    if (strcmp(name, "PB_RST") == 0)
        return O_RST;

    printf("Error output '%s' don't have a valid id! \n", name);
    return INVALID_ID;
}

// Constructor called once on board creation

cboard_Arduino_Uno::cboard_Arduino_Uno(void) {
    Proc = "atmega328p";  // default microcontroller if none defined in preferences
    ReadMaps();           // Read input and output board maps

    LED_pin = 18;

    PWM_pins[0] = 4;
    PWM_pins[1] = 10;
    PWM_pins[2] = 11;
    PWM_pins[3] = 14;
    PWM_pins[4] = 15;
    PWM_pins[5] = 16;

    // controls properties and creation

    PICSimLab.UpdateGUI(PWM0, GT_GAUGE, GA_ADD, (void*)"*3");
    PICSimLab.UpdateGUI(PWM1, GT_GAUGE, GA_ADD, (void*)"*5");
    PICSimLab.UpdateGUI(PWM2, GT_GAUGE, GA_ADD, (void*)"*6");
    PICSimLab.UpdateGUI(PWM3, GT_GAUGE, GA_ADD, (void*)"*9");
    PICSimLab.UpdateGUI(PWM4, GT_GAUGE, GA_ADD, (void*)"*10");
    PICSimLab.UpdateGUI(PWM5, GT_GAUGE, GA_ADD, (void*)"*11");
}

// Destructor called once on board destruction

cboard_Arduino_Uno::~cboard_Arduino_Uno(void) {
    // controls destruction
    PICSimLab.UpdateGUI(PWM0, GT_GAUGE, GA_DEL, NULL);
    PICSimLab.UpdateGUI(PWM1, GT_GAUGE, GA_DEL, NULL);
    PICSimLab.UpdateGUI(PWM2, GT_GAUGE, GA_DEL, NULL);
    PICSimLab.UpdateGUI(PWM3, GT_GAUGE, GA_DEL, NULL);
    PICSimLab.UpdateGUI(PWM4, GT_GAUGE, GA_DEL, NULL);
    PICSimLab.UpdateGUI(PWM5, GT_GAUGE, GA_DEL, NULL);
}

// Reset board status

void cboard_Arduino_Uno::Reset(void) {
    // write button state to pic pin 19 (RD0)
    // pic_set_pin(19,p_BT1);
    // write switch state to pic pin 20 (RD1)
    // pic_set_pin(20,p_BT2);
    MReset(0);

    // verify serial port state and refresh status bar
    PICSimLab.UpdateStatus(PS_SERIAL, GetUARTStrStatus(0));
    /*
      //reset mean value
      for(int pi=0;pi < pic.PINCOUNT;pi++)
      {
        lm[pi]=0;
      }
     */
    if (use_spare)
        SpareParts.Reset();

    RegisterRemoteControl();
}

void cboard_Arduino_Uno::RegisterRemoteControl(void) {
    output_ids[O_L]->status = &pins[LED_pin].oavalue;
}

// Called to save board preferences in configuration file

void cboard_Arduino_Uno::WritePreferences(void) {
    // write selected microcontroller of board_x to preferences
    PICSimLab.SavePrefs("Arduino_Uno_proc", Proc);
    PICSimLab.SavePrefs("Arduino_Uno_clock", FloatStrFormat("%2.1f", PICSimLab.GetClock()));
}

// Called whe configuration file load  preferences

void cboard_Arduino_Uno::ReadPreferences(char* name, char* value) {
    // read microcontroller of preferences
    if (!strcmp(name, "Arduino_Uno_proc")) {
        Proc = value;
    }

    if (!strcmp(name, "Arduino_Uno_clock")) {
        PICSimLab.SetClock(atof(value));
    }
}

// Event on the board

void cboard_Arduino_Uno::EvKeyPress(unsigned int key, unsigned int mask) {}

// Event on the board

void cboard_Arduino_Uno::EvKeyRelease(unsigned int key, unsigned int mask) {}

// Event on the board

void cboard_Arduino_Uno::EvMouseButtonPress(unsigned int button, unsigned int x, unsigned int y, unsigned int state) {
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

void cboard_Arduino_Uno::EvMouseButtonRelease(unsigned int button, unsigned int x, unsigned int y, unsigned int state) {
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

void cboard_Arduino_Uno::Draw(void) {
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
            update++;          // set to update buffer
            if (!output[i].r)  // if output shape is a rectangle
            {
                switch (output[i].id) {
                    case O_ON:
                        PICSimLab.CanvasCmd(
                            {.cmd = CC_SETCOLOR, .SetColor{0, (unsigned int)(200 * PICSimLab.GetMcuPwr() + 55), 0}});
                        break;
                    case O_RX:
                        PICSimLab.CanvasCmd(
                            {.cmd = CC_SETCOLOR,
                             .SetColor{0, (unsigned int)(255 - pins[1].oavalue) * PICSimLab.GetMcuPwr(), 0}});
                        break;
                    case O_TX:
                        PICSimLab.CanvasCmd(
                            {.cmd = CC_SETCOLOR,
                             .SetColor{
                                 0, (unsigned int)(255 - ((unsigned char)pins[2].oavalue * 10)) * PICSimLab.GetMcuPwr(),
                                 0}});
                        break;
                    case O_L:
                        PICSimLab.CanvasCmd({.cmd = CC_SETCOLOR, .SetColor{0, (unsigned int)pins[LED_pin].oavalue, 0}});
                        break;
                    case O_RST:
                        PICSimLab.CanvasCmd({.cmd = CC_SETCOLOR, .SetColor{100, 100, 100}});
                        break;
                    default:
                        PICSimLab.CanvasCmd({.cmd = CC_SETCOLOR, .SetColor{0, 0, 0}});
                        break;
                }

                if (output[i].id == O_RST) {
                    PICSimLab.CanvasCmd(
                        {.cmd = CC_CIRCLE,
                         .Circle{1, output[i].cx, output[i].cy, ((output[i].x2 - output[i].x1) / 2) - 5}});
                    if (p_RST) {
                        PICSimLab.CanvasCmd({.cmd = CC_SETCOLOR, .SetColor{15, 15, 15}});
                    } else {
                        PICSimLab.CanvasCmd({.cmd = CC_SETCOLOR, .SetColor{55, 55, 55}});
                    }
                    PICSimLab.CanvasCmd(
                        {.cmd = CC_CIRCLE,
                         .Circle{1, output[i].cx, output[i].cy, ((output[i].x2 - output[i].x1) / 2) - 7}});
                } else {
                    PICSimLab.CanvasCmd({.cmd = CC_RECTANGLE,
                                         .Rectangle{1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1,
                                                    output[i].y2 - output[i].y1}});
                }
            }
        }
    }

    // end draw
    if (update) {
        PICSimLab.CanvasCmd({.cmd = CC_END});
    }

    int value = (pins[PWM_pins[0]].oavalue - 55) / 2;
    PICSimLab.UpdateGUI(PWM0, GT_GAUGE, GA_SET, (void*)&value);
    value = (pins[PWM_pins[1]].oavalue - 55) / 2;
    PICSimLab.UpdateGUI(PWM1, GT_GAUGE, GA_SET, (void*)&value);
    value = (pins[PWM_pins[2]].oavalue - 55) / 2;
    PICSimLab.UpdateGUI(PWM2, GT_GAUGE, GA_SET, (void*)&value);
    value = (pins[PWM_pins[3]].oavalue - 55) / 2;
    PICSimLab.UpdateGUI(PWM3, GT_GAUGE, GA_SET, (void*)&value);
    value = (pins[PWM_pins[4]].oavalue - 55) / 2;
    PICSimLab.UpdateGUI(PWM4, GT_GAUGE, GA_SET, (void*)&value);
    value = (pins[PWM_pins[5]].oavalue - 55) / 2;
    PICSimLab.UpdateGUI(PWM5, GT_GAUGE, GA_SET, (void*)&value);
}

void cboard_Arduino_Uno::Run_CPU(void) {
    int i;
    // int j;
    unsigned char pi;
    const picpin* pins;
    unsigned int alm[100];

    // int JUMPSTEPS =
    // Window1.GetJUMPSTEPS
    // ()*4.0; //number of steps
    // skipped
    const int pinc = MGetPinCount();
    const long int NSTEP = 4.0 * PICSimLab.GetNSTEP();  // number
                                                        // of
                                                        // steps
                                                        // in
                                                        // 100ms
    const float RNSTEP = 200.0 * pinc / NSTEP;

    long long unsigned int cycle_start;
    int twostep = 0;

    // reset mean value

    memset(alm, 0, pinc * sizeof(unsigned int));

    // read pic.pins to a local
    // variable to speed up

    pins = MGetPinsValues();

    if (use_spare)
        SpareParts.PreProcess();

    // j = JUMPSTEPS; //step
    // counter
    pi = 0;
    if (PICSimLab.GetMcuPwr())       // if
                                     // powered
        for (i = 0; i < NSTEP; i++)  // repeat for
                                     // number of
                                     // steps in
                                     // 100ms
        {
            // verify if a
            // breakpoint is
            // reached if not
            // run one
            // instruction
            if (avr_debug_type || (!mplabxd_testbp())) {
                if (twostep) {
                    twostep = 0;  // NOP
                } else {
                    cycle_start = avr->cycle;
                    avr_run(avr);
                    if ((avr->cycle - cycle_start) > 1) {
                        twostep = 1;
                    }
                }
            }

            InstCounterInc();
            UpdateHardware();

            // avr->sleep_usec=0;
            if (use_oscope)
                Oscilloscope.SetSample();
            if (use_spare)
                SpareParts.Process();
            ioupdated = 0;

            // increment mean
            // value counter if
            // pin is high
            alm[pi] += pins[pi].value;
            pi++;
            if (pi == pinc)
                pi = 0;
            /*
                if (j >=
               JUMPSTEPS)//if
               number of step is
               bigger than steps
               to skip
                 {
                  //set analog
               pin 2 (AN0) with
               value from scroll
                  //pic_set_apin(2,((5.0*(scroll1->GetPosition()))/
                  //
               (scroll1->GetRange()-1)));

                  j = -1;
               //reset counter
                 }
                j++; //counter
               increment
             */
        }

    // calculate mean value
    for (pi = 0; pi < MGetPinCount(); pi++) {
        cboard_Arduino_Uno::pins[pi].oavalue = (int)((alm[pi] * RNSTEP) + 55);
    }

    if (use_spare)
        SpareParts.PostProcess();

    // verifiy if LEDS need
    // update
    if (output_ids[O_RX]->value != pins[1].oavalue) {
        output_ids[O_RX]->value = pins[1].oavalue;
        output_ids[O_RX]->update = 1;
    }
    if (output_ids[O_TX]->value != pins[2].oavalue) {
        output_ids[O_TX]->value = pins[2].oavalue;
        output_ids[O_TX]->update = 1;
    }
    if (output_ids[O_L]->value != pins[LED_pin].oavalue) {
        output_ids[O_L]->value = pins[LED_pin].oavalue;
        output_ids[O_L]->update = 1;
    }
}

board_init(BOARD_Arduino_Uno_Name, cboard_Arduino_Uno);
