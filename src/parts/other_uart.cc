/* ########################################################################

   PICSimLab - Programmable IC Simulator Laboratory

   ########################################################################

   Copyright (c) : 2020-2025  Luis Claudio Gambôa Lopes <lcgamboa@yahoo.com>

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

#include "other_uart.h"
#include "../lib/oscilloscope.h"
#include "../lib/picsimlab.h"
#include "../lib/spareparts.h"

/* outputs */
enum { O_RX, O_TX, O_FILE, O_LCON, O_LTX, O_LRX };

/* inputs */
enum { I_CONN };

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

static PCWProp pcwprop[7] = {
    {PCW_LABEL, "P1 - GND,GND"}, {PCW_COMBO, "P2 - RX"}, {PCW_COMBO, "P3 - TX"}, {PCW_LABEL, "P4 - VCC,+5V"},
    {PCW_COMBO, "Port"},         {PCW_COMBO, "Speed"},   {PCW_END, ""}};

cpart_UART::cpart_UART(const unsigned x, const unsigned y, const char* name, const char* type, board* pboard_,
                       const int id_)
    : part(x, y, name, type, pboard_, id_) {
    uart_init(&sr, pboard);
    uart_rst(&sr);

    pins[0] = 0;
    pins[1] = 0;

    _ret = -1;

    uart_name[0] = '*';
    uart_name[1] = 0;
    uart_speed = 9600;

    SetPCWProperties(pcwprop);

    PinCount = 2;
    Pins = pins;
}

cpart_UART::~cpart_UART(void) {
    SpareParts.SetPartOnDraw(id);
    SpareParts.CanvasCmd({.cmd = CC_FREEBITMAP, .FreeBitmap{BitmapId}});
    SpareParts.CanvasCmd({.cmd = CC_DESTROY});
    uart_end(&sr);
}

void cpart_UART::Reset(void) {
    uart_rst(&sr);
}

void cpart_UART::DrawOutput(const unsigned int i) {
    switch (output[i].id) {
        case O_LTX:
            SpareParts.CanvasCmd({.cmd = CC_SETCOLOR, .SetColor{0, (sr.bb_uart.leds & 0x02) * 125, 0}});
            SpareParts.CanvasCmd(
                {.cmd = CC_RECTANGLE,
                 .Rectangle{1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1}});
            sr.bb_uart.leds &= ~0x02;
            break;
        case O_LRX:
            SpareParts.CanvasCmd({.cmd = CC_SETCOLOR, .SetColor{0, (sr.bb_uart.leds & 0x01) * 250, 0}});
            SpareParts.CanvasCmd(
                {.cmd = CC_RECTANGLE,
                 .Rectangle{1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1}});
            sr.bb_uart.leds &= ~0x01;
            break;
        case O_LCON:
            SpareParts.CanvasCmd({.cmd = CC_SETCOLOR, .SetColor{255, 0, 0}});
            SpareParts.CanvasCmd(
                {.cmd = CC_RECTANGLE,
                 .Rectangle{1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1}});
            break;
        case O_FILE:
            SpareParts.CanvasCmd({.cmd = CC_SETFONTSIZE, .SetFontSize{8}});
            SpareParts.CanvasCmd({.cmd = CC_SETCOLOR, .SetColor{49, 61, 99}});
            SpareParts.CanvasCmd(
                {.cmd = CC_RECTANGLE,
                 .Rectangle{1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1}});
            SpareParts.CanvasCmd({.cmd = CC_SETFGCOLOR, .SetFgColor{255, 255, 255}});
            SpareParts.CanvasCmd(
                {.cmd = CC_ROTATEDTEXT,
                 .RotatedText{("port:" + std::string(uart_name) + "   speed:" + std::to_string(uart_speed)).c_str(),
                              output[i].x1, output[i].y1, 0}});
            break;
        default:
            SpareParts.CanvasCmd({.cmd = CC_SETCOLOR, .SetColor{49, 61, 99}});
            SpareParts.CanvasCmd(
                {.cmd = CC_RECTANGLE,
                 .Rectangle{1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1}});

            SpareParts.CanvasCmd({.cmd = CC_SETFGCOLOR, .SetFgColor{155, 155, 155}});

            int pinv = output[i].id - O_RX;
            int pin = 0;
            switch (pinv) {
                case 0:
                case 1:
                    pin = pinv;
                    if (pins[pin] == 0)
                        SpareParts.CanvasCmd(
                            {.cmd = CC_ROTATEDTEXT, .RotatedText{"NC", output[i].x1, output[i].y2, 90.0}});
                    else
                        SpareParts.CanvasCmd(
                            {.cmd = CC_ROTATEDTEXT,
                             .RotatedText{SpareParts.GetPinName(pins[pin]).c_str(), output[i].x1, output[i].y2, 90.0}});
            }
            break;
    }
}

unsigned short cpart_UART::GetInputId(char* name) {
    if (strcmp(name, "CN_CONN") == 0)
        return I_CONN;

    printf("Error input '%s' don't have a valid id! \n", name);
    return INVALID_ID;
}

unsigned short cpart_UART::GetOutputId(char* name) {
    if (strcmp(name, "PN_RX") == 0)
        return O_RX;
    if (strcmp(name, "PN_TX") == 0)
        return O_TX;
    if (strcmp(name, "LD_CON") == 0)
        return O_LCON;
    if (strcmp(name, "LD_TX") == 0)
        return O_LTX;
    if (strcmp(name, "LD_RX") == 0)
        return O_LRX;
    if (strcmp(name, "DI_FILE") == 0)
        return O_FILE;

    printf("Error output '%s' don't have a valid id! \n", name);
    return INVALID_ID;
}

std::string cpart_UART::WritePreferences(void) {
    char prefs[256];

    sprintf(prefs, "%hhu,%hhu,%u,%s", pins[0], pins[1], uart_speed, uart_name);

    return prefs;
}

void cpart_UART::ReadPreferences(std::string value) {
    sscanf(value.c_str(), "%hhu,%hhu,%u,%[^\n]", &pins[0], &pins[1], &uart_speed, uart_name);

    Reset();
    if (uart_name[0] != '*') {
        uart_set_port(&sr, uart_name, uart_speed);
    }
}

void cpart_UART::ConfigurePropertiesWindow(void) {
    SetPCWComboWithPinNames("combo2", pins[0]);
    SetPCWComboWithPinNames("combo3", pins[1]);

    char* resp = serial_port_list();
    if (resp) {
        SpareParts.WPropCmd("combo5", PWA_COMBOSETITEMS, resp);
        free(resp);
    }
    if (uart_name[0] == '*')
        SpareParts.WPropCmd("combo5", PWA_COMBOSETTEXT, " ");
    else {
        SpareParts.WPropCmd("combo5", PWA_COMBOSETTEXT, uart_name);
    }

    SpareParts.WPropCmd("combo6", PWA_COMBOSETITEMS, "1200,2400,4800,9600,19200,38400,57600,115200,");
    SpareParts.WPropCmd("combo6", PWA_COMBOSETTEXT, std::to_string(uart_speed).c_str());
}

void cpart_UART::ReadPropertiesWindow(void) {
    pins[0] = GetPWCComboSelectedPin("combo2");
    pins[1] = GetPWCComboSelectedPin("combo3");

    char buff[200];
    SpareParts.WPropCmd("combo5", PWA_COMBOGETTEXT, NULL, buff);
    strncpy(uart_name, buff, 200);

    SpareParts.WPropCmd("combo6", PWA_COMBOGETTEXT, NULL, buff);
    uart_speed = std::stoi(buff);

    uart_set_port(&sr, uart_name, uart_speed);
}

void cpart_UART::PreProcess(void) {
    Process();  // check for input updates
}

void cpart_UART::Process(void) {
    const picpin* ppins = SpareParts.GetPinsValues();

    unsigned short ret = 0;

    unsigned char val;

    if (pins[0]) {
        val = ppins[pins[0] - 1].value;
    } else {
        val = 1;
    }
    ret = uart_io(&sr, val);

    if (_ret != ret) {
        SpareParts.SetPin(pins[1], ret);
    }
    _ret = ret;
}

void cpart_UART::PostProcess(void) {
    if (output_ids[O_LTX]->value != (sr.bb_uart.leds & 0x02)) {
        output_ids[O_LTX]->value = (sr.bb_uart.leds & 0x02);
        output_ids[O_LTX]->update = 1;
    }

    if (output_ids[O_LRX]->value != (sr.bb_uart.leds & 0x01)) {
        output_ids[O_LRX]->value = (sr.bb_uart.leds & 0x01);
        output_ids[O_LRX]->update = 1;
    }
}

part_init(PART_UART_Name, cpart_UART, "Other");
