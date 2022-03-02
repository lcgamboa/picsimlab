/* ########################################################################

   PICsimLab - PIC laboratory simulator

   ########################################################################

   Copyright (c) : 2010-2022  Luis Claudio Gambôa Lopes

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

#ifndef CPWINDOW4
#define CPWINDOW4

#include <lxrad.h>
#include <picsim/picsim.h>

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

/**
 * @brief CPWindow4 class
 *
 * class definition of PICSimLab oscilloscope window.
 */
class CPWindow4 : public CPWindow {
public:
    // lxrad automatic generated block start, don't edit below!
    /*#Controls*/
    CDraw draw1;
    CSpind spind1;
    CSpind spind2;
    CSpind spind3;
    CSpind spind4;
    CSpind spind5;
    CSpind spind6;
    CToggleButton togglebutton1;
    CButton button1;
    CToggleButton togglebutton2;
    CButton button2;
    CToggleButton togglebutton3;
    CToggleButton togglebutton4;
    CColorDialog colordialog1;
    CToggleButton togglebutton5;
    CSpind spind7;
    CCombo combo1;
    CTimer timer1;
    CCombo combo2;
    CCombo combo3;
    CLabel label1;
    CLabel label2;
    CLabel label3;
    CLabel label4;
    CLabel label5;
    CLabel label6;
    CLabel label7;
    CLabel label8;
    CLabel label9;
    CLabel label10;
    CLabel label11;
    CButton button3;
    CToggleButton togglebutton6;
    CToggleButton togglebutton7;
    CButton button4;
    CFileDialog filedialog1;
    CButton button5;
    CButton button6;
    CButton button7;
    CButton button8;
    CButton button9;
    /*#Events*/
    void _EvOnDestroy(CControl* control);
    void _EvOnShow(CControl* control);
    void _EvOnHide(CControl* control);
    void draw1_EvMouseButtonClick(CControl* control, const uint button, const uint x, const uint y, const uint state);
    void spind5_EvOnChangeSpinDouble(CControl* control);
    void spind6_EvOnChangeSpinDouble(CControl* control);
    void button1_EvMouseButtonClick(CControl* control, const uint button, const uint x, const uint y, const uint state);
    void button2_EvMouseButtonClick(CControl* control, const uint button, const uint x, const uint y, const uint state);
    void togglebutton5_EvOnToggleButton(CControl* control);
    void spind7_EvOnChangeSpinDouble(CControl* control);
    void timer1_EvOnTime(CControl* control);
    void combo2_EvOnComboChange(CControl* control);
    void combo3_EvOnComboChange(CControl* control);
    void button3_EvMouseButtonClick(CControl* control, const uint button, const uint x, const uint y, const uint state);
    void togglebutton6_EvOnToggleButton(CControl* control);
    void button4_EvMouseButtonClick(CControl* control, const uint button, const uint x, const uint y, const uint state);
    void filedialog1_EvOnClose(const int retId);
    void colordialog1_EvOnClose(const int retId);
    void button5_EvMouseButtonPress(CControl* control, const uint button, const uint x, const uint y, const uint state);
    void button6_EvMouseButtonPress(CControl* control, const uint button, const uint x, const uint y, const uint state);
    void button7_EvMouseButtonPress(CControl* control, const uint button, const uint x, const uint y, const uint state);
    void button8_EvMouseButtonPress(CControl* control, const uint button, const uint x, const uint y, const uint state);
    void button9_EvMouseButtonPress(CControl* control, const uint button, const uint x, const uint y, const uint state);

    /*#Others*/
    // lxrad automatic generated block end, don't edit above!
    CPWindow4(void);

    void WritePreferences(void);
    void ReadPreferences(char* name, char* value);
    void SetBaseTimer(void);
    void Reset(void);

    /**
     * @brief  Sample and update oscilloscope data aquisition
     */
    void SetSample(void);

    void DrawScreen(void);

    void CalculateStats(int channel);

private:
    CButton* ctrl;
    double Dt;  // Delta T
    double Rt;  // Relative delta T
    double xz;
    int usetrigger;
    double triggerlv;
    int tch;  // trigger channel
    int toffset;
    int chpin[2];
    int run;
    double databuffer[2][2][NPOINTS];  // flip buffers + 2 channels + 700 points
    int fp;                            // actual flip buffer
    double* ch[2];                     // actual channel data (pointer to databuffer)
    ch_status_t ch_status[2];          // channel measurament status
    double pins_[2];                   // last value of input pins
    int is;                            // input samples
    double t;                          // time
    int tr;                            // trigger
    int update;
    lxFont* font;
    int measures[5];
    float vmax;
};

extern CPWindow4 Window4;

#endif /*#CPWINDOW4*/
