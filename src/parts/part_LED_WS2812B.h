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

#ifndef PART_LED_WS2812B_H
#define PART_LED_WS2812B_H

#include "../devices/led_ws2812b.h"
#include "part.h"
#include <lxrad.h>

#define PART_LED_WS2812B_Name "RGB LED WS2812B"

class cpart_led_ws2812b : public part {
public:
  lxString GetName(void) override { return lxT(PART_LED_WS2812B_Name); };
  lxString GetAboutInfo(void) override {
    return lxT("L.C. Gamboa \n <lcgamboa@yahoo.com>");
  };
  cpart_led_ws2812b(unsigned x, unsigned y);
  ~cpart_led_ws2812b(void);
  void Draw(void) override;
  void PreProcess(void) override;
  void Process(void) override;
  void PostProcess(void) override;
  void ConfigurePropertiesWindow(CPWindow *WProp) override;
  void ReadPropertiesWindow(CPWindow *WProp) override;
  lxString WritePreferences(void) override;
  void ReadPreferences(lxString value) override;
  void LoadImage(void) override;
  unsigned short get_in_id(char *name) override;
  unsigned short get_out_id(char *name) override;

private:
  void ChangeType(const unsigned int rows,const unsigned int cols, const unsigned char diffuser );
  void RegisterRemoteControl(void) override;
  unsigned char input_pins[1];
  unsigned char output_pins[1];
  led_ws2812b_t led;
  lxFont font;
  int OWidth;
  int OHeight;
  int xoff;
  int yoff;
};

#endif /* PART_LED_WS2812B_H */
