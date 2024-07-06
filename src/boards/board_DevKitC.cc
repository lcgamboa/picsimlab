/* ########################################################################

   PICSimLab - Programmable IC Simulator Laboratory

   ########################################################################

   Copyright (c) : 2021-2024  Luis Claudio Gambôa Lopes <lcgamboa@yahoo.com>

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
#include "board_DevKitC.h"
#include "../lib/oscilloscope.h"
#include "../lib/picsimlab.h"
#include "../lib/spareparts.h"

/* ids of inputs of input map*/
enum {
    I_ICSP,  // ICSP connector
    I_PWR,   // Power button
    I_RST,   // Reset button
    I_BOOT   // Boot button
};

/* ids of outputs of output map*/
enum {
    O_LON,   // Power LED
    O_RST,   // Reset button
    O_BOOT,  // Boot button
    O_LED    // User LED
};

enum { MIPS = 0, CONFIG = 2 };

// return the input ids numbers of names used in input map

enum {
    VDD = 1,
    EN,
    IO36,
    IO39,
    IO34,
    IO35,
    IO32,
    IO33,
    IO25,
    IO26,
    IO27,
    IO14,
    IO12,
    GND,
    IO13,
    IO9,
    IO10,
    IO11,
    VIN,
    IO6,
    IO7,
    IO8,
    IO15,
    IO2,
    IO0,
    IO4,
    IO16,
    IO17,
    IO5,
    IO18,
    IO19,
    GND2,
    IO21,
    IO3,
    IO1,
    IO22,
    IO23
};

static int io2pin(int io) {
    switch (io) {
        case 0:
            return IO0;
            break;
        case 1:
            return IO1;
            break;
        case 2:
            return IO2;
            break;
        case 3:
            return IO3;
            break;
        case 4:
            return IO4;
            break;
        case 5:
            return IO5;
            break;
        case 6:
            return IO6;
            break;
        case 7:
            return IO7;
            break;
        case 8:
            return IO8;
            break;
        case 9:
            return IO9;
            break;
        case 10:
            return IO10;
            break;
        case 11:
            return IO11;
            break;
        case 12:
            return IO12;
            break;
        case 13:
            return IO13;
            break;
        case 14:
            return IO14;
            break;
        case 15:
            return IO15;
            break;
        case 16:
            return IO16;
            break;
        case 17:
            return IO17;
            break;
        case 18:
            return IO18;
            break;
        case 19:
            return IO19;
            break;
        case 21:
            return IO21;
            break;
        case 22:
            return IO22;
            break;
        case 23:
            return IO23;
            break;
        case 25:
            return IO25;
            break;
        case 26:
            return IO26;
            break;
        case 27:
            return IO27;
            break;
        case 32:
            return IO32;
            break;
        case 33:
            return IO33;
            break;
        case 34:
            return IO34;
            break;
        case 35:
            return IO35;
            break;
        case 36:
            return IO36;
            break;
        case 39:
            return IO39;
            break;
    }
    return 0;
}

static const short int pinmap[39] = {
    38,  // number of pins
    -1,  // 1-3V3
    -1,  // 2-EN
    36,  // 3-GPIO36
    39,  // 4-GPIO39
    34,  // 5-GPIO34
    35,  // 6-GPIO35
    32,  // 7-GPIO32
    33,  // 8-GPIO33
    25,  // 9-GPIO25
    26,  // 10-GPIO26
    27,  // 11-GPIO27
    14,  // 12-GPIO14
    12,  // 13-GPIO12
    -1,  // 14-GND
    13,  // 15-GPIO13
    9,   // 16-GPIO9
    10,  // 17-GPIO10
    11,  // 18-GPIO11
    -1,  // 19-VIN
    6,   // 20-GPIO6
    7,   // 21-GPIO7
    8,   // 22-GPIO8
    15,  // 23-GPIO15
    2,   // 24-GPIO2
    0,   // 25-GPIO0
    4,   // 26-GPIO4
    16,  // 27-GPIO16
    17,  // 28-GPIO17
    5,   // 29-GPIO5
    18,  // 30-GPIO18
    19,  // 31-GPIO19
    -1,  // 32-GND
    21,  // 33-GPIO21
    3,   // 34-GPIO3
    1,   // 35-GPIO1
    22,  // 36-GPIO22
    23,  // 37-GPIO23
    -1,  // 38-GND
};

const short int* cboard_DevKitC::GetPinMap(void) {
    return pinmap;
}

unsigned short cboard_DevKitC::GetInputId(char* name) {
    if (strcmp(name, "PG_ICSP") == 0)
        return I_ICSP;
    if (strcmp(name, "SW_PWR") == 0)
        return I_PWR;
    if (strcmp(name, "PB_RST") == 0)
        return I_RST;
    if (strcmp(name, "PB_BOOT") == 0)
        return I_BOOT;

    printf("Error input '%s' don't have a valid id! \n", name);
    return INVALID_ID;
}

// return the output ids numbers of names used in output map

unsigned short cboard_DevKitC::GetOutputId(char* name) {
    if (strcmp(name, "LD_ON") == 0)
        return O_LON;
    if (strcmp(name, "PB_RST") == 0)
        return O_RST;
    if (strcmp(name, "PB_BOOT") == 0)
        return O_BOOT;
    if (strcmp(name, "LD_USER") == 0)
        return O_LED;

    printf("Error output '%s' don't have a valid id! \n", name);
    return INVALID_ID;
}

// Constructor called once on board creation

cboard_DevKitC::cboard_DevKitC(void) {
    char buffer[1024];

    SimType = QEMU_SIM_ESP32;
    p_BOOT = 1;

    Proc = "ESP32";  // default microcontroller if none defined in preferences
    ReadMaps();      // Read input and output board maps

    ConfEnableWifi = 1;
    ConfDisableWdt = 1;
    ConfEnableEthernet = 0;

    application_offset = 0x10000;

    master_i2c[0].scl_pin = 0;
    master_i2c[0].sda_pin = 0;
    master_i2c[1].scl_pin = 0;
    master_i2c[1].sda_pin = 0;

    // HSPI
    master_spi[0].sck_pin = 0;
    master_spi[0].copi_pin = 0;
    master_spi[0].cipo_pin = 0;
    master_spi[0].cs_pin[0] = 0;
    master_spi[0].cs_pin[1] = 0;
    master_spi[0].cs_pin[2] = 0;

    // VSPI
    master_spi[1].sck_pin = 0;
    master_spi[1].copi_pin = 0;
    master_spi[1].cipo_pin = 0;
    master_spi[1].cs_pin[0] = 0;
    master_spi[1].cs_pin[1] = 0;
    master_spi[1].cs_pin[2] = 0;

    master_uart[0].tx_pin = 0;
    master_uart[0].rx_pin = IO3;  // FIXME without this Micropython don´t work
    master_uart[1].tx_pin = 0;
    master_uart[1].rx_pin = 0;
    master_uart[2].tx_pin = 0;
    master_uart[2].rx_pin = 0;

    bitbang_pwm_init(&pwm_out, this, 16);
    bitbang_out_init(&rmt_out, this, 8);

    PICSimLab.UpdateGUI(MIPS, GT_COMBO, GA_ADD, (void*)"Qemu CPU MIPS");
    buffer[0] = ',';
    IcountToMipsItens(buffer + 1);
    PICSimLab.UpdateGUI(MIPS, GT_COMBO, GA_SET, (void*)buffer);
    PICSimLab.UpdateGUI(MIPS, GT_COMBO, GA_SET, (void*)IcountToMipsStr(icount));

    PICSimLab.UpdateGUI(CONFIG, GT_BUTTON, GA_ADD, (void*)"Config Qemu");

    wconfigId = PICSimLab.WindowCmd(-1, "window1", PWA_WINDOWCREATE, NULL);
}

// Destructor called once on board destruction

cboard_DevKitC::~cboard_DevKitC(void) {
    PICSimLab.UpdateGUI(MIPS, GT_COMBO, GA_DEL, NULL);
    PICSimLab.UpdateGUI(CONFIG, GT_BUTTON, GA_DEL, NULL);
    PICSimLab.WindowCmd(wconfigId, NULL, PWA_WINDOWDESTROY, NULL);
    bitbang_pwm_end(&pwm_out);
    bitbang_out_end(&rmt_out);
}

// Reset board status

void cboard_DevKitC::Reset(void) {
    if (qemu_started != 1) {
        return;
    }
    uint32_t* strap_mode = qemu_picsimlab_get_internals(QEMU_INTERNAL_STRAP);

    if (p_BOOT) {
        *strap_mode = 0x12;  // SPI_FAST_FLASH_BOOT
    } else {
        *strap_mode = 0x0f;  // UART_BOOT(UART0)
    }

    MReset(1);

    uint32_t baud_rate = *qemu_picsimlab_get_internals(QEMU_INTERNAL_UART0_BAUD);
    bitbang_uart_set_speed(&master_uart[0], baud_rate);
    PICSimLab.UpdateStatus(PS_SERIAL, "Serial: " + std::string(SERIALDEVICE) + "(" + std::to_string(baud_rate) + ")");

    if (use_spare)
        SpareParts.Reset();

    RegisterRemoteControl();
}

void cboard_DevKitC::RegisterRemoteControl(void) {
    output_ids[O_LED]->status = &pins[23].oavalue;
}

// Called ever 1s to refresh status

void cboard_DevKitC::RefreshStatus(void) {
    if (serial_open) {
        uint32_t baud_rate = *qemu_picsimlab_get_internals(QEMU_INTERNAL_UART0_BAUD);
        bitbang_uart_set_speed(&master_uart[0], baud_rate);
        PICSimLab.UpdateStatus(PS_SERIAL,
                               "Serial: " + std::string(SERIALDEVICE) + "(" + std::to_string(baud_rate) + ")");
    } else {
        PICSimLab.UpdateStatus(PS_SERIAL, "Serial: Error");
    }
}

// Called to save board preferences in configuration file

void cboard_DevKitC::WritePreferences(void) {
    // write selected microcontroller of board_x to preferences
    PICSimLab.SavePrefs("ESP32_DevKitC_proc", Proc);
    // write microcontroller clock to preferences
    PICSimLab.SavePrefs("ESP32_DevKitC_clock", FloatStrFormat("%2.1f", PICSimLab.GetClock()));
    // write microcontroller icount to preferences
    PICSimLab.SavePrefs("ESP32_DevKitC_icount", std::to_string(icount));

    PICSimLab.SavePrefs("ESP32_DevKitC_cfgeserial", std::to_string(ConfEnableSerial));
    PICSimLab.SavePrefs("ESP32_DevKitC_cfgewifi", std::to_string(ConfEnableWifi));
    PICSimLab.SavePrefs("ESP32_DevKitC_cfgdwdt", std::to_string(ConfDisableWdt));
    PICSimLab.SavePrefs("ESP32_DevKitC_cfgeeth", std::to_string(ConfEnableEthernet));
    PICSimLab.SavePrefs("ESP32_DevKitC_cfgwgdb", std::to_string(ConfigWaitGdb));
    PICSimLab.SavePrefs("ESP32_DevKitC_cfguextra", std::to_string(use_cmdline_extra));
    PICSimLab.SavePrefs("ESP32_DevKitC_cmdextra", cmdline_extra);
    PICSimLab.SavePrefs("ESP32_DevKitC_app_off", std::to_string(application_offset));
}

// Called whe configuration file load  preferences

void cboard_DevKitC::ReadPreferences(char* name, char* value) {
    // read microcontroller of preferences
    if (!strcmp(name, "ESP32_DevKitC_proc")) {
        Proc = value;
    }
    // read microcontroller clock
    if (!strcmp(name, "ESP32_DevKitC_clock")) {
        PICSimLab.SetClock(atof(value));
    }
    // read microcontroller icount
    if (!strcmp(name, "ESP32_DevKitC_icount")) {
        icount = atoi(value);
        PICSimLab.UpdateGUI(MIPS, GT_COMBO, GA_SET, (void*)IcountToMipsStr(icount));
    }

    if (!strcmp(name, "ESP32_DevKitC_cfgeserial")) {
        ConfEnableSerial = atoi(value);
    }
    if (!strcmp(name, "ESP32_DevKitC_cfgewifi")) {
        ConfEnableWifi = atoi(value);
    }
    if (!strcmp(name, "ESP32_DevKitC_cfgdwdt")) {
        ConfDisableWdt = atoi(value);
    }
    if (!strcmp(name, "ESP32_DevKitC_cfgeeth")) {
        ConfEnableEthernet = atoi(value);
    }
    if (!strcmp(name, "ESP32_DevKitC_cfgwgdb")) {
        ConfigWaitGdb = atoi(value);
    }
    if (!strcmp(name, "ESP32_DevKitC_cfguextra")) {
        use_cmdline_extra = atoi(value);
    }
    if (!strcmp(name, "ESP32_DevKitC_cmdextra")) {
        cmdline_extra = value;
    }
    if (!strcmp(name, "ESP32_DevKitC_app_off")) {
        application_offset = atoi(value);
    }
}

// Event on the board

void cboard_DevKitC::EvKeyPress(unsigned int key, unsigned int mask) {}

// Event on the board

void cboard_DevKitC::EvKeyRelease(unsigned int key, unsigned int mask) {}

// Event on the board

void cboard_DevKitC::EvMouseButtonPress(unsigned int button, unsigned int x, unsigned int y, unsigned int state) {
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
                case I_BOOT:
                    p_BOOT = 0;
                    break;
            }
        }
    }
}

// Event on the board

void cboard_DevKitC::EvMouseButtonRelease(unsigned int button, unsigned int x, unsigned int y, unsigned int state) {
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
                case I_BOOT:
                    p_BOOT = 1;
                    break;
            }
        }
    }
}

// Called ever 100ms to draw board
// This is the critical code for simulator running speed

void cboard_DevKitC::Draw(void) {
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
                case O_LON:  // Blue using mcupwr value
                    PICSimLab.CanvasCmd(
                        {.cmd = CC_SETCOLOR, .SetColor{(unsigned int)(200 * PICSimLab.GetMcuPwr() + 55), 0, 0}});
                    break;
                case O_LED:  // Blue using mcupwr value
                    PICSimLab.CanvasCmd({.cmd = CC_SETCOLOR, .SetColor{0, 0, (unsigned int)pins[23].oavalue}});
                    break;
                case O_RST:
                case O_BOOT:
                    PICSimLab.CanvasCmd({.cmd = CC_SETCOLOR, .SetColor{100, 100, 100}});
                    break;
            }

            if (output[i].id == O_RST) {
                PICSimLab.CanvasCmd({.cmd = CC_CIRCLE, .Circle{1, output[i].cx, output[i].cy, 13}});
                if (p_RST) {
                    PICSimLab.CanvasCmd({.cmd = CC_SETCOLOR, .SetColor{15, 15, 15}});
                } else {
                    PICSimLab.CanvasCmd({.cmd = CC_SETCOLOR, .SetColor{55, 55, 55}});
                }
                PICSimLab.CanvasCmd({.cmd = CC_CIRCLE, .Circle{1, output[i].cx, output[i].cy, 11}});
            } else if (output[i].id == O_BOOT) {
                PICSimLab.CanvasCmd({.cmd = CC_CIRCLE, .Circle{1, output[i].cx, output[i].cy, 13}});
                if (p_BOOT) {
                    PICSimLab.CanvasCmd({.cmd = CC_SETCOLOR, .SetColor{15, 15, 15}});
                } else {
                    PICSimLab.CanvasCmd({.cmd = CC_SETCOLOR, .SetColor{55, 55, 55}});
                }
                PICSimLab.CanvasCmd({.cmd = CC_CIRCLE, .Circle{1, output[i].cx, output[i].cy, 11}});
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

void cboard_DevKitC::Run_CPU_ns(uint64_t time) {
    static unsigned char pi = 0;
    static unsigned int alm[64];
    static const int pinc = MGetPinCount();

    const float RNSTEP = 200.0 * pinc * inc_ns / TTIMEOUT;

    MSetPin(IO0, p_BOOT);

    for (uint64_t c = 0; c < time; c += inc_ns) {
        if (ns_count < inc_ns) {
            // reset pins mean value
            memset(alm, 0, 64 * sizeof(unsigned int));

            // Spare parts window pre
            // process
            if (use_spare)
                SpareParts.PreProcess();

            // j = JUMPSTEPS; //step counter
            pi = 0;
        }

        if (PICSimLab.GetMcuPwr())  // if powered
                                    // for (i =
                                    // 0; i <
                                    // NSTEP;
                                    // i++)  //
                                    // repeat for
                                    // number of
                                    // steps in
                                    // 100ms
        {
            /*
            if (j >= JUMPSTEPS)//if number
            of step is bigger than steps to
            skip
             {
             }
             */
            // verify if a breakpoint is
            // reached if not run one
            // instruction
            MStep();
            InstCounterInc();
            // Oscilloscope window process
            if (use_oscope)
                Oscilloscope.SetSample();
            // Spare parts window process
            if (use_spare)
                SpareParts.Process();

            //  increment mean value counter
            //  if pin is high
            alm[pi] += pins[pi].value;
            pi++;
            if (pi == pinc)
                pi = 0;
            /*
                if (j >= JUMPSTEPS)//if
               number of step is bigger than
               steps to skip
                 {
                  j = -1; //reset counter
                 }

                j++; //counter increment
             */
        }

        ns_count += inc_ns;
        if (ns_count >= TTIMEOUT) {  // every 100ms
            ns_count -= TTIMEOUT;
            //  calculate mean value
            for (pi = 0; pi < MGetPinCount(); pi++) {
                pins[pi].oavalue = (int)((alm[pi] * RNSTEP) + 55);
            }
            // Spare parts window pre post
            // process
            if (use_spare)
                SpareParts.PostProcess();
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
    if (qemu_started != 1) {
        return;
    }

    if (serial_open && PICSimLab.GetUseDSRReset()) {
        static unsigned int status_ = 0;
        unsigned int status = qemu_picsimlab_get_TIOCM();

        if (status_ != status) {
            status_ = status;

            if ((status & CHR_TIOCM_CTS) && !(status & CHR_TIOCM_DSR)) {
                Reset();
            }

            if (!(status & CHR_TIOCM_CTS) && (status & CHR_TIOCM_DSR)) {
                uint32_t* strap_mode = qemu_picsimlab_get_internals(QEMU_INTERNAL_STRAP);
                *strap_mode = 0x0f;  // UART_BOOT(UART0)
                MReset(1);
            }
        }
    }
}

void cboard_DevKitC::board_Event(const char* controlname) {
    char line[128] = "";
    PICSimLab.UpdateGUI(MIPS, GT_COMBO, GA_GET, (void*)line);
    icount = MipsStrToIcount(line);
    PICSimLab.EndSimulation();
}

void cboard_DevKitC::BoardOptions(int* argc, char** argv) {
    if (ConfEnableWifi) {
        strcpy(argv[(*argc)++], "-nic");
        strcpy(argv[(*argc)++],
               "user,model=esp32_wifi,id="
               "u1,net=192.168.4.0/24");
    }
    if (ConfDisableWdt) {
        strcpy(argv[(*argc)++], "-global");
        strcpy(argv[(*argc)++],
               "driver=timer.esp32.timg,"
               "property=wdt_disable,value="
               "true");
    }
    if (ConfEnableEthernet) {
        strcpy(argv[(*argc)++], "-nic");
        strcpy(argv[(*argc)++],
               "user,model=open_eth,id=u2,"
               "net=192.168.3.0/24");
    }
}

void cboard_DevKitC::board_ButtonEvent(const char* controlname, unsigned int button, unsigned int x, unsigned int y,
                                       unsigned int state) {
    if (!strcmp(controlname, "b_button2")) {
        std::string fname = PICSimLab.GetSharePath() +
                            "boards"
                            "/" BOARD_DevKitC_Name +
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

                PICSimLab.WindowCmd(wconfigId, "checkbox1", PWA_CHECKBOXSETCHECK,
                                    std::to_string(ConfEnableWifi).c_str());
                PICSimLab.WindowCmd(wconfigId, "checkbox2", PWA_CHECKBOXSETCHECK,
                                    std::to_string(ConfDisableWdt).c_str());
                PICSimLab.WindowCmd(wconfigId, "checkbox3", PWA_CHECKBOXSETCHECK,
                                    std::to_string(use_cmdline_extra).c_str());
                PICSimLab.WindowCmd(wconfigId, "checkbox4", PWA_CHECKBOXSETCHECK,
                                    std::to_string(ConfEnableEthernet).c_str());
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
        PICSimLab.WindowCmd(wconfigId, "checkbox1", PWA_CHECKBOXGETCHECK, NULL, &ConfEnableWifi);
        PICSimLab.WindowCmd(wconfigId, "checkbox2", PWA_CHECKBOXGETCHECK, NULL, &ConfDisableWdt);
        PICSimLab.WindowCmd(wconfigId, "checkbox3", PWA_CHECKBOXGETCHECK, NULL, &use_cmdline_extra);
        PICSimLab.WindowCmd(wconfigId, "checkbox4", PWA_CHECKBOXGETCHECK, NULL, &ConfEnableEthernet);
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

void cboard_DevKitC::PinsExtraConfig(int cfg) {
    switch ((cfg & 0xf000) >> 12) {
        case QEMU_EXTRA_PIN_IN_CFG: {
            uint32_t* gpio_in_sel = qemu_picsimlab_get_internals(QEMU_INTERNAL_GPIO_IN_SEL);
            int function = cfg & 0x1ff;
            int gpio = gpio_in_sel[cfg & 0x1ff] & 0x3F;
            // printf("sel in    %3i =
            // %3i\n", gpio, function);

            switch (function) {
                case 9:  // HSPIQ
                    master_spi[0].cipo_pin = io2pin(gpio);
                    master_spi[0].ctrl_on = 1;
                    break;

                case 14:  // U0RXD
                    master_uart[0].rx_pin = io2pin(gpio);
                    master_uart[0].ctrl_on = 1;
                    break;
                case 17:  // U1RXD
                    master_uart[1].rx_pin = io2pin(gpio);
                    master_uart[1].ctrl_on = 1;
                    break;

                case 29:  // I2CEXT0_SCL
                    master_i2c[0].scl_pin = io2pin(gpio);
                    master_i2c[0].ctrl_on = 1;
                    break;
                case 30:  // I2CEXT0_SDA
                    master_i2c[0].sda_pin = io2pin(gpio);
                    master_i2c[0].ctrl_on = 1;
                    break;

                case 64:  // VSPIQ
                    master_spi[1].cipo_pin = io2pin(gpio);
                    master_spi[1].ctrl_on = 1;
                    break;

                case 95:  // I2CEXT1_SCL
                    master_i2c[1].scl_pin = io2pin(gpio);
                    master_i2c[1].ctrl_on = 1;
                    break;
                case 96:  // I2CEXT1_SDA
                    master_i2c[1].sda_pin = io2pin(gpio);
                    master_i2c[1].ctrl_on = 1;
                    break;

                case 198:  // U2RXD
                    master_uart[2].rx_pin = io2pin(gpio);
                    master_uart[2].ctrl_on = 1;
                    break;
            }

        } break;
        case QEMU_EXTRA_PIN_OUT_CFG: {
            uint32_t* gpio_out_sel = qemu_picsimlab_get_internals(QEMU_INTERNAL_GPIO_OUT_SEL);

            int function = gpio_out_sel[cfg & 0xff] & 0x1FF;
            int gpio = cfg & 0xff;
            // printf("sel out   %3i =
            // %3i\n", gpio, function);

            switch (function) {
                case 8:  // HSPICLK
                    master_spi[0].sck_pin = io2pin(gpio);
                    master_spi[0].ctrl_on = 1;
                    break;
                case 9:  // HSPIQ
                    master_spi[0].cipo_pin = io2pin(gpio);
                    master_spi[0].ctrl_on = 1;
                    break;
                case 10:  // HSPID
                    master_spi[0].copi_pin = io2pin(gpio);
                    master_spi[0].ctrl_on = 1;
                    break;
                case 11:  // HSPICS0
                    master_spi[0].cs_pin[0] = io2pin(gpio);
                    master_spi[0].ctrl_on = 1;
                    break;

                case 14:  // U0TXD
                    master_uart[0].tx_pin = io2pin(gpio);
                    master_uart[0].ctrl_on = 1;
                    break;
                case 17:  // U1TXD
                    master_uart[1].tx_pin = io2pin(gpio);
                    master_uart[1].ctrl_on = 1;
                    break;

                case 29:  // I2CEXT0_SCL
                    master_i2c[0].scl_pin = io2pin(gpio);
                    master_i2c[0].ctrl_on = 1;
                    break;
                case 30:  // I2CEXT0_SDA
                    master_i2c[0].sda_pin = io2pin(gpio);
                    master_i2c[0].ctrl_on = 1;
                    break;

                case 61:  // HSPICS1
                    master_spi[0].cs_pin[1] = io2pin(gpio);
                    master_spi[0].ctrl_on = 1;
                    break;
                case 62:  // HSPICS2
                    master_spi[0].cs_pin[2] = io2pin(gpio);
                    master_spi[0].ctrl_on = 1;
                    break;

                case 63:  // VSPICLK
                    master_spi[1].sck_pin = io2pin(gpio);
                    master_spi[1].ctrl_on = 1;
                    break;
                case 64:  // VSPIQ
                    master_spi[1].cipo_pin = io2pin(gpio);
                    master_spi[1].ctrl_on = 1;
                    break;
                case 65:  // VSPID
                    master_spi[1].copi_pin = io2pin(gpio);
                    master_spi[1].ctrl_on = 1;
                    break;
                case 68:  // VSPICS0
                    master_spi[1].cs_pin[0] = io2pin(gpio);
                    master_spi[1].ctrl_on = 1;
                    break;
                case 69:  // VSPICS1
                    master_spi[1].cs_pin[1] = io2pin(gpio);
                    master_spi[1].ctrl_on = 1;
                    break;
                case 70:  // VSPICS2
                    master_spi[1].cs_pin[2] = io2pin(gpio);
                    master_spi[1].ctrl_on = 1;
                    break;
                case 71:  // ledc_hs_sig_out0
                case 72:  // ledc_hs_sig_out1
                case 73:  // ledc_hs_sig_out2
                case 74:  // ledc_hs_sig_out3
                case 75:  // ledc_hs_sig_out4
                case 76:  // ledc_hs_sig_out5
                case 77:  // ledc_hs_sig_out6
                case 78:  // ledc_hs_sig_out7
                case 79:  // ledc_ls_sig_out0
                case 80:  // ledc_ls_sig_out1
                case 81:  // ledc_ls_sig_out2
                case 82:  // ledc_ls_sig_out3
                case 83:  // ledc_ls_sig_out4
                case 84:  // ledc_ls_sig_out5
                case 85:  // ledc_ls_sig_out6
                case 86:  // ledc_ls_sig_out7
                    // printf("LEDC channel %i in GPIO %i\n",function - 71, gpio);
                    bitbang_pwm_set_pin(&pwm_out, function - 71, io2pin(gpio));
                    break;
                case 87:  // rmt_sig_out0
                    // case 88:  // rmt_sig_out1  //FIXME only channel 0 enabled
                    // case 89:  // rmt_sig_out2
                    // case 90:  // rmt_sig_out3
                    // case 91:  // rmt_sig_out4
                    // case 92:  // rmt_sig_out5
                    // case 93:  // rmt_sig_out6
                    // case 94:  // rmt_sig_out7
                    //  printf("RMT channel %i in GPIO %i\n", function - 87, gpio);
                    rmt_out.pins[function - 87] = io2pin(gpio);
                    break;
                case 95:  // I2CEXT1_SCL
                    master_i2c[1].scl_pin = io2pin(gpio);
                    master_i2c[1].ctrl_on = 1;
                    break;
                case 96:  // I2CEXT1_SDA
                    master_i2c[1].sda_pin = io2pin(gpio);
                    master_i2c[1].ctrl_on = 1;
                    break;

                case 198:  // U2RTXD
                    master_uart[2].tx_pin = io2pin(gpio);
                    master_uart[2].ctrl_on = 1;
                    break;
            }

        } break;
        case QEMU_EXTRA_PIN_IOMUX_CFG: {
            uint32_t* muxgpios = qemu_picsimlab_get_internals(QEMU_INTERNAL_IOMUX_GPIOS);

            int function = (muxgpios[cfg & 0xff] & 0x7000) >> 12;
            int gpio = cfg & 0xff;
            // printf("iomux fun 0x%02x =
            // 0x%08x\n", gpio, function);

            switch (function) {
                case 0:
                    switch (gpio) {
                        case 1:
                            master_uart[0].tx_pin = IO1;
                            master_uart[0].ctrl_on = 1;
                            break;
                        case 3:
                            master_uart[0].rx_pin = IO3;
                            master_uart[0].ctrl_on = 1;
                            break;
                    }
                    break;
                case 1:
                    switch (gpio) {
                        case 12:
                            master_spi[0].cipo_pin = IO12;
                            master_spi[0].ctrl_on = 1;
                            break;
                        case 13:
                            master_spi[0].copi_pin = IO13;
                            master_spi[0].ctrl_on = 1;
                            break;
                        case 14:
                            master_spi[0].sck_pin = IO14;
                            master_spi[0].ctrl_on = 1;
                            break;
                        case 15:
                            master_spi[0].cs_pin[0] = IO15;
                            master_spi[0].ctrl_on = 1;
                            break;
                        case 18:
                            master_spi[1].sck_pin = IO18;
                            master_spi[1].ctrl_on = 1;
                            break;
                        case 19:
                            master_spi[1].cipo_pin = IO19;
                            master_spi[1].ctrl_on = 1;
                            break;
                        case 23:
                            master_spi[1].copi_pin = IO23;
                            master_spi[1].ctrl_on = 1;
                            break;
                        case 5:
                            master_spi[1].cs_pin[0] = IO5;
                            master_spi[1].ctrl_on = 1;
                            break;
                    }
                    break;
                case 2:
                    break;
                case 3:
                    break;
                case 4:
                    switch (gpio) {
                        case 9:
                            master_uart[1].rx_pin = IO9;
                            master_uart[1].ctrl_on = 1;
                            break;
                        case 10:
                            master_uart[1].tx_pin = IO10;
                            master_uart[1].ctrl_on = 1;
                            break;
                        case 16:
                            master_uart[2].rx_pin = IO16;
                            master_uart[2].ctrl_on = 1;
                            break;
                        case 17:
                            master_uart[2].tx_pin = IO17;
                            master_uart[2].ctrl_on = 1;
                            break;
                    }
                    break;
                case 5:
                    break;
            }

        } break;
        case QEMU_EXTRA_PIN_LEDC_CFG:
            uint32_t* channel_conf0_reg = qemu_picsimlab_get_internals(QEMU_INTERNAL_LEDC_CHANNEL_CONF);  // 16
            uint32_t* timer_freq = qemu_picsimlab_get_internals(QEMU_INTERNAL_LEDC_TIMER_FREQ);           // 8
            float* channel_duty = (float*)qemu_picsimlab_get_internals(QEMU_INTERNAL_LEDC_CHANNEL_DUTY);  // 16
            int channel = (cfg & 0x0F00) >> 8;
            int timern = channel_conf0_reg[channel] & 0x03;
            if (channel >= 8) {
                timern += 4;  // use LSTimers
            }

            bitbang_pwm_set_freq(&pwm_out, channel, timer_freq[timern]);
            // bitbang_pwm_set_duty(&pwm_out, channel, cfg & 0xFF);
            bitbang_pwm_set_duty_f(&pwm_out, channel, channel_duty[channel]);
            break;
    }
}

std::string cboard_DevKitC::MGetPinName(int pin) {
    std::string pinname = "error";

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
    if (!pin)
        return;
    if ((pins[pin - 1].avalue != value)) {
        unsigned char channel = 0xFF;

        pins[pin - 1].avalue = value;

        switch (pin) {
            case IO36:
                channel = 0;
                break;
                /*
                //GPIO37 and GPIO38 are not
                exposed in devkitc case x:
                // GPIO37 channel = 1;
                                break;
                            case x:  //
                GPIO38 channel = 2; break;
                */
            case IO39:
                channel = 3;
                break;
            case IO32:
                channel = 4;
                break;
            case IO33:
                channel = 5;
                break;
            case IO34:
                channel = 6;
                break;
            case IO35:
                channel = 7;
                break;
            case IO4:
                channel = 0x8 + 0;
                break;
            case IO0:
                channel = 0x8 + 1;
                break;
            case IO2:
                channel = 0x8 + 2;
                break;
            case IO15:
                channel = 0x8 + 3;
                break;
            case IO13:
                channel = 0x8 + 4;
                break;
            case IO12:
                channel = 0x8 + 5;
                break;
            case IO14:
                channel = 0x8 + 6;
                break;
            case IO27:
                channel = 0x8 + 7;
                break;
            case IO25:
                channel = 0x8 + 8;
                break;
            case IO26:
                channel = 0x8 + 9;
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
                // printf("Analog channel
                // %02X = %i\n", channel,
                // svalue);
            }
        }
    }
}

// Register the board in PICSimLab
board_init(BOARD_DevKitC_Name, cboard_DevKitC);
