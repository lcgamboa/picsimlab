/* ########################################################################

   PICSimLab - Programmable IC Simulator Laboratory

   ########################################################################

   Copyright (c) : 2019-2024  Luis Claudio Gambôa Lopes <lcgamboa@yahoo.com>

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

#include "input_keypad.h"
#include "../lib/oscilloscope.h"
#include "../lib/picsimlab.h"
#include "../lib/spareparts.h"

/* outputs */
enum {
    O_K1,
    O_K2,
    O_K3,
    O_KA,
    O_K4,
    O_K5,
    O_K6,
    O_KB,
    O_K7,
    O_K8,
    O_K9,
    O_KC,
    O_Ka,
    O_K0,
    O_KT,
    O_KD,
    O_L1,
    O_L2,
    O_L3,
    O_L4,
    O_C1,
    O_C2,
    O_C3,
    O_C4,
    O_C5
};

/* inputs */
enum { I_K1, I_K2, I_K3, I_KA, I_K4, I_K5, I_K6, I_KB, I_K7, I_K8, I_K9, I_KC, I_Ka, I_K0, I_KT, I_KD };

/* types */
enum { KT4x4 = 1, KT4x3, KT2x5 };

static PCWProp pcwprop[13] = {{PCW_COMBO, "P1 -L1"},
                              {PCW_COMBO, "P2 -L2"},
                              {PCW_COMBO, "P3 -L3"},
                              {PCW_COMBO, "P4 -L4"},
                              {PCW_COMBO, "P5 -C1"},
                              {PCW_COMBO, "P6 -C2"},
                              {PCW_COMBO, "P7 -C3"},
                              {PCW_COMBO, "P8 -C4"},
                              {PCW_LABEL, "P10-VCC,+5V"},
                              {PCW_LABEL, "P11-GND,GND"},
                              {PCW_COMBO, "Pull"},
                              {PCW_COMBO, "Type"},
                              {PCW_END, ""}};

std::string cpart_keypad::GetPictureFileName(void) {
    switch (type) {
        case KT4x4:
            return "Keypad/keypad_4x4.svg";
            break;
        case KT4x3:
            return "Keypad/keypad_4x3.svg";
            break;
        case KT2x5:
            return "Keypad/keypad_2x5.svg";
            break;
    }
    return "Keypad/keypad_4x4.svg";
}

std::string cpart_keypad::GetMapFile(void) {
    switch (type) {
        case KT4x4:
            return "Keypad/keypad_4x4.map";
            break;
        case KT4x3:
            return "Keypad/keypad_4x3.map";
            break;
        case KT2x5:
            return "Keypad/keypad_2x5.map";
            break;
    }
    return "Keypad/keypad_4x4.map";
}

cpart_keypad::cpart_keypad(const unsigned x, const unsigned y, const char* name, const char* type, board* pboard_,
                           const int id_)
    : part(x, y, name, type, pboard_, id_, 9) {
    always_update = 1;
    pull = 0;
    type = 0;

    output_pins[0] = 0;
    output_pins[1] = 0;
    output_pins[2] = 0;
    output_pins[3] = 0;
    output_pins[4] = 0;
    output_pins[5] = 0;
    output_pins[6] = 0;
    output_pins[7] = 0;

    memset(keys, 0, 16);
    memset(keys2, 0, 10);

    refresh = 0;

    ChangeType(KT4x4);

    SetPCWProperties(pcwprop);

    PinCount = 8;
    Pins = output_pins;
}

void cpart_keypad::RegisterRemoteControl(void) {
    if ((type == KT4x3) || (type == KT4x4)) {
        input_ids[I_K1]->status = &keys[0][0];
        input_ids[I_K1]->update = &output_ids[O_K1]->update;
        input_ids[I_K2]->status = &keys[0][1];
        input_ids[I_K2]->update = &output_ids[O_K2]->update;
        input_ids[I_K3]->status = &keys[0][2];
        input_ids[I_K3]->update = &output_ids[O_K3]->update;
        input_ids[I_K4]->status = &keys[1][0];
        input_ids[I_K4]->update = &output_ids[O_K4]->update;
        input_ids[I_K5]->status = &keys[1][1];
        input_ids[I_K5]->update = &output_ids[O_K5]->update;
        input_ids[I_K6]->status = &keys[1][2];
        input_ids[I_K6]->update = &output_ids[O_K6]->update;
        input_ids[I_K7]->status = &keys[2][0];
        input_ids[I_K7]->update = &output_ids[O_K7]->update;
        input_ids[I_K8]->status = &keys[2][1];
        input_ids[I_K8]->update = &output_ids[O_K8]->update;
        input_ids[I_K9]->status = &keys[2][2];
        input_ids[I_K9]->update = &output_ids[O_K9]->update;
        input_ids[I_Ka]->status = &keys[3][0];
        input_ids[I_Ka]->update = &output_ids[O_Ka]->update;
        input_ids[I_K0]->status = &keys[3][1];
        input_ids[I_K0]->update = &output_ids[O_K0]->update;
        input_ids[I_KT]->status = &keys[3][2];
        input_ids[I_KT]->update = &output_ids[O_KT]->update;
        input_ids[I_KA]->status = &keys[0][3];
        input_ids[I_KA]->update = &output_ids[O_KA]->update;
        input_ids[I_KB]->status = &keys[1][3];
        input_ids[I_KB]->update = &output_ids[O_KB]->update;
        input_ids[I_KC]->status = &keys[2][3];
        input_ids[I_KC]->update = &output_ids[O_KC]->update;
        input_ids[I_KD]->status = &keys[3][3];
        input_ids[I_KD]->update = &output_ids[O_KD]->update;
    } else if (type == KT2x5) {
        input_ids[I_K1]->status = &keys2[0][0];
        input_ids[I_K1]->update = &output_ids[O_K1]->update;
        input_ids[I_K2]->status = &keys2[0][1];
        input_ids[I_K2]->update = &output_ids[O_K2]->update;
        input_ids[I_K3]->status = &keys2[0][2];
        input_ids[I_K3]->update = &output_ids[O_K3]->update;
        input_ids[I_K4]->status = &keys2[0][3];
        input_ids[I_K4]->update = &output_ids[O_K4]->update;
        input_ids[I_K5]->status = &keys2[0][4];
        input_ids[I_K5]->update = &output_ids[O_K5]->update;
        input_ids[I_K6]->status = &keys2[1][0];
        input_ids[I_K6]->update = &output_ids[O_K6]->update;
        input_ids[I_K7]->status = &keys2[1][1];
        input_ids[I_K7]->update = &output_ids[O_K7]->update;
        input_ids[I_K8]->status = &keys2[1][2];
        input_ids[I_K8]->update = &output_ids[O_K8]->update;
        input_ids[I_K9]->status = &keys2[1][3];
        input_ids[I_K9]->update = &output_ids[O_K9]->update;
        input_ids[I_K0]->status = &keys2[1][4];
        input_ids[I_K0]->update = &output_ids[O_K0]->update;
    }
}

cpart_keypad::~cpart_keypad(void) {
    SpareParts.SetPartOnDraw(id);
    SpareParts.CanvasCmd({CC_FREEBITMAP, .FreeBitmap{BitmapId}});
    SpareParts.CanvasCmd({CC_DESTROY});
}

void cpart_keypad::ChangeType(unsigned char tp) {
    if (!tp)
        tp = KT4x4;
    if (tp > KT2x5)
        tp = KT4x4;

    // if same
    if (tp == type)
        return;

    if (BitmapId >= 0) {
        SpareParts.SetPartOnDraw(id);
        SpareParts.CanvasCmd({CC_FREEBITMAP, .FreeBitmap{BitmapId}});
    }

    type = tp;

    ReadMaps();

    LoadPartImage();
}

void cpart_keypad::DrawOutput(const unsigned int i) {
    switch (output[i].id) {
        case O_L1:
        case O_L2:
        case O_L3:
        case O_L4:
        case O_C1:
        case O_C2:
        case O_C3:
        case O_C4:
        case O_C5: {
            SpareParts.CanvasCmd({CC_SETCOLOR, .SetColor{49, 61, 99}});
            SpareParts.CanvasCmd({CC_RECTANGLE, .Rectangle{1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1,
                                                           output[i].y2 - output[i].y1}});
            SpareParts.CanvasCmd({CC_SETFGCOLOR, .SetFgColor{255, 255, 255}});

            int id = output[i].id - O_L1;
            if ((type == KT2x5) && (id > 1)) {
                id -= 2;
            }
            if (output_pins[id] == 0)
                SpareParts.CanvasCmd({CC_ROTATEDTEXT, .RotatedText{"NC", output[i].x1, output[i].y2, 90.0}});
            else
                SpareParts.CanvasCmd({CC_ROTATEDTEXT, .RotatedText{SpareParts.GetPinName(output_pins[id]).c_str(),
                                                                   output[i].x1, output[i].y2, 90.0}});
        } break;
        case O_K1 ... O_KD:
            if (output[i].value) {
                SpareParts.CanvasCmd({CC_SETLINEWIDTH, .SetLineWidth{4}});
                SpareParts.CanvasCmd({CC_SETCOLOR, .SetColor{255, 255, 0}});
            } else {
                SpareParts.CanvasCmd({CC_SETLINEWIDTH, .SetLineWidth{6}});
                if ((output[i].id == O_Ka) || (output[i].id == O_KT) || !((output[i].id + 1) % 4)) {
                    SpareParts.CanvasCmd({CC_SETCOLOR, .SetColor{190, 46, 37}});
                } else {
                    SpareParts.CanvasCmd({CC_SETCOLOR, .SetColor{50, 118, 179}});
                }
            }

            SpareParts.CanvasCmd(
                {CC_RECTANGLE, .Rectangle{0, output[i].x1 + 5, output[i].y1 + 5, output[i].x2 - output[i].x1 - 10,
                                          output[i].y2 - output[i].y1 - 10}});
            SpareParts.CanvasCmd({CC_SETLINEWIDTH, .SetLineWidth{1}});
            break;
    }
}

void cpart_keypad::Process(void) {
    if (refresh > 10) {
        const picpin* ppins = SpareParts.GetPinsValues();
        refresh = 0;

        for (int i = 0; i < 8; i++)
            SpareParts.SetPin(output_pins[i], !pull);

        switch (type) {
            case KT4x4:
                for (int c = 0; c < 4; c++) {
                    for (int l = 0; l < 4; l++) {
                        if (keys[l][c]) {
                            SpareParts.SetPin(output_pins[l], ppins[output_pins[4 + c] - 1].value);
                            SpareParts.SetPin(output_pins[4 + c], ppins[output_pins[l] - 1].value);
                        }
                    }
                }
                break;
            case KT4x3:
                for (int c = 0; c < 3; c++) {
                    for (int l = 0; l < 4; l++) {
                        if (keys[l][c]) {
                            SpareParts.SetPin(output_pins[l], ppins[output_pins[4 + c] - 1].value);
                            SpareParts.SetPin(output_pins[4 + c], ppins[output_pins[l] - 1].value);
                        }
                    }
                }
                break;
            case KT2x5:
                for (int c = 0; c < 5; c++) {
                    for (int l = 0; l < 2; l++) {
                        if (keys2[l][c]) {
                            SpareParts.SetPin(output_pins[l], ppins[output_pins[2 + c] - 1].value);
                            SpareParts.SetPin(output_pins[2 + c], ppins[output_pins[l] - 1].value);
                        }
                    }
                }
                break;
        }
    }
    refresh++;
}

void cpart_keypad::OnMouseButtonPress(uint inputId, uint button, uint x, uint y, uint state) {
    switch (inputId) {
        case I_K1:
            keys[0][0] = 1;
            keys2[0][0] = 1;
            output_ids[O_K1]->value = 1;
            output_ids[O_K1]->update = 1;
            break;
        case I_K2:
            keys[0][1] = 1;
            keys2[0][1] = 1;
            output_ids[O_K2]->value = 1;
            output_ids[O_K2]->update = 1;
            break;
        case I_K3:
            keys[0][2] = 1;
            keys2[0][2] = 1;
            output_ids[O_K3]->value = 1;
            output_ids[O_K3]->update = 1;
            break;
        case I_KA:
            keys[0][3] = 1;
            output_ids[O_KA]->value = 1;
            output_ids[O_KA]->update = 1;
            break;
        case I_K4:
            keys[1][0] = 1;
            keys2[0][3] = 1;
            output_ids[O_K4]->value = 1;
            output_ids[O_K4]->update = 1;
            break;
        case I_K5:
            keys[1][1] = 1;
            keys2[0][4] = 1;
            output_ids[O_K5]->value = 1;
            output_ids[O_K5]->update = 1;
            break;
        case I_K6:
            keys[1][2] = 1;
            keys2[1][0] = 1;
            output_ids[O_K6]->value = 1;
            output_ids[O_K6]->update = 1;
            break;
        case I_KB:
            keys[1][3] = 1;
            output_ids[O_KB]->value = 1;
            output_ids[O_KB]->update = 1;
            break;
        case I_K7:
            keys[2][0] = 1;
            keys2[1][1] = 1;
            output_ids[O_K7]->value = 1;
            output_ids[O_K7]->update = 1;
            break;
        case I_K8:
            keys[2][1] = 1;
            keys2[1][2] = 1;
            output_ids[O_K8]->value = 1;
            output_ids[O_K8]->update = 1;
            break;
        case I_K9:
            keys[2][2] = 1;
            keys2[1][3] = 1;
            output_ids[O_K9]->value = 1;
            output_ids[O_K9]->update = 1;
            break;
        case I_KC:
            keys[2][3] = 1;
            output_ids[O_KC]->value = 1;
            output_ids[O_KC]->update = 1;
            break;
        case I_Ka:
            keys[3][0] = 1;
            output_ids[O_Ka]->value = 1;
            output_ids[O_Ka]->update = 1;
            break;
        case I_K0:
            keys[3][1] = 1;
            keys2[1][4] = 1;
            output_ids[O_K0]->value = 1;
            output_ids[O_K0]->update = 1;
            break;
        case I_KT:
            keys[3][2] = 1;
            output_ids[O_KT]->value = 1;
            output_ids[O_KT]->update = 1;
            break;
        case I_KD:
            keys[3][3] = 1;
            output_ids[O_KD]->value = 1;
            output_ids[O_KD]->update = 1;
            break;
    }
}

void cpart_keypad::OnMouseButtonRelease(uint inputId, uint button, uint x, uint y, uint state) {
    switch (inputId) {
        case I_K1:
            keys[0][0] = 0;
            keys2[0][0] = 0;
            output_ids[O_K1]->value = 0;
            output_ids[O_K1]->update = 1;
            break;
        case I_K2:
            keys[0][1] = 0;
            keys2[0][1] = 0;
            output_ids[O_K2]->value = 0;
            output_ids[O_K2]->update = 1;
            break;
        case I_K3:
            keys[0][2] = 0;
            keys2[0][2] = 0;
            output_ids[O_K3]->value = 0;
            output_ids[O_K3]->update = 1;
            break;
        case I_KA:
            keys[0][3] = 0;
            output_ids[O_KA]->value = 0;
            output_ids[O_KA]->update = 1;
            break;
        case I_K4:
            keys[1][0] = 0;
            keys2[0][3] = 0;
            output_ids[O_K4]->value = 0;
            output_ids[O_K4]->update = 1;
            break;
        case I_K5:
            keys[1][1] = 0;
            keys2[0][4] = 0;
            output_ids[O_K5]->value = 0;
            output_ids[O_K5]->update = 1;
            break;
        case I_K6:
            keys[1][2] = 0;
            keys2[1][0] = 0;
            output_ids[O_K6]->value = 0;
            output_ids[O_K6]->update = 1;
            break;
        case I_KB:
            keys[1][3] = 0;
            output_ids[O_KB]->value = 0;
            output_ids[O_KB]->update = 1;
            break;
        case I_K7:
            keys[2][0] = 0;
            keys2[1][1] = 0;
            output_ids[O_K7]->value = 0;
            output_ids[O_K7]->update = 1;
            break;
        case I_K8:
            keys[2][1] = 0;
            output_ids[O_K8]->value = 0;
            output_ids[O_K8]->update = 1;
            keys2[1][2] = 0;
            break;
        case I_K9:
            keys[2][2] = 0;
            keys2[1][3] = 0;
            output_ids[O_K9]->value = 0;
            output_ids[O_K9]->update = 1;
            break;
        case I_KC:
            keys[2][3] = 0;
            output_ids[O_KC]->value = 0;
            output_ids[O_KC]->update = 1;
            break;
        case I_Ka:
            keys[3][0] = 0;
            output_ids[O_Ka]->value = 0;
            output_ids[O_Ka]->update = 1;
            break;
        case I_K0:
            keys[3][1] = 0;
            keys2[1][4] = 0;
            output_ids[O_K0]->value = 0;
            output_ids[O_K0]->update = 1;
            break;
        case I_KT:
            keys[3][2] = 0;
            output_ids[O_KT]->value = 0;
            output_ids[O_KT]->update = 1;
            break;
        case I_KD:
            keys[3][3] = 0;
            output_ids[O_KD]->value = 0;
            output_ids[O_KD]->update = 1;
            break;
    }
}

unsigned short cpart_keypad::GetInputId(char* name) {
    if (strcmp(name, "KB_1") == 0)
        return I_K1;
    if (strcmp(name, "KB_2") == 0)
        return I_K2;
    if (strcmp(name, "KB_3") == 0)
        return I_K3;
    if (strcmp(name, "KB_A") == 0)
        return I_KA;

    if (strcmp(name, "KB_4") == 0)
        return I_K4;
    if (strcmp(name, "KB_5") == 0)
        return I_K5;
    if (strcmp(name, "KB_6") == 0)
        return I_K6;
    if (strcmp(name, "KB_B") == 0)
        return I_KB;

    if (strcmp(name, "KB_7") == 0)
        return I_K7;
    if (strcmp(name, "KB_8") == 0)
        return I_K8;
    if (strcmp(name, "KB_9") == 0)
        return I_K9;
    if (strcmp(name, "KB_C") == 0)
        return I_KC;

    if (strcmp(name, "KB_a") == 0)
        return I_Ka;
    if (strcmp(name, "KB_0") == 0)
        return I_K0;
    if (strcmp(name, "KB_T") == 0)
        return I_KT;
    if (strcmp(name, "KB_D") == 0)
        return I_KD;

    printf("Error input '%s' don't have a valid id! \n", name);
    return INVALID_ID;
}

unsigned short cpart_keypad::GetOutputId(char* name) {
    if (strcmp(name, "PN_L1") == 0)
        return O_L1;
    if (strcmp(name, "PN_L2") == 0)
        return O_L2;
    if (strcmp(name, "PN_L3") == 0)
        return O_L3;
    if (strcmp(name, "PN_L4") == 0)
        return O_L4;
    if (strcmp(name, "PN_C1") == 0)
        return O_C1;
    if (strcmp(name, "PN_C2") == 0)
        return O_C2;
    if (strcmp(name, "PN_C3") == 0)
        return O_C3;
    if (strcmp(name, "PN_C4") == 0)
        return O_C4;
    if (strcmp(name, "PN_C5") == 0)
        return O_C5;

    if (strcmp(name, "KB_1") == 0)
        return O_K1;
    if (strcmp(name, "KB_2") == 0)
        return O_K2;
    if (strcmp(name, "KB_3") == 0)
        return O_K3;
    if (strcmp(name, "KB_A") == 0)
        return O_KA;

    if (strcmp(name, "KB_4") == 0)
        return O_K4;
    if (strcmp(name, "KB_5") == 0)
        return O_K5;
    if (strcmp(name, "KB_6") == 0)
        return O_K6;
    if (strcmp(name, "KB_B") == 0)
        return O_KB;

    if (strcmp(name, "KB_7") == 0)
        return O_K7;
    if (strcmp(name, "KB_8") == 0)
        return O_K8;
    if (strcmp(name, "KB_9") == 0)
        return O_K9;
    if (strcmp(name, "KB_C") == 0)
        return O_KC;

    if (strcmp(name, "KB_a") == 0)
        return O_Ka;
    if (strcmp(name, "KB_0") == 0)
        return O_K0;
    if (strcmp(name, "KB_T") == 0)
        return O_KT;
    if (strcmp(name, "KB_D") == 0)
        return O_KD;

    printf("Error output '%s' don't have a valid id! \n", name);
    return INVALID_ID;
}

std::string cpart_keypad::WritePreferences(void) {
    char prefs[256];

    sprintf(prefs, "%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu", output_pins[0], output_pins[1], output_pins[2],
            output_pins[3], output_pins[4], output_pins[5], output_pins[6], output_pins[7], pull, type);

    return prefs;
}

void cpart_keypad::ReadPreferences(std::string value) {
    unsigned char tp;
    sscanf(value.c_str(), "%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu", &output_pins[0], &output_pins[1],
           &output_pins[2], &output_pins[3], &output_pins[4], &output_pins[5], &output_pins[6], &output_pins[7], &pull,
           &tp);
    memset(keys, 0, 16);
    memset(keys2, 0, 10);
    ChangeType(tp);
}

void cpart_keypad::ConfigurePropertiesWindow(void) {
    SetPCWComboWithPinNames("combo1", output_pins[0]);
    SetPCWComboWithPinNames("combo2", output_pins[1]);
    SetPCWComboWithPinNames("combo3", output_pins[2]);
    SetPCWComboWithPinNames("combo4", output_pins[3]);
    SetPCWComboWithPinNames("combo5", output_pins[4]);
    SetPCWComboWithPinNames("combo6", output_pins[5]);
    SetPCWComboWithPinNames("combo7", output_pins[6]);
    SetPCWComboWithPinNames("combo8", output_pins[7]);

    SpareParts.WPropCmd("combo11", WPA_COMBOSETITEMS, "UP,DOWN,");
    if (!pull)
        SpareParts.WPropCmd("combo11", WPA_COMBOSETTEXT, "UP");
    else
        SpareParts.WPropCmd("combo11", WPA_COMBOSETTEXT, "DOWN");

    SpareParts.WPropCmd("combo12", WPA_COMBOSETITEMS, "4x4,4x3,2x5,");
    SpareParts.WPropCmd("combo12", WPA_COMBOENABLEEV, "1");

    switch (type) {
        case KT4x4:
            SpareParts.WPropCmd("label1", WPA_LABELSETTEXT, "P1 - L1");
            SpareParts.WPropCmd("label2", WPA_LABELSETTEXT, "P2 - L2");
            SpareParts.WPropCmd("label3", WPA_LABELSETTEXT, "P3 - L3");
            SpareParts.WPropCmd("label4", WPA_LABELSETTEXT, "P4 - L4");
            SpareParts.WPropCmd("label5", WPA_LABELSETTEXT, "P5 - C1");
            SpareParts.WPropCmd("label6", WPA_LABELSETTEXT, "P6 - C2");
            SpareParts.WPropCmd("label7", WPA_LABELSETTEXT, "P7 - C3");
            SpareParts.WPropCmd("label8", WPA_LABELSETTEXT, "P8 - C4");
            SpareParts.WPropCmd("combo8", WPA_SETENABLE, "1");
            SpareParts.WPropCmd("combo12", WPA_COMBOSETTEXT, "4x4");
            break;
        case KT4x3:
            SpareParts.WPropCmd("label1", WPA_LABELSETTEXT, "P1 - L1");
            SpareParts.WPropCmd("label2", WPA_LABELSETTEXT, "P2 - L2");
            SpareParts.WPropCmd("label3", WPA_LABELSETTEXT, "P3 - L3");
            SpareParts.WPropCmd("label4", WPA_LABELSETTEXT, "P4 - L4");
            SpareParts.WPropCmd("label5", WPA_LABELSETTEXT, "P5 - C1");
            SpareParts.WPropCmd("label6", WPA_LABELSETTEXT, "P6 - C2");
            SpareParts.WPropCmd("label7", WPA_LABELSETTEXT, "P7 - C3");
            SpareParts.WPropCmd("label8", WPA_LABELSETTEXT, "P8 - NC");
            SpareParts.WPropCmd("combo8", WPA_SETENABLE, "0");
            SpareParts.WPropCmd("combo12", WPA_COMBOSETTEXT, "4x3");
            break;
        case KT2x5:
            SpareParts.WPropCmd("label1", WPA_LABELSETTEXT, "P1 - L1");
            SpareParts.WPropCmd("label2", WPA_LABELSETTEXT, "P2 - L2");
            SpareParts.WPropCmd("label3", WPA_LABELSETTEXT, "P3 - C1");
            SpareParts.WPropCmd("label4", WPA_LABELSETTEXT, "P4 - C2");
            SpareParts.WPropCmd("label5", WPA_LABELSETTEXT, "P5 - C3");
            SpareParts.WPropCmd("label6", WPA_LABELSETTEXT, "P6 - C4");
            SpareParts.WPropCmd("label7", WPA_LABELSETTEXT, "P7 - C5");
            SpareParts.WPropCmd("label8", WPA_LABELSETTEXT, "P8 - NC");
            SpareParts.WPropCmd("combo8", WPA_SETENABLE, "0");
            SpareParts.WPropCmd("combo12", WPA_COMBOSETTEXT, "2x5");
            break;
    }
}

void cpart_keypad::ReadPropertiesWindow(void) {
    output_pins[0] = GetPWCComboSelectedPin("combo1");
    output_pins[1] = GetPWCComboSelectedPin("combo2");
    output_pins[2] = GetPWCComboSelectedPin("combo3");
    output_pins[3] = GetPWCComboSelectedPin("combo4");
    output_pins[4] = GetPWCComboSelectedPin("combo5");
    output_pins[5] = GetPWCComboSelectedPin("combo6");
    output_pins[6] = GetPWCComboSelectedPin("combo7");
    output_pins[7] = GetPWCComboSelectedPin("combo8");

    char buff[64];
    SpareParts.WPropCmd("combo11", WPA_COMBOGETTEXT, NULL, buff);

    pull = !(strcmp(buff, "UP") == 0);

    SpareParts.WPropCmd("combo12", WPA_COMBOGETTEXT, NULL, buff);

    ComboChange(NULL, NULL, buff);

    memset(keys, 0, 16);
    memset(keys2, 0, 10);
}

void cpart_keypad::ComboChange(CPWindow* WProp, CCombo* control, std::string value) {
    if (!value.compare("4x3")) {
        ChangeType(KT4x3);
    } else if (!value.compare("2x5")) {
        ChangeType(KT2x5);
    } else {
        ChangeType(KT4x4);
    }
}

part_init(PART_KEYPAD_Name, cpart_keypad, "Input");
