/* ########################################################################

   PICSimLab - Programmable IC Simulator Laboratory

   ########################################################################

   Copyright (c) : 2010-2024  Luis Claudio Gambôa Lopes <lcgamboa@yahoo.com>

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
    CC_LAST
};

#include <string>

#include <lxrad.h>  //FIXME remove lxrad

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
            lxBitmap* bitmap;
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
            lxBitmap* bitmap;
            float x;
            float y;
        } PutBitmap;
        struct {
            unsigned int* r;
            unsigned int* g;
            unsigned int* b;
        } GetBgColor;
        struct {
            lxBitmap* bitmap;
        } Create;
        struct {
        } Destroy;
    };
} CanvasCmd_t;

// Draw Functions
void DrawLED(void (*CanvasCmd)(CanvasCmd_t), const output_t* output);
void DrawSlider(void (*CanvasCmd)(CanvasCmd_t), const output_t* output, const unsigned char pos, const std::string val,
                const int FontPointSize);
void DrawPotentiometer(void (*CanvasCmd)(CanvasCmd_t), const output_t* output, const unsigned char pos,
                       const std::string val, const int FontPointSize);

#endif  // PCANVACMD_H