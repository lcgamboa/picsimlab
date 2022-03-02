/* ########################################################################

   PICsimLab - PIC laboratory simulator

   ########################################################################

   Copyright (c) : 2020-2022  Luis Claudio Gambôa Lopes

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

#include "part_vterm.h"
#include "../picsimlab1.h"
#include "../picsimlab4.h"
#include "../picsimlab5.h"

/* outputs */
enum { O_RX, O_TX, O_LTX, O_LRX };

/* inputs */
enum { I_TERM };

/* line ending*/
enum { LE_NONE, LE_NL, LE_CR, LE_NL_CR };

/*
const char pin_names[10][10] = {
 "GND",
 "VCC",
 "MISO",
 "MOSI",
 "SCK",
 "CS"
};

const char pin_values[10][10] = {
 "GND",
 "+5V",
 {0},
 {1},
 {2},
 {3}
};
 */

cpart_vterm::cpart_vterm(unsigned x, unsigned y)
    : font(8, lxFONTFAMILY_TELETYPE, lxFONTSTYLE_NORMAL, lxFONTWEIGHT_BOLD) {
    aways_update = 1;
    X = x;
    Y = y;
    ReadMaps();
    Bitmap = NULL;

    LoadImage();

    vterm_init(&vt);
    vterm_rst(&vt);

    input_pins[0] = 0;

    output_pins[0] = 0;

    _ret = -1;

    send_text = 0;

    vterm_speed = 9600;

    lending = LE_NL;

    wvterm = new CPWindow();
    wvterm->SetName("window1");  // must be the same as in xml
    wvterm->SetVisible(0);
    Application->ACreateWindow(wvterm);
    wvterm->LoadXMLContextAndCreateChilds(Window1.GetSharePath() + lxT("parts/IO Virtual Term/terminal_1.lxrad"));
    wvterm->SetVisible(0);
    wvterm->Hide();
    wvterm->SetCanDestroy(false);

    wvterm->EvOnShow = EVONSHOW & CPWindow5::PartEvent;

    // wvterm.Draw ();
    // wvterm.Show ();

    vttext = (CText*)wvterm->GetChildByName("text1");
    vtedit = (CEdit*)wvterm->GetChildByName("edit1");

    vtbtn_send = ((CButton*)wvterm->GetChildByName("button1"));
    vtbtn_clear = ((CButton*)wvterm->GetChildByName("button2"));
    vtcmb_ending = ((CCombo*)wvterm->GetChildByName("combo1"));
    vtcmb_speed = ((CCombo*)wvterm->GetChildByName("combo2"));

    vtbtn_send->EvMouseButtonRelease = EVMOUSEBUTTONRELEASE & CPWindow5::PartButtonEvent;
    vtbtn_clear->EvMouseButtonRelease = EVMOUSEBUTTONRELEASE & CPWindow5::PartButtonEvent;
    vtcmb_speed->EvOnComboChange = EVONCOMBOCHANGE & CPWindow5::PartEvent;
    vtcmb_ending->EvOnComboChange = EVONCOMBOCHANGE & CPWindow5::PartEvent;
    vtedit->EvKeyboardPress = EVKEYBOARDPRESS & CPWindow5::PartKeyEvent;
}

cpart_vterm::~cpart_vterm(void) {
    delete Bitmap;
    canvas.Destroy();
    vterm_end(&vt);
    wvterm->Hide();
    wvterm->DestroyChilds();
    wvterm->SetCanDestroy(true);
    wvterm->WDestroy();
}

void cpart_vterm::SetId(int _id) {
    part::SetId(_id);
    vtbtn_send->SetTag(id);
    vtbtn_clear->SetTag(id);
    vtcmb_speed->SetTag(id);
    vtcmb_ending->SetTag(id);
    vtedit->SetTag(id);
    wvterm->SetTag(id);
}

void cpart_vterm::ButtonEvent(CControl* control, uint button, uint x, uint y, uint state) {
    if (control == vtbtn_send) {
        send_text = 1;
    } else if (control == vtbtn_clear) {
        vttext->Clear();
    }
}

void cpart_vterm::KeyEvent(CControl* control, uint keysym, uint ukeysym, uint state) {
    if (control == vtedit) {
        if (ukeysym == 13) {
            send_text = 1;
        }
    }
}

void cpart_vterm::Event(CControl* control) {
    if (control == vtcmb_speed) {
        vterm_speed = atoi(vtcmb_speed->GetText());
        vterm_set_speed(&vt, vterm_speed);
    } else if (control == vtcmb_ending) {
        if (!vtcmb_ending->GetText().Cmp("No line ending")) {
            lending = LE_NONE;
        } else if (!vtcmb_ending->GetText().Cmp("New line")) {
            lending = LE_NL;
        } else if (!vtcmb_ending->GetText().Cmp("Carriage return")) {
            lending = LE_CR;
        } else if (!vtcmb_ending->GetText().Cmp("Both NL and CR")) {
            lending = LE_NL_CR;
        }
    } else if (control == wvterm) {
        vtedit->SetWidth(wvterm->GetWidth() - 100);
        vttext->SetWidth(wvterm->GetWidth() - 29);

        vtbtn_send->SetX(wvterm->GetWidth() - 87);
        vtbtn_clear->SetX(wvterm->GetWidth() - 90);
        vtcmb_ending->SetX(wvterm->GetWidth() - 405);
        vtcmb_speed->SetX(wvterm->GetWidth() - 215);

        vttext->SetHeight(wvterm->GetHeight() - 119);

        vtbtn_clear->SetY(wvterm->GetHeight() - 74);
        vtcmb_ending->SetY(wvterm->GetHeight() - 73);
        vtcmb_speed->SetY(wvterm->GetHeight() - 73);
    }
}

void cpart_vterm::Reset(void) {
    vterm_rst(&vt);
    vterm_set_speed(&vt, vterm_speed);

    vtcmb_speed->SetText(itoa(vterm_speed));

    switch (lending) {
        case LE_NONE:
            vtcmb_ending->SetText("No line ending");
            break;
        case LE_NL:
            vtcmb_ending->SetText("New line");
            break;
        case LE_CR:
            vtcmb_ending->SetText("Carriage return");
            break;
        case LE_NL_CR:
            vtcmb_ending->SetText("Both NL and CR");
            break;
    }
}

void cpart_vterm::Draw(void) {
    int i;

    Update = 0;

    for (i = 0; i < outputc; i++) {
        if (output[i].update)  // only if need update
        {
            output[i].update = 0;

            if (!Update) {
                canvas.Init(Scale, Scale, Orientation);
                canvas.SetFont(font);
            }
            Update++;  // set to update buffer

            switch (output[i].id) {
                case O_LTX:
                    canvas.SetColor(0, (vt.bb_uart.leds & 0x02) * 125, 0);
                    canvas.Rectangle(1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1,
                                     output[i].y2 - output[i].y1);
                    vt.bb_uart.leds &= ~0x02;
                    break;
                case O_LRX:
                    canvas.SetColor(0, (vt.bb_uart.leds & 0x01) * 250, 0);
                    canvas.Rectangle(1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1,
                                     output[i].y2 - output[i].y1);
                    vt.bb_uart.leds &= ~0x01;
                    break;
                default:
                    canvas.SetColor(49, 61, 99);
                    canvas.Rectangle(1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1,
                                     output[i].y2 - output[i].y1);

                    canvas.SetFgColor(155, 155, 155);

                    int pinv = output[i].id - O_RX;
                    int pin = 0;
                    switch (pinv) {
                        case 0:
                            pin = pinv;
                            if (input_pins[pin] == 0)
                                canvas.RotatedText("NC", output[i].x1, output[i].y2, 90.0);
                            else
                                canvas.RotatedText(Window5.GetPinName(input_pins[pin]), output[i].x1, output[i].y2,
                                                   90.0);
                        case 1:
                            pin = pinv - 1;
                            if (output_pins[pin] == 0)
                                canvas.RotatedText("NC", output[i].x1, output[i].y2, 90.0);
                            else
                                canvas.RotatedText(Window5.GetPinName(output_pins[pin]), output[i].x1, output[i].y2,
                                                   90.0);
                            break;
                    }
                    break;
            }
        }
    }

    if (Update) {
        canvas.End();
    }

    if (vt.count_in) {
        char str[200];
        strncpy(str, (char*)vt.buff_in, vt.count_in);
        str[vt.count_in] = 0;
        vttext->Append(str);
        vt.count_in = 0;

        while (vttext->GetCountLines() > 1000) {
            vttext->SetCursorPos(0);
            vttext->DelLine();
        }
    }
}

unsigned short cpart_vterm::get_in_id(char* name) {
    if (strcmp(name, "PB_TERM") == 0)
        return I_TERM;

    printf("Erro input '%s' don't have a valid id! \n", name);
    return -1;
}

unsigned short cpart_vterm::get_out_id(char* name) {
    if (strcmp(name, "PN_RX") == 0)
        return O_RX;
    if (strcmp(name, "PN_TX") == 0)
        return O_TX;
    if (strcmp(name, "LD_TX") == 0)
        return O_LTX;
    if (strcmp(name, "LD_RX") == 0)
        return O_LRX;

    printf("Erro output '%s' don't have a valid id! \n", name);
    return 1;
}

lxString cpart_vterm::WritePreferences(void) {
    char prefs[256];

    sprintf(prefs, "%hhu,%hhu,%hhu,%u", input_pins[0], output_pins[0], lending, vterm_speed);

    return prefs;
}

void cpart_vterm::ReadPreferences(lxString value) {
    sscanf(value.c_str(), "%hhu,%hhu,%hhu,%u", &input_pins[0], &output_pins[0], &lending, &vterm_speed);

    Reset();
}

void cpart_vterm::ConfigurePropertiesWindow(CPWindow* WProp) {
    lxString Items = Window5.GetPinsNames();
    lxString spin;

    ((CCombo*)WProp->GetChildByName("combo1"))->SetItems(Items);
    if (input_pins[0] == 0)
        ((CCombo*)WProp->GetChildByName("combo1"))->SetText("0  NC");
    else {
        spin = Window5.GetPinName(input_pins[0]);
        ((CCombo*)WProp->GetChildByName("combo1"))->SetText(itoa(input_pins[0]) + "  " + spin);
    }

    ((CCombo*)WProp->GetChildByName("combo2"))->SetItems(Items);
    if (output_pins[0] == 0)
        ((CCombo*)WProp->GetChildByName("combo2"))->SetText("0  NC");
    else {
        spin = Window5.GetPinName(output_pins[0]);
        ((CCombo*)WProp->GetChildByName("combo2"))->SetText(itoa(output_pins[0]) + "  " + spin);
    }

    ((CCombo*)WProp->GetChildByName("combo3"))->SetItems("1200,2400,4800,9600,19200,38400,57600,115200,");
    ((CCombo*)WProp->GetChildByName("combo3"))->SetText(itoa(vterm_speed));

    ((CButton*)WProp->GetChildByName("button1"))->EvMouseButtonRelease =
        EVMOUSEBUTTONRELEASE & CPWindow5::PropButtonRelease;
    ((CButton*)WProp->GetChildByName("button1"))->SetTag(1);

    ((CButton*)WProp->GetChildByName("button2"))->EvMouseButtonRelease =
        EVMOUSEBUTTONRELEASE & CPWindow5::PropButtonRelease;
}

void cpart_vterm::ReadPropertiesWindow(CPWindow* WProp) {
    input_pins[0] = atoi(((CCombo*)WProp->GetChildByName("combo1"))->GetText());
    output_pins[0] = atoi(((CCombo*)WProp->GetChildByName("combo2"))->GetText());
    vterm_speed = atoi(((CCombo*)WProp->GetChildByName("combo3"))->GetText());

    Reset();
}

void cpart_vterm::PreProcess(void) {
    vterm_set_clk_freq(&vt, Window1.GetBoard()->MGetInstClockFreq());

    if (send_text) {
        if (!vt.count_out) {
            send_text = 0;
            vt.count_out = vtedit->GetText().size();
            for (int i = 0; i < vt.count_out; i++) {
                vt.buff_out[i] = vtedit->GetText()[i];
            }
            vtedit->SetText("");
            switch (lending) {
                case LE_NL:
                    vt.buff_out[vt.count_out++] = '\n';
                    break;
                case LE_CR:
                    vt.buff_out[vt.count_out++] = '\r';
                    break;
                case LE_NL_CR:
                    vt.buff_out[vt.count_out++] = '\r';
                    vt.buff_out[vt.count_out++] = '\n';
                    break;
            }
        }
    }
}

void cpart_vterm::Process(void) {
    int ret = 0;

    unsigned char val = 1;

    const picpin* ppins = Window5.GetPinsValues();

    if (input_pins[0]) {
        val = ppins[input_pins[0] - 1].value;
    }

    ret = vterm_io(&vt, val);

    if (_ret != ret) {
        Window5.SetPin(output_pins[0], ret);
    }
    _ret = ret;
}

void cpart_vterm::EvMouseButtonPress(uint button, uint x, uint y, uint state) {
    int i;

    for (i = 0; i < inputc; i++) {
        if (PointInside(x, y, input[i])) {
            switch (input[i].id) {
                case I_TERM:
                    if (button == 1) {
                        wvterm->Show();
                    }
                    break;
            }
        }
    }
}

void cpart_vterm::PostProcess(void) {
    if (output_ids[O_LTX]->value != (vt.bb_uart.leds & 0x02)) {
        output_ids[O_LTX]->value = (vt.bb_uart.leds & 0x02);
        output_ids[O_LTX]->update = 1;
    }

    if (output_ids[O_LRX]->value != (vt.bb_uart.leds & 0x01)) {
        output_ids[O_LRX]->value = (vt.bb_uart.leds & 0x01);
        output_ids[O_LRX]->update = 1;
    }
}

part_init(PART_vterm_Name, cpart_vterm, "Virtual");
