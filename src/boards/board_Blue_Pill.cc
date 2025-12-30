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

enum { MIPS = 0, CONFIG = 2 };

// copied from qemu, must be the same
enum {
    STM32_PERIPH_UNDEFINED = -1,
    STM32_RCC_PERIPH = 0,
    STM32_GPIOA,
    STM32_GPIOB,
    STM32_GPIOC,
    STM32_GPIOD,
    STM32_GPIOE,
    STM32_GPIOF,
    STM32_GPIOG,
    STM32_GPIOH,
    STM32_GPIOI,
    STM32_GPIOJ,
    STM32_GPIOK,
    STM32_SYSCFG,
    STM32_AFIO_PERIPH,
    STM32_UART1,
    STM32_UART2,
    STM32_UART3,
    STM32_UART4,
    STM32_UART5,
    STM32_UART6,
    STM32_UART7,
    STM32_UART8,
    STM32_ADC1,
    STM32_ADC2,
    STM32_ADC3,
    STM32_DAC,
    STM32_TIM1,
    STM32_TIM2,
    STM32_TIM3,
    STM32_TIM4,
    STM32_TIM5,
    STM32_TIM6,
    STM32_TIM7,
    STM32_TIM8,
    STM32_TIM9,
    STM32_TIM10,
    STM32_TIM11,
    STM32_TIM12,
    STM32_TIM13,
    STM32_TIM14,
    STM32_BKP,
    STM32_PWR,
    STM32_I2C1,
    STM32_I2C2,
    STM32_I2C3,
    STM32_I2S2,
    STM32_I2S3,
    STM32_WWDG,
    STM32_IWDG,
    STM32_CAN1,
    STM32_CAN2,
    STM32_CAN,
    STM32_USB,
    STM32_SPI1,
    STM32_SPI2,
    STM32_SPI3,
    STM32_EXTI_PERIPH,
    STM32_SDIO,
    STM32_FSMC,
    STM32_RTC,
    STM32_COMP,
    STM_LCD,
    STM32_CRC,
    STM32_DMA1,
    STM32_DMA2,
    STM32_DCMI_PERIPH,
    STM32_CRYP_PERIPH,
    STM32_HASH_PERIPH,
    STM32_RNG_PERIPH,

    STM32_FLASH,
    STM32_FLASH_REGS,
    STM32_PERIPH_COUNT,
};

#define A 0x1000
#define B 0x2000
#define C 0x3000
#define D 0x4000

static const short int pinmap[49] = {
    48,      // number of pins
    -1,      // 1 VBAT
    C | 13,  // 2 C13
    C | 14,  // 3 C14
    C | 15,  // 4 C15
    D | 0,   // 5 D0
    D | 1,   // 6 D1
    -1,      // 7 NRST
    -1,      // 8 VSSA
    -1,      // 9 VDDA
    A | 0,   // 10 A0
    A | 1,   // 11 A1
    A | 2,   // 12 A2
    A | 3,   // 13 A3
    A | 4,   // 14 A4
    A | 5,   // 15 A5
    A | 6,   // 16 A6
    A | 7,   // 17 A7
    B | 0,   // 18 B0
    B | 1,   // 19 B1
    B | 2,   // 20 B2
    B | 10,  // 21 B10
    B | 11,  // 22 B11
    -1,      // 23 VSS
    -1,      // 24 VDD
    B | 12,  // 25 B12
    B | 13,  // 26 B13
    B | 14,  // 27 B14
    B | 15,  // 28 B15
    A | 8,   // 29 A8
    A | 9,   // 30 A9
    A | 10,  // 31 A10
    A | 11,  // 32 A11
    A | 12,  // 33 A12
    A | 13,  // 34 A13
    -1,      // 35 VSS
    -1,      // 36 VDD
    A | 14,  // 37 A14
    A | 15,  // 38 A15
    B | 3,   // 39 B3
    B | 4,   // 40 B4
    B | 5,   // 41 B5
    B | 6,   // 42 B6
    B | 7,   // 43 B7
    -1,      // 44 BOOT0
    B | 8,   // 45 B8
    B | 9,   // 46 B9
    -1,      // 47 VSS
    -1       // 48 VDD
};

static unsigned char iopin(const unsigned short port, const unsigned int num) {
    unsigned char pin = 0;
    unsigned short value = port | num;
    for (int i = 0; i < 49; i++) {
        if (pinmap[i] == value) {
            pin = i;
        }
    }
    return pin;
}

const short int* cboard_Blue_Pill::GetPinMap(void) {
    return pinmap;
}

// return the input ids numbers of names used in input map

unsigned short cboard_Blue_Pill::GetInputId(char* name) {
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

unsigned short cboard_Blue_Pill::GetOutputId(char* name) {
    if (strcmp(name, "LD_LED") == 0)
        return O_LED;
    if (strcmp(name, "LD_PWR") == 0)
        return O_LPWR;
    if (strcmp(name, "PB_RST") == 0)
        return O_RST;

    printf("Error output '%s' don't have a valid id! \n", name);
    return INVALID_ID;
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

    bitbang_pwm_init(&pwm_out, this, 20);
    bitbang_out_init(&rmt_out, this, 0);

    PICSimLab.UpdateGUI(MIPS, GT_COMBO, GA_ADD, (void*)"Qemu CPU MIPS");
    buffer[0] = ',';
    IcountToMipsItens(buffer + 1);
    PICSimLab.UpdateGUI(MIPS, GT_COMBO, GA_SET, (void*)buffer);
    PICSimLab.UpdateGUI(MIPS, GT_COMBO, GA_SET, (void*)IcountToMipsStr(icount));

    PICSimLab.UpdateGUI(CONFIG, GT_BUTTON, GA_ADD, (void*)"Config Qemu");

    wconfigId = PICSimLab.WindowCmd(-1, "window1", PWA_WINDOWCREATE, NULL);
}

// Destructor called once on board destruction

cboard_Blue_Pill::~cboard_Blue_Pill(void) {
    PICSimLab.UpdateGUI(MIPS, GT_COMBO, GA_DEL, NULL);
    PICSimLab.WindowCmd(wconfigId, NULL, PWA_WINDOWDESTROY, NULL);
    bitbang_pwm_end(&pwm_out);
}

// Reset board status

void cboard_Blue_Pill::Reset(void) {
    if (qemu_started != 1) {
        return;
    }

    MReset(1);

    uint32_t baud_rate = *qemu_picsimlab_get_internals(QEMU_INTERNAL_UART0_BAUD);
    bitbang_uart_set_speed(&master_uart[0], baud_rate);
    PICSimLab.UpdateStatus(PS_SERIAL, "HWSerial: " + std::string(SERIALDEVICE) + "(" + std::to_string(baud_rate) + ")");

    if (use_spare)
        SpareParts.Reset();

    RegisterRemoteControl();
}

void cboard_Blue_Pill::RegisterRemoteControl(void) {
    output_ids[O_LED]->status = &pins[1].oavalue;
}

// Called to save board preferences in configuration file

void cboard_Blue_Pill::WritePreferences(void) {
    // write selected microcontroller of board_x to preferences
    PICSimLab.SavePrefs("Blue_Pill_proc", Proc);
    // write microcontroller clock to preferences
    PICSimLab.SavePrefs("Blue_Pill_clock", FloatStrFormat("%2.1f", PICSimLab.GetClock()));
    // write microcontroller icount to preferences
    PICSimLab.SavePrefs("Blue_Pill_icount", std::to_string(icount));

    PICSimLab.SavePrefs("Blue_Pill_cfgeserial", std::to_string(ConfEnableSerial));
    PICSimLab.SavePrefs("Blue_Pill_cfgwgdb", std::to_string(ConfigWaitGdb));
    PICSimLab.SavePrefs("Blue_Pill_cfguextra", std::to_string(use_cmdline_extra));
    PICSimLab.SavePrefs("Blue_Pill_cmdextra", cmdline_extra);
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
        PICSimLab.UpdateGUI(MIPS, GT_COMBO, GA_SET, (void*)IcountToMipsStr(icount));
    }

    if (!strcmp(name, "Blue_Pill_cfgeserial")) {
        ConfEnableSerial = atoi(value);
    }
    if (!strcmp(name, "Blue_Pill_cfgwgdb")) {
        ConfigWaitGdb = atoi(value);
    }
    if (!strcmp(name, "Blue_Pill_cfguextra")) {
        use_cmdline_extra = atoi(value);
    }
    if (!strcmp(name, "Blue_Pill_cmdextra")) {
        cmdline_extra = value;
    }
}

// Event on the board

void cboard_Blue_Pill::EvKeyPress(unsigned int key, unsigned int mask) {}

// Event on the board

void cboard_Blue_Pill::EvKeyRelease(unsigned int key, unsigned int mask) {}

// Event on the board

void cboard_Blue_Pill::EvMouseButtonPress(unsigned int button, unsigned int x, unsigned int y, unsigned int state) {
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
                    if (PICSimLab.Get_mcupwr () && reset (-1))//if powered
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

void cboard_Blue_Pill::EvMouseButtonRelease(unsigned int button, unsigned int x, unsigned int y, unsigned int state) {
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

void cboard_Blue_Pill::Draw(void) {
    int i;

    PICSimLab.CanvasCmd({.cmd = CC_INIT, .Init{Scale, Scale, 0}});  // initialize draw context
    PICSimLab.CanvasCmd({.cmd = CC_SETFONTWEIGHT, .SetFontWeight{CC_FONTWEIGHT_BOLD}});

    // board_x draw
    for (i = 0; i < outputc; i++)  // run over all outputs
    {
        if (!output[i].r)  // if output shape is a rectangle
        {
            PICSimLab.CanvasCmd({.cmd = CC_SETFGCOLOR, .SetFgColor{0, 0, 0}});  // black

            switch (output[i].id)  // search for color of output
            {
                case O_LED:  // White using pc13 mean value
                    PICSimLab.CanvasCmd({.cmd = CC_SETCOLOR, .SetColor{(unsigned int)(310 - pins[1].oavalue), 0, 0}});
                    break;
                case O_LPWR:  // Blue using mcupwr value
                    PICSimLab.CanvasCmd(
                        {.cmd = CC_SETCOLOR, .SetColor{(unsigned int)(200 * PICSimLab.GetMcuPwr() + 55), 0, 0}});
                    break;
                case O_RST:
                    PICSimLab.CanvasCmd({.cmd = CC_SETCOLOR, .SetColor{100, 100, 100}});
                    break;
            }

            if (output[i].id == O_RST) {
                PICSimLab.CanvasCmd({.cmd = CC_CIRCLE, .Circle{1, output[i].cx, output[i].cy, 15}});
                if (p_RST) {
                    PICSimLab.CanvasCmd({.cmd = CC_SETCOLOR, .SetColor{15, 15, 15}});
                } else {
                    PICSimLab.CanvasCmd({.cmd = CC_SETCOLOR, .SetColor{55, 55, 55}});
                }
                PICSimLab.CanvasCmd({.cmd = CC_CIRCLE, .Circle{1, output[i].cx, output[i].cy, 13}});
            } else {
                PICSimLab.CanvasCmd({.cmd = CC_RECTANGLE,
                                     .Rectangle{1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1,
                                                output[i].y2 - output[i].y1}});
            }
        }
    }

    // end draw
    PICSimLab.CanvasCmd({.cmd = CC_END});
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
                                    // for (i = 0; i < NSTEP;
                                    // i++)  // repeat for number
                                    // of steps in 100ms
        {
            /*
            if (j >= JUMPSTEPS)//if number of step is bigger
            than steps to skip
             {
             }
             */
            // verify if a breakpoint is reached if not run
            // one instruction
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
                if (j >= JUMPSTEPS)//if number of step is
               bigger than steps to skip
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

void cboard_Blue_Pill::board_Event(const char* controlname) {
    char line[128] = "";
    PICSimLab.UpdateGUI(MIPS, GT_COMBO, GA_GET, (void*)line);
    icount = MipsStrToIcount(line);
    PICSimLab.EndSimulation();
}

std::string cboard_Blue_Pill::MGetPinName(int pin) {
    std::string pinname = "error";

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
                // printf("Analog channel %02X =
                // %i\n",channel,svalue);
            }
        }
    }
}

void cboard_Blue_Pill::board_ButtonEvent(const char* controlname, unsigned int button, unsigned int x, unsigned int y,
                                         unsigned int state) {
    if (!strcmp(controlname, "b_button2")) {
        std::string fname = PICSimLab.GetSharePath() +
                            "boards"
                            "/" BOARD_Blue_Pill_Name +
                            "/config.lxrad";

        if (PICSimLab.SystemCmd(PSC_FILEEXISTS, fname.c_str())) {
            if (PICSimLab.WindowCmd(wconfigId, NULL, PWA_WINDOWLOADXML, fname.c_str())) {
                char buff[2048];
                char line[1024];
                strncpy(buff, (const char*)cmdline.c_str(), 2047);

                char* arg = strtok(buff, " \n");
                line[0] = 0;

                while (arg) {
                    if (!line[0]) {
                        strcpy(line, arg);
                        if (line[0] != '-') {
                            PICSimLab.WindowCmd(wconfigId, "text1", PWA_TEXTADDLINE, line);
                            line[0] = 0;
                        }
                    } else {
                        if (arg[0] == '-') {
                            PICSimLab.WindowCmd(wconfigId, "text1", PWA_TEXTADDLINE, line);
                            strcpy(line, arg);
                        } else {
                            strcat(line, " ");
                            strcat(line, arg);
                            PICSimLab.WindowCmd(wconfigId, "text1", PWA_TEXTADDLINE, line);
                            line[0] = 0;
                        }
                    }
                    arg = strtok(NULL, " \n");

                    if ((!arg && (line[0] == '-')) || ((arg) && (arg[0] == '-') && (line[0] == '-'))) {
                        PICSimLab.WindowCmd(wconfigId, "text1", PWA_TEXTADDLINE, line);
                        line[0] = 0;
                    }
                }

                if (cmdline_extra.length()) {
                    strncpy(buff, (const char*)cmdline_extra.c_str(), 2047);

                    arg = strtok(buff, " \n");
                    line[0] = 0;

                    while (arg) {
                        if (!line[0]) {
                            strcpy(line, arg);
                            if (line[0] != '-') {
                                PICSimLab.WindowCmd(wconfigId, "text2", PWA_TEXTADDLINE, line);
                                line[0] = 0;
                            }
                        } else {
                            if (arg[0] == '-') {
                                PICSimLab.WindowCmd(wconfigId, "text2", PWA_TEXTADDLINE, line);
                                strcpy(line, arg);
                            } else {
                                strcat(line, " ");
                                strcat(line, arg);
                                PICSimLab.WindowCmd(wconfigId, "text2", PWA_TEXTADDLINE, line);
                                line[0] = 0;
                            }
                        }
                        arg = strtok(NULL, " \n");

                        if ((!arg && (line[0] == '-')) || ((arg) && (arg[0] == '-') && (line[0] == '-'))) {
                            PICSimLab.WindowCmd(wconfigId, "text2", PWA_TEXTADDLINE, line);
                            line[0] = 0;
                        }
                    }
                } else {
                    PICSimLab.WindowCmd(wconfigId, "text2", PWA_TEXTCLEAR, NULL);
                }

                PICSimLab.WindowCmd(wconfigId, "checkbox3", PWA_CHECKBOXSETCHECK,
                                    std::to_string(use_cmdline_extra).c_str());
                PICSimLab.WindowCmd(wconfigId, "checkbox5", PWA_CHECKBOXSETCHECK,
                                    std::to_string(ConfEnableSerial).c_str());
                PICSimLab.WindowCmd(wconfigId, "checkbox6", PWA_CHECKBOXSETCHECK,
                                    std::to_string(ConfigWaitGdb).c_str());

                PICSimLab.WindowCmd(wconfigId, "button1", PWA_BUTTONBOARDEV, "1");
                PICSimLab.WindowCmd(wconfigId, "button2", PWA_BUTTONBOARDEV, "1");

                int x, y;
                PICSimLab.WindowCmd(PW_MAIN, NULL, PWA_GETX, NULL, &x);
                PICSimLab.WindowCmd(PW_MAIN, NULL, PWA_GETY, NULL, &y);

                PICSimLab.WindowCmd(wconfigId, NULL, PWA_SETX, std::to_string(x + 50).c_str());
                PICSimLab.WindowCmd(wconfigId, NULL, PWA_SETY, std::to_string(y + 50).c_str());

                PICSimLab.WindowCmd(wconfigId, NULL, PWA_WINDOWSHOWEX, NULL);
            }
        } else {
            PICSimLab.RegisterError("File " + fname + " not found!");
        }
    } else if (!strcmp(controlname, "button1")) {
        PICSimLab.WindowCmd(wconfigId, "checkbox3", PWA_CHECKBOXGETCHECK, NULL, &use_cmdline_extra);
        PICSimLab.WindowCmd(wconfigId, "checkbox5", PWA_CHECKBOXGETCHECK, NULL, &ConfEnableSerial);
        PICSimLab.WindowCmd(wconfigId, "checkbox6", PWA_CHECKBOXGETCHECK, NULL, &ConfigWaitGdb);

        cmdline_extra = "";
        unsigned int lc;
        PICSimLab.WindowCmd(wconfigId, "text2", PWA_TEXTGETLINECOUNT, NULL, &lc);
        for (unsigned int i = 0; i < lc; i++) {
            char buff[256];
            PICSimLab.WindowCmd(wconfigId, "text2", PWA_TEXTGETLINE, std::to_string(i).c_str(), buff);
            cmdline_extra += buff;
            cmdline_extra += " ";
        }
        PICSimLab.WindowCmd(wconfigId, NULL, PWA_WINDOWHIDEEX, NULL);
        PICSimLab.EndSimulation();
    } else if (!strcmp(controlname, "button2")) {
        PICSimLab.WindowCmd(wconfigId, NULL, PWA_WINDOWHIDEEX, NULL);
    }
}

void cboard_Blue_Pill::PinsExtraConfig(int cfg) {
    if ((cfg & 0xF00008) == 0x800008) {  // Alternate function
        int pin = (cfg & 0x0FF0) >> 4;
        int port = cfg & 0x0003;
        uint32_t* afio;
        // int cfg_ = (cfg & 0x000C) >> 2;
        // printf("Extra CFG Alternate function port(%c)
        // pin[%02i]=0x%02X \n", port + 'A', pin, cfg_);

        switch (port) {
            case 0:  // GPIOA
                switch (pin) {
                    case 0:
                        // tim2 ch1
                        afio = qemu_picsimlab_get_internals(0x1000 | STM32_TIM2);
                        if (*afio == 0) {
                            bitbang_pwm_set_pin(&pwm_out, (1 << 2) + 0, iopin(A, 0));
                        }
                        break;
                    case 1:
                        // tim2 ch2
                        afio = qemu_picsimlab_get_internals(0x1000 | STM32_TIM2);
                        if (*afio == 0) {
                            bitbang_pwm_set_pin(&pwm_out, (1 << 2) + 1, iopin(A, 1));
                        }
                        break;
                    case 2:
                        // uart2
                        afio = qemu_picsimlab_get_internals(0x1000 | STM32_UART2);
                        if (!(*afio)) {
                            master_uart[1].tx_pin = iopin(A, 2);
                            master_uart[1].rx_pin = iopin(A, 3);
                            master_uart[1].ctrl_on = 1;
                        }
                        // tim2 ch3
                        afio = qemu_picsimlab_get_internals(0x1000 | STM32_TIM2);
                        if (*afio == 0) {
                            bitbang_pwm_set_pin(&pwm_out, (1 << 2) + 2, iopin(A, 2));
                        }
                        break;
                    case 3:
                        // uart2
                        afio = qemu_picsimlab_get_internals(0x1000 | STM32_UART2);
                        if (!(*afio)) {
                            master_uart[1].tx_pin = iopin(A, 2);
                            master_uart[1].rx_pin = iopin(A, 3);
                            master_uart[1].ctrl_on = 1;
                        }
                        // tim2 ch4
                        afio = qemu_picsimlab_get_internals(0x1000 | STM32_TIM2);
                        if (*afio == 0) {
                            bitbang_pwm_set_pin(&pwm_out, (1 << 2) + 3, iopin(A, 3));
                        }
                        break;
                    case 4:
                        break;
                    case 5:
                        // spi1
                        afio = qemu_picsimlab_get_internals(0x1000 | STM32_SPI1);
                        if (!(*afio)) {
                            master_spi[0].sck_pin = iopin(A, 5);
                            master_spi[0].copi_pin = iopin(A, 7);
                            master_spi[0].cipo_pin = iopin(A, 6);
                            master_spi[0].ctrl_on = 1;
                        }
                        break;
                    case 6:
                        // spi1
                        afio = qemu_picsimlab_get_internals(0x1000 | STM32_SPI1);
                        if (!(*afio)) {
                            master_spi[0].sck_pin = iopin(A, 5);
                            master_spi[0].copi_pin = iopin(A, 7);
                            master_spi[0].cipo_pin = iopin(A, 6);
                            master_spi[0].ctrl_on = 1;
                        }
                        // tim3 ch1
                        afio = qemu_picsimlab_get_internals(0x1000 | STM32_TIM3);
                        if (*afio == 0) {
                            bitbang_pwm_set_pin(&pwm_out, (2 << 2) + 0, iopin(A, 6));
                        }
                        break;
                    case 7:
                        // spi1
                        afio = qemu_picsimlab_get_internals(0x1000 | STM32_SPI1);
                        if (!(*afio)) {
                            master_spi[0].sck_pin = iopin(A, 5);
                            master_spi[0].copi_pin = iopin(A, 7);
                            master_spi[0].cipo_pin = iopin(A, 6);
                            master_spi[0].ctrl_on = 1;
                        }
                        // tim1_ch1n (alt)
                        afio = qemu_picsimlab_get_internals(0x1000 | STM32_TIM1);
                        if (*afio == 1) {
                            bitbang_pwm_set_pin(&pwm_out, (0 << 2) + 0, iopin(A, 7));
                        }
                        // tim3 ch2
                        afio = qemu_picsimlab_get_internals(0x1000 | STM32_TIM3);
                        if (*afio == 0) {
                            bitbang_pwm_set_pin(&pwm_out, (2 << 2) + 1, iopin(A, 7));
                        }
                        break;
                    case 8:
                        // tim1 ch1
                        afio = qemu_picsimlab_get_internals(0x1000 | STM32_TIM1);
                        if (*afio == 0) {
                            bitbang_pwm_set_pin(&pwm_out, (0 << 2) + 0, iopin(A, 8));
                        }
                        break;
                    case 9:
                        // uart1
                        afio = qemu_picsimlab_get_internals(0x1000 | STM32_UART1);
                        if (!(*afio)) {
                            master_uart[0].tx_pin = iopin(A, 9);
                            master_uart[0].rx_pin = iopin(A, 10);
                            master_uart[0].ctrl_on = 1;
                        }
                        // tim1 ch2
                        afio = qemu_picsimlab_get_internals(0x1000 | STM32_TIM1);
                        if (*afio == 0) {
                            bitbang_pwm_set_pin(&pwm_out, (0 << 2) + 1, iopin(A, 9));
                        }
                        break;
                    case 10:
                        // uart1
                        afio = qemu_picsimlab_get_internals(0x1000 | STM32_UART1);
                        if (!(*afio)) {
                            master_uart[0].tx_pin = iopin(A, 9);
                            master_uart[0].rx_pin = iopin(A, 10);
                            master_uart[0].ctrl_on = 1;
                        }
                        // tim1 ch3
                        afio = qemu_picsimlab_get_internals(0x1000 | STM32_TIM1);
                        if (*afio == 0) {
                            bitbang_pwm_set_pin(&pwm_out, (0 << 2) + 2, iopin(A, 10));
                        }
                        break;
                    case 11:
                        // tim1 ch4
                        afio = qemu_picsimlab_get_internals(0x1000 | STM32_TIM1);
                        if (*afio == 0) {
                            bitbang_pwm_set_pin(&pwm_out, (0 << 2) + 3, iopin(A, 11));
                        }
                        break;
                    case 12:
                        break;
                    case 13:
                        break;
                    case 14:
                        break;
                    case 15:
                        break;
                }
                break;
            case 1:  // GPIOB
                switch (pin) {
                    case 0:
                        // tim1_ch2n (alt)
                        afio = qemu_picsimlab_get_internals(0x1000 | STM32_TIM1);
                        if (*afio == 1) {
                            bitbang_pwm_set_pin(&pwm_out, (0 << 2) + 1, iopin(B, 0));
                        }
                        // tim3_ch3
                        afio = qemu_picsimlab_get_internals(0x1000 | STM32_TIM3);
                        if (*afio == 0) {
                            bitbang_pwm_set_pin(&pwm_out, (2 << 2) + 2, iopin(B, 0));
                        }
                        break;
                    case 1:
                        // tim1_ch3n (alt)
                        afio = qemu_picsimlab_get_internals(0x1000 | STM32_TIM1);
                        if (*afio == 1) {
                            bitbang_pwm_set_pin(&pwm_out, (0 << 2) + 2, iopin(B, 1));
                        }
                        // tim3_ch4
                        afio = qemu_picsimlab_get_internals(0x1000 | STM32_TIM3);
                        if (*afio == 0) {
                            bitbang_pwm_set_pin(&pwm_out, (2 << 2) + 3, iopin(B, 1));
                        }
                        break;
                    case 2:
                        break;
                    case 3:
                        // tim2_ch2n (alt)
                        afio = qemu_picsimlab_get_internals(0x1000 | STM32_TIM2);
                        if (*afio == 1) {
                            bitbang_pwm_set_pin(&pwm_out, (1 << 2) + 1, iopin(B, 3));
                        }
                        break;
                    case 4:
                        // tim3_ch1n (alt)
                        afio = qemu_picsimlab_get_internals(0x1000 | STM32_TIM3);
                        if (*afio == 2) {
                            bitbang_pwm_set_pin(&pwm_out, (2 << 2) + 0, iopin(B, 4));
                        }
                        break;
                    case 5:
                        // spi1 (alt)
                        afio = qemu_picsimlab_get_internals(0x1000 | STM32_SPI1);
                        if (!(*afio)) {
                            master_spi[0].sck_pin = iopin(B, 3);
                            master_spi[0].copi_pin = iopin(B, 5);
                            master_spi[0].cipo_pin = iopin(B, 4);
                            master_spi[0].ctrl_on = 1;
                        }
                        // tim3_ch2n (alt)
                        afio = qemu_picsimlab_get_internals(0x1000 | STM32_TIM3);
                        if (*afio == 2) {
                            bitbang_pwm_set_pin(&pwm_out, (2 << 2) + 1, iopin(B, 5));
                        }
                        break;
                    case 6:
                        // uart1 (alt)
                        afio = qemu_picsimlab_get_internals(0x1000 | STM32_UART1);
                        if ((*afio)) {
                            master_uart[0].tx_pin = iopin(B, 6);
                            master_uart[0].rx_pin = iopin(B, 7);
                            master_uart[0].ctrl_on = 1;
                        }
                        // i2c
                        afio = qemu_picsimlab_get_internals(0x1000 | STM32_I2C1);
                        if (!(*afio)) {
                            master_i2c[0].scl_pin = iopin(B, 6);
                            master_i2c[0].sda_pin = iopin(B, 7);
                            master_i2c[0].ctrl_on = 1;
                        }
                        // tim4 ch1
                        afio = qemu_picsimlab_get_internals(0x1000 | STM32_TIM4);
                        if (*afio == 0) {
                            bitbang_pwm_set_pin(&pwm_out, (3 << 2) + 0, iopin(B, 6));
                        }
                        break;
                    case 7:
                        // uart1 (alt)
                        afio = qemu_picsimlab_get_internals(0x1000 | STM32_UART1);
                        if ((*afio)) {
                            master_uart[0].tx_pin = iopin(B, 6);
                            master_uart[0].rx_pin = iopin(B, 7);
                            master_uart[0].ctrl_on = 1;
                        }
                        // i2c0
                        afio = qemu_picsimlab_get_internals(0x1000 | STM32_I2C1);
                        if (!(*afio)) {
                            master_i2c[0].scl_pin = iopin(B, 6);
                            master_i2c[0].sda_pin = iopin(B, 7);
                            master_i2c[0].ctrl_on = 1;
                        }
                        // tim4 ch2
                        afio = qemu_picsimlab_get_internals(0x1000 | STM32_TIM4);
                        if (*afio == 0) {
                            bitbang_pwm_set_pin(&pwm_out, (3 << 2) + 1, iopin(B, 7));
                        }
                        break;
                    case 8:
                        // i2c0 (alt)
                        afio = qemu_picsimlab_get_internals(0x1000 | STM32_I2C1);
                        if ((*afio)) {
                            master_i2c[0].scl_pin = iopin(B, 8);
                            master_i2c[0].sda_pin = iopin(B, 9);
                            master_i2c[0].ctrl_on = 1;
                        }
                        // tim4 ch3
                        afio = qemu_picsimlab_get_internals(0x1000 | STM32_TIM4);
                        if (*afio == 0) {
                            bitbang_pwm_set_pin(&pwm_out, (3 << 2) + 2, iopin(B, 8));
                        }
                        break;
                    case 9:
                        // i2c0 (alt)
                        afio = qemu_picsimlab_get_internals(0x1000 | STM32_I2C1);
                        if ((*afio)) {
                            master_i2c[0].scl_pin = iopin(B, 8);
                            master_i2c[0].sda_pin = iopin(B, 9);
                            master_i2c[0].ctrl_on = 1;
                        }
                        // tim4 ch4
                        afio = qemu_picsimlab_get_internals(0x1000 | STM32_TIM4);
                        if (*afio == 0) {
                            bitbang_pwm_set_pin(&pwm_out, (3 << 2) + 3, iopin(B, 9));
                        }
                        break;
                    case 10:
                        // tim2_ch3n (alt)
                        afio = qemu_picsimlab_get_internals(0x1000 | STM32_TIM2);
                        if (*afio == 1) {
                            bitbang_pwm_set_pin(&pwm_out, (1 << 2) + 2, iopin(B, 10));
                        }
                        /*
                        // uart3
                        uart_afio =
                        qemu_picsimlab_get_internals(0x1000
                        | STM32_UART3); if (!(*uart_afio)) {
                            master_uart[2].tx_pin = iopin(B,
                        10); master_uart[2].rx_pin =
                        iopin(B, 11); master_uart[2].ctrl_on
                        = 1; break
                        }
                        */
                        // i2c1
                        afio = qemu_picsimlab_get_internals(0x1000 | STM32_I2C1);
                        if (*afio == 0) {
                            master_i2c[1].scl_pin = iopin(B, 10);
                            master_i2c[1].sda_pin = iopin(B, 11);
                            master_i2c[1].ctrl_on = 1;
                        }
                        break;
                    case 11:
                        // tim2_ch4n (alt)
                        afio = qemu_picsimlab_get_internals(0x1000 | STM32_TIM2);
                        if (*afio == 1) {
                            bitbang_pwm_set_pin(&pwm_out, (1 << 2) + 3, iopin(B, 10));
                        }
                        /*
                        // uart3
                        uart_afio =
                        qemu_picsimlab_get_internals(0x1000
                        | STM32_UART3); if (!(*uart_afio)) {
                            master_uart[2].tx_pin = iopin(B,
                        10); master_uart[2].rx_pin =
                        iopin(B, 11); master_uart[2].ctrl_on
                        = 1; break;
                        }
                        */
                        // i2c1
                        afio = qemu_picsimlab_get_internals(0x1000 | STM32_I2C1);
                        if (*afio == 0) {
                            master_i2c[1].scl_pin = iopin(B, 10);
                            master_i2c[1].sda_pin = iopin(B, 11);
                            master_i2c[1].ctrl_on = 1;
                        }
                        break;
                    case 12:
                        break;
                    case 13:
                    case 14:
                    case 15:
                        // spi2
                        afio = qemu_picsimlab_get_internals(0x1000 | STM32_SPI2);
                        if (*afio == 0) {
                            master_spi[1].sck_pin = iopin(B, 13);
                            master_spi[1].copi_pin = iopin(B, 15);
                            master_spi[1].cipo_pin = iopin(B, 14);
                            master_spi[1].ctrl_on = 1;
                        }
                        break;
                }
                break;
                /*
            case 2:  // GPIOC
                switch (pin) {
                    case 13:
                        break;
                    case 14:
                        break;
                    case 15:
                        break;
                }
                break;
            case 3:  // GPIOD
                switch (pin) {
                    case 0:
                        break;
                    case 1:
                        break;
                }
                break;
                */
        }
    } else if ((cfg & 0xF00000) == 0xC00000) {  // timer ccmr1 function
        int duty = (cfg & 0xFFFF0) >> 4;
        int chn = (cfg & 0x000C) >> 2;
        int timer = cfg & 0x0003;
        // printf("TIM %i chn %i dut set to %i\n", timer + 1, chn + 1, duty);
        bitbang_pwm_set_duty(&pwm_out, (timer << 2) + chn, duty);
    } else if ((cfg & 0xF00000) == 0xD00000) {  // timer frequency
        int freq = (cfg & 0xFFFFC) >> 2;
        int timer = cfg & 0x0003;
        // printf("TIM %i freq set to %i\n", timer + 1, freq);
        bitbang_pwm_set_freq(&pwm_out, (timer << 2) + 0, freq);
        bitbang_pwm_set_freq(&pwm_out, (timer << 2) + 1, freq);
        bitbang_pwm_set_freq(&pwm_out, (timer << 2) + 2, freq);
        bitbang_pwm_set_freq(&pwm_out, (timer << 2) + 3, freq);
    } else if ((cfg & 0xF00000) == 0xE00000) {  // timer enable output
        int ccer = (cfg & 0xFFFFC) >> 2;
        int timer = cfg & 0x0003;
        // printf("TIM %i ccer 0x%04X\n", timer + 1, ccer);
        bitbang_pwm_set_enable(&pwm_out, (timer << 2) + 0, (ccer & 0x0005) > 0);
        bitbang_pwm_set_enable(&pwm_out, (timer << 2) + 1, (ccer & 0x0050) > 0);
        bitbang_pwm_set_enable(&pwm_out, (timer << 2) + 2, (ccer & 0x0500) > 0);
        bitbang_pwm_set_enable(&pwm_out, (timer << 2) + 3, (ccer & 0x5000) > 0);
    }
}

// Register the board in PICSimLab
board_init(BOARD_Blue_Pill_Name, cboard_Blue_Pill);
