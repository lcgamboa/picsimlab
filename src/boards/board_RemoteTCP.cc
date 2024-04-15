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
#ifndef _WIN_
#include <netinet/in.h>
#else
#include <winsock2.h>
#include <ws2tcpip.h>
#define MSG_NOSIGNAL 0
#endif

#include "../lib/oscilloscope.h"
#include "../lib/picsimlab.h"
#include "../lib/spareparts.h"
#include "board_RemoteTCP.h"

#define dprintf \
    if (1) {    \
    } else      \
        printf

const char json_info[] =
    "{"
    "  \"name\": \"PICSimLab\","
    "  \"description\": \"PICSimLab IO\","
    "  \"base address\": 0,"
    "  \"address width\": 2,"
    "  \"symbols\": {"
    "    \"PORTA\": 0,"
    "    \"DIRA\":  2,"
    "    \"PORTB\": 4,"
    "    \"DIRB\":  6,"
    "    \"T0CNT\": 8,"
    "    \"T0STA\": 10,"
    "    \"T0CON\": 12,"
    "    \"T0PR\":  14,"
    "    \"PFREQ\": 16,"
    "    \"UART0CFG\": 18,"
    "    \"UART0STA\": 20,"
    "    \"UART0BRG\": 22,"
    "    \"UART0RXR\": 24,"
    "    \"UART0TXR\": 26,"
    "    \"UART1CFG\": 28,"
    "    \"UART1STA\": 30,"
    "    \"UART1BRG\": 32,"
    "    \"UART1RXR\": 34,"
    "    \"UART1TXR\": 36,"
    "    \"SPI0CFG\": 38,"
    "    \"SPI0STA\": 40,"
    "    \"SPI0DAT\": 42,"
    "    \"SPI1CFG\": 44,"
    "    \"SPI1STA\": 46,"
    "    \"SPI1DAT\": 48,"
    "    \"I2C0CFG\": 50,"
    "    \"I2C0STA\": 52,"
    "    \"I2C0ADD\": 54,"
    "    \"I2C0DAT\": 56,"
    "    \"I2C1CFG\": 58,"
    "    \"I2C1STA\": 60,"
    "    \"I2C1ADD\": 62,"
    "    \"I2C1DAT\": 64,"
    "    \"ADCCFG\": 66,"
    "    \"ADCDAT\": 68"
    "  }"
    "}";

#define PORTA 0
#define DIRA 2
#define PORTB 4
#define DIRB 6
#define T0CNT 8
#define T0STA 10
#define T0CON 12
#define T0PR 14
#define PFREQ 16
#define UART0CFG 18
#define UART0STA 20
#define UART0BRG 22
#define UART0RXR 24
#define UART0TXR 26
#define UART1CFG 28
#define UART1STA 30
#define UART1BRG 32
#define UART1RXR 34
#define UART1TXR 36
#define SPI0CFG 38
#define SPI0STA 40
#define SPI0DAT 42
#define SPI1CFG 44
#define SPI1STA 46
#define SPI1DAT 48
#define I2C0CFG 50
#define I2C0STA 52
#define I2C0ADD 54
#define I2C0DAT 56
#define I2C1CFG 58
#define I2C1STA 60
#define I2C1ADD 62
#define I2C1DAT 64
#define ADCCFG 66
#define ADCDAT 68

/* ids of inputs of input map*/
enum {
    I_ICSP,  // ICSP connector
    I_PWR,   // Power button
    I_RST    // Reset button
};

/* ids of outputs of output map*/
enum {
    O_LPWR,  // Power LED
    O_MP,    // uController name
    O_RST    // Reset button
};
// return the input ids numbers of names used in input map

unsigned short cboard_RemoteTCP::GetInputId(char* name) {
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

unsigned short cboard_RemoteTCP::GetOutputId(char* name) {
    if (strcmp(name, "LD_LPWR") == 0)
        return O_LPWR;
    if (strcmp(name, "IC_CPU") == 0)
        return O_MP;
    if (strcmp(name, "PB_RST") == 0)
        return O_RST;

    printf("Error output '%s' don't have a valid id! \n", name);
    return INVALID_ID;
}

// Constructor called once on board creation

cboard_RemoteTCP::cboard_RemoteTCP(void) {
    Proc = "Ripes";  // default microcontroller if none defined in preferences
    ReadMaps();      // Read input and output board maps

    micbmp = PICSimLab.CanvasCmd(
        {.cmd = CC_LOADIMAGE, .LoadImage{(PICSimLab.GetSharePath() + "boards/Common/ic48.svg").c_str(), Scale, 1, 0}});

    // TODO define pins

    master_uart[0].tx_pin = 49;
    master_uart[0].rx_pin = 50;
    master_uart[1].tx_pin = 51;
    master_uart[1].rx_pin = 52;

    master_spi[0].sck_pin = 53;
    master_spi[0].copi_pin = 54;
    master_spi[0].cipo_pin = 55;
    master_spi[0].cs_pin[0] = 56;
    master_spi[0].cs_pin[1] = 0;
    master_spi[0].cs_pin[2] = 0;

    master_spi[1].sck_pin = 57;
    master_spi[1].copi_pin = 58;
    master_spi[1].cipo_pin = 59;
    master_spi[1].cs_pin[0] = 60;
    master_spi[1].cs_pin[1] = 0;
    master_spi[1].cs_pin[2] = 0;

    master_i2c[0].scl_pin = 61;
    master_i2c[0].sda_pin = 62;
    master_i2c[1].scl_pin = 63;
    master_i2c[1].sda_pin = 64;
}

// Destructor called once on board destruction

cboard_RemoteTCP::~cboard_RemoteTCP(void) {
    PICSimLab.CanvasCmd({.cmd = CC_FREEBITMAP, .FreeBitmap{micbmp}});
    micbmp = -1;
}

// Reset board status

void cboard_RemoteTCP::Reset(void) {
    MReset(1);

    PICSimLab.UpdateStatus(PS_SERIAL, "Serial: " + std::string(SERIALDEVICE));

    if (use_spare)
        SpareParts.Reset();

    RegisterRemoteControl();
}

int cboard_RemoteTCP::MInit(const char* processor, const char* fname, float freq) {
    int ret = 0;

    ret = bsim_remote::MInit(processor, fname, freq);

    int bmp = PICSimLab.CanvasCmd(
        {.cmd = CC_LOADIMAGE,
         .LoadImage{(PICSimLab.GetSharePath() + "boards/Common/ic" + std::to_string(MGetPinCount()) + ".svg").c_str(),
                    Scale, 1, 0}});

    if (bmp < 0) {
        bmp = PICSimLab.CanvasCmd(
            {.cmd = CC_LOADIMAGE,
             .LoadImage{(PICSimLab.GetSharePath() + "boards/Common/ic6.svg").c_str(), Scale, 1, 0}});
        printf("picsimlab: IC package with %i pins not found!\n", MGetPinCount());
        printf("picsimlab: %s not found!\n",
               (const char*)(PICSimLab.GetSharePath() + "boards/Common/ic" + std::to_string(MGetPinCount()) + ".svg")
                   .c_str());
    }

    if (micbmp)
        PICSimLab.CanvasCmd({.cmd = CC_FREEBITMAP, .FreeBitmap{micbmp}});
    micbmp = bmp;

    PICSimLab.SetMcuPwr(0);

    return ret;
}

void cboard_RemoteTCP::RegisterRemoteControl(void) {}

// Called ever 1s to refresh status

void cboard_RemoteTCP::RefreshStatus(void) {
    output_ids[O_LPWR]->update = 1;
    PICSimLab.UpdateStatus(PS_SERIAL, "Serial: " + std::string(SERIALDEVICE));
}

// Called to save board preferences in configuration file

void cboard_RemoteTCP::WritePreferences(void) {
    // write selected microcontroller of board_x to preferences
    PICSimLab.SavePrefs("RemoteTCP_proc", Proc);
    // write microcontroller clock to preferences
    PICSimLab.SavePrefs("RemoteTCP_clock", FloatStrFormat("%2.1f", PICSimLab.GetClock()));
}

// Called whe configuration file load  preferences

void cboard_RemoteTCP::ReadPreferences(char* name, char* value) {
    // read microcontroller of preferences
    if (!strcmp(name, "RemoteTCP_proc")) {
        Proc = value;
    }
    // read microcontroller clock
    if (!strcmp(name, "RemoteTCP_clock")) {
        PICSimLab.SetClock(atof(value));
    }
}

// Event on the board

void cboard_RemoteTCP::EvKeyPress(unsigned int key, unsigned int mask) {}

// Event on the board

void cboard_RemoteTCP::EvKeyRelease(unsigned int key, unsigned int mask) {}

// Event on the board

void cboard_RemoteTCP::EvMouseButtonPress(unsigned int button, unsigned int x, unsigned int y, unsigned int state) {
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
                    /*
                    case I_PWR:
                     if (Window1.Get_mcupwr ()) //if on turn off
                      {
                       PICSimLab.Set_mcupwr (0);
                       Reset ();
                      }
                     else //if off turn on
                      {
                       PICSimLab.Set_mcupwr (1);
                       Reset ();
                      }
                     output_ids[O_LPWR]->update = 1;
                     break;
                     */
                    // if event is over I_RST area then turn off and reset
                case I_RST:
                    /*
                    if (Window1.Get_mcupwr () && reset (-1))//if powered
                     {
                      PICSimLab.Set_mcupwr (0);
                      PICSimLab.Set_mcurst (1);
                     }
                     */
                    MReset(-1);
                    p_RST = 0;
                    output_ids[O_RST]->update = 1;
                    break;
            }
        }
    }
}

// Event on the board

void cboard_RemoteTCP::EvMouseButtonRelease(unsigned int button, unsigned int x, unsigned int y, unsigned int state) {
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
                    output_ids[O_RST]->update = 1;
                    break;
            }
        }
    }
}

// Called ever 100ms to draw board
// This is the critical code for simulator running speed

void cboard_RemoteTCP::Draw(void) {
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
                PICSimLab.CanvasCmd({.cmd = CC_INIT, .Init{Scale, Scale, 0}});
                PICSimLab.CanvasCmd({.cmd = CC_SETFONTWEIGHT, .SetFontWeight{CC_FONTWEIGHT_BOLD}});
            }
            update++;  // set to update buffer

            PICSimLab.CanvasCmd({.cmd = CC_SETFGCOLOR, .SetFgColor{0, 0, 0}});  // black

            switch (output[i].id)  // search for color of output
            {
                case O_LPWR:  // Blue using mcupwr value
                    PICSimLab.CanvasCmd(
                        {.cmd = CC_SETCOLOR, .SetColor{0, 0, (unsigned int)(200 * PICSimLab.GetMcuPwr() + 55)}});
                    PICSimLab.CanvasCmd({.cmd = CC_RECTANGLE,
                                         .Rectangle{1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1,
                                                    output[i].y2 - output[i].y1}});
                    break;
                case O_MP:

                    PICSimLab.CanvasCmd({.cmd = CC_SETFONTSIZE, .SetFontSize{10}});

                    PICSimLab.CanvasCmd({.cmd = CC_SETFONTSIZE,
                                         .SetFontSize{((MGetPinCount() >= 44) || (MGetPinCount() <= 8))
                                                          ? 6
                                                          : ((MGetPinCount() > 14) ? 12 : 10)}});

                    PICSimLab.CanvasCmd({.cmd = CC_GETBITMAPSIZE, .GetBitmapSize{micbmp, &w, &h}});
                    PICSimLab.CanvasCmd({.cmd = CC_CHANGESCALE, .ChangeScale{1.0, 1.0}});
                    PICSimLab.CanvasCmd(
                        {.cmd = CC_PUTBITMAP, .PutBitmap{micbmp, output[i].x1 * Scale, output[i].y1 * Scale}});
                    PICSimLab.CanvasCmd({.cmd = CC_CHANGESCALE, .ChangeScale{Scale, Scale}});
                    PICSimLab.CanvasCmd({.cmd = CC_SETFGCOLOR, .SetFgColor{230, 230, 230}});

                    rec.x = output[i].x1;
                    rec.y = output[i].y1;
                    rec.width = w / Scale;
                    rec.height = h / Scale;
                    PICSimLab.CanvasCmd({.cmd = CC_TEXTONRECT,
                                         .TextOnRect{Proc.c_str(), rec, CC_ALIGN_CENTER | CC_ALIGN_CENTER_VERTICAL}});
                    break;
                case O_RST:
                    PICSimLab.CanvasCmd({.cmd = CC_SETCOLOR, .SetColor{100, 100, 100}});
                    PICSimLab.CanvasCmd({.cmd = CC_CIRCLE, .Circle{1, output[i].cx, output[i].cy, 11}});
                    if (p_RST) {
                        PICSimLab.CanvasCmd({.cmd = CC_SETCOLOR, .SetColor{15, 15, 15}});
                    } else {
                        PICSimLab.CanvasCmd({.cmd = CC_SETCOLOR, .SetColor{55, 55, 55}});
                    }
                    PICSimLab.CanvasCmd({.cmd = CC_CIRCLE, .Circle{1, output[i].cx, output[i].cy, 9}});
                    break;
            }
        }
    }

    // end draw
    if (update) {
        PICSimLab.CanvasCmd({.cmd = CC_END});
    }
}

void cboard_RemoteTCP::Run_CPU(void) {
    /*
    int i;
    // int j;
    unsigned char pi;
    unsigned int alm[48];
    const int pinc = MGetPinCount();

    // const int JUMPSTEPS = Window1.GetJUMPSTEPS ();
    //number of steps skipped const long int NSTEP = 4.0 *
    PICSimLab.GetNSTEP();  // number of steps in 100ms const
    float RNSTEP = 200.0 * pinc / NSTEP;

    // reset pins mean value
    memset(alm, 0, 48 * sizeof(unsigned int));
    */
    if (!TestConnection())
        return;

    PICSimLab.SetMcuPwr(1);
    /*
    // Spare parts window pre process
    if (use_spare)
        SpareParts.PreProcess();

    double delay = (PICSimLab.GetIdleMs() * 1000) / NSTEP;

    unsigned int countM = 1100 / delay;  // added 100us of
overhead unsigned int count = countM;

    // printf ("idle %6.1f   %10.5f  %u\n",
Window1.GetIdleMs (), delay, count);

    // j = JUMPSTEPS; //step counter
    pi = 0;
    if (PICSimLab.GetMcuPwr())       // if powered
        for (i = 0; i < NSTEP; i++)  // repeat for number of
steps in 100ms
        {
            / *
            if (j >= JUMPSTEPS)//if number of step is bigger
than steps to skip
             {
             }
             * /
    // verify if a breakpoint is reached if not run one
instruction
    // MStep ();

    --count;
    if (!count) {
#ifdef _WIN_
        Sleep(1);
#else
        usleep(1000);
#endif
        count = countM;
    }

    if (t0CON & 0x8000)  // Timer on
    {
        t0iclk++;        // prescaler clk
        if (t0iclk == (t0CON & 0x7FFF)) {
            t0iclk = 0;
            t0CNT++;
            if (t0CNT == t0PR)  // max value
            {
                t0CNT = 0;
                t0STA |= 1;  // overflow
            }
        }
    }
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
    if (pi == pinc)
        pi = 0;
    / *
        if (j >= JUMPSTEPS)//if number of step is bigger
than steps to skip
         {
          j = -1; //reset counter
         }

        j++; //counter increment
     * /
}

// calculate mean value
for (pi = 0; pi < MGetPinCount(); pi++) {
    pins[pi].oavalue = (int)((alm[pi] * RNSTEP) + 55);
}

// Spare parts window pre post process
if (use_spare)
    SpareParts.PostProcess();
*/
}

void cboard_RemoteTCP::Run_CPU_ns(uint64_t time) {
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
            if (t0CON & 0x8000)  // Timer on
            {
                t0iclk++;  // prescaler clk
                if (t0iclk == (t0CON & 0x7FFF)) {
                    t0iclk = 0;
                    t0CNT++;
                    if (t0CNT == t0PR)  // max value
                    {
                        t0CNT = 0;
                        t0STA |= 1;  // overflow
                    }
                }
            }
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
        if (ns_count >= TTIMEOUT) {  // every 100ms
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

void cboard_RemoteTCP::EvThreadRun(void) {
    int tdestroy = 0;
    do {
        cmd_header_t cmd_header;

        if (recv_cmd(&cmd_header) < 0) {
            ConnectionError("recv_cmd");
            return;
        }

        dprintf("MSG type = %i size=%i  timestamp= %lu  ", cmd_header.msg_type, cmd_header.payload_size,
                (long unsigned int)cmd_header.time);

        switch (cmd_header.msg_type) {
            case VB_PINFO: {
                if (send_cmd(VB_PINFO, json_info, strlen(json_info)) < 0) {
                    ConnectionError("send_cmd");
                    break;
                }

                dprintf("VB_PINFO %s\n", json_info);
            } break;
            case VB_PWRITE: {
                if (cmd_header.payload_size) {
                    uint32_t* payload = new uint32_t[cmd_header.payload_size / 4];
                    if (recv_payload((char*)payload, cmd_header.payload_size) < 0) {
                        ConnectionError("recv_payload");
                        break;
                    }
                    for (uint32_t i = 0; i < (cmd_header.payload_size / 4); i++) {
                        payload[i] = ntohl(payload[i]);
                    }

                    switch (payload[0]) {
                        case PORTA:
                            Ports[0] = (payload[1] & (~Dirs[0])) | (Ports[0] & Dirs[0]);
                            for (int pin = 0; pin < 16; pin++) {
                                if (Ports[0] & (1 << pins[pin].pord)) {
                                    pins[pin].value = 1;
                                } else {
                                    pins[pin].value = 0;
                                }
                            }
                            ioupdated = 1;
                            break;
                        case DIRA:
                            Dirs[0] = payload[1];
                            for (int pin = 0; pin < 16; pin++) {
                                if (Dirs[0] & (1 << pins[pin].pord)) {
                                    pins[pin].dir = PD_IN;
                                } else {
                                    pins[pin].dir = PD_OUT;
                                }
                            }
                            ioupdated = 1;
                            break;
                        case PORTB:
                            Ports[1] = (payload[1] & (~Dirs[1])) | (Ports[1] & Dirs[1]);
                            for (int pin = 16; pin < 32; pin++) {
                                if (Ports[1] & (1 << pins[pin].pord)) {
                                    pins[pin].value = 1;
                                } else {
                                    pins[pin].value = 0;
                                }
                            }
                            ioupdated = 1;
                            break;
                        case DIRB:
                            Dirs[1] = payload[1];
                            for (int pin = 16; pin < 32; pin++) {
                                if (Dirs[1] & (1 << pins[pin].pord)) {
                                    pins[pin].dir = PD_IN;
                                } else {
                                    pins[pin].dir = PD_OUT;
                                }
                            }
                            ioupdated = 1;
                            break;
                        case T0CNT:
                            t0CNT = payload[1];
                            break;
                        case T0CON:
                            t0CON = payload[1];
                            break;
                        case T0STA:
                            t0STA = payload[1];
                            break;
                        case T0PR:
                            t0PR = payload[1];
                            break;
                        case UART0CFG:
                            master_uart[0].ctrl_on = (payload[1] & 0x8000) > 0;
                            break;
                        case UART0STA:
                            // value= payload[1];
                            break;
                        case UART0BRG:
                            // value= payload[1];
                            break;
                        case UART0RXR:
                            // value= payload[1];
                            break;
                        case UART0TXR:
                            bitbang_uart_send(&master_uart[0], payload[1]);
                            break;
                        case UART1CFG:
                            master_uart[0].ctrl_on = (payload[1] & 0x8000) > 0;
                            break;
                        case UART1STA:
                            // value= payload[1];
                            break;
                        case UART1BRG:
                            // value= payload[1];
                            break;
                        case UART1RXR:
                            // value= payload[1];
                            break;
                        case UART1TXR:
                            bitbang_uart_send(&master_uart[1], payload[1]);
                            break;
                        case SPI0CFG:
                            master_spi[0].ctrl_on = (payload[1] & 0x8000) > 0;
                            break;
                        case SPI0STA:
                            // value= payload[1];
                            break;
                        case SPI0DAT:
                            master_spi[0].cs_value[0] = 0;
                            bitbang_spi_ctrl_write(&master_spi[0], payload[1]);
                            break;
                        case SPI1CFG:
                            master_spi[1].ctrl_on = (payload[1] & 0x8000) > 0;
                            break;
                        case SPI1STA:
                            // value= payload[1];
                            break;
                        case SPI1DAT:
                            master_spi[1].cs_value[0] = 0;
                            bitbang_spi_ctrl_write(&master_spi[1], payload[1]);
                            break;
                        case I2C0CFG:
                            if ((payload[1] & 0x8000) > 0) {
                                master_i2c[0].ctrl_on = 1;
                                if (payload[1] & 0x0001) {
                                    bitbang_i2c_ctrl_start(&master_i2c[0]);
                                }
                                if (payload[1] & 0x0002) {
                                    bitbang_i2c_ctrl_stop(&master_i2c[0]);
                                }
                            } else {
                                master_i2c[0].ctrl_on = 0;
                            }
                            break;
                        case I2C0STA:
                            // value= payload[1];
                            break;
                        case I2C0ADD:
                            // value= payload[1];
                            break;
                        case I2C0DAT:
                            if (master_i2c[0].byte == 0) {
                                master_i2c[0].addr = payload[1];
                                bitbang_i2c_ctrl_write(&master_i2c[0], payload[1]);
                            } else {
                                if (master_i2c[0].addr & 0x01) {
                                    bitbang_i2c_ctrl_read(&master_i2c[0]);
                                } else {
                                    bitbang_i2c_ctrl_write(&master_i2c[0], payload[1]);
                                }
                            }
                            break;
                        case I2C1CFG:
                            if ((payload[1] & 0x8000) > 0) {
                                master_i2c[0].ctrl_on = 1;
                                if (payload[1] & 0x0001) {
                                    bitbang_i2c_ctrl_start(&master_i2c[1]);
                                }
                                if (payload[1] & 0x0002) {
                                    bitbang_i2c_ctrl_stop(&master_i2c[1]);
                                }
                            } else {
                                master_i2c[0].ctrl_on = 0;
                            }
                            break;
                        case I2C1STA:
                            // value= payload[1];
                            break;
                        case I2C1ADD:
                            // value= payload[1];
                            break;
                        case I2C1DAT:
                            if (master_i2c[1].byte == 0) {
                                master_i2c[1].addr = payload[1];
                                bitbang_i2c_ctrl_write(&master_i2c[1], payload[1]);
                            } else {
                                if (master_i2c[1].addr & 0x01) {
                                    bitbang_i2c_ctrl_read(&master_i2c[1]);
                                } else {
                                    bitbang_i2c_ctrl_write(&master_i2c[1], payload[1]);
                                }
                            }
                            break;
                        case ADCCFG:
                            ADCChanel = payload[1] & 0x000F;
                            break;
                        case ADCDAT:
                            // value= payload[1];
                            break;
                    }

                    dprintf("VB_PWRITE reg[%i] = %x\n", payload[0], payload[1]);
                    delete[] payload;
                }
                if (send_cmd(cmd_header.msg_type) < 0) {
                    ConnectionError("send_cmd");
                    break;
                }
            } break;
            case VB_PREAD: {
                uint32_t addr = 0;
                if (cmd_header.payload_size) {
                    recv_payload((char*)&addr, 4);
                    addr = ntohl(addr);
                }

                uint32_t payload[2];
                payload[0] = htonl(addr);

                switch (addr) {
                    case PORTA:
                        payload[1] = htonl(Ports[0]);
                        break;
                    case DIRA:
                        payload[1] = htonl(Dirs[0]);
                        break;
                    case PORTB:
                        payload[1] = htonl(Ports[1]);
                        break;
                    case DIRB:
                        payload[1] = htonl(Dirs[1]);
                        break;
                    case T0CNT:
                        payload[1] = htonl(t0CNT);
                        break;
                    case T0CON:
                        payload[1] = htonl(t0CON);
                        break;
                    case T0STA:
                        payload[1] = htonl(t0STA);
                        break;
                    case T0PR:
                        payload[1] = htonl(t0PR);
                        break;
                    case PFREQ:
                        payload[1] = htonl(MGetFreq() / 1000000);
                        break;
                    case UART0CFG:
                        payload[1] = htonl(0);
                        break;
                    case UART0STA:
                        payload[1] = htonl((bitbang_uart_transmitting(&master_uart[0]) << 1) |
                                           bitbang_uart_data_available(&master_uart[0]));
                        break;
                    case UART0BRG:
                        payload[1] = htonl(0);
                        break;
                    case UART0RXR:
                        payload[1] = htonl(bitbang_uart_recv(&master_uart[0]));
                        break;
                    case UART0TXR:
                        payload[1] = htonl(0);
                        break;
                    case UART1CFG:
                        payload[1] = htonl(0);
                        break;
                    case UART1STA:
                        payload[1] = htonl((bitbang_uart_transmitting(&master_uart[1]) << 1) |
                                           bitbang_uart_data_available(&master_uart[1]));
                        break;
                    case UART1BRG:
                        payload[1] = htonl(0);
                        break;
                    case UART1RXR:
                        payload[1] = htonl(bitbang_uart_recv(&master_uart[1]));
                        break;
                    case UART1TXR:
                        payload[1] = htonl(0);
                        break;
                    case SPI0CFG:
                        payload[1] = htonl(0);
                        break;
                    case SPI0STA:
                        payload[1] = htonl(!master_spi[0].transmitting);
                        break;
                    case SPI0DAT:
                        payload[1] = htonl(master_spi[0].data);
                        break;
                    case SPI1CFG:
                        payload[1] = htonl(0);
                        break;
                    case SPI1STA:
                        payload[1] = htonl(!master_spi[1].transmitting);
                        break;
                    case SPI1DAT:
                        payload[1] = htonl(master_spi[1].data);
                        break;
                    case I2C0CFG:
                        payload[1] = htonl(0);
                        break;
                    case I2C0STA:
                        payload[1] = htonl(bitbang_i2c_get_status(&master_i2c[0]));
                        break;
                    case I2C0ADD:
                        payload[1] = htonl(0);
                        break;
                    case I2C0DAT:
                        payload[1] = htonl(0);
                        break;
                    case I2C1CFG:
                        payload[1] = htonl(0);
                        break;
                    case I2C1STA:
                        payload[1] = htonl(bitbang_i2c_get_status(&master_i2c[1]));
                        break;
                    case I2C1ADD:
                        payload[1] = htonl(0);
                        break;
                    case I2C1DAT:
                        payload[1] = htonl(0);
                        break;
                    case ADCCFG:
                        payload[1] = htonl(ADCChanel);
                        break;
                    case ADCDAT:
                        payload[1] = htonl(ADCvalues[ADCChanel]);
                        break;
                    default:
                        payload[1] = htonl(0);
                        printf(
                            "Read invalid reg addr %i "
                            "!!!!!!!!!!!!!!!!!!\n",
                            addr);
                        break;
                }

                if (send_cmd(cmd_header.msg_type, (const char*)&payload, 8) < 0) {
                    ConnectionError("send_cmd");
                    break;
                }
                dprintf("VB_PREAD  reg[%x] = %x \n", addr, (unsigned int)ntohl(payload[1]));
            } break;
            case VB_QUIT:
                send_cmd(VB_QUIT);
                Disconnect();
                dprintf("VB_QUIT\n");
                break;
            case VB_SYNC:
                // already sync by method recv_cmd
                send_cmd(VB_SYNC);
                dprintf("VB_SYNC\n");
                break;
            case VB_PSTATUS:
                uint32_t payload[1];
                payload[0] = htonl(0);
                send_cmd(VB_PSTATUS, (const char*)&payload, 4);
                dprintf("VB_PSTATUS\n");
                break;
            default:
                printf("Invalid cmd !!!!!!!!!!!!\n");
                if (send_cmd(VB_LAST)) {
                    ConnectionError("send_cmd");
                    break;
                }
                break;
        }
        PICSimLab.WindowCmd(PW_MAIN, "thread3", PWA_THREADTESTDESTROY, NULL, &tdestroy);
    } while (!tdestroy);
}

// Register the board in PICSimLab
board_init(BOARD_RemoteTCP_Name, cboard_RemoteTCP);
