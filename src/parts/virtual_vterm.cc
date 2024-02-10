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

#include "virtual_vterm.h"
#include "../lib/oscilloscope.h"
#include "../lib/picsimlab.h"
#include "../lib/spareparts.h"

/* outputs */
enum { O_RX, O_TX, O_LTX, O_LRX, O_VT };

/* inputs */
enum { I_TERM, I_VT };

/* line ending*/
enum { LE_NONE, LE_NL, LE_CR, LE_NL_CR, O_TERM };

static PCWProp pcwprop[6] = {{PCW_LABEL, "P1 - VCC,+5V"}, {PCW_COMBO, "P2 - RX"}, {PCW_COMBO, "P3 - TX"},
                             {PCW_LABEL, "P4 - GND,GND"}, {PCW_COMBO, "Speed"},   {PCW_END, ""}};

int cpart_vterm::count = 0;

cpart_vterm::cpart_vterm(const unsigned x, const unsigned y, const char* name, const char* type, board* pboard_,
                         const int id_)
    : part(x, y, name, type, pboard_, id_) {
    vterm_init(&vt, pboard);
    vterm_rst(&vt);

    pins[0] = 0;
    pins[1] = 0;

    show = 0;

    send_text = 0;

    vterm_speed = 9600;

    lending = LE_NL;

    wvtermId = SpareParts.WindowCmd(-1, "window1", PWA_WINDOWCREATE, NULL);

    std::string fname =
        (const char*)GetLocalFile(PICSimLab.GetSharePath() + "parts/Virtual/IO Virtual Term/terminal_1.lxrad").c_str();

    if (SpareParts.WindowCmd(wvtermId, NULL, PWA_WINDOWLOADXML, fname.c_str())) {
        SpareParts.WindowCmd(wvtermId, NULL, PWA_SETVISIBLE, "0");
        SpareParts.WindowCmd(wvtermId, NULL, PWA_WINDOWHIDE, NULL);

        SpareParts.WindowCmd(wvtermId, NULL, PWA_WINDOWPARTEV, "1");
        SpareParts.WindowCmd(wvtermId, "button1", PWA_BUTTONPARTEV, "1");
        SpareParts.WindowCmd(wvtermId, "button2", PWA_BUTTONPARTEV, "1");
        SpareParts.WindowCmd(wvtermId, "combo1", PWA_COMBOPARTEV, "1");
        SpareParts.WindowCmd(wvtermId, "combo2", PWA_COMBOPARTEV, "1");
        SpareParts.WindowCmd(wvtermId, "edit1", PWA_EDITPARTEV, "1");

        SpareParts.WindowCmd(wvtermId, "text1", PWA_TEXTTELETYPE, NULL);

        SpareParts.WindowCmd(wvtermId, "button1", PWA_SETTAG, std::to_string(id).c_str());
        SpareParts.WindowCmd(wvtermId, "button2", PWA_SETTAG, std::to_string(id).c_str());
        SpareParts.WindowCmd(wvtermId, "combo1", PWA_SETTAG, std::to_string(id).c_str());
        SpareParts.WindowCmd(wvtermId, "combo2", PWA_SETTAG, std::to_string(id).c_str());
        SpareParts.WindowCmd(wvtermId, "text1", PWA_SETTAG, std::to_string(id).c_str());
        SpareParts.WindowCmd(wvtermId, "edit1", PWA_SETTAG, std::to_string(id).c_str());
        SpareParts.WindowCmd(wvtermId, NULL, PWA_SETTAG, std::to_string(id).c_str());
    } else {
        printf("PICSimLab: Vterm error loading file %s\n", (const char*)fname.c_str());
        PICSimLab.RegisterError("Vterm error loading file:\n" + fname);
        SpareParts.WindowCmd(wvtermId, NULL, PWA_SETVISIBLE, "0");
        SpareParts.WindowCmd(wvtermId, NULL, PWA_WINDOWHIDE, NULL);
    }

    SetPCWProperties(pcwprop);

    PinCount = 2;
    Pins = pins;

    pins[0] = pboard->GetUARTTX(count);
    pins[1] = pboard->GetUARTRX(count);
    count++;
}

cpart_vterm::~cpart_vterm(void) {
    SpareParts.SetPartOnDraw(id);
    SpareParts.CanvasCmd({CC_FREEBITMAP, .FreeBitmap{BitmapId}});
    SpareParts.CanvasCmd({CC_DESTROY});
    vterm_end(&vt);

    SpareParts.WindowCmd(wvtermId, NULL, PWA_WINDOWDESTROY, NULL);
    wvtermId = -1;

    count--;
}

void cpart_vterm::RegisterRemoteControl(void) {
    input_ids[I_TERM]->status = &show;
    input_ids[I_VT]->status = &vt;
    input_ids[I_VT]->update = &output_ids[O_VT]->update;
    output_ids[O_VT]->status = &vt;
}

void cpart_vterm::ButtonEvent(const char* controlname, unsigned int button, unsigned int x, unsigned int y,
                              unsigned int state) {
    if (!strcmp(controlname, "button1")) {
        char buff[256];
        SpareParts.WindowCmd(wvtermId, "edit1", PWA_EDITGETTEXT, NULL, buff);
        text_to_send = buff;
        SpareParts.WindowCmd(wvtermId, "edit1", PWA_EDITSETTEXT, "");
        send_text = 1;
    } else if (!strcmp(controlname, "button2")) {
        SpareParts.WindowCmd(wvtermId, "text1", PWA_TEXTCLEAR, NULL);
    }
}

void cpart_vterm::KeyEvent(const char* controlname, unsigned int keysym, unsigned int ukeysym, unsigned int state) {
    if (!strcmp(controlname, "edit1")) {
        if (ukeysym == 13) {
            char buff[256];
            SpareParts.WindowCmd(wvtermId, "edit1", PWA_EDITGETTEXT, NULL, buff);
            text_to_send = buff;
            SpareParts.WindowCmd(wvtermId, "edit1", PWA_EDITSETTEXT, "");
            send_text = 1;
        }
    }
}

void cpart_vterm::Event(const char* controlname) {
    if (!strcmp(controlname, "combo2")) {
        char buff[32];
        SpareParts.WindowCmd(wvtermId, "combo2", PWA_COMBOGETTEXT, NULL, buff);
        vterm_speed = std::stoi(buff);
        vterm_set_speed(&vt, vterm_speed);
    } else if (!strcmp(controlname, "combo1")) {
        char buff[32];
        SpareParts.WindowCmd(wvtermId, "combo1", PWA_COMBOGETTEXT, NULL, buff);

        if (!strcmp(buff, "No line ending")) {
            lending = LE_NONE;
        } else if (!strcmp(buff, "New line")) {
            lending = LE_NL;
        } else if (!strcmp(buff, "Carriage return")) {
            lending = LE_CR;
        } else if (!strcmp(buff, "Both NL and CR")) {
            lending = LE_NL_CR;
        }
    } else if (!strcmp(controlname, "window1")) {
        int w, h;
        SpareParts.WindowCmd(wvtermId, NULL, PWA_GETWIDTH, NULL, &w);
        SpareParts.WindowCmd(wvtermId, NULL, PWA_GETHEIGHT, NULL, &h);

        SpareParts.WindowCmd(wvtermId, "edit1", PWA_SETWIDTH, std::to_string(w - 100).c_str());
        SpareParts.WindowCmd(wvtermId, "text1", PWA_SETWIDTH, std::to_string(w - 29).c_str());

        SpareParts.WindowCmd(wvtermId, "button1", PWA_SETX, std::to_string(w - 87).c_str());
        SpareParts.WindowCmd(wvtermId, "button2", PWA_SETX, std::to_string(w - 90).c_str());
        SpareParts.WindowCmd(wvtermId, "combo1", PWA_SETX, std::to_string(w - 405).c_str());
        SpareParts.WindowCmd(wvtermId, "combo2", PWA_SETX, std::to_string(w - 215).c_str());

        SpareParts.WindowCmd(wvtermId, "text1", PWA_SETHEIGHT, std::to_string(h - 119).c_str());

        SpareParts.WindowCmd(wvtermId, "button2", PWA_SETY, std::to_string(h - 74).c_str());
        SpareParts.WindowCmd(wvtermId, "combo1", PWA_SETY, std::to_string(h - 73).c_str());
        SpareParts.WindowCmd(wvtermId, "combo2", PWA_SETY, std::to_string(h - 73).c_str());
    }
}

void cpart_vterm::Reset(void) {
    vterm_rst(&vt);
    vterm_set_speed(&vt, vterm_speed);

    SpareParts.WindowCmd(wvtermId, "combo2", PWA_COMBOSETTEXT, std::to_string(vterm_speed).c_str());

    switch (lending) {
        case LE_NONE:
            SpareParts.WindowCmd(wvtermId, "combo1", PWA_COMBOSETTEXT, "No line ending");
            break;
        case LE_NL:
            SpareParts.WindowCmd(wvtermId, "combo1", PWA_COMBOSETTEXT, "New line");
            break;
        case LE_CR:
            SpareParts.WindowCmd(wvtermId, "combo1", PWA_COMBOSETTEXT, "Carriage return");
            break;
        case LE_NL_CR:
            SpareParts.WindowCmd(wvtermId, "combo1", PWA_COMBOSETTEXT, "Both NL and CR");
            break;
    }
}

void cpart_vterm::DrawOutput(const unsigned int i) {
    switch (output[i].id) {
        case O_LTX:
            SpareParts.CanvasCmd({CC_SETCOLOR, .SetColor{0, (vt.bb_uart.leds & 0x02) * 125, 0}});
            SpareParts.CanvasCmd({CC_RECTANGLE, .Rectangle{1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1,
                                                           output[i].y2 - output[i].y1}});
            vt.bb_uart.leds &= ~0x02;
            break;
        case O_LRX:
            SpareParts.CanvasCmd({CC_SETCOLOR, .SetColor{0, (vt.bb_uart.leds & 0x01) * 250, 0}});
            SpareParts.CanvasCmd({CC_RECTANGLE, .Rectangle{1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1,
                                                           output[i].y2 - output[i].y1}});
            vt.bb_uart.leds &= ~0x01;
            break;
        case O_TERM:
            char str[SBUFFMAX];

            if (show & 0x80) {
                if (show & 0x01) {
                    SpareParts.WindowCmd(wvtermId, NULL, PWA_WINDOWSHOW, NULL);
                } else {
                    SpareParts.WindowCmd(wvtermId, NULL, PWA_WINDOWHIDE, NULL);
                }
                show &= ~0x80;
            }

            vt.inMutex->Lock();
            strncpy(str, (char*)vt.buff_in, vt.count_in);
            str[vt.count_in] = 0;
            vt.count_in = 0;
            // printf("Data recv: \n[\n%s]\n", str);
            vt.inMutex->Unlock();

            SpareParts.WindowCmd(wvtermId, "text1", PWA_TEXTAPPEND, str);
            break;
        default:
            SpareParts.CanvasCmd({CC_SETFONTSIZE, .SetFontSize{8}});
            SpareParts.CanvasCmd({CC_SETCOLOR, .SetColor{49, 61, 99}});
            SpareParts.CanvasCmd({CC_RECTANGLE, .Rectangle{1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1,
                                                           output[i].y2 - output[i].y1}});

            SpareParts.CanvasCmd({CC_SETFGCOLOR, .SetFgColor{155, 155, 155}});

            int pinv = output[i].id - O_RX;
            switch (pinv) {
                case 0:
                    if (pins[pinv] == 0)
                        SpareParts.CanvasCmd({CC_ROTATEDTEXT, .RotatedText{"NC", output[i].x1, output[i].y2, 90.0}});
                    else
                        SpareParts.CanvasCmd({CC_ROTATEDTEXT, .RotatedText{SpareParts.GetPinName(pins[pinv]).c_str(),
                                                                           output[i].x1, output[i].y2, 90.0}});
                case 1:
                    if (pins[pinv] == 0)
                        SpareParts.CanvasCmd({CC_ROTATEDTEXT, .RotatedText{"NC", output[i].x1, output[i].y2, 90.0}});
                    else
                        SpareParts.CanvasCmd({CC_ROTATEDTEXT, .RotatedText{SpareParts.GetPinName(pins[pinv]).c_str(),
                                                                           output[i].x1, output[i].y2, 90.0}});
                    break;
            }
            break;
    }
}

unsigned short cpart_vterm::GetInputId(char* name) {
    if (strcmp(name, "PB_TERM") == 0)
        return I_TERM;
    if (strcmp(name, "VT_VTERM") == 0)
        return I_VT;

    printf("Error input '%s' don't have a valid id! \n", name);
    return INVALID_ID;
}

unsigned short cpart_vterm::GetOutputId(char* name) {
    if (strcmp(name, "PN_RX") == 0)
        return O_RX;
    if (strcmp(name, "PN_TX") == 0)
        return O_TX;
    if (strcmp(name, "LD_TX") == 0)
        return O_LTX;
    if (strcmp(name, "LD_RX") == 0)
        return O_LRX;
    if (strcmp(name, "PB_TERM") == 0)
        return O_TERM;
    if (strcmp(name, "VT_VTERM") == 0)
        return O_VT;

    printf("Error output '%s' don't have a valid id! \n", name);
    return INVALID_ID;
}

std::string cpart_vterm::WritePreferences(void) {
    char prefs[256];

    int x, y, w, h;

    SpareParts.WindowCmd(wvtermId, NULL, PWA_GETX, NULL, &x);
    SpareParts.WindowCmd(wvtermId, NULL, PWA_GETY, NULL, &y);
    SpareParts.WindowCmd(wvtermId, NULL, PWA_GETWIDTH, NULL, &w);
    SpareParts.WindowCmd(wvtermId, NULL, PWA_GETHEIGHT, NULL, &h);

    sprintf(prefs, "%hhu,%hhu,%hhu,%u,%hhu,%i,%i,%i,%i", pins[0], pins[1], lending, vterm_speed, show, x, y, w, h);

    return prefs;
}

void cpart_vterm::ReadPreferences(std::string value) {
    int x, y, w, h;
    int ret = sscanf(value.c_str(), "%hhu,%hhu,%hhu,%u,%hhu,%i,%i,%i,%i", &pins[0], &pins[1], &lending, &vterm_speed,
                     &show, &x, &y, &w, &h);
    show |= 0x80;

    SpareParts.WindowCmd(wvtermId, NULL, PWA_SETX, std::to_string(x).c_str());
    SpareParts.WindowCmd(wvtermId, NULL, PWA_SETY, std::to_string(y).c_str());

    if (ret != 9) {  // for compatibility with older versions
        w = 530;
        h = 400;
    }
    if (w < 100) {
        w = 100;
    }
    if (h < 100) {
        h = 100;
    }
    SpareParts.WindowCmd(wvtermId, NULL, PWA_SETWIDTH, std::to_string(w).c_str());
    SpareParts.WindowCmd(wvtermId, NULL, PWA_SETHEIGHT, std::to_string(h).c_str());

    Reset();
}

void cpart_vterm::ConfigurePropertiesWindow(void) {
    SetPCWComboWithPinNames("combo2", pins[0]);
    SetPCWComboWithPinNames("combo3", pins[1]);

    SpareParts.WPropCmd("combo5", PWA_COMBOSETITEMS, "1200,2400,4800,9600,19200,38400,57600,115200,");
    SpareParts.WPropCmd("combo5", PWA_COMBOSETTEXT, std::to_string(vterm_speed).c_str());
}

void cpart_vterm::ReadPropertiesWindow(void) {
    pins[0] = GetPWCComboSelectedPin("combo2");
    pins[1] = GetPWCComboSelectedPin("combo3");

    char buff[64];
    SpareParts.WPropCmd("combo5", PWA_COMBOGETTEXT, NULL, buff);

    vterm_speed = std::stoi(buff);
    Reset();
}

void cpart_vterm::PreProcess(void) {
    if (send_text) {
        if (!vt.count_out) {
            send_text = 0;
            vt.count_out = text_to_send.size();
            for (unsigned int i = 0; i < vt.count_out; i++) {
                vt.buff_out[i] = text_to_send[i];
            }
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
    Process();  // check for input updates
}

void cpart_vterm::Process(void) {
    int ret = 0;

    unsigned char val = 1;

    const picpin* ppins = SpareParts.GetPinsValues();

    if (pins[0]) {
        val = ppins[pins[0] - 1].value;
    }

    ret = vterm_io(&vt, val);
    SpareParts.SetPin(pins[1], ret);
}

void cpart_vterm::OnMouseButtonPress(unsigned int inputId, unsigned int button, unsigned int x, unsigned int y,
                                     unsigned int state) {
    switch (inputId) {
        case I_TERM:
            if (button == 1) {
                show = (show & 0x01) ^ 1;
                show |= 0x80;
            }
            break;
    }
}

void cpart_vterm::PostProcess(void) {
    if (show & 0x80) {
        output_ids[O_TERM]->update = 1;
    }

    if (output_ids[O_LTX]->value != (vt.bb_uart.leds & 0x02)) {
        output_ids[O_LTX]->value = (vt.bb_uart.leds & 0x02);
        output_ids[O_LTX]->update = 1;
    }

    if (output_ids[O_LRX]->value != (vt.bb_uart.leds & 0x01)) {
        output_ids[O_LRX]->value = (vt.bb_uart.leds & 0x01);
        output_ids[O_LRX]->update = 1;
    }
    if (vt.count_in) {
        output_ids[O_TERM]->update = 1;
    }
}

part_init(PART_vterm_Name, cpart_vterm, "Virtual");
