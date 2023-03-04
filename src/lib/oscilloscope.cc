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

#include "oscilloscope.h"
#include "picsimlab.h"
#include "spareparts.h"

#include <picsim/picsim.h>

COscilloscope Oscilloscope;

COscilloscope::COscilloscope() {
    Dt = 0;
    Rt = 0;
    usetrigger = 1;
    triggerlv = 2.5;
    chpin[0] = 0;
    chpin[1] = 1;
    toffset = 250;
    run = 1;

    fp = 0;
    tch = 0;
    is = 0;
    t = 0;
    tr = 0;
    update = 0;

    measures[0] = 1;
    measures[1] = 2;
    measures[2] = 0;
    measures[3] = 0;
    measures[4] = 0;

    vmax = 5.0;

    tbstop = NULL;
    tbsingle = NULL;
}

void COscilloscope::SetSample(void) {
    double pins[2];

    const picpin* ppins = pboard->MGetPinsValues();

    if ((!run) || (tbsingle == NULL))
        return;

    if ((ppins[chpin[0]].ptype == PT_ANALOG) && (ppins[chpin[0]].dir == PD_IN))
        pins[0] = ppins[chpin[0]].avalue;
    else
        pins[0] = ppins[chpin[0]].value * vmax;

    if ((ppins[chpin[1]].ptype == PT_ANALOG) && (ppins[chpin[1]].dir == PD_IN))
        pins[1] = ppins[chpin[1]].avalue;
    else
        pins[1] = ppins[chpin[1]].value * vmax;

    // sampling
    if (t > Rt) {
        t -= Rt;
        databuffer[fp][0][is] = -pins[0] + ((1.0 * rand() / RAND_MAX) - 0.5) * 0.1;
        databuffer[fp][1][is] = -pins[1] + ((1.0 * rand() / RAND_MAX) - 0.5) * 0.1;
        is++;
        if (is >= NPOINTS)  // buffer full
        {
            if (tr && tbsingle->GetCheck()) {
                tbstop->SetCheck(1);
            }
            is = 0;
            tr = 0;
            t = 0;
            ch[0] = &databuffer[fp][0][toffset];
            ch[1] = &databuffer[fp][1][toffset];
            fp = !fp;    // togle fp
            update = 1;  // Request redraw screen
        }
    }
    t += Dt;

    // trigger
    if (usetrigger) {
        if ((!tr) && (is >= NPOINTS / 2)) {
            if ((pins_[tch] < triggerlv) && (pins[tch] >= triggerlv)) {
                tr = 1;

                is = is - (NPOINTS / 2);

                for (int i = 0; i < NPOINTS / 2; i++) {
                    databuffer[fp][0][i] = databuffer[fp][0][i + is];
                    databuffer[fp][1][i] = databuffer[fp][1][i + is];
                }
                is = (NPOINTS / 2);
            }
        }
    }

    pins_[0] = pins[0];
    pins_[1] = pins[1];
}

void COscilloscope::NextMeasure(int mn) {
    measures[mn]++;
    if (measures[mn] >= MAX_MEASURES) {
        measures[mn] = 0;
    }
}

int COscilloscope::GetMeasures(int mn) {
    return measures[mn];
}

void COscilloscope::SetMeasure(int nm, int value) {
    measures[nm] = value;
}

void COscilloscope::CalculateStats(int channel) {
    ch_status[channel].Vmax = -1000;
    ch_status[channel].Vmin = 1000;
    double sumSamples = 0;
    double sumSquares = 0;
    double val = -ch[channel][0];
    int i = 1;
    bool ltr_down = (val < ch_status[channel].Vavr);  // last transition down
    bool firstUp = true;
    unsigned short ltrsHigh = 0;    // last transition sample high number
    unsigned short sumFCW = 0;      // Full cycle width sum
    unsigned short sumPCW = 0;      // Positive semi-cycle width sum
    unsigned short numFCycles = 0;  // Number of full cycles
    unsigned short numPCycles = 0;  // Number of positive semi-cycles

    for (i = 1; i < (NPOINTS / 2) - 1; i++) {
        val = -ch[channel][i];

        if (ch_status[channel].Vmax < val)
            ch_status[channel].Vmax = val;
        if (ch_status[channel].Vmin > val)
            ch_status[channel].Vmin = val;
        sumSamples += val;
        sumSquares += (val * val);

        // find out frequency (using previous Vavr)
        if (ltr_down && (val > ch_status[channel].Vavr))  // transition UP
        {
            if (!firstUp) {
                sumFCW += (i - 1 - ltrsHigh);
                numFCycles++;
            } else {
                firstUp = false;
            }
            ltr_down = false;
            ltrsHigh = i - 1;
        } else if (!ltr_down && (val < ch_status[channel].Vavr))  // transition Down
        {
            if (!firstUp) {
                sumPCW += (i - 1 - ltrsHigh);
                numPCycles++;
            }
            ltr_down = true;
        }
    }

    ch_status[channel].Vavr = sumSamples / (NPOINTS / 2);  // Voltage average
    ch_status[channel].Vrms = sqrt(sumSquares / (NPOINTS / 2));

    double avgFCycleWidth = sumFCW * Rt / numFCycles;
    double avgPCycleWidth = sumPCW * Rt / numPCycles;

    int pulseValid = (numFCycles > 0) && (avgFCycleWidth != 0) && (avgPCycleWidth != 0) &&
                     ((ch_status[channel].Vmax - ch_status[channel].Vmin) > 0.2);

    if (pulseValid) {
        ch_status[channel].PCycle_ms = avgPCycleWidth * 1000;
        ch_status[channel].FCycle_ms = avgFCycleWidth * 1000;
        ch_status[channel].Freq = 1.0 / avgFCycleWidth;
        ch_status[channel].Duty = avgPCycleWidth * 100 / avgFCycleWidth;
    } else {
        ch_status[channel].PCycle_ms = -1;
        ch_status[channel].FCycle_ms = -1;
        ch_status[channel].Freq = -1;
        ch_status[channel].Duty = -1;
    }
    /*
    printf ("================\n");
    printf ("avgPW %lf ms\n", ch_status[channel].PCycle_ms);
    printf ("duty %lf %%\n", ch_status[channel].Duty);
    printf ("freq %lf Hz\n", ch_status[channel].Freq);
    printf ("cycle %lf ms\n", ch_status[channel].FCycle_ms);
    printf ("Vrms %lf V\n", ch_status[channel].Vrms);
    printf ("Vavr %lf V\n", ch_status[channel].Vavr);
    printf ("Vmax %lf V\n", ch_status[channel].Vmax);
    printf ("Vmin %lf V\n", ch_status[channel].Vmax);
     */
}

void COscilloscope::ClearStats(int channel) {
    memset(&ch_status[channel], 0, sizeof(ch_status_t));
}

void COscilloscope::Reset(void) {
    vmax = pboard->MGetVCC();
}

void COscilloscope::Init(CWindow* win) {
    Window = win;
    tbstop = (CToggleButton*)Window->GetChildByName("togglebutton6");
    tbsingle = (CToggleButton*)Window->GetChildByName("togglebutton7");
}

void COscilloscope::WritePreferences(void) {
    if (!Window) {
        return;
    }

    PICSimLab.SavePrefs(lxT("osc_scale1"), ftoa(((CSpind*)Window->GetChildByName("spind1"))->GetValue()));
    PICSimLab.SavePrefs(lxT("osc_offset1"), ftoa(((CSpind*)Window->GetChildByName("spind2"))->GetValue()));
    PICSimLab.SavePrefs(lxT("osc_on1"), itoa(((CToggleButton*)Window->GetChildByName("togglebutton1"))->GetCheck()));
    PICSimLab.SavePrefs(lxT("osc_color1"),
                        ((CButton*)Window->GetChildByName("button1"))->GetColor().GetAsString(lxC2S_HTML_SYNTAX));
    PICSimLab.SavePrefs(lxT("osc_inv1"), itoa(((CToggleButton*)Window->GetChildByName("togglebutton3"))->GetCheck()));
    PICSimLab.SavePrefs(lxT("osc_ch1"), ((CCombo*)Window->GetChildByName("combo2"))->GetText());

    PICSimLab.SavePrefs(lxT("osc_scale2"), ftoa(((CSpind*)Window->GetChildByName("spind3"))->GetValue()));
    PICSimLab.SavePrefs(lxT("osc_offset2"), ftoa(((CSpind*)Window->GetChildByName("spind4"))->GetValue()));
    PICSimLab.SavePrefs(lxT("osc_on2"), itoa(((CToggleButton*)Window->GetChildByName("togglebutton2"))->GetCheck()));
    PICSimLab.SavePrefs(lxT("osc_color2"),
                        ((CButton*)Window->GetChildByName("button2"))->GetColor().GetAsString(lxC2S_HTML_SYNTAX));
    PICSimLab.SavePrefs(lxT("osc_inv2"), itoa(((CToggleButton*)Window->GetChildByName("togglebutton4"))->GetCheck()));
    PICSimLab.SavePrefs(lxT("osc_ch2"), ((CCombo*)Window->GetChildByName("combo3"))->GetText());

    PICSimLab.SavePrefs(lxT("osc_tscale"), ftoa(((CSpind*)Window->GetChildByName("spind5"))->GetValue()));
    PICSimLab.SavePrefs(lxT("osc_toffset"), ftoa(((CSpind*)Window->GetChildByName("spind6"))->GetValue()));
    PICSimLab.SavePrefs(lxT("osc_usetrigger"),
                        itoa(((CToggleButton*)Window->GetChildByName("togglebutton5"))->GetCheck()));
    PICSimLab.SavePrefs(lxT("osc_tch"), ((CCombo*)Window->GetChildByName("combo1"))->GetText());
    PICSimLab.SavePrefs(lxT("osc_tlevel"), ftoa(((CSpind*)Window->GetChildByName("spind7"))->GetValue()));
    PICSimLab.SavePrefs(lxT("osc_position"), itoa(Window->GetX()) + lxT(",") + itoa(Window->GetY()));

    PICSimLab.SavePrefs(lxT("osc_measures"), itoa(GetMeasures(0)) + lxT(",") + itoa(GetMeasures(1)) + lxT(",") +
                                                 itoa(GetMeasures(2)) + lxT(",") + itoa(GetMeasures(3)) + lxT(",") +
                                                 itoa(GetMeasures(4)));
}

void COscilloscope::ReadPreferences(char* name, char* value) {
    if (!Window) {
        return;
    }

    if (!strcmp(name, "osc_scale1")) {
        ((CSpind*)Window->GetChildByName("spind1"))->SetValue(atof(value));
    }

    if (!strcmp(name, "osc_offset1")) {
        ((CSpind*)Window->GetChildByName("spind2"))->SetValue(atof(value));
    }

    if (!strcmp(name, "osc_on1")) {
        ((CToggleButton*)Window->GetChildByName("togglebutton1"))->SetCheck(atoi(value));
    }

    if (!strcmp(name, "osc_color1")) {
        ((CButton*)Window->GetChildByName("button1"))->SetColor(lxColor(value));
    }

    if (!strcmp(name, "osc_inv1")) {
        ((CToggleButton*)Window->GetChildByName("togglebutton3"))->SetCheck(atoi(value));
    }

    if (!strcmp(name, "osc_ch1")) {
        ((CCombo*)Window->GetChildByName("combo2"))->SetText(value);
        SetChannelPin(0, atoi(((CCombo*)Window->GetChildByName("combo2"))->GetText()) - 1);
    }

    if (!strcmp(name, "osc_scale2")) {
        ((CSpind*)Window->GetChildByName("spind3"))->SetValue(atof(value));
    }

    if (!strcmp(name, "osc_offset2")) {
        ((CSpind*)Window->GetChildByName("spind4"))->SetValue(atof(value));
    }

    if (!strcmp(name, "osc_on2")) {
        ((CToggleButton*)Window->GetChildByName("togglebutton2"))->SetCheck(atoi(value));
    }

    if (!strcmp(name, "osc_color2")) {
        ((CButton*)Window->GetChildByName("button2"))->SetColor(lxColor(value));
    }

    if (!strcmp(name, "osc_inv2")) {
        ((CToggleButton*)Window->GetChildByName("togglebutton4"))->SetCheck(atoi(value));
    }

    if (!strcmp(name, "osc_ch2")) {
        ((CCombo*)Window->GetChildByName("combo3"))->SetText(value);
        SetChannelPin(1, atoi(((CCombo*)Window->GetChildByName("combo3"))->GetText()) - 1);
    }

    if (!strcmp(name, "osc_tscale")) {
        ((CSpind*)Window->GetChildByName("spind5"))->SetValue(atof(value));
    }

    if (!strcmp(name, "osc_toffset")) {
        ((CSpind*)Window->GetChildByName("spind6"))->SetValue(atof(value));
    }

    if (!strcmp(name, "osc_usetrigger")) {
        ((CToggleButton*)Window->GetChildByName("togglebutton5"))->SetCheck(atoi(value));
        SetUseTrigger(atoi(value));
    }

    if (!strcmp(name, "osc_tch")) {
        ((CCombo*)Window->GetChildByName("combo1"))->SetText(value);
    }

    if (!strcmp(name, "osc_tlevel")) {
        ((CSpind*)Window->GetChildByName("spind7"))->SetValue(atof(value));
    }

    if (!strcmp(name, "osc_position")) {
        int i, j;
        sscanf(value, "%i,%i", &i, &j);
        Window->SetX(i);
        Window->SetY(j);
    }

    if (!strcmp(name, "osc_measures")) {
        int measures[5];
        sscanf(value, "%i,%i,%i,%i,%i", &measures[0], &measures[1], &measures[2], &measures[3], &measures[4]);
        for (int i = 0; i < 5; i++) {
            SetMeasure(i, measures[i]);
        }
    }
}

lxStringList COscilloscope::WritePreferencesList(void) {
    lxStringList list;
    lxString line;

    if (!Window) {
        return list;
    }

    line = "osc_cfg," + itoa(Window->GetX()) + "," + itoa(Window->GetY()) + ",0:";
    // osc_tscale
    line += ftoa(((CSpind*)Window->GetChildByName("spind5"))->GetValue()) + ",";
    // osc_toffset
    line += ftoa(((CSpind*)Window->GetChildByName("spind6"))->GetValue()) + ",";
    // osc_usetrigger
    line += itoa(((CToggleButton*)Window->GetChildByName("togglebutton5"))->GetCheck()) + ",";
    // osc_tch
    line += ((CCombo*)Window->GetChildByName("combo1"))->GetText() + ",";
    // osc_tlevel
    line += ftoa(((CSpind*)Window->GetChildByName("spind7"))->GetValue()) + ",";
    // osc_measures
    line += itoa(GetMeasures(0)) + "," + itoa(GetMeasures(1)) + +"," + itoa(GetMeasures(2)) + +"," +
            itoa(GetMeasures(3)) + +"," + itoa(GetMeasures(4));
    list.AddLine(line);

    line = "osc_ch1,0,0,0:";
    // osc_scale1
    line += ftoa(((CSpind*)Window->GetChildByName("spind1"))->GetValue()) + ",";
    // osc_offset1
    line += ftoa(((CSpind*)Window->GetChildByName("spind2"))->GetValue()) + ",";
    // osc_on1
    line += itoa(((CToggleButton*)Window->GetChildByName("togglebutton1"))->GetCheck()) + ",";
    // osc_color1
    line += ((CButton*)Window->GetChildByName("button1"))->GetColor().GetAsString(lxC2S_HTML_SYNTAX) + ",";
    // osc_inv1
    line += itoa(((CToggleButton*)Window->GetChildByName("togglebutton3"))->GetCheck()) + ",";
    // osc_ch1
    line += ((CCombo*)Window->GetChildByName("combo2"))->GetText();
    list.AddLine(line);

    line = "osc_ch2,0,0,0:";
    // osc_scale2
    line += ftoa(((CSpind*)Window->GetChildByName("spind3"))->GetValue()) + ",";
    // osc_offset2
    line += ftoa(((CSpind*)Window->GetChildByName("spind4"))->GetValue()) + ",";
    // osc_on2
    line += itoa(((CToggleButton*)Window->GetChildByName("togglebutton2"))->GetCheck()) + ",";
    // osc_color2
    line += ((CButton*)Window->GetChildByName("button2"))->GetColor().GetAsString(lxC2S_HTML_SYNTAX) + ",";
    // osc_inv2
    line += itoa(((CToggleButton*)Window->GetChildByName("togglebutton4"))->GetCheck()) + ",";
    // osc_ch2
    line += ((CCombo*)Window->GetChildByName("combo3"))->GetText();
    list.AddLine(line);

    return list;
}

void COscilloscope::ReadPreferencesList(lxStringList pl) {
    char line[1024];
    char* tokens[15];

    if (!Window) {
        return;
    }

    strncpy(line, (const char*)pl.GetLine(0).c_str(), 1024);
    tokens[0] = strtok(line, ",:\n");
    for (int i = 1; i < 15; i++) {
        tokens[i] = strtok(NULL, ",:\n");
        if (tokens[i] == NULL) {
            break;
        }
    }

    // cfg
    Window->SetX(atoi(tokens[1]));
    Window->SetY(atoi(tokens[2]));

    // osc_tscale
    ((CSpind*)Window->GetChildByName("spind5"))->SetValue(atof(tokens[4]));

    // osc_toffset
    ((CSpind*)Window->GetChildByName("spind6"))->SetValue(atof(tokens[5]));

    // osc_usetrigger
    ((CToggleButton*)Window->GetChildByName("togglebutton5"))->SetCheck(atoi(tokens[6]));
    SetUseTrigger(atoi(tokens[6]));

    // osc_tch
    ((CCombo*)Window->GetChildByName("combo1"))->SetText(tokens[7]);

    // osc_tlevel
    ((CSpind*)Window->GetChildByName("spind7"))->SetValue(atof(tokens[8]));

    // osc_measures
    for (int i = 0; i < 5; i++) {
        SetMeasure(i, atoi(tokens[9 + i]));
    }

    strncpy(line, (const char*)pl.GetLine(1).c_str(), 1024);
    tokens[0] = strtok(line, ",:\n");
    for (int i = 1; i < 15; i++) {
        tokens[i] = strtok(NULL, ",:\n");
        if (tokens[i] == NULL) {
            break;
        }
    }

    // ch1
    // osc_scale1
    ((CSpind*)Window->GetChildByName("spind1"))->SetValue(atof(tokens[4]));

    // osc_offset1
    ((CSpind*)Window->GetChildByName("spind2"))->SetValue(atof(tokens[5]));

    // osc_on1
    ((CToggleButton*)Window->GetChildByName("togglebutton1"))->SetCheck(atoi(tokens[6]));

    // osc_color1
    ((CButton*)Window->GetChildByName("button1"))->SetColor(lxColor(tokens[7]));

    // osc_inv1
    ((CToggleButton*)Window->GetChildByName("togglebutton3"))->SetCheck(atoi(tokens[8]));

    // osc_ch1
    ((CCombo*)Window->GetChildByName("combo2"))->SetText(tokens[9]);
    SetChannelPin(0, atoi(((CCombo*)Window->GetChildByName("combo2"))->GetText()) - 1);

    strncpy(line, (const char*)pl.GetLine(2).c_str(), 1024);
    tokens[0] = strtok(line, ",:\n");
    for (int i = 1; i < 15; i++) {
        tokens[i] = strtok(NULL, ",:\n");
        if (tokens[i] == NULL) {
            break;
        }
    }
    // ch2
    // osc_scale2
    ((CSpind*)Window->GetChildByName("spind3"))->SetValue(atof(tokens[4]));

    // osc_offset2
    ((CSpind*)Window->GetChildByName("spind4"))->SetValue(atof(tokens[5]));

    // osc_on2
    ((CToggleButton*)Window->GetChildByName("togglebutton2"))->SetCheck(atoi(tokens[6]));

    // osc_color2
    ((CButton*)Window->GetChildByName("button2"))->SetColor(lxColor(tokens[7]));

    // osc_inv2
    ((CToggleButton*)Window->GetChildByName("togglebutton4"))->SetCheck(atoi(tokens[8]));

    // osc_ch2
    ((CCombo*)Window->GetChildByName("combo3"))->SetText(tokens[9]);
    SetChannelPin(1, atoi(((CCombo*)Window->GetChildByName("combo3"))->GetText()) - 1);
}

void COscilloscope::SetBaseTimer(void) {
    board* pboard = PICSimLab.GetBoard();

    if (!Window) {
        return;
    }

    if (!pboard)
        return;

    int PinCount = pboard->MGetPinCount();

    if (pboard->CpuInitialized() == 0)
        return;

    SetDT(1.0 / pboard->MGetInstClockFreq());

    int chp[2];

    // printf("Dt=%e  Rt=%e  Rt/Dt=%f\n",Dt,Rt,Rt/Dt);

    chp[0] = atoi(((CCombo*)Window->GetChildByName("combo2"))->GetText());
    chp[1] = atoi(((CCombo*)Window->GetChildByName("combo3"))->GetText());

    ((CCombo*)Window->GetChildByName("combo2"))->DeleteItems();
    ((CCombo*)Window->GetChildByName("combo3"))->DeleteItems();
    for (int i = 1; i <= PinCount; i++) {
        lxString spin;
        if (pboard->GetUseSpareParts()) {
            spin = SpareParts.GetPinName(i);
        } else {
            spin = pboard->MGetPinName(i);
        }
        if (spin.Cmp(lxT("error"))) {
            ((CCombo*)Window->GetChildByName("combo2"))->AddItem(itoa(i) + "  " + spin);
            ((CCombo*)Window->GetChildByName("combo3"))->AddItem(itoa(i) + "  " + spin);
        }
    }

    if (chp[0] <= PinCount) {
        lxString spin;
        if (pboard->GetUseSpareParts()) {
            spin = SpareParts.GetPinName(chp[0]);
        } else {
            spin = pboard->MGetPinName(chp[0]);
        }
        ((CCombo*)Window->GetChildByName("combo2"))->SetText(itoa(chp[0]) + "  " + spin);
    } else
        ((CCombo*)Window->GetChildByName("combo2"))->SetText("1");

    if (chp[1] <= PinCount) {
        lxString spin;
        if (pboard->GetUseSpareParts()) {
            spin = SpareParts.GetPinName(chp[1]);
        } else {
            spin = pboard->MGetPinName(chp[1]);
        }
        ((CCombo*)Window->GetChildByName("combo3"))->SetText(itoa(chp[1]) + "  " + spin);
    } else
        ((CCombo*)Window->GetChildByName("combo3"))->SetText("2");

    CSpind* spind = (CSpind*)Window->GetChildByName("spind5");
    (Window->*(spind->EvOnChangeSpinDouble))(NULL);
}