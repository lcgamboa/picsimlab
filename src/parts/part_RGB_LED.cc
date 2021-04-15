/* ########################################################################

   PICsimLab - PIC laboratory simulator

   ########################################################################

   Copyright (c) : 2010-2018  Luis Claudio Gambôa Lopes

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

#include"../picsimlab1.h"
#include"../picsimlab4.h"
#include"../picsimlab5.h"
#include"part_RGB_LED.h"

/* outputs */
enum
{
 O_P1, O_P2, O_P3, O_L1
};

cpart_rgb_led::cpart_rgb_led(unsigned x, unsigned y)
:font (9, lxFONTFAMILY_TELETYPE, lxFONTSTYLE_NORMAL, lxFONTWEIGHT_BOLD)
{
 X = x;
 Y = y;
 active = 1;

 ReadMaps ();

 lxImage image(&Window5);
 image.LoadFile (Window1.GetSharePath () + lxT ("parts/") + GetPictureFileName (), Orientation, Scale, Scale);

 Bitmap = new lxBitmap (&image, &Window5);
 image.Destroy ();
 canvas.Create (Window5.GetWWidget (), Bitmap);



 input_pins[0] = 0;
 input_pins[1] = 0;
 input_pins[2] = 0;

 color[0] = 0;
 color[1] = 0;
 color[2] = 0;

}

cpart_rgb_led::~cpart_rgb_led(void)
{
 delete Bitmap;
 canvas.Destroy ();
}

void
cpart_rgb_led::Draw(void)
{

 int i;

 const picpin * ppins = Window5.GetPinsValues ();

 canvas.Init (Scale, Scale, Orientation);
 canvas.SetFont (font);

 for (i = 0; i < outputc; i++)
  {

   switch (output[i].id)
    {
    case O_P1:
    case O_P2:
    case O_P3:
     canvas.SetColor (49, 61, 99);
     canvas.Rectangle (1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);
     canvas.SetFgColor (255, 255, 255);
     if (input_pins[output[i].id - O_P1] == 0)
      canvas.RotatedText ("NC", output[i].x1, output[i].y1, 0);
     else
      canvas.RotatedText (Window5.GetPinName (input_pins[output[i].id - O_P1]), output[i].x1, output[i].y1, 0);
     break;
    case O_L1:
     if (input_pins[0] > 0)
      color[0] = ppins[input_pins[0] - 1].oavalue;
     if (input_pins[1] > 0)
      color[1] = ppins[input_pins[1] - 1].oavalue;
     if (input_pins[2] > 0)
      color[2] = ppins[input_pins[2] - 1].oavalue;
     
     if(!active)
      {
        color[0]=285-color[0];
        color[1]=285-color[1];
        color[2]=285-color[2];
      }
   
     canvas.SetColor (255, 255, 224);
     canvas.Circle (1, output[i].x1, output[i].y1, output[i].r-5);
     canvas.SetFgColor (0, 0, 0);
     canvas.SetBgColor (color[0], color[1], color[2]);
     canvas.Circle (1, output[i].x1-0.5, output[i].y1, output[i].r-7);
     break;
    }

  }


 canvas.End ();

}

unsigned short
cpart_rgb_led::get_in_id(char * name)
{
 printf ("Erro input '%s' don't have a valid id! \n", name);
 return -1;
}

unsigned short
cpart_rgb_led::get_out_id(char * name)
{

 if (strcmp (name, "PN_1") == 0)return O_P1;
 if (strcmp (name, "PN_2") == 0)return O_P2;
 if (strcmp (name, "PN_3") == 0)return O_P3;

 if (strcmp (name, "LR_1") == 0)return O_L1;

 printf ("Erro output '%s' don't have a valid id! \n", name);
 return 1;
}

lxString
cpart_rgb_led::WritePreferences(void)
{
 char prefs[256];

 sprintf (prefs, "%hhu,%hhu,%hhu,%hhu", input_pins[0], input_pins[1], input_pins[2], active);

 return prefs;
}

void
cpart_rgb_led::ReadPreferences(lxString value)
{
 sscanf (value.c_str (), "%hhu,%hhu,%hhu,%hhu", &input_pins[0], &input_pins[1], &input_pins[2], &active);
 RegisterRemoteControl ();
}

void
cpart_rgb_led::RegisterRemoteControl(void)
{
 for (int i = 0; i < outputc; i++)
  {
   switch (output[i].id)
    {
    case O_L1:
     output[i].status = (void *) color;
     break;
    }
  }
}

void
cpart_rgb_led::ConfigurePropertiesWindow(CPWindow * WProp)
{
 lxString Items = Window5.GetPinsNames ();
 lxString spin;

 ((CCombo*) WProp->GetChildByName ("combo1"))->SetItems (Items);
 if (input_pins[0] == 0)
  ((CCombo*) WProp->GetChildByName ("combo1"))->SetText ("0  NC");
 else
  {
   spin = Window5.GetPinName (input_pins[0]);
   ((CCombo*) WProp->GetChildByName ("combo1"))->SetText (itoa (input_pins[0]) + "  " + spin);
  }

 ((CCombo*) WProp->GetChildByName ("combo2"))->SetItems (Items);
 if (input_pins[1] == 0)
  ((CCombo*) WProp->GetChildByName ("combo2"))->SetText ("0  NC");
 else
  {
   spin = Window5.GetPinName (input_pins[1]);
   ((CCombo*) WProp->GetChildByName ("combo2"))->SetText (itoa (input_pins[1]) + "  " + spin);
  }

 ((CCombo*) WProp->GetChildByName ("combo3"))->SetItems (Items);
 if (input_pins[2] == 0)
  ((CCombo*) WProp->GetChildByName ("combo3"))->SetText ("0  NC");
 else
  {
   spin = Window5.GetPinName (input_pins[2]);
   ((CCombo*) WProp->GetChildByName ("combo3"))->SetText (itoa (input_pins[2]) + "  " + spin);
  }

 if (active)
  ((CCombo*) WProp->GetChildByName ("combo4"))->SetText ("HIGH");
 else
  ((CCombo*) WProp->GetChildByName ("combo4"))->SetText ("LOW ");



 ((CButton*) WProp->GetChildByName ("button1"))->EvMouseButtonRelease = EVMOUSEBUTTONRELEASE & CPWindow5::PropButtonRelease;
 ((CButton*) WProp->GetChildByName ("button1"))->SetTag (1);

 ((CButton*) WProp->GetChildByName ("button2"))->EvMouseButtonRelease = EVMOUSEBUTTONRELEASE & CPWindow5::PropButtonRelease;
}

void
cpart_rgb_led::ReadPropertiesWindow(CPWindow * WProp)
{
 input_pins[0] = atoi (((CCombo*) WProp->GetChildByName ("combo1"))->GetText ());
 input_pins[1] = atoi (((CCombo*) WProp->GetChildByName ("combo2"))->GetText ());
 input_pins[2] = atoi (((CCombo*) WProp->GetChildByName ("combo3"))->GetText ());

 active = (((CCombo*) WProp->GetChildByName ("combo4"))->GetText ().compare ("HIGH") == 0);

 RegisterRemoteControl ();
}

part_init("RGB LED", cpart_rgb_led, "Output");

