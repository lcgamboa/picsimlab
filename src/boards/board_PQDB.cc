/* ########################################################################

   PICSimLab - Programmable IC Simulator Laboratory

   ########################################################################

   Copyright (c) : 2020-2024  Luis Claudio Gambôa Lopes <lcgamboa@yahoo.com>

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

#include "board_PQDB.h"

#include <math.h>
#include "../lib/oscilloscope.h"
#include "../lib/picsimlab.h"
#include "../lib/spareparts.h"

#define selectColorByValue(Value)                                                    \
    {                                                                                \
        if (Value) {                                                                 \
            PICSimLab.CanvasCmd({.cmd = CC_SETBGCOLOR, .SetBgColor{205, 0, 0}});     \
        } else {                                                                     \
            PICSimLab.CanvasCmd({.cmd = CC_SETBGCOLOR, .SetBgColor{205, 180, 180}}); \
        }                                                                            \
        PICSimLab.CanvasCmd({.cmd = CC_SETFGCOLOR, .SetFgColor{205, 180, 180}});     \
    }
#define selectColorByPinValue(PIN_ID)                                                                       \
    PICSimLab.CanvasCmd({.cmd = CC_SETBGCOLOR, .SetBgColor{(unsigned int)pic.pins[PIN_ID].oavalue, 0, 0}}); \
    PICSimLab.CanvasCmd({.cmd = CC_SETFGCOLOR, .SetFgColor{205, 180, 180}})
/* outputs */
enum {
    O_SS1,
    O_SS2,
    O_SS3,
    O_SS4,
    O_B1,
    O_F1,
    O_A1,
    O_G1,
    O_C1,
    O_P1,
    O_D1,
    O_E1,
    O_B2,
    O_F2,
    O_A2,
    O_G2,
    O_C2,
    O_P2,
    O_D2,
    O_E2,
    O_B3,
    O_F3,
    O_A3,
    O_G3,
    O_C3,
    O_P3,
    O_D3,
    O_E3,
    O_B4,
    O_F4,
    O_A4,
    O_G4,
    O_C4,
    O_P4,
    O_D4,
    O_E4,
    O_SOD0,
    O_SOD1,
    O_SOD2,
    O_SOD3,
    O_SOD4,
    O_SOD5,
    O_SOD6,
    O_SOD7,
    O_TC1,
    O_TC2,
    O_TC3,
    O_TC4,
    O_TC5,
    O_TC6,
    O_TC7,
    O_TC8,
    O_TC9,
    O_TC0,
    O_RST,
    O_POT,
    O_LPWR,
    O_LCD,
    O_PRG,
    O_RUN,
    O_RGB,
    O_KP1,
    O_KP2,
    O_PWM,
    O_DISP1,
    O_DISP2,
    O_DISP3,
    O_DISP4,
    O_SOEN,
    O_SOCLK,
    O_SODATA,
    O_LCDRS,
    O_LCDEN,
    O_SCL,
    O_SDA,
    O_TX,
    O_RX,
    O_LED,
    O_MP
};

/*inputs*/

enum {
    I_TC1,
    I_TC2,
    I_TC3,
    I_TC4,
    I_TC5,
    I_TC6,
    I_TC7,
    I_TC8,
    I_TC9,
    I_TC0,
    I_POT,
    I_RST,
    I_PWR,
};

enum { TEMP, LIGHT };

cboard_PQDB::cboard_PQDB(void) {
    Proc = "PIC18F4520";

    vtc = 0;
    vt = 0;

    pot = 100;

    active = 0;

    vPOT = 0;
    vLDR = 2.5;
    vLM = 2.5;

    vp2[0] = 2.5;
    vp2[1] = 2.5;
    temp[0] = 27.5;
    temp[1] = 27.5;
    ref = 0;

    rpmstp = 0;  //(NSTEP*2)/100;
    rpmc = 0;

    d = 0;
    lcde = 0;
    sound_on = 0;

    sda = 0;

    srDATA = 0;
    srCLK = 0;
    srLAT = 0;

    lcd_init(&lcd, 16, 2, this);
    rtc_ds1307_init(&rtc2, this);
    io_74xx595_init(&shiftReg);
    _srret = 0;

    ReadMaps();

    buzzerId = PICSimLab.SystemCmd(PSC_AUDIOCHCREATE, NULL);

    scroll1_old = 255;  // force updated
    scroll2_old = 255;

    PICSimLab.UpdateGUI(TEMP, GT_SCROLL, GA_ADD, (void*)"Temp:");
    PICSimLab.UpdateGUI(LIGHT, GT_SCROLL, GA_ADD, (void*)"Light:");
}

cboard_PQDB::~cboard_PQDB(void) {
    PICSimLab.SystemCmd(PSC_AUDIOCHBEEPSTOP, (const char*)&buzzerId);
    PICSimLab.SystemCmd(PSC_AUDIOCHDESTROY, (const char*)&buzzerId);

    PICSimLab.UpdateGUI(TEMP, GT_SCROLL, GA_DEL, NULL);
    PICSimLab.UpdateGUI(LIGHT, GT_SCROLL, GA_DEL, NULL);

    rtc_ds1307_end(&rtc2);
    lcd_end(&lcd);
}

void cboard_PQDB::Draw(void) {
    int i;
    int update = 0;  // verifiy if updated is needed

    // pqdb draw
    for (i = 0; i < outputc; i++) {
        if (output[i].update)  // only if need update
        {
            output[i].update = 0;

            if (!update) {
                PICSimLab.CanvasCmd({.cmd = CC_INIT, .Init{Scale, Scale, 0}});
                PICSimLab.CanvasCmd({.cmd = CC_SETFONTWEIGHT, .SetFontWeight{CC_FONTWEIGHT_BOLD}});
            }
            update++;  // set to update buffer

            if (!output[i].r) {
                PICSimLab.CanvasCmd({.cmd = CC_SETFGCOLOR, .SetFgColor{30, 0, 0}});
                // seven segments display
                switch (output[i].id) {
                    case O_A1:
                    case O_B1:
                    case O_C1:
                    case O_D1:
                    case O_E1:
                    case O_F1:
                    case O_G1:
                    case O_A2:
                    case O_B2:
                    case O_C2:
                    case O_D2:
                    case O_E2:
                    case O_F2:
                    case O_G2:
                    case O_A3:
                    case O_B3:
                    case O_C3:
                    case O_D3:
                    case O_E3:
                    case O_F3:
                    case O_G3:
                    case O_A4:
                    case O_B4:
                    case O_C4:
                    case O_D4:
                    case O_E4:
                    case O_F4:
                    case O_G4:
                        PICSimLab.CanvasCmd(
                            {.cmd = CC_SETBGCOLOR, .SetBgColor{(unsigned int)lm7seg[output[i].id - O_B1], 30, 30}});
                        PICSimLab.CanvasCmd({.cmd = CC_SETFGCOLOR, .SetFgColor{10, 10, 10}});
                        break;
                    case O_MP:
                        PICSimLab.CanvasCmd({.cmd = CC_SETCOLOR, .SetColor{26, 26, 26}});
                        break;
                    case O_SS1:
                        output_ids[O_A1]->update = 1;
                        output_ids[O_B1]->update = 1;
                        output_ids[O_C1]->update = 1;
                        output_ids[O_D1]->update = 1;
                        output_ids[O_E1]->update = 1;
                        output_ids[O_F1]->update = 1;
                        output_ids[O_G1]->update = 1;
                        output_ids[O_P1]->update = 1;
                        PICSimLab.CanvasCmd({.cmd = CC_SETCOLOR, .SetColor{10, 10, 10}});
                        break;
                    case O_SS2:
                        output_ids[O_A2]->update = 1;
                        output_ids[O_B2]->update = 1;
                        output_ids[O_C2]->update = 1;
                        output_ids[O_D2]->update = 1;
                        output_ids[O_E2]->update = 1;
                        output_ids[O_F2]->update = 1;
                        output_ids[O_G2]->update = 1;
                        output_ids[O_P2]->update = 1;
                        PICSimLab.CanvasCmd({.cmd = CC_SETCOLOR, .SetColor{10, 10, 10}});
                        break;
                    case O_SS3:
                        output_ids[O_A3]->update = 1;
                        output_ids[O_B3]->update = 1;
                        output_ids[O_C3]->update = 1;
                        output_ids[O_D3]->update = 1;
                        output_ids[O_E3]->update = 1;
                        output_ids[O_F3]->update = 1;
                        output_ids[O_G3]->update = 1;
                        output_ids[O_P3]->update = 1;
                        PICSimLab.CanvasCmd({.cmd = CC_SETCOLOR, .SetColor{10, 10, 10}});
                        break;
                    case O_SS4:
                        output_ids[O_A4]->update = 1;
                        output_ids[O_B4]->update = 1;
                        output_ids[O_C4]->update = 1;
                        output_ids[O_D4]->update = 1;
                        output_ids[O_E4]->update = 1;
                        output_ids[O_F4]->update = 1;
                        output_ids[O_G4]->update = 1;
                        output_ids[O_P4]->update = 1;
                        PICSimLab.CanvasCmd({.cmd = CC_SETCOLOR, .SetColor{10, 10, 10}});
                        break;
                    case O_LCD:
                        PICSimLab.CanvasCmd(
                            {.cmd = CC_SETCOLOR, .SetColor{0, (unsigned int)(90 * PICSimLab.GetMcuPwr() + 40), 0}});
                        break;
                    case O_RST:
                        PICSimLab.CanvasCmd({.cmd = CC_SETCOLOR, .SetColor{100, 100, 100}});
                        PICSimLab.CanvasCmd({.cmd = CC_CIRCLE, .Circle{1, output[i].cx, output[i].cy, 11}});
                        if (p_RST) {
                            PICSimLab.CanvasCmd({.cmd = CC_SETCOLOR, .SetColor{15, 15, 15}});
                        } else {
                            PICSimLab.CanvasCmd({.cmd = CC_SETCOLOR, .SetColor{55, 55, 55}});
                        }
                        break;
                    case O_LPWR:
                        PICSimLab.CanvasCmd(
                            {.cmd = CC_SETCOLOR, .SetColor{0, (unsigned int)(255 * PICSimLab.GetMcuPwr()), 0}});
                        break;
                    case O_LED:
                        PICSimLab.CanvasCmd({.cmd = CC_SETCOLOR, .SetColor{0, (unsigned int)pic.pins[6].oavalue, 0}});
                        break;
                    case O_SOEN:
                        selectColorByPinValue(SO_EN_PIN);
                        break;
                    case O_SOCLK:
                        selectColorByPinValue(SO_CLK_PIN);
                        break;
                    case O_SODATA:
                        selectColorByPinValue(SO_DATA_PIN);
                        break;
                    case O_DISP1:
                        selectColorByPinValue(DISP_1_PIN);
                        break;
                    case O_DISP2:
                        selectColorByPinValue(DISP_2_PIN);
                        break;
                    case O_DISP3:
                        selectColorByPinValue(DISP_3_PIN);
                        break;
                    case O_DISP4:
                        selectColorByPinValue(DISP_4_PIN);
                        break;
                    case O_KP1:
                        selectColorByPinValue(KEYPAD_1_PIN);
                        break;
                    case O_KP2:
                        selectColorByPinValue(KEYPAD_2_PIN);
                        break;
                    case O_PWM:
                        selectColorByPinValue(PWM_PIN);
                        break;
                    case O_LCDRS:
                        selectColorByPinValue(LCD_RS_PIN);
                        break;
                    case O_LCDEN:
                        selectColorByPinValue(LCD_EN_PIN);
                        break;
                    case O_SCL:
                        selectColorByPinValue(SCL_PIN);
                        break;
                    case O_SDA:
                        // use sda instead pinValue because switching input/output
                        selectColorByValue(sda);
                        break;
                    case O_TX:
                        selectColorByPinValue(TX_PIN);
                        break;
                    case O_RX:
                        selectColorByPinValue(RX_PIN);
                        break;
                    case O_SOD0:
                    case O_SOD1:
                    case O_SOD2:
                    case O_SOD3:
                    case O_SOD4:
                    case O_SOD5:
                    case O_SOD6:
                    case O_SOD7:
                        selectColorByPinValue(PSRD0 + (output[i].id - O_SOD0));
                        break;
                    case O_TC1:
                    case O_TC2:
                    case O_TC3:
                    case O_TC4:
                    case O_TC5:
                    case O_TC6:
                    case O_TC7:
                    case O_TC8:
                    case O_TC9:
                    case O_TC0:
                        PICSimLab.CanvasCmd({.cmd = CC_SETCOLOR, .SetColor{100, 100, 100}});
                        PICSimLab.CanvasCmd({.cmd = CC_CIRCLE, .Circle{1, output[i].cx, output[i].cy, 11}});
                        if (p_KEY[output[i].id - O_TC1]) {
                            PICSimLab.CanvasCmd({.cmd = CC_SETCOLOR, .SetColor{55, 55, 55}});
                        } else {
                            PICSimLab.CanvasCmd({.cmd = CC_SETCOLOR, .SetColor{15, 15, 15}});
                        }
                        break;
                    default:
                        break;
                }

                if ((output[i].id >= O_TC1) && (output[i].id <= O_TC0)) {
                    PICSimLab.CanvasCmd({.cmd = CC_CIRCLE, .Circle{1, output[i].cx, output[i].cy, 9}});
                } else if (output[i].id == O_RST) {
                    PICSimLab.CanvasCmd({.cmd = CC_CIRCLE, .Circle{1, output[i].cx, output[i].cy, 9}});
                } else if (output[i].id == O_POT) {
                    PICSimLab.CanvasCmd({.cmd = CC_SETBGCOLOR, .SetBgColor{66, 109, 246}});
                    PICSimLab.CanvasCmd({.cmd = CC_RECTANGLE,
                                         .Rectangle{1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1,
                                                    output[i].y2 - output[i].y1}});

                    PICSimLab.CanvasCmd({.cmd = CC_SETBGCOLOR, .SetBgColor{250, 250, 250}});
                    PICSimLab.CanvasCmd({.cmd = CC_CIRCLE, .Circle{1, output[i].cx, output[i].cy, 20}});

                    PICSimLab.CanvasCmd({.cmd = CC_SETBGCOLOR, .SetBgColor{150, 150, 150}});
                    int x = -15 * sin((5.585 * (pot / 200.0)) + 0.349);
                    int y = 15 * cos((5.585 * (pot / 200.0)) + 0.349);
                    PICSimLab.CanvasCmd({.cmd = CC_CIRCLE, .Circle{1, output[i].cx + x, output[i].cy + y, 3}});
                } else if (output[i].id == O_LCD) {  // draw lcd text
                                                     // strech lcd background
                    PICSimLab.CanvasCmd(
                        {.cmd = CC_RECTANGLE,
                         .Rectangle{1, output[i].x1 - 15, output[i].y1 - 5, output[i].x2 - output[i].x1 + 32,
                                    output[i].y2 - output[i].y1 + 13}});
                    lcd_draw(&lcd, PICSimLab.CanvasCmd, output[i].x1, output[i].y1, output[i].x2 - output[i].x1,
                             output[i].y2 - output[i].y1, PICSimLab.GetMcuPwr());
                } else if (output[i].id == O_MP) {
                    PICSimLab.CanvasCmd({.cmd = CC_SETFONTSIZE, .SetFontSize{10}});
                    PICSimLab.CanvasCmd({.cmd = CC_RECTANGLE,
                                         .Rectangle{1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1,
                                                    output[i].y2 - output[i].y1}});
                    PICSimLab.CanvasCmd({.cmd = CC_SETCOLOR, .SetColor{230, 230, 230}});
                    PICSimLab.CanvasCmd(
                        {.cmd = CC_ROTATEDTEXT, .RotatedText{Proc.c_str(), output[i].x1 + 85, output[i].y1 + 5, 0}});
                } else {
                    PICSimLab.CanvasCmd({.cmd = CC_RECTANGLE,
                                         .Rectangle{1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1,
                                                    output[i].y2 - output[i].y1}});
                }
            } else {
                if (output[i].id == O_RGB) {
                    PICSimLab.CanvasCmd({.cmd = CC_SETFGCOLOR, .SetFgColor{55, 55, 55}});
                    PICSimLab.CanvasCmd({.cmd = CC_SETBGCOLOR,
                                         .SetBgColor{(unsigned int)pic.pins[LED_RED_PIN].oavalue,
                                                     (unsigned int)pic.pins[LED_GREEN_PIN].oavalue,
                                                     (unsigned int)pic.pins[LED_BLUE_PIN].oavalue}});
                    DrawLED(PICSimLab.CanvasCmd, &output[i]);
                } else if ((output[i].id == O_P1) || (output[i].id == O_P2) || (output[i].id == O_P3) ||
                           (output[i].id == O_P4)) {
                    PICSimLab.CanvasCmd(
                        {.cmd = CC_SETBGCOLOR, .SetBgColor{(unsigned int)lm7seg[output[i].id - O_B1], 30, 30}});
                    PICSimLab.CanvasCmd({.cmd = CC_SETFGCOLOR, .SetFgColor{10, 10, 10}});
                    PICSimLab.CanvasCmd({.cmd = CC_CIRCLE, .Circle{1, output[i].x1, output[i].y1, output[i].r}});
                }
            }
        }
    }

    // end draw

    if (update) {
        PICSimLab.CanvasCmd({.cmd = CC_END});
    }

    if (((0.5 * (pic.pins[PWM_PIN].oavalue - 55)) > 10) && (PICSimLab.GetMcuPwr())) {
        if (!sound_on) {
            PICSimLab.SystemCmd(PSC_AUDIOCHBEEPSTART, (const char*)&buzzerId);
            sound_on = 1;
        }
    } else {
        PICSimLab.SystemCmd(PSC_AUDIOCHBEEPSTOP, (const char*)&buzzerId);
        sound_on = 0;
    }

    // tensão p2
    vPOT = (3.3 * pot / 199);

    int position = 0;
    PICSimLab.UpdateGUI(TEMP, GT_SCROLL, GA_GET, (void*)&position);
    if (position != scroll1_old) {
        char buff[100];
        scroll1_old = position;
        vLM = (1.5 * scroll1_old / 199);
        sprintf(buff,
                "Te"
                "mp"
                ": "
                "%4"
                ".1"
                "f "
                "C",
                scroll1_old * 150.0 / 199.0);
        PICSimLab.UpdateGUI(TEMP, GT_SCROLL, GA_SET_LABEL, (void*)buff);
    }

    PICSimLab.UpdateGUI(LIGHT, GT_SCROLL, GA_GET, (void*)&position);
    if (position != scroll2_old) {
        char buff[100];
        scroll2_old = position;
        const float lux = powf(10, ((scroll2_old + 0.98) / 33.33) - 1);
        const float gamma = 0.7;
        const float r10 = 20000.0;
        const float res = r10 / (powf(10, gamma * log10(lux / 10.0)));
        vLDR = (res * 3.3) / (res + 10000.0);
        sprintf(buff,
                "Li"
                "gh"
                "t:"
                " %"
                "4."
                "0f"
                " l"
                "u"
                "x",
                lux);
        PICSimLab.UpdateGUI(LIGHT, GT_SCROLL, GA_SET_LABEL, (void*)buff);
    }
}

void cboard_PQDB::Run_CPU(void) {
    int i;
    int j;
    int pi;
    const picpin* pins;

    unsigned int alm[40];  // valor médio dos
                           // pinos de IO

    unsigned int alm7seg[32];  // luminosidade media
                               // display 7 seg

    const int JUMPSTEPS = PICSimLab.GetJUMPSTEPS();
    const long int NSTEP = PICSimLab.GetNSTEP();
    const float RNSTEP = 200.0 * pic.PINCOUNT / NSTEP;

    if (use_spare)
        SpareParts.PreProcess();

    memset(alm7seg, 0, 32 * sizeof(unsigned int));

    memset(alm, 0, 40 * sizeof(unsigned int));

    memset(shiftReg_alm, 0, 8 * sizeof(unsigned long));

    pins = pic.pins;

    j = JUMPSTEPS;
    pi = 0;
    if (PICSimLab.GetMcuPwr()) {
        for (i = 0; i < NSTEP; i++) {
            if (j >= JUMPSTEPS) {
                pic_set_pin(&pic, pic.mclr, p_RST);

                // keyboard
                // D3-7 do shiftReg
                // 0-9: UDLRS sABXY
                if (pins[KEYPAD_1_PIN].dir) {
                    if ((p_KEY[0] && (shiftReg.out & SRD3)) || (p_KEY[1] && (shiftReg.out & SRD4)) ||
                        (p_KEY[2] && (shiftReg.out & SRD5)) || (p_KEY[3] && (shiftReg.out & SRD6)) ||
                        (p_KEY[4] && (shiftReg.out & SRD7))) {
                        pic_set_pin(&pic, KEYPAD_1_PIN + 1, 1);
                    } else {
                        pic_set_pin(&pic, KEYPAD_1_PIN + 1, 0);
                    }
                }
                if (pins[KEYPAD_2_PIN].dir) {
                    if ((p_KEY[5] && (shiftReg.out & SRD3)) || (p_KEY[6] && (shiftReg.out & SRD4)) ||
                        (p_KEY[7] && (shiftReg.out & SRD5)) || (p_KEY[8] && (shiftReg.out & SRD6)) ||
                        (p_KEY[9] && (shiftReg.out & SRD7))) {
                        pic_set_pin(&pic, KEYPAD_2_PIN + 1, 1);
                    } else {
                        pic_set_pin(&pic, KEYPAD_2_PIN + 1, 0);
                    }
                }
            }

            if (!mplabxd_testbp())
                pic_step(&pic);
            ioupdated = pic.ioupdated;
            InstCounterInc();

            if (ioupdated) {
                // keyboard
                // D3-7 do shiftReg
                // 0-9: UDLRS sABXY
                if (pins[KEYPAD_1_PIN].dir) {
                    if ((p_KEY[0] && (shiftReg.out & SRD3)) || (p_KEY[1] && (shiftReg.out & SRD4)) ||
                        (p_KEY[2] && (shiftReg.out & SRD5)) || (p_KEY[3] && (shiftReg.out & SRD6)) ||
                        (p_KEY[4] && (shiftReg.out & SRD7))) {
                        pic_set_pin(&pic, KEYPAD_1_PIN + 1, 1);
                    } else {
                        pic_set_pin(&pic, KEYPAD_1_PIN + 1, 0);
                    }
                }
                if (pins[KEYPAD_2_PIN].dir) {
                    if ((p_KEY[5] && (shiftReg.out & SRD3)) || (p_KEY[6] && (shiftReg.out & SRD4)) ||
                        (p_KEY[7] && (shiftReg.out & SRD5)) || (p_KEY[8] && (shiftReg.out & SRD6)) ||
                        (p_KEY[9] && (shiftReg.out & SRD7))) {
                        pic_set_pin(&pic, KEYPAD_2_PIN + 1, 1);
                    } else {
                        pic_set_pin(&pic, KEYPAD_2_PIN + 1, 0);
                    }
                }
            }

            if (use_oscope)
                Oscilloscope.SetSample();
            if (use_spare)
                SpareParts.Process();

            // increment mean value
            // counter if pin is
            // high
            alm[pi] += pins[pi].value;
            pi++;
            if (pi == pic.PINCOUNT)
                pi = 0;

            if (j >= JUMPSTEPS) {
                // contabilizando a
                // média do 7
                // segmentos
                for (int iDisp = DISP_1_PIN; iDisp <= DISP_4_PIN; iDisp++) {
                    if (pins[iDisp].value && !pins[iDisp].dir) {
                        for (int iSeg = 0; iSeg < 8; iSeg++) {
                            if (shiftReg.out & (1 << iSeg)) {
                                alm7seg[(iDisp - DISP_1_PIN) * 8 + iSeg]++;
                            }
                        }
                    }
                }

                alm[32] = 0;

                // potenciometro
                pic_set_apin(&pic, POT_PIN + 1,
                             vPOT);  // pot
                pic_set_apin(&pic, LDR_PIN + 1,
                             vLDR);  // ldr
                pic_set_apin(&pic, LM_PIN + 1,
                             vLM);  // temp

                // valor medio shift
                // register
                if (pic.pins[pic.PINCOUNT].value)
                    shiftReg_alm[0]++;
                if (pic.pins[pic.PINCOUNT + 1].value)
                    shiftReg_alm[1]++;
                if (pic.pins[pic.PINCOUNT + 2].value)
                    shiftReg_alm[2]++;
                if (pic.pins[pic.PINCOUNT + 3].value)
                    shiftReg_alm[3]++;
                if (pic.pins[pic.PINCOUNT + 4].value)
                    shiftReg_alm[4]++;
                if (pic.pins[pic.PINCOUNT + 5].value)
                    shiftReg_alm[5]++;
                if (pic.pins[pic.PINCOUNT + 6].value)
                    shiftReg_alm[6]++;
                if (pic.pins[pic.PINCOUNT + 7].value)
                    shiftReg_alm[7]++;

                j = -1;
            }
            j++;

            if (ioupdated) {
                // lcd display code
                if ((!pins[LCD_EN_PIN].dir) && (!pins[LCD_EN_PIN].value)) {
                    if (!lcde) {
                        d = (shiftReg.out & 0x0f) << 4;

                        if ((!pins[LCD_RS_PIN].dir) && (!pins[LCD_RS_PIN].value)) {
                            lcd_cmd(&lcd, d);
                        } else if ((!pins[LCD_RS_PIN].dir) && (pins[LCD_RS_PIN].value)) {
                            lcd_data(&lcd, d);
                        }
                        lcde = 1;
                    }
                } else {
                    lcde = 0;
                }
                // end display code

                // ds1307 over i2c
                // code
                if (pins[SDA_PIN].dir) {
                    sda = 1;
                } else {
                    sda = pins[SDA_PIN].value;
                }
                if (pins[SCL_PIN].dir) {
                    sck = 1;
                    pic_set_pin(&pic, SCL_PIN + 1, 1);
                } else {
                    sck = pins[SCL_PIN].value;
                }
                pic_set_pin(&pic, SDA_PIN + 1, rtc_ds1307_I2C_io(&rtc2, sck, sda));

                // 74hc595 code
                if (pins[SO_DATA_PIN].dir == 0) {
                    srDATA = pins[SO_DATA_PIN].value;
                }
                if (pins[SO_CLK_PIN].dir == 0) {
                    srCLK = pins[SO_CLK_PIN].value;
                }
                if (pins[SO_EN_PIN].dir == 0) {
                    srLAT = pins[SO_EN_PIN].value;
                }
                unsigned short ret = io_74xx595_io(&shiftReg, srDATA, srCLK, srLAT, 1);
                if (_srret != ret) {
                    pic.pins[PSRD0].value = (ret & 0x01) != 0;
                    pic.pins[PSRD1].value = (ret & 0x02) != 0;
                    pic.pins[PSRD2].value = (ret & 0x04) != 0;
                    pic.pins[PSRD3].value = (ret & 0x08) != 0;
                    pic.pins[PSRD4].value = (ret & 0x10) != 0;
                    pic.pins[PSRD5].value = (ret & 0x20) != 0;
                    pic.pins[PSRD6].value = (ret & 0x40) != 0;
                    pic.pins[PSRD7].value = (ret & 0x80) != 0;
                }
                _srret = ret;
            }
        }
        pic.ioupdated = 0;
    }
    // fim STEP

    // alm[23] =
    // 0;
    // //aquecedor
    // alm[16] =
    // 0;
    // //ventilador

    for (i = 0; i < pic.PINCOUNT; i++) {
        if (pic.pins[i].port == P_VDD) {
            pic.pins[i].oavalue = 255;
        } else {
            pic.pins[i].oavalue = (int)((alm[i] * RNSTEP) + 55);
        }
    }

    const long int NSTEPJ = PICSimLab.GetNSTEPJ();

    for (i = 0; i < 32; i++) {
        lm7seg[i] = (int)(((3000.0 * alm7seg[i]) / NSTEPJ) + 30);
        if (lm7seg[i] > 255)
            lm7seg[i] = 255;
    }

    // sr mean
    // value
    pic.pins[PSRD0].oavalue = (pic.pins[PSRD0].oavalue + ((shiftReg_alm[0] * 200.0) / NSTEPJ) + 55) / 2;
    pic.pins[PSRD1].oavalue = (pic.pins[PSRD1].oavalue + ((shiftReg_alm[1] * 200.0) / NSTEPJ) + 55) / 2;
    pic.pins[PSRD2].oavalue = (pic.pins[PSRD2].oavalue + ((shiftReg_alm[2] * 200.0) / NSTEPJ) + 55) / 2;
    pic.pins[PSRD3].oavalue = (pic.pins[PSRD3].oavalue + ((shiftReg_alm[3] * 200.0) / NSTEPJ) + 55) / 2;
    pic.pins[PSRD4].oavalue = (pic.pins[PSRD4].oavalue + ((shiftReg_alm[4] * 200.0) / NSTEPJ) + 55) / 2;
    pic.pins[PSRD5].oavalue = (pic.pins[PSRD5].oavalue + ((shiftReg_alm[5] * 200.0) / NSTEPJ) + 55) / 2;
    pic.pins[PSRD6].oavalue = (pic.pins[PSRD6].oavalue + ((shiftReg_alm[6] * 200.0) / NSTEPJ) + 55) / 2;
    pic.pins[PSRD7].oavalue = (pic.pins[PSRD7].oavalue + ((shiftReg_alm[7] * 200.0) / NSTEPJ) + 55) / 2;
    if (use_spare)
        SpareParts.PostProcess();

    if (lcd.update)
        output_ids[O_LCD]->update = 1;

    // verifiy
    // if LEDS
    // need
    // update
    for (i = 0; i < 32; i++) {
        if (output_ids[O_B1 + i]->value != lm7seg[i]) {
            output_ids[O_B1 + i]->value = lm7seg[i];
            output_ids[O_SS1 + (i >> 3)]->update = 1;
        }
    }

    for (i = 0; i < 8; i++) {
        if (output_ids[O_SOD0 + i]->value != pic.pins[PSRD0 + i].oavalue) {
            output_ids[O_SOD0 + i]->value = pic.pins[PSRD0 + i].oavalue;
            output_ids[O_SOD0 + i]->update = 1;
        }
    }

    if (output_ids[O_LED]->value != pic.pins[6].oavalue) {
        output_ids[O_LED]->value = pic.pins[6].oavalue;
        output_ids[O_LED]->update = 1;
    }
    if (output_ids[O_SOEN]->value != pic.pins[SO_EN_PIN].oavalue) {
        output_ids[O_SOEN]->value = pic.pins[SO_EN_PIN].oavalue;
        output_ids[O_SOEN]->update = 1;
    }
    if (output_ids[O_SOCLK]->value != pic.pins[SO_CLK_PIN].oavalue) {
        output_ids[O_SOCLK]->value = pic.pins[SO_CLK_PIN].oavalue;
        output_ids[O_SOCLK]->update = 1;
    }
    if (output_ids[O_SODATA]->value != pic.pins[SO_DATA_PIN].oavalue) {
        output_ids[O_SODATA]->value = pic.pins[SO_DATA_PIN].oavalue;
        output_ids[O_SODATA]->update = 1;
    }
    if (output_ids[O_DISP1]->value != pic.pins[DISP_1_PIN].oavalue) {
        output_ids[O_DISP1]->value = pic.pins[DISP_1_PIN].oavalue;
        output_ids[O_DISP1]->update = 1;
        output_ids[O_RGB]->update = 1;
    }
    if (output_ids[O_DISP2]->value != pic.pins[DISP_2_PIN].oavalue) {
        output_ids[O_DISP2]->value = pic.pins[DISP_2_PIN].oavalue;
        output_ids[O_DISP2]->update = 1;
        output_ids[O_RGB]->update = 1;
    }
    if (output_ids[O_DISP3]->value != pic.pins[DISP_3_PIN].oavalue) {
        output_ids[O_DISP3]->value = pic.pins[DISP_3_PIN].oavalue;
        output_ids[O_DISP3]->update = 1;
        output_ids[O_RGB]->update = 1;
    }
    if (output_ids[O_DISP4]->value != pic.pins[DISP_4_PIN].oavalue) {
        output_ids[O_DISP4]->value = pic.pins[DISP_4_PIN].oavalue;
        output_ids[O_DISP4]->update = 1;
    }
    if (output_ids[O_KP1]->value != pic.pins[KEYPAD_1_PIN].oavalue) {
        output_ids[O_KP1]->value = pic.pins[KEYPAD_1_PIN].oavalue;
        output_ids[O_KP1]->update = 1;
    }
    if (output_ids[O_KP2]->value != pic.pins[KEYPAD_2_PIN].oavalue) {
        output_ids[O_KP2]->value = pic.pins[KEYPAD_2_PIN].oavalue;
        output_ids[O_KP2]->update = 1;
    }
    if (output_ids[O_PWM]->value != pic.pins[PWM_PIN].oavalue) {
        output_ids[O_PWM]->value = pic.pins[PWM_PIN].oavalue;
        output_ids[O_PWM]->update = 1;
    }
    if (output_ids[O_LCDRS]->value != pic.pins[LCD_RS_PIN].oavalue) {
        output_ids[O_LCDRS]->value = pic.pins[LCD_RS_PIN].oavalue;
        output_ids[O_LCDRS]->update = 1;
    }
    if (output_ids[O_LCDEN]->value != pic.pins[LCD_EN_PIN].oavalue) {
        output_ids[O_LCDEN]->value = pic.pins[LCD_EN_PIN].oavalue;
        output_ids[O_LCDEN]->update = 1;
    }
    if (output_ids[O_SCL]->value != pic.pins[SCL_PIN].oavalue) {
        output_ids[O_SCL]->value = pic.pins[SCL_PIN].oavalue;
        output_ids[O_SCL]->update = 1;
    }
    if (output_ids[O_SDA]->value != sda) {
        output_ids[O_SDA]->value = sda;
        output_ids[O_SDA]->update = 1;
    }
    if (output_ids[O_TX]->value != pic.pins[TX_PIN].oavalue) {
        output_ids[O_TX]->value = pic.pins[TX_PIN].oavalue;
        output_ids[O_TX]->update = 1;
    }
    if (output_ids[O_RX]->value != pic.pins[RX_PIN].oavalue) {
        output_ids[O_RX]->value = pic.pins[RX_PIN].oavalue;
        output_ids[O_RX]->update = 1;
    }
}

void cboard_PQDB::Reset(void) {
    pic_reset(&pic, 1);

    lcd_rst(&lcd);
    rtc_ds1307_rst(&rtc2);
    io_74xx595_rst(&shiftReg);

    p_KEY[0] = 0;
    p_KEY[1] = 0;
    p_KEY[2] = 0;
    p_KEY[3] = 0;
    p_KEY[4] = 0;
    p_KEY[5] = 0;
    p_KEY[6] = 0;
    p_KEY[7] = 0;
    p_KEY[8] = 0;
    p_KEY[9] = 0;

    PICSimLab.UpdateStatus(PS_SERIAL, GetUARTStrStatus(0));

    if (use_spare)
        SpareParts.Reset();

    for (int i = 0; i < 8; i++) {
        pic.pins[PSRD0 + i].ptype = PT_DIGITAL;
        pic.pins[PSRD0 + i].dir = PD_OUT;
    }

    for (int i = 0; i < 32; i++) {
        lm7seg[i] = 30;
    }
    RegisterRemoteControl();
}

void cboard_PQDB::RegisterRemoteControl(void) {
    input_ids[I_TC1]->status = &p_KEY[0];
    input_ids[I_TC1]->update = &output_ids[O_TC1]->update;
    input_ids[I_TC2]->status = &p_KEY[1];
    input_ids[I_TC2]->update = &output_ids[O_TC2]->update;
    input_ids[I_TC3]->status = &p_KEY[2];
    input_ids[I_TC3]->update = &output_ids[O_TC3]->update;
    input_ids[I_TC4]->status = &p_KEY[3];
    input_ids[I_TC4]->update = &output_ids[O_TC4]->update;
    input_ids[I_TC5]->status = &p_KEY[4];
    input_ids[I_TC5]->update = &output_ids[O_TC5]->update;
    input_ids[I_TC6]->status = &p_KEY[5];
    input_ids[I_TC6]->update = &output_ids[O_TC6]->update;
    input_ids[I_TC7]->status = &p_KEY[6];
    input_ids[I_TC7]->update = &output_ids[O_TC7]->update;
    input_ids[I_TC8]->status = &p_KEY[7];
    input_ids[I_TC8]->update = &output_ids[O_TC8]->update;
    input_ids[I_TC9]->status = &p_KEY[8];
    input_ids[I_TC9]->update = &output_ids[O_TC9]->update;
    input_ids[I_TC0]->status = &p_KEY[9];
    input_ids[I_TC0]->update = &output_ids[O_TC0]->update;
    input_ids[I_POT]->status = &pot;
    input_ids[I_POT]->update = &output_ids[O_POT]->update;

    output_ids[O_LCD]->status = &lcd;
    output_ids[O_B1]->status = &lm7seg[0];
    output_ids[O_F1]->status = &lm7seg[1];
    output_ids[O_A1]->status = &lm7seg[2];
    output_ids[O_G1]->status = &lm7seg[3];
    output_ids[O_C1]->status = &lm7seg[4];
    output_ids[O_P1]->status = &lm7seg[5];
    output_ids[O_D1]->status = &lm7seg[6];
    output_ids[O_E1]->status = &lm7seg[7];
    output_ids[O_B2]->status = &lm7seg[8];
    output_ids[O_F2]->status = &lm7seg[9];
    output_ids[O_A2]->status = &lm7seg[10];
    output_ids[O_G2]->status = &lm7seg[11];
    output_ids[O_C2]->status = &lm7seg[12];
    output_ids[O_P2]->status = &lm7seg[13];
    output_ids[O_D2]->status = &lm7seg[14];
    output_ids[O_E2]->status = &lm7seg[15];
    output_ids[O_B3]->status = &lm7seg[16];
    output_ids[O_F3]->status = &lm7seg[17];
    output_ids[O_A3]->status = &lm7seg[18];
    output_ids[O_G3]->status = &lm7seg[19];
    output_ids[O_C3]->status = &lm7seg[20];
    output_ids[O_P3]->status = &lm7seg[21];
    output_ids[O_D3]->status = &lm7seg[22];
    output_ids[O_E3]->status = &lm7seg[23];
    output_ids[O_B4]->status = &lm7seg[24];
    output_ids[O_F4]->status = &lm7seg[25];
    output_ids[O_A4]->status = &lm7seg[26];
    output_ids[O_G4]->status = &lm7seg[27];
    output_ids[O_C4]->status = &lm7seg[28];
    output_ids[O_P4]->status = &lm7seg[29];
    output_ids[O_D4]->status = &lm7seg[30];
    output_ids[O_E4]->status = &lm7seg[31];
}

void cboard_PQDB::EvMouseMove(unsigned int button, unsigned int x, unsigned int y, unsigned int state) {
    int i;

    for (i = 0; i < inputc; i++) {
        switch (input[i].id) {
            case I_POT:
                if (((input[i].x1 <= x) && (input[i].x2 >= x)) && ((input[i].y1 <= y) && (input[i].y2 >= y))) {
                    if (active) {
                        pot = CalcAngle(i, x, y);
                        output_ids[O_POT]->update = 1;
                    }
                } else {
                    active = 0;
                }
                break;
        }
    }
}

void cboard_PQDB::EvMouseButtonPress(unsigned int button, unsigned int x, unsigned int y, unsigned int state) {
    int i;

    for (i = 0; i < inputc; i++) {
        if ((input[i].x1 <= x) && (input[i].x2 >= x) && (input[i].y1 <= y) && (input[i].y2 >= y)) {
            switch (input[i].id) {
                case I_PWR: {
                    if (PICSimLab.GetMcuPwr()) {
                        PICSimLab.SetMcuPwr(0);
                        Reset();
                    } else {
                        PICSimLab.SetMcuPwr(1);
                        Reset();
                    }
                }
                    output_ids[O_LPWR]->update = 1;
                    break;

                case I_RST: {
                    if (PICSimLab.GetMcuPwr() && pic_reset(&pic, -1)) {
                        PICSimLab.SetMcuPwr(0);
                        PICSimLab.SetMcuRst(1);
                    }
                    p_RST = 0;
                    output_ids[O_RST]->update = 1;
                } break;

                case I_TC1: {
                    p_KEY[0] = 1;
                    output_ids[O_TC1]->update = 1;
                } break;
                case I_TC2: {
                    p_KEY[1] = 1;
                    output_ids[O_TC2]->update = 1;
                } break;
                case I_TC3: {
                    p_KEY[2] = 1;
                    output_ids[O_TC3]->update = 1;
                } break;

                case I_TC4: {
                    p_KEY[3] = 1;
                    output_ids[O_TC4]->update = 1;
                } break;
                case I_TC5: {
                    p_KEY[4] = 1;
                    output_ids[O_TC5]->update = 1;
                } break;
                case I_TC6: {
                    p_KEY[5] = 1;
                    output_ids[O_TC6]->update = 1;
                } break;

                case I_TC7: {
                    p_KEY[6] = 1;
                    output_ids[O_TC7]->update = 1;
                } break;
                case I_TC8: {
                    p_KEY[7] = 1;
                    output_ids[O_TC8]->update = 1;
                } break;
                case I_TC9: {
                    p_KEY[8] = 1;
                    output_ids[O_TC9]->update = 1;
                } break;

                case I_TC0: {
                    p_KEY[9] = 1;
                    output_ids[O_TC0]->update = 1;
                } break;

                case I_POT: {
                    active = 1;
                    pot = CalcAngle(i, x, y);
                    output_ids[O_POT]->update = 1;
                } break;
            }
        }
    }
}

void cboard_PQDB::EvMouseButtonRelease(unsigned int button, unsigned int x, unsigned int y, unsigned int state) {
    int i;

    for (i = 0; i < inputc; i++) {
        if (((input[i].x1 <= x) && (input[i].x2 >= x)) && ((input[i].y1 <= y) && (input[i].y2 >= y))) {
            switch (input[i].id) {
                case I_RST: {
                    if (PICSimLab.GetMcuRst()) {
                        PICSimLab.SetMcuPwr(1);
                        PICSimLab.SetMcuRst(0);

                        if (pic_reset(&pic, -1)) {
                            Reset();
                        }
                    }
                    p_RST = 1;
                    output_ids[O_RST]->update = 1;
                } break;

                case I_TC1: {
                    p_KEY[0] = 0;
                    output_ids[O_TC1]->update = 1;
                } break;
                case I_TC2: {
                    p_KEY[1] = 0;
                    output_ids[O_TC2]->update = 1;
                } break;
                case I_TC3: {
                    p_KEY[2] = 0;
                    output_ids[O_TC3]->update = 1;
                } break;

                case I_TC4: {
                    p_KEY[3] = 0;
                    output_ids[O_TC4]->update = 1;
                } break;
                case I_TC5: {
                    p_KEY[4] = 0;
                    output_ids[O_TC5]->update = 1;
                } break;
                case I_TC6: {
                    p_KEY[5] = 0;
                    output_ids[O_TC6]->update = 1;
                } break;

                case I_TC7: {
                    p_KEY[6] = 0;
                    output_ids[O_TC7]->update = 1;
                } break;
                case I_TC8: {
                    p_KEY[7] = 0;
                    output_ids[O_TC8]->update = 1;
                } break;
                case I_TC9: {
                    p_KEY[8] = 0;
                    output_ids[O_TC9]->update = 1;
                } break;

                case I_TC0: {
                    p_KEY[9] = 0;
                    output_ids[O_TC0]->update = 1;
                } break;
                case I_POT: {
                    active = 0;
                    output_ids[O_POT]->update = 1;
                } break;
            }
        }
    }
}

void cboard_PQDB::EvKeyPress(unsigned int key, unsigned int mask) {
    if (key == '1') {
        p_KEY[0] = 1;
        output_ids[O_TC1]->update = 1;
    }
    if (key == '2') {
        p_KEY[1] = 1;
        output_ids[O_TC2]->update = 1;
    }
    if (key == '3') {
        p_KEY[2] = 1;
        output_ids[O_TC3]->update = 1;
    }
    if (key == '4') {
        p_KEY[3] = 1;
        output_ids[O_TC4]->update = 1;
    }
    if (key == '5') {
        p_KEY[4] = 1;
        output_ids[O_TC5]->update = 1;
    }
    if (key == '6') {
        p_KEY[5] = 1;
        output_ids[O_TC6]->update = 1;
    }
    if (key == '7') {
        p_KEY[6] = 1;
        output_ids[O_TC7]->update = 1;
    }
    if (key == '8') {
        p_KEY[7] = 1;
        output_ids[O_TC8]->update = 1;
    }
    if (key == '9') {
        p_KEY[8] = 1;
        output_ids[O_TC9]->update = 1;
    }
    if (key == '0') {
        p_KEY[9] = 1;
        output_ids[O_TC0]->update = 1;
    }
}

void cboard_PQDB::EvKeyRelease(unsigned int key, unsigned int mask) {
    if (key == '1') {
        p_KEY[0] = 0;
        output_ids[O_TC1]->update = 1;
    }
    if (key == '2') {
        p_KEY[1] = 0;
        output_ids[O_TC2]->update = 1;
    }
    if (key == '3') {
        p_KEY[2] = 0;
        output_ids[O_TC3]->update = 1;
    }
    if (key == '4') {
        p_KEY[3] = 0;
        output_ids[O_TC4]->update = 1;
    }
    if (key == '5') {
        p_KEY[4] = 0;
        output_ids[O_TC5]->update = 1;
    }
    if (key == '6') {
        p_KEY[5] = 0;
        output_ids[O_TC6]->update = 1;
    }
    if (key == '7') {
        p_KEY[6] = 0;
        output_ids[O_TC7]->update = 1;
    }
    if (key == '8') {
        p_KEY[7] = 0;
        output_ids[O_TC8]->update = 1;
    }
    if (key == '9') {
        p_KEY[8] = 0;
        output_ids[O_TC9]->update = 1;
    }
    if (key == '0') {
        p_KEY[9] = 0;
        output_ids[O_TC0]->update = 1;
    }
}

void cboard_PQDB::EvOnShow(void) {
    lcd.update = 1;
    board::EvOnShow();
}

unsigned short cboard_PQDB::GetInputId(char* name) {
    if (strcmp(name,
               "PB"
               "_R"
               "S"
               "T") == 0)
        return I_RST;
    if (strcmp(name,
               "SW"
               "_P"
               "W"
               "R") == 0)
        return I_PWR;

    if (strcmp(name,
               "PB"
               "_T"
               "C"
               "1") == 0)
        return I_TC1;
    if (strcmp(name,
               "PB"
               "_T"
               "C"
               "2") == 0)
        return I_TC2;
    if (strcmp(name,
               "PB"
               "_T"
               "C"
               "3") == 0)
        return I_TC3;
    if (strcmp(name,
               "PB"
               "_T"
               "C"
               "4") == 0)
        return I_TC4;
    if (strcmp(name,
               "PB"
               "_T"
               "C"
               "5") == 0)
        return I_TC5;
    if (strcmp(name,
               "PB"
               "_T"
               "C"
               "6") == 0)
        return I_TC6;
    if (strcmp(name,
               "PB"
               "_T"
               "C"
               "7") == 0)
        return I_TC7;
    if (strcmp(name,
               "PB"
               "_T"
               "C"
               "8") == 0)
        return I_TC8;
    if (strcmp(name,
               "PB"
               "_T"
               "C"
               "9") == 0)
        return I_TC9;
    if (strcmp(name,
               "PB"
               "_T"
               "C"
               "0") == 0)
        return I_TC0;

    if (strcmp(name,
               "PO"
               "_"
               "1") == 0)
        return I_POT;

    printf(
        "Error "
        "input "
        "'%s' "
        "don't "
        "have "
        "a "
        "valid "
        "id! "
        "\n",
        name);
    return INVALID_ID;
}

unsigned short cboard_PQDB::GetOutputId(char* name) {
    if (strcmp(name,
               "PB"
               "_R"
               "S"
               "T") == 0)
        return O_RST;

    if (strcmp(name,
               "DI"
               "SP"
               "1") == 0)
        return O_DISP1;
    if (strcmp(name,
               "DI"
               "SP"
               "2") == 0)
        return O_DISP2;
    if (strcmp(name,
               "DI"
               "SP"
               "3") == 0)
        return O_DISP3;
    if (strcmp(name,
               "DI"
               "SP"
               "4") == 0)
        return O_DISP4;

    if (strcmp(name,
               "KP"
               "1") == 0)
        return O_KP1;
    if (strcmp(name,
               "KP"
               "2") == 0)
        return O_KP2;

    if (strcmp(name,
               "PW"
               "M") == 0)
        return O_PWM;

    if (strcmp(name,
               "LC"
               "D_"
               "R"
               "S") == 0)
        return O_LCDRS;
    if (strcmp(name,
               "LC"
               "D_"
               "E"
               "N") == 0)
        return O_LCDEN;

    if (strcmp(name,
               "SC"
               "L") == 0)
        return O_SCL;
    if (strcmp(name,
               "SD"
               "A") == 0)
        return O_SDA;

    if (strcmp(name,
               "T"
               "X") == 0)
        return O_TX;
    if (strcmp(name,
               "R"
               "X") == 0)
        return O_RX;

    if (strcmp(name,
               "SO"
               "_D"
               "0") == 0)
        return O_SOD0;
    if (strcmp(name,
               "SO"
               "_D"
               "1") == 0)
        return O_SOD1;
    if (strcmp(name,
               "SO"
               "_D"
               "2") == 0)
        return O_SOD2;
    if (strcmp(name,
               "SO"
               "_D"
               "3") == 0)
        return O_SOD3;
    if (strcmp(name,
               "SO"
               "_D"
               "4") == 0)
        return O_SOD4;
    if (strcmp(name,
               "SO"
               "_D"
               "5") == 0)
        return O_SOD5;
    if (strcmp(name,
               "SO"
               "_D"
               "6") == 0)
        return O_SOD6;
    if (strcmp(name,
               "SO"
               "_D"
               "7") == 0)
        return O_SOD7;

    if (strcmp(name,
               "SO"
               "_D"
               "AT"
               "A") == 0)
        return O_SODATA;
    if (strcmp(name,
               "SO"
               "_C"
               "L"
               "K") == 0)
        return O_SOCLK;
    if (strcmp(name,
               "SO"
               "_E"
               "N") == 0)
        return O_SOEN;

    if (strcmp(name,
               "SS"
               "_A"
               "1") == 0)
        return O_A1;
    if (strcmp(name,
               "SS"
               "_B"
               "1") == 0)
        return O_B1;
    if (strcmp(name,
               "SS"
               "_C"
               "1") == 0)
        return O_C1;
    if (strcmp(name,
               "SS"
               "_D"
               "1") == 0)
        return O_D1;
    if (strcmp(name,
               "SS"
               "_E"
               "1") == 0)
        return O_E1;
    if (strcmp(name,
               "SS"
               "_F"
               "1") == 0)
        return O_F1;
    if (strcmp(name,
               "SS"
               "_G"
               "1") == 0)
        return O_G1;
    if (strcmp(name,
               "SS"
               "_P"
               "1") == 0)
        return O_P1;

    if (strcmp(name,
               "SS"
               "_A"
               "2") == 0)
        return O_A2;
    if (strcmp(name,
               "SS"
               "_B"
               "2") == 0)
        return O_B2;
    if (strcmp(name,
               "SS"
               "_C"
               "2") == 0)
        return O_C2;
    if (strcmp(name,
               "SS"
               "_D"
               "2") == 0)
        return O_D2;
    if (strcmp(name,
               "SS"
               "_E"
               "2") == 0)
        return O_E2;
    if (strcmp(name,
               "SS"
               "_F"
               "2") == 0)
        return O_F2;
    if (strcmp(name,
               "SS"
               "_G"
               "2") == 0)
        return O_G2;
    if (strcmp(name,
               "SS"
               "_P"
               "2") == 0)
        return O_P2;

    if (strcmp(name,
               "LD"
               "_P"
               "W"
               "R") == 0)
        return O_LPWR;
    if (strcmp(name,
               "LD"
               "_L"
               "E"
               "D") == 0)
        return O_LED;
    if (strcmp(name,
               "DS"
               "_L"
               "C"
               "D") == 0)
        return O_LCD;

    if (strcmp(name,
               "SS"
               "_A"
               "3") == 0)
        return O_A3;
    if (strcmp(name,
               "SS"
               "_B"
               "3") == 0)
        return O_B3;
    if (strcmp(name,
               "SS"
               "_C"
               "3") == 0)
        return O_C3;
    if (strcmp(name,
               "SS"
               "_D"
               "3") == 0)
        return O_D3;
    if (strcmp(name,
               "SS"
               "_E"
               "3") == 0)
        return O_E3;
    if (strcmp(name,
               "SS"
               "_F"
               "3") == 0)
        return O_F3;
    if (strcmp(name,
               "SS"
               "_G"
               "3") == 0)
        return O_G3;
    if (strcmp(name,
               "SS"
               "_P"
               "3") == 0)
        return O_P3;

    if (strcmp(name,
               "SS"
               "_A"
               "4") == 0)
        return O_A4;
    if (strcmp(name,
               "SS"
               "_B"
               "4") == 0)
        return O_B4;
    if (strcmp(name,
               "SS"
               "_C"
               "4") == 0)
        return O_C4;
    if (strcmp(name,
               "SS"
               "_D"
               "4") == 0)
        return O_D4;
    if (strcmp(name,
               "SS"
               "_E"
               "4") == 0)
        return O_E4;
    if (strcmp(name,
               "SS"
               "_F"
               "4") == 0)
        return O_F4;
    if (strcmp(name,
               "SS"
               "_G"
               "4") == 0)
        return O_G4;
    if (strcmp(name,
               "SS"
               "_P"
               "4") == 0)
        return O_P4;

    if (strcmp(name,
               "PB"
               "_T"
               "C"
               "1") == 0)
        return O_TC1;
    if (strcmp(name,
               "PB"
               "_T"
               "C"
               "2") == 0)
        return O_TC2;
    if (strcmp(name,
               "PB"
               "_T"
               "C"
               "3") == 0)
        return O_TC3;
    if (strcmp(name,
               "PB"
               "_T"
               "C"
               "4") == 0)
        return O_TC4;
    if (strcmp(name,
               "PB"
               "_T"
               "C"
               "5") == 0)
        return O_TC5;
    if (strcmp(name,
               "PB"
               "_T"
               "C"
               "6") == 0)
        return O_TC6;
    if (strcmp(name,
               "PB"
               "_T"
               "C"
               "7") == 0)
        return O_TC7;
    if (strcmp(name,
               "PB"
               "_T"
               "C"
               "8") == 0)
        return O_TC8;
    if (strcmp(name,
               "PB"
               "_T"
               "C"
               "9") == 0)
        return O_TC9;
    if (strcmp(name,
               "PB"
               "_T"
               "C"
               "0") == 0)
        return O_TC0;

    if (strcmp(name,
               "PO"
               "_"
               "1") == 0)
        return O_POT;

    if (strcmp(name,
               "RG"
               "B") == 0)
        return O_RGB;

    if (strcmp(name,
               "SS"
               "_"
               "1") == 0)
        return O_SS1;
    if (strcmp(name,
               "SS"
               "_"
               "2") == 0)
        return O_SS2;
    if (strcmp(name,
               "SS"
               "_"
               "3") == 0)
        return O_SS3;
    if (strcmp(name,
               "SS"
               "_"
               "4") == 0)
        return O_SS4;

    if (strcmp(name,
               "IC"
               "_C"
               "P"
               "U") == 0)
        return O_MP;

    printf(
        "Error "
        "output"
        " '%s' "
        "don't "
        "have "
        "a "
        "valid "
        "id! "
        "\n",
        name);
    return INVALID_ID;
}

void cboard_PQDB::RefreshStatus(void) {
    PICSimLab.UpdateStatus(PS_SERIAL, GetUARTStrStatus(0));
}

void cboard_PQDB::WritePreferences(void) {
    PICSimLab.SavePrefs(
        "PQ"
        "DB"
        "_p"
        "ro"
        "c",
        Proc);
    PICSimLab.SavePrefs(
        "PQDB_"
        "clock",
        FloatStrFormat("%2"
                       ".1"
                       "f",
                       PICSimLab.GetClock()));
    PICSimLab.SavePrefs(
        "PQDB_"
        "pot",
        std::to_string(pot));
    int position = 0;
    PICSimLab.UpdateGUI(LIGHT, GT_SCROLL, GA_GET, (void*)&position);
    PICSimLab.SavePrefs(
        "PQDB_"
        "light",
        std::to_string(position));
    PICSimLab.UpdateGUI(TEMP, GT_SCROLL, GA_GET, (void*)&position);
    PICSimLab.SavePrefs(
        "PQDB_"
        "temp",
        std::to_string(position));
}

void cboard_PQDB::ReadPreferences(char* name, char* value) {
    if (!strcmp(name,
                "PQ"
                "DB"
                "_p"
                "ro"
                "c")) {
        Proc = value;
    }

    if (!strcmp(name,
                "PQ"
                "DB"
                "_c"
                "lo"
                "c"
                "k")) {
        PICSimLab.SetClock(atof(value));
    }

    if (!strcmp(name,
                "PQ"
                "DB"
                "_p"
                "o"
                "t")) {
        pot = atoi(value);
    }

    if (!strcmp(name,
                "PQ"
                "DB"
                "_l"
                "ig"
                "h"
                "t")) {
        int ivalue = atoi(value);
        PICSimLab.UpdateGUI(LIGHT, GT_SCROLL, GA_SET, (void*)&ivalue);
    }

    if (!strcmp(name,
                "PQ"
                "DB"
                "_t"
                "em"
                "p")) {
        int ivalue = atoi(value);
        PICSimLab.UpdateGUI(TEMP, GT_SCROLL, GA_SET, (void*)&ivalue);
    }
}

std::string cboard_PQDB::MGetPinName(int pin) {
    if (pin <= bsim_picsim::MGetPinCount()) {
        std::string pname = bsim_picsim::MGetPinName(pin);

        switch (pin) {
            case 2:
                pname += "(AD0)";
                break;
            case 3:
                pname += "(AD1)";
                break;
            case 4:
                pname += "(AD2)";
                break;
            case 16:
                pname += "(D9)";
                break;
            case 19:
                pname += "(D2)";
                break;
            case 20:
                pname += "(D3)";
                break;
            case 21:
                pname += "(D4)";
                break;
            case 22:
                pname += "(D5)";
                break;
            case 27:
                pname += "(D6)";
                break;
            case 28:
                pname += "(D7)";
                break;
            case 29:
                pname += "(D8)";
                break;
            case 30:
                pname += "(D10)";
                break;
            case 33:
                pname += "(D11)";
                break;
            case 34:
                pname += "(D12)";
                break;
            case 35:
                pname += "(D13)";
                break;
            case 36:
                pname += "(SCL)";
                break;
            case 37:
                pname += "(SDA)";
                break;
        }
        return pname;
    } else {
        char pinname[100];
        snprintf(pinname, 100,
                 "d%"
                 "i",
                 pin - bsim_picsim::MGetPinCount() - 1);
        return pinname;
    }
}

int cboard_PQDB::MGetPinCount(void) {
    return bsim_picsim::MGetPinCount() + 8;
}

board_init(BOARD_PQDB_Name, cboard_PQDB);
