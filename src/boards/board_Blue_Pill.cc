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
#include "board_Blue_Pill.h"
#include "../lib/oscilloscope.h"
#include "../lib/picsimlab.h"
#include "../lib/spareparts.h"

/* ids of inputs of input map*/
enum {
    I_ICSP,  // ICSP connector
    I_PWR,   // Power button
    I_RST,   // Reset button
};

/* ids of outputs of output map*/
enum {
    O_LPWR,  // Power LED
    O_LED,   // LED on PC13 output
    O_RST    // Reset button
};
// return the input ids numbers of names used in input map

unsigned short cboard_Blue_Pill::GetInputId(char* name) {
    if (strcmp(name, "PG_ICSP") == 0)
        return I_ICSP;
    if (strcmp(name, "SW_PWR") == 0)
        return I_PWR;
    if (strcmp(name, "PB_RST") == 0)
        return I_RST;

    printf("Error input '%s' don't have a valid id! \n", name);
    return -1;
}

// return the output ids numbers of names used in output map

unsigned short cboard_Blue_Pill::GetOutputId(char* name) {
    if (strcmp(name, "LD_LED") == 0)
        return O_LED;
    if (strcmp(name, "LD_PWR") == 0)
        return O_LPWR;
    if (strcmp(name, "PB_RST") == 0)
        return O_RST;

    printf("Error output '%s' don't have a valid id! \n", name);
    return 1;
}

// Constructor called once on board creation

cboard_Blue_Pill::cboard_Blue_Pill(void) {
    char buffer[1024];

    SimType = QEMU_SIM_STM32;

    Proc = "stm32f103c8t6";  // default microcontroller if none defined in preferences
    ReadMaps();              // Read input and output board maps

    master_i2c[0].scl_pin = 0;
    master_i2c[0].sda_pin = 0;
    master_i2c[1].scl_pin = 0;
    master_i2c[1].sda_pin = 0;

    master_spi[0].sck_pin = 0;
    master_spi[0].copi_pin = 0;
    master_spi[0].cipo_pin = 0;
    master_spi[0].cs_pin[0] = 0;
    master_spi[0].cs_pin[1] = 0;
    master_spi[0].cs_pin[2] = 0;

    master_spi[1].sck_pin = 0;
    master_spi[1].copi_pin = 0;
    master_spi[1].cipo_pin = 0;
    master_spi[1].cs_pin[0] = 0;
    master_spi[1].cs_pin[1] = 0;
    master_spi[1].cs_pin[2] = 0;

    master_uart[0].tx_pin = 0;
    master_uart[0].rx_pin = 0;
    master_uart[1].tx_pin = 0;
    master_uart[1].rx_pin = 0;
    master_uart[2].tx_pin = 0;
    master_uart[2].rx_pin = 0;

    if (PICSimLab.GetWindow()) {
        // label1
        label1 = new CLabel();
        label1->SetFOwner(PICSimLab.GetWindow());
        label1->SetName(lxT("label1_"));
        label1->SetX(13);
        label1->SetY(54 + 20);
        label1->SetWidth(120);
        label1->SetHeight(24);
        label1->SetEnable(1);
        label1->SetVisible(1);
        label1->SetText(lxT("Qemu CPU MIPS"));
        label1->SetAlign(1);
        PICSimLab.GetWindow()->CreateChild(label1);
        // combo1
        combo1 = new CCombo();
        combo1->SetFOwner(PICSimLab.GetWindow());
        combo1->SetName(lxT("combo1_"));
        combo1->SetX(13);
        combo1->SetY(78 + 20);
        combo1->SetWidth(130);
        combo1->SetHeight(24);
        combo1->SetEnable(1);
        combo1->SetVisible(1);
        combo1->SetText(IcountToMipsStr(icount));
        combo1->SetItems(IcountToMipsItens(buffer));
        combo1->SetTag(3);
        combo1->EvOnComboChange = PICSimLab.board_Event;
        PICSimLab.GetWindow()->CreateChild(combo1);
    }
}

// Destructor called once on board destruction

cboard_Blue_Pill::~cboard_Blue_Pill(void) {
    if (PICSimLab.GetWindow()) {
        PICSimLab.GetWindow()->DestroyChild(label1);
        PICSimLab.GetWindow()->DestroyChild(combo1);
    }
}

// Reset board status

void cboard_Blue_Pill::Reset(void) {
    MReset(1);

    if (PICSimLab.GetStatusBar()) {
        PICSimLab.GetStatusBar()->SetField(2, lxT("Serial: ") + lxString::FromAscii(SERIALDEVICE));
    }

    if (use_spare)
        SpareParts.Reset();

    RegisterRemoteControl();
}

void cboard_Blue_Pill::RegisterRemoteControl(void) {
    output_ids[O_LED]->status = &pins[1].oavalue;
}

// Called ever 1s to refresh status

void cboard_Blue_Pill::RefreshStatus(void) {
    if (serial_open) {
        PICSimLab.GetStatusBar()->SetField(2, lxT("Serial: ") + lxString::FromAscii(SERIALDEVICE));
    } else {
        PICSimLab.GetStatusBar()->SetField(2, lxT("Serial: Error"));
    }
}

// Called to save board preferences in configuration file

void cboard_Blue_Pill::WritePreferences(void) {
    // write selected microcontroller of board_x to preferences
    PICSimLab.SavePrefs(lxT("Blue_Pill_proc"), Proc);
    // write microcontroller clock to preferences
    PICSimLab.SavePrefs(lxT("Blue_Pill_clock"), lxString().Format("%2.1f", PICSimLab.GetClock()));
    // write microcontroller icount to preferences
    PICSimLab.SavePrefs(lxT("Blue_Pill_icount"), itoa(icount));
}

// Called whe configuration file load  preferences

void cboard_Blue_Pill::ReadPreferences(char* name, char* value) {
    // read microcontroller of preferences
    if (!strcmp(name, "Blue_Pill_proc")) {
        Proc = value;
    }
    // read microcontroller clock
    if (!strcmp(name, "Blue_Pill_clock")) {
        PICSimLab.SetClock(atof(value));
    }
    // read microcontroller icount
    if (!strcmp(name, "Blue_Pill_icount")) {
        icount = atoi(value);
        if (combo1) {
            combo1->SetText(IcountToMipsStr(icount));
        }
    }
}

// Event on the board

void cboard_Blue_Pill::EvKeyPress(uint key, uint mask) {}

// Event on the board

void cboard_Blue_Pill::EvKeyRelease(uint key, uint mask) {}

// Event on the board

void cboard_Blue_Pill::EvMouseButtonPress(uint button, uint x, uint y, uint state) {
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
                    break;
                    // if event is over I_RST area then turn off and reset
                case I_RST:
                    /*
                    if (PICSimLab.GetWindow()->
            Get_mcupwr () && reset (-1))//if powered
                     {
                      PICSimLab.Set_mcupwr (0);
                      PICSimLab.Set_mcurst (1);
                     }
                     */
                    Reset();
                    p_RST = 0;
                    break;
            }
        }
    }
}

// Event on the board

void cboard_Blue_Pill::EvMouseButtonRelease(uint button, uint x, uint y, uint state) {
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
                        /*
                                 if (reset (-1))
                                  {
                                   Reset ();
                                  }
                         */
                    }
                    p_RST = 1;
                    break;
            }
        }
    }
}

// Called ever 100ms to draw board
// This is the critical code for simulator running speed

void cboard_Blue_Pill::Draw(CDraw* draw) {
    int i;

    draw->Canvas.Init(Scale, Scale);  // initialize draw context

    // board_x draw
    for (i = 0; i < outputc; i++)              // run over all outputs
    {
        if (!output[i].r)                      // if output shape is a rectangle
        {
            draw->Canvas.SetFgColor(0, 0, 0);  // black

            switch (output[i].id)              // search for color of output
            {
                case O_LED:                    // White using pc13 mean value
                    draw->Canvas.SetColor(pins[1].oavalue, 0, 0);
                    break;
                case O_LPWR:  // Blue using mcupwr value
                    draw->Canvas.SetColor(200 * PICSimLab.GetMcuPwr() + 55, 0, 0);
                    break;
                case O_RST:
                    draw->Canvas.SetColor(100, 100, 100);
                    break;
            }

            if (output[i].id == O_RST) {
                draw->Canvas.Circle(1, output[i].cx, output[i].cy, 15);
                if (p_RST) {
                    draw->Canvas.SetColor(15, 15, 15);
                } else {
                    draw->Canvas.SetColor(55, 55, 55);
                }
                draw->Canvas.Circle(1, output[i].cx, output[i].cy, 13);
            } else {
                draw->Canvas.Rectangle(1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1,
                                       output[i].y2 - output[i].y1);
            }
        }
    }

    // end draw
    draw->Canvas.End();
    draw->Update();
}

void cboard_Blue_Pill::Run_CPU_ns(uint64_t time) {
    static unsigned char pi = 0;
    static unsigned int alm[64];
    static const int pinc = MGetPinCount();

    const float RNSTEP = 200.0 * pinc * inc_ns / TTIMEOUT;

    for (uint64_t c = 0; c < time; c += inc_ns) {
        if (ns_count < inc_ns) {
            // reset pins mean value
            memset(alm, 0, 64 * sizeof(unsigned int));

            // Spare parts window pre process
            if (use_spare)
                SpareParts.PreProcess();

            // j = JUMPSTEPS; //step counter
            pi = 0;
        }

        if (PICSimLab.GetMcuPwr())  // if powered
                                    // for (i = 0; i < NSTEP; i++)  // repeat for number of steps in 100ms
        {
            /*
            if (j >= JUMPSTEPS)//if number of step is bigger than steps to skip
             {
             }
             */
            // verify if a breakpoint is reached if not run one instruction
            MStep();
            InstCounterInc();
            // Oscilloscope window process
            if (use_oscope)
                Oscilloscope.SetSample();
            // Spare parts window process
            if (use_spare)
                SpareParts.Process();

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

        ns_count += inc_ns;
        if (ns_count >= TTIMEOUT) {
            ns_count -= TTIMEOUT;
            //  calculate mean value
            for (pi = 0; pi < MGetPinCount(); pi++) {
                pins[pi].oavalue = (int)((alm[pi] * RNSTEP) + 55);
            }
            // Spare parts window pre post process
            if (use_spare)
                SpareParts.PostProcess();
        }
    }
}

void cboard_Blue_Pill::board_Event(CControl* control) {
    icount = MipsStrToIcount(combo1->GetText().c_str());
    PICSimLab.EndSimulation();
}

lxString cboard_Blue_Pill::MGetPinName(int pin) {
    lxString pinname = "error";

    switch (pin) {
        case 1:
            pinname = "VBAT";
            break;
        case 2:
            pinname = "PC13";
            break;
        case 3:
            pinname = "PC14";
            break;
        case 4:
            pinname = "PC15";
            break;
        case 5:
            pinname = "PD0";
            break;
        case 6:
            pinname = "PD1";
            break;
        case 7:
            pinname = "NRST";
            break;
        case 8:
            pinname = "VSSA";
            break;
        case 9:
            pinname = "VDDA";
            break;
        case 10:
            pinname = "PA0";
            break;
        case 11:
            pinname = "PA1";
            break;
        case 12:
            pinname = "PA2";
            break;
        case 13:
            pinname = "PA3";
            break;
        case 14:
            pinname = "PA4";
            break;
        case 15:
            pinname = "PA5";
            break;
        case 16:
            pinname = "PA6";
            break;
        case 17:
            pinname = "PA7";
            break;
        case 18:
            pinname = "PB0";
            break;
        case 19:
            pinname = "PB1";
            break;
        case 20:
            pinname = "PB2";
            break;
        case 21:
            pinname = "PB10";
            break;
        case 22:
            pinname = "PB11";
            break;
        case 23:
            pinname = "VSS";
            break;
        case 24:
            pinname = "VDD";
            break;
        case 25:
            pinname = "PB12";
            break;
        case 26:
            pinname = "PB13";
            break;
        case 27:
            pinname = "PB14";
            break;
        case 28:
            pinname = "PB15";
            break;
        case 29:
            pinname = "PA8";
            break;
        case 30:
            pinname = "PA9";
            break;
        case 31:
            pinname = "PA10";
            break;
        case 32:
            pinname = "PA11";
            break;
        case 33:
            pinname = "PA12";
            break;
        case 34:
            pinname = "PA13";
            break;
        case 35:
            pinname = "VSS";
            break;
        case 36:
            pinname = "VDD";
            break;
        case 37:
            pinname = "PA14";
            break;
        case 38:
            pinname = "PA15";
            break;
        case 39:
            pinname = "PB3";
            break;
        case 40:
            pinname = "PB4";
            break;
        case 41:
            pinname = "PB5";
            break;
        case 42:
            pinname = "PB6";
            break;
        case 43:
            pinname = "PB7";
            break;
        case 44:
            pinname = "BOOT0";
            break;
        case 45:
            pinname = "PB8";
            break;
        case 46:
            pinname = "PB9";
            break;
        case 47:
            pinname = "VSS";
            break;
        case 48:
            pinname = "VDD";
            break;
    }

    return pinname;
}

int cboard_Blue_Pill::MGetPinCount(void) {
    return 48;
}

void cboard_Blue_Pill::MSetAPin(int pin, float value) {
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
}

void cboard_Blue_Pill::PinsExtraConfig(int cfg) {
    if ((cfg & 0x8008) == 0x8008) {  // Alternate function
        int pin = (cfg & 0x7FF0) >> 4;
        int port = cfg & 0x0003;
        uint32_t* uart_afio;
        // int cfg_ = (cfg & 0x000C) >> 2;
        // printf("Extra CFG port(%i) pin[%02i]=0x%02X \n", port, pin, cfg_);

        switch (port) {
            case 0:          // GPIOA
                switch (pin) {
                    case 2:  // uart2
                        uart_afio = qemu_picsimlab_get_internals(0x1000 | 15);
                        if (!(*uart_afio)) {
                            master_uart[1].ctrl_on = 1;
                            master_uart[1].tx_pin = 12;  // pa2
                            master_uart[1].rx_pin = 13;  // pa3
                        }
                        break;
                    case 5:
                    case 6:
                    case 7:  // spi1
                        uart_afio = qemu_picsimlab_get_internals(0x1000 | 53);
                        if (!(*uart_afio)) {
                            master_spi[0].ctrl_on = 1;
                            master_spi[0].sck_pin = 15;   // pa5
                            master_spi[0].copi_pin = 17;  // pa7
                            master_spi[0].cipo_pin = 16;  // pa6
                        }
                        break;
                    case 9:  // uart1
                        uart_afio = qemu_picsimlab_get_internals(0x1000 | 14);
                        if (!(*uart_afio)) {
                            master_uart[0].ctrl_on = 1;
                            master_uart[0].tx_pin = 30;  // pa9
                            master_uart[0].rx_pin = 31;  // pa10
                        }
                        break;
                }
                break;
            case 1:  // GPIOB
                switch (pin) {
                    case 3:
                    case 4:
                    case 5:  // spi1 (alt)
                        uart_afio = qemu_picsimlab_get_internals(0x1000 | 53);
                        if (!(*uart_afio)) {
                            master_spi[0].ctrl_on = 1;
                            master_spi[0].sck_pin = 39;   // pb3
                            master_spi[0].copi_pin = 41;  // pb5
                            master_spi[0].cipo_pin = 40;  // pb4
                        }
                        break;
                    case 6:  // uart1 (alt) and i2c
                        uart_afio = qemu_picsimlab_get_internals(0x1000 | 14);
                        if ((*uart_afio)) {
                            master_uart[0].ctrl_on = 1;
                            master_uart[0].tx_pin = 42;  // pb6
                            master_uart[0].rx_pin = 43;  // pb7
                        }
                        // break;
                    case 7:  // i2c0
                        uart_afio = qemu_picsimlab_get_internals(0x1000 | 42);
                        if (!(*uart_afio)) {
                            master_i2c[0].ctrl_on = 1;
                            master_i2c[0].scl_pin = 42;  // pb6
                            master_i2c[0].sda_pin = 43;  // pb7
                        }
                        break;
                    case 8:
                    case 9:  // i2c0 (alt)
                        uart_afio = qemu_picsimlab_get_internals(0x1000 | 42);
                        if ((*uart_afio)) {
                            master_i2c[0].ctrl_on = 1;
                            master_i2c[0].scl_pin = 45;  // pb8
                            master_i2c[0].sda_pin = 46;  // pb9
                        }
                        break;
                    case 10:  // uart3
                    /*
                        uart_afio = qemu_picsimlab_get_internals(0x1000 | 16);
                        if (!(*uart_afio)) {
                            master_uart[2].ctrl_on = 1;
                            master_uart[2].tx_pin = 21;  // pb10
                            master_uart[2].rx_pin = 22;  // pb11
                        }
                        */
                    case 11:                         // i2c1
                        master_i2c[1].ctrl_on = 1;
                        master_i2c[1].scl_pin = 21;  // pb10
                        master_i2c[1].sda_pin = 22;  // pb11
                        break;

                    case 13:
                    case 14:
                    case 15:                          // spi2
                        master_spi[1].ctrl_on = 1;
                        master_spi[1].sck_pin = 26;   // pb13
                        master_spi[1].copi_pin = 28;  // pb15
                        master_spi[1].cipo_pin = 27;  // pb14
                        break;
                }
                break;
            case 2:  // GPIOC
                break;
            case 3:  // GPIOD
                break;
        }
    }
}

// Register the board in PICSimLab
board_init(BOARD_Blue_Pill_Name, cboard_Blue_Pill);
