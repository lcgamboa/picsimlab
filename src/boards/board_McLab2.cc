/* ########################################################################

   PICSimLab - Programmable IC Simulator Laboratory

   ########################################################################

   Copyright (c) : 2010-2024  Luis Claudio Gambôa Lopes <lcgamboa@yahoo.com>

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

#include "board_McLab2.h"
#include <math.h>
#include <unistd.h>
#include "../lib/oscilloscope.h"
#include "../lib/picsimlab.h"
#include "../lib/spareparts.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

/* outputs */
enum {
    O_SS1,
    O_SS2,
    O_SS3,
    O_SS4,
    O_POT1,
    O_RB0,
    O_RB1,
    O_RB2,
    O_RB3,
    O_LPWR,
    O_LCD,
    O_A1,
    O_B1,
    O_C1,
    O_D1,
    O_E1,
    O_F1,
    O_G1,
    O_P1,
    O_A2,
    O_B2,
    O_C2,
    O_D2,
    O_E2,
    O_F2,
    O_G2,
    O_P2,
    O_A3,
    O_B3,
    O_C3,
    O_D3,
    O_E3,
    O_F3,
    O_G3,
    O_P3,
    O_A4,
    O_B4,
    O_C4,
    O_D4,
    O_E4,
    O_F4,
    O_G4,
    O_P4,
    O_JP1,
    O_JP2,
    O_JP3,
    O_JP4,
    O_JP5,
    O_JP6,
    O_RST,
    O_BRB0,
    O_BRB1,
    O_BRB2,
    O_BRB3,
    O_VT,
    O_MP
};

/*inputs*/
enum { I_POT1, I_RST, I_PWR, I_ICSP, I_JP1, I_JP2, I_JP3, I_JP4, I_JP5, I_JP6, I_RB0, I_RB1, I_RB2, I_RB3, I_VIEW };

// TODO jumper support

enum { HEATER = 2, COOLER, TEMP };

static void cboard_McLab2_callback(void* arg) {
    cboard_McLab2* McLab2 = (cboard_McLab2*)arg;
    McLab2->OnTime();
}

cboard_McLab2::cboard_McLab2(void) {
    Proc = "PIC16F877A";

    vp2in = 2.5;
    vp2[0] = 2.5;
    vp2[1] = 2.5;
    temp[0] = 27.5;
    temp[1] = 27.5;
    ref = 0;

    rpmstp = 0;  //(NSTEP*2)/100;
    rpmc = 0;

    vtc = 0;
    vt = 0;

    d = 0;
    lcde = 0;
    sound_on = 0;

    lcd_init(&lcd, 16, 2, this);
    mi2c_init(&mi2c, 4);

    ReadMaps();

    jmp[0] = 0;
    jmp[1] = 0;
    jmp[2] = 0;
    jmp[3] = 0;
    jmp[4] = 0;
    jmp[5] = 0;

    heater_pwr = 0;
    cooler_pwr = 0;

    buzzerId = PICSimLab.SystemCmd(PSC_AUDIOCHCREATE, NULL);

    pot1 = 100;

    active = 0;

    PICSimLab.UpdateGUI(HEATER, GT_GAUGE, GA_ADD, (void*)"Heater");
    PICSimLab.UpdateGUI(COOLER, GT_GAUGE, GA_ADD, (void*)"Cooler");
    PICSimLab.UpdateGUI(TEMP, GT_LABEL, GA_ADD, (void*)"Temp: 00.0C");

    vent[0] = PICSimLab.CanvasCmd(
        {.cmd = CC_LOADIMAGE, .LoadImage{(PICSimLab.GetSharePath() + "boards/Common/VT1.svg").c_str(), 1.0, 0, 0}});
    vent[1] = PICSimLab.CanvasCmd(
        {.cmd = CC_LOADIMAGE, .LoadImage{(PICSimLab.GetSharePath() + "boards/Common/VT2.svg").c_str(), 1.0, 0, 0}});

    char tname[128];
    PICSimLab.SystemCmd(PSC_GETTEMPDIR, NULL, tname);
    snprintf(mi2c_tmp_name, 200, "%s/picsimlab-XXXXXX", tname);
    close(mkstemp(mi2c_tmp_name));
    unlink(mi2c_tmp_name);
    strncat(mi2c_tmp_name, ".txt", 200);

    SWBounce_init(&bounce, 4);

    TimerID = TimerRegister_ms(100, cboard_McLab2_callback, this);
}

cboard_McLab2::~cboard_McLab2(void) {
    PICSimLab.SystemCmd(PSC_AUDIOCHBEEPSTOP, (const char*)&buzzerId);
    PICSimLab.SystemCmd(PSC_AUDIOCHDESTROY, (const char*)&buzzerId);
    mi2c_end(&mi2c);
    lcd_end(&lcd);
    PICSimLab.CanvasCmd({.cmd = CC_FREEBITMAP, .FreeBitmap{vent[0]}});
    PICSimLab.CanvasCmd({.cmd = CC_FREEBITMAP, .FreeBitmap{vent[1]}});
    vent[0] = -1;
    vent[1] = -1;

    PICSimLab.UpdateGUI(HEATER, GT_GAUGE, GA_DEL, NULL);
    PICSimLab.UpdateGUI(COOLER, GT_GAUGE, GA_DEL, NULL);
    PICSimLab.UpdateGUI(TEMP, GT_LABEL, GA_DEL, NULL);

    unlink(mi2c_tmp_name);

    SWBounce_end(&bounce);
    TimerUnregister(TimerID);
}

int cboard_McLab2::MInit(const char* processor, const char* fname, float freq) {
    char fnamem[1024];
    FILE* fout;

    PICSimLab.SystemCmd(PSC_DIRNAME, fname, fnamem);
    strncat(fnamem, "/mdump_McLab2_EEPROM.bin", 1023);

    fout = fopen_UTF8(fnamem, "rb");
    if (fout) {
        fread(mi2c.data, mi2c.SIZE, 1, fout);
        fclose(fout);
    } else {
        printf("Error loading from file: %s \n", fnamem);
    }
    return bsim_picsim::MInit(processor, fname, freq);
}

void cboard_McLab2::SetScale(double scale) {
    if (Scale != scale) {
        Scale = scale;
        if (vent[0] >= 0) {
            PICSimLab.CanvasCmd({.cmd = CC_FREEBITMAP, .FreeBitmap{vent[0]}});
            PICSimLab.CanvasCmd({.cmd = CC_FREEBITMAP, .FreeBitmap{vent[1]}});
            vent[0] = PICSimLab.CanvasCmd(
                {.cmd = CC_LOADIMAGE,
                 .LoadImage{(PICSimLab.GetSharePath() + "boards/Common/VT1.svg").c_str(), Scale, 0, 0}});
            vent[1] = PICSimLab.CanvasCmd(
                {.cmd = CC_LOADIMAGE,
                 .LoadImage{(PICSimLab.GetSharePath() + "boards/Common/VT2.svg").c_str(), Scale, 0, 0}});
        }
    }
}

int cboard_McLab2::MDumpMemory(const char* mfname) {
    FILE* fout;
    char fname[1024];

    PICSimLab.SystemCmd(PSC_DIRNAME, mfname, fname);
    strncat(fname, "/mdump_McLab2_EEPROM.bin", 1023);

    fout = fopen_UTF8(fname, "wb");
    if (fout) {
        fwrite(mi2c.data, mi2c.SIZE, 1, fout);
        fclose(fout);
    } else {
        printf("Error saving to file: %s \n", fname);
    }

    return bsim_picsim::MDumpMemory(mfname);
}

void cboard_McLab2::Draw(void) {
    int i;
    int update = 0;  // verifiy if updated is needed

    // lab3 draw
    for (i = 0; i < outputc; i++) {
        if (output[i].update)  // only if need update
        {
            output[i].update = 0;

            if (!update) {
                PICSimLab.CanvasCmd({.cmd = CC_INIT, .Init{Scale, Scale, 0}});
                PICSimLab.CanvasCmd({.cmd = CC_SETFONTWEIGHT, .SetFontWeight{CC_FONTWEIGHT_BOLD}});
            }
            update++;  // set to update buffer

            if (!output[i].r)  // rectangle
            {
                PICSimLab.CanvasCmd({.cmd = CC_SETFGCOLOR, .SetFgColor{0, 0, 0}});

                switch (output[i].id) {
                    case O_A1:
                        PICSimLab.CanvasCmd({.cmd = CC_SETBGCOLOR, .SetBgColor{lm1[18], 0, 0}});
                        break;
                    case O_B1:
                        PICSimLab.CanvasCmd({.cmd = CC_SETBGCOLOR, .SetBgColor{lm1[19], 0, 0}});
                        break;
                    case O_C1:
                        PICSimLab.CanvasCmd({.cmd = CC_SETBGCOLOR, .SetBgColor{lm1[20], 0, 0}});
                        break;
                    case O_D1:
                        PICSimLab.CanvasCmd({.cmd = CC_SETBGCOLOR, .SetBgColor{lm1[21], 0, 0}});
                        break;
                    case O_E1:
                        PICSimLab.CanvasCmd({.cmd = CC_SETBGCOLOR, .SetBgColor{lm1[26], 0, 0}});
                        break;
                    case O_F1:
                        PICSimLab.CanvasCmd({.cmd = CC_SETBGCOLOR, .SetBgColor{lm1[27], 0, 0}});
                        break;
                    case O_G1:
                        PICSimLab.CanvasCmd({.cmd = CC_SETBGCOLOR, .SetBgColor{lm1[28], 0, 0}});
                        break;

                    case O_A2:
                        PICSimLab.CanvasCmd({.cmd = CC_SETBGCOLOR, .SetBgColor{lm2[18], 0, 0}});
                        break;
                    case O_B2:
                        PICSimLab.CanvasCmd({.cmd = CC_SETBGCOLOR, .SetBgColor{lm2[19], 0, 0}});
                        break;
                    case O_C2:
                        PICSimLab.CanvasCmd({.cmd = CC_SETBGCOLOR, .SetBgColor{lm2[20], 0, 0}});
                        break;
                    case O_D2:
                        PICSimLab.CanvasCmd({.cmd = CC_SETBGCOLOR, .SetBgColor{lm2[21], 0, 0}});
                        break;
                    case O_E2:
                        PICSimLab.CanvasCmd({.cmd = CC_SETBGCOLOR, .SetBgColor{lm2[26], 0, 0}});
                        break;
                    case O_F2:
                        PICSimLab.CanvasCmd({.cmd = CC_SETBGCOLOR, .SetBgColor{lm2[27], 0, 0}});
                        break;
                    case O_G2:
                        PICSimLab.CanvasCmd({.cmd = CC_SETBGCOLOR, .SetBgColor{lm2[28], 0, 0}});
                        break;

                    case O_A3:
                        PICSimLab.CanvasCmd({.cmd = CC_SETBGCOLOR, .SetBgColor{lm3[18], 0, 0}});
                        break;
                    case O_B3:
                        PICSimLab.CanvasCmd({.cmd = CC_SETBGCOLOR, .SetBgColor{lm3[19], 0, 0}});
                        break;
                    case O_C3:
                        PICSimLab.CanvasCmd({.cmd = CC_SETBGCOLOR, .SetBgColor{lm3[20], 0, 0}});
                        break;
                    case O_D3:
                        PICSimLab.CanvasCmd({.cmd = CC_SETBGCOLOR, .SetBgColor{lm3[21], 0, 0}});
                        break;
                    case O_E3:
                        PICSimLab.CanvasCmd({.cmd = CC_SETBGCOLOR, .SetBgColor{lm3[26], 0, 0}});
                        break;
                    case O_F3:
                        PICSimLab.CanvasCmd({.cmd = CC_SETBGCOLOR, .SetBgColor{lm3[27], 0, 0}});
                        break;
                    case O_G3:
                        PICSimLab.CanvasCmd({.cmd = CC_SETBGCOLOR, .SetBgColor{lm3[28], 0, 0}});
                        break;

                    case O_A4:
                        PICSimLab.CanvasCmd({.cmd = CC_SETBGCOLOR, .SetBgColor{lm4[18], 0, 0}});
                        break;
                    case O_B4:
                        PICSimLab.CanvasCmd({.cmd = CC_SETBGCOLOR, .SetBgColor{lm4[19], 0, 0}});
                        break;
                    case O_C4:
                        PICSimLab.CanvasCmd({.cmd = CC_SETBGCOLOR, .SetBgColor{lm4[20], 0, 0}});
                        break;
                    case O_D4:
                        PICSimLab.CanvasCmd({.cmd = CC_SETBGCOLOR, .SetBgColor{lm4[21], 0, 0}});
                        break;
                    case O_E4:
                        PICSimLab.CanvasCmd({.cmd = CC_SETBGCOLOR, .SetBgColor{lm4[26], 0, 0}});
                        break;
                    case O_F4:
                        PICSimLab.CanvasCmd({.cmd = CC_SETBGCOLOR, .SetBgColor{lm4[27], 0, 0}});
                        break;
                    case O_G4:
                        PICSimLab.CanvasCmd({.cmd = CC_SETBGCOLOR, .SetBgColor{lm4[28], 0, 0}});
                        break;
                    case O_LCD:
                        PICSimLab.CanvasCmd(
                            {.cmd = CC_SETCOLOR, .SetColor{0, (unsigned int)(90 * PICSimLab.GetMcuPwr() + 40), 0}});
                        break;
                    case O_BRB0:
                    case O_BRB1:
                    case O_BRB2:
                    case O_BRB3:
                    case O_RST:
                        PICSimLab.CanvasCmd({.cmd = CC_SETCOLOR, .SetColor{100, 100, 100}});
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

                    case O_POT1:
                        PICSimLab.CanvasCmd({.cmd = CC_SETBGCOLOR, .SetBgColor{66, 109, 246}});
                        break;

                    case O_MP:
                        PICSimLab.CanvasCmd({.cmd = CC_SETCOLOR, .SetColor{26, 26, 26}});
                        break;

                    default:
                        if ((output[i].name[0] == 'J') && (output[i].name[1] == 'P'))
                            PICSimLab.CanvasCmd({.cmd = CC_SETCOLOR, .SetColor{150, 150, 150}});
                        break;
                }

                if ((output[i].id >= O_BRB0) && (output[i].id <= O_BRB3)) {
                    PICSimLab.CanvasCmd({.cmd = CC_CIRCLE, .Circle{1, output[i].cx, output[i].cy, 22}});
                    if (p_BT[output[i].id - O_BRB0]) {
                        PICSimLab.CanvasCmd({.cmd = CC_SETCOLOR, .SetColor{15, 15, 15}});
                    } else {
                        PICSimLab.CanvasCmd({.cmd = CC_SETCOLOR, .SetColor{55, 55, 55}});
                    }
                    PICSimLab.CanvasCmd({.cmd = CC_CIRCLE, .Circle{1, output[i].cx, output[i].cy, 19}});
                } else if (output[i].id == O_RST) {
                    PICSimLab.CanvasCmd({.cmd = CC_CIRCLE, .Circle{1, output[i].cx, output[i].cy, 11}});
                    if (p_RST) {
                        PICSimLab.CanvasCmd({.cmd = CC_SETCOLOR, .SetColor{15, 15, 15}});
                    } else {
                        PICSimLab.CanvasCmd({.cmd = CC_SETCOLOR, .SetColor{55, 55, 55}});
                    }
                    PICSimLab.CanvasCmd({.cmd = CC_CIRCLE, .Circle{1, output[i].cx, output[i].cy, 9}});
                } else if (output[i].id == O_POT1) {
                    PICSimLab.CanvasCmd({.cmd = CC_RECTANGLE,
                                         .Rectangle{1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1,
                                                    output[i].y2 - output[i].y1}});

                    PICSimLab.CanvasCmd({.cmd = CC_SETBGCOLOR, .SetBgColor{250, 250, 250}});
                    PICSimLab.CanvasCmd({.cmd = CC_CIRCLE, .Circle{1, output[i].cx, output[i].cy, 20}});

                    PICSimLab.CanvasCmd({.cmd = CC_SETBGCOLOR, .SetBgColor{150, 150, 150}});
                    int x = -15 * sin((5.585 * (pot1 / 200.0)) + 0.349);
                    int y = 15 * cos((5.585 * (pot1 / 200.0)) + 0.349);
                    PICSimLab.CanvasCmd({.cmd = CC_CIRCLE, .Circle{1, output[i].cx + x, output[i].cy + y, 3}});
                } else if (output[i].id == O_MP) {
                    PICSimLab.CanvasCmd({.cmd = CC_SETFONTSIZE, .SetFontSize{10}});
                    PICSimLab.CanvasCmd({.cmd = CC_RECTANGLE,
                                         .Rectangle{1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1,
                                                    output[i].y2 - output[i].y1}});
                    PICSimLab.CanvasCmd({.cmd = CC_SETCOLOR, .SetColor{230, 230, 230}});
                    PICSimLab.CanvasCmd(
                        {.cmd = CC_ROTATEDTEXT, .RotatedText{Proc.c_str(), output[i].x1 + 10, output[i].y2 - 50, 90}});
                } else if (output[i].id == O_VT) {
                    PICSimLab.CanvasCmd({.cmd = CC_CHANGESCALE, .ChangeScale{1.0, 1.0}});
                    PICSimLab.CanvasCmd(
                        {.cmd = CC_PUTBITMAP, .PutBitmap{vent[vt], output[i].x1 * Scale, output[i].y1 * Scale}});
                    PICSimLab.CanvasCmd({.cmd = CC_CHANGESCALE, .ChangeScale{Scale, Scale}});
                } else if (output[i].id == O_LCD) {
                    if (lcd.update) {
                        PICSimLab.CanvasCmd(
                            {.cmd = CC_RECTANGLE,
                             .Rectangle{1, output[i].x1 - 1, output[i].y1 - 1, output[i].x2 - output[i].x1 + 2,
                                        output[i].y2 - output[i].y1 + 3}});
                        lcd_draw(&lcd, PICSimLab.CanvasCmd, output[i].x1, output[i].y1, output[i].x2 - output[i].x1,
                                 output[i].y2 - output[i].y1, PICSimLab.GetMcuPwr());
                    }
                } else if ((output[i].name[0] == 'J') && (output[i].name[1] == 'P')) {
                    if (!jmp[output[i].name[3] - 0x31]) {
                        PICSimLab.CanvasCmd({.cmd = CC_SETCOLOR, .SetColor{70, 70, 70}});
                        PICSimLab.CanvasCmd(
                            {.cmd = CC_RECTANGLE,
                             .Rectangle{1, output[i].x1, output[i].y1, ((output[i].x2 - output[i].x1) * 0.65f),
                                        output[i].y2 - output[i].y1}});
                        PICSimLab.CanvasCmd({.cmd = CC_SETCOLOR, .SetColor{220, 220, 0}});
                        PICSimLab.CanvasCmd({.cmd = CC_CIRCLE,
                                             .Circle{1, output[i].x1 + (int)((output[i].x2 - output[i].x1) * 0.80),
                                                     output[i].y1 + ((output[i].y2 - output[i].y1) / 2), 3}});
                    } else {
                        PICSimLab.CanvasCmd({.cmd = CC_SETCOLOR, .SetColor{70, 70, 70}});
                        PICSimLab.CanvasCmd(
                            {.cmd = CC_RECTANGLE,
                             .Rectangle{1, output[i].x1 + ((int)((output[i].x2 - output[i].x1) * 0.35)), output[i].y1,
                                        ((output[i].x2 - output[i].x1) * 0.65f), output[i].y2 - output[i].y1}});
                        PICSimLab.CanvasCmd({.cmd = CC_SETCOLOR, .SetColor{220, 220, 0}});
                        PICSimLab.CanvasCmd({.cmd = CC_CIRCLE,
                                             .Circle{1, output[i].x1 + (int)((output[i].x2 - output[i].x1) * 0.20),
                                                     output[i].y1 + ((output[i].y2 - output[i].y1) / 2), 3}});
                    }
                } else {
                    PICSimLab.CanvasCmd({.cmd = CC_RECTANGLE,
                                         .Rectangle{1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1,
                                                    output[i].y2 - output[i].y1}});
                }
            } else  // circle
            {
                int led = 1;

                PICSimLab.CanvasCmd({.cmd = CC_SETFGCOLOR, .SetFgColor{55, 0, 0}});

                switch (output[i].id) {
                    case O_RB0:
                        PICSimLab.CanvasCmd(
                            {.cmd = CC_SETBGCOLOR, .SetBgColor{(unsigned int)pic.pins[32].oavalue, 0, 0}});
                        break;
                    case O_RB1:
                        PICSimLab.CanvasCmd(
                            {.cmd = CC_SETBGCOLOR, .SetBgColor{(unsigned int)pic.pins[33].oavalue, 0, 0}});
                        break;
                    case O_RB2:
                        PICSimLab.CanvasCmd(
                            {.cmd = CC_SETBGCOLOR, .SetBgColor{(unsigned int)pic.pins[34].oavalue, 0, 0}});
                        break;
                    case O_RB3:
                        PICSimLab.CanvasCmd(
                            {.cmd = CC_SETBGCOLOR, .SetBgColor{(unsigned int)pic.pins[35].oavalue, 0, 0}});
                        break;
                    case O_LPWR:
                        PICSimLab.CanvasCmd({.cmd = CC_SETBGCOLOR,
                                             .SetBgColor{0, (unsigned int)(200 * PICSimLab.GetMcuPwr() + 55), 0}});
                        break;
                    case O_P1:
                        led = 0;
                        PICSimLab.CanvasCmd({.cmd = CC_SETBGCOLOR, .SetBgColor{lm1[29], 0, 0}});
                        break;
                    case O_P2:
                        led = 0;
                        PICSimLab.CanvasCmd({.cmd = CC_SETBGCOLOR, .SetBgColor{lm2[29], 0, 0}});
                        break;
                    case O_P3:
                        led = 0;
                        PICSimLab.CanvasCmd({.cmd = CC_SETBGCOLOR, .SetBgColor{lm3[29], 0, 0}});
                        break;
                    case O_P4:
                        led = 0;
                        PICSimLab.CanvasCmd({.cmd = CC_SETBGCOLOR, .SetBgColor{lm4[29], 0, 0}});
                        break;
                }

                if (led) {
                    DrawLED(PICSimLab.CanvasCmd, &output[i]);
                } else {
                    PICSimLab.CanvasCmd({.cmd = CC_CIRCLE, .Circle{1, output[i].x1, output[i].y1, output[i].r}});
                }
            }
        }
    }
    // end draw

    if (update) {
        PICSimLab.CanvasCmd({.cmd = CC_END});
    }

    if ((((pic.pins[6].oavalue - 55) / 2) > 40) && PICSimLab.GetMcuPwr()) {
        if (!sound_on) {
            PICSimLab.SystemCmd(PSC_AUDIOCHBEEPSTART, (const char*)&buzzerId);
            sound_on = 1;
        }
    } else {
        PICSimLab.SystemCmd(PSC_AUDIOCHBEEPSTOP, (const char*)&buzzerId);
        sound_on = 0;
    }

    // Cooler
    cooler_pwr = pic.pins[15].oavalue - 55;
    int value = cooler_pwr / 2;
    PICSimLab.UpdateGUI(COOLER, GT_GAUGE, GA_SET, (void*)&value);

    // Heater
    heater_pwr = pic.pins[16].oavalue - 55;
    value = heater_pwr / 2;
    PICSimLab.UpdateGUI(HEATER, GT_GAUGE, GA_SET, (void*)&value);

    // potentiometer
    vp2in = (5.0 * pot1 / 199);

    // referencia
    pic_set_apin(&pic, 5, 2.5);
}

void cboard_McLab2::OnTime(void) {
    if (cooler_pwr > 40) {
        vtc++;
    }
    if (vtc > (4 - 0.04 * cooler_pwr)) {
        vtc = 0;
        vt ^= 1;
        output_ids[O_VT]->update = 1;
    }

    // thacometer
    rpmstp = 2000.0 / (0.7196 * (cooler_pwr + 1));

    // temp
    ref = (0.25 * heater_pwr) - (0.25 * cooler_pwr);

    temp[1] = temp[0];
    temp[0] = ((27.5 + ref) * 0.003) + temp[1] * (0.997);

    if (temp[0] < 27.5) {
        temp[0] = 27.5;
    }

    pic_set_apin(&pic, 2, (10.0 / 255.0) * (temp[0] + 15.0));
}

void cboard_McLab2::Run_CPU(void) {
    int i;
    int j;
    unsigned char pi, pj;
    unsigned char pinv;
    const picpin* pins;
    int bret;

    unsigned int alm[40];   // luminosidade media
    unsigned int alm1[40];  // luminosidade media display
    unsigned int alm2[40];  // luminosidade media display
    unsigned int alm3[40];  // luminosidade media display
    unsigned int alm4[40];  // luminosidade media display

    const int JUMPSTEPS = PICSimLab.GetJUMPSTEPS();
    const long int NSTEPJ = PICSimLab.GetNSTEPJ();
    const long int NSTEP = PICSimLab.GetNSTEP();
    const float RNSTEP = 200.0 * pic.PINCOUNT / NSTEP;

    memset(alm, 0, 40 * sizeof(unsigned int));
    memset(alm1, 0, 40 * sizeof(unsigned int));
    memset(alm2, 0, 40 * sizeof(unsigned int));
    memset(alm3, 0, 40 * sizeof(unsigned int));
    memset(alm4, 0, 40 * sizeof(unsigned int));

    pins = pic.pins;

    if (use_spare)
        SpareParts.PreProcess();

    unsigned char p_BT_[4];
    memcpy(p_BT_, p_BT, 4);

    SWBounce_prepare(&bounce, PICSimLab.GetBoard()->MGetInstClockFreq());
    for (int pl = 0; pl < 4; pl++) {
        if ((pins[33 + pl - 1].dir == PD_IN) && (pins[33 + pl - 1].value != p_BT_[pl])) {
            SWBounce_bounce(&bounce, pl);
        }
    }

    j = JUMPSTEPS;
    pi = 0;
    if (PICSimLab.GetMcuPwr())
        for (i = 0; i < NSTEP; i++) {
            if (j >= JUMPSTEPS) {
                pic_set_pin(&pic, pic.mclr, p_RST);

                if (!bounce.do_bounce) {
                    pic_set_pin(&pic, 33, p_BT_[0]);
                    pic_set_pin(&pic, 34, p_BT_[1]);
                    pic_set_pin(&pic, 35, p_BT_[2]);
                    pic_set_pin(&pic, 36, p_BT_[3]);
                }

                rpmc++;
                if (rpmc > rpmstp) {
                    rpmc = 0;
                    pic_set_pin(&pic, 15, !pic_get_pin(&pic, 15));
                }
            }

            if (bounce.do_bounce) {
                bret = SWBounce_process(&bounce);
                if (bret) {
                    for (int pl = 0; pl < 4; pl++) {
                        if (bounce.bounce[pl]) {
                            if (bret == 1) {
                                pic_set_pin(&pic, 33 + pl, !pins[33 + pl - 1].value);
                            } else {
                                pic_set_pin(&pic, 33 + pl, p_BT_[pl]);
                            }
                        }
                    }
                }
            }

            if (!mplabxd_testbp())
                pic_step(&pic);
            ioupdated = pic.ioupdated;
            InstCounterInc();

            if (ioupdated) {
                if (!bounce.do_bounce) {
                    pic_set_pin(&pic, 33, p_BT_[0]);
                    pic_set_pin(&pic, 34, p_BT_[1]);
                    pic_set_pin(&pic, 35, p_BT_[2]);
                    pic_set_pin(&pic, 36, p_BT_[3]);
                }
            }

            if (use_oscope)
                Oscilloscope.SetSample();
            if (use_spare)
                SpareParts.Process();

            // increment mean value counter if pin is high
            alm[pi] += pins[pi].value;
            pi++;
            if (pi == pic.PINCOUNT)
                pi = 0;

            if (j >= JUMPSTEPS) {
                for (pj = 18; pj < 30; pj++) {
                    pinv = pins[pj].value;
                    if ((pinv) && (pins[39].value))
                        alm1[pj]++;
                    if ((pinv) && (pins[38].value))
                        alm2[pj]++;
                    if ((pinv) && (pins[37].value))
                        alm3[pj]++;
                    if ((pinv) && (pins[36].value))
                        alm4[pj]++;
                }

                j = -1;
            }
            j++;

            // potênciometro p2
            // p2 rc circuit

            if (!pins[2].dir) {
                // decarga por RA1
                vp2[1] = vp2[0] = 5 * pins[2].value;
            }

            vp2[1] = vp2[0];

            vp2[0] = vp2in * 0.00021 + vp2[1] * 0.99979;

            if (pins[2].ptype < 3)
                pic_set_pin(&pic, 3, vp2[0] > 1.25);
            else
                pic_set_apin(&pic, 3, vp2[0]);

            if (ioupdated) {
                // lcd dipins[2].dirsplay code
                if ((!pins[8].dir) && (!pins[8].value)) {
                    if (!lcde) {
                        d = 0;
                        if (pins[29].value)
                            d |= 0x80;
                        if (pins[28].value)
                            d |= 0x40;
                        if (pins[27].value)
                            d |= 0x20;
                        if (pins[26].value)
                            d |= 0x10;
                        if (pins[21].value)
                            d |= 0x08;
                        if (pins[20].value)
                            d |= 0x04;
                        if (pins[19].value)
                            d |= 0x02;
                        if (pins[18].value)
                            d |= 0x01;

                        if ((!pins[7].dir) && (!pins[7].value)) {
                            lcd_cmd(&lcd, d);
                        } else if ((!pins[7].dir) && (pins[7].value)) {
                            lcd_data(&lcd, d);
                        }
                        lcde = 1;
                    }

                } else {
                    lcde = 0;
                }

                // i2c code
                if (pins[22].dir) {
                    sda = 1;
                } else {
                    sda = pins[22].value;
                }

                if (pins[17].dir) {
                    sck = 1;
                    pic_set_pin(&pic, 18, 1);
                } else {
                    sck = pins[17].value;
                }
                pic_set_pin(&pic, 23, mi2c_io(&mi2c, sck, sda));
            }
            pic.ioupdated = 0;
        }
    // fim STEP

    for (pi = 0; pi < pic.PINCOUNT; pi++) {
        if (pic.pins[pi].port == P_VDD)
            pic.pins[pi].oavalue = 255;
        else
            pic.pins[pi].oavalue = (int)((alm[pi] * RNSTEP) + 55);

        lm1[pi] = (int)(((600.0 * alm1[pi]) / NSTEPJ) + 30);
        lm2[pi] = (int)(((600.0 * alm2[pi]) / NSTEPJ) + 30);
        lm3[pi] = (int)(((600.0 * alm3[pi]) / NSTEPJ) + 30);
        lm4[pi] = (int)(((600.0 * alm4[pi]) / NSTEPJ) + 30);
        if (lm1[pi] > 255)
            lm1[pi] = 255;
        if (lm2[pi] > 255)
            lm2[pi] = 255;
        if (lm3[pi] > 255)
            lm3[pi] = 255;
        if (lm4[pi] > 255)
            lm4[pi] = 255;
    }

    if (use_spare)
        SpareParts.PostProcess();

    if (lcd.update)
        output_ids[O_LCD]->update = 1;

    // verifiy if LEDS need update
    for (i = 0; i < 4; i++) {
        // PORTB LEDS
        if (output_ids[O_RB0 + i]->value != pic.pins[32 + i].oavalue) {
            output_ids[O_RB0 + i]->value = pic.pins[32 + i].oavalue;
            output_ids[O_RB0 + i]->update = 1;
        }
    }

    for (i = 0; i < 8; i++) {
        if (i < 4) {
            j = i;
        } else {
            j = i + 4;
        }
        // 7s DISP
        if (output_ids[O_A1 + i]->value != lm1[18 + j]) {
            output_ids[O_A1 + i]->value = lm1[18 + j];
            output_ids[O_SS1]->update = 1;
        }
        if (output_ids[O_A2 + i]->value != lm2[18 + j]) {
            output_ids[O_A2 + i]->value = lm2[18 + j];
            output_ids[O_SS2]->update = 1;
        }
        if (output_ids[O_A3 + i]->value != lm3[18 + j]) {
            output_ids[O_A3 + i]->value = lm3[18 + j];
            output_ids[O_SS3]->update = 1;
        }
        if (output_ids[O_A4 + i]->value != lm4[18 + j]) {
            output_ids[O_A4 + i]->value = lm4[18 + j];
            output_ids[O_SS4]->update = 1;
        }
    }
}

void cboard_McLab2::Reset(void) {
    pic_reset(&pic, 1);
    lcd_rst(&lcd);
    mi2c_rst(&mi2c);

    p_BT[0] = 1;
    p_BT[1] = 1;
    p_BT[2] = 1;
    p_BT[3] = 1;

    pic_set_pin(&pic, 33, p_BT[0]);
    pic_set_pin(&pic, 34, p_BT[1]);
    pic_set_pin(&pic, 35, p_BT[2]);
    pic_set_pin(&pic, 36, p_BT[3]);

    PICSimLab.UpdateStatus(PS_SERIAL, GetUARTStrStatus(0));

    for (int pi = 0; pi < pic.PINCOUNT; pi++) {
        lm1[pi] = 30;
        lm2[pi] = 30;
        lm3[pi] = 30;
        lm4[pi] = 30;
    }
    if (use_spare)
        SpareParts.Reset();

    RegisterRemoteControl();
}

void cboard_McLab2::RegisterRemoteControl(void) {
    input_ids[I_RB0]->status = &p_BT[0];
    input_ids[I_RB0]->update = &output_ids[O_BRB0]->update;
    input_ids[I_RB1]->status = &p_BT[1];
    input_ids[I_RB1]->update = &output_ids[O_BRB1]->update;
    input_ids[I_RB2]->status = &p_BT[2];
    input_ids[I_RB2]->update = &output_ids[O_BRB2]->update;
    input_ids[I_RB3]->status = &p_BT[3];
    input_ids[I_RB3]->update = &output_ids[O_BRB3]->update;
    input_ids[I_POT1]->status = &pot1;
    input_ids[I_POT1]->update = &output_ids[O_POT1]->update;

    output_ids[O_RB0]->status = &pic.pins[32].oavalue;
    output_ids[O_RB1]->status = &pic.pins[33].oavalue;
    output_ids[O_RB2]->status = &pic.pins[34].oavalue;
    output_ids[O_RB3]->status = &pic.pins[35].oavalue;
    output_ids[O_LCD]->status = &lcd;
    output_ids[O_A1]->status = &lm1[18];
    output_ids[O_B1]->status = &lm1[19];
    output_ids[O_C1]->status = &lm1[20];
    output_ids[O_D1]->status = &lm1[21];
    output_ids[O_E1]->status = &lm1[26];
    output_ids[O_F1]->status = &lm1[27];
    output_ids[O_G1]->status = &lm1[28];
    output_ids[O_P1]->status = &lm1[29];
    output_ids[O_A2]->status = &lm2[18];
    output_ids[O_B2]->status = &lm2[19];
    output_ids[O_C2]->status = &lm2[20];
    output_ids[O_D2]->status = &lm2[21];
    output_ids[O_E2]->status = &lm2[26];
    output_ids[O_F2]->status = &lm2[27];
    output_ids[O_G2]->status = &lm2[28];
    output_ids[O_P2]->status = &lm2[29];
    output_ids[O_A3]->status = &lm3[18];
    output_ids[O_B3]->status = &lm3[19];
    output_ids[O_C3]->status = &lm3[20];
    output_ids[O_D3]->status = &lm3[21];
    output_ids[O_E3]->status = &lm3[26];
    output_ids[O_F3]->status = &lm3[27];
    output_ids[O_G3]->status = &lm3[28];
    output_ids[O_P3]->status = &lm3[29];
    output_ids[O_A4]->status = &lm4[18];
    output_ids[O_B4]->status = &lm4[19];
    output_ids[O_C4]->status = &lm4[20];
    output_ids[O_D4]->status = &lm4[21];
    output_ids[O_E4]->status = &lm4[26];
    output_ids[O_F4]->status = &lm4[27];
    output_ids[O_G4]->status = &lm4[28];
    output_ids[O_P4]->status = &lm4[29];
}

void cboard_McLab2::EvMouseButtonPress(unsigned int button, unsigned int x, unsigned int y, unsigned int state) {
    int i;

    for (i = 0; i < inputc; i++) {
        if (((input[i].x1 <= x) && (input[i].x2 >= x)) && ((input[i].y1 <= y) && (input[i].y2 >= y))) {
            switch (input[i].id) {
                case I_ICSP: {
                    PICSimLab.OpenLoadHexFileDialog();
                    ;
                } break;

                case I_JP1: {
                    jmp[0] ^= 0x01;
                    output_ids[O_JP1]->update = 1;
                } break;
                case I_JP2: {
                    jmp[1] ^= 0x01;
                    output_ids[O_JP2]->update = 1;
                } break;
                case I_JP3: {
                    jmp[2] ^= 0x01;
                    output_ids[O_JP3]->update = 1;
                } break;
                case I_JP4: {
                    jmp[3] ^= 0x01;
                    output_ids[O_JP4]->update = 1;
                } break;
                case I_JP5: {
                    jmp[4] ^= 0x01;
                    output_ids[O_JP5]->update = 1;
                } break;
                case I_JP6: {
                    jmp[5] ^= 0x01;
                    output_ids[O_JP6]->update = 1;
                } break;

                case I_PWR: {
                    if (PICSimLab.GetMcuPwr()) {
                        PICSimLab.SetMcuPwr(0);
                        Reset();
                        p_BT[0] = 0;
                        p_BT[1] = 0;
                        p_BT[2] = 0;
                        p_BT[3] = 0;
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

                case I_RB0: {
                    p_BT[0] = 0;
                    output_ids[O_BRB0]->update = 1;
                } break;
                case I_RB1: {
                    p_BT[1] = 0;
                    output_ids[O_BRB1]->update = 1;
                } break;
                case I_RB2: {
                    p_BT[2] = 0;
                    output_ids[O_BRB2]->update = 1;
                } break;
                case I_RB3: {
                    p_BT[3] = 0;
                    output_ids[O_BRB3]->update = 1;
                } break;
                case I_POT1: {
                    active = 1;
                    pot1 = CalcAngle(i, x, y);
                    output_ids[O_POT1]->update = 1;
                } break;
                case I_VIEW:
                    FILE* fout;
                    fout = fopen_UTF8(mi2c_tmp_name, "w");
                    if (fout) {
                        for (unsigned int i = 0; i < mi2c.SIZE; i += 16) {
                            fprintf(fout, "%04X: ", i);
                            for (int j = 0; j < 16; j++) {
                                fprintf(fout, "%02X ", mi2c.data[j + i]);
                            }
                            fprintf(fout, "\r\n");
                        }
                        fclose(fout);
#ifdef __EMSCRIPTEN__
                        EM_ASM_(
                            {
                                var filename = UTF8ToString($0);
                                var buf = FS.readFile(filename);
                                var blob = new Blob([buf], { "type" : "application/octet-stream" });
                                var text = URL.createObjectURL(blob);

                                var element = document.createElement('a');
                                element.setAttribute('href', text);
                                element.setAttribute('download', filename);

                                element.style.display = 'none';
                                document.body.appendChild(element);

                                element.click();

                                document.body.removeChild(element);
                                URL.revokeObjectURL(text);
                            },
                            mi2c_tmp_name);
#else
                        PICSimLab.SystemCmd(PSC_LAUNCHDEFAULAPPLICATION, mi2c_tmp_name);
#endif
                    } else {
                        printf("Error saving to file: %s \n", mi2c_tmp_name);
                    }
                    break;
            }
        }
    }
}

void cboard_McLab2::EvMouseMove(unsigned int button, unsigned int x, unsigned int y, unsigned int state) {
    int i;

    for (i = 0; i < inputc; i++) {
        switch (input[i].id) {
            case I_POT1:
                if (((input[i].x1 <= x) && (input[i].x2 >= x)) && ((input[i].y1 <= y) && (input[i].y2 >= y))) {
                    if (active) {
                        pot1 = CalcAngle(i, x, y);
                        output_ids[O_POT1]->update = 1;
                    }
                } else {
                    active = 0;
                }
                break;
        }
    }
}

void cboard_McLab2::EvMouseButtonRelease(unsigned int button, unsigned int x, unsigned int y, unsigned int state) {
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

                case I_RB0: {
                    p_BT[0] = 1;
                    output_ids[O_BRB0]->update = 1;
                } break;
                case I_RB1: {
                    p_BT[1] = 1;
                    output_ids[O_BRB1]->update = 1;
                } break;
                case I_RB2: {
                    p_BT[2] = 1;
                    output_ids[O_BRB2]->update = 1;
                } break;
                case I_RB3: {
                    p_BT[3] = 1;
                    output_ids[O_BRB3]->update = 1;
                } break;
                case I_POT1: {
                    active = 0;
                    output_ids[O_POT1]->update = 1;
                } break;
            }
        }
    }
}

void cboard_McLab2::EvKeyPress(unsigned int key, unsigned int mask) {
    if (key == '1') {
        p_BT[0] = 0;
        output_ids[O_BRB0]->update = 1;
    }
    if (key == '2') {
        p_BT[1] = 0;
        output_ids[O_BRB1]->update = 1;
    }
    if (key == '3') {
        p_BT[2] = 0;
        output_ids[O_BRB2]->update = 1;
    }
    if (key == '4') {
        p_BT[3] = 0;
        output_ids[O_BRB3]->update = 1;
    }
}

void cboard_McLab2::EvKeyRelease(unsigned int key, unsigned int mask) {
    if (key == '1') {
        p_BT[0] = 1;
        output_ids[O_BRB0]->update = 1;
    }

    if (key == '2') {
        p_BT[1] = 1;
        output_ids[O_BRB1]->update = 1;
    }

    if (key == '3') {
        p_BT[2] = 1;
        output_ids[O_BRB2]->update = 1;
    }

    if (key == '4') {
        p_BT[3] = 1;
        output_ids[O_BRB3]->update = 1;
    }
}

void cboard_McLab2::EvOnShow(void) {
    lcd.update = 1;
    board::EvOnShow();
}

unsigned short cboard_McLab2::GetInputId(char* name) {
    if (strcmp(name, "PB_RST") == 0)
        return I_RST;

    if (strcmp(name, "SW_PWR") == 0)
        return I_PWR;
    if (strcmp(name, "PG_ICSP") == 0)
        return I_ICSP;

    if (strcmp(name, "PB_RB0") == 0)
        return I_RB0;
    if (strcmp(name, "PB_RB1") == 0)
        return I_RB1;
    if (strcmp(name, "PB_RB2") == 0)
        return I_RB2;
    if (strcmp(name, "PB_RB3") == 0)
        return I_RB3;

    if (strcmp(name, "JP_1") == 0)
        return I_JP1;
    if (strcmp(name, "JP_2") == 0)
        return I_JP2;
    if (strcmp(name, "JP_3") == 0)
        return I_JP3;
    if (strcmp(name, "JP_4") == 0)
        return I_JP4;
    if (strcmp(name, "JP_5") == 0)
        return I_JP5;
    if (strcmp(name, "JP_6") == 0)
        return I_JP6;

    if (strcmp(name, "MD_VIEW") == 0)
        return I_VIEW;

    if (strcmp(name, "PO_2") == 0)
        return I_POT1;

    printf("Error input '%s' don't have a valid id! \n", name);
    return INVALID_ID;
}

unsigned short cboard_McLab2::GetOutputId(char* name) {
    if (strcmp(name, "LD_RB0") == 0)
        return O_RB0;
    if (strcmp(name, "LD_RB1") == 0)
        return O_RB1;
    if (strcmp(name, "LD_RB2") == 0)
        return O_RB2;
    if (strcmp(name, "LD_RB3") == 0)
        return O_RB3;

    if (strcmp(name, "SS_1") == 0)
        return O_SS1;
    if (strcmp(name, "SS_2") == 0)
        return O_SS2;
    if (strcmp(name, "SS_3") == 0)
        return O_SS3;
    if (strcmp(name, "SS_4") == 0)
        return O_SS4;

    if (strcmp(name, "SS_A1") == 0)
        return O_A1;
    if (strcmp(name, "SS_B1") == 0)
        return O_B1;
    if (strcmp(name, "SS_C1") == 0)
        return O_C1;
    if (strcmp(name, "SS_D1") == 0)
        return O_D1;
    if (strcmp(name, "SS_E1") == 0)
        return O_E1;
    if (strcmp(name, "SS_F1") == 0)
        return O_F1;
    if (strcmp(name, "SS_G1") == 0)
        return O_G1;
    if (strcmp(name, "SS_P1") == 0)
        return O_P1;

    if (strcmp(name, "SS_A2") == 0)
        return O_A2;
    if (strcmp(name, "SS_B2") == 0)
        return O_B2;
    if (strcmp(name, "SS_C2") == 0)
        return O_C2;
    if (strcmp(name, "SS_D2") == 0)
        return O_D2;
    if (strcmp(name, "SS_E2") == 0)
        return O_E2;
    if (strcmp(name, "SS_F2") == 0)
        return O_F2;
    if (strcmp(name, "SS_G2") == 0)
        return O_G2;
    if (strcmp(name, "SS_P2") == 0)
        return O_P2;

    if (strcmp(name, "LD_LPWR") == 0)
        return O_LPWR;
    if (strcmp(name, "DS_LCD") == 0)
        return O_LCD;

    if (strcmp(name, "SS_A3") == 0)
        return O_A3;
    if (strcmp(name, "SS_B3") == 0)
        return O_B3;
    if (strcmp(name, "SS_C3") == 0)
        return O_C3;
    if (strcmp(name, "SS_D3") == 0)
        return O_D3;
    if (strcmp(name, "SS_E3") == 0)
        return O_E3;
    if (strcmp(name, "SS_F3") == 0)
        return O_F3;
    if (strcmp(name, "SS_G3") == 0)
        return O_G3;
    if (strcmp(name, "SS_P3") == 0)
        return O_P3;

    if (strcmp(name, "SS_A4") == 0)
        return O_A4;
    if (strcmp(name, "SS_B4") == 0)
        return O_B4;
    if (strcmp(name, "SS_C4") == 0)
        return O_C4;
    if (strcmp(name, "SS_D4") == 0)
        return O_D4;
    if (strcmp(name, "SS_E4") == 0)
        return O_E4;
    if (strcmp(name, "SS_F4") == 0)
        return O_F4;
    if (strcmp(name, "SS_G4") == 0)
        return O_G4;
    if (strcmp(name, "SS_P4") == 0)
        return O_P4;

    if (strcmp(name, "JP_1") == 0)
        return O_JP1;
    if (strcmp(name, "JP_2") == 0)
        return O_JP2;
    if (strcmp(name, "JP_3") == 0)
        return O_JP3;
    if (strcmp(name, "JP_4") == 0)
        return O_JP4;
    if (strcmp(name, "JP_5") == 0)
        return O_JP5;
    if (strcmp(name, "JP_6") == 0)
        return O_JP6;

    if (strcmp(name, "PB_RB0") == 0)
        return O_BRB0;
    if (strcmp(name, "PB_RB1") == 0)
        return O_BRB1;
    if (strcmp(name, "PB_RB2") == 0)
        return O_BRB2;
    if (strcmp(name, "PB_RB3") == 0)
        return O_BRB3;

    if (strcmp(name, "PO_2") == 0)
        return O_POT1;

    if (strcmp(name, "PB_RST") == 0)
        return O_RST;

    if (strcmp(name, "MC_VT") == 0)
        return O_VT;

    if (strcmp(name, "IC_CPU") == 0)
        return O_MP;

    printf("Error output '%s' don't have a valid id! \n", name);
    return INVALID_ID;
}

void cboard_McLab2::RefreshStatus(void) {
    char svalue[128];
    snprintf(svalue, 128, "Temp: %5.2fC", temp[0]);
    PICSimLab.UpdateGUI(TEMP, GT_LABEL, GA_SET_LABEL, (void*)svalue);

    PICSimLab.UpdateStatus(PS_SERIAL, GetUARTStrStatus(0));
}

void cboard_McLab2::WritePreferences(void) {
    char line[100];
    PICSimLab.SavePrefs("McLab2_proc", Proc);

    line[0] = 0;
    for (int i = 0; i < 6; i++)
        sprintf(line + i, "%i", jmp[i]);

    PICSimLab.SavePrefs("McLab2_jmp", line);
    PICSimLab.SavePrefs("McLab2_clock", FloatStrFormat("%2.1f", PICSimLab.GetClock()));
    PICSimLab.SavePrefs("McLab2_pot1", std::to_string(pot1));
}

void cboard_McLab2::ReadPreferences(char* name, char* value) {
    if (!strcmp(name, "McLab2_proc")) {
        Proc = value;
    }

    int i;
    if (!strcmp(name, "McLab2_jmp")) {
        for (i = 0; i < 6; i++) {
            if (value[i] == '0')
                jmp[i] = 0;
            else
                jmp[i] = 1;
        }
    }

    if (!strcmp(name, "McLab2_clock")) {
        PICSimLab.SetClock(atof(value));
    }

    if (!strcmp(name, "McLab2_pot1")) {
        pot1 = atoi(value);
    }
}

board_init(BOARD_McLab2_Name, cboard_McLab2);
