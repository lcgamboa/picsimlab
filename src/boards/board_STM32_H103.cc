/* ########################################################################

   PICsimLab - PIC laboratory simulator

   ########################################################################

   Copyright (c) : 2015-2022  Luis Claudio Gambôa Lopes

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
#include "board_STM32_H103.h"
#include "../oscilloscope.h"
#include "../picsimlab.h"
#include "../spareparts.h"

/* ids of inputs of input map*/
enum {
    I_ICSP,  // ICSP connector
    I_PWR,   // Power button
    I_RST,   // Reset button
    I_BUT,   // User button
};

/* ids of outputs of output map*/
enum {
    O_LPWR,  // Power LED
    O_LED,   // LED on PC13 output
    O_BUT,   // User button
    O_RST    // Reset button
};
// return the input ids numbers of names used in input map

unsigned short cboard_STM32_H103::GetInputId(char* name) {
    if (strcmp(name, "PG_ICSP") == 0)
        return I_ICSP;
    if (strcmp(name, "SW_PWR") == 0)
        return I_PWR;
    if (strcmp(name, "PB_RST") == 0)
        return I_RST;
    if (strcmp(name, "PB_BUT") == 0)
        return I_BUT;

    printf("Error input '%s' don't have a valid id! \n", name);
    return -1;
}

// return the output ids numbers of names used in output map

unsigned short cboard_STM32_H103::GetOutputId(char* name) {
    if (strcmp(name, "LD_LED") == 0)
        return O_LED;
    if (strcmp(name, "LD_LPWR") == 0)
        return O_LPWR;
    if (strcmp(name, "PB_BUT") == 0)
        return O_BUT;
    if (strcmp(name, "PB_RST") == 0)
        return O_RST;

    printf("Error output '%s' don't have a valid id! \n", name);
    return 1;
}

// Constructor called once on board creation

cboard_STM32_H103::cboard_STM32_H103(void) {
    char buffer[1024];

    SimType = QEMU_SIM_STM32;

    Proc = "stm32f103rbt6";  // default microcontroller if none defined in preferences
    ReadMaps();              // Read input and output board maps
    p_BUT = 0;

    // TODO read pin configuration from registers instead use fixed pins
    master_i2c[0].scl_pin = 58;  // pb6 or pb8
    master_i2c[0].sda_pin = 59;  // pb7 or pb9
    master_i2c[1].scl_pin = 29;  // pb10
    master_i2c[1].sda_pin = 30;  // pb11

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
    combo1->SetText(IcountToMipsStr(5));
    combo1->SetItems(IcountToMipsItens(buffer));
    combo1->SetTag(3);
    combo1->EvOnComboChange = PICSimLab.board_Event;
    PICSimLab.GetWindow()->CreateChild(combo1);
}

// Destructor called once on board destruction

cboard_STM32_H103::~cboard_STM32_H103(void) {
    PICSimLab.GetWindow()->DestroyChild(label1);
    PICSimLab.GetWindow()->DestroyChild(combo1);
}

// Reset board status

void cboard_STM32_H103::Reset(void) {
    p_BUT = 0;

    MReset(1);

    PICSimLab.GetStatusBar()->SetField(2, lxT("Serial: ") + lxString::FromAscii(SERIALDEVICE));

    if (use_spare)
        SpareParts.Reset();

    RegisterRemoteControl();
}

void cboard_STM32_H103::RegisterRemoteControl(void) {
    input_ids[I_BUT]->status = &p_BUT;
    input_ids[I_BUT]->update = &output_ids[O_BUT]->update;

    output_ids[O_LED]->status = &pins[52].oavalue;
}

// Called ever 1s to refresh status

void cboard_STM32_H103::RefreshStatus(void) {
    if (serial_open) {
        PICSimLab.GetStatusBar()->SetField(2, lxT("Serial: ") + lxString::FromAscii(SERIALDEVICE));
    } else {
        PICSimLab.GetStatusBar()->SetField(2, lxT("Serial: Error"));
    }
}

// Called to save board preferences in configuration file

void cboard_STM32_H103::WritePreferences(void) {
    // write selected microcontroller of board_x to preferences
    PICSimLab.saveprefs(lxT("STM32_H103_proc"), Proc);
    // write microcontroller clock to preferences
    PICSimLab.saveprefs(lxT("STM32_H103_clock"), lxString().Format("%2.1f", PICSimLab.GetClock()));
    // write microcontroller icount to preferences
    PICSimLab.saveprefs(lxT("STM32_H103_icount"), itoa(icount));
}

// Called whe configuration file load  preferences

void cboard_STM32_H103::ReadPreferences(char* name, char* value) {
    // read microcontroller of preferences
    if (!strcmp(name, "STM32_H103_proc")) {
        Proc = value;
    }
    // read microcontroller clock
    if (!strcmp(name, "STM32_H103_clock")) {
        PICSimLab.SetClock(atof(value));
    }
    // read microcontroller icount
    if (!strcmp(name, "STM32_H103_icount")) {
        icount = atoi(value);
        combo1->SetText(IcountToMipsStr(icount));
    }
}

// Event on the board

void cboard_STM32_H103::EvKeyPress(uint key, uint mask) {}

// Event on the board

void cboard_STM32_H103::EvKeyRelease(uint key, uint mask) {}

// Event on the board

void cboard_STM32_H103::EvMouseButtonPress(uint button, uint x, uint y, uint state) {
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
                    if (PICSimLab.Get_mcupwr())  // if on turn off
                    {
                        PICSimLab.Set_mcupwr(0);
                        Reset();
                    } else  // if off turn on
                    {
                        PICSimLab.Set_mcupwr(1);
                        Reset();
                    }
                    break;
                    // if event is over I_RST area then turn off and reset
                case I_RST:
                    /*
                    if (PICSimLab.GetWindow()->Get_mcupwr () && reset (-1))//if powered
                     {
                      PICSimLab.Set_mcupwr (0);
                      PICSimLab.Set_mcurst (1);
                     }
                     */
                    Reset();
                    p_RST = 0;
                    break;
                case I_BUT:
                    p_BUT = 1;
                    break;
            }
        }
    }
}

// Event on the board

void cboard_STM32_H103::EvMouseButtonRelease(uint button, uint x, uint y, uint state) {
    int i;

    // search for the input area which owner the event
    for (i = 0; i < inputc; i++) {
        if (((input[i].x1 <= x) && (input[i].x2 >= x)) && ((input[i].y1 <= y) && (input[i].y2 >= y))) {
            switch (input[i].id) {
                    // if event is over I_RST area then turn on
                case I_RST:
                    if (PICSimLab.Get_mcurst())  // if powered
                    {
                        PICSimLab.Set_mcupwr(1);
                        PICSimLab.Set_mcurst(0);
                        /*
                                 if (reset (-1))
                                  {
                                   Reset ();
                                  }
                         */
                    }
                    p_RST = 1;
                    break;
                case I_BUT:
                    p_BUT = 0;
                    break;
            }
        }
    }
}

// Called ever 100ms to draw board
// This is the critical code for simulator running speed

void cboard_STM32_H103::Draw(CDraw* draw) {
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
                case O_LED:  // White using pc12 mean value
                    draw->Canvas.SetColor(0, pins[52].oavalue, 0);
                    draw->Canvas.Rectangle(1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1,
                                           output[i].y2 - output[i].y1);

                    break;
                case O_LPWR:  // Blue using mcupwr value
                    draw->Canvas.SetColor(200 * PICSimLab.Get_mcupwr() + 55, 0, 0);
                    draw->Canvas.Rectangle(1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1,
                                           output[i].y2 - output[i].y1);

                    break;
                case O_BUT:
                    draw->Canvas.SetColor(100, 100, 100);
                    draw->Canvas.Circle(1, output[i].cx, output[i].cy, 13);
                    if (p_BUT) {
                        draw->Canvas.SetColor(55, 55, 55);
                    } else {
                        draw->Canvas.SetColor(15, 15, 15);
                    }
                    draw->Canvas.Circle(1, output[i].cx, output[i].cy, 11);
                    break;
                case O_RST:
                    draw->Canvas.SetColor(100, 100, 100);
                    draw->Canvas.Circle(1, output[i].cx, output[i].cy, 13);
                    if (p_RST) {
                        draw->Canvas.SetColor(15, 15, 15);
                    } else {
                        draw->Canvas.SetColor(55, 55, 55);
                    }
                    draw->Canvas.Circle(1, output[i].cx, output[i].cy, 11);
                    break;
            }
        }
    }

    // end draw
    draw->Canvas.End();
    draw->Update();
}

void cboard_STM32_H103::Run_CPU_ns(uint64_t time) {
    static int j = 0;
    static unsigned char pi = 0;
    static unsigned int alm[64];
    static const int pinc = MGetPinCount();

    const int JUMPSTEPS = 4.0 * PICSimLab.GetJUMPSTEPS();  // number of steps skipped

    const int inc = 1000000000L / MGetInstClockFreq();

    const float RNSTEP = 200.0 * pinc * inc / 100000000L;

    for (uint64_t c = 0; c < time; c += inc) {
        if (!ns_count) {
            // reset pins mean value
            memset(alm, 0, 64 * sizeof(unsigned int));

            // Spare parts window pre process
            if (use_spare)
                SpareParts.PreProcess();

            j = JUMPSTEPS;  // step counter
            pi = 0;
        }

        if (PICSimLab.Get_mcupwr())  // if powered
        {
            if (j >= JUMPSTEPS)  // if number of step is bigger than steps to skip
            {
                MSetPin(14, p_BUT);
            }

            // verify if a breakpoint is reached if not run one instruction
            MStep();
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

            if (j >= JUMPSTEPS)  // if number of step is bigger than steps to skip
            {
                j = -1;  // reset counter
            }

            j++;  // counter increment
        }
        ns_count += inc;
        if (ns_count > 100000000) {
            ns_count = 0;
            // calculate mean value
            for (pi = 0; pi < MGetPinCount(); pi++) {
                pins[pi].oavalue = (int)((alm[pi] * RNSTEP) + 55);
            }

            // Spare parts window pre post process
            if (use_spare)
                SpareParts.PostProcess();
        }
    }
}

void cboard_STM32_H103::board_Event(CControl* control) {
    icount = MipsStrToIcount(combo1->GetText().c_str());
    PICSimLab.EndSimulation();
}

lxString cboard_STM32_H103::MGetPinName(int pin) {
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
            pinname = "PC0";
            break;
        case 9:
            pinname = "PC1";
            break;
        case 10:
            pinname = "PC2";
            break;
        case 11:
            pinname = "PC3";
            break;
        case 12:
            pinname = "VSSA";
            break;
        case 13:
            pinname = "VDDA";
            break;
        case 14:
            pinname = "PA0";
            break;
        case 15:
            pinname = "PA1";
            break;
        case 16:
            pinname = "PA2";
            break;
        case 17:
            pinname = "PA3";
            break;
        case 18:
            pinname = "VSS";
            break;
        case 19:
            pinname = "VDD";
            break;
        case 20:
            pinname = "PA4";
            break;
        case 21:
            pinname = "PA5";
            break;
        case 22:
            pinname = "PA6";
            break;
        case 23:
            pinname = "PA7";
            break;
        case 24:
            pinname = "PC4";
            break;
        case 25:
            pinname = "PC5";
            break;
        case 26:
            pinname = "PB0";
            break;
        case 27:
            pinname = "PB1";
            break;
        case 28:
            pinname = "PB2";
            break;
        case 29:
            pinname = "PB10";
            break;
        case 30:
            pinname = "PB11";
            break;
        case 31:
            pinname = "VSS";
            break;
        case 32:
            pinname = "VDD";
            break;
        case 33:
            pinname = "PB12";
            break;
        case 34:
            pinname = "PB13";
            break;
        case 35:
            pinname = "PB14";
            break;
        case 36:
            pinname = "PB15";
            break;
        case 37:
            pinname = "PC6";
            break;
        case 38:
            pinname = "PC7";
            break;
        case 39:
            pinname = "PC8";
            break;
        case 40:
            pinname = "PC9";
            break;
        case 41:
            pinname = "PA8";
            break;
        case 42:
            pinname = "PA9";
            break;
        case 43:
            pinname = "PA10";
            break;
        case 44:
            pinname = "PA11";
            break;
        case 45:
            pinname = "PA12";
            break;
        case 46:
            pinname = "PA13";
            break;
        case 47:
            pinname = "VSS";
            break;
        case 48:
            pinname = "VDD";
            break;
        case 49:
            pinname = "PA14";
            break;
        case 50:
            pinname = "PA15";
            break;
        case 51:
            pinname = "PC10";
            break;
        case 52:
            pinname = "PC11";
            break;
        case 53:
            pinname = "PC12";
            break;
        case 54:
            pinname = "PD2";
            break;
        case 55:
            pinname = "PB3";
            break;
        case 56:
            pinname = "PB4";
            break;
        case 57:
            pinname = "PB5";
            break;
        case 58:
            pinname = "PB6";
            break;
        case 59:
            pinname = "PB7";
            break;
        case 60:
            pinname = "BOOT0";
            break;
        case 61:
            pinname = "PB8";
            break;
        case 62:
            pinname = "PB9";
            break;
        case 63:
            pinname = "VSS";
            break;
        case 64:
            pinname = "VDD";
            break;
    }

    return pinname;
}

int cboard_STM32_H103::MGetPinCount(void) {
    return 64;
}

void cboard_STM32_H103::MSetAPin(int pin, float value) {
    if (!pin)
        return;
    if ((pins[pin - 1].avalue != value)) {
        unsigned char channel = 0xFF;

        pins[pin - 1].avalue = value;

        switch (pin) {
            case 8:  // PC0
                channel = 10;
                break;
            case 9:  // PC1
                channel = 11;
                break;
            case 10:  // PC2
                channel = 12;
                break;
            case 11:  // PC3
                channel = 13;
                break;
            case 14:  // PA0
                channel = 0;
                break;
            case 15:  // PA1
                channel = 1;
                break;
            case 16:  // PA2
                channel = 2;
                break;
            case 17:  // PA3
                channel = 3;
                break;
            case 20:  // PA4
                channel = 4;
                break;
            case 21:  // PA5
                channel = 5;
                break;
            case 22:  // PA6
                channel = 6;
                break;
            case 23:  // PA7
                channel = 7;
                break;
            case 24:  // PC4
                channel = 14;
                break;
            case 25:  // PC5
                channel = 15;
                break;
            case 26:  // PB0
                channel = 8;
                break;
            case 27:  // PB1
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

// Register the board in PICSimLab
board_init(BOARD_STM32_H103_Name, cboard_STM32_H103);
