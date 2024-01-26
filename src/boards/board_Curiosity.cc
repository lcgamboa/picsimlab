/* ########################################################################

   PICSimLab - Programmable IC Simulator Laboratory

   ########################################################################

   Copyright (c) : 2015-2024  Luis Claudio Gambôa Lopes <lcgamboa@yahoo.com>

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
#include "board_Curiosity.h"
#include "../lib/oscilloscope.h"
#include "../lib/picsimlab.h"
#include "../lib/spareparts.h"

/* ids of inputs of input map*/
enum {
    I_POT1,  // potentiometer
    I_ICSP,  // ICSP connector
    I_PWR,   // Power button
    I_RST,   // Reset button
    I_S1,    // S1 push button
    I_JMP    // JMP
};

/* ids of outputs of output map*/
enum {
    O_POT1,
    O_JMP,  // JMP
    O_D1,   // LED D1
    O_D2,   // LED D2
    O_D4,   // LED D4
    O_D5,   // LED D5
    O_D6,   // LED D6
    O_D7,   // LED D7
    O_RST,  // Reset button
    O_S1,   // S1 push button
    O_MP    // uController name
};

enum { PWM0 = 1, PWM1, PWM2, PWM3 };

// return the input ids numbers of names used in input map

unsigned short cboard_Curiosity::GetInputId(char* name) {
    if (strcmp(name, "PG_ICSP") == 0)
        return I_ICSP;
    if (strcmp(name, "SW_PWR") == 0)
        return I_PWR;
    if (strcmp(name, "PB_RST") == 0)
        return I_RST;
    if (strcmp(name, "PB_S1") == 0)
        return I_S1;
    if (strcmp(name, "JP_1") == 0)
        return I_JMP;
    if (strcmp(name, "PO_1") == 0)
        return I_POT1;

    printf("Error input '%s' don't have a valid id! \n", name);
    return INVALID_ID;
}

// return the output ids numbers of names used in output map

unsigned short cboard_Curiosity::GetOutputId(char* name) {
    if (strcmp(name, "JP_1") == 0)
        return O_JMP;
    if (strcmp(name, "LD_D1") == 0)
        return O_D1;
    if (strcmp(name, "LD_D2") == 0)
        return O_D2;
    if (strcmp(name, "LD_D4") == 0)
        return O_D4;
    if (strcmp(name, "LD_D5") == 0)
        return O_D5;
    if (strcmp(name, "LD_D6") == 0)
        return O_D6;
    if (strcmp(name, "LD_D7") == 0)
        return O_D7;
    if (strcmp(name, "PB_S1") == 0)
        return O_S1;
    if (strcmp(name, "PO_1") == 0)
        return O_POT1;
    if (strcmp(name, "PB_RST") == 0)
        return O_RST;
    if (strcmp(name, "IC_CPU") == 0)
        return O_MP;

    printf("Error output '%s' don't have a valid id! \n", name);
    return INVALID_ID;
}

// Constructor called once on board creation

cboard_Curiosity::cboard_Curiosity(void) {
    Proc = "PIC16F1619";  // default microcontroller if none defined in preferences
    ReadMaps();           // Read input and output board maps
    jmp[0] = 0;

    pic.vcc = 5.0;
    pot1 = 100;

    active = 0;

    PICSimLab.UpdateGUI(PWM0, GT_GAUGE, GA_ADD, (void*)"*RA5");
    PICSimLab.UpdateGUI(PWM1, GT_GAUGE, GA_ADD, (void*)"*RA1");
    PICSimLab.UpdateGUI(PWM2, GT_GAUGE, GA_ADD, (void*)"*RA2");
    PICSimLab.UpdateGUI(PWM3, GT_GAUGE, GA_ADD, (void*)"*RC5");
}

// Destructor called once on board destruction

cboard_Curiosity::~cboard_Curiosity(void) {
    PICSimLab.UpdateGUI(PWM0, GT_GAUGE, GA_DEL, NULL);
    PICSimLab.UpdateGUI(PWM1, GT_GAUGE, GA_DEL, NULL);
    PICSimLab.UpdateGUI(PWM2, GT_GAUGE, GA_DEL, NULL);
    PICSimLab.UpdateGUI(PWM3, GT_GAUGE, GA_DEL, NULL);
}

// Reset board status

void cboard_Curiosity::Reset(void) {
    pic_reset(&pic, 1);

    p_BT1 = 1;  // set push button  in default state (high)

    // write button state to pic pin 6 (RC4)
    pic_set_pin(&pic, 6, p_BT1);

    // verify serial port state and refresh status bar

    if (pic.serial[0].serialfd != INVALID_SERIAL)

        PICSimLab.UpdateStatus(
            PS_SERIAL,
            "Serial: " + std::string(SERIALDEVICE) + ":" + std::to_string(pic.serial[0].serialbaud) + "(" +
                FloatStrFormat("%4.1f", fabs((100.0 * pic.serial[0].serialexbaud - 100.0 * pic.serial[0].serialbaud) /
                                             pic.serial[0].serialexbaud)) +
                "%)");
    else
        PICSimLab.UpdateStatus(PS_SERIAL, "Serial: " + std::string(SERIALDEVICE) + " (ERROR)");

    if (jmp[0]) {
        pic.vcc = 5.0;
    } else {
        pic.vcc = 3.3;
    }

    if (use_spare)
        SpareParts.Reset();
    if (use_oscope)
        Oscilloscope.Reset();

    RegisterRemoteControl();
}

void cboard_Curiosity::RegisterRemoteControl(void) {
    input_ids[I_S1]->status = &p_BT1;
    input_ids[I_S1]->update = &output_ids[O_S1]->update;
    input_ids[I_POT1]->status = &pot1;
    input_ids[I_POT1]->update = &output_ids[O_POT1]->update;

    output_ids[O_D4]->status = &pic.pins[1].oavalue;
    output_ids[O_D5]->status = &pic.pins[17].oavalue;
    output_ids[O_D6]->status = &pic.pins[16].oavalue;
    output_ids[O_D7]->status = &pic.pins[4].oavalue;
}

// Called ever 1s to refresh status

void cboard_Curiosity::RefreshStatus(void) {
    // verify serial port state and refresh status bar

    if (pic.serial[0].serialfd != INVALID_SERIAL)
        PICSimLab.UpdateStatus(
            PS_SERIAL,
            "Serial: " + std::string(SERIALDEVICE) + ":" + std::to_string(pic.serial[0].serialbaud) + "(" +
                FloatStrFormat("%4.1f", fabs((100.0 * pic.serial[0].serialexbaud - 100.0 * pic.serial[0].serialbaud) /
                                             pic.serial[0].serialexbaud)) +
                "%)");
    else
        PICSimLab.UpdateStatus(PS_SERIAL, "Serial: " + std::string(SERIALDEVICE) + " (ERROR)");
}

// Called to save board preferences in configuration file

void cboard_Curiosity::WritePreferences(void) {
    // write selected microcontroller of board_5 to preferences
    PICSimLab.SavePrefs("Curiosity_proc", Proc);
    PICSimLab.SavePrefs("Curiosity_jmp", std::to_string(jmp[0]));
    PICSimLab.SavePrefs("Curiosity_clock", FloatStrFormat("%2.1f", PICSimLab.GetClock()));
    PICSimLab.SavePrefs("Curiosity_pot1", std::to_string(pot1));
}

// Called whe configuration file load  preferences

void cboard_Curiosity::ReadPreferences(char* name, char* value) {
    // read microcontroller of preferences
    if (!strcmp(name, "Curiosity_proc")) {
        Proc = value;
    }

    if (!strcmp(name, "Curiosity_jmp")) {
        int i;
        for (i = 0; i < 1; i++) {
            if (value[i] == '0')
                jmp[i] = 0;
            else
                jmp[i] = 1;
        }
    }

    if (!strcmp(name, "Curiosity_clock")) {
        PICSimLab.SetClock(atof(value));
    }

    if (!strcmp(name, "Curiosity_pot1")) {
        pot1 = atoi(value);
    }
}

// Event on the board

void cboard_Curiosity::EvKeyPress(uint key, uint mask) {
    // if keyboard key 1 is pressed then activate button (state=0)
    if (key == '1') {
        p_BT1 = 0;
    }
}

// Event on the board

void cboard_Curiosity::EvKeyRelease(uint key, uint mask) {
    // if keyboard key 1 is pressed then deactivate button (state=1)
    if (key == '1') {
        p_BT1 = 1;
    }
}

// Event on the board

void cboard_Curiosity::EvMouseButtonPress(uint button, uint x, uint y, uint state) {
    int i;

    // search for the input area which owner the event
    for (i = 0; i < inputc; i++) {
        if (((input[i].x1 <= x) && (input[i].x2 >= x)) && ((input[i].y1 <= y) && (input[i].y2 >= y))) {
            switch (input[i].id) {
                    // if event is over I_ISCP area then load hex file
                case I_ICSP:
                    PICSimLab.OpenLoadHexFileDialog();
                    ;
                    break;
                    // if event is over I_PWR area then toggle board on/off
                case I_PWR:
                    if (PICSimLab.GetMcuPwr())  // if on turn off
                    {
                        PICSimLab.SetMcuPwr(0);
                        Reset();
                        p_BT1 = 1;
                    } else  // if off turn on
                    {
                        PICSimLab.SetMcuPwr(1);
                        Reset();
                    }
                    break;
                    // if event is over I_RST area then turn off and reset
                case I_RST:
                    if (PICSimLab.GetMcuPwr() && pic_reset(&pic, -1))  // if powered
                    {
                        PICSimLab.SetMcuPwr(0);
                        PICSimLab.SetMcuRst(1);
                    }
                    p_RST = 0;
                    break;
                    // if event is over I_S1 area then activate button (state=0)
                case I_S1:
                    p_BT1 = 0;
                    break;
                case I_JMP:
                    jmp[0] ^= 0x01;
                    Reset();
                    break;
                case I_POT1: {
                    active = 1;
                    pot1 = CalcAngle(i, x, y);
                } break;
            }
        }
    }
}

void cboard_Curiosity::EvMouseMove(uint button, uint x, uint y, uint state) {
    int i;

    for (i = 0; i < inputc; i++) {
        switch (input[i].id) {
            case I_POT1:
                if (((input[i].x1 <= x) && (input[i].x2 >= x)) && ((input[i].y1 <= y) && (input[i].y2 >= y))) {
                    if (active) {
                        pot1 = CalcAngle(i, x, y);
                    }
                } else {
                    active = 0;
                }
                break;
        }
    }
}

// Event on the board

void cboard_Curiosity::EvMouseButtonRelease(uint button, uint x, uint y, uint state) {
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

                        if (pic_reset(&pic, -1)) {
                            Reset();
                        }
                    }
                    p_RST = 1;
                    break;
                    // if event is over I_S1 area then deactivate button (state=1)
                case I_S1:
                    p_BT1 = 1;
                    break;
                case I_POT1: {
                    active = 0;
                } break;
            }
        }
    }
}

// Called ever 100ms to draw board
// This is the critical code for simulator running speed

void cboard_Curiosity::Draw(CCanvas* Canvas) {
    int i;

    Canvas->Init(Scale, Scale);  // initialize draw context
    Canvas->SetFontWeight(lxFONTWEIGHT_BOLD);

    // board_5 draw
    for (i = 0; i < outputc; i++)  // run over all outputs
    {
        if (!output[i].r)  // if output shape is a rectangle
        {
            Canvas->SetFgColor(0, 0, 0);  // black

            switch (output[i].id)  // search for color of output
            {
                case O_D1:  // green using picpwr value
                    Canvas->SetColor(0, 200 * PICSimLab.GetMcuPwr() + 55, 0);
                    break;
                case O_D2:  // green using picpwr value
                    Canvas->SetColor(0, 200 * PICSimLab.GetMcuPwr() + 55, 0);
                    break;
                case O_D4:  // Red using pin 2 mean  value (RA5)
                    Canvas->SetColor(0, pic.pins[1].oavalue, 0);
                    break;
                case O_D5:  // Red using pin 18 mean value (RA1)
                    Canvas->SetColor(0, pic.pins[17].oavalue, 0);
                    break;
                case O_D6:  // Red using pin 17 mean value (RA2)
                    Canvas->SetColor(0, pic.pins[16].oavalue, 0);
                    break;
                case O_D7:  // Red using pin 5 mean value (RC5)
                    Canvas->SetColor(0, pic.pins[4].oavalue, 0);
                    break;
                case O_JMP:
                    Canvas->SetColor(150, 150, 150);
                    break;
                case O_S1:
                case O_RST:
                    Canvas->SetColor(100, 100, 100);
                    break;
                case O_MP:
                    Canvas->SetColor(26, 26, 26);
                    break;
            }

            if (output[i].id == O_MP) {
                lxRect rec;
                Canvas->SetFontSize(12);
                Canvas->Rectangle(1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1,
                                  output[i].y2 - output[i].y1);
                Canvas->SetFgColor(230, 230, 230);
                rec.x = output[i].x1;
                rec.y = output[i].y1;
                rec.width = output[i].x2 - output[i].x1;
                rec.height = output[i].y2 - output[i].y1;
                Canvas->TextOnRect(Proc, rec, lxALIGN_CENTER | lxALIGN_CENTER_VERTICAL);
            } else if (output[i].id == O_S1) {
                Canvas->Circle(1, output[i].cx, output[i].cy, 16);
                if (p_BT1) {
                    Canvas->SetColor(15, 15, 15);
                } else {
                    Canvas->SetColor(55, 55, 55);
                }
                Canvas->Circle(1, output[i].cx, output[i].cy, 14);
            } else if (output[i].id == O_RST) {
                Canvas->Circle(1, output[i].cx, output[i].cy, 16);
                if (p_RST) {
                    Canvas->SetColor(15, 15, 15);
                } else {
                    Canvas->SetColor(55, 55, 55);
                }
                Canvas->Circle(1, output[i].cx, output[i].cy, 14);
            } else if (output[i].id == O_POT1) {
                Canvas->SetBgColor(66, 109, 246);
                Canvas->Rectangle(1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1,
                                  output[i].y2 - output[i].y1);

                Canvas->SetBgColor(250, 250, 250);
                Canvas->Circle(1, output[i].cx, output[i].cy, 30);

                Canvas->SetBgColor(150, 150, 150);
                int x = -22 * sin((5.585 * (pot1 / 200.0)) + 0.349);
                int y = 22 * cos((5.585 * (pot1 / 200.0)) + 0.349);
                Canvas->Circle(1, output[i].cx + x, output[i].cy + y, 5);
            } else if (output[i].id == O_JMP) {
                // draw a rectangle
                Canvas->Rectangle(1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1,
                                  output[i].y2 - output[i].y1);
                if (jmp[0]) {
                    Canvas->SetColor(70, 70, 70);
                    Canvas->Rectangle(1, output[i].x1, output[i].y1, (int)((output[i].x2 - output[i].x1) * 0.65),
                                      output[i].y2 - output[i].y1);
                    Canvas->SetColor(220, 220, 0);
                    Canvas->Circle(1, output[i].x1 + (int)((output[i].x2 - output[i].x1) * 0.80),
                                   output[i].y1 + ((output[i].y2 - output[i].y1) / 2), 3);
                } else {
                    Canvas->SetColor(70, 70, 70);
                    Canvas->Rectangle(1, output[i].x1 + ((int)((output[i].x2 - output[i].x1) * 0.35)), output[i].y1,
                                      (int)((output[i].x2 - output[i].x1) * 0.65), output[i].y2 - output[i].y1);
                    Canvas->SetColor(220, 220, 0);
                    Canvas->Circle(1, output[i].x1 + (int)((output[i].x2 - output[i].x1) * 0.20),
                                   output[i].y1 + ((output[i].y2 - output[i].y1) / 2), 3);
                }
            } else {
                // draw a rectangle
                Canvas->Rectangle(1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1,
                                  output[i].y2 - output[i].y1);
            }
        } else  // if output shape is a circle
        {
        }
    }

    // end draw
    Canvas->End();

    int value = (pic.pins[1].oavalue - 55) / 2;  // RA5 mean value
    PICSimLab.UpdateGUI(PWM0, GT_GAUGE, GA_SET, (void*)&value);
    value = (pic.pins[17].oavalue - 55) / 2;  // RA1 mean value
    PICSimLab.UpdateGUI(PWM1, GT_GAUGE, GA_SET, (void*)&value);
    value = (pic.pins[16].oavalue - 55) / 2;  // RA2 mean value
    PICSimLab.UpdateGUI(PWM2, GT_GAUGE, GA_SET, (void*)&value);
    value = (pic.pins[4].oavalue - 55) / 2;  // RC5 mean value
    PICSimLab.UpdateGUI(PWM3, GT_GAUGE, GA_SET, (void*)&value);
}

void cboard_Curiosity::Run_CPU(void) {
    int i;
    int j;
    unsigned char pi;
    const picpin* pins;
    unsigned int alm[20];

    const int JUMPSTEPS = PICSimLab.GetJUMPSTEPS();  // number of steps skipped
    const long int NSTEP = PICSimLab.GetNSTEP();     // number of steps in 100ms
    const float RNSTEP = 200.0 * pic.PINCOUNT / NSTEP;

    // reset mean value

    memset(alm, 0, 20 * sizeof(unsigned int));

    // read pic.pins to a local variable to speed up
    pins = pic.pins;

    if (use_spare)
        SpareParts.PreProcess();

    j = JUMPSTEPS;  // step counter
    pi = 0;
    if (PICSimLab.GetMcuPwr())       // if powered
        for (i = 0; i < NSTEP; i++)  // repeat for number of steps in 100ms
        {
            if (j >= JUMPSTEPS)  // if number of step is bigger than steps to skip
            {
                pic_set_pin(&pic, pic.mclr, p_RST);
                pic_set_pin(&pic, 6, p_BT1);  // Set pin 6 (RC4) with button state
            }

            // verify if a breakpoint is reached if not run one instruction
            if (!mplabxd_testbp())
                pic_step(&pic);
            ioupdated = pic.ioupdated;
            InstCounterInc();
            if (use_oscope)
                Oscilloscope.SetSample();
            if (use_spare)
                SpareParts.Process();

            // increment mean value counter if pin is high
            alm[pi] += pins[pi].value;
            pi++;
            if (pi == pic.PINCOUNT)
                pi = 0;

            if (j >= JUMPSTEPS)  // if number of step is bigger than steps to skip
            {
                // set analog pin 16 (RC0 AN4) with value from scroll
                pic_set_apin(&pic, 16, (pic.vcc * pot1 / 199));

                j = -1;  // reset counter
            }

            j++;  // counter increment
            pic.ioupdated = 0;
        }

    // calculate mean value
    for (pi = 0; pi < pic.PINCOUNT; pi++) {
        pic.pins[pi].oavalue = (int)((alm[pi] * RNSTEP) + 55);
    }
    if (use_spare)
        SpareParts.PostProcess();
}

board_init(BOARD_Curiosity_Name, cboard_Curiosity);
