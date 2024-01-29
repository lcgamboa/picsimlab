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
#include "board_Breadboard.h"
#include "../lib/oscilloscope.h"
#include "../lib/picsimlab.h"
#include "../lib/spareparts.h"

/* ids of inputs of input map*/
enum {
    I_ICSP,  // ICSP connector
    I_PWR,   // Power button
    I_RST,   // Reset button
    I_JMP    // Vcc selection
};

/* ids of outputs of output map*/
enum {
    O_LPWR,  // Power LED
    O_MP,    // uController name
    O_RST,   // Reset button
    O_JMP    // Vcc selection
};
// return the input ids numbers of names used in input map

unsigned short cboard_Breadboard::GetInputId(char* name) {
    if (strcmp(name, "PG_ICSP") == 0)
        return I_ICSP;
    if (strcmp(name, "SW_PWR") == 0)
        return I_PWR;
    if (strcmp(name, "PB_RST") == 0)
        return I_RST;
    if (strcmp(name, "JP_1") == 0)
        return I_JMP;

    printf("Error input '%s' don't have a valid id! \n", name);
    return INVALID_ID;
}

// return the output ids numbers of names used in output map

unsigned short cboard_Breadboard::GetOutputId(char* name) {
    if (strcmp(name, "LD_LPWR") == 0)
        return O_LPWR;
    if (strcmp(name, "IC_CPU") == 0)
        return O_MP;
    if (strcmp(name, "PB_RST") == 0)
        return O_RST;
    if (strcmp(name, "JP_1") == 0)
        return O_JMP;

    printf("Error output '%s' don't have a valid id! \n", name);
    return INVALID_ID;
}

// Constructor called once on board creation

cboard_Breadboard::cboard_Breadboard(void) {
    ptype = _PIC;
    jmp[0] = 0;
    pic.vcc = 5.0;
    Proc = "PIC18F4620";  // default microcontroller if none defined in preferences
    ReadMaps();           // Read input and output board maps

    micbmp = PICSimLab.CanvasCmd(
        {CC_LOADIMAGE, .LoadImage{(PICSimLab.GetSharePath() + "boards/Common/ic40.svg").c_str(), Scale, 1, 0}});
}

// Destructor called once on board destruction

cboard_Breadboard::~cboard_Breadboard(void) {
    PICSimLab.CanvasCmd({CC_FREEBITMAP, .FreeBitmap{micbmp}});
    micbmp = -1;
}

// Reset board status

void cboard_Breadboard::Reset(void) {
    switch (ptype) {
        case _PIC:

            pic_reset(&pic, 1);

            // verify serial port state and refresh status bar

            if (pic.serial[0].serialfd != INVALID_SERIAL)
                PICSimLab.UpdateStatus(PS_SERIAL, "Serial: " + std::string(SERIALDEVICE) + ":" +
                                                      std::to_string(pic.serial[0].serialbaud) + "(" +
                                                      FloatStrFormat("%4.1f", fabs((100.0 * pic.serial[0].serialexbaud -
                                                                                    100.0 * pic.serial[0].serialbaud) /
                                                                                   pic.serial[0].serialexbaud)) +
                                                      "%)");
            else
                PICSimLab.UpdateStatus(PS_SERIAL, "Serial: " + std::string(SERIALDEVICE) + " (ERROR)");
            break;
        case _AVR:
            MReset(0);
            // verify serial port state and refresh status bar
            if (serialfd != INVALID_SERIAL)
                PICSimLab.UpdateStatus(
                    PS_SERIAL, "Serial: " + std::string(SERIALDEVICE) + ":" + std::to_string(serialbaud[0]) + "(" +
                                   FloatStrFormat("%4.1f", fabs((100.0 * serialexbaud[0] - 100.0 * serialbaud[0]) /
                                                                serialexbaud[0])) +
                                   "%)");
            else
                PICSimLab.UpdateStatus(PS_SERIAL, "Serial: " + std::string(SERIALDEVICE) + " (ERROR)");
            break;
    }

    if (jmp[0]) {
        MSetVCC(3.3);
    } else {
        MSetVCC(5.0);
    }

    if (use_spare)
        SpareParts.Reset();
    if (use_oscope)
        Oscilloscope.Reset();
}

// Called ever 1s to refresh status

void cboard_Breadboard::RefreshStatus(void) {
    switch (ptype) {
        case _PIC:
            // verify serial port state and refresh status bar

            if (pic.serial[0].serialfd != INVALID_SERIAL)
                PICSimLab.UpdateStatus(PS_SERIAL, "Serial: " + std::string(SERIALDEVICE) + ":" +
                                                      std::to_string(pic.serial[0].serialbaud) + "(" +
                                                      FloatStrFormat("%4.1f", fabs((100.0 * pic.serial[0].serialexbaud -
                                                                                    100.0 * pic.serial[0].serialbaud) /
                                                                                   pic.serial[0].serialexbaud)) +
                                                      "%)");
            else
                PICSimLab.UpdateStatus(PS_SERIAL, "Serial: " + std::string(SERIALDEVICE) + " (ERROR)");
            break;
        case _AVR:
            // verify serial port state and refresh status bar

            if (serialfd != INVALID_SERIAL)
                PICSimLab.UpdateStatus(
                    PS_SERIAL, "Serial: " + std::string(SERIALDEVICE) + ":" + std::to_string(serialbaud[0]) + "(" +
                                   FloatStrFormat("%4.1f", fabs((100.0 * serialexbaud[0] - 100.0 * serialbaud[0]) /
                                                                serialexbaud[0])) +
                                   "%)");
            else
                PICSimLab.UpdateStatus(PS_SERIAL, "Serial: " + std::string(SERIALDEVICE) + " (ERROR)");

            if (PICSimLab.GetMcuPwr()) {
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
                    break;
                } else {
                    PICSimLab.SetCpuState(CPU_ERROR);
                }
            } else {
                PICSimLab.SetCpuState(CPU_POWER_OFF);
            }
    }
}

// Called to save board preferences in configuration file

void cboard_Breadboard::WritePreferences(void) {
    // write selected microcontroller of board_x to preferences
    PICSimLab.SavePrefs("Breadboard_proc", Proc);
    PICSimLab.SavePrefs("Breadboard_clock", FloatStrFormat("%2.1f", PICSimLab.GetClock()));
    PICSimLab.SavePrefs("Breadboard_jmp", std::to_string(jmp[0]));
}

// Called whe configuration file load  preferences

void cboard_Breadboard::ReadPreferences(char* name, char* value) {
    // read microcontroller of preferences
    if (!strcmp(name, "Breadboard_proc")) {
        Proc = value;
    }
    if (!strcmp(name, "Breadboard_clock")) {
        PICSimLab.SetClock(atof(value));
    }
    if (!strcmp(name, "Breadboard_jmp")) {
        int i;
        for (i = 0; i < 1; i++) {
            if (value[i] == '0')
                jmp[i] = 0;
            else
                jmp[i] = 1;
        }
    }
}

// Event on the board

void cboard_Breadboard::EvKeyPress(uint key, uint mask) {}

// Event on the board

void cboard_Breadboard::EvKeyRelease(uint key, uint mask) {}

// Event on the board

void cboard_Breadboard::EvMouseButtonPress(uint button, uint x, uint y, uint state) {
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
                    output_ids[O_LPWR]->update = 1;
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
                case I_JMP:
                    jmp[0] ^= 0x01;
                    Reset();
                    output_ids[O_JMP]->update = 1;
                    break;
            }
        }
    }
}

// Event on the board

void cboard_Breadboard::EvMouseButtonRelease(uint button, uint x, uint y, uint state) {
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

void cboard_Breadboard::Draw(void) {
    int i;
    Rect_t rec;
    unsigned int w, h;
    int update = 0;  // verifiy if updated is needed

    // board_0 draw
    for (i = 0; i < outputc; i++)  // run over all outputs
    {
        if (output[i].update)  // only if need update
        {
            output[i].update = 0;

            if (!update) {
                PICSimLab.CanvasCmd({CC_INIT, .Init{Scale, Scale, 0}});
                PICSimLab.CanvasCmd({CC_SETFONTWEIGHT, .SetFontWeight{lxFONTWEIGHT_BOLD}});
            }
            update++;  // set to update buffer

            PICSimLab.CanvasCmd({CC_SETFGCOLOR, .SetFgColor{0, 0, 0}});  // black

            switch (output[i].id)  // search for color of output
            {
                case O_LPWR:  // Blue using mcupwr value
                    PICSimLab.CanvasCmd(
                        {CC_SETCOLOR, .SetColor{0, 0, (unsigned int)(200 * PICSimLab.GetMcuPwr() + 55)}});
                    PICSimLab.CanvasCmd(
                        {CC_RECTANGLE, .Rectangle{1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1,
                                                  output[i].y2 - output[i].y1}});
                    break;
                case O_MP:
                    PICSimLab.CanvasCmd({CC_SETFONTSIZE, .SetFontSize{((MGetPinCount() >= 44) || (MGetPinCount() <= 8))
                                                                          ? 6
                                                                          : ((MGetPinCount() > 14) ? 12 : 10)}});

                    PICSimLab.CanvasCmd({CC_GETBITMAPSIZE, .GetBitmapSize{micbmp, &w, &h}});
                    PICSimLab.CanvasCmd({CC_CHANGESCALE, .ChangeScale{1.0, 1.0}});
                    PICSimLab.CanvasCmd({CC_PUTBITMAP, .PutBitmap{micbmp, output[i].x1 * Scale, output[i].y1 * Scale}});
                    PICSimLab.CanvasCmd({CC_CHANGESCALE, .ChangeScale{Scale, Scale}});
                    PICSimLab.CanvasCmd({CC_SETFGCOLOR, .SetFgColor{230, 230, 230}});

                    rec.x = output[i].x1;
                    rec.y = output[i].y1;
                    rec.width = w / Scale;
                    rec.height = h / Scale;
                    PICSimLab.CanvasCmd(
                        {CC_TEXTONRECT, .TextOnRect{Proc.c_str(), rec, lxALIGN_CENTER | lxALIGN_CENTER_VERTICAL}});
                    break;
                case O_RST:
                    PICSimLab.CanvasCmd({CC_SETCOLOR, .SetColor{100, 100, 100}});
                    PICSimLab.CanvasCmd({CC_CIRCLE, .Circle{1, output[i].cx, output[i].cy, 11}});
                    if (p_RST) {
                        PICSimLab.CanvasCmd({CC_SETCOLOR, .SetColor{15, 15, 15}});
                    } else {
                        PICSimLab.CanvasCmd({CC_SETCOLOR, .SetColor{55, 55, 55}});
                    }
                    PICSimLab.CanvasCmd({CC_CIRCLE, .Circle{1, output[i].cx, output[i].cy, 9}});
                    break;
                case O_JMP:
                    PICSimLab.CanvasCmd({CC_SETCOLOR, .SetColor{0, 0, 0}});
                    PICSimLab.CanvasCmd(
                        {CC_RECTANGLE, .Rectangle{1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1,
                                                  output[i].y2 - output[i].y1}});

                    if (!jmp[0]) {
                        PICSimLab.CanvasCmd({CC_SETBGCOLOR, .SetBgColor{70, 70, 70}});
                        PICSimLab.CanvasCmd({CC_RECTANGLE, .Rectangle{1, output[i].x1, output[i].y1,
                                                                      ((output[i].x2 - output[i].x1) * 0.65f),
                                                                      output[i].y2 - output[i].y1}});
                        PICSimLab.CanvasCmd({CC_SETBGCOLOR, .SetBgColor{220, 220, 0}});
                        PICSimLab.CanvasCmd(
                            {CC_CIRCLE, .Circle{1, output[i].x1 + (int)((output[i].x2 - output[i].x1) * 0.80),
                                                output[i].y1 + ((output[i].y2 - output[i].y1) / 2), 3}});
                    } else {
                        PICSimLab.CanvasCmd({CC_SETBGCOLOR, .SetBgColor{70, 70, 70}});
                        PICSimLab.CanvasCmd(
                            {CC_RECTANGLE,
                             .Rectangle{1, output[i].x1 + ((int)((output[i].x2 - output[i].x1) * 0.35)), output[i].y1,
                                        ((output[i].x2 - output[i].x1) * 0.65f), output[i].y2 - output[i].y1}});
                        PICSimLab.CanvasCmd({CC_SETBGCOLOR, .SetBgColor{220, 220, 0}});
                        PICSimLab.CanvasCmd(
                            {CC_CIRCLE, .Circle{1, output[i].x1 + (int)((output[i].x2 - output[i].x1) * 0.20),
                                                output[i].y1 + ((output[i].y2 - output[i].y1) / 2), 3}});
                    }
            }
        }
    }

    // end draw
    if (update) {
        PICSimLab.CanvasCmd({CC_END});
    }
}

void cboard_Breadboard::Run_CPU(void) {
    int i;
    int j;
    unsigned char pi;
    const picpin* pins;
    unsigned int alm[100];

    switch (ptype) {
        case _PIC: {
            const int JUMPSTEPS = PICSimLab.GetJUMPSTEPS();  // number
                                                             // of
                                                             // steps
                                                             // skipped
            const long int NSTEP = PICSimLab.GetNSTEP();     // number
                                                             // of
                                                             // steps
                                                             // in
                                                             // 100ms
            const float RNSTEP = 200.0 * pic.PINCOUNT / NSTEP;

            // reset mean value
            memset(alm, 0, 100 * sizeof(unsigned int));

            // read pic.pins to a
            // local variable to
            // speed up
            pins = MGetPinsValues();
            if (use_spare)
                SpareParts.PreProcess();

            j = JUMPSTEPS;  // step
                            // counter
            pi = 0;
            if (PICSimLab.GetMcuPwr())       // if
                                             // powered
                for (i = 0; i < NSTEP; i++)  // repeat
                                             // for
                                             // number
                                             // of
                                             // steps
                                             // in
                                             // 100ms
                {
                    if (j >= JUMPSTEPS)  // if
                                         // number
                                         // of
                                         // step
                                         // is
                                         // bigger
                                         // than
                                         // steps
                                         // to
                                         // skip
                    {
                        pic_set_pin(&pic, pic.mclr, p_RST);
                    }

                    // verify if a
                    // breakpoint is
                    // reached if
                    // not run one
                    // instruction
                    if (!mplabxd_testbp())
                        pic_step(&pic);
                    ioupdated = pic.ioupdated;
                    InstCounterInc();

                    if (use_oscope)
                        Oscilloscope.SetSample();
                    if (use_spare)
                        SpareParts.Process();

                    // increment
                    // mean value
                    // counter if
                    // pin is high
                    alm[pi] += pins[pi].value;
                    pi++;
                    if (pi == pic.PINCOUNT)
                        pi = 0;

                    if (j >= JUMPSTEPS)  // if
                                         // number
                                         // of
                                         // step
                                         // is
                                         // bigger
                                         // than
                                         // steps
                                         // to
                                         // skip
                    {
                        j = -1;  // reset
                                 // counter
                    }
                    j++;  // counter
                          // increment
                    pic.ioupdated = 0;
                }
            // calculate mean value
            for (pi = 0; pi < MGetPinCount(); pi++) {
                bsim_picsim::pic.pins[pi].oavalue = (int)((alm[pi] * RNSTEP) + 55);
            }
            if (use_spare)
                SpareParts.PostProcess();
            break;
        }
        case _AVR: {
            const int pinc = bsim_simavr::MGetPinCount();
            // const int JUMPSTEPS =
            // Window1.GetJUMPSTEPS
            // ()*4.0; //number of
            // steps skipped
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

            // read pic.pins to a
            // local variable to
            // speed up

            pins = bsim_simavr::MGetPinsValues();

            if (use_spare)
                SpareParts.PreProcess();

            // j = JUMPSTEPS; //step
            // counter
            pi = 0;
            if (PICSimLab.GetMcuPwr())       // if
                                             // powered
                for (i = 0; i < NSTEP; i++)  // repeat
                                             // for
                                             // number
                                             // of
                                             // steps
                                             // in
                                             // 100ms
                {
                    // verify if a
                    // breakpoint is
                    // reached if
                    // not run one
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
                    bsim_simavr::UpdateHardware();

                    // avr->sleep_usec=0;
                    if (use_oscope)
                        Oscilloscope.SetSample();
                    if (use_spare)
                        SpareParts.Process();
                    ioupdated = 0;

                    // increment
                    // mean value
                    // counter if
                    // pin is high
                    alm[pi] += pins[pi].value;
                    pi++;
                    if (pi == pinc)
                        pi = 0;
                    /*
                    if (j >=
                    JUMPSTEPS)//if
                    number of step
                    is bigger than
                    steps to skip
                     {
                      //set analog
                    pin 2 (AN0) with
                    value from
                    scroll
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
                bsim_simavr::pins[pi].oavalue = (int)((alm[pi] * RNSTEP) + 55);
            }
            if (use_spare)
                SpareParts.PostProcess();
            break;
        }
    }
}

// class
// dependent

int cboard_Breadboard::DebugInit(int dtyppe) {
    switch (ptype) {
        case _PIC:
            return bsim_picsim::DebugInit(dtyppe);
            break;
        case _AVR:
            return bsim_simavr::DebugInit(dtyppe);
            break;
    }
    return 0;
}

std::string cboard_Breadboard::GetDebugName(void) {
    switch (ptype) {
        case _PIC:
            return bsim_picsim::GetDebugName();
            break;
        case _AVR:
            return bsim_simavr::GetDebugName();
            break;
    }
    return "";
}

void cboard_Breadboard::DebugLoop(void) {
    switch (ptype) {
        case _PIC:
            bsim_picsim::DebugLoop();
            break;
        case _AVR:
            bsim_simavr::DebugLoop();
            break;
    }
}

int cboard_Breadboard::CpuInitialized(void) {
    switch (ptype) {
        case _PIC:
            return bsim_picsim::CpuInitialized();
            break;
        case _AVR:
            return bsim_simavr::CpuInitialized();
            break;
    }
    return 0;
}

void cboard_Breadboard::MSetSerial(const char* port) {
    switch (ptype) {
        case _PIC:
            return bsim_picsim::MSetSerial(port);
            break;
        case _AVR:
            return bsim_simavr::MSetSerial(port);
            break;
    }
}

int cboard_Breadboard::MInit(const char* processor, const char* fname, float freq) {
    int ret = 0;

    if (processor[0] == 'P')
        ptype = _PIC;
    else
        ptype = _AVR;

    switch (ptype) {
        case _PIC:
            ret = bsim_picsim::MInit(processor, fname, freq);
            break;
        case _AVR:
            ret = bsim_simavr::MInit(processor, fname, freq);
            break;
    }

    int bmp = PICSimLab.CanvasCmd(
        {CC_LOADIMAGE,
         .LoadImage{(PICSimLab.GetSharePath() + "boards/Common/ic" + std::to_string(MGetPinCount()) + ".svg").c_str(),
                    Scale, 1, 0}});

    if (bmp < 0) {
        bmp = PICSimLab.CanvasCmd(
            {CC_LOADIMAGE, .LoadImage{(PICSimLab.GetSharePath() + "boards/Common/ic6.svg").c_str(), Scale, 1, 0}});
        printf("picsimlab: IC package with %i pins not found!\n", MGetPinCount());
        printf("picsimlab: %s not found!\n",
               (const char*)(PICSimLab.GetSharePath() + "boards/Common/ic" + std::to_string(MGetPinCount()) + ".svg")
                   .c_str());
    }

    if (micbmp)
        PICSimLab.CanvasCmd({CC_FREEBITMAP, .FreeBitmap{micbmp}});
    micbmp = bmp;

    return ret;
}

void cboard_Breadboard::MEnd(void) {
    switch (ptype) {
        case _PIC:
            return bsim_picsim::MEnd();
            break;
        case _AVR:
            return bsim_simavr::MEnd();
            break;
    }
}

int cboard_Breadboard::MGetArchitecture(void) {
    switch (ptype) {
        case _PIC:
            return bsim_picsim::MGetArchitecture();
            break;
        case _AVR:
            return bsim_simavr::MGetArchitecture();
            break;
    }
    return ARCH_UNKNOWN;
}

int cboard_Breadboard::MDumpMemory(const char* fname) {
    switch (ptype) {
        case _PIC:
            return bsim_picsim::MDumpMemory(fname);
            break;
        case _AVR:
            return bsim_simavr::MDumpMemory(fname);
            break;
    }
    return 1;
}

void cboard_Breadboard::MEraseFlash(void) {
    switch (ptype) {
        case _PIC:
            return bsim_picsim::MEraseFlash();
            break;
        case _AVR:
            return bsim_simavr::MEraseFlash();
            break;
    }
}

void cboard_Breadboard::MSetFreq(float freq) {
    switch (ptype) {
        case _PIC:
            return bsim_picsim::MSetFreq(freq);
            break;
        case _AVR:
            return bsim_simavr::MSetFreq(freq);
            break;
    }
}

float cboard_Breadboard::MGetFreq(void) {
    switch (ptype) {
        case _PIC:
            return bsim_picsim::MGetFreq();
            break;
        case _AVR:
            return bsim_simavr::MGetFreq();
            break;
    }
    return 0;
}

void cboard_Breadboard::MSetVCC(float vcc) {
    switch (ptype) {
        case _PIC:
            return bsim_picsim::MSetVCC(vcc);
            break;
        case _AVR:
            return bsim_simavr::MSetVCC(vcc);
            break;
    }
}

float cboard_Breadboard::MGetVCC(void) {
    switch (ptype) {
        case _PIC:
            return bsim_picsim::MGetVCC();
            break;
        case _AVR:
            return bsim_simavr::MGetVCC();
            break;
    }
    return 0;
}

float cboard_Breadboard::MGetInstClockFreq(void) {
    switch (ptype) {
        case _PIC:
            return bsim_picsim::MGetInstClockFreq();
            break;
        case _AVR:
            return bsim_simavr::MGetInstClockFreq();
            break;
    }
    return 0;
}

int cboard_Breadboard::MGetPinCount(void) {
    switch (ptype) {
        case _PIC:
            return bsim_picsim::MGetPinCount();
            break;
        case _AVR:
            return bsim_simavr::MGetPinCount();
            break;
    }
    return 0;
}

std::string cboard_Breadboard::MGetPinName(int pin) {
    switch (ptype) {
        case _PIC:
            return bsim_picsim::MGetPinName(pin);
            break;
        case _AVR:
            return bsim_simavr::MGetPinName(pin);
            break;
    }
    return "";
}

void cboard_Breadboard::MSetPin(int pin, unsigned char value) {
    switch (ptype) {
        case _PIC:
            return bsim_picsim::MSetPin(pin, value);
            break;
        case _AVR:
            return bsim_simavr::MSetPin(pin, value);
            break;
    }
}

void cboard_Breadboard::MSetPinDOV(int pin, unsigned char ovalue) {
    switch (ptype) {
        case _PIC:
            return bsim_picsim::MSetPinDOV(pin, ovalue);
            break;
        case _AVR:
            return bsim_simavr::MSetPinDOV(pin, ovalue);
            break;
    }
}

void cboard_Breadboard::MSetAPin(int pin, float value) {
    switch (ptype) {
        case _PIC:
            return bsim_picsim::MSetAPin(pin, value);
            break;
        case _AVR:
            return bsim_simavr::MSetAPin(pin, value);
            break;
    }
}

unsigned char cboard_Breadboard::MGetPin(int pin) {
    switch (ptype) {
        case _PIC:
            return bsim_picsim::MGetPin(pin);
            break;
        case _AVR:
            return bsim_simavr::MGetPin(pin);
            break;
    }
    return 0;
}

const picpin* cboard_Breadboard::MGetPinsValues(void) {
    switch (ptype) {
        case _PIC:
            return bsim_picsim::MGetPinsValues();
            break;
        case _AVR:
            return bsim_simavr::MGetPinsValues();
            break;
    }
    return NULL;
}

void cboard_Breadboard::MStep(void) {
    switch (ptype) {
        case _PIC:
            return bsim_picsim::MStep();
            break;
        case _AVR:
            return bsim_simavr::MStep();
            break;
    }
}

void cboard_Breadboard::MStepResume(void) {
    switch (ptype) {
        case _PIC:
            return bsim_picsim::MStepResume();
            break;
        case _AVR:
            return bsim_simavr::MStepResume();
            break;
    }
}

void cboard_Breadboard::MReset(int flags) {
    switch (ptype) {
        case _PIC:
            return bsim_picsim::MReset(flags);
            break;
        case _AVR:
            return bsim_simavr::MReset(flags);
            break;
    }
}

unsigned short* cboard_Breadboard::DBGGetProcID_p(void) {
    switch (ptype) {
        case _PIC:
            return bsim_picsim::DBGGetProcID_p();
            break;
        case _AVR:
            return bsim_simavr::DBGGetProcID_p();
            break;
    }
    return NULL;
}

unsigned int cboard_Breadboard::DBGGetPC(void) {
    switch (ptype) {
        case _PIC:
            return bsim_picsim::DBGGetPC();
            break;
        case _AVR:
            return bsim_simavr::DBGGetPC();
            break;
    }
    return 0;
}

void cboard_Breadboard::DBGSetPC(unsigned int pc) {
    switch (ptype) {
        case _PIC:
            bsim_picsim::DBGSetPC(pc);
            return;
            break;
        case _AVR:
            bsim_simavr::DBGSetPC(pc);
            return;
            break;
    }
    return;
}

unsigned char* cboard_Breadboard::DBGGetRAM_p(void) {
    switch (ptype) {
        case _PIC:
            return bsim_picsim::DBGGetRAM_p();
            break;
        case _AVR:
            return bsim_simavr::DBGGetRAM_p();
            break;
    }
    return NULL;
}

unsigned char* cboard_Breadboard::DBGGetROM_p(void) {
    switch (ptype) {
        case _PIC:
            return bsim_picsim::DBGGetROM_p();
            break;
        case _AVR:
            return bsim_simavr::DBGGetROM_p();
            break;
    }
    return NULL;
}

unsigned char* cboard_Breadboard::DBGGetCONFIG_p(void) {
    switch (ptype) {
        case _PIC:
            return bsim_picsim::DBGGetCONFIG_p();
            break;
        case _AVR:
            return bsim_simavr::DBGGetCONFIG_p();
            break;
    }
    return NULL;
}

unsigned char* cboard_Breadboard::DBGGetID_p(void) {
    switch (ptype) {
        case _PIC:
            return bsim_picsim::DBGGetID_p();
            break;
        case _AVR:
            return bsim_simavr::DBGGetID_p();
            break;
    }
    return NULL;
}

unsigned char* cboard_Breadboard::DBGGetEEPROM_p(void) {
    switch (ptype) {
        case _PIC:
            return bsim_picsim::DBGGetEEPROM_p();
            break;
        case _AVR:
            return bsim_simavr::DBGGetEEPROM_p();
            break;
    }
    return NULL;
}

unsigned int cboard_Breadboard::DBGGetRAMSize(void) {
    switch (ptype) {
        case _PIC:
            return bsim_picsim::DBGGetRAMSize();
            break;
        case _AVR:
            return bsim_simavr::DBGGetRAMSize();
            break;
    }
    return 0;
}

unsigned int cboard_Breadboard::DBGGetROMSize(void) {
    switch (ptype) {
        case _PIC:
            return bsim_picsim::DBGGetROMSize();
            break;
        case _AVR:
            return bsim_simavr::DBGGetROMSize();
            break;
    }
    return 0;
}

unsigned int cboard_Breadboard::DBGGetCONFIGSize(void) {
    switch (ptype) {
        case _PIC:
            return bsim_picsim::DBGGetCONFIGSize();
            break;
        case _AVR:
            return bsim_simavr::DBGGetCONFIGSize();
            break;
    }
    return 0;
}

unsigned int cboard_Breadboard::DBGGetIDSize(void) {
    switch (ptype) {
        case _PIC:
            return bsim_picsim::DBGGetIDSize();
            break;
        case _AVR:
            return bsim_simavr::DBGGetIDSize();
            break;
    }
    return 0;
}

unsigned int cboard_Breadboard::DBGGetEEPROM_Size(void) {
    switch (ptype) {
        case _PIC:
            return bsim_picsim::DBGGetEEPROM_Size();
            break;
        case _AVR:
            return bsim_simavr::DBGGetEEPROM_Size();
            break;
    }
    return 0;
}

int cboard_Breadboard::GetDefaultClock(void) {
    switch (ptype) {
        case _PIC:
            return bsim_picsim::GetDefaultClock();
            break;
        case _AVR:
            return bsim_simavr::GetDefaultClock();
            break;
    }
    return 0;
}

int cboard_Breadboard::GetUARTRX(const int uart_num) {
    switch (ptype) {
        case _PIC:
            return bsim_picsim::GetUARTRX(uart_num);
            break;
        case _AVR:
            return bsim_simavr::GetUARTRX(uart_num);
            break;
    }
    return 0;
}
int cboard_Breadboard::GetUARTTX(const int uart_num) {
    switch (ptype) {
        case _PIC:
            return bsim_picsim::GetUARTTX(uart_num);
            break;
        case _AVR:
            return bsim_simavr::GetUARTTX(uart_num);
            break;
    }
    return 0;
}

void cboard_Breadboard::SetScale(double scale) {
    if (Scale == scale)
        return;

    Scale = scale;

    int bmp = -1;

    if (MGetPinCount()) {
        bmp = PICSimLab.CanvasCmd(
            {CC_LOADIMAGE,
             .LoadImage{
                 (PICSimLab.GetSharePath() + "boards/Common/ic" + std::to_string(MGetPinCount()) + ".svg").c_str(),
                 Scale, 1, 0}});
        if (bmp < 0) {
            bmp = PICSimLab.CanvasCmd(
                {CC_LOADIMAGE, .LoadImage{(PICSimLab.GetSharePath() + "boards/Common/ic6.svg").c_str(), Scale, 1, 0}});
            printf("picsimlab: IC package with %i pins not found!\n", MGetPinCount());
            printf("picsimlab: %s not found!\n", (const char*)(PICSimLab.GetSharePath() + "boards/Common/ic" +
                                                               std::to_string(MGetPinCount()) + ".svg")
                                                     .c_str());
        }
    } else {
        bmp = PICSimLab.CanvasCmd(
            {CC_LOADIMAGE, .LoadImage{(PICSimLab.GetSharePath() + "boards/Common/ic40.svg").c_str(), Scale, 1, 0}});
    }
    if (micbmp >= 0)
        PICSimLab.CanvasCmd({CC_FREEBITMAP, .FreeBitmap{micbmp}});
    micbmp = bmp;
}

void cboard_Breadboard::EndServers(void) {
    mplabxd_server_end();
}

board_init(BOARD_Breadboard_Name, cboard_Breadboard);
