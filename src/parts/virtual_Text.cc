/* ########################################################################

   PICSimLab - Programmable IC Simulator Laboratory

   ########################################################################

   Copyright (c) : 2022-2024  Luis Claudio Gambôa Lopes <lcgamboa@yahoo.com>

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

cpart_TEXT::cpart_TEXT(const unsigned x, const unsigned y, const char* name, const char* type, board* pboard_,
                       const int id_)
    : part(x, y, name, type, pboard_, id_, 8) {
    BitmapId = -1;
    Size = 12;
    Textcolor = 4;
    Bgcolor = 7;
    Lines.push_back("text");
    Link = "";

    SetPCWProperties(pcwprop);
}

cpart_TEXT::~cpart_TEXT(void) {
    SpareParts.SetPartOnDraw(id);
    SpareParts.CanvasCmd({CC_FREEBITMAP, .FreeBitmap{BitmapId}});
    SpareParts.CanvasCmd({CC_DESTROY});
}

void cpart_TEXT::PostInit(void) {
    ChangeText(12, 4, 7);
}

void cpart_TEXT::LoadPartImage(void) {
    unsigned int max = 0;

    for (unsigned int l = 0; l < Lines.size(); l++) {
        if (Lines.at(l).length() > max) {
            max = Lines.at(l).length();
        }
    }

    Width = max * Size * 0.9;
    Height = 2 * Size * Lines.size();

    if (Width == 0) {
        Width = 10;
    }
    if (Height == 0) {
        Height = 10;
    }

    SpareParts.SetPartOnDraw(id);
    if (BitmapId >= 0) {
        SpareParts.CanvasCmd({CC_FREEBITMAP, .FreeBitmap{BitmapId}});
    }

    BitmapId = SpareParts.CanvasCmd({CC_CREATEIMAGE, .CreateImage{Width, Height, Scale, 0, Orientation}});
    SpareParts.CanvasCmd({CC_DESTROY});
    SpareParts.CanvasCmd({CC_CREATE, .Create{BitmapId}});
}

void cpart_TEXT::DrawOutput(const unsigned int i) {
    unsigned int l;
    Rect_t rec;
    std::string Text;

    switch (output[i].id) {
        case O_TEXTB:

            SpareParts.CanvasCmd({CC_SETFONTSIZE, .SetFontSize{Size}});

            SpareParts.CanvasCmd(
                {CC_SETCOLOR, .SetColor{colortable[Bgcolor].r, colortable[Bgcolor].g, colortable[Bgcolor].b}});
            SpareParts.CanvasCmd({CC_RECTANGLE, .Rectangle{1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1,
                                                           output[i].y2 - output[i].y1}});

            if (Link.length()) {
                SpareParts.CanvasCmd({CC_SETCOLOR, .SetColor{0, 0, 0xff}});
                SpareParts.CanvasCmd({CC_SETLINEWIDTH, .SetLineWidth{2}});
                SpareParts.CanvasCmd(
                    {CC_RECTANGLE, .Rectangle{0, output[i].x1, output[i].y1, output[i].x2 - output[i].x1,
                                              output[i].y2 - output[i].y1}});
            }

            SpareParts.CanvasCmd(
                {CC_SETCOLOR, .SetColor{colortable[Textcolor].r, colortable[Textcolor].g, colortable[Textcolor].b}});
            rec.x = output[i].x1;
            rec.y = output[i].y1;
            rec.width = output[i].x2 - output[i].x1;
            rec.height = output[i].y2 - output[i].y1;

            Text = Lines.at(0);
            for (l = 1; l < Lines.size(); l++) {
                Text += "\n" + Lines.at(l);
            }

            SpareParts.CanvasCmd(
                {CC_TEXTONRECT, .TextOnRect{Text.c_str(), rec, CC_ALIGN_CENTER | CC_ALIGN_CENTER_VERTICAL}});
            break;
    }
}

unsigned short cpart_TEXT::GetInputId(char* name) {
    if (strcmp(name, "DI_TEXT") == 0)
        return I_TEXTB;

    printf("Error input '%s' don't have a valid id! \n", name);
    return INVALID_ID;
}

unsigned short cpart_TEXT::GetOutputId(char* name) {
    if (strcmp(name, "DI_TEXT") == 0)
        return O_TEXTB;

    printf("Error output '%s' don't have a valid id! \n", name);
    return INVALID_ID;
}

std::string cpart_TEXT::WritePreferences(void) {
    char prefs[4096];
    std::string Text = "";
    std::string link = Link;
    // avoid save empty field
    if (!link.length()) {
        link = " ";
    }
    for (unsigned int l = 0; l < Lines.size(); l++) {
        Text += Lines.at(l) + '\a';
    }
    sprintf(prefs, "%hhu,%hhu,%hhu,%.1024s%%%.2048s", Size, Textcolor, Bgcolor, (const char*)link.c_str(),
            (const char*)Text.c_str());
    return prefs;
}

void cpart_TEXT::ReadPreferences(std::string value) {
    char text[2048 + 1024];
    char link[1024 + 1024];
    char* line;

    sscanf(value.c_str(), "%hhu,%hhu,%hhu,%[^%%]%%%[^\n]", &Size, &Textcolor, &Bgcolor, link, text);

    Lines.clear();
    line = strtok(text, "\a\n");
    while (line) {
        Lines.push_back(line);
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

    LoadPartImage();

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

void cpart_TEXT::ConfigurePropertiesWindow(void) {
    std::string Colors = "";

    for (int i = 0; i < C_END; i++) {
        Colors += Colorname[i];
        Colors += ",";
    }

    SpareParts.WPropCmd("text1", PWA_TEXTCLEAR, NULL);
    for (unsigned int l = 0; l < Lines.size(); l++) {
        SpareParts.WPropCmd("text1", PWA_TEXTADDLINE, Lines.at(l).c_str());
    }

    SpareParts.WPropCmd("spin2", PWA_SPINSETMAX, "100");
    SpareParts.WPropCmd("spin2", PWA_SPINSETMIN, "6");
    SpareParts.WPropCmd("spin2", PWA_SPINSETVALUE, std::to_string(Size).c_str());

    SpareParts.WPropCmd("combo3", PWA_COMBOSETITEMS, Colors.c_str());
    SpareParts.WPropCmd("combo3", PWA_COMBOSETTEXT, Colorname[Textcolor]);

    SpareParts.WPropCmd("combo4", PWA_COMBOSETITEMS, Colors.c_str());
    SpareParts.WPropCmd("combo4", PWA_COMBOSETTEXT, Colorname[Bgcolor]);

    SpareParts.WPropCmd("edit5", PWA_EDITSETTEXT, Link.c_str());
}

void cpart_TEXT::ReadPropertiesWindow(void) {
    Lines.clear();
    unsigned int linecount;
    SpareParts.WPropCmd("text1", PWA_TEXTGETLINECOUNT, NULL, &linecount);
    for (unsigned int l = 0; l < linecount; l++) {
        char buff[64];
        SpareParts.WPropCmd("text1", PWA_TEXTGETLINE, std::to_string(l).c_str(), buff);
        if (strlen(buff) > 0) {
            Lines.push_back(buff);
        }
    }

    int size;
    SpareParts.WPropCmd("spin2", PWA_SPINGETVALUE, NULL, &size);

    Size = size;

    char buff[64];
    SpareParts.WPropCmd("combo3", PWA_COMBOGETTEXT, NULL, buff);

    std::string val = buff;
    for (int j = 0; j < C_END; j++) {
        if (!val.compare(Colorname[j])) {
            Textcolor = j;
            break;
        }
    }

    SpareParts.WPropCmd("combo4", PWA_COMBOGETTEXT, NULL, buff);

    val = buff;
    for (int j = 0; j < C_END; j++) {
        if (!val.compare(Colorname[j])) {
            Bgcolor = j;
            break;
        }
    }

    SpareParts.WPropCmd("edit5", PWA_EDITGETTEXT, NULL, buff);

    Link = buff;

    ChangeText(Size, Textcolor, Bgcolor);
}

void cpart_TEXT::OnMouseButtonPress(unsigned int inputId, unsigned int button, unsigned int x, unsigned int y,
                                    unsigned int state) {
    if ((button == 1) && Link.length()) {
        PICSimLab.SystemCmd(PSC_LAUNCHDEFAULTBROWSER, Link.c_str());
    }
}

part_init(PART_TEXT_Name, cpart_TEXT, "Virtual");
