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

#include "part.h"
#include "../picsimlab.h"
#include "../spareparts.h"

part::part(const unsigned x, const unsigned y, const char* name, const char* type, const int fsize)
    : font(fsize, lxFONTFAMILY_TELETYPE, lxFONTSTYLE_NORMAL, lxFONTWEIGHT_BOLD) {
    always_update = 0;
    inputc = 0;
    outputc = 0;
    Orientation = 0;
    Scale = 1.0;
    Update = 1;
    PCWProperties = NULL;
    PCWCount = 0;

    Name = name;
    Type = type;
    X = x;
    Y = y;
    Bitmap = NULL;
}

void part::Init(void) {
    ReadMaps();
    LoadImage();
#ifndef __EMSCRIPTEN__
    RegisterRemoteControl();
#endif
    PostInit();
}

void part::ReadMaps(void) {
    inputc = 0;
    outputc = 0;
    ReadInputMap(lxGetLocalFile(PICSimLab.GetSharePath() + lxT("parts/") + Type + "/" + GetMapFile()));
    ReadOutputMap(lxGetLocalFile(PICSimLab.GetSharePath() + lxT("parts/") + Type + "/" + GetMapFile()));

    for (int i = 0; i < inputc; i++) {
        input_ids[GetInputId(input[i].name)] = &input[i];
    }

    for (int i = 0; i < outputc; i++) {
        output_ids[GetOutputId(output[i].name)] = &output[i];
    }
}

void part::ReadInputMap(lxString fname) {
    FILE* fin;

    char line[256];

    char* it;
    char* shape;
    char* coords;
    char* name;
    char* value;

    int x1, y1, x2, y2, r;

    fin = fopen(fname.c_str(), "r");

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
                        // Window1.SetplWidth(x1);
                        // Window1.draw1.SetWidth(x1*Window1.GetScale());
                        // Window1.SetWidth(185+x1*Window1.GetScale());
                    }

                    if (!strcmp("height", name)) {
                        sscanf(value, "%i", &y1);
                        Height = y1;
                        // Window1.SetplHeight(y1);
                        // Window1.draw1.SetHeight(y1*Window1.GetScale());
                        /*
                        #ifdef _WIN_
                                    Window1.SetHeight(75+y1*Window1.GetScale());
                        #else
                                    Window1.SetHeight(90+y1*Window1.GetScale());
                        #endif
                         */
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

void part::ReadOutputMap(lxString fname) {
    FILE* fin;

    char line[256];

    char* it;
    char* shape;
    char* coords;
    char* name;

    int x1, y1, x2, y2, r;

    fin = fopen(fname.c_str(), "r");

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

void part::LoadImage(void) {
    lxImage image(SpareParts.GetWindow());
    lxString iname = lxGetLocalFile(PICSimLab.GetSharePath() + lxT("parts/") + Type + "/" + GetPictureFileName());

    if (image.LoadFile(iname, Orientation, Scale, Scale)) {
        Bitmap = new lxBitmap(&image, SpareParts.GetWindow());
        image.Destroy();
        canvas.Destroy();
        canvas.Create(SpareParts.GetWindow()->GetWWidget(), Bitmap);
    } else if (image.LoadFile(lxGetLocalFile(PICSimLab.GetSharePath() + lxT("parts/Common/notfound.svg")), Orientation,
                              Scale, Scale)) {
        Bitmap = new lxBitmap(&image, SpareParts.GetWindow());
        image.Destroy();
        canvas.Destroy();
        canvas.Create(SpareParts.GetWindow()->GetWWidget(), Bitmap);
        printf("PICSimLab: (%s) Error loading image %s\n", (const char*)Name.c_str(), (const char*)iname.c_str());
        PICSimLab.RegisterError("Error loading image:\n " + iname);
    } else {
        printf("PICSimLab: (%s) Error loading image %s\n", (const char*)Name.c_str(), (const char*)iname.c_str());
        exit(-1);
    }
}

int part::GetOrientation(void) {
    return Orientation;
}

void part::SetOrientation(int orientation) {
    if (Orientation == orientation)
        return;

    if (!Bitmap)
        return;

    Orientation = orientation;

    delete Bitmap;

    LoadImage();

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

    if (!Bitmap)
        return;

    Scale = scale;

    delete Bitmap;

    LoadImage();

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

lxString part::GetPictureFileName(void) {
    return GetName() + lxT("/part.svg");
}

lxString part::GetMapFile(void) {
    return GetName() + lxT("/part.map");
}

lxString part::GetPropertiesWindowFile(void) {
    return Type + "/" + Name + lxT("/part.lxrad");
}

lxString part::GetHelpURL(void) {
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

    lxString stemp;
    stemp.Printf(lxT("%s.html"), pname);

    return stemp;
}

// Draw Functions

void part::DrawSlider(const output_t* output, const unsigned char pos, const lxString val, const lxFont font) {
    float dy = pos / 1.66;
    canvas.SetFgColor(255, 255, 255);
    canvas.SetBgColor(89, 89, 89);
    canvas.Rectangle(1, output->x1, output->y1, output->x2 - output->x1, output->y2 - output->y1);
    canvas.SetColor(0, 0, 200);
    canvas.Rectangle(1, output->x1 + 1, output->y1 + 1 + dy, output->x2 - output->x1 - 2,
                     output->y2 - output->y1 - 2 - dy);

    canvas.SetFgColor(0, 0, 0);
    canvas.SetBgColor(46, 46, 46);
    canvas.Rectangle(1, output->x1, output->y1 + pos / 1.66, 32, 19);
    canvas.SetColor(250, 250, 250);
    canvas.SetFont(font);
    canvas.RotatedText(val, output->x1 + 1, output->y1 + 5 + pos / 1.66, 0);
}

void part::DrawPotentiometer(const output_t* output, const unsigned char pos, const lxString val, const lxFont font) {
    canvas.SetColor(179, 179, 179);
    canvas.Rectangle(1, output->x1, output->y1, output->x2 - output->x1, output->y2 - output->y1);
    canvas.SetFgColor(0, 0, 0);
    canvas.SetBgColor(96, 96, 96);
    canvas.Rectangle(1, output->x1 + 9, output->y1 + 9, output->x2 - output->x1 - 18, output->y2 - output->y1 - 18);
    canvas.SetBgColor(46, 46, 46);
    canvas.Rectangle(1, output->x1, output->y1 + (200 - pos) / 1.66, 32, 19);
    canvas.SetColor(250, 250, 250);
    canvas.SetFont(font);
    canvas.RotatedText(val, output->x1 + 4, output->y1 + 5 + (200 - pos) / 1.66, 0);
}

int part::GetAwaysUpdate(void) {
    return always_update;
}

void part::SetAwaysUpdate(int sau) {
    always_update = sau;
}

void part::SetPCWProperties(const PCWProp* pcwprop, const int pcwcount) {
    PCWProperties = pcwprop;
    PCWCount = pcwcount;
}

const int part::GetPCWCount(void) {
    return PCWCount;
}

const PCWProp* part::GetPCWProperties(void) {
    return PCWProperties;
}

void part::SetPCWComboWithPinNames(CPWindow* WProp, const char* combo_name, const unsigned char pin) {
    lxString spin;

    CCombo* combo = (CCombo*)WProp->GetChildByName(combo_name);

    if (combo) {
        combo->SetItems(SpareParts.GetPinsNames());
        if (pin == 0)
            combo->SetText("0  NC");
        else {
            spin = SpareParts.GetPinName(pin);
            combo->SetText(itoa(pin) + "  " + spin);
        }
    } else {
        printf("PICSimLab: Combo (%s) not found in Part Configuration Window!\n", combo_name);
    }
}

unsigned char part::GetPWCComboSelectedPin(CPWindow* WProp, const char* combo_name) {
    int selectedpin = 0;
    CCombo* combo = (CCombo*)WProp->GetChildByName(combo_name);

    if (combo) {
        selectedpin = atoi(combo->GetText());
    } else {
        printf("PICSimLab: Combo (%s) not found in Part Configuration Window!\n", combo_name);
    }
    return selectedpin;
}

lxString part::GetName(void) {
    return Name;
}

void part::Draw(void) {
    Update = 0;

    for (int i = 0; i < outputc; i++) {
        if (output[i].update)  // only if need update
        {
            output[i].update = 0;

            if (!Update) {
                canvas.Init(Scale, Scale, Orientation);
                canvas.SetFont(font);
            }
            Update++;  // set to update buffer

            DrawOutput(i);
        }
    }

    if (Update) {
        canvas.End();
    }
}

void part::EvMouseButtonPress(uint button, uint x, uint y, uint state) {
    for (int i = 0; i < inputc; i++) {
        if (PointInside(x, y, input[i])) {
            unsigned int xr = x;
            unsigned int yr = y;
            RotateCoords(&xr, &yr);
            OnMouseButtonPress(input[i].id, button, xr, yr, state);
        }
    }
}

void part::EvMouseButtonRelease(uint button, uint x, uint y, uint state) {
    for (int i = 0; i < inputc; i++) {
        if (PointInside(x, y, input[i])) {
            unsigned int xr = x;
            unsigned int yr = y;
            RotateCoords(&xr, &yr);
            OnMouseButtonRelease(input[i].id, button, xr, yr, state);
        }
    }
}

void part::EvMouseMove(uint button, uint x, uint y, uint state) {
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