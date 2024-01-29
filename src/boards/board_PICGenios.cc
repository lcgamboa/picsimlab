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

#include "board_PICGenios.h"
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
    O_RST,
    O_BRB0,
    O_BRB1,
    O_BRB2,
    O_BRB3,
    O_BRB4,
    O_BRB5,
    O_BRA5,
    O_TC1,
    O_TC2,
    O_TC3,
    O_TC4,
    O_TC5,
    O_TC6,
    O_TC7,
    O_TC8,
    O_TC9,
    O_TCA,
    O_TC0,
    O_TCT,
    O_POT1,
    O_POT2,
    O_RB0,
    O_RB1,
    O_RB2,
    O_RB3,
    O_RB4,
    O_RB5,
    O_RB6,
    O_RB7,
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
    O_RD0,
    O_RD1,
    O_RD2,
    O_RD3,
    O_RD4,
    O_RD5,
    O_RD6,
    O_RD7,
    O_RL1,
    O_RL2,
    O_D01,
    O_D02,
    O_D03,
    O_D04,
    O_D05,
    O_D06,
    O_D07,
    O_D08,
    O_D09,
    O_D10,
    O_D11,
    O_D12,
    O_D13,
    O_D14,
    O_D15,
    O_D16,
    O_D17,
    O_D18,
    O_D19,
    O_D20,
    O_VT,
    O_PRG,
    O_RUN,
    O_JP1,
    O_MP
};

/*inputs*/

enum {
    I_RB0,
    I_RB1,
    I_RB2,
    I_RB3,
    I_RB4,
    I_RB5,
    I_RA5,
    I_TC1,
    I_TC2,
    I_TC3,
    I_TC4,
    I_TC5,
    I_TC6,
    I_TC7,
    I_TC8,
    I_TC9,
    I_TCA,
    I_TC0,
    I_TCT,
    I_POT1,
    I_POT2,
    I_RST,
    I_PWR,
    I_ICSP,
    I_D01,
    I_D02,
    I_D03,
    I_D04,
    I_D05,
    I_D06,
    I_D07,
    I_D08,
    I_D09,
    I_D10,
    I_D11,
    I_D12,
    I_D13,
    I_D14,
    I_D15,
    I_D16,
    I_D17,
    I_D18,
    I_D19,
    I_D20,
    I_JP1,
    I_VIEW
};

// TODO TEMP cooler must don't work with AQUE=0

enum { LCD_TYPE = 0, HEATER = 3, COOLER, TEMP };

static void cboard_PICGenios_callback(void* arg) {
    cboard_PICGenios* PICGenios = (cboard_PICGenios*)arg;
    PICGenios->OnTime();
}

cboard_PICGenios::cboard_PICGenios(void) {
    Proc = "PIC18F4520";

    vtc = 0;
    vt = 0;

    pot[0] = 100;
    pot[1] = 100;

    active[0] = 0;
    active[1] = 0;

    vp1in = 2.5;
    vp2in = 2.5;
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

    heater_pwr = 0;
    cooler_pwr = 0;

    vent[0] = PICSimLab.LoadImageFile(PICSimLab.GetSharePath() + "boards/Common/VT1.svg");
    vent[1] = PICSimLab.LoadImageFile(PICSimLab.GetSharePath() + "boards/Common/VT2.svg");
    lcdbmp[0] = PICSimLab.LoadImageFile(PICSimLab.GetSharePath() + "boards/Common/lcd2.svg");
    lcdbmp[1] = PICSimLab.LoadImageFile(PICSimLab.GetSharePath() + "boards/Common/lcd4.svg");

    lcd_init(&lcd, 16, 2, this);
    mi2c_init(&mi2c, 4);
    rtc_ds1307_init(&rtc2, this);

    ReadMaps();

    jmp[0] = 0;

    for (int i = 0; i < 20; i++)
        dip[i] = 1;

    buzzer.Init();

    PICSimLab.UpdateGUI(LCD_TYPE, GT_COMBO, GA_ADD, (void*)"LCD");
    PICSimLab.UpdateGUI(HEATER, GT_GAUGE, GA_ADD, (void*)"Heater");
    PICSimLab.UpdateGUI(COOLER, GT_GAUGE, GA_ADD, (void*)"Cooler");
    PICSimLab.UpdateGUI(TEMP, GT_LABEL, GA_ADD, (void*)"Temp: 00.0C");

    PICSimLab.UpdateGUI(LCD_TYPE, GT_COMBO, GA_SET, (void*)",hd44780 16x2,hd44780 16x4,");
    PICSimLab.UpdateGUI(LCD_TYPE, GT_COMBO, GA_SET, (void*)"hd44780 16x2");

    snprintf(mi2c_tmp_name, 200, "%s/picsimlab-XXXXXX", (const char*)lxGetTempDir("PICSimLab").c_str());
    close(mkstemp(mi2c_tmp_name));
    unlink(mi2c_tmp_name);
    strncat(mi2c_tmp_name, ".txt", 200);

    SWBounce_init(&bounce, 7);

    TimerID = TimerRegister_ms(100, cboard_PICGenios_callback, this);
}

cboard_PICGenios::~cboard_PICGenios(void) {
    buzzer.BeepStop();
    buzzer.End();
    delete vent[0];
    delete vent[1];
    vent[0] = NULL;
    vent[1] = NULL;

    delete lcdbmp[0];
    delete lcdbmp[1];
    lcdbmp[0] = NULL;
    lcdbmp[1] = NULL;

    mi2c_end(&mi2c);
    rtc_ds1307_end(&rtc2);
    lcd_end(&lcd);

    PICSimLab.UpdateGUI(LCD_TYPE, GT_COMBO, GA_DEL, NULL);
    PICSimLab.UpdateGUI(HEATER, GT_GAUGE, GA_DEL, NULL);
    PICSimLab.UpdateGUI(COOLER, GT_GAUGE, GA_DEL, NULL);
    PICSimLab.UpdateGUI(TEMP, GT_LABEL, GA_DEL, NULL);

    unlink(mi2c_tmp_name);

    SWBounce_end(&bounce);
    TimerUnregister(TimerID);
}

int cboard_PICGenios::MInit(const char* processor, const char* fname, float freq) {
    char fnamem[1024];
    FILE* fout;

    strncpy(fnamem, (const char*)dirname(fname).c_str(), 1023);
    strncat(fnamem, "/mdump_PICGenios_EEPROM.bin", 1023);

    fout = fopen_UTF8(fnamem, "rb");
    if (fout) {
        fread(mi2c.data, mi2c.SIZE, 1, fout);
        fclose(fout);
    } else {
        printf("Error loading from file: %s \n", fnamem);
    }
    return bsim_picsim::MInit(processor, fname, freq);
}

void cboard_PICGenios::SetScale(double scale) {
    if (Scale != scale) {
        Scale = scale;
        if (vent[0]) {
            delete vent[0];
            delete vent[1];
            delete lcdbmp[0];
            delete lcdbmp[1];

            vent[0] = PICSimLab.LoadImageFile(PICSimLab.GetSharePath() + "boards/Common/VT1.svg", Scale);
            vent[1] = PICSimLab.LoadImageFile(PICSimLab.GetSharePath() + "boards/Common/VT2.svg", Scale);
            lcdbmp[0] = PICSimLab.LoadImageFile(PICSimLab.GetSharePath() + "boards/Common/lcd2.svg", Scale);
            lcdbmp[1] = PICSimLab.LoadImageFile(PICSimLab.GetSharePath() + "boards/Common/lcd4.svg", Scale);
        }
    }
}

int cboard_PICGenios::MDumpMemory(const char* mfname) {
    FILE* fout;
    char fname[1024];

    strncpy(fname, (const char*)dirname(mfname).c_str(), 1023);
    strncat(fname, "/mdump_PICGenios_EEPROM.bin", 1023);

    fout = fopen_UTF8(fname, "wb");
    if (fout) {
        fwrite(mi2c.data, mi2c.SIZE, 1, fout);
        fclose(fout);
    } else {
        printf("Error saving to file: %s \n", fname);
    }

    return bsim_picsim::MDumpMemory(mfname);
}

void cboard_PICGenios::Draw(void) {
    int i;
    int update = 0;

    pic_set_pin(&pic, 39, 1);
    pic_set_pin(&pic, 40, 1);

    pic_set_pin(&pic, 19, 1);
    pic_set_pin(&pic, 20, 1);
    pic_set_pin(&pic, 21, 1);
    pic_set_pin(&pic, 22, 1);
    pic_set_pin(&pic, 27, 1);
    pic_set_pin(&pic, 28, 1);
    pic_set_pin(&pic, 29, 1);
    pic_set_pin(&pic, 30, 1);

    // lab4 draw
    for (i = 0; i < outputc; i++) {
        if (output[i].update) {
            output[i].update = 0;

            if (!update) {
                PICSimLab.CanvasCmd({CC_INIT, .Init{Scale, Scale, 0}});
                PICSimLab.CanvasCmd({CC_SETFONTWEIGHT, .SetFontWeight{lxFONTWEIGHT_BOLD}});
            }
            update++;

            if (!output[i].r)  // rectangle
            {
                PICSimLab.CanvasCmd({CC_SETFGCOLOR, .SetFgColor{0, 0, 0}});

                switch (output[i].id) {
                    case O_A1:
                        PICSimLab.CanvasCmd({CC_SETBGCOLOR, .SetBgColor{0, lm1[18], 0}});
                        break;
                    case O_B1:
                        PICSimLab.CanvasCmd({CC_SETBGCOLOR, .SetBgColor{0, lm1[19], 0}});
                        break;
                    case O_C1:
                        PICSimLab.CanvasCmd({CC_SETBGCOLOR, .SetBgColor{0, lm1[20], 0}});
                        break;
                    case O_D1:
                        PICSimLab.CanvasCmd({CC_SETBGCOLOR, .SetBgColor{0, lm1[21], 0}});
                        break;
                    case O_E1:
                        PICSimLab.CanvasCmd({CC_SETBGCOLOR, .SetBgColor{0, lm1[26], 0}});
                        break;
                    case O_F1:
                        PICSimLab.CanvasCmd({CC_SETBGCOLOR, .SetBgColor{0, lm1[27], 0}});
                        break;
                    case O_G1:
                        PICSimLab.CanvasCmd({CC_SETBGCOLOR, .SetBgColor{0, lm1[28], 0}});
                        break;

                    case O_A2:
                        PICSimLab.CanvasCmd({CC_SETBGCOLOR, .SetBgColor{0, lm2[18], 0}});
                        break;
                    case O_B2:
                        PICSimLab.CanvasCmd({CC_SETBGCOLOR, .SetBgColor{0, lm2[19], 0}});
                        break;
                    case O_C2:
                        PICSimLab.CanvasCmd({CC_SETBGCOLOR, .SetBgColor{0, lm2[20], 0}});
                        break;
                    case O_D2:
                        PICSimLab.CanvasCmd({CC_SETBGCOLOR, .SetBgColor{0, lm2[21], 0}});
                        break;
                    case O_E2:
                        PICSimLab.CanvasCmd({CC_SETBGCOLOR, .SetBgColor{0, lm2[26], 0}});
                        break;
                    case O_F2:
                        PICSimLab.CanvasCmd({CC_SETBGCOLOR, .SetBgColor{0, lm2[27], 0}});
                        break;
                    case O_G2:
                        PICSimLab.CanvasCmd({CC_SETBGCOLOR, .SetBgColor{0, lm2[28], 0}});
                        break;

                    case O_A3:
                        PICSimLab.CanvasCmd({CC_SETBGCOLOR, .SetBgColor{0, lm3[18], 0}});
                        break;
                    case O_B3:
                        PICSimLab.CanvasCmd({CC_SETBGCOLOR, .SetBgColor{0, lm3[19], 0}});
                        break;
                    case O_C3:
                        PICSimLab.CanvasCmd({CC_SETBGCOLOR, .SetBgColor{0, lm3[20], 0}});
                        break;
                    case O_D3:
                        PICSimLab.CanvasCmd({CC_SETBGCOLOR, .SetBgColor{0, lm3[21], 0}});
                        break;
                    case O_E3:
                        PICSimLab.CanvasCmd({CC_SETBGCOLOR, .SetBgColor{0, lm3[26], 0}});
                        break;
                    case O_F3:
                        PICSimLab.CanvasCmd({CC_SETBGCOLOR, .SetBgColor{0, lm3[27], 0}});
                        break;
                    case O_G3:
                        PICSimLab.CanvasCmd({CC_SETBGCOLOR, .SetBgColor{0, lm3[28], 0}});
                        break;

                    case O_A4:
                        PICSimLab.CanvasCmd({CC_SETBGCOLOR, .SetBgColor{0, lm4[18], 0}});
                        break;
                    case O_B4:
                        PICSimLab.CanvasCmd({CC_SETBGCOLOR, .SetBgColor{0, lm4[19], 0}});
                        break;
                    case O_C4:
                        PICSimLab.CanvasCmd({CC_SETBGCOLOR, .SetBgColor{0, lm4[20], 0}});
                        break;
                    case O_D4:
                        PICSimLab.CanvasCmd({CC_SETBGCOLOR, .SetBgColor{0, lm4[21], 0}});
                        break;
                    case O_E4:
                        PICSimLab.CanvasCmd({CC_SETBGCOLOR, .SetBgColor{0, lm4[26], 0}});
                        break;
                    case O_F4:
                        PICSimLab.CanvasCmd({CC_SETBGCOLOR, .SetBgColor{0, lm4[27], 0}});
                        break;
                    case O_G4:
                        PICSimLab.CanvasCmd({CC_SETBGCOLOR, .SetBgColor{0, lm4[28], 0}});
                        break;

                    case O_LCD:
                        PICSimLab.CanvasCmd(
                            {CC_SETCOLOR, .SetColor{0, (unsigned int)(90 * PICSimLab.GetMcuPwr() + 40), 0}});
                        break;
                    case O_BRB0:
                    case O_BRB1:
                    case O_BRB2:
                    case O_BRB3:
                    case O_BRB4:
                    case O_BRB5:
                    case O_BRA5:
                        PICSimLab.CanvasCmd({CC_SETCOLOR, .SetColor{100, 100, 100}});
                        PICSimLab.CanvasCmd({CC_CIRCLE, .Circle{1, output[i].cx, output[i].cy, 11}});
                        if (p_BT[output[i].id - O_BRB0]) {
                            PICSimLab.CanvasCmd({CC_SETCOLOR, .SetColor{15, 15, 15}});
                        } else {
                            PICSimLab.CanvasCmd({CC_SETCOLOR, .SetColor{55, 55, 55}});
                        }
                        break;
                    case O_RST:
                        PICSimLab.CanvasCmd({CC_SETCOLOR, .SetColor{100, 100, 100}});
                        PICSimLab.CanvasCmd({CC_CIRCLE, .Circle{1, output[i].cx, output[i].cy, 11}});
                        if (p_RST) {
                            PICSimLab.CanvasCmd({CC_SETCOLOR, .SetColor{15, 15, 15}});
                        } else {
                            PICSimLab.CanvasCmd({CC_SETCOLOR, .SetColor{55, 55, 55}});
                        }
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
                    case O_TCA:
                    case O_TC0:
                    case O_TCT:
                        PICSimLab.CanvasCmd({CC_SETCOLOR, .SetColor{100, 100, 100}});
                        PICSimLab.CanvasCmd({CC_CIRCLE, .Circle{1, output[i].cx, output[i].cy, 11}});
                        if (p_BT[output[i].id - O_BRB0]) {
                            PICSimLab.CanvasCmd({CC_SETCOLOR, .SetColor{55, 55, 55}});
                        } else {
                            PICSimLab.CanvasCmd({CC_SETCOLOR, .SetColor{15, 15, 1}});
                        }
                        break;
                    case O_MP:
                        PICSimLab.CanvasCmd({CC_SETCOLOR, .SetColor{26, 26, 26}});
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
                        PICSimLab.CanvasCmd({CC_SETCOLOR, .SetColor{10, 10, 10}});
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
                        PICSimLab.CanvasCmd({CC_SETCOLOR, .SetColor{10, 10, 10}});
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
                        PICSimLab.CanvasCmd({CC_SETCOLOR, .SetColor{10, 10, 10}});
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
                        PICSimLab.CanvasCmd({CC_SETCOLOR, .SetColor{10, 10, 10}});
                        break;
                    default:
                        if ((output[i].name[0] == 'D') && (output[i].name[1] == 'P')) {
                            PICSimLab.CanvasCmd({CC_SETBGCOLOR, .SetBgColor{250, 250, 250}});
                            break;
                        }
                }

                if (output[i].id == O_JP1)
                    PICSimLab.CanvasCmd({CC_SETCOLOR, .SetColor{150, 150, 150}});

                if ((output[i].id >= O_BRB0) && (output[i].id <= O_TCT)) {
                    PICSimLab.CanvasCmd({CC_CIRCLE, .Circle{1, output[i].cx, output[i].cy, 9}});
                } else if (output[i].id == O_RST) {
                    PICSimLab.CanvasCmd({CC_CIRCLE, .Circle{1, output[i].cx, output[i].cy, 9}});
                } else if ((output[i].id == O_POT1) || (output[i].id == O_POT2)) {
                    PICSimLab.CanvasCmd({CC_SETBGCOLOR, .SetBgColor{66, 109, 246}});
                    PICSimLab.CanvasCmd(
                        {CC_RECTANGLE, .Rectangle{1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1,
                                                  output[i].y2 - output[i].y1}});

                    PICSimLab.CanvasCmd({CC_SETBGCOLOR, .SetBgColor{250, 250, 250}});
                    PICSimLab.CanvasCmd({CC_CIRCLE, .Circle{1, output[i].cx, output[i].cy, 20}});

                    PICSimLab.CanvasCmd({CC_SETBGCOLOR, .SetBgColor{150, 150, 150}});
                    int x = -15 * sin((5.585 * (pot[output[i].id - O_POT1] / 200.0)) + 0.349);
                    int y = 15 * cos((5.585 * (pot[output[i].id - O_POT1] / 200.0)) + 0.349);
                    PICSimLab.CanvasCmd({CC_CIRCLE, .Circle{1, output[i].cx + x, output[i].cy + y, 3}});
                } else if ((output[i].id != O_LCD) && (output[i].id != O_VT)) {
                    PICSimLab.CanvasCmd(
                        {CC_RECTANGLE, .Rectangle{1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1,
                                                  output[i].y2 - output[i].y1}});
                }

                if (output[i].id == O_VT) {
                    PICSimLab.CanvasCmd({CC_CHANGESCALE, .ChangeScale{1.0, 1.0}});
                    PICSimLab.CanvasCmd(
                        {CC_PUTBITMAP, .PutBitmap{vent[vt], output[i].x1 * Scale, output[i].y1 * Scale}});
                    PICSimLab.CanvasCmd({CC_CHANGESCALE, .ChangeScale{Scale, Scale}});
                } else if (output[i].id == O_LCD) {
                    if (lcd.update) {
                        PICSimLab.CanvasCmd({CC_CHANGESCALE, .ChangeScale{1.0, 1.0}});
                        if (lcd.lnum == 2) {
                            PICSimLab.CanvasCmd({CC_PUTBITMAP, .PutBitmap{lcdbmp[0], (output[i].x1 - 41) * Scale,
                                                                          (output[i].y1 - 58) * Scale}});
                        } else {
                            PICSimLab.CanvasCmd({CC_PUTBITMAP, .PutBitmap{lcdbmp[1], (output[i].x1 - 41) * Scale,
                                                                          (output[i].y1 - 58) * Scale}});
                        }
                        PICSimLab.CanvasCmd({CC_CHANGESCALE, .ChangeScale{Scale, Scale}});
                        PICSimLab.CanvasCmd(
                            {CC_RECTANGLE,
                             .Rectangle{1, output[i].x1 - 1, output[i].y1 - 2, output[i].x2 - output[i].x1 + 2,
                                        output[i].y2 - output[i].y1 + ((lcd.lnum == 2) ? 3 : 78)}});
                        if (dip[0]) {
                            lcd_draw(&lcd, PICSimLab.CanvasCmd, output[i].x1, output[i].y1, output[i].x2 - output[i].x1,
                                     output[i].y2 - output[i].y1, PICSimLab.GetMcuPwr());
                        }
                    }
                } else if ((output[i].name[0] == 'D') && (output[i].name[1] == 'P')) {
                    if (dip[(((output[i].name[3] - 0x30) * 10) + (output[i].name[4] - 0x30)) - 1]) {
                        PICSimLab.CanvasCmd({CC_SETBGCOLOR, .SetBgColor{70, 70, 70}});
                        PICSimLab.CanvasCmd(
                            {CC_RECTANGLE,
                             .Rectangle{1, output[i].x1, output[i].y1 + ((int)((output[i].y2 - output[i].y1) * 0.35)),
                                        output[i].x2 - output[i].x1, ((output[i].y2 - output[i].y1) * 0.65f)}});
                    } else {
                        PICSimLab.CanvasCmd({CC_SETBGCOLOR, .SetBgColor{70, 70, 70}});
                        PICSimLab.CanvasCmd(
                            {CC_RECTANGLE, .Rectangle{1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1,
                                                      ((output[i].y2 - output[i].y1) * 0.65f)}});
                    }
                } else if (output[i].id == O_JP1) {
                    if (!jmp[0]) {
                        PICSimLab.CanvasCmd({CC_SETCOLOR, .SetColor{70, 70, 70}});
                        PICSimLab.CanvasCmd({CC_RECTANGLE, .Rectangle{1, output[i].x1, output[i].y1,
                                                                      ((output[i].x2 - output[i].x1) * 0.65f),
                                                                      output[i].y2 - output[i].y1}});
                        PICSimLab.CanvasCmd({CC_SETCOLOR, .SetColor{220, 220, 0}});
                        PICSimLab.CanvasCmd(
                            {CC_CIRCLE, .Circle{1, output[i].x1 + (int)((output[i].x2 - output[i].x1) * 0.80),
                                                output[i].y1 + ((output[i].y2 - output[i].y1) / 2), 3}});
                    } else {
                        PICSimLab.CanvasCmd({CC_SETCOLOR, .SetColor{70, 70, 70}});
                        PICSimLab.CanvasCmd(
                            {CC_RECTANGLE,
                             .Rectangle{1, output[i].x1 + ((int)((output[i].x2 - output[i].x1) * 0.35)), output[i].y1,
                                        ((output[i].x2 - output[i].x1) * 0.65f), output[i].y2 - output[i].y1}});
                        PICSimLab.CanvasCmd({CC_SETCOLOR, .SetColor{220, 220, 0}});
                        PICSimLab.CanvasCmd(
                            {CC_CIRCLE, .Circle{1, output[i].x1 + (int)((output[i].x2 - output[i].x1) * 0.20),
                                                output[i].y1 + ((output[i].y2 - output[i].y1) / 2), 3}});
                    }
                } else if (output[i].id == O_MP) {
                    PICSimLab.CanvasCmd({CC_SETFONTSIZE, .SetFontSize{10}});
                    PICSimLab.CanvasCmd(
                        {CC_RECTANGLE, .Rectangle{1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1,
                                                  output[i].y2 - output[i].y1}});
                    PICSimLab.CanvasCmd({CC_SETCOLOR, .SetColor{230, 230, 230}});
                    PICSimLab.CanvasCmd(
                        {CC_ROTATEDTEXT, .RotatedText{Proc.c_str(), output[i].x1 + 20, output[i].y1 + 50, -90}});
                }
            } else  // circle output
            {
                int led = 1;

                PICSimLab.CanvasCmd({CC_SETCOLOR, .SetColor{55, 0, 0}});

                if (dip[9])
                    switch (output[i].id) {
                        case O_RB0:
                            PICSimLab.CanvasCmd({CC_SETBGCOLOR, .SetBgColor{(unsigned int)pic.pins[32].oavalue, 0, 0}});
                            break;
                        case O_RB1:
                            PICSimLab.CanvasCmd({CC_SETBGCOLOR, .SetBgColor{(unsigned int)pic.pins[33].oavalue, 0, 0}});
                            break;
                        case O_RB2:
                            PICSimLab.CanvasCmd({CC_SETBGCOLOR, .SetBgColor{(unsigned int)pic.pins[34].oavalue, 0, 0}});
                            break;
                        case O_RB3:
                            PICSimLab.CanvasCmd({CC_SETBGCOLOR, .SetBgColor{(unsigned int)pic.pins[35].oavalue, 0, 0}});
                            break;
                        case O_RB4:
                            PICSimLab.CanvasCmd({CC_SETBGCOLOR, .SetBgColor{(unsigned int)pic.pins[36].oavalue, 0, 0}});
                            break;
                        case O_RB5:
                            PICSimLab.CanvasCmd({CC_SETBGCOLOR, .SetBgColor{(unsigned int)pic.pins[37].oavalue, 0, 0}});
                            break;
                        case O_RB6:
                            PICSimLab.CanvasCmd({CC_SETBGCOLOR, .SetBgColor{(unsigned int)pic.pins[38].oavalue, 0, 0}});
                            break;
                        case O_RB7:
                            PICSimLab.CanvasCmd({CC_SETBGCOLOR, .SetBgColor{(unsigned int)pic.pins[39].oavalue, 0, 0}});
                            break;
                    }

                if (dip[8])
                    switch (output[i].id) {
                        case O_RD0:
                            PICSimLab.CanvasCmd({CC_SETBGCOLOR, .SetBgColor{(unsigned int)pic.pins[18].oavalue, 0, 0}});
                            break;
                        case O_RD1:
                            PICSimLab.CanvasCmd({CC_SETBGCOLOR, .SetBgColor{(unsigned int)pic.pins[19].oavalue, 0, 0}});
                            break;
                        case O_RD2:
                            PICSimLab.CanvasCmd({CC_SETBGCOLOR, .SetBgColor{(unsigned int)pic.pins[20].oavalue, 0, 0}});
                            break;
                        case O_RD3:
                            PICSimLab.CanvasCmd({CC_SETBGCOLOR, .SetBgColor{(unsigned int)pic.pins[21].oavalue, 0, 0}});
                            break;
                        case O_RD4:
                            PICSimLab.CanvasCmd({CC_SETBGCOLOR, .SetBgColor{(unsigned int)pic.pins[26].oavalue, 0, 0}});
                            break;
                        case O_RD5:
                            PICSimLab.CanvasCmd({CC_SETBGCOLOR, .SetBgColor{(unsigned int)pic.pins[27].oavalue, 0, 0}});
                            break;
                        case O_RD6:
                            PICSimLab.CanvasCmd({CC_SETBGCOLOR, .SetBgColor{(unsigned int)pic.pins[28].oavalue, 0, 0}});
                            break;
                        case O_RD7:
                            PICSimLab.CanvasCmd({CC_SETBGCOLOR, .SetBgColor{(unsigned int)pic.pins[29].oavalue, 0, 0}});
                            break;
                    }

                switch (output[i].id) {
                    case O_RL1:
                        PICSimLab.CanvasCmd({CC_SETFGCOLOR, .SetFgColor{0, 55, 0}});
                        if (dip[3])
                            PICSimLab.CanvasCmd({CC_SETBGCOLOR, .SetBgColor{0, (unsigned int)pic.pins[14].oavalue, 0}});
                        else
                            PICSimLab.CanvasCmd({CC_SETBGCOLOR, .SetBgColor{0, 15, 0}});
                        break;
                    case O_RL2:
                        PICSimLab.CanvasCmd({CC_SETFGCOLOR, .SetFgColor{0, 55, 0}});
                        if (dip[4])
                            PICSimLab.CanvasCmd({CC_SETBGCOLOR, .SetBgColor{0, (unsigned int)pic.pins[7].oavalue, 0}});
                        else
                            PICSimLab.CanvasCmd({CC_SETBGCOLOR, .SetBgColor{0, 15, 0}});
                        break;
                    case O_P1:
                        PICSimLab.CanvasCmd({CC_SETFGCOLOR, .SetFgColor{0, 55, 0}});
                        PICSimLab.CanvasCmd({CC_SETBGCOLOR, .SetBgColor{0, lm1[29], 0}});
                        led = 0;
                        break;
                    case O_P2:
                        PICSimLab.CanvasCmd({CC_SETFGCOLOR, .SetFgColor{0, 55, 0}});
                        PICSimLab.CanvasCmd({CC_SETBGCOLOR, .SetBgColor{0, lm2[29], 0}});
                        led = 0;
                        break;
                    case O_P3:
                        PICSimLab.CanvasCmd({CC_SETFGCOLOR, .SetFgColor{0, 55, 0}});
                        PICSimLab.CanvasCmd({CC_SETBGCOLOR, .SetBgColor{0, lm3[29], 0}});
                        led = 0;
                        break;
                    case O_P4:
                        PICSimLab.CanvasCmd({CC_SETFGCOLOR, .SetFgColor{0, 55, 0}});
                        PICSimLab.CanvasCmd({CC_SETBGCOLOR, .SetBgColor{0, lm4[29], 0}});
                        led = 0;
                        break;
                    case O_RUN:
                    case O_LPWR:
                        PICSimLab.CanvasCmd({CC_SETFGCOLOR, .SetFgColor{0, 55, 0}});
                        PICSimLab.CanvasCmd(
                            {CC_SETBGCOLOR, .SetBgColor{0, (unsigned int)(200 * PICSimLab.GetMcuPwr() + 55), 0}});
                        break;
                }

                if (led) {
                    DrawLED(PICSimLab.CanvasCmd, &output[i]);
                } else {
                    PICSimLab.CanvasCmd({CC_CIRCLE, .Circle{1, output[i].x1, output[i].y1, output[i].r - 1}});
                }
            }
        }
    }
    // end draw

    if (update) {
        PICSimLab.CanvasCmd({CC_END});
    }

    // buzzer
    if ((((pic.pins[15].oavalue - 55) / 2) > 10) && (PICSimLab.GetMcuPwr()) && jmp[0]) {
        if (!sound_on) {
            buzzer.BeepStart();
            sound_on = 1;
        }
    } else {
        buzzer.BeepStop();
        sound_on = 0;
    }

    // Cooler
    if (dip[17]) {
        cooler_pwr = pic.pins[16].oavalue - 55;
    } else {
        cooler_pwr = 0;
    }
    int value = cooler_pwr / 2;
    PICSimLab.UpdateGUI(COOLER, GT_GAUGE, GA_SET, (void*)&value);

    // Heater
    if (dip[15]) {
        heater_pwr = pic.pins[23].oavalue - 55;
    } else {
        heater_pwr = 0;
    }
    value = heater_pwr / 2;
    PICSimLab.UpdateGUI(HEATER, GT_GAUGE, GA_SET, (void*)&value);

    // potentiometers
    vp2in = (5.0 * pot[0] / 199);
    vp1in = (5.0 * pot[1] / 199);

    // ref
    // pic_set_apin(pic,5,2.5);
}

void cboard_PICGenios::OnTime(void) {
    if (cooler_pwr > 40) {
        vtc++;
    }
    if (vtc > (4 - 0.02 * cooler_pwr)) {
        vtc = 0;
        vt ^= 1;
        output_ids[O_VT]->update = 1;
    }

    // thacometer
    rpmstp = 2000.0 / (0.7196 * (cooler_pwr + 1));

    // temp
    ref = ((0.25 * (heater_pwr))) - (0.25 * (cooler_pwr));

    temp[1] = temp[0];
    temp[0] = ((27.5 + ref) * 0.003) + temp[1] * (0.997);

    if (temp[0] < 27.5)
        temp[0] = 27.5;

    if (dip[16])
        pic_set_apin(&pic, 4, temp[0] / 100.0);
}

void cboard_PICGenios::Run_CPU(void) {
    int i;
    int j;
    unsigned char pi, pj;
    unsigned char pinv;
    const picpin* pins;
    int bret;

    unsigned int alm[40];   // luminosidade
                            // media
    unsigned int alm1[40];  // luminosidade
                            // media
                            // display
    unsigned int alm2[40];  // luminosidade
                            // media
                            // display
    unsigned int alm3[40];  // luminosidade
                            // media
                            // display
    unsigned int alm4[40];  // luminosidade
                            // media
                            // display

    const int JUMPSTEPS = PICSimLab.GetJUMPSTEPS();
    const long int NSTEPJ = PICSimLab.GetNSTEPJ();
    const long int NSTEP = PICSimLab.GetNSTEP();
    const float RNSTEP = 200.0 * pic.PINCOUNT / NSTEP;

    if (use_spare)
        SpareParts.PreProcess();

    memset(alm, 0, 40 * sizeof(unsigned int));
    memset(alm1, 0, 40 * sizeof(unsigned int));
    memset(alm2, 0, 40 * sizeof(unsigned int));
    memset(alm3, 0, 40 * sizeof(unsigned int));
    memset(alm4, 0, 40 * sizeof(unsigned int));

    pins = pic.pins;

    unsigned char p_BT_[7];
    memcpy(p_BT_, p_BT, 7);

    SWBounce_prepare(&bounce, PICSimLab.GetBoard()->MGetInstClockFreq());
    for (int pl = 0; pl < 6; pl++) {
        if ((pins[33 + pl - 1].dir == PD_IN) && (pins[33 + pl - 1].value != p_BT_[pl])) {
            switch (pl) {
                case 0:
                    if (!(p_KEY[0] | p_KEY[3] | p_KEY[6] | p_KEY[9])) {
                        SWBounce_bounce(&bounce, pl);
                    }
                    break;
                case 1:
                    if (!(p_KEY[1] | p_KEY[4] | p_KEY[7] | p_KEY[10])) {
                        SWBounce_bounce(&bounce, pl);
                    }
                    break;
                case 2:
                    if (!(p_KEY[2] | p_KEY[5] | p_KEY[8] | p_KEY[11])) {
                        SWBounce_bounce(&bounce, pl);
                    }
                    break;
                default:
                    SWBounce_bounce(&bounce, pl);
                    break;
            }
        }
    }
    if ((pins[7 - 1].dir == PD_IN) && (pins[7 - 1].value != p_BT_[6])) {
        SWBounce_bounce(&bounce, 6);
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
                    pic_set_pin(&pic, 37, p_BT_[4]);
                    pic_set_pin(&pic, 38, p_BT_[5]);
                    pic_set_pin(&pic, 7, p_BT_[6]);
                }

                /*
                    pic_set_pin(&pic,
                   39,
                   1);
                    pic_set_pin(&pic,
                   40,
                   1);
                    pic_set_pin(&pic,
                   19,1);
                    pic_set_pin(&pic,
                   20,1);
                    pic_set_pin(&pic,
                   21,1);
                    pic_set_pin(&pic,
                   22,1);
                    pic_set_pin(&pic,
                   27,1);
                    pic_set_pin(&pic,
                   28,1);
                    pic_set_pin(&pic,
                   29,1);
                    pic_set_pin(&pic,
                   30,1);
                     */

                // keyboard

                if (p_KEY[0]) {
                    pic_set_pin(&pic, 22, pic_get_pin(&pic, 33));
                    pic_set_pin(&pic, 33, pic_get_pin(&pic, 22));
                }

                if (p_KEY[1]) {
                    pic_set_pin(&pic, 22, pic_get_pin(&pic, 34));
                    pic_set_pin(&pic, 34, pic_get_pin(&pic, 22));
                }

                if (p_KEY[2]) {
                    pic_set_pin(&pic, 22, pic_get_pin(&pic, 35));
                    pic_set_pin(&pic, 35, pic_get_pin(&pic, 22));
                }

                if (p_KEY[3]) {
                    pic_set_pin(&pic, 21, pic_get_pin(&pic, 33));
                    pic_set_pin(&pic, 33, pic_get_pin(&pic, 21));
                }

                if (p_KEY[4]) {
                    pic_set_pin(&pic, 21, pic_get_pin(&pic, 34));
                    pic_set_pin(&pic, 34, pic_get_pin(&pic, 21));
                }

                if (p_KEY[5]) {
                    pic_set_pin(&pic, 21, pic_get_pin(&pic, 35));
                    pic_set_pin(&pic, 35, pic_get_pin(&pic, 21));
                }

                if (p_KEY[6]) {
                    pic_set_pin(&pic, 20, pic_get_pin(&pic, 33));
                    pic_set_pin(&pic, 33, pic_get_pin(&pic, 20));
                }

                if (p_KEY[7]) {
                    pic_set_pin(&pic, 20, pic_get_pin(&pic, 34));
                    pic_set_pin(&pic, 34, pic_get_pin(&pic, 20));
                }

                if (p_KEY[8]) {
                    pic_set_pin(&pic, 20, pic_get_pin(&pic, 35));
                    pic_set_pin(&pic, 35, pic_get_pin(&pic, 20));
                }

                if (p_KEY[9]) {
                    pic_set_pin(&pic, 19, pic_get_pin(&pic, 33));
                    pic_set_pin(&pic, 33, pic_get_pin(&pic, 19));
                }

                if (p_KEY[10]) {
                    pic_set_pin(&pic, 19, pic_get_pin(&pic, 34));
                    pic_set_pin(&pic, 34, pic_get_pin(&pic, 19));
                }

                if (p_KEY[11]) {
                    pic_set_pin(&pic, 19, pic_get_pin(&pic, 35));
                    pic_set_pin(&pic, 35, pic_get_pin(&pic, 19));
                }

                if (dip[14]) {
                    if (cooler_pwr > 55) {
                        rpmc++;
                        if (rpmc > rpmstp) {
                            rpmc = 0;
                            pic_set_pin(&pic, 15, !pins[14].value);
                        }
                    } else
                        pic_set_pin(&pic, 15, 0);
                }
            }

            if (bounce.do_bounce) {
                bret = SWBounce_process(&bounce);
                if (bret) {
                    for (int pl = 0; pl < 6; pl++) {
                        if (bounce.bounce[pl]) {
                            if (bret == 1) {
                                pic_set_pin(&pic, 33 + pl, !pins[33 + pl - 1].value);
                            } else {
                                pic_set_pin(&pic, 33 + pl, p_BT_[pl]);
                            }
                        }
                    }
                    if (bounce.bounce[6]) {
                        if (bret == 1) {
                            pic_set_pin(&pic, 7, !pins[7 - 1].value);
                        } else {
                            pic_set_pin(&pic, 7, p_BT_[6]);
                        }
                    }
                }
            }

            if (!mplabxd_testbp())
                pic_step(&pic);
            ioupdated = pic.ioupdated;
            InstCounterInc();
            if (use_oscope)
                Oscilloscope.SetSample();
            if (use_spare)
                SpareParts.Process();

            // increment
            // mean
            // value
            // counter
            // if pin is
            // high
            alm[pi] += pins[pi].value;
            pi++;
            if (pi == pic.PINCOUNT)
                pi = 0;

            if (j >= JUMPSTEPS) {
                for (pj = 18; pj < 30; pj++) {
                    pinv = pins[pj].value;
                    if ((pinv) && (pins[3].value) && (dip[10]))
                        alm1[pj]++;
                    if ((pinv) && (pins[4].value) && (dip[11]))
                        alm2[pj]++;
                    if ((pinv) && (pins[5].value) && (dip[12]))
                        alm3[pj]++;
                    if ((pinv) && (pins[6].value) && (dip[13]))
                        alm4[pj]++;
                }

                if (dip[7])
                    alm[32] = 0;

                // potenciometro
                // p1 e
                // p2
                if (dip[18])
                    pic_set_apin(&pic, 2, vp1in);
                if (dip[19])
                    pic_set_apin(&pic, 3, vp2in);

                j = -1;
            }
            j++;

            if (ioupdated) {
                // lcd
                // dipins[2].display
                // code

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

                        if ((!pins[9].dir) && (!pins[9].value)) {
                            lcd_cmd(&lcd, d);
                        } else if ((!pins[9].dir) && (pins[9].value)) {
                            lcd_data(&lcd, d);
                        }
                        lcde = 1;
                    }
                } else {
                    lcde = 0;
                }
                // end
                // display
                // code

                // i2c
                // code
                if (pins[22].dir) {
                    sda = 1;
                } else {
                    sda = pins[22].value;
                }

                if (pins[17].dir) {
                    sck = 1;
                    if (dip[5]) {
                        pic_set_pin(&pic, 18, 1);
                    }
                } else {
                    sck = pins[17].value;
                }
                if (dip[6]) {
                    pic_set_pin(&pic, 23, mi2c_io(&mi2c, sck, sda) & rtc_ds1307_I2C_io(&rtc2, sck, sda));
                }
            }
            pic.ioupdated = 0;
        }

    // fim STEP

    for (i = 0; i < pic.PINCOUNT; i++) {
        if (pic.pins[i].port == P_VDD)
            pic.pins[i].oavalue = 255;
        else
            pic.pins[i].oavalue = (int)((alm[i] * RNSTEP) + 55);

        lm1[i] = (int)(((600.0 * alm1[i]) / NSTEPJ) + 30);
        lm2[i] = (int)(((600.0 * alm2[i]) / NSTEPJ) + 30);
        lm3[i] = (int)(((600.0 * alm3[i]) / NSTEPJ) + 30);
        lm4[i] = (int)(((600.0 * alm4[i]) / NSTEPJ) + 30);
        if (lm1[i] > 255)
            lm1[i] = 255;
        if (lm2[i] > 255)
            lm2[i] = 255;
        if (lm3[i] > 255)
            lm3[i] = 255;
        if (lm4[i] > 255)
            lm4[i] = 255;
    }

    if (use_spare)
        SpareParts.PostProcess();

    if ((lcd.update) && (dip[0]))
        output_ids[O_LCD]->update = 1;

    for (i = 0; i < 8; i++) {
        // PORTB LEDS
        if ((dip[9]) && (output_ids[O_RB0 + i]->value != pic.pins[32 + i].oavalue)) {
            output_ids[O_RB0 + i]->value = pic.pins[32 + i].oavalue;
            output_ids[O_RB0 + i]->update = 1;
        }

        if (i < 4) {
            j = i;
        } else {
            j = i + 4;
        }
        // PORTD LEDS
        if ((dip[8]) && (output_ids[O_RD0 + i]->value != pic.pins[18 + j].oavalue)) {
            output_ids[O_RD0 + i]->value = pic.pins[18 + j].oavalue;
            output_ids[O_RD0 + i]->update = 1;
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

    // RELAY
    if ((dip[3]) && (output_ids[O_RL1]->value != pic.pins[14].oavalue)) {
        output_ids[O_RL1]->value = pic.pins[14].oavalue;
        output_ids[O_RL1]->update = 1;
    }
    if ((dip[4]) && (output_ids[O_RL2]->value != pic.pins[7].oavalue)) {
        output_ids[O_RL2]->value = pic.pins[7].oavalue;
        output_ids[O_RL2]->update = 1;
    }
}

void cboard_PICGenios::Reset(void) {
    pic_reset(&pic, 1);

    lcd_rst(&lcd);
    mi2c_rst(&mi2c);
    rtc_ds1307_rst(&rtc2);

    p_BT[0] = 1;
    p_BT[1] = 1;
    p_BT[2] = 1;
    p_BT[3] = 1;
    p_BT[4] = 1;
    p_BT[5] = 1;
    p_BT[6] = 1;

    pic_set_pin(&pic, 33, p_BT[0]);
    pic_set_pin(&pic, 34, p_BT[1]);
    pic_set_pin(&pic, 35, p_BT[2]);
    pic_set_pin(&pic, 36, p_BT[3]);
    pic_set_pin(&pic, 37, p_BT[4]);
    pic_set_pin(&pic, 38, p_BT[5]);
    pic_set_pin(&pic, 7, p_BT[6]);

    pic_set_pin_DOV(&pic, 39, 1);
    pic_set_pin_DOV(&pic, 40, 1);

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
    p_KEY[10] = 0;
    p_KEY[11] = 0;
    pic_set_pin(&pic, 33, 0);
    pic_set_pin(&pic, 34, 0);
    pic_set_pin(&pic, 35, 0);

    pic_set_pin(&pic, 20, 0);
    pic_set_pin(&pic, 21, 0);
    pic_set_pin(&pic, 22, 0);
    pic_set_pin(&pic, 23, 0);

    if (pic.serial[0].serialfd != INVALID_SERIAL)
        PICSimLab.UpdateStatus(
            PS_SERIAL, "Serial: " + std::string(SERIALDEVICE) + ":" + std::to_string(pic.serial[0].serialbaud) + "(" +
                           FloatStrFormat("%4"
                                          ".1"
                                          "f",
                                          fabs((100.0 * pic.serial[0].serialexbaud - 100.0 * pic.serial[0].serialbaud) /
                                               pic.serial[0].serialexbaud)) +
                           "%)");
    else
        PICSimLab.UpdateStatus(PS_SERIAL, "Serial: " + std::string(SERIALDEVICE) +
                                              " ("
                                              "ERROR"
                                              ")");

    for (int i = 0; i < pic.PINCOUNT; i++) {
        lm1[i] = 30;
        lm2[i] = 30;
        lm3[i] = 30;
        lm4[i] = 30;
    }

    if (use_spare)
        SpareParts.Reset();

    RegisterRemoteControl();
}

void cboard_PICGenios::RegisterRemoteControl(void) {
    input_ids[I_RB0]->status = &p_BT[0];
    input_ids[I_RB0]->update = &output_ids[O_BRB0]->update;
    input_ids[I_RB1]->status = &p_BT[1];
    input_ids[I_RB1]->update = &output_ids[O_BRB1]->update;
    input_ids[I_RB2]->status = &p_BT[2];
    input_ids[I_RB2]->update = &output_ids[O_BRB2]->update;
    input_ids[I_RB3]->status = &p_BT[3];
    input_ids[I_RB3]->update = &output_ids[O_BRB3]->update;
    input_ids[I_RB4]->status = &p_BT[4];
    input_ids[I_RB4]->update = &output_ids[O_BRB4]->update;
    input_ids[I_RB5]->status = &p_BT[5];
    input_ids[I_RB5]->update = &output_ids[O_BRB5]->update;
    input_ids[I_RA5]->status = &p_BT[6];
    input_ids[I_RA5]->update = &output_ids[O_BRA5]->update;
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
    input_ids[I_TCA]->status = &p_KEY[9];
    input_ids[I_TCA]->update = &output_ids[O_TCA]->update;
    input_ids[I_TC0]->status = &p_KEY[10];
    input_ids[I_TC0]->update = &output_ids[O_TC0]->update;
    input_ids[I_TCT]->status = &p_KEY[11];
    input_ids[I_TCT]->update = &output_ids[O_TCT]->update;
    input_ids[I_POT1]->status = &pot[1];
    input_ids[I_POT1]->update = &output_ids[O_POT2]->update;
    input_ids[I_POT2]->status = &pot[0];
    input_ids[I_POT2]->update = &output_ids[O_POT1]->update;

    output_ids[O_RB0]->status = &pic.pins[32].oavalue;
    output_ids[O_RB1]->status = &pic.pins[33].oavalue;
    output_ids[O_RB2]->status = &pic.pins[34].oavalue;
    output_ids[O_RB3]->status = &pic.pins[35].oavalue;
    output_ids[O_RB4]->status = &pic.pins[36].oavalue;
    output_ids[O_RB5]->status = &pic.pins[37].oavalue;
    output_ids[O_RB6]->status = &pic.pins[38].oavalue;
    output_ids[O_RB7]->status = &pic.pins[39].oavalue;
    output_ids[O_RD0]->status = &pic.pins[18].oavalue;
    output_ids[O_RD1]->status = &pic.pins[19].oavalue;
    output_ids[O_RD2]->status = &pic.pins[20].oavalue;
    output_ids[O_RD3]->status = &pic.pins[21].oavalue;
    output_ids[O_RD4]->status = &pic.pins[26].oavalue;
    output_ids[O_RD5]->status = &pic.pins[27].oavalue;
    output_ids[O_RD6]->status = &pic.pins[28].oavalue;
    output_ids[O_RD7]->status = &pic.pins[29].oavalue;
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

void cboard_PICGenios::EvMouseMove(uint button, uint x, uint y, uint state) {
    int i;

    for (i = 0; i < inputc; i++) {
        switch (input[i].id) {
            case I_POT1:
            case I_POT2:
                if (((input[i].x1 <= x) && (input[i].x2 >= x)) && ((input[i].y1 <= y) && (input[i].y2 >= y))) {
                    if (active[input[i].id - I_POT1]) {
                        pot[input[i].id - I_POT1] = CalcAngle(i, x, y);
                        output_ids[O_POT1 + input[i].id - I_POT1]->update = 1;
                    }
                } else {
                    active[input[i].id - I_POT1] = 0;
                }
                break;
        }
    }
}

void cboard_PICGenios::EvMouseButtonPress(uint button, uint x, uint y, uint state) {
    int i;

    for (i = 0; i < inputc; i++) {
        if (((input[i].x1 <= x) && (input[i].x2 >= x)) && ((input[i].y1 <= y) && (input[i].y2 >= y))) {
            switch (input[i].id) {
                case I_ICSP: {
                    PICSimLab.OpenLoadHexFileDialog();
                    ;
                } break;

                case I_D01: {
                    dip[0] ^= 0x01;
                    output_ids[O_D01]->update = 1;

                    output_ids[O_LCD]->update = 1;
                } break;
                case I_D02: {
                    dip[1] ^= 0x01;
                    output_ids[O_D02]->update = 1;
                } break;
                case I_D03: {
                    dip[2] ^= 0x01;
                    output_ids[O_D03]->update = 1;
                } break;
                case I_D04: {
                    dip[3] ^= 0x01;
                    output_ids[O_D04]->update = 1;
                    output_ids[O_RL1]->update = 1;
                } break;
                case I_D05: {
                    dip[4] ^= 0x01;
                    output_ids[O_D05]->update = 1;
                    output_ids[O_RL2]->update = 1;
                } break;
                case I_D06: {
                    dip[5] ^= 0x01;
                    output_ids[O_D06]->update = 1;
                } break;
                case I_D07: {
                    dip[6] ^= 0x01;
                    output_ids[O_D07]->update = 1;
                } break;
                case I_D08: {
                    dip[7] ^= 0x01;
                    output_ids[O_D08]->update = 1;
                } break;
                case I_D09: {
                    dip[8] ^= 0x01;
                    output_ids[O_D09]->update = 1;

                    output_ids[O_RD0]->update = 1;
                    output_ids[O_RD1]->update = 1;
                    output_ids[O_RD2]->update = 1;
                    output_ids[O_RD3]->update = 1;
                    output_ids[O_RD4]->update = 1;
                    output_ids[O_RD5]->update = 1;
                    output_ids[O_RD6]->update = 1;
                    output_ids[O_RD7]->update = 1;
                } break;
                case I_D10: {
                    dip[9] ^= 0x01;
                    output_ids[O_D10]->update = 1;

                    output_ids[O_RB0]->update = 1;
                    output_ids[O_RB1]->update = 1;
                    output_ids[O_RB2]->update = 1;
                    output_ids[O_RB3]->update = 1;
                    output_ids[O_RB4]->update = 1;
                    output_ids[O_RB5]->update = 1;
                    output_ids[O_RB6]->update = 1;
                    output_ids[O_RB7]->update = 1;
                } break;
                case I_D11: {
                    dip[10] ^= 0x01;
                    output_ids[O_D11]->update = 1;
                } break;
                case I_D12: {
                    dip[11] ^= 0x01;
                    output_ids[O_D12]->update = 1;
                } break;
                case I_D13: {
                    dip[12] ^= 0x01;
                    output_ids[O_D13]->update = 1;
                } break;
                case I_D14: {
                    dip[13] ^= 0x01;
                    output_ids[O_D14]->update = 1;
                } break;
                case I_D15: {
                    dip[14] ^= 0x01;
                    output_ids[O_D15]->update = 1;
                } break;
                case I_D16: {
                    dip[15] ^= 0x01;
                    output_ids[O_D16]->update = 1;
                } break;
                case I_D17: {
                    dip[16] ^= 0x01;
                    output_ids[O_D17]->update = 1;
                    if (!dip[16]) {
                        pic_set_apin(&pic, 4, 0);
                    }
                } break;
                case I_D18: {
                    dip[17] ^= 0x01;
                    output_ids[O_D18]->update = 1;
                } break;
                case I_D19: {
                    dip[18] ^= 0x01;
                    output_ids[O_D19]->update = 1;
                } break;
                case I_D20: {
                    dip[19] ^= 0x01;
                    output_ids[O_D20]->update = 1;
                } break;

                case I_PWR: {
                    if (PICSimLab.GetMcuPwr()) {
                        PICSimLab.SetMcuPwr(0);
                        Reset();
                        p_BT[0] = 0;
                        p_BT[1] = 0;
                        p_BT[2] = 0;
                        p_BT[3] = 0;
                        p_BT[4] = 0;
                        p_BT[5] = 0;
                        p_BT[6] = 0;
                    } else {
                        PICSimLab.SetMcuPwr(1);
                        Reset();
                    }
                    output_ids[O_LPWR]->update = 1;
                    output_ids[O_RUN]->update = 1;
                } break;

                case I_RST: {
                    if (PICSimLab.GetMcuPwr() && pic_reset(&pic, -1)) {
                        PICSimLab.SetMcuPwr(0);
                        PICSimLab.SetMcuRst(1);
                    }
                    p_RST = 0;
                    output_ids[O_RST]->update = 1;
                } break;

                case I_JP1: {
                    jmp[0] ^= 0x01;
                    output_ids[O_JP1]->update = 1;
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
                case I_RB4: {
                    p_BT[4] = 0;
                    output_ids[O_BRB4]->update = 1;
                } break;
                case I_RB5: {
                    p_BT[5] = 0;
                    output_ids[O_BRB5]->update = 1;
                } break;
                case I_RA5: {
                    p_BT[6] = 0;
                    output_ids[O_BRA5]->update = 1;
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

                case I_TCA: {
                    p_KEY[9] = 1;
                    output_ids[O_TCA]->update = 1;
                } break;
                case I_TC0: {
                    p_KEY[10] = 1;
                    output_ids[O_TC0]->update = 1;
                } break;
                case I_TCT: {
                    p_KEY[11] = 1;
                    output_ids[O_TCT]->update = 1;
                } break;
                case I_POT1: {
                    active[0] = 1;
                    pot[0] = CalcAngle(i, x, y);
                    output_ids[O_POT1]->update = 1;
                } break;
                case I_POT2: {
                    active[1] = 1;
                    pot[1] = CalcAngle(i, x, y);
                    output_ids[O_POT2]->update = 1;
                } break;
                case I_VIEW:
                    FILE* fout;
                    fout = fopen_UTF8(mi2c_tmp_name, "w");
                    if (fout) {
                        for (unsigned int i = 0; i < mi2c.SIZE; i += 16) {
                            fprintf(fout,
                                    "%0"
                                    "4X"
                                    ":"
                                    " ",
                                    i);
                            for (int j = 0; j < 16; j++) {
                                fprintf(fout, "%02X ", mi2c.data[j + i]);
                            }
                            fprintf(fout,
                                    "\r"
                                    "\n");
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
                        lxLaunchDefaultApplication(mi2c_tmp_name);
#endif
                    } else {
                        printf(
                            "Error "
                            "saving"
                            " to "
                            "file: "
                            "%s \n",
                            mi2c_tmp_name);
                    }
                    break;
            }
        }
    }
}

void cboard_PICGenios::EvMouseButtonRelease(uint button, uint x, uint y, uint state) {
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
                case I_RB4: {
                    p_BT[4] = 1;
                    output_ids[O_BRB4]->update = 1;
                } break;
                case I_RB5: {
                    p_BT[5] = 1;
                    output_ids[O_BRB5]->update = 1;
                } break;
                case I_RA5: {
                    p_BT[6] = 1;
                    output_ids[O_BRA5]->update = 1;
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

                case I_TCA: {
                    p_KEY[9] = 0;
                    output_ids[O_TCA]->update = 1;
                } break;
                case I_TC0: {
                    p_KEY[10] = 0;
                    output_ids[O_TC0]->update = 1;
                } break;
                case I_TCT: {
                    p_KEY[11] = 0;
                    output_ids[O_TCT]->update = 1;
                } break;
                case I_POT1: {
                    active[0] = 0;
                    output_ids[O_POT1]->update = 1;
                } break;
                case I_POT2: {
                    active[1] = 0;
                    output_ids[O_POT2]->update = 1;
                } break;
            }
        }
    }
}

void cboard_PICGenios::EvKeyPress(uint key, uint mask) {
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

    if (key == '*') {
        p_KEY[9] = 1;
        output_ids[O_TCA]->update = 1;
    }
    if (key == '0') {
        p_KEY[10] = 1;
        output_ids[O_TC0]->update = 1;
    }
    if (key == '#') {
        p_KEY[11] = 1;
        output_ids[O_TCT]->update = 1;
    }

    if (key == 'Q') {
        p_BT[0] = 0;
        output_ids[O_BRB0]->update = 1;
    }
    if (key == 'W') {
        p_BT[1] = 0;
        output_ids[O_BRB1]->update = 1;
    }
    if (key == 'E') {
        p_BT[2] = 0;
        output_ids[O_BRB2]->update = 1;
    }
    if (key == 'R') {
        p_BT[3] = 0;
        output_ids[O_BRB3]->update = 1;
    }
    if (key == 'T') {
        p_BT[4] = 0;
        output_ids[O_BRB4]->update = 1;
    }
    if (key == 'Y') {
        p_BT[5] = 0;
        output_ids[O_BRB5]->update = 1;
    }
    if (key == 'U') {
        p_BT[6] = 0;
        output_ids[O_BRA5]->update = 1;
    }
}

void cboard_PICGenios::EvKeyRelease(uint key, uint mask) {
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

    if (key == '*') {
        p_KEY[9] = 0;
        output_ids[O_TCA]->update = 1;
    }
    if (key == '0') {
        p_KEY[10] = 0;
        output_ids[O_TC0]->update = 1;
    }
    if (key == '#') {
        p_KEY[11] = 0;
        output_ids[O_TCT]->update = 1;
    }

    if (key == 'Q') {
        p_BT[0] = 1;
        output_ids[O_BRB0]->update = 1;
    }
    if (key == 'W') {
        p_BT[1] = 1;
        output_ids[O_BRB1]->update = 1;
    }
    if (key == 'E') {
        p_BT[2] = 1;
        output_ids[O_BRB2]->update = 1;
    }
    if (key == 'R') {
        p_BT[3] = 1;
        output_ids[O_BRB3]->update = 1;
    }
    if (key == 'T') {
        p_BT[4] = 1;
        output_ids[O_BRB4]->update = 1;
    }
    if (key == 'Y') {
        p_BT[5] = 1;
        output_ids[O_BRB5]->update = 1;
    }
    if (key == 'U') {
        p_BT[6] = 1;
        output_ids[O_BRA5]->update = 1;
    }
}

void cboard_PICGenios::EvOnShow(void) {
    lcd.update = 1;
    board::EvOnShow();
}

unsigned short cboard_PICGenios::GetInputId(char* name) {
    if (strcmp(name, "PB_RST") == 0)
        return I_RST;
    if (strcmp(name, "SW_PWR") == 0)
        return I_PWR;
    if (strcmp(name,
               "PG_"
               "ICSP") == 0)
        return I_ICSP;

    if (strcmp(name, "PB_RB0") == 0)
        return I_RB0;
    if (strcmp(name, "PB_RB1") == 0)
        return I_RB1;
    if (strcmp(name, "PB_RB2") == 0)
        return I_RB2;
    if (strcmp(name, "PB_RB3") == 0)
        return I_RB3;
    if (strcmp(name, "PB_RB4") == 0)
        return I_RB4;
    if (strcmp(name, "PB_RB5") == 0)
        return I_RB5;
    if (strcmp(name, "PB_RA5") == 0)
        return I_RA5;

    if (strcmp(name, "KB_TC1") == 0)
        return I_TC1;
    if (strcmp(name, "KB_TC2") == 0)
        return I_TC2;
    if (strcmp(name, "KB_TC3") == 0)
        return I_TC3;
    if (strcmp(name, "KB_TC4") == 0)
        return I_TC4;
    if (strcmp(name, "KB_TC5") == 0)
        return I_TC5;
    if (strcmp(name, "KB_TC6") == 0)
        return I_TC6;
    if (strcmp(name, "KB_TC7") == 0)
        return I_TC7;
    if (strcmp(name, "KB_TC8") == 0)
        return I_TC8;
    if (strcmp(name, "KB_TC9") == 0)
        return I_TC9;
    if (strcmp(name, "KB_TC*") == 0)
        return I_TCA;
    if (strcmp(name, "KB_TC0") == 0)
        return I_TC0;
    if (strcmp(name, "KB_TC#") == 0)
        return I_TCT;

    if (strcmp(name, "DP_01") == 0)
        return I_D01;
    if (strcmp(name, "DP_02") == 0)
        return I_D02;
    if (strcmp(name, "DP_03") == 0)
        return I_D03;
    if (strcmp(name, "DP_04") == 0)
        return I_D04;
    if (strcmp(name, "DP_05") == 0)
        return I_D05;
    if (strcmp(name, "DP_06") == 0)
        return I_D06;
    if (strcmp(name, "DP_07") == 0)
        return I_D07;
    if (strcmp(name, "DP_08") == 0)
        return I_D08;
    if (strcmp(name, "DP_09") == 0)
        return I_D09;
    if (strcmp(name, "DP_10") == 0)
        return I_D10;
    if (strcmp(name, "DP_11") == 0)
        return I_D11;
    if (strcmp(name, "DP_12") == 0)
        return I_D12;
    if (strcmp(name, "DP_13") == 0)
        return I_D13;
    if (strcmp(name, "DP_14") == 0)
        return I_D14;
    if (strcmp(name, "DP_15") == 0)
        return I_D15;
    if (strcmp(name, "DP_16") == 0)
        return I_D16;
    if (strcmp(name, "DP_17") == 0)
        return I_D17;
    if (strcmp(name, "DP_18") == 0)
        return I_D18;
    if (strcmp(name, "DP_19") == 0)
        return I_D19;
    if (strcmp(name, "DP_20") == 0)
        return I_D20;

    if (strcmp(name, "JP_1") == 0)
        return I_JP1;
    if (strcmp(name,
               "MD_"
               "VIEW") == 0)
        return I_VIEW;

    if (strcmp(name, "PO_1") == 0)
        return I_POT1;
    if (strcmp(name, "PO_2") == 0)
        return I_POT2;

    printf(
        "Error input "
        "'%s' don't "
        "have a valid "
        "id! \n",
        name);
    return INVALID_ID;
}

unsigned short cboard_PICGenios::GetOutputId(char* name) {
    if (strcmp(name, "PB_RST") == 0)
        return O_RST;

    if (strcmp(name, "LD_RB0") == 0)
        return O_RB0;
    if (strcmp(name, "LD_RB1") == 0)
        return O_RB1;
    if (strcmp(name, "LD_RB2") == 0)
        return O_RB2;
    if (strcmp(name, "LD_RB3") == 0)
        return O_RB3;
    if (strcmp(name, "LD_RB4") == 0)
        return O_RB4;
    if (strcmp(name, "LD_RB5") == 0)
        return O_RB5;
    if (strcmp(name, "LD_RB6") == 0)
        return O_RB6;
    if (strcmp(name, "LD_RB7") == 0)
        return O_RB7;

    if (strcmp(name, "LD_RD0") == 0)
        return O_RD0;
    if (strcmp(name, "LD_RD1") == 0)
        return O_RD1;
    if (strcmp(name, "LD_RD2") == 0)
        return O_RD2;
    if (strcmp(name, "LD_RD3") == 0)
        return O_RD3;
    if (strcmp(name, "LD_RD4") == 0)
        return O_RD4;
    if (strcmp(name, "LD_RD5") == 0)
        return O_RD5;
    if (strcmp(name, "LD_RD6") == 0)
        return O_RD6;
    if (strcmp(name, "LD_RD7") == 0)
        return O_RD7;

    if (strcmp(name, "LD_RL1") == 0)
        return O_RL1;
    if (strcmp(name, "LD_RL2") == 0)
        return O_RL2;

    if (strcmp(name, "LD_PRG") == 0)
        return O_PRG;
    if (strcmp(name, "LD_RUN") == 0)
        return O_RUN;

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

    if (strcmp(name, "LD_PWR") == 0)
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

    if (strcmp(name, "DP_01") == 0)
        return O_D01;
    if (strcmp(name, "DP_02") == 0)
        return O_D02;
    if (strcmp(name, "DP_03") == 0)
        return O_D03;
    if (strcmp(name, "DP_04") == 0)
        return O_D04;
    if (strcmp(name, "DP_05") == 0)
        return O_D05;
    if (strcmp(name, "DP_06") == 0)
        return O_D06;
    if (strcmp(name, "DP_07") == 0)
        return O_D07;
    if (strcmp(name, "DP_08") == 0)
        return O_D08;
    if (strcmp(name, "DP_09") == 0)
        return O_D09;
    if (strcmp(name, "DP_10") == 0)
        return O_D10;
    if (strcmp(name, "DP_11") == 0)
        return O_D11;
    if (strcmp(name, "DP_12") == 0)
        return O_D12;
    if (strcmp(name, "DP_13") == 0)
        return O_D13;
    if (strcmp(name, "DP_14") == 0)
        return O_D14;
    if (strcmp(name, "DP_15") == 0)
        return O_D15;
    if (strcmp(name, "DP_16") == 0)
        return O_D16;
    if (strcmp(name, "DP_17") == 0)
        return O_D17;
    if (strcmp(name, "DP_18") == 0)
        return O_D18;
    if (strcmp(name, "DP_19") == 0)
        return O_D19;
    if (strcmp(name, "DP_20") == 0)
        return O_D20;
    if (strcmp(name, "MC_VT") == 0)
        return O_VT;

    if (strcmp(name, "JP_1") == 0)
        return O_JP1;

    if (strcmp(name, "PB_RB0") == 0)
        return O_BRB0;
    if (strcmp(name, "PB_RB1") == 0)
        return O_BRB1;
    if (strcmp(name, "PB_RB2") == 0)
        return O_BRB2;
    if (strcmp(name, "PB_RB3") == 0)
        return O_BRB3;
    if (strcmp(name, "PB_RB4") == 0)
        return O_BRB4;
    if (strcmp(name, "PB_RB5") == 0)
        return O_BRB5;
    if (strcmp(name, "PB_RA5") == 0)
        return O_BRA5;

    if (strcmp(name, "KB_TC1") == 0)
        return O_TC1;
    if (strcmp(name, "KB_TC2") == 0)
        return O_TC2;
    if (strcmp(name, "KB_TC3") == 0)
        return O_TC3;
    if (strcmp(name, "KB_TC4") == 0)
        return O_TC4;
    if (strcmp(name, "KB_TC5") == 0)
        return O_TC5;
    if (strcmp(name, "KB_TC6") == 0)
        return O_TC6;
    if (strcmp(name, "KB_TC7") == 0)
        return O_TC7;
    if (strcmp(name, "KB_TC8") == 0)
        return O_TC8;
    if (strcmp(name, "KB_TC9") == 0)
        return O_TC9;
    if (strcmp(name, "KB_TC*") == 0)
        return O_TCA;
    if (strcmp(name, "KB_TC0") == 0)
        return O_TC0;
    if (strcmp(name, "KB_TC#") == 0)
        return O_TCT;

    if (strcmp(name, "PO_1") == 0)
        return O_POT1;
    if (strcmp(name, "PO_2") == 0)
        return O_POT2;

    if (strcmp(name, "IC_CPU") == 0)
        return O_MP;

    printf(
        "Error output "
        "'%s' don't "
        "have a valid "
        "id! \n",
        name);
    return INVALID_ID;
}

void cboard_PICGenios::RefreshStatus(void) {
    char svalue[128];
    snprintf(svalue, 128, "Temp: %5.2fC", temp[0]);
    PICSimLab.UpdateGUI(TEMP, GT_LABEL, GA_SET_LABEL, (void*)svalue);

    if (pic.serial[0].serialfd != INVALID_SERIAL)
        PICSimLab.UpdateStatus(
            PS_SERIAL, "Serial: " + std::string(SERIALDEVICE) + ":" + std::to_string(pic.serial[0].serialbaud) + "(" +
                           FloatStrFormat("%4"
                                          ".1"
                                          "f",
                                          fabs((100.0 * pic.serial[0].serialexbaud - 100.0 * pic.serial[0].serialbaud) /
                                               pic.serial[0].serialexbaud)) +
                           "%)");
    else
        PICSimLab.UpdateStatus(PS_SERIAL, "Serial: " + std::string(SERIALDEVICE) +
                                              " ("
                                              "ERROR"
                                              ")");
}

void cboard_PICGenios::WritePreferences(void) {
    char line[128];
    PICSimLab.SavePrefs(
        "PICGenios_"
        "proc",
        Proc);

    PICSimLab.SavePrefs("PICGenios_jmp", std::to_string(jmp[0]));

    PICSimLab.UpdateGUI(LCD_TYPE, GT_COMBO, GA_GET, (void*)line);
    if (line[0]) {
        PICSimLab.SavePrefs(
            "PICGen"
            "ios_"
            "lcd",
            line);
    }

    line[0] = 0;
    for (int i = 0; i < 20; i++)
        sprintf(line + i, "%i", dip[i]);

    PICSimLab.SavePrefs("PICGenios_dip", line);
    PICSimLab.SavePrefs(
        "PICGenios_"
        "clock",
        FloatStrFormat("%2.1f", PICSimLab.GetClock()));

    PICSimLab.SavePrefs(
        "PICGenios_"
        "pot1",
        std::to_string(pot[0]));
    PICSimLab.SavePrefs(
        "PICGenios_"
        "pot2",
        std::to_string(pot[1]));
}

void cboard_PICGenios::ReadPreferences(char* name, char* value) {
    int i;

    if (!strcmp(name,
                "PICGenios_"
                "proc")) {
        Proc = value;
    }

    if (!strcmp(name,
                "PICGenios_"
                "jmp")) {
        for (i = 0; i < 1; i++) {
            if (value[i] == '0')
                jmp[i] = 0;
            else
                jmp[i] = 1;
        }
    }

    if (!strcmp(name,
                "PICGenios_"
                "dip")) {
        for (i = 0; i < 20; i++) {
            if (value[i] == '0')
                dip[i] = 0;
            else
                dip[i] = 1;
        }
    }

    if (!strcmp(name,
                "PICGenios_"
                "lcd")) {
        PICSimLab.UpdateGUI(LCD_TYPE, GT_COMBO, GA_SET, (void*)value);

        if (!strcmp(value,
                    "hd4478"
                    "0 "
                    "16x"
                    "2")) {
            lcd_end(&lcd);
            lcd_init(&lcd, 16, 2, this);
        } else {
            lcd_end(&lcd);
            lcd_init(&lcd, 16, 4, this);
        }
    }

    if (!strcmp(name,
                "PICGenios_"
                "clock")) {
        PICSimLab.SetClock(atof(value));
    }

    if (!strcmp(name,
                "PICGenios_"
                "pot1")) {
        pot[0] = atoi(value);
    }

    if (!strcmp(name,
                "PICGenios_"
                "pot2")) {
        pot[1] = atoi(value);
    }
}

// Change lcd

void cboard_PICGenios::board_Event(CControl* control) {
    char text[128] = "";
    PICSimLab.UpdateGUI(LCD_TYPE, GT_COMBO, GA_GET, (void*)text);

    if (strcmp(text,
               "hd44780 "
               "16x2") == 0) {
        lcd_end(&lcd);
        lcd_init(&lcd, 16, 2, this);
    } else if (strcmp(text,
                      "hd44780 "
                      "16x4") == 0) {
        lcd_end(&lcd);
        lcd_init(&lcd, 16, 4, this);
    } else if (strcmp(text,
                      "hd44780 "
                      "20x2") == 0) {
        lcd_end(&lcd);
        lcd_init(&lcd, 20, 2, this);
    } else if (strcmp(text,
                      "hd44780 "
                      "20x4") == 0) {
        lcd_end(&lcd);
        lcd_init(&lcd, 20, 4, this);
    }
}

board_init(BOARD_PICGenios_Name, cboard_PICGenios);
