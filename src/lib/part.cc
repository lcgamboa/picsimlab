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

#include "../lib/part.h"
#include "../lib/picsimlab.h"
#include "../lib/spareparts.h"

part::part(const unsigned x, const unsigned y, const char* name, const char* type, board* pboard_, const int _id,
           const int fsize) {
    always_update = 0;
    inputc = 0;
    outputc = 0;
    Orientation = 0;
    Scale = 1.0;
    Update = 1;
    PCWProperties = NULL;
    PCWCount = 0;
    pboard = pboard_;
    Fsize = fsize;
    id = _id;

    Name = name;
    Type = type;
    X = x;
    Y = y;
    BitmapId = -1;
    PinCount = 0;
    Pins = NULL;

    for (int i = 0; i < MAX_IDS; i++) {
        input_ids[i] = &input[i];
        output_ids[i] = &output[i];
    }
}

void part::Init(void) {
    ReadMaps();
    LoadPartImage();
#ifndef __EMSCRIPTEN__
    RegisterRemoteControl();
#endif
    PostInit();
}

void part::ReadMaps(void) {
    inputc = 0;
    outputc = 0;
    ReadInputMap((const char*)GetLocalFile(PICSimLab.GetSharePath() + "parts/" + Type + "/" + GetMapFile()).c_str());
    ReadOutputMap((const char*)GetLocalFile(PICSimLab.GetSharePath() + "parts/" + Type + "/" + GetMapFile()).c_str());

    for (int i = 0; i < inputc; i++) {
        input_ids[GetInputId(input[i].name)] = &input[i];
    }

    for (int i = 0; i < outputc; i++) {
        output_ids[GetOutputId(output[i].name)] = &output[i];
    }
}

void part::ReadInputMap(std::string fname) {
    FILE* fin;

    char line[256];

    char* it;
    char* shape;
    char* coords;
    char* name;
    char* value;

    int x1, y1, x2, y2, r;

    fin = fopen_UTF8(fname.c_str(), "r");

    if (fin) {
        while (fgets(line, 256, fin)) {
            it = strtok(line, "< =\"");
            if (!strcmp("img", it)) {
                do {
                    name = strtok(NULL, "< =\"");
                    value = strtok(NULL, "<=\"");

                    if (!strcmp("width", name)) {
                        sscanf(value, "%i", &x1);
                        Width = x1;
                    }

                    if (!strcmp("height", name)) {
                        sscanf(value, "%i", &y1);
                        Height = y1;
                    }

                } while (value != NULL);

                //        printf("%s %s %s\n",name,shape,coords);

            } else if (!strcmp("area", it)) {
                strtok(NULL, "< =\"");
                shape = strtok(NULL, "< =\"");
                strtok(NULL, "< =\"");
                coords = strtok(NULL, "< =\"");
                strtok(NULL, "< =\"");
                name = strtok(NULL, "< =\"");

                //        printf("%s %s %s\n",name,shape,coords);

                if (((name[0] == 'I') || (name[0] == 'B')) && (name[1] == '_')) {
                    if (strcmp("rect", shape) == 0) {
                        sscanf(coords, "%i,%i,%i,%i\n", &x1, &y1, &x2, &y2);
                        //          printf("rect=%i,%i,%i,%i\n",x1,y1,x2,y2);

                        input[inputc].x1 = x1;
                        input[inputc].y1 = y1;
                        input[inputc].x2 = x2;
                        input[inputc].y2 = y2;
                    } else {
                        sscanf(coords, "%i,%i,%i\n", &x1, &y1, &r);
                        //          printf("circle=%i,%i,%i\n",x1,y1,r);
                        input[inputc].x1 = x1 - r;
                        input[inputc].y1 = y1 - r;
                        input[inputc].x2 = x1 + r;
                        input[inputc].y2 = y1 + r;
                    }
                    strcpy(input[inputc].name, name + 2);
                    input[inputc].id = GetInputId(input[inputc].name);
                    input[inputc].cx = ((input[inputc].x2 - input[inputc].x1) / 2.0) + input[inputc].x1;
                    input[inputc].cy = ((input[inputc].y2 - input[inputc].y1) / 2.0) + input[inputc].y1;
                    input[inputc].status = NULL;
                    input[inputc].update = NULL;
                    input[inputc].value = 0;
                    input[inputc].value_s = 0;
                    input[inputc].value_f = 0;
                    inputc++;
                }
            }
        }
        fclose(fin);
    } else {
        printf("PICSimLab: (%s) Error open input.map \"%s\"!\n", (const char*)Name.c_str(), (const char*)fname.c_str());
        PICSimLab.RegisterError(Name + ": Error open input.map:\n" + fname);
    }
}

void part::ReadOutputMap(std::string fname) {
    FILE* fin;

    char line[256];

    char* it;
    char* shape;
    char* coords;
    char* name;

    int x1, y1, x2, y2, r;

    fin = fopen_UTF8(fname.c_str(), "r");

    if (fin) {
        while (fgets(line, 256, fin)) {
            it = strtok(line, "< =\"");
            if (!strcmp("area", it)) {
                strtok(NULL, "< =\"");
                shape = strtok(NULL, "< =\"");
                strtok(NULL, "< =\"");
                coords = strtok(NULL, "< =\"");
                strtok(NULL, "< =\"");
                name = strtok(NULL, "< =\"");

                //        printf("%s %s %s\n",name,shape,coords);
                if (((name[0] == 'O') || (name[0] == 'B')) && (name[1] == '_')) {
                    if (!strcmp("rect", shape)) {
                        sscanf(coords, "%i,%i,%i,%i\n", &x1, &y1, &x2, &y2);
                        //          printf("rect=%i,%i,%i,%i\n",x1,y1,x2,y2);

                        output[outputc].x1 = x1;
                        output[outputc].y1 = y1;
                        output[outputc].x2 = x2;
                        output[outputc].y2 = y2;
                        output[outputc].r = 0;
                        //          output[outputc].lval=-1;
                        strcpy(output[outputc].name, name + 2);
                        output[outputc].id = GetOutputId(output[outputc].name);
                        output[outputc].cx = ((output[outputc].x2 - output[outputc].x1) / 2.0) + output[outputc].x1;
                        output[outputc].cy = ((output[outputc].y2 - output[outputc].y1) / 2.0) + output[outputc].y1;
                        output[outputc].status = NULL;
                        output[outputc].value = 0;
                        output[outputc].value_s = 0;
                        output[outputc].value_f = 0;
                        outputc++;
                    } else {
                        sscanf(coords, "%i,%i,%i\n", &x1, &y1, &r);
                        //          printf("circle=%i,%i,%i\n",x1,y1,r);
                        output[outputc].x1 = x1;
                        output[outputc].y1 = y1;
                        output[outputc].x2 = 0;
                        output[outputc].y2 = 0;
                        output[outputc].r = r;
                        //          output[outputc].lval=-1;
                        strcpy(output[outputc].name, name + 2);
                        output[outputc].id = GetOutputId(output[outputc].name);
                        output[outputc].cx = output[outputc].x1;
                        output[outputc].cy = output[outputc].y1;
                        output[outputc].status = NULL;
                        output[outputc].value = 0;
                        output[outputc].value_s = 0;
                        output[outputc].value_f = 0;
                        outputc++;
                    }
                }
            }
        }

        fclose(fin);
    } else {
        printf("PICSimLab: (%s) Error open output.map \"%s\"!\n", (const char*)Name.c_str(),
               (const char*)fname.c_str());
        PICSimLab.RegisterError(Name + ": Error open output.map:\n" + fname);
    }
}

int part::PointInside(int x, int y) {
    switch (Orientation) {
        case 0:
        case 2:
            if (((x >= X) && (x <= (X + (int)Width))) && ((y >= Y) && (y <= (Y + (int)Height))))
                return 1;
            else
                return 0;
            break;
        case 1:
        case 3:
            if (((x >= X) && (x <= (X + (int)Height))) && ((y >= Y) && (y <= (Y + (int)Width))))
                return 1;
            else
                return 0;
            break;
    }
    return 0;
}

int part::PointInside(int x, int y, input_t input) {
    int temp;
    int X1 = 0, X2 = 0;
    int Y1 = 0, Y2 = 0;

    switch (Orientation) {
        case 0:
            X1 = input.x1;
            Y1 = input.y1;
            X2 = input.x2;
            Y2 = input.y2;
            break;
        case 1:
            X1 = Height - input.y1;
            Y1 = input.x1;
            X2 = Height - input.y2;
            Y2 = input.x2;
            break;
        case 2:
            X1 = Width - input.x1;
            Y1 = Height - input.y1;
            X2 = Width - input.x2;
            Y2 = Height - input.y2;
            break;
        case 3:
            X1 = input.y1;
            Y1 = Width - input.x1;
            X2 = input.y2;
            Y2 = Width - input.x2;
            break;
        default:
            break;
    }

    if (X1 > X2) {
        temp = X1;
        X1 = X2;
        X2 = temp;
    }
    if (Y1 > Y2) {
        temp = Y1;
        Y1 = Y2;
        Y2 = temp;
    }

    if (((x >= X1) && (x <= X2)) && ((y >= Y1) && (y <= Y2)))
        return 1;
    else
        return 0;
}

void part::LoadPartImage(void) {
    int created = 0;
    SpareParts.WindowCmd(PW_MAIN, NULL, PWA_WINDOWHASCREATED, NULL, &created);
    if (created == 1) {
        SpareParts.SetPartOnDraw(id);
        std::string iname = PICSimLab.GetSharePath() + "parts/" + Type + "/" + GetPictureFileName();

        if (BitmapId >= 0) {
            SpareParts.CanvasCmd({.cmd = CC_FREEBITMAP, .FreeBitmap{BitmapId}});
            BitmapId = -1;
        }

        int bmp = SpareParts.CanvasCmd({.cmd = CC_LOADIMAGE, .LoadImage{iname.c_str(), Scale, 0, Orientation}});

        if (bmp >= 0) {
            BitmapId = bmp;
            SpareParts.CanvasCmd({.cmd = CC_DESTROY});
            SpareParts.CanvasCmd({.cmd = CC_CREATE, .Create{BitmapId}});
        } else if ((bmp = SpareParts.CanvasCmd(
                        {.cmd = CC_LOADIMAGE,
                         .LoadImage{(PICSimLab.GetSharePath() + "parts/Common/notfound.svg").c_str(), Scale, 0,
                                    Orientation}})) >= 0) {
            BitmapId = bmp;
            SpareParts.CanvasCmd({.cmd = CC_DESTROY});
            SpareParts.CanvasCmd({.cmd = CC_CREATE, .Create{BitmapId}});

            printf("PICSimLab: (%s) Error loading image %s\n", (const char*)Name.c_str(), (const char*)iname.c_str());
            PICSimLab.RegisterError("Error loading image:\n " + iname);
        } else {
            printf("PICSimLab: (%s) Error loading image %s\n", (const char*)Name.c_str(), (const char*)iname.c_str());
            exit(-1);
        }
    }
}

int part::GetOrientation(void) {
    return Orientation;
}

void part::SetOrientation(int orientation) {
    if (Orientation == orientation)
        return;

    Orientation = orientation;

    if (BitmapId < 0)
        return;

    SpareParts.SetPartOnDraw(id);
    SpareParts.CanvasCmd({.cmd = CC_FREEBITMAP, .FreeBitmap{BitmapId}});
    BitmapId = -1;

    LoadPartImage();

    for (int i = 0; i < outputc; i++) {
        output[i].update = 1;
    }
}

float part::GetScale(void) {
    return Scale;
}

void part::SetScale(double scale) {
    if (Scale == scale)
        return;

    if (BitmapId < 0)
        return;

    Scale = scale;

    SpareParts.SetPartOnDraw(id);
    SpareParts.CanvasCmd({.cmd = CC_FREEBITMAP, .FreeBitmap{BitmapId}});
    BitmapId = -1;

    LoadPartImage();

    for (int i = 0; i < outputc; i++) {
        output[i].update = 1;
    }
}

int part::GetInputCount(void) {
    return inputc;
}

input_t* part::GetInput(int n) {
    if (n < inputc) {
        return &input[n];
    }
    return NULL;
}

int part::GetOutputCount(void) {
    return outputc;
}

output_t* part::GetOutput(int n) {
    if (n < outputc) {
        return &output[n];
    }
    return NULL;
}

void part::RotateCoords(unsigned int* x, unsigned int* y) {
    int temp;

    switch (Orientation) {
        case 1:
            temp = *x;
            *x = *y;
            *y = Height - temp;
            break;
        case 2:
            *x = Width - *x;
            *y = Height - *y;
            break;
        case 3:
            temp = *y;
            *y = *x;
            *x = Width - temp;
            break;
    }
}

unsigned char part::GetUpdate(void) {
    return Update;
}

void part::SetUpdate(unsigned char upd) {
    Update = upd;
    for (int i = 0; i < outputc; i++) {
        output[i].update = Update;
    }
}

std::string part::GetPictureFileName(void) {
    return GetName() + "/part.svg";
}

std::string part::GetMapFile(void) {
    return GetName() + "/part.map";
}

std::string part::GetPropertiesWindowFile(void) {
    return Type + "/" + Name + "/part.lxrad";
}

std::string part::GetHelpURL(void) {
    char pname[50];
    strncpy(pname, (const char*)GetName().c_str(), 49);

    char* ptr;
    // remove space from names
    while ((ptr = strchr(pname, ' '))) {
        memmove(ptr, ptr + 1, strlen(ptr) + 1);
    }
    // remove ( from names
    while ((ptr = strchr(pname, '('))) {
        memmove(ptr, ptr + 1, strlen(ptr) + 1);
    }
    // remove ) from names
    while ((ptr = strchr(pname, ')'))) {
        memmove(ptr, ptr + 1, strlen(ptr) + 1);
    }
    // remove . from names
    while ((ptr = strchr(pname, '.'))) {
        memmove(ptr, ptr + 1, strlen(ptr) + 1);
    }
    // remove - from names
    while ((ptr = strchr(pname, '-'))) {
        memmove(ptr, ptr + 1, strlen(ptr) + 1);
    }

    char stemp[256];
    snprintf(stemp, 100, "%s.html", pname);

    return stemp;
}

int part::GetAlwaysUpdate(void) {
    return always_update;
}

void part::SetAlwaysUpdate(int sau) {
    always_update = sau;
}

void part::SetPCWProperties(const PCWProp* pcwprop) {
    PCWProperties = pcwprop;
    PCWCount = 0;

    while (PCWProperties[PCWCount].pcw_type != PCW_END) {
        PCWCount++;
    }
}

const int part::GetPCWCount(void) {
    return PCWCount;
}

const PCWProp* part::GetPCWProperties(void) {
    return PCWProperties;
}

void part::SetPCWComboWithPinNames(const char* combo_name, const unsigned char pin) {
    std::string spin;

    SpareParts.WPropCmd(combo_name, PWA_COMBOSETITEMS, SpareParts.GetPinsNames().c_str());

    if (pin == 0) {
        SpareParts.WPropCmd(combo_name, PWA_COMBOSETTEXT, "0  NC");
    } else {
        spin = SpareParts.GetPinName(pin);
        SpareParts.WPropCmd(combo_name, PWA_COMBOSETTEXT, (std::to_string(pin) + "  " + spin).c_str());
    }
}

unsigned char part::GetPWCComboSelectedPin(const char* combo_name) {
    char buff[64];
    SpareParts.WPropCmd(combo_name, PWA_COMBOGETTEXT, NULL, buff);
    return atoi(buff);
}

std::string part::GetName(void) {
    return Name;
}

void part::Draw(void) {
    Update = 0;

    for (int i = 0; i < outputc; i++) {
        if (output[i].update)  // only if need update
        {
            output[i].update = 0;

            if (!Update) {
                SpareParts.CanvasCmd({.cmd = CC_INIT, .Init{Scale, Scale, Orientation}});
                SpareParts.CanvasCmd({.cmd = CC_SETFONTSIZE, .SetFontSize{Fsize}});
                SpareParts.CanvasCmd({.cmd = CC_SETFONTWEIGHT, .SetFontWeight{CC_FONTWEIGHT_BOLD}});
            }
            Update++;  // set to update buffer

            DrawOutput(i);
        }
    }

    if (Update) {
        SpareParts.CanvasCmd({.cmd = CC_END});
    }
}

void part::EvMouseButtonPress(unsigned int button, unsigned int x, unsigned int y, unsigned int state) {
    for (int i = 0; i < inputc; i++) {
        if (PointInside(x, y, input[i])) {
            unsigned int xr = x;
            unsigned int yr = y;
            RotateCoords(&xr, &yr);
            OnMouseButtonPress(input[i].id, button, xr, yr, state);
        }
    }
}

void part::EvMouseButtonRelease(unsigned int button, unsigned int x, unsigned int y, unsigned int state) {
    for (int i = 0; i < inputc; i++) {
        if (PointInside(x, y, input[i])) {
            unsigned int xr = x;
            unsigned int yr = y;
            RotateCoords(&xr, &yr);
            OnMouseButtonRelease(input[i].id, button, xr, yr, state);
        }
    }
}

void part::EvMouseMove(unsigned int button, unsigned int x, unsigned int y, unsigned int state) {
    int none = 1;
    for (int i = 0; i < inputc; i++) {
        if (PointInside(x, y, input[i])) {
            unsigned int xr = x;
            unsigned int yr = y;
            RotateCoords(&xr, &yr);
            OnMouseMove(input[i].id, button, xr, yr, state);
            none = 0;
        }
    }

    if (none) {
        unsigned int xr = x;
        unsigned int yr = y;
        RotateCoords(&xr, &yr);
        OnMouseMove(0xFFFFFF, button, xr, yr, state);
    }
}

// PARTS_DEFS

part_desc parts_list[MAX_PARTS];

int NUM_PARTS = 0;

// boards object creation

part* create_part(std::string name, const unsigned int x, const unsigned int y, board* pboard_, const int id_) {
    part* part_ = NULL;

    for (int i = 0; i < NUM_PARTS; i++) {
        if (name.compare(parts_list[i].name) == 0) {
            part_ = parts_list[i].pcreate(x, y, pboard_, id_);
            break;
        }
    }

    return part_;
}

void part_register(const char* name, part_create_func pcreate, const char* menu) {
    int in;

    if (NUM_PARTS == MAX_PARTS) {
        printf("Number of parts greater than MAX_PARTS!\n");
        exit(-1);
    }

    // insert in ascendent order
    in = NUM_PARTS;
    for (int i = NUM_PARTS; i > 0; i--) {
        if (strcmp(name, parts_list[i - 1].name) > 0) {
            break;
        } else {
            in = i - 1;
            memcpy((void*)&parts_list[i], (void*)&parts_list[i - 1], sizeof(part_desc));
        }
    }

    // insert new
    parts_list[in].pcreate = pcreate;
    strncpy(parts_list[in].name, name, 30);
    strncpy(parts_list[in].menu, menu, 30);

    NUM_PARTS++;
}