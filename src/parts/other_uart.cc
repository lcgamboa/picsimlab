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

cpart_UART::cpart_UART(const unsigned x, const unsigned y, const char* name, const char* type, board* pboard_)
    : part(x, y, name, type, pboard_), font(8, lxFONTFAMILY_TELETYPE, lxFONTSTYLE_NORMAL, lxFONTWEIGHT_BOLD) {
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
    delete Bitmap;
    canvas.Destroy();
    uart_end(&sr);
}

void cpart_UART::Reset(void) {
    uart_rst(&sr);
}

void cpart_UART::DrawOutput(const unsigned int i) {
    switch (output[i].id) {
        case O_LTX:
            canvas.SetColor(0, (sr.bb_uart.leds & 0x02) * 125, 0);
            canvas.Rectangle(1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);
            sr.bb_uart.leds &= ~0x02;
            break;
        case O_LRX:
            canvas.SetColor(0, (sr.bb_uart.leds & 0x01) * 250, 0);
            canvas.Rectangle(1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);
            sr.bb_uart.leds &= ~0x01;
            break;
        case O_LCON:
            canvas.SetColor(255, 0, 0);
            canvas.Rectangle(1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);
            break;
        case O_FILE:
            canvas.SetColor(49, 61, 99);
            canvas.Rectangle(1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);
            canvas.SetFgColor(255, 255, 255);
            canvas.RotatedText("port:" + std::string(uart_name) + "   speed:" + std::to_string(uart_speed), output[i].x1,
                               output[i].y1, 0);
            break;
        default:
            canvas.SetColor(49, 61, 99);
            canvas.Rectangle(1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);

            canvas.SetFgColor(155, 155, 155);

            int pinv = output[i].id - O_RX;
            int pin = 0;
            switch (pinv) {
                case 0:
                case 1:
                    pin = pinv;
                    if (pins[pin] == 0)
                        canvas.RotatedText("NC", output[i].x1, output[i].y2, 90.0);
                    else
                        canvas.RotatedText(SpareParts.GetPinName(pins[pin]), output[i].x1, output[i].y2, 90.0);
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
    sscanf(value.c_str(), "%hhu,%hhu,%u,%s", &pins[0], &pins[1], &uart_speed, uart_name);

    Reset();
    if (uart_name[0] != '*') {
        uart_set_port(&sr, uart_name, uart_speed);
    }
}

void cpart_UART::ConfigurePropertiesWindow(CPWindow* WProp) {
    SetPCWComboWithPinNames(WProp, "combo2", pins[0]);
    SetPCWComboWithPinNames(WProp, "combo3", pins[1]);

    char* resp = serial_port_list();
    if (resp) {
        ((CCombo*)WProp->GetChildByName("combo5"))->SetItems(resp);
        free(resp);
    }
    if (uart_name[0] == '*')
        ((CCombo*)WProp->GetChildByName("combo5"))->SetText(" ");
    else {
        ((CCombo*)WProp->GetChildByName("combo5"))->SetText(uart_name);
    }

    ((CCombo*)WProp->GetChildByName("combo6"))->SetItems("1200,2400,4800,9600,19200,38400,57600,115200,");
    ((CCombo*)WProp->GetChildByName("combo6"))->SetText(std::to_string(uart_speed));
}

void cpart_UART::ReadPropertiesWindow(CPWindow* WProp) {
    pins[0] = GetPWCComboSelectedPin(WProp, "combo2");
    pins[1] = GetPWCComboSelectedPin(WProp, "combo3");
    strncpy(uart_name, (((CCombo*)WProp->GetChildByName("combo5"))->GetText()).c_str(), 199);
    uart_speed = atoi(((CCombo*)WProp->GetChildByName("combo6"))->GetText());

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
