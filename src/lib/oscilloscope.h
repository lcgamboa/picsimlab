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

#ifndef OSCILLOSCOPE
#define OSCILLOSCOPE

#include <vector>
#include "board.h"

#define WMAX 350
#define HMAX 250

#define NPOINTS (2 * WMAX)

#define MAX_MEASURES 10

typedef struct {
    double Vrms;
    double Vavr;
    double Vmax;
    double Vmin;
    double Freq;
    double FCycle_ms;
    double PCycle_ms;
    double Duty;
} ch_status_t;

class COscilloscope {
public:
    COscilloscope();

    void Init(CWindow* win);

    /**
     * @brief  Sample and update oscilloscope data aquisition
     */
    void SetSample(void);

    void SetBoard(board* b) { pboard = b; };

    void NextMeasure(int mn);

    int GetMeasures(int mn);

    void SetMeasure(int nm, int value);

    int GetUseTrigger(void) { return usetrigger; };
    void SetUseTrigger(int utg) { usetrigger = utg; };

    int GetTriggerChannel(void) { return tch; };
    void SetTriggerChannel(int tc) { tch = tc; };

    double* GetChannel(int cn) { return ch[cn]; };

    void CalculateStats(int channel);
    void ClearStats(int channel);

    ch_status_t GetChannelStatus(int cn) { return ch_status[cn]; };

    int GetUpdate(void) { return update; };
    void SetUpdate(int up) { update = up; };

    void SetChannelPin(int ch, int pin) { chpin[ch] = pin; };

    int GetTimeOffset(void) { return toffset; };
    void SetTimeOffset(int to) { toffset = to; };

    int GetRun(void) { return run; };
    void SetRun(int r) { run = r; };

    void SetTriggerLevel(double tl) { triggerlv = tl; };

    void SetVMax(float vm) { vmax = vm; };

    void SetRT(double rt) { Rt = rt; };
    double GetRT(void) { return Rt; };

    void SetDT(double dt) { Dt = dt; };
    double GetDT(void) { return Dt; };

    void Reset(void);

    void SetBaseTimer(void);

    CWindow* GetWindow(void) { return Window; };

    void WritePreferences(void);
    void ReadPreferences(char* name, char* value);

    std::vector<std::string> WritePreferencesList(void);
    void ReadPreferencesList(std::vector<std::string>& pl);

private:
    CWindow* Window;
    board* pboard;
    double Dt;  // Delta T
    double Rt;  // Relative delta T
    int usetrigger;
    double triggerlv;
    int tch;  // trigger channel
    int toffset;
    int chpin[2];
    double databuffer[2][2][NPOINTS];  // flip buffers + 2 channels + 700 points
    int fp;                            // actual flip buffer
    double* ch[2];                     // actual channel data (pointer to databuffer)
    ch_status_t ch_status[2];          // channel measurament status
    double pins_[2];                   // last value of input pins
    int is;                            // input samples
    double t;                          // time
    int tr;                            // trigger
    int run;
    int update;
    int measures[5];
    float vmax;

    CToggleButton* tbstop;
    CToggleButton* tbsingle;
};

extern COscilloscope Oscilloscope;

#endif  // OSCILLOSCOPE