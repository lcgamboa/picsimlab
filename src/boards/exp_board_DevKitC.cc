/* ########################################################################

   PICsimLab - PIC laboratory simulator

   ########################################################################

   Copyright (c) : 2021-2022  Luis Claudio Gambôa Lopes

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
#include "exp_board_DevKitC.h"
#include "../picsimlab1.h"
#include "../picsimlab4.h"  //Oscilloscope
#include "../picsimlab5.h"  //Spare Parts

/* ids of inputs of input map*/
enum {
    I_ICSP,  // ICSP connector
    I_PWR,   // Power button
    I_RST,   // Reset button
    I_BOOT   // Boot button
};

/* ids of outputs of output map*/
enum {
    O_LON,  // Power LED
    O_RST,  // Reset button
    O_BOOT  // Boot button
};
// return the input ids numbers of names used in input map

unsigned short cboard_DevKitC::get_in_id(char* name) {
    if (strcmp(name, "PG_ICSP") == 0)
        return I_ICSP;
    if (strcmp(name, "SW_PWR") == 0)
        return I_PWR;
    if (strcmp(name, "PB_RST") == 0)
        return I_RST;
    if (strcmp(name, "PB_BOOT") == 0)
        return I_BOOT;

    printf("Error input '%s' don't have a valid id! \n", name);
    return -1;
}

// return the output ids numbers of names used in output map

unsigned short cboard_DevKitC::get_out_id(char* name) {
    if (strcmp(name, "LD_ON") == 0)
        return O_LON;
    if (strcmp(name, "PB_RST") == 0)
        return O_RST;
    if (strcmp(name, "PB_BOOT") == 0)
        return O_BOOT;

    printf("Error output '%s' don't have a valid id! \n", name);
    return 1;
}

// Constructor called once on board creation

cboard_DevKitC::cboard_DevKitC(void) {
    char buffer[1024];

    SimType = QEMU_SIM_ESP32;
    p_BOOT = 1;

    Proc = "ESP32";  // default microcontroller if none defined in preferences
    ReadMaps();      // Read input and output board maps

    // label1
    label1 = new CLabel();
    label1->SetFOwner(&Window1);
    label1->SetName(lxT("label1_"));
    label1->SetX(13);
    label1->SetY(54 + 20);
    label1->SetWidth(120);
    label1->SetHeight(24);
    label1->SetEnable(1);
    label1->SetVisible(1);
    label1->SetText(lxT("Qemu CPU MIPS"));
    label1->SetAlign(1);
    Window1.CreateChild(label1);
    // combo1
    combo1 = new CCombo();
    combo1->SetFOwner(&Window1);
    combo1->SetName(lxT("combo1_"));
    combo1->SetX(13);
    combo1->SetY(78 + 20);
    combo1->SetWidth(130);
    combo1->SetHeight(24);
    combo1->SetEnable(1);
    combo1->SetVisible(1);
    combo1->SetText(IcountToMipsStr(5));
    combo1->SetItems(IcountToMipsItens(buffer));
    combo1->SetTag(3);
    combo1->EvOnComboChange = EVONCOMBOCHANGE & CPWindow1::board_Event;
    Window1.CreateChild(combo1);
}

// Destructor called once on board destruction

cboard_DevKitC::~cboard_DevKitC(void) {
    Window1.DestroyChild(label1);
    Window1.DestroyChild(combo1);
}

// Reset board status

void cboard_DevKitC::Reset(void) {
    uint32_t* strap_mode = qemu_picsimlab_get_strap();

    if (p_BOOT) {
        *strap_mode = 0x12;  // SPI_FAST_FLASH_BOOT
    } else {
        *strap_mode = 0x0f;  // UART_BOOT(UART0)
    }

    MReset(1);

    Window1.statusbar1.SetField(2, lxT("Serial: ") + lxString::FromAscii(SERIALDEVICE));

    if (use_spare)
        Window5.Reset();

    RegisterRemoteControl();
}

void cboard_DevKitC::RegisterRemoteControl(void) {
    /*
    for (int i = 0; i < outputc; i++) {
        switch (output[i].id) {
            case O_LED:
                output[i].status = &pins[1].oavalue;
                break;
        }
    }*/
}

// Called ever 1s to refresh status

void cboard_DevKitC::RefreshStatus(void) {
    Window1.statusbar1.SetField(2, lxT("Serial: ") + lxString::FromAscii(SERIALDEVICE));
}

// Called to save board preferences in configuration file

void cboard_DevKitC::WritePreferences(void) {
    // write selected microcontroller of board_x to preferences
    Window1.saveprefs(lxT("DevKitC_proc"), Proc);
    // write microcontroller clock to preferences
    Window1.saveprefs(lxT("DevKitC_clock"), lxString().Format("%2.1f", Window1.GetClock()));
    // write microcontroller icount to preferences
    Window1.saveprefs(lxT("DevKitC_icount"), itoa(icount));
}

// Called whe configuration file load  preferences

void cboard_DevKitC::ReadPreferences(char* name, char* value) {
    // read microcontroller of preferences
    if (!strcmp(name, "DevKitC_proc")) {
        Proc = value;
    }
    // read microcontroller clock
    if (!strcmp(name, "DevKitC_clock")) {
        Window1.SetClock(atof(value));
    }
    // read microcontroller icount
    if (!strcmp(name, "DevKitC_icount")) {
        icount = atoi(value);
        combo1->SetText(IcountToMipsStr(icount));
    }
}

// Event on the board

void cboard_DevKitC::EvKeyPress(uint key, uint mask) {}

// Event on the board

void cboard_DevKitC::EvKeyRelease(uint key, uint mask) {}

// Event on the board

void cboard_DevKitC::EvMouseButtonPress(uint button, uint x, uint y, uint state) {
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
                    break;
                    // if event is over I_RST area then turn off and reset
                case I_RST:
                    /*
                    if (Window1.Get_mcupwr () && reset (-1))//if powered
                     {
                      Window1.Set_mcupwr (0);
                      Window1.Set_mcurst (1);
                     }
                     */
                    Reset();
                    p_RST = 0;
                    break;
                case I_BOOT:
                    p_BOOT = 0;
                    break;
            }
        }
    }
}

// Event on the board

void cboard_DevKitC::EvMouseButtonRelease(uint button, uint x, uint y, uint state) {
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
                        /*
                                 if (reset (-1))
                                  {
                                   Reset ();
                                  }
                         */
                    }
                    p_RST = 1;
                    break;
                case I_BOOT:
                    p_BOOT = 1;
                    break;
            }
        }
    }
}

// Called ever 100ms to draw board
// This is the critical code for simulator running speed

void cboard_DevKitC::Draw(CDraw* draw) {
    int i;

    draw->Canvas.Init(Scale, Scale);  // initialize draw context

    // board_x draw
    for (i = 0; i < outputc; i++)  // run over all outputs
    {
        if (!output[i].r)  // if output shape is a rectangle
        {
            draw->Canvas.SetFgColor(0, 0, 0);  // black

            switch (output[i].id)  // search for color of output
            {
                case O_LON:  // Blue using mcupwr value
                    draw->Canvas.SetColor(200 * Window1.Get_mcupwr() + 55, 0, 0);
                    break;
                case O_RST:
                case O_BOOT:
                    draw->Canvas.SetColor(100, 100, 100);
                    break;
            }

            draw->Canvas.Rectangle(1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1,
                                   output[i].y2 - output[i].y1);

            if (output[i].id == O_RST) {
                if (p_RST) {
                    draw->Canvas.SetColor(15, 15, 15);
                } else {
                    draw->Canvas.SetColor(55, 55, 55);
                }
                draw->Canvas.Circle(1, output[i].cx, output[i].cy, 11);
            } else if (output[i].id == O_BOOT) {
                if (p_BOOT) {
                    draw->Canvas.SetColor(15, 15, 15);
                } else {
                    draw->Canvas.SetColor(55, 55, 55);
                }
                draw->Canvas.Circle(1, output[i].cx, output[i].cy, 11);
            }
        }
    }

    // end draw
    draw->Canvas.End();
    draw->Update();
}

void cboard_DevKitC::Run_CPU_ns(uint64_t time) {
    static unsigned char pi = 0;
    static unsigned int alm[64];
    static const int pinc = MGetPinCount();

    const long int NSTEP = 4.0 * Window1.GetNSTEP();  // number of steps in 100ms

    const int inc = 16000000L / NSTEP;

    const float RNSTEP = 200.0 * pinc * inc / 100000000;

    for (uint64_t c = 0; c < time; c += inc) {
        if (!ns_count) {
            // reset pins mean value
            memset(alm, 0, 64 * sizeof(unsigned int));

            // Spare parts window pre process
            if (use_spare)
                Window5.PreProcess();

            // j = JUMPSTEPS; //step counter
            pi = 0;
        }

        if (Window1.Get_mcupwr())  // if powered
                                   // for (i = 0; i < NSTEP; i++)  // repeat for number of steps in 100ms
        {
            /*
            if (j >= JUMPSTEPS)//if number of step is bigger than steps to skip
             {
             }
             */
            // verify if a breakpoint is reached if not run one instruction
            MStep();
            // Oscilloscope window process
            if (use_oscope)
                Window4.SetSample();
            // Spare parts window process
            if (use_spare)
                Window5.Process();

            //  increment mean value counter if pin is high
            alm[pi] += pins[pi].value;
            pi++;
            if (pi == pinc)
                pi = 0;
            /*
                if (j >= JUMPSTEPS)//if number of step is bigger than steps to skip
                 {
                  j = -1; //reset counter
                 }

                j++; //counter increment
             */
        }

        ns_count += inc;
        if (ns_count > 100000000) {
            ns_count = 0;
            //  calculate mean value
            for (pi = 0; pi < MGetPinCount(); pi++) {
                pins[pi].oavalue = (int)((alm[pi] * RNSTEP) + 55);
            }
            // Spare parts window pre post process
            if (use_spare)
                Window5.PostProcess();
        }
    }
}

void cboard_DevKitC::Run_CPU(void) {
#define CHR_TIOCM_CTS 0x020
#define CHR_TIOCM_CAR 0x040
#define CHR_TIOCM_DSR 0x100
#define CHR_TIOCM_RI 0x080
#define CHR_TIOCM_DTR 0x002
#define CHR_TIOCM_RTS 0x004

    static unsigned int status_ = 0;
    unsigned int status = qemu_picsimlab_get_TIOCM();

    if (status_ != status) {
        status_ = status;

        if ((status & CHR_TIOCM_CTS) && !(status & CHR_TIOCM_DSR)) {
            Reset();
        }

        if (!(status & CHR_TIOCM_CTS) && (status & CHR_TIOCM_DSR)) {
            uint32_t* strap_mode = qemu_picsimlab_get_strap();
            *strap_mode = 0x0f;  // UART_BOOT(UART0)
            MReset(1);
        }
    }
}

void cboard_DevKitC::board_Event(CControl* control) {
    icount = MipsStrToIcount(combo1->GetText().c_str());
    Window1.EndSimulation();
}

lxString cboard_DevKitC::MGetPinName(int pin) {
    lxString pinname = "error";

    switch (pin) {
        case 1:
            pinname = "VDD";
            break;
        case 2:
            pinname = "EN";
            break;
        case 3:
            pinname = "IO36";
            break;
        case 4:
            pinname = "IO39";
            break;
        case 5:
            pinname = "IO34";
            break;
        case 6:
            pinname = "IO35";
            break;
        case 7:
            pinname = "IO32";
            break;
        case 8:
            pinname = "IO33";
            break;
        case 9:
            pinname = "IO25";
            break;
        case 10:
            pinname = "IO26";
            break;
        case 11:
            pinname = "IO27";
            break;
        case 12:
            pinname = "IO14";
            break;
        case 13:
            pinname = "IO12";
            break;
        case 14:
            pinname = "GND";
            break;
        case 15:
            pinname = "IO13";
            break;
        case 16:
            pinname = "IO9";
            break;
        case 17:
            pinname = "IO10";
            break;
        case 18:
            pinname = "IO11";
            break;
        case 19:
            pinname = "VIN";
            break;
        case 20:
            pinname = "IO6";
            break;
        case 21:
            pinname = "IO7";
            break;
        case 22:
            pinname = "IO8";
            break;
        case 23:
            pinname = "IO15";
            break;
        case 24:
            pinname = "IO2";
            break;
        case 25:
            pinname = "IO0";
            break;
        case 26:
            pinname = "IO4";
            break;
        case 27:
            pinname = "IO16";
            break;
        case 28:
            pinname = "IO17";
            break;
        case 29:
            pinname = "IO5";
            break;
        case 30:
            pinname = "IO18";
            break;
        case 31:
            pinname = "IO19";
            break;
        case 32:
            pinname = "GND";
            break;
        case 33:
            pinname = "IO21";
            break;
        case 34:
            pinname = "IO3";
            break;
        case 35:
            pinname = "IO1";
            break;
        case 36:
            pinname = "IO22";
            break;
        case 37:
            pinname = "IO23";
            break;
        case 38:
            pinname = "GND";
            break;
    }

    return pinname;
}

int cboard_DevKitC::MGetPinCount(void) {
    return 38;
}

void cboard_DevKitC::MSetAPin(int pin, float value) {
    /*
    if (!pin)
        return;
    if ((pins[pin - 1].avalue != value)) {
        unsigned char channel = 0xFF;

        pins[pin - 1].avalue = value;

            switch (pin) {
                case 10:  // PA0
                    channel = 0;
                    break;
                case 11:  // PA1
                    channel = 1;
                    break;
                case 12:  // PA2
                    channel = 2;
                    break;
                case 13:  // PA3
                    channel = 3;
                    break;
                case 14:  // PA4
                    channel = 4;
                    break;
                case 15:  // PA5
                    channel = 5;
                    break;
                case 16:  // PA6
                    channel = 6;
                    break;
                case 17:  // PA7
                    channel = 7;
                    break;
                case 18:  // PB0
                    channel = 8;
                    break;
                case 19:  // PB1
                    channel = 9;
                    break;
            }

        if (channel != 0xFF) {
            if (value > 3.3)
                value = 3.3;
            if (value < 0)
                value = 0;

            unsigned short svalue = (unsigned short)(4096 * value / 3.3);

            pins[pin - 1].ptype = PT_ANALOG;

            if (ADCvalues[channel] != svalue) {
                qemu_picsimlab_set_apin(channel, svalue);
                ADCvalues[channel] = svalue;
                // printf("Analog channel %02X = %i\n",channel,svalue);
            }
        }
    }
    */
}

// Register the board in PICSimLab
board_init(BOARD_DevKitC_Name, cboard_DevKitC);