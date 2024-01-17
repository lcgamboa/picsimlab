/* ########################################################################

   PICSimLab - Programmable IC Simulator Laboratory

   ########################################################################

   Copyright (c) : 2015-2023  Luis Claudio Gambôa Lopes <lcgamboa@yahoo.com>

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
#include "board_x.h"
#include "../lib/oscilloscope.h"
#include "../lib/picsimlab.h"
#include "../lib/spareparts.h"

/* ids of inputs of input map*/
enum {
    I_POT1,  // potentiometer
    I_ICSP,  // ICSP connector
    I_PWR,   // Power button
    I_RST,   // Reset button
    I_BD0,   // RD0 push button
    I_SD1    // RD1 switch
};

/* ids of outputs of output map*/
enum {
    O_POT1,  // potentiometer
    O_RST,   // Reset button
    O_SD1,   // switch position (On/Off)
    O_LD0,   // LED on RD0 push button
    O_LD1,   // LED on RD1 switch
    O_LPWR,  // Power LED
    O_RB0,   // LED on RB0 output
    O_RB1,   // LED on RB1 output
    O_BD0,   // RD1 switch
    O_CPU    // CPU name
};

enum { PWM0 = 1, PWM1 };

// return the input ids numbers of names used in input map

unsigned short cboard_x::GetInputId(char* name) {
    if (strcmp(name, "PG_ICSP") == 0)
        return I_ICSP;
    if (strcmp(name, "SW_PWR") == 0)
        return I_PWR;
    if (strcmp(name, "PB_RST") == 0)
        return I_RST;
    if (strcmp(name, "PB_D0") == 0)
        return I_BD0;
    if (strcmp(name, "SW_D1") == 0)
        return I_SD1;
    if (strcmp(name, "PO_1") == 0)
        return I_POT1;

    printf("Error input '%s' don't have a valid id! \n", name);
    return INVALID_ID;
}

// return the output ids numbers of names used in output map

unsigned short cboard_x::GetOutputId(char* name) {
    if (strcmp(name, "SW_D1") == 0)
        return O_SD1;
    if (strcmp(name, "LD_LD0") == 0)
        return O_LD0;
    if (strcmp(name, "LD_LD1") == 0)
        return O_LD1;
    if (strcmp(name, "LD_LPWR") == 0)
        return O_LPWR;
    if (strcmp(name, "LD_RB1") == 0)
        return O_RB1;
    if (strcmp(name, "LD_RB0") == 0)
        return O_RB0;
    if (strcmp(name, "PB_D0") == 0)
        return O_BD0;
    if (strcmp(name, "PO_1") == 0)
        return O_POT1;
    if (strcmp(name, "PB_RST") == 0)
        return O_RST;
    if (strcmp(name, "IC_CPU") == 0)
        return O_CPU;

    printf("Error output '%s' don't have a valid id! \n", name);
    return INVALID_ID;
}

// Constructor called once on board creation

cboard_x::cboard_x(void) : font(10, lxFONTFAMILY_TELETYPE, lxFONTSTYLE_NORMAL, lxFONTWEIGHT_BOLD) {
    Proc = "PIC18F4550";  // default microcontroller if none defined in preferences
    ReadMaps();           // Read input and output board maps

    pot1 = 100;

    active = 0;

    PICSimLab.UpdateGUI(PWM0, GT_GAUGE, GA_ADD, (void*)"*RB0");
    PICSimLab.UpdateGUI(PWM1, GT_GAUGE, GA_ADD, (void*)"*RB1");

    SWBounce_init(&bounce, 2);
}

// Destructor called once on board destruction

cboard_x::~cboard_x(void) {
    PICSimLab.UpdateGUI(PWM0, GT_GAUGE, GA_DEL, NULL);
    PICSimLab.UpdateGUI(PWM1, GT_GAUGE, GA_DEL, NULL);
    SWBounce_end(&bounce);
}

// Reset board status

void cboard_x::Reset(void) {
    pic_reset(&pic, 1);

    p_BT1 = 1;  // set push button  in default state (high)

    // write button state to pic pin 19 (RD0)
    pic_set_pin(&pic, 19, p_BT1);
    // write switch state to pic pin 20 (RD1)
    pic_set_pin(&pic, 20, p_BT2);

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

    if (use_spare)
        SpareParts.Reset();

    RegisterRemoteControl();
}

// Register variables to be controled by remote control

void cboard_x::RegisterRemoteControl(void) {
    // register inputa
    input_ids[I_BD0]->status = &p_BT1;
    input_ids[I_SD1]->status = &p_BT2;
    input_ids[I_POT1]->status = &pot1;

    // register output to be updated on input change
    input_ids[I_BD0]->update = &output_ids[O_BD0]->update;
    input_ids[I_SD1]->update = &output_ids[O_SD1]->update;
    input_ids[I_POT1]->update = &output_ids[O_POT1]->update;

    // register outputa
    output_ids[O_RB0]->status = &pic.pins[32].oavalue;
    output_ids[O_RB1]->status = &pic.pins[33].oavalue;
    output_ids[O_LD0]->status = &pic.pins[18].oavalue;
    output_ids[O_LD1]->status = &pic.pins[19].oavalue;
}

// Called ever 1s to refresh status

void cboard_x::RefreshStatus(void) {
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

void cboard_x::WritePreferences(void) {
    // write selected microcontroller of board_x to preferences
    PICSimLab.SavePrefs("X_proc", Proc);
    // write switch state of board_x to preferences
    PICSimLab.SavePrefs("X_bt2", std::to_string(p_BT2));
    // write microcontroller clock to preferences
    PICSimLab.SavePrefs("X_clock", FloatStrFormat("%2.1f", PICSimLab.GetClock()));
    // write potentiometer position to preferences
    PICSimLab.SavePrefs("X_pot1", std::to_string(pot1));
}

// Called whe configuration file load  preferences

void cboard_x::ReadPreferences(char* name, char* value) {
    // read switch state of board_x of preferences
    if (!strcmp(name, "X_bt2")) {
        if (value[0] == '0')
            p_BT2 = 0;
        else
            p_BT2 = 1;
    }
    // read microcontroller of preferences
    if (!strcmp(name, "X_proc")) {
        Proc = value;
    }
    // read microcontroller clock
    if (!strcmp(name, "X_clock")) {
        PICSimLab.SetClock(atof(value));
    }

    // read potentiometer position
    if (!strcmp(name, "X_pot1")) {
        pot1 = atoi(value);
    }
}

// Event on the board

void cboard_x::EvKeyPress(uint key, uint mask) {
    // if keyboard key 1 is pressed then activate button (state=0)
    if (key == '1') {
        p_BT1 = 0;
        output_ids[O_BD0]->update = 1;
    }

    // if keyboard key 2 is pressed then toggle switch state
    if (key == '2') {
        p_BT2 ^= 1;
        output_ids[O_SD1]->update = 1;
    }
}

// Event on the board

void cboard_x::EvKeyRelease(uint key, uint mask) {
    // if keyboard key 1 is pressed then deactivate button (state=1)
    if (key == '1') {
        p_BT1 = 1;
        output_ids[O_BD0]->update = 1;
    }
}

// Event on the board

void cboard_x::EvMouseButtonPress(uint button, uint x, uint y, uint state) {
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
                    output_ids[O_LPWR]->update = 1;
                    break;
                    // if event is over I_RST area then turn off and reset
                case I_RST:
                    if (PICSimLab.GetMcuPwr() && pic_reset(&pic, -1))  // if powered
                    {
                        PICSimLab.SetMcuPwr(0);
                        PICSimLab.SetMcuRst(1);
                    }
                    p_RST = 0;
                    output_ids[O_RST]->update = 1;
                    break;
                    // if event is over I_D0 area then activate button (state=0)
                case I_BD0:
                    p_BT1 = 0;
                    output_ids[O_BD0]->update = 1;
                    break;
                    // if event is over I_D1 area then toggle switch state
                case I_SD1:
                    p_BT2 ^= 1;
                    output_ids[O_SD1]->update = 1;
                    break;
                case I_POT1: {
                    active = 1;
                    pot1 = (x - input[i].x1) * 2.77;
                    if (pot1 > 199)
                        pot1 = 199;
                    output_ids[O_POT1]->update = 1;
                } break;
            }
        }
    }
}

// Event on the board

void cboard_x::EvMouseMove(uint button, uint x, uint y, uint state) {
    int i;

    for (i = 0; i < inputc; i++) {
        switch (input[i].id) {
            case I_POT1:
                if (((input[i].x1 <= x) && (input[i].x2 >= x)) && ((input[i].y1 <= y) && (input[i].y2 >= y))) {
                    if (active) {
                        pot1 = (x - input[i].x1) * 2.77;
                        if (pot1 > 199)
                            pot1 = 199;
                        output_ids[O_POT1]->update = 1;
                    }
                } else {
                    active = 0;
                }
                break;
        }
    }
}

// Event on the board

void cboard_x::EvMouseButtonRelease(uint button, uint x, uint y, uint state) {
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
                    output_ids[O_RST]->update = 1;
                    break;
                    // if event is over I_D0 area then deactivate button (state=1)
                case I_BD0:
                    p_BT1 = 1;
                    output_ids[O_BD0]->update = 1;
                    break;
                case I_POT1: {
                    active = 0;
                    output_ids[O_POT1]->update = 1;
                } break;
            }
        }
    }
}

// Called ever 100ms to draw board
// This is the critical code for simulator running speed

void cboard_x::Draw(CDraw* draw) {
    int update = 0;  // verifiy if updated is needed
    int i;

    // board_x draw
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
                if (output[i].id == O_SD1)  // if output is switch
                {
                    // draw a background white rectangle
                    draw->Canvas.SetBgColor(255, 255, 255);
                    draw->Canvas.Rectangle(1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1,
                                           output[i].y2 - output[i].y1);

                    if (!p_BT2)  // draw switch off
                    {
                        // draw a grey rectangle
                        draw->Canvas.SetBgColor(70, 70, 70);
                        draw->Canvas.Rectangle(
                            1, output[i].x1, output[i].y1 + ((int)((output[i].y2 - output[i].y1) * 0.35)),
                            output[i].x2 - output[i].x1, (int)((output[i].y2 - output[i].y1) * 0.65));
                    } else  // draw switch on
                    {
                        // draw a grey rectangle
                        draw->Canvas.SetBgColor(70, 70, 70);
                        draw->Canvas.Rectangle(1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1,
                                               (int)((output[i].y2 - output[i].y1) * 0.65));
                    }
                } else if (output[i].id == O_BD0) {
                    draw->Canvas.SetColor(102, 102, 102);
                    draw->Canvas.Circle(1, output[i].cx, output[i].cy, 10);
                    if (p_BT1) {
                        draw->Canvas.SetColor(15, 15, 15);
                    } else {
                        draw->Canvas.SetColor(55, 55, 55);
                    }
                    draw->Canvas.Circle(1, output[i].cx, output[i].cy, 8);
                } else if (output[i].id == O_RST) {
                    draw->Canvas.SetColor(102, 102, 102);
                    draw->Canvas.Circle(1, output[i].cx, output[i].cy, 10);

                    if (p_RST) {
                        draw->Canvas.SetColor(15, 15, 15);
                    } else {
                        draw->Canvas.SetColor(55, 55, 55);
                    }
                    draw->Canvas.Circle(1, output[i].cx, output[i].cy, 8);
                } else if (output[i].id == O_POT1) {
                    draw->Canvas.SetColor(0, 50, 215);
                    draw->Canvas.Rectangle(1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1,
                                           output[i].y2 - output[i].y1);
                    draw->Canvas.SetColor(250, 250, 250);
                    draw->Canvas.Rectangle(1, output[i].x1 + pot1 / 2.77, output[i].y1 + 2, 10, 15);
                } else if (output[i].id == O_CPU) {
                    draw->Canvas.SetFont(font);
                    int x, y, w, h;
                    draw->Canvas.SetColor(26, 26, 26);
                    draw->Canvas.Rectangle(1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1,
                                           output[i].y2 - output[i].y1);

                    draw->Canvas.SetColor(230, 230, 230);
                    w = output[i].x2 - output[i].x1;
                    h = output[i].y2 - output[i].y2;
                    x = output[i].x1 + (w / 2) + 7;
                    y = output[i].y1 + (h / 2) + (Proc.length());
                    draw->Canvas.RotatedText(Proc, x, y, 270);
                }
            } else  // if output shape is a circle
            {
                draw->Canvas.SetFgColor(0, 0, 0);  // black

                switch (output[i].id)  // search for color of output
                {
                    case O_LD0:  // White using pin 19 mean value (RD0)
                        draw->Canvas.SetBgColor(pic.pins[18].oavalue, pic.pins[18].oavalue, pic.pins[18].oavalue);
                        break;
                    case O_LD1:  // Yelllow using pin 20 mean value (RD1)
                        draw->Canvas.SetBgColor(pic.pins[19].oavalue, pic.pins[19].oavalue, 0);
                        break;
                    case O_LPWR:  // Blue using mcupwr value
                        draw->Canvas.SetBgColor(0, 0, 200 * PICSimLab.GetMcuPwr() + 55);
                        break;
                    case O_RB0:  // Green using pin 33 mean value (RB0)
                        draw->Canvas.SetBgColor(0, pic.pins[32].oavalue, 0);
                        break;
                    case O_RB1:  // Red using pin 34 mean value (RB1)
                        draw->Canvas.SetBgColor(pic.pins[33].oavalue, 0, 0);
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

    int value = (pic.pins[33].oavalue - 55) / 2;  // RB0 mean value
    PICSimLab.UpdateGUI(PWM0, GT_GAUGE, GA_SET, (void*)&value);
    value = (pic.pins[32].oavalue - 55) / 2;  // RB1 mean value
    PICSimLab.UpdateGUI(PWM1, GT_GAUGE, GA_SET, (void*)&value);
}

void cboard_x::Run_CPU(void) {
    int i;
    int j;
    unsigned char pi;
    const picpin* pins;
    unsigned int alm[40];
    int bret;

    const int JUMPSTEPS = PICSimLab.GetJUMPSTEPS();  // number of steps skipped
    const long int NSTEP = PICSimLab.GetNSTEP();     // number of steps in 100ms
    const float RNSTEP = 200.0 * pic.PINCOUNT / NSTEP;

    // reset pins mean value
    memset(alm, 0, 40 * sizeof(unsigned int));

    // read pic.pins to a local variable to speed up
    pins = pic.pins;

    // Spare parts window pre process
    if (use_spare)
        SpareParts.PreProcess();

    SWBounce_prepare(&bounce, PICSimLab.GetBoard()->MGetInstClockFreq());

    unsigned char p_BT1_ = p_BT1;
    unsigned char p_BT2_ = p_BT2;

    if ((pins[19 - 1].dir == PD_IN) && (pins[19 - 1].value != p_BT1_)) {
        SWBounce_bounce(&bounce, 0);
    }
    if ((pins[20 - 1].dir == PD_IN) && (pins[20 - 1].value != p_BT2_)) {
        SWBounce_bounce(&bounce, 1);
    }

    j = JUMPSTEPS;  // step counter
    pi = 0;
    if (PICSimLab.GetMcuPwr())       // if powered
        for (i = 0; i < NSTEP; i++)  // repeat for number of steps in 100ms
        {
            if (j >= JUMPSTEPS)  // if number of step is bigger than steps to skip
            {
                pic_set_pin(&pic, pic.mclr, p_RST);
                if (!bounce.do_bounce) {
                    pic_set_pin(&pic, 19, p_BT1_);  // Set pin 19 (RD0) with button state
                    pic_set_pin(&pic, 20, p_BT2_);  // Set pin 20 (RD1) with switch state
                }
            }

            if (bounce.do_bounce) {
                bret = SWBounce_process(&bounce);
                if (bret) {
                    if (bounce.bounce[0]) {
                        if (bret == 1) {
                            pic_set_pin(&pic, 19, !pins[19 - 1].value);
                        } else {
                            pic_set_pin(&pic, 19, p_BT1_);
                        }
                    }
                    if (bounce.bounce[1]) {
                        if (bret == 1) {
                            pic_set_pin(&pic, 20, !pins[20 - 1].value);
                        } else {
                            pic_set_pin(&pic, 20, p_BT2_);
                        }
                    }
                }
            }

            // verify if a breakpoint is reached if not run one instruction
            if (!mplabxd_testbp())
                pic_step(&pic);
            ioupdated = pic.ioupdated;
            InstCounterInc();
            // Oscilloscope window process
            if (use_oscope)
                Oscilloscope.SetSample();
            // Spare parts window process
            if (use_spare)
                SpareParts.Process();

            // increment mean value counter if pin is high
            alm[pi] += pins[pi].value;
            pi++;
            if (pi == pic.PINCOUNT)
                pi = 0;

            if (j >= JUMPSTEPS)  // if number of step is bigger than steps to skip
            {
                // set analog pin 2 (AN0) with value from scroll
                pic_set_apin(&pic, 2, (5.0 * pot1 / 199));

                j = -1;  // reset counter
            }
            j++;  // counter increment
            pic.ioupdated = 0;
        }

    // calculate mean value
    for (pi = 0; pi < pic.PINCOUNT; pi++) {
        pic.pins[pi].oavalue = (int)((alm[pi] * RNSTEP) + 55);
    }

    // Spare parts window pre post process
    if (use_spare)
        SpareParts.PostProcess();

    // verifiy if LEDS need update
    if (output_ids[O_LD0]->value != pic.pins[18].oavalue) {
        output_ids[O_LD0]->value = pic.pins[18].oavalue;
        output_ids[O_LD0]->update = 1;
    }
    if (output_ids[O_LD1]->value != pic.pins[19].oavalue) {
        output_ids[O_LD1]->value = pic.pins[19].oavalue;
        output_ids[O_LD1]->update = 1;
    }
    if (output_ids[O_RB0]->value != pic.pins[32].oavalue) {
        output_ids[O_RB0]->value = pic.pins[32].oavalue;
        output_ids[O_RB0]->update = 1;
    }
    if (output_ids[O_RB1]->value != pic.pins[33].oavalue) {
        output_ids[O_RB1]->value = pic.pins[33].oavalue;
        output_ids[O_RB1]->update = 1;
    }
}

// Register the board in PICSimLab
board_init(BOARD_x_Name, cboard_x);
