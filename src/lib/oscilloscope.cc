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

#include "oscilloscope.h"
#include "picsimlab.h"
#include "spareparts.h"

#include <math.h>
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

    OnWindowCmd = NULL;
}

void COscilloscope::SetSample(void) {
    double pins[2];

    const picpin* ppins = pboard->MGetPinsValues();

    if (!run)
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
            int checked;
            WindowCmd(PW_MAIN, "togglebutton7", PWA_TOGGLEBGETCHECK, NULL, &checked);
            if (tr && checked) {
                WindowCmd(PW_MAIN, "togglebutton6", PWA_TOGGLEBSETCHECK, "1");
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

void COscilloscope::Init(void) {}

void COscilloscope::WritePreferences(void) {
    int created = 0;
    WindowCmd(PW_MAIN, NULL, PWA_WINDOWHASCREATED, NULL, &created);

    if (created != 1) {
        return;
    }

    char buff[256];
    int value;
    float fvalue;
    WindowCmd(PW_MAIN, "spind1", PWA_SPINDGETVALUE, NULL, &fvalue);
    PICSimLab.SavePrefs("osc_scale1", std::to_string(fvalue));
    WindowCmd(PW_MAIN, "spind2", PWA_SPINDGETVALUE, NULL, &fvalue);
    PICSimLab.SavePrefs("osc_offset1", std::to_string(fvalue));
    WindowCmd(PW_MAIN, "togglebutton1", PWA_TOGGLEBGETCHECK, NULL, &value);
    PICSimLab.SavePrefs("osc_on1", std::to_string(value));
    WindowCmd(PW_MAIN, "button1", PWA_GETCOLOR, NULL, &buff);
    PICSimLab.SavePrefs("osc_color1", buff);
    WindowCmd(PW_MAIN, "togglebutton3", PWA_TOGGLEBGETCHECK, NULL, &value);
    PICSimLab.SavePrefs("osc_inv1", std::to_string(value));
    WindowCmd(PW_MAIN, "combo2", PWA_COMBOGETTEXT, NULL, &buff);
    PICSimLab.SavePrefs("osc_ch1", buff);

    WindowCmd(PW_MAIN, "spind3", PWA_SPINDGETVALUE, NULL, &fvalue);
    PICSimLab.SavePrefs("osc_scale2", std::to_string(fvalue));
    WindowCmd(PW_MAIN, "spind4", PWA_SPINDGETVALUE, NULL, &fvalue);
    PICSimLab.SavePrefs("osc_offset2", std::to_string(fvalue));
    WindowCmd(PW_MAIN, "togglebutton2", PWA_TOGGLEBGETCHECK, NULL, &value);
    PICSimLab.SavePrefs("osc_on2", std::to_string(value));
    WindowCmd(PW_MAIN, "button2", PWA_GETCOLOR, NULL, &buff);
    PICSimLab.SavePrefs("osc_color2", buff);
    WindowCmd(PW_MAIN, "togglebutton4", PWA_TOGGLEBGETCHECK, NULL, &value);
    PICSimLab.SavePrefs("osc_inv2", std::to_string(value));
    WindowCmd(PW_MAIN, "combo3", PWA_COMBOGETTEXT, NULL, &buff);
    PICSimLab.SavePrefs("osc_ch2", buff);

    WindowCmd(PW_MAIN, "spind5", PWA_SPINDGETVALUE, NULL, &fvalue);
    PICSimLab.SavePrefs("osc_tscale", std::to_string(fvalue));
    WindowCmd(PW_MAIN, "spind6", PWA_SPINDGETVALUE, NULL, &fvalue);
    PICSimLab.SavePrefs("osc_toffset", std::to_string(fvalue));
    WindowCmd(PW_MAIN, "togglebutton5", PWA_TOGGLEBGETCHECK, NULL, &value);
    PICSimLab.SavePrefs("osc_usetrigger", std::to_string(value));
    WindowCmd(PW_MAIN, "combo1", PWA_COMBOGETTEXT, NULL, &buff);
    PICSimLab.SavePrefs("osc_tch", buff);
    WindowCmd(PW_MAIN, "spind7", PWA_SPINDGETVALUE, NULL, &fvalue);
    PICSimLab.SavePrefs("osc_tlevel", std::to_string(fvalue));

    int x, y;
    WindowCmd(PW_MAIN, "NULL", PWA_GETX, NULL, &x);
    WindowCmd(PW_MAIN, "NULL", PWA_GETY, NULL, &y);

    PICSimLab.SavePrefs("osc_position", std::to_string(x) + "," + std::to_string(y));

    PICSimLab.SavePrefs("osc_measures", std::to_string(GetMeasures(0)) + "," + std::to_string(GetMeasures(1)) + "," +
                                            std::to_string(GetMeasures(2)) + "," + std::to_string(GetMeasures(3)) +
                                            "," + std::to_string(GetMeasures(4)));
}

void COscilloscope::ReadPreferences(char* name, char* value) {
    int created = 0;
    WindowCmd(PW_MAIN, NULL, PWA_WINDOWHASCREATED, NULL, &created);

    if (created != 1) {
        return;
    }

    if (!strcmp(name, "osc_scale1")) {
        WindowCmd(PW_MAIN, "spind1", PWA_SPINDSETVALUE, value);
    }

    if (!strcmp(name, "osc_offset1")) {
        WindowCmd(PW_MAIN, "spind2", PWA_SPINDSETVALUE, value);
    }

    if (!strcmp(name, "osc_on1")) {
        WindowCmd(PW_MAIN, "togglebutton1", PWA_TOGGLEBSETCHECK, value);
    }

    if (!strcmp(name, "osc_color1")) {
        WindowCmd(PW_MAIN, "button1", PWA_SETCOLOR, value);
    }

    if (!strcmp(name, "osc_inv1")) {
        WindowCmd(PW_MAIN, "togglebutton3", PWA_TOGGLEBSETCHECK, value);
    }

    if (!strcmp(name, "osc_ch1")) {
        WindowCmd(PW_MAIN, "combo2", PWA_COMBOSETTEXT, value);
        SetChannelPin(0, atoi(value) - 1);
    }

    if (!strcmp(name, "osc_scale2")) {
        WindowCmd(PW_MAIN, "spind3", PWA_SPINDSETVALUE, value);
    }

    if (!strcmp(name, "osc_offset2")) {
        WindowCmd(PW_MAIN, "spind4", PWA_SPINDSETVALUE, value);
    }

    if (!strcmp(name, "osc_on2")) {
        WindowCmd(PW_MAIN, "togglebutton2", PWA_TOGGLEBSETCHECK, value);
    }

    if (!strcmp(name, "osc_color2")) {
        WindowCmd(PW_MAIN, "button2", PWA_SETCOLOR, value);
    }

    if (!strcmp(name, "osc_inv2")) {
        WindowCmd(PW_MAIN, "togglebutton4", PWA_TOGGLEBSETCHECK, value);
    }

    if (!strcmp(name, "osc_ch2")) {
        WindowCmd(PW_MAIN, "combo3", PWA_COMBOSETTEXT, value);
        SetChannelPin(1, atoi(value) - 1);
    }

    if (!strcmp(name, "osc_tscale")) {
        WindowCmd(PW_MAIN, "spind5", PWA_SPINDSETVALUE, value);
    }

    if (!strcmp(name, "osc_toffset")) {
        WindowCmd(PW_MAIN, "spind6", PWA_SPINDSETVALUE, value);
    }

    if (!strcmp(name, "osc_usetrigger")) {
        WindowCmd(PW_MAIN, "togglebutton5", PWA_TOGGLEBSETCHECK, value);
        SetUseTrigger(atoi(value));
    }

    if (!strcmp(name, "osc_tch")) {
        WindowCmd(PW_MAIN, "combo1", PWA_COMBOSETTEXT, value);
    }

    if (!strcmp(name, "osc_tlevel")) {
        WindowCmd(PW_MAIN, "spind7", PWA_SPINDSETVALUE, value);
    }

    if (!strcmp(name, "osc_position")) {
        int i, j;
        sscanf(value, "%i,%i", &i, &j);
        WindowCmd(PW_MAIN, NULL, PWA_SETX, std::to_string(i).c_str());
        WindowCmd(PW_MAIN, NULL, PWA_SETY, std::to_string(j).c_str());
    }

    if (!strcmp(name, "osc_measures")) {
        int measures[5];
        sscanf(value, "%i,%i,%i,%i,%i", &measures[0], &measures[1], &measures[2], &measures[3], &measures[4]);
        for (int i = 0; i < 5; i++) {
            SetMeasure(i, measures[i]);
        }
    }
}

std::vector<std::string> COscilloscope::WritePreferencesList(void) {
    std::vector<std::string> list;
    std::string line;

    int created = 0;
    Oscilloscope.WindowCmd(PW_MAIN, NULL, PWA_WINDOWHASCREATED, NULL, &created);

    if (created != 1) {
        return list;
    }
    int x, y;
    float fvalue;
    int value;
    char buff[256];

    WindowCmd(PW_MAIN, NULL, PWA_GETX, NULL, &x);
    WindowCmd(PW_MAIN, NULL, PWA_GETY, NULL, &y);
    line = "osc_cfg," + std::to_string(x) + "," + std::to_string(y) + ",0:";
    // osc_tscale
    WindowCmd(PW_MAIN, "spind5", PWA_SPINDGETVALUE, NULL, &fvalue);
    line += std::to_string(fvalue) + ",";
    // osc_toffset
    WindowCmd(PW_MAIN, "spind6", PWA_SPINDGETVALUE, NULL, &fvalue);
    line += std::to_string(fvalue) + ",";
    // osc_usetrigger
    WindowCmd(PW_MAIN, "togglebutton5", PWA_TOGGLEBGETCHECK, NULL, &value);
    line += std::to_string(value) + ",";
    // osc_tch
    WindowCmd(PW_MAIN, "combo1", PWA_COMBOGETTEXT, NULL, &buff);
    line += buff;
    line += ",";
    // osc_tlevel
    WindowCmd(PW_MAIN, "spind7", PWA_SPINDGETVALUE, NULL, &fvalue);
    line += std::to_string(fvalue) + ",";
    // osc_measures
    line += std::to_string(GetMeasures(0)) + "," + std::to_string(GetMeasures(1)) + +"," +
            std::to_string(GetMeasures(2)) + +"," + std::to_string(GetMeasures(3)) + +"," +
            std::to_string(GetMeasures(4));
    list.push_back(line);

    line = "osc_ch1,0,0,0:";
    // osc_scale1
    WindowCmd(PW_MAIN, "spind1", PWA_SPINDGETVALUE, NULL, &fvalue);
    line += std::to_string(fvalue) + ",";
    // osc_offset1
    WindowCmd(PW_MAIN, "spind2", PWA_SPINDGETVALUE, NULL, &fvalue);
    line += std::to_string(fvalue) + ",";
    // osc_on1
    WindowCmd(PW_MAIN, "togglebutton1", PWA_TOGGLEBGETCHECK, NULL, &value);
    line += std::to_string(value) + ",";
    // osc_color1
    WindowCmd(PW_MAIN, "button1", PWA_GETCOLOR, NULL, &buff);
    line += buff;
    line += ",";
    // osc_inv1
    WindowCmd(PW_MAIN, "togglebutton3", PWA_TOGGLEBGETCHECK, NULL, &value);
    line += std::to_string(value) + ",";
    // osc_ch1
    WindowCmd(PW_MAIN, "combo2", PWA_COMBOGETTEXT, NULL, &buff);
    line += buff;
    list.push_back(line);

    line = "osc_ch2,0,0,0:";
    // osc_scale2
    WindowCmd(PW_MAIN, "spind3", PWA_SPINDGETVALUE, NULL, &fvalue);
    line += std::to_string(fvalue) + ",";
    // osc_offset2
    WindowCmd(PW_MAIN, "spind4", PWA_SPINDGETVALUE, NULL, &fvalue);
    line += std::to_string(fvalue) + ",";
    // osc_on2
    WindowCmd(PW_MAIN, "togglebutton2", PWA_TOGGLEBGETCHECK, NULL, &value);
    line += std::to_string(value) + ",";
    // osc_color2
    WindowCmd(PW_MAIN, "button2", PWA_GETCOLOR, NULL, &buff);
    line += buff;
    line += ",";
    // osc_inv2
    WindowCmd(PW_MAIN, "togglebutton4", PWA_TOGGLEBGETCHECK, NULL, &value);
    line += std::to_string(value) + ",";
    // osc_ch2
    WindowCmd(PW_MAIN, "combo3", PWA_COMBOGETTEXT, NULL, &buff);
    line += buff;
    list.push_back(line);

    return list;
}

void COscilloscope::ReadPreferencesList(std::vector<std::string>& pl) {
    char line[1024];
    char* tokens[15];

    int created = 0;
    WindowCmd(PW_MAIN, NULL, PWA_WINDOWHASCREATED, NULL, &created);

    if (created != 1) {
        return;
    }

    strncpy(line, (const char*)pl.at(0).c_str(), 1023);
    tokens[0] = strtok(line, ",:\n");
    for (int i = 1; i < 15; i++) {
        tokens[i] = strtok(NULL, ",:\n");
        if (tokens[i] == NULL) {
            break;
        }
    }

    // cfg
    WindowCmd(PW_MAIN, NULL, PWA_SETX, tokens[1]);
    WindowCmd(PW_MAIN, NULL, PWA_SETY, tokens[2]);

    // osc_tscale
    WindowCmd(PW_MAIN, "spind5", PWA_SPINDSETVALUE, tokens[4]);

    // osc_toffset
    WindowCmd(PW_MAIN, "spind6", PWA_SPINDSETVALUE, tokens[5]);

    // osc_usetrigger
    WindowCmd(PW_MAIN, "togglebutton5", PWA_TOGGLEBSETCHECK, tokens[6]);
    SetUseTrigger(atoi(tokens[6]));

    // osc_tch
    WindowCmd(PW_MAIN, "combo1", PWA_COMBOSETTEXT, tokens[7]);

    // osc_tlevel
    WindowCmd(PW_MAIN, "spind7", PWA_SPINDSETVALUE, tokens[8]);

    // osc_measures
    for (int i = 0; i < 5; i++) {
        SetMeasure(i, atoi(tokens[9 + i]));
    }

    strncpy(line, (const char*)pl.at(1).c_str(), 1023);
    tokens[0] = strtok(line, ",:\n");
    for (int i = 1; i < 15; i++) {
        tokens[i] = strtok(NULL, ",:\n");
        if (tokens[i] == NULL) {
            break;
        }
    }

    // ch1
    // osc_scale1
    WindowCmd(PW_MAIN, "spind1", PWA_SPINDSETVALUE, tokens[4]);

    // osc_offset1
    WindowCmd(PW_MAIN, "spind2", PWA_SPINDSETVALUE, tokens[5]);

    // osc_on1
    WindowCmd(PW_MAIN, "togglebutton1", PWA_TOGGLEBSETCHECK, tokens[6]);

    // osc_color1
    WindowCmd(PW_MAIN, "button1", PWA_SETCOLOR, tokens[7]);

    // osc_inv1
    WindowCmd(PW_MAIN, "togglebutton3", PWA_TOGGLEBSETCHECK, tokens[8]);

    // osc_ch1
    WindowCmd(PW_MAIN, "combo2", PWA_COMBOSETTEXT, tokens[9]);
    SetChannelPin(0, atoi(tokens[9]) - 1);

    strncpy(line, (const char*)pl.at(2).c_str(), 1023);
    tokens[0] = strtok(line, ",:\n");
    for (int i = 1; i < 15; i++) {
        tokens[i] = strtok(NULL, ",:\n");
        if (tokens[i] == NULL) {
            break;
        }
    }
    // ch2
    // osc_scale2
    WindowCmd(PW_MAIN, "spind3", PWA_SPINDSETVALUE, tokens[4]);

    // osc_offset2
    WindowCmd(PW_MAIN, "spind4", PWA_SPINDSETVALUE, tokens[5]);

    // osc_on2
    WindowCmd(PW_MAIN, "togglebutton2", PWA_TOGGLEBSETCHECK, tokens[6]);

    // osc_color2
    WindowCmd(PW_MAIN, "button2", PWA_SETCOLOR, tokens[7]);

    // osc_inv2
    WindowCmd(PW_MAIN, "togglebutton4", PWA_TOGGLEBSETCHECK, tokens[8]);

    // osc_ch2
    WindowCmd(PW_MAIN, "combo3", PWA_COMBOSETTEXT, tokens[9]);
    SetChannelPin(1, atoi(tokens[9]) - 1);
}

void COscilloscope::SetBaseTimer(void) {
    board* pboard = PICSimLab.GetBoard();

    int created = 0;
    WindowCmd(PW_MAIN, NULL, PWA_WINDOWHASCREATED, NULL, &created);

    if (created != 1) {
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

    char buff[128];
    WindowCmd(PW_MAIN, "combo2", PWA_COMBOGETTEXT, NULL, buff);
    chp[0] = atoi(buff);
    WindowCmd(PW_MAIN, "combo3", PWA_COMBOGETTEXT, NULL, buff);
    chp[1] = atoi(buff);

    WindowCmd(PW_MAIN, "combo2", PWA_COMBODELETEITEMS, NULL);
    WindowCmd(PW_MAIN, "combo3", PWA_COMBODELETEITEMS, NULL);
    for (int i = 1; i <= PinCount; i++) {
        std::string spin;
        if (pboard->GetUseSpareParts()) {
            spin = SpareParts.GetPinName(i);
        } else {
            spin = pboard->MGetPinName(i);
        }
        if (spin.compare("error")) {
            WindowCmd(PW_MAIN, "combo2", PWM_COMBOADDITEM, (std::to_string(i) + "  " + spin).c_str());
            WindowCmd(PW_MAIN, "combo3", PWM_COMBOADDITEM, (std::to_string(i) + "  " + spin).c_str());
        }
    }

    if (chp[0] <= PinCount) {
        std::string spin;
        if (pboard->GetUseSpareParts()) {
            spin = SpareParts.GetPinName(chp[0]);
        } else {
            spin = pboard->MGetPinName(chp[0]);
        }
        WindowCmd(PW_MAIN, "combo2", PWA_COMBOSETTEXT, (std::to_string(chp[0]) + "  " + spin).c_str());
    } else
        WindowCmd(PW_MAIN, "combo2", PWA_COMBOSETTEXT, "1");

    if (chp[1] <= PinCount) {
        std::string spin;
        if (pboard->GetUseSpareParts()) {
            spin = SpareParts.GetPinName(chp[1]);
        } else {
            spin = pboard->MGetPinName(chp[1]);
        }
        WindowCmd(PW_MAIN, "combo3", PWA_COMBOSETTEXT, (std::to_string(chp[1]) + "  " + spin).c_str());
    } else
        WindowCmd(PW_MAIN, "combo2", PWA_COMBOSETTEXT, "2");

    float tscale;
    WindowCmd(PW_MAIN, "spind5", PWA_SPINDGETVALUE, NULL, &tscale);

    SetRT((tscale * 1e-3 * 10) / WMAX);

    if ((GetRT() / GetDT()) < 1.0) {
        xz = GetDT() / GetRT();
    } else
        xz = 1.0;

    WindowCmd(PW_MAIN, "spind6", PWA_SPINDSETMIN, std::to_string(-5 * tscale).c_str());
    WindowCmd(PW_MAIN, "spind6", PWA_SPINDSETMAX, std::to_string(5 * tscale).c_str());

    float toffset;
    WindowCmd(PW_MAIN, "spind6", PWA_SPINDGETVALUE, NULL, &toffset);

    Oscilloscope.SetTimeOffset((WMAX / 2) - (((WMAX / 2) * toffset) / (5 * tscale)));
}

int COscilloscope::WindowCmd(const int id, const char* ControlName, const PICSimLabWindowAction action,
                             const char* Value, void* ReturnBuff) {
    if (Oscilloscope.OnWindowCmd) {
        return (*Oscilloscope.OnWindowCmd)(id, ControlName, action, Value, ReturnBuff);
    }
    return -1;
}