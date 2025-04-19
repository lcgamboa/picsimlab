/* ########################################################################

   PICSimLab - Programmable IC Simulator Laboratory

   ########################################################################

   Copyright (c) : 2010-2025  Luis Claudio Gambôa Lopes <lcgamboa@yahoo.com>

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

// Oscilloscope

#include "picsimlab4.h"
#include "lib/oscilloscope.h"
#include "lib/picsimlab.h"
#include "lib/spareparts.h"
#include "picsimlab1.h"

#include "picsimlab4_d.cc"

CPWindow4 Window4;

// Implementation

void CPWindow4::DrawScreen(void) {
    double xz = Oscilloscope.Getxz();

    draw1.Canvas.Init();
    draw1.Canvas.SetFontSize(9);
    draw1.Canvas.SetFontWeight(lxFONTWEIGHT_BOLD);

    // Clear background
    draw1.Canvas.SetFgColor(0, 0, 0);
    draw1.Canvas.SetBgColor(0, 0, 0);
    draw1.Canvas.Rectangle(1, 0, 0, WMAX + 80, HMAX + 30);

    // draw grid lines
    draw1.Canvas.SetFgColor(50, 50, 50);

    int inch = HMAX / 10;
    for (int i = 1; i < 10; i++)
        draw1.Canvas.Line(0, inch * i, WMAX, inch * i);

    int incw = WMAX / 10;
    for (int i = 1; i < 10; i++)
        draw1.Canvas.Line(incw * i, 0, incw * i, HMAX);

    draw1.Canvas.SetFgColor(140, 140, 140);
    draw1.Canvas.Line(0, HMAX / 2, WMAX, HMAX / 2);
    draw1.Canvas.Line(WMAX / 2, 0, WMAX / 2, HMAX);

    float gain[3], nivel[3];
    lxPoint pts[3];

    // draw ch 0
    if (togglebutton1.GetCheck()) {
        gain[0] = (HMAX / spind1.GetValue()) / 10.0;
        nivel[0] = (gain[0] * -spind2.GetValue()) + (HMAX / 2.0);
        if (togglebutton3.GetCheck())
            gain[0] = -gain[0];
        draw1.Canvas.SetFgColor(button1.GetColor());

        pts[0].x = 0;
        pts[0].y = nivel[0] - 8;
        pts[1].x = 8;
        pts[1].y = nivel[0];
        pts[2].x = 0;
        pts[2].y = nivel[0] + 8;
        draw1.Canvas.Polygon(1, pts, 3);

        draw1.Canvas.SetLineWidth(2);
        for (int t = 0; t < (NPOINTS / 2) - 1; t++) {
            draw1.Canvas.Line((t * xz + xz) - ((NPOINTS * (xz - 1.0)) / 4.0),
                              gain[0] * Oscilloscope.GetChannel(0)[t] + nivel[0],
                              ((t + 1) * xz + xz) - ((NPOINTS * (xz - 1.0)) / 4.0),
                              gain[0] * Oscilloscope.GetChannel(0)[t + 1] + nivel[0]);
        }
    }
    draw1.Canvas.SetLineWidth(1);

    // draw ch 1
    if (togglebutton2.GetCheck()) {
        gain[1] = (HMAX / spind3.GetValue()) / 10.0;
        nivel[1] = (gain[1] * -spind4.GetValue()) + (HMAX / 2.0);
        if (togglebutton4.GetCheck())
            gain[1] = -gain[1];
        draw1.Canvas.SetFgColor(button2.GetColor());

        pts[0].x = 0;
        pts[0].y = nivel[1] - 5;
        pts[1].x = 5;
        pts[1].y = nivel[1];
        pts[2].x = 0;
        pts[2].y = nivel[1] + 5;
        draw1.Canvas.Polygon(1, pts, 3);

        draw1.Canvas.SetLineWidth(2);
        for (int t = 0; t < (NPOINTS / 2) - 1; t++) {
            draw1.Canvas.Line((t * xz + xz) - ((NPOINTS * (xz - 1.0)) / 4.0),
                              gain[1] * Oscilloscope.GetChannel(1)[t] + nivel[1],
                              ((t + 1) * xz + xz) - ((NPOINTS * (xz - 1.0)) / 4.0),
                              gain[1] * Oscilloscope.GetChannel(1)[t + 1] + nivel[1]);
        }
    }
    draw1.Canvas.SetLineWidth(1);

    // draw update cursor
    draw1.Canvas.SetFgColor(250, 250, 50);
    draw1.Canvas.Line(update_pos, 0, update_pos, HMAX);

    // draw trigger level
    if (Oscilloscope.GetUseTrigger()) {
        if (combo1.GetText().compare("1") == 0)
            Oscilloscope.SetTriggerChannel(0);
        else
            Oscilloscope.SetTriggerChannel(1);

        draw1.Canvas.SetFgColor(255, 255, 0);
        nivel[2] =
            (gain[Oscilloscope.GetTriggerChannel()] * -spind7.GetValue()) + nivel[Oscilloscope.GetTriggerChannel()];
        pts[0].x = 0;
        pts[0].y = nivel[2] - 3;
        pts[1].x = 3;
        pts[1].y = nivel[2];
        pts[2].x = 0;
        pts[2].y = nivel[2] + 3;
        draw1.Canvas.Polygon(1, pts, 3);
    }

    // draw toffset level
    draw1.Canvas.SetFgColor(255, 255, 0);
    nivel[2] = ((WMAX - Oscilloscope.GetSampleOffset()) * xz + xz) - ((NPOINTS * (xz - 1.0)) / 4.0);
    pts[0].y = 1;
    pts[0].x = nivel[2] - 3;
    pts[1].y = 1 + 3;
    pts[1].x = nivel[2];
    pts[2].y = 1;
    pts[2].x = nivel[2] + 3;
    draw1.Canvas.Polygon(1, pts, 3);

    // draw text info
    char text[100];
    draw1.Canvas.SetFgColor(0, 0, 0);
    draw1.Canvas.SetBgColor(0, 0, 0);
    draw1.Canvas.Rectangle(1, 0, HMAX, WMAX, HMAX + 30);

    draw1.Canvas.SetFgColor(button1.GetColor());
    if (togglebutton1.GetCheck()) {
        snprintf(text, 100, "CH1 %6.2fV %6.2fV %c", spind1.GetValue(), spind2.GetValue(),
                 ((togglebutton3.GetCheck() == 1) ? 'I' : 'N'));
    } else {
        snprintf(text, 100, "CH1 Off");
    }
    draw1.Canvas.RotatedText(text, 10, HMAX + 1, 0);

    draw1.Canvas.SetFgColor(button2.GetColor());
    if (togglebutton2.GetCheck()) {
        snprintf(text, 100, "CH2 %6.2fV %6.2fV %c", spind3.GetValue(), spind4.GetValue(),
                 ((togglebutton4.GetCheck() == 1) ? 'I' : 'N'));
    } else {
        snprintf(text, 100, "CH2 Off");
    }
    draw1.Canvas.RotatedText(text, 10, HMAX + 14, 0);

    draw1.Canvas.SetFgColor(200, 200, 200);
    snprintf(text, 100, "TM %6.3fms  %6.3fms", spind5.GetValue(), spind6.GetValue());
    draw1.Canvas.RotatedText(text, WMAX / 2, HMAX + 1, 0);

    draw1.Canvas.SetFgColor(200, 200, 0);
    if (togglebutton5.GetCheck()) {
        snprintf(text, 100, "TRG CH%s %6.2fV ", (const char*)combo1.GetText().c_str(), spind7.GetValue());
    } else {
        snprintf(text, 100, "TRG Off");
    }
    draw1.Canvas.RotatedText(text, WMAX / 2, HMAX + 14, 0);

    char text1[100];
    char text2[100];
    const int x = WMAX + 2;
    int y;

    for (int i = 0; i < 5; i++) {
        y = (56 * i);

        // Clear background
        draw1.Canvas.SetFgColor(200, 200, 200);
        draw1.Canvas.SetBgColor(0, 0, 0);
        draw1.Canvas.Rectangle(1, x, y, 77, 55);

        switch (Oscilloscope.GetMeasures(i)) {
            case 1:
                strcpy(text, "Vmax");
                snprintf(text1, 100, "%7.3f V", Oscilloscope.GetChannelStatus(0).Vmax);
                snprintf(text2, 100, "%7.3f V", Oscilloscope.GetChannelStatus(1).Vmax);
                break;
            case 2:
                strcpy(text, "Vmin");
                snprintf(text1, 100, "%7.3f V", Oscilloscope.GetChannelStatus(0).Vmin);
                snprintf(text2, 100, "%7.3f V", Oscilloscope.GetChannelStatus(1).Vmin);
                break;
            case 3:
                strcpy(text, "Vavg");
                snprintf(text1, 100, "%7.3f V", Oscilloscope.GetChannelStatus(0).Vavr);
                snprintf(text2, 100, "%7.3f V", Oscilloscope.GetChannelStatus(1).Vavr);
                break;
            case 4:
                strcpy(text, "Vrms");
                snprintf(text1, 100, "%7.3f V", Oscilloscope.GetChannelStatus(0).Vrms);
                snprintf(text2, 100, "%7.3f V", Oscilloscope.GetChannelStatus(1).Vrms);
                break;
            case 5:
                strcpy(text, "Frequency");
                snprintf(text1, 100, "%7.0f Hz", Oscilloscope.GetChannelStatus(0).Freq);
                snprintf(text2, 100, "%7.0f Hz", Oscilloscope.GetChannelStatus(1).Freq);
                break;
            case 6:
                strcpy(text, "Duty cycle");
                snprintf(text1, 100, "%7.0f %%", Oscilloscope.GetChannelStatus(0).Duty);
                snprintf(text2, 100, "%7.0f %%", Oscilloscope.GetChannelStatus(1).Duty);
                break;
            case 7:
                strcpy(text, "Pos. cycle");
                snprintf(text1, 100, "%7.3f ms", Oscilloscope.GetChannelStatus(0).PCycle_ms);
                snprintf(text2, 100, "%7.3f ms", Oscilloscope.GetChannelStatus(1).PCycle_ms);
                break;
            case 8:
                strcpy(text, "Neg. cycle");
                snprintf(text1, 100, "%7.3f ms",
                         Oscilloscope.GetChannelStatus(0).FCycle_ms - Oscilloscope.GetChannelStatus(0).PCycle_ms);
                snprintf(text2, 100, "%7.3f ms",
                         Oscilloscope.GetChannelStatus(1).FCycle_ms - Oscilloscope.GetChannelStatus(1).PCycle_ms);
                break;
            case 9:
                strcpy(text, "Full cycle");
                snprintf(text1, 100, "%7.3f ms", Oscilloscope.GetChannelStatus(0).FCycle_ms);
                snprintf(text2, 100, "%7.3f ms", Oscilloscope.GetChannelStatus(1).FCycle_ms);
                break;
            default:
                strcpy(text, "");
                strcpy(text1, "");
                strcpy(text2, "");
        }

        if (strlen(text) > 0) {
            draw1.Canvas.SetFgColor(200, 200, 200);
            draw1.Canvas.RotatedText(text, x + 4, y + 4, 0);
            draw1.Canvas.SetFgColor(button1.GetColor());
            draw1.Canvas.RotatedText(text1, x + 2, y + 20, 0);
            draw1.Canvas.SetFgColor(button2.GetColor());
            draw1.Canvas.RotatedText(text2, x + 2, y + 34, 0);
        }
    }

    draw1.Canvas.End();
}

void CPWindow4::button1_EvMouseButtonClick(CControl* control, unsigned int button, unsigned int x, unsigned int y,
                                           unsigned int state) {
#ifndef __WXX11__
    colordialog1.SetColor(button1.GetColor());
    ctrl = &button1;
    colordialog1.Run();
#endif
}

void CPWindow4::button2_EvMouseButtonClick(CControl* control, unsigned int button, unsigned int x, unsigned int y,
                                           unsigned int state) {
#ifndef __WXX11__
    colordialog1.SetColor(button2.GetColor());
    ctrl = &button2;
    colordialog1.Run();
#endif
}

void CPWindow4::draw1_EvMouseButtonClick(CControl* control, unsigned int button, unsigned int x, unsigned int y,
                                         unsigned int state) {
    // code here:)
    mprint("draw1_EvMouseButtonClick\n");
}

void CPWindow4::spind5_EvOnChangeSpinDouble(CControl* control) {
    spind6.SetMin(-5 * spind5.GetValue());
    spind6.SetMax(5 * spind5.GetValue());

    float inc = spind5.GetValue() / 100.0;

    if (inc < 0.001000) {
        spind5.SetInc(0.001000);
    } else if (inc >= 1) {
        spind5.SetInc(5);
    } else if (inc >= 0.1) {
        spind5.SetInc(0.5);
    } else {
        spind5.SetInc(0.005);
    }

    Oscilloscope.SetTimeScaleAndOffset(spind5.GetValue(), spind6.GetValue());
}

void CPWindow4::spind6_EvOnChangeSpinDouble(CControl* control) {
    float inc = fabs(spind6.GetValue() / 100.0);

    if (inc < 0.001000) {
        spind6.SetInc(0.001000);
    } else if (inc >= 1) {
        spind6.SetInc(5);
    } else if (inc >= 0.1) {
        spind6.SetInc(0.5);
    } else {
        spind6.SetInc(0.005);
    }

    Oscilloscope.SetTimeScaleAndOffset(spind5.GetValue(), spind6.GetValue());
}

void CPWindow4::togglebutton5_EvOnToggleButton(CControl* control) {
    Oscilloscope.SetUseTrigger(togglebutton5.GetCheck());
}

void CPWindow4::spind7_EvOnChangeSpinDouble(CControl* control) {
    Oscilloscope.SetTriggerLevel(spind7.GetValue());
}

void CPWindow4::timer1_EvOnTime(CControl* control) {
    static int count = 0;

    update_pos = Oscilloscope.GetUpdate();
    if (update_pos) {
        Oscilloscope.SetUpdate(0);

        count++;
        if (count >= 5)  // Update at 2Hz
        {
            count = 0;
            if (togglebutton1.GetCheck()) {
                Oscilloscope.CalculateStats(0);
            } else {
                Oscilloscope.ClearStats(0);
            }
            if (togglebutton2.GetCheck()) {
                Oscilloscope.CalculateStats(1);
            } else {
                Oscilloscope.ClearStats(1);
            }
        }
        DrawScreen();
        update_pos = 0;
#ifndef _WIN_
        Draw();
#endif
        if (togglebutton6.GetCheck() && spind1.GetEnable())
            togglebutton6_EvOnToggleButton(this);
    }
}

void CPWindow4::combo2_EvOnComboChange(CControl* control) {
    Oscilloscope.SetChannelPin(0, atoi(combo2.GetText()) - 1);
}

void CPWindow4::combo3_EvOnComboChange(CControl* control) {
    Oscilloscope.SetChannelPin(1, atoi(combo3.GetText()) - 1);
}

void CPWindow4::_EvOnDestroy(CControl* control) {}

void CPWindow4::_EvOnCreate(CControl* control) {
    SetTitle(((PICSimLab.GetInstanceNumber() > 0)
                  ? ("PICSimLab[" + std::to_string(PICSimLab.GetInstanceNumber()) + "] - ")
                  : ("PICSimLab - ")) +
             "Oscilloscope");
}

void CPWindow4::_EvOnShow(CControl* control) {
    Oscilloscope.Reset();
    timer1.SetRunState(1);
}

void CPWindow4::_EvOnHide(CControl* control) {
    timer1.SetRunState(0);
    board* pboard = PICSimLab.GetBoard();
    if (pboard) {
        pboard->SetUseOscilloscope(0);
    }
}

// autoset

void CPWindow4::button3_EvMouseButtonClick(CControl* control, unsigned int button, unsigned int x, unsigned int y,
                                           unsigned int state) {
    int ce = togglebutton1.GetCheck() + togglebutton2.GetCheck() * 2;

    togglebutton5.SetCheck(1);  // trigguer
    spind5.SetValue(1.0);       // time scale
    spind5_EvOnChangeSpinDouble(control);
    // spind6.SetValue(0.0);//time offset
    spind7.SetValue(2.5);  // trigguer level
    switch (ce) {
        case 0:
            break;
        case 1:
            spind1.SetValue(2.0);       // ch1 gain
            spind2.SetValue(0.0);       // ch1 level
            togglebutton3.SetCheck(0);  // ch1 inverse off
            combo1.SetText("1");        // trigguer channel
            break;
        case 2:
            spind3.SetValue(2.0);       // ch2 gain
            spind4.SetValue(0.0);       // ch2 level
            togglebutton4.SetCheck(0);  // ch2 inverse off
            combo1.SetText("2");        // trigguer channel
            break;
        case 3:
            spind1.SetValue(2.0);       // ch1 gain
            spind2.SetValue(0.0);       // ch1 level
            spind3.SetValue(2.0);       // ch2 gain
            spind4.SetValue(-6.0);      // ch2 level
            togglebutton3.SetCheck(0);  // ch1 inverse off
            togglebutton4.SetCheck(0);  // ch2 inverse off
            // TODO select the better mode for channel trigguer
            combo1.SetText("1");  // trigguer channel
            break;
    }
}

// stop

void CPWindow4::togglebutton6_EvOnToggleButton(CControl* control) {
    Oscilloscope.SetRun(!togglebutton6.GetCheck());
    spind1.SetEnable(Oscilloscope.GetRun());
    spind2.SetEnable(Oscilloscope.GetRun());
    spind3.SetEnable(Oscilloscope.GetRun());
    spind4.SetEnable(Oscilloscope.GetRun());
    spind5.SetEnable(Oscilloscope.GetRun());
    spind6.SetEnable(Oscilloscope.GetRun());
    spind7.SetEnable(Oscilloscope.GetRun());
    button1.SetEnable(Oscilloscope.GetRun());
    button2.SetEnable(Oscilloscope.GetRun());
    button3.SetEnable(Oscilloscope.GetRun());
    togglebutton1.SetEnable(Oscilloscope.GetRun());
    togglebutton2.SetEnable(Oscilloscope.GetRun());
    togglebutton3.SetEnable(Oscilloscope.GetRun());
    togglebutton4.SetEnable(Oscilloscope.GetRun());
    togglebutton5.SetEnable(Oscilloscope.GetRun());
    combo1.SetEnable(Oscilloscope.GetRun());
    combo2.SetEnable(Oscilloscope.GetRun());
    combo3.SetEnable(Oscilloscope.GetRun());
}

// save PNG

void CPWindow4::button4_EvMouseButtonClick(CControl* control, unsigned int button, unsigned int x, unsigned int y,
                                           unsigned int state) {
    filedialog1.SetType(lxFD_SAVE | lxFD_CHANGE_DIR);
    filedialog1.Run();
}

void CPWindow4::filedialog1_EvOnClose(int retId) {
    if (retId) {
        draw1.WriteImgToFile(filedialog1.GetFileName());
    }
}

void CPWindow4::colordialog1_EvOnClose(int retId) {
    if (retId) {
        ctrl->SetColor(colordialog1.GetColor());
    }
}

void CPWindow4::button5_EvMouseButtonPress(CControl* control, const unsigned int button, const unsigned int x,
                                           const unsigned int y, const unsigned int state) {
    Oscilloscope.NextMeasure(0);
}

void CPWindow4::button6_EvMouseButtonPress(CControl* control, const unsigned int button, const unsigned int x,
                                           const unsigned int y, const unsigned int state) {
    Oscilloscope.NextMeasure(1);
}

void CPWindow4::button7_EvMouseButtonPress(CControl* control, const unsigned int button, const unsigned int x,
                                           const unsigned int y, const unsigned int state) {
    Oscilloscope.NextMeasure(2);
}

void CPWindow4::button8_EvMouseButtonPress(CControl* control, const unsigned int button, const unsigned int x,
                                           const unsigned int y, const unsigned int state) {
    Oscilloscope.NextMeasure(3);
}

void CPWindow4::button9_EvMouseButtonPress(CControl* control, const unsigned int button, const unsigned int x,
                                           const unsigned int y, const unsigned int state) {
    Oscilloscope.NextMeasure(4);
}

int CPWindow4::OnWindowCmd(const int id, const char* ControlName, const PICSimLabWindowAction action, const char* Value,
                           void* ReturnBuff) {
    CControl* ctrl = NULL;

    if (id == 0) {
        if (ControlName) {
            ctrl = Window4.GetChildByName(ControlName);
        } else {
            ctrl = &Window4;
        }

        if (ctrl == NULL) {
            return -1;
        }
    } else {
        return -1;
    }

    return Window1.WinCmd(ctrl, action, Value, ReturnBuff);
}