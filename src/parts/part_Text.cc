/* ########################################################################

   PICsimLab - PIC laboratory simulator

   ########################################################################

   Copyright (c) : 2022-2022  Luis Claudio Gambôa Lopes

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

#include "part_Text.h"
#include "../picsimlab1.h"
#include "../picsimlab4.h"
#include "../picsimlab5.h"

/* outputs */
enum { O_TEXTB };

enum { C_RED = 0, C_GREEN, C_BLUE, C_YELLOW, C_WHITE, C_BLACK, C_MINE, C_RHINO, C_END };

static const char Colorname[C_END][12] = {"Red", "Green", "Blue", "Yellow", "White", "Black", "Mine Shaft", "Rhino"};

typedef struct {
    unsigned char r, g, b;
} colorval_t;

static const colorval_t colortable[C_END] = {{0xFF, 0, 0},       {0, 0xFF, 0}, {0, 0, 0xFF},       {0xFF, 0xFF, 0},
                                             {0xFF, 0xFF, 0XFF}, {0, 0, 0},    {0x32, 0x32, 0x32}, {0x31, 0x3d, 0x63}};

cpart_TEXT::cpart_TEXT(unsigned x, unsigned y) : font(8, lxFONTFAMILY_TELETYPE, lxFONTSTYLE_NORMAL, lxFONTWEIGHT_BOLD) {
    X = x;
    Y = y;
    ReadMaps();

    Bitmap = NULL;

    Lines.AddLine("text");

    ChangeText(10, 4, 7);
}

cpart_TEXT::~cpart_TEXT(void) {
    delete Bitmap;
    canvas.Destroy();
}

void cpart_TEXT::LoadImage(void) {
    lxString ifname;
    FILE* fimg;

    unsigned int max = 0;

    for (unsigned int l = 0; l < Lines.GetLinesCount(); l++) {
        if (Lines.GetLine(l).length() > max) {
            max = Lines.GetLine(l).length();
        }
    }

    Width = max * Size * 0.9;
    Height = 2 * Size * Lines.GetLinesCount();

    ifname = lxGetTempDir(lxT("picsimlab")) + lxT("/text.svg");

    fimg = fopen((const char*)ifname.c_str(), "w");

    if (fimg) {
        fprintf(fimg,
                "<svg width=\"%i\" height=\"%i\" version=\"1.1\" viewBox=\"0 0 100 "
                "100\" xmlns=\"http://www.w3.org/2000/svg\">"
                "\n <rect x =\"0\" y=\"0\" width=\"100\" height=\"100\"  />\n</svg>",
                Width, Height);
        fclose(fimg);
    } else {
        printf("PICSimLab: Erro open file %s\n", (const char*)ifname.c_str());
    }

    lxImage image(&Window5);

    image.LoadFile(lxGetLocalFile(ifname), Orientation, Scale, Scale);
    Bitmap = new lxBitmap(&image, &Window5);
    image.Destroy();

    canvas.Destroy();
    canvas.Create(Window5.GetWWidget(), Bitmap);

    lxRemoveFile(ifname);
}

void cpart_TEXT::Draw(void) {
    int i;
    unsigned int l;
    lxRect rec;
    lxString Text;
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
                case O_TEXTB:

                    font.SetPointSize(Size);

                    canvas.SetColor(colortable[Bgcolor].r, colortable[Bgcolor].g, colortable[Bgcolor].b);
                    canvas.Rectangle(1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1,
                                     output[i].y2 - output[i].y1);

                    canvas.SetColor(colortable[Textcolor].r, colortable[Textcolor].g, colortable[Textcolor].b);
                    rec.x = output[i].x1;
                    rec.y = output[i].y1;
                    rec.width = output[i].x2 - output[i].x1;
                    rec.height = output[i].y2 - output[i].y1;

                    Text = Lines.GetLine(0);
                    for (l = 1; l < Lines.GetLinesCount(); l++) {
                        Text += "\n" + Lines.GetLine(l);
                    }

                    canvas.TextOnRect(Text, rec, lxALIGN_CENTER | lxALIGN_CENTER_VERTICAL);
                    break;
            }
        }
    }

    if (Update) {
        canvas.End();
    }
}

unsigned short cpart_TEXT::get_in_id(char* name) {
    printf("Erro input '%s' don't have a valid id! \n", name);
    return -1;
}

unsigned short cpart_TEXT::get_out_id(char* name) {
    if (strcmp(name, "DI_TEXT") == 0)
        return O_TEXTB;

    printf("Erro output '%s' don't have a valid id! \n", name);
    return 1;
}

lxString cpart_TEXT::WritePreferences(void) {
    char prefs[256];
    lxString Text = "";
    for (unsigned int l = 0; l < Lines.GetLinesCount(); l++) {
        Text += Lines.GetLine(l) + '\a';
    }
    sprintf(prefs, "%hhu,%hhu,%hhu,%s", Size, Textcolor, Bgcolor, (const char*)Text.c_str());

    return prefs;
}

void cpart_TEXT::ReadPreferences(lxString value) {
    char text[4096];
    char* line;
    sscanf(value.c_str(), "%hhu,%hhu,%hhu,%[^\n]", &Size, &Textcolor, &Bgcolor, text);

    Lines.Clear();
    line = strtok(text, "\a\n");
    while (line) {
        Lines.AddLine(line);
        line = strtok(NULL, "\a\n");
    }

    ChangeText(Size, Textcolor, Bgcolor);

    Reset();

    RegisterRemoteControl();
}

void cpart_TEXT::ChangeText(int size, int textcolor, int bgcolor) {
    Size = size;
    Textcolor = textcolor;
    Bgcolor = bgcolor;
    if (Bitmap) {
        delete Bitmap;
    }
    LoadImage();

    output_ids[O_TEXTB]->x1 = 0;
    output_ids[O_TEXTB]->x2 = Width;
    output_ids[O_TEXTB]->y1 = 0;
    output_ids[O_TEXTB]->y2 = Height;

    output_ids[O_TEXTB]->update = 1;
}

void cpart_TEXT::RegisterRemoteControl(void) {
    /*
    for (int i = 0; i < outputc; i++) {
        switch (output[i].id) {
            case O_TEXTB:
                output[i].status = (void*)&led;
                break;
        }
    }
    */
}

void cpart_TEXT::ConfigurePropertiesWindow(CPWindow* WProp) {
    lxString Colors = "";

    for (int i = 0; i < C_END; i++) {
        Colors += Colorname[i];
        Colors += ",";
    }

    ((CText*)WProp->GetChildByName("text1"))->Clear();
    for (unsigned int l = 0; l < Lines.GetLinesCount(); l++) {
        ((CText*)WProp->GetChildByName("text1"))->AddLine(Lines.GetLine(l));
    }

    ((CSpin*)WProp->GetChildByName("spin1"))->SetValue(Size);

    ((CCombo*)WProp->GetChildByName("combo1"))->SetItems(Colors);
    ((CCombo*)WProp->GetChildByName("combo1"))->SetText(Colorname[Textcolor]);

    ((CCombo*)WProp->GetChildByName("combo2"))->SetItems(Colors);
    ((CCombo*)WProp->GetChildByName("combo2"))->SetText(Colorname[Bgcolor]);

    ((CButton*)WProp->GetChildByName("button1"))->EvMouseButtonRelease =
        EVMOUSEBUTTONRELEASE & CPWindow5::PropButtonRelease;
    ((CButton*)WProp->GetChildByName("button1"))->SetTag(1);

    ((CButton*)WProp->GetChildByName("button2"))->EvMouseButtonRelease =
        EVMOUSEBUTTONRELEASE & CPWindow5::PropButtonRelease;
}

void cpart_TEXT::ReadPropertiesWindow(CPWindow* WProp) {
    Lines.Clear();
    CText* Text = ((CText*)WProp->GetChildByName("text1"));
    for (unsigned int l = 0; l < Text->GetCountLines(); l++) {
        if (Text->GetLine(l).length() > 0) {
            Lines.AddLine(Text->GetLine(l));
        }
    }

    Size = ((CSpin*)WProp->GetChildByName("spin1"))->GetValue();

    lxString val = ((CCombo*)WProp->GetChildByName("combo1"))->GetText();
    for (int j = 0; j < C_END; j++) {
        if (!val.compare(Colorname[j])) {
            Textcolor = j;
            break;
        }
    }

    val = ((CCombo*)WProp->GetChildByName("combo2"))->GetText();
    for (int j = 0; j < C_END; j++) {
        if (!val.compare(Colorname[j])) {
            Bgcolor = j;
            break;
        }
    }

    ChangeText(Size, Textcolor, Bgcolor);
}

part_init(PART_TEXT_Name, cpart_TEXT, "Virtual");
