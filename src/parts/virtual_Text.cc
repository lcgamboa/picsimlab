/* ########################################################################

   PICSimLab - Programmable IC Simulator Laboratory

   ########################################################################

   Copyright (c) : 2022-2023  Luis Claudio Gambôa Lopes <lcgamboa@yahoo.com>

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

#include "virtual_Text.h"
#include "../lib/oscilloscope.h"
#include "../lib/picsimlab.h"
#include "../lib/spareparts.h"

/* inputs */
enum { I_TEXTB };

/* outputs */
enum { O_TEXTB };

enum { C_RED = 0, C_GREEN, C_BLUE, C_YELLOW, C_WHITE, C_BLACK, C_MINE, C_RHINO, C_GRANDIS, C_END };

static const char Colorname[C_END][12] = {"Red",   "Green",      "Blue",  "Yellow", "White",
                                          "Black", "Mine Shaft", "Rhino", "Grandis"};

typedef struct {
    unsigned char r, g, b;
} colorval_t;

static const colorval_t colortable[C_END] = {{0xFF, 0, 0},       {0, 0xFF, 0},       {0, 0, 0xFF},
                                             {0xFF, 0xFF, 0},    {0xFF, 0xFF, 0XFF}, {0, 0, 0},
                                             {0x32, 0x32, 0x32}, {0x31, 0x3d, 0x63}, {0xff, 0xd3, 0x8c}};

static PCWProp pcwprop[] = {{PCW_TEXT, "text"},     {PCW_SPIN, "Size"}, {PCW_COMBO, "Color"},
                            {PCW_COMBO, "Backgrd"}, {PCW_EDIT, "Link"}, {PCW_END, ""}};

cpart_TEXT::cpart_TEXT(const unsigned x, const unsigned y, const char* name, const char* type, board* pboard_)
    : part(x, y, name, type, pboard_, 8) {
    Bitmap = NULL;
    Size = 12;
    Textcolor = 4;
    Bgcolor = 7;
    Lines.AddLine("text");
    Link = "";

    SetPCWProperties(pcwprop);
}

cpart_TEXT::~cpart_TEXT(void) {
    delete Bitmap;
    canvas.Destroy();
}

void cpart_TEXT::PostInit(void) {
    ChangeText(12, 4, 7);
}

void cpart_TEXT::LoadImage(void) {
    unsigned int max = 0;

    for (unsigned int l = 0; l < Lines.GetLinesCount(); l++) {
        if (Lines.GetLine(l).length() > max) {
            max = Lines.GetLine(l).length();
        }
    }

    Width = max * Size * 0.9;
    Height = 2 * Size * Lines.GetLinesCount();

    if (Width == 0) {
        Width = 10;
    }
    if (Height == 0) {
        Height = 10;
    }

    if (Bitmap) {
        delete Bitmap;
    }

    lxImage image(SpareParts.GetWindow());

    image.CreateBlank(Width, Height, Orientation, Scale, Scale);
    Bitmap = new lxBitmap(&image, SpareParts.GetWindow());
    image.Destroy();
    canvas.Destroy();
    canvas.Create(SpareParts.GetWindow()->GetWWidget(), Bitmap);
}

void cpart_TEXT::DrawOutput(const unsigned int i) {
    unsigned int l;
    lxRect rec;
    lxString Text;

    switch (output[i].id) {
        case O_TEXTB:

            font.SetPointSize(Size);

            canvas.SetColor(colortable[Bgcolor].r, colortable[Bgcolor].g, colortable[Bgcolor].b);
            canvas.Rectangle(1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);

            if (Link.length()) {
                canvas.SetColor(0, 0, 0xff);
                canvas.SetLineWidth(2);
                canvas.Rectangle(0, output[i].x1, output[i].y1, output[i].x2 - output[i].x1,
                                 output[i].y2 - output[i].y1);
            }

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

unsigned short cpart_TEXT::GetInputId(char* name) {
    if (strcmp(name, "DI_TEXT") == 0)
        return I_TEXTB;

    printf("Erro input '%s' don't have a valid id! \n", name);
    return -1;
}

unsigned short cpart_TEXT::GetOutputId(char* name) {
    if (strcmp(name, "DI_TEXT") == 0)
        return O_TEXTB;

    printf("Erro output '%s' don't have a valid id! \n", name);
    return 1;
}

lxString cpart_TEXT::WritePreferences(void) {
    char prefs[256];
    lxString Text = "";
    lxString link = Link;
    // avoid save empty field
    if (!link.length()) {
        link = " ";
    }
    for (unsigned int l = 0; l < Lines.GetLinesCount(); l++) {
        Text += Lines.GetLine(l) + '\a';
    }
    sprintf(prefs, "%hhu,%hhu,%hhu,%s%%%s", Size, Textcolor, Bgcolor, (const char*)link.c_str(),
            (const char*)Text.c_str());

    return prefs;
}

void cpart_TEXT::ReadPreferences(lxString value) {
    char text[4096];
    char link[2048];
    char* line;
    sscanf(value.c_str(), "%hhu,%hhu,%hhu,%[^%%]%%%[^\n]", &Size, &Textcolor, &Bgcolor, link, text);

    Lines.Clear();
    line = strtok(text, "\a\n");
    while (line) {
        Lines.AddLine(line);
        line = strtok(NULL, "\a\n");
    }

    if (strcmp(link, " ")) {
        Link = link;
    } else {
        Link = "";
    }

    ChangeText(Size, Textcolor, Bgcolor);

    Reset();
}

void cpart_TEXT::ChangeText(int size, int textcolor, int bgcolor) {
    Size = size;
    Textcolor = textcolor;
    Bgcolor = bgcolor;

    LoadImage();

    input_ids[I_TEXTB]->x1 = 0;
    input_ids[I_TEXTB]->x2 = Width;
    input_ids[I_TEXTB]->y1 = 0;
    input_ids[I_TEXTB]->y2 = Height;

    output_ids[O_TEXTB]->x1 = 0;
    output_ids[O_TEXTB]->x2 = Width;
    output_ids[O_TEXTB]->y1 = 0;
    output_ids[O_TEXTB]->y2 = Height;

    output_ids[O_TEXTB]->update = 1;
}

void cpart_TEXT::RegisterRemoteControl(void) {}

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

    ((CSpin*)WProp->GetChildByName("spin2"))->SetValue(Size);
    ((CSpin*)WProp->GetChildByName("spin2"))->SetMin(6);
    ((CSpin*)WProp->GetChildByName("spin2"))->SetMax(100);

    ((CCombo*)WProp->GetChildByName("combo3"))->SetItems(Colors);
    ((CCombo*)WProp->GetChildByName("combo3"))->SetText(Colorname[Textcolor]);

    ((CCombo*)WProp->GetChildByName("combo4"))->SetItems(Colors);
    ((CCombo*)WProp->GetChildByName("combo4"))->SetText(Colorname[Bgcolor]);

    ((CEdit*)WProp->GetChildByName("edit5"))->SetText(Link);
}

void cpart_TEXT::ReadPropertiesWindow(CPWindow* WProp) {
    Lines.Clear();
    CText* Text = ((CText*)WProp->GetChildByName("text1"));
    for (unsigned int l = 0; l < Text->GetCountLines(); l++) {
        if (Text->GetLine(l).length() > 0) {
            Lines.AddLine(Text->GetLine(l));
        }
    }

    Size = ((CSpin*)WProp->GetChildByName("spin2"))->GetValue();

    lxString val = ((CCombo*)WProp->GetChildByName("combo3"))->GetText();
    for (int j = 0; j < C_END; j++) {
        if (!val.compare(Colorname[j])) {
            Textcolor = j;
            break;
        }
    }

    val = ((CCombo*)WProp->GetChildByName("combo4"))->GetText();
    for (int j = 0; j < C_END; j++) {
        if (!val.compare(Colorname[j])) {
            Bgcolor = j;
            break;
        }
    }

    Link = ((CEdit*)WProp->GetChildByName("edit5"))->GetText();

    ChangeText(Size, Textcolor, Bgcolor);
}

void cpart_TEXT::OnMouseButtonPress(uint inputId, uint button, uint x, uint y, uint state) {
    if ((button == 1) && Link.length()) {
        lxLaunchDefaultBrowser(Link);
    }
}

part_init(PART_TEXT_Name, cpart_TEXT, "Virtual");
