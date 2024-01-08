/* ########################################################################

   PICSimLab - Programmable IC Simulator Laboratory

   ########################################################################

   Copyright (c) : 2010-2023  Luis Claudio Gambôa Lopes <lcgamboa@yahoo.com>

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

#include "board_K16F.h"
#include "../lib/oscilloscope.h"
#include "../lib/picsimlab.h"
#include "../lib/spareparts.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

/* outputs */
enum {
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
    O_RST,
    O_RA1,
    O_RA2,
    O_RA6,
    O_RA7,
    O_LPWR,
    O_LCD,
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
    I_TCA,
    I_TC0,
    I_TCT,
    I_RST,
    I_PWR,
    I_ICSP,
    I_VIEW
};

cboard_K16F::cboard_K16F(void) : font(10, lxFONTFAMILY_TELETYPE, lxFONTSTYLE_NORMAL, lxFONTWEIGHT_BOLD) {
    Proc = "PIC16F628A";

    clko = 0;
    d = 0;
    lcde = 0;

    lcd_init(&lcd, 16, 2, this);
    mi2c_init(&mi2c, 512);
    rtc_pfc8563_init(&rtc, this);
    ReadMaps();

    snprintf(mi2c_tmp_name, 200, "%s/picsimlab-XXXXXX", (const char*)lxGetTempDir("PICSimLab").c_str());
    close(mkstemp(mi2c_tmp_name));
    unlink(mi2c_tmp_name);
    strncat(mi2c_tmp_name, ".txt", 200);
}

cboard_K16F::~cboard_K16F(void) {
    mi2c_end(&mi2c);
    lcd_end(&lcd);
    rtc_pfc8563_end(&rtc);
    unlink(mi2c_tmp_name);
}

int cboard_K16F::MInit(const char* processor, const char* fname, float freq) {
    char fnamem[1024];
    FILE* fout;

    strncpy(fnamem, (const char*)dirname(fname).c_str(), 1023);
    strncat(fnamem, "/mdump_K16F_EEPROM.bin", 1023);

    fout = fopen(fnamem, "rb");
    if (fout) {
        fread(mi2c.data, mi2c.SIZE, 1, fout);
        fclose(fout);
    } else {
        printf("Error loading from file: %s \n", fnamem);
    }
    return bsim_picsim::MInit(processor, fname, freq);
}

void cboard_K16F::MDumpMemory(const char* mfname) {
    FILE* fout;
    char fname[1024];

    strncpy(fname, (const char*)dirname(mfname).c_str(), 1023);
    strncat(fname, "/mdump_K16F_EEPROM.bin", 1023);

    fout = fopen(fname, "wb");
    if (fout) {
        fwrite(mi2c.data, mi2c.SIZE, 1, fout);
        fclose(fout);
    } else {
        printf("Error saving to file: %s \n", fname);
    }

    bsim_picsim::MDumpMemory(mfname);
}

void cboard_K16F::Draw(CDraw* draw) {
    int i;
    int update = 0;  // verifiy if updated is needed

    // lab2 draw
    for (i = 0; i < outputc; i++) {
        if (output[i].update)  // only if need update
        {
            output[i].update = 0;

            if (!update) {
                draw->Canvas.Init(Scale, Scale);
            }
            update++;  // set to update buffer

            if (!output[i].r)  // rectangle
            {
                draw->Canvas.SetFgColor(30, 0, 0);

                switch (output[i].id) {
                    case O_LCD:
                        draw->Canvas.SetColor(0, 90 * PICSimLab.GetMcuPwr() + 40, 0);
                        break;
                    case O_RST:
                        draw->Canvas.SetColor(100, 100, 100);
                        break;
                    case O_MP:
                        draw->Canvas.SetColor(26, 26, 26);
                        break;
                }

                // draw lcd text

                if (output[i].id == O_LCD) {
                    if (lcd.update) {
                        draw->Canvas.Rectangle(1, output[i].x1 - 1, output[i].y1 - 1, output[i].x2 - output[i].x1 + 2,
                                               output[i].y2 - output[i].y1 + 3);
                        lcd_draw(&lcd, &draw->Canvas, output[i].x1, output[i].y1, output[i].x2 - output[i].x1,
                                 output[i].y2 - output[i].y1, PICSimLab.GetMcuPwr());
                    }
                } else if (output[i].id == O_RST) {
                    draw->Canvas.Circle(1, output[i].cx, output[i].cy, 11);
                    if (p_RST) {
                        draw->Canvas.SetColor(15, 15, 15);
                    } else {
                        draw->Canvas.SetColor(55, 55, 55);
                    }
                    draw->Canvas.Circle(1, output[i].cx, output[i].cy, 9);
                } else if (output[i].id == O_MP) {
                    draw->Canvas.SetFont(font);
                    draw->Canvas.Rectangle(1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1,
                                           output[i].y2 - output[i].y1);
                    draw->Canvas.SetColor(230, 230, 230);
                    draw->Canvas.RotatedText(Proc, output[i].x2, output[i].y1 + 5, -90);
                } else if (output[i].id <= O_TCT) {
                    if (p_KEY[output[i].id - O_TC1]) {
                        draw->Canvas.SetLineWidth(4);
                        draw->Canvas.SetColor(255, 0, 0);
                    } else {
                        draw->Canvas.SetLineWidth(6);
                        draw->Canvas.SetColor(249, 249, 249);
                    }

                    draw->Canvas.Rectangle(0, output[i].x1 + 3, output[i].y1 + 3, output[i].x2 - output[i].x1 - 6,
                                           output[i].y2 - output[i].y1 - 6);
                    draw->Canvas.SetLineWidth(1);
                } else {
                    draw->Canvas.Rectangle(1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1,
                                           output[i].y2 - output[i].y1);
                }
            } else  // circle
            {
                draw->Canvas.SetFgColor(0, 0, 0);

                switch (output[i].id) {
                    case O_RA1:
                        draw->Canvas.SetBgColor(pic.pins[17].oavalue, 0, 0);
                        break;
                    case O_RA2:
                        draw->Canvas.SetBgColor(pic.pins[0].oavalue, 0, 0);
                        break;
                    case O_RA6:
                        draw->Canvas.SetBgColor(pic.pins[14].oavalue, 0, 0);
                        break;
                    case O_RA7:
                        draw->Canvas.SetBgColor(pic.pins[15].oavalue, 0, 0);
                        break;
                    case O_LPWR:
                        draw->Canvas.SetBgColor(0, 200 * PICSimLab.GetMcuPwr() + 55, 0);
                        break;
                }

                // draw a LED
                color1 = draw->Canvas.GetBgColor();
                int r = color1.Red() - 120;
                int g = color1.Green() - 120;
                int b = color1.Blue() - 120;
                if (r < 0)
                    r = 0;
                if (g < 0)
                    g = 0;
                if (b < 0)
                    b = 0;
                color2.Set(r, g, b);
                draw->Canvas.SetBgColor(color2);
                draw->Canvas.Circle(1, output[i].x1, output[i].y1, output[i].r + 1);
                draw->Canvas.SetBgColor(color1);
                draw->Canvas.Circle(1, output[i].x1, output[i].y1, output[i].r - 2);
            }
        }
    }

    // end draw
    if (update) {
        draw->Canvas.End();
        draw->Update();
    }
}

void cboard_K16F::Run_CPU(void) {
    int i;
    int j;
    unsigned char pi;
    const picpin* pins;
    unsigned int alm[18];  // luminosidade media

    const int JUMPSTEPS = PICSimLab.GetJUMPSTEPS();
    const long int NSTEP = PICSimLab.GetNSTEP();
    const float RNSTEP = 200.0 * pic.PINCOUNT / NSTEP;

    pins = pic.pins;

    memset(alm, 0, 18 * sizeof(unsigned int));

    if (use_spare)
        SpareParts.PreProcess();

    j = JUMPSTEPS;
    pi = 0;
    if (PICSimLab.GetMcuPwr())
        for (i = 0; i < NSTEP; i++) {
            if (j >= JUMPSTEPS) {
                pic_set_pin(&pic, pic.mclr, p_RST);

                pic_set_pin(&pic, 18, 0);
                pic_set_pin(&pic, 1, 0);
                pic_set_pin(&pic, 15, 0);
                pic_set_pin(&pic, 16, 0);
                pic_set_pin(&pic, 13, 0);
                pic_set_pin(&pic, 12, 0);
                pic_set_pin(&pic, 11, 0);
            }

            // keyboard

            if (p_KEY[0]) {
                pic_set_pin(&pic, 18, pic_get_pin(&pic, 13));
                pic_set_pin(&pic, 13, pic_get_pin(&pic, 18));
            }

            if (p_KEY[1]) {
                pic_set_pin(&pic, 18, pic_get_pin(&pic, 12));
                pic_set_pin(&pic, 12, pic_get_pin(&pic, 18));
            }

            if (p_KEY[2]) {
                pic_set_pin(&pic, 18, pic_get_pin(&pic, 11));
                pic_set_pin(&pic, 11, pic_get_pin(&pic, 18));
            }

            if (p_KEY[3]) {
                pic_set_pin(&pic, 1, pic_get_pin(&pic, 13));
                pic_set_pin(&pic, 13, pic_get_pin(&pic, 1));
            }

            if (p_KEY[4]) {
                pic_set_pin(&pic, 1, pic_get_pin(&pic, 12));
                pic_set_pin(&pic, 12, pic_get_pin(&pic, 1));
            }

            if (p_KEY[5]) {
                pic_set_pin(&pic, 1, pic_get_pin(&pic, 11));
                pic_set_pin(&pic, 11, pic_get_pin(&pic, 1));
            }

            if (p_KEY[6]) {
                pic_set_pin(&pic, 15, pic_get_pin(&pic, 13));
                pic_set_pin(&pic, 13, pic_get_pin(&pic, 15));
            }

            if (p_KEY[7]) {
                pic_set_pin(&pic, 15, pic_get_pin(&pic, 12));
                pic_set_pin(&pic, 12, pic_get_pin(&pic, 15));
            }

            if (p_KEY[8]) {
                pic_set_pin(&pic, 15, pic_get_pin(&pic, 11));
                pic_set_pin(&pic, 11, pic_get_pin(&pic, 15));
            }

            if (p_KEY[9]) {
                pic_set_pin(&pic, 16, pic_get_pin(&pic, 13));
                pic_set_pin(&pic, 13, pic_get_pin(&pic, 16));
            }

            if (p_KEY[10]) {
                pic_set_pin(&pic, 16, pic_get_pin(&pic, 12));
                pic_set_pin(&pic, 12, pic_get_pin(&pic, 16));
            }

            if (p_KEY[11]) {
                pic_set_pin(&pic, 16, pic_get_pin(&pic, 11));
                pic_set_pin(&pic, 11, pic_get_pin(&pic, 16));
            }

            if (!mplabxd_testbp())
                pic_step(&pic);
            ioupdated = pic.ioupdated;
            InstCounterInc();
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
                j = -1;
            }
            j++;

            if (ioupdated) {
                // serial lcd display code
                if ((pins[9].value) && (!clko)) {
                    d = (d << 1) | pins[8].value;
                }

                clko = pins[9].value;

                if ((!pins[16].dir) && (!pins[16].value)) {
                    if (!lcde) {
                        if ((!pins[8].dir) && (!pins[8].value)) {
                            lcd_cmd(&lcd, d);
                        } else if ((!pins[8].dir) && (pins[8].value)) {
                            lcd_data(&lcd, d);
                        }
                        lcde = 1;
                    }
                } else {
                    lcde = 0;
                }

                // i2c code
                if (pins[2].dir) {
                    sda = 1;
                } else {
                    sda = pins[2].value;
                }

                if (pins[1].dir) {
                    sck = 1;
                    pic_set_pin(&pic, 2, 1);
                } else {
                    sck = pins[1].value;
                }
                pic_set_pin(&pic, 3, mi2c_io(&mi2c, sck, sda) & rtc_pfc8563_I2C_io(&rtc, sck, sda));
            }
            pic.ioupdated = 0;
        }
    // fim STEP

    for (pi = 0; pi < pic.PINCOUNT; pi++) {
        pic.pins[pi].oavalue = (int)((alm[pi] * RNSTEP) + 55);
    }

    if (use_spare)
        SpareParts.PostProcess();

    if (lcd.update)
        output_ids[O_LCD]->update = 1;

    // verifiy if LEDS need update
    if (output_ids[O_RA1]->value != pic.pins[17].oavalue) {
        output_ids[O_RA1]->value = pic.pins[17].oavalue;
        output_ids[O_RA1]->update = 1;
    }
    if (output_ids[O_RA2]->value != pic.pins[0].oavalue) {
        output_ids[O_RA2]->value = pic.pins[0].oavalue;
        output_ids[O_RA2]->update = 1;
    }
    if (output_ids[O_RA6]->value != pic.pins[14].oavalue) {
        output_ids[O_RA6]->value = pic.pins[14].oavalue;
        output_ids[O_RA6]->update = 1;
    }
    if (output_ids[O_RA7]->value != pic.pins[15].oavalue) {
        output_ids[O_RA7]->value = pic.pins[15].oavalue;
        output_ids[O_RA7]->update = 1;
    }
}

void cboard_K16F::Reset(void) {
    pic_reset(&pic, 1);

    lcd_rst(&lcd);
    mi2c_rst(&mi2c);
    rtc_pfc8563_rst(&rtc);
    pic_set_pin_DOV(&pic, 18, 0);
    pic_set_pin_DOV(&pic, 1, 0);
    pic_set_pin_DOV(&pic, 15, 0);
    pic_set_pin_DOV(&pic, 16, 0);

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

    pic_set_pin(&pic, 13, 0);
    pic_set_pin(&pic, 12, 0);
    pic_set_pin(&pic, 11, 0);

    if (pic.serial[0].serialfd != INVALID_SERIAL)
        PICSimLab.UpdateStatus(PS_SERIAL, lxT("Serial: ") + lxString(SERIALDEVICE) + lxT(":") +
                                      itoa(pic.serial[0].serialbaud) + lxT("(") +
                                      lxString().Format("%4.1f", fabs((100.0 * pic.serial[0].serialexbaud -
                                                                       100.0 * pic.serial[0].serialbaud) /
                                                                      pic.serial[0].serialexbaud)) +
                                      lxT("%)"));
    else
        PICSimLab.UpdateStatus(PS_SERIAL, lxT("Serial: ") + lxString(SERIALDEVICE) + lxT(" (ERROR)"));

    for (int pi = 0; pi < pic.PINCOUNT; pi++) {
        pic.pins[pi].oavalue = 0;
    }

    if (use_spare)
        SpareParts.Reset();

    RegisterRemoteControl();
}

void cboard_K16F::RegisterRemoteControl(void) {
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

    output_ids[O_RA1]->status = &pic.pins[17].oavalue;
    output_ids[O_RA2]->status = &pic.pins[0].oavalue;
    output_ids[O_RA6]->status = &pic.pins[14].oavalue;
    output_ids[O_RA7]->status = &pic.pins[15].oavalue;
    output_ids[O_LCD]->status = &lcd;
}

void cboard_K16F::EvMouseButtonPress(uint button, uint x, uint y, uint state) {
    int i;

    for (i = 0; i < inputc; i++) {
        if (((input[i].x1 <= x) && (input[i].x2 >= x)) && ((input[i].y1 <= y) && (input[i].y2 >= y))) {
            switch (input[i].id) {
                case I_ICSP: {
                    PICSimLab.OpenLoadHexFileDialog();
                    ;
                } break;

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
                case I_VIEW:
                    FILE* fout;
                    fout = fopen(mi2c_tmp_name, "w");
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
                        lxLaunchDefaultApplication(mi2c_tmp_name);
#endif
                    } else {
                        printf("Error saving to file: %s \n", mi2c_tmp_name);
                    }
                    break;
            }
        }
    }
}

void cboard_K16F::EvMouseButtonRelease(uint button, uint x, uint y, uint state) {
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
                case I_TC4: {
                    p_KEY[3] = 0;
                    output_ids[O_TC4]->update = 1;
                } break;
                case I_TC7: {
                    p_KEY[6] = 0;
                    output_ids[O_TC7]->update = 1;
                } break;
                case I_TCA: {
                    p_KEY[9] = 0;
                    output_ids[O_TCA]->update = 1;
                } break;
                case I_TC2: {
                    p_KEY[1] = 0;
                    output_ids[O_TC2]->update = 1;
                } break;
                case I_TC5: {
                    p_KEY[4] = 0;
                    output_ids[O_TC5]->update = 1;
                } break;
                case I_TC8: {
                    p_KEY[7] = 0;
                    output_ids[O_TC8]->update = 1;
                } break;
                case I_TC0: {
                    p_KEY[10] = 0;
                    output_ids[O_TC0]->update = 1;
                } break;
                case I_TC3: {
                    p_KEY[2] = 0;
                    output_ids[O_TC3]->update = 1;
                } break;
                case I_TC6: {
                    p_KEY[5] = 0;
                    output_ids[O_TC6]->update = 1;
                }
                case I_TC9: {
                    p_KEY[8] = 0;
                    output_ids[O_TC9]->update = 1;
                }
                case I_TCT: {
                    p_KEY[11] = 0;
                    output_ids[O_TCT]->update = 1;
                } break;
            }
        }
    }
}

void cboard_K16F::EvKeyPress(uint key, uint mask) {
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
}

void cboard_K16F::EvKeyRelease(uint key, uint mask) {
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
}

void cboard_K16F::EvOnShow(void) {
    lcd.update = 1;
    board::EvOnShow();
}

unsigned short cboard_K16F::GetInputId(char* name) {
    if (strcmp(name, "PB_RST") == 0)
        return I_RST;
    if (strcmp(name, "SW_PWR") == 0)
        return I_PWR;
    if (strcmp(name, "PG_ICSP") == 0)
        return I_ICSP;
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

    if (strcmp(name, "MD_VIEW") == 0)
        return I_VIEW;

    printf("Error input '%s' don't have a valid id! \n", name);
    return INVALID_ID;
}

unsigned short cboard_K16F::GetOutputId(char* name) {
    if (strcmp(name, "LD_RA1") == 0)
        return O_RA1;
    if (strcmp(name, "LD_RA2") == 0)
        return O_RA2;
    if (strcmp(name, "LD_RA6") == 0)
        return O_RA6;
    if (strcmp(name, "LD_RA7") == 0)
        return O_RA7;
    if (strcmp(name, "LD_LPWR") == 0)
        return O_LPWR;
    if (strcmp(name, "DS_LCD") == 0)
        return O_LCD;
    if (strcmp(name, "PB_RST") == 0)
        return O_RST;

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

    if (strcmp(name, "IC_CPU") == 0)
        return O_MP;

    printf("Error output '%s' don't have a valid id! \n", name);
    return INVALID_ID;
}

void cboard_K16F::WritePreferences(void) {
    PICSimLab.SavePrefs(lxT("K16F_proc"), Proc);
    PICSimLab.SavePrefs(lxT("K16F_clock"), lxString().Format("%2.1f", PICSimLab.GetClock()));
}

void cboard_K16F::ReadPreferences(char* name, char* value) {
    if (!strcmp(name, "K16F_proc")) {
        Proc = value;
    }

    if (!strcmp(name, "K16F_clock")) {
        PICSimLab.SetClock(atof(value));
    }
}

void cboard_K16F::RefreshStatus(void) {
    if (pic.serial[0].serialfd != INVALID_SERIAL)
        PICSimLab.UpdateStatus(PS_SERIAL, lxT("Serial: ") + lxString(SERIALDEVICE) + lxT(":") +
                                      itoa(pic.serial[0].serialbaud) + lxT("(") +
                                      lxString().Format("%4.1f", fabs((100.0 * pic.serial[0].serialexbaud -
                                                                       100.0 * pic.serial[0].serialbaud) /
                                                                      pic.serial[0].serialexbaud)) +
                                      lxT("%)"));
    else
        PICSimLab.UpdateStatus(PS_SERIAL, lxT("Serial: ") + lxString(SERIALDEVICE) + lxT(" (ERROR)"));
}

board_init(BOARD_K16F_Name, cboard_K16F);
