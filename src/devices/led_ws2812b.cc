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

#define dprint                                                                 \
  if (1) {                                                                     \
  } else                                                                       \
    printf

#include "led_ws2812b.h"

void led_ws2812b_rst(led_ws2812b_t *led) {
  unsigned int i;
  for (i = 0; i < led->nleds; i++)
    led->color[i].fcolor = 0xFFFFFFF;
  led->update = 1;
  led->dat = 0;
  led->adin = 0;
  led->ccounter = 0;
  led->bit = 0;
}

void led_ws2812b_init(led_ws2812b_t *led, const int rows, const int cols,
                      const int diffuser) {
  led->diffuser = diffuser;
  led->nrows = rows;
  led->ncols = cols;
  led->nleds = rows * cols;
  led->nbits = 24 * led->nleds; 
  led->color = (rgb_color *)malloc(led->nleds * sizeof(rgb_color));

  led_ws2812b_rst(led);
}

void led_ws2812b_end(led_ws2812b_t *led) { free(led->color); }

void led_ws2812b_prepare(led_ws2812b_t *led, float freq) {
  // convert time in number of instructions
  led->T0H = freq * 0.4e-6;
  // led->T0L = freq * 0.85e-6;
  // led->T1H = freq * 0.8e-6;
  // led->T1L = freq * 0.45e-6;
  led->TRES = freq * 50e-6;
}

unsigned char led_ws2812b_io(led_ws2812b_t *led, const unsigned char din) {
  led->ccounter++;

  if (led->ccounter > led->TRES) {
    led->bit = 0;
    led->dat = 0;
    led->ledc = 0;
  }

  if (led->bit < led->nbits ) {
    if ((!led->adin) && (din)) // DIN rising edge
    {
      led->ccounter = 0;
    }
    if ((led->adin) && (!din)) // DIN falling edge
    {
      led->dat = led->dat << 1;
      if (led->ccounter > led->T0H) {
        led->dat |= 1;
      }
      led->bit++;
      led->ccounter = 0;
    }

    led->adin = din;

    if (led->bit == (24*(led->ledc+1)) ) {
      // printf("color = 0x%06X \n",led->dat);
      led->color[led->ledc].fcolor = led->dat;
      led->ledc++;
      led->update = 1;
    }
    return 0;
  }

  if ((led->adin) != (din)) // DIN rising/falling edge
  {
    led->ccounter = 0;
  }
  // bit > 24 * nleds
  return din;
}

void led_ws2812b_draw(led_ws2812b_t *led, CCanvas *canvas, const int x1,
                      const int y1, const int w1, const int h1,
                      const int picpwr) {
  unsigned int x, y, index;
  int R,G,B; 
  led->update = 0;
  canvas->SetFgColor(0, 0, 0);
  for (x = 0; x < led->nrows; x++) {
    for (y = 0; y < led->ncols; y++) {
      index = (x * led->ncols) + y;

      R= ((led->color[index].R*4) > 255) ? 255 : (led->color[index].R*4);
      G= ((led->color[index].G*4) > 255) ? 255 : (led->color[index].G*4);
      B= ((led->color[index].B*4) > 255) ? 255 : (led->color[index].B*4);
    
      canvas->SetBgColor(R,G,B);

      if (led->diffuser) {
        canvas->Rectangle(1, x1 + (y * 40) - 8, y1 - (x * 40) - 8, w1 + 16,
                          h1 + 16);
      } else {
        canvas->SetFgColor(led->color[index].R, led->color[index].G,
                           led->color[index].B);
        canvas->Circle(1, x1 + (y * 40) + 12, y1 - (x * 40) + 12, 7);
      }
    }
  }
}
