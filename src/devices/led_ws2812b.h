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

#ifndef LED_WS2812B
#define LED_WS2812B

#include <lxrad.h>

/* pinout
  1 VDD
  2 DOUT
  3 VSS
  4 DIN
*/

#define MAXROWS 64
#define MAXCOLS 64

typedef struct
{
union
{
  unsigned int fcolor;
  struct 
  {
    unsigned char B;
    unsigned char R;
    unsigned char G;   
    unsigned char PAD;
  };
};
} rgb_color;


typedef struct {
  int T0H;
  int TRES;
  int ccounter; //cycle counter
  unsigned int bit; //bit counter
  unsigned int nrows;
  unsigned int ncols;
  unsigned int nleds;
  unsigned int nbits;
  unsigned int ledc; //led counter
  unsigned char diffuser;
  rgb_color * color; //RGB array
  unsigned short adin;
  unsigned int dat;
  unsigned char update;
} led_ws2812b_t;

void led_ws2812b_rst(led_ws2812b_t *led);
void led_ws2812b_init(led_ws2812b_t *led, const int rows, const int cols, const int diffuser);
void led_ws2812b_end(led_ws2812b_t *led);
void led_ws2812b_prepare(led_ws2812b_t *led, const float freq);

unsigned char led_ws2812b_io(led_ws2812b_t *led, const unsigned char din);

void led_ws2812b_draw(led_ws2812b_t *led, CCanvas *canvas, const int x1,
                      const int y1, const int w1, const int h1,
                      const int picpwr);

#endif // LED_WS2812B
