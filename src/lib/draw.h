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

#ifndef PCANVACMD_H
#define PCANVACMD_H

enum PICSimLabCanvasCmd {
    CC_INIT,
    CC_CHANGESCALE,
    CC_END,
    CC_SETBITMAP,
    CC_SETCOLOR,
    CC_SETFGCOLOR,
    CC_SETBGCOLOR,
    CC_SETFONTSIZE,
    CC_SETFONTWEIGHT,
    CC_SETLINEWIDTH,
    CC_POINT,
    CC_LINE,
    CC_RECTANGLE,
    CC_CIRCLE,
    CC_ROTATEDTEXT,
    CC_TEXTONRECT,
    CC_POLYGON,
    CC_PUTBITMAP,
    CC_GETBGCOLOR,
    CC_CREATE,
    CC_DESTROY,
    CC_FREEBITMAP,
    CC_GETBITMAPSIZE,
    CC_LOADIMAGE,
    CC_CREATEIMAGE,
    CC_ARC,
    CC_ELLIPTICARC,
    CC_LINES,
    CC_LAST
};

#define CC_FONTWEIGHT_BOLD 92

#define CC_ALIGN_LEFT 0x0000
#define CC_ALIGN_RIGHT 0x0200
#define CC_ALIGN_CENTER_VERTICAL 0x0800
#define CC_ALIGN_CENTER 0x0900

#include <string>

#include "board.h"

typedef struct {
    int x, y, width, height;
} Rect_t;

typedef struct {
    int x, y;
} Point_t;

typedef struct {
    PICSimLabCanvasCmd cmd;
    union {
        struct {
            const double sx;
            const double sy;
            const int angle;
        } Init;
        struct {
            const double sx;
            const double sy;
        } ChangeScale;
        struct {
        } End;
        struct {
            const int BitmapId;
            const double xs;
            const double ys;
        } SetBitmap;
        struct {
            const unsigned int r;
            const unsigned int g;
            const unsigned int b;
        } SetColor;
        struct {
            const unsigned int r;
            const unsigned int g;
            const unsigned int b;
        } SetFgColor;
        struct {
            const unsigned int r;
            const unsigned int g;
            const unsigned int b;
        } SetBgColor;
        struct {
            const int pointsize;
        } SetFontSize;
        struct {
            const int weight;
        } SetFontWeight;
        struct {
            const unsigned int lwidth;
        } SetLineWidth;
        struct {
            float x;
            float y;
        } Point;
        struct {
            float x1;
            float y1;
            float x2;
            float y2;
        } Line;
        struct {
            const bool filled;
            float x;
            float y;
            const float width;
            const float height;
        } Rectangle;
        struct {
            const bool filled;
            float x;
            float y;
            const float radius;
        } Circle;
        struct {
            const char* str;
            float x;
            float y;
            const float angle;
        } RotatedText;
        struct {
            const char* str;
            const Rect_t rect;
            const unsigned int align;
        } TextOnRect;
        struct {
            const bool filled;
            const Point_t* points;
            const int npoints;
        } Polygon;
        struct {
            const int BitmapId;
            float x;
            float y;
        } PutBitmap;
        struct {
            unsigned int* r;
            unsigned int* g;
            unsigned int* b;
        } GetBgColor;
        struct {
            const int BitmapId;
        } Create;
        struct {
        } Destroy;
        struct {
            const int BitmapId;
        } FreeBitmap;
        struct {
            const int BitmapId;
            unsigned int* w;
            unsigned int* h;
        } GetBitmapSize;
        struct {
            const char* fname;
            const float scale;
            const int usealpha;
            const int orientation;
        } LoadImage;
        struct {
            const unsigned int width;
            const unsigned int height;
            const float scale;
            const int usealpha;
            const int orientation;
        } CreateImage;
        struct {
            bool filled;
            float x1;
            float y1;
            float x2;
            float y2;
            float xc;
            float yc;
        } Arc;
        struct {
            bool filled;
            float x;
            float y;
            float width;
            float height;
            double start;
            double end;
        } EllipticArc;
        struct {
            const Point_t* points;
            const int npoints;
        } Lines;
    };
} CanvasCmd_t;

typedef int (*CanvasCmd_ft)(CanvasCmd_t);

// Draw Functions
void DrawLED(CanvasCmd_ft CanvasCmd, const output_t* output);
void DrawSlider(CanvasCmd_ft CanvasCmd, const output_t* output, const unsigned char pos, const std::string val,
                const int FontPointSize);
void DrawPotentiometer(CanvasCmd_ft CanvasCmd, const output_t* output, const unsigned char pos, const std::string val,
                       const int FontPointSize);

#endif  // PCANVACMD_H