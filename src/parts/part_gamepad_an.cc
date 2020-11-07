/* ########################################################################

   PICsimLab - PIC laboratory simulator

   ########################################################################

   Copyright (c) : 2010-2020  Luis Claudio Gambôa Lopes

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
#include"part_gamepad_an.h"

/* outputs */
enum
{
 O_P1
};

/* inputs */
enum
{
 I_B1, I_B2, I_B3, I_B4, I_B5
};

cpart_gamepad_an::cpart_gamepad_an(unsigned x, unsigned y)
{
 X = x;
 Y = y;
 active = 1;

 ReadMaps ();

 lxImage image;
 image.LoadFile (Window1.GetSharePath () + lxT ("parts/") + GetPictureFileName ());

 Bitmap = lxGetBitmapRotated(&image, &Window5, orientation);
 image.Destroy ();

 canvas.Create (Window5.GetWWidget (), Bitmap);



 output_pins[0] = 0;

 output_value_an = active * 5.0;

}

void
cpart_gamepad_an::Reset(void)
{
 //release all
 output_value_an = active * 5.0;

 Window5.SetAPin (output_pins[0], output_value_an);

}

cpart_gamepad_an::~cpart_gamepad_an(void)
{
 delete Bitmap;
 canvas.Destroy ();
}

void
cpart_gamepad_an::Draw(void)
{

 int i;
 lxString temp;
 canvas.Init (1.0, 1.0, orientation);

 lxFont font (9, lxFONTFAMILY_TELETYPE, lxFONTSTYLE_NORMAL, lxFONTWEIGHT_BOLD);
 canvas.SetFont (font);

 for (i = 0; i < outputc; i++)
  {

   switch (output[i].id)
    {
    case O_P1:
     canvas.SetColor (49, 61, 99);
     canvas.Rectangle (1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1+30, output[i].y2 - output[i].y1+30);
     canvas.SetFgColor (255, 255, 255);

     temp=lxString().Format("%3.1f", output_value_an)+lxT("V");
     canvas.RotatedText ("Out= "+temp, output[i].x1, output[i].y1+30, 0);
     if (output_pins[0] == 0)
      canvas.RotatedText ("NC", output[i].x1, output[i].y1, 0);
     else
      canvas.RotatedText (Window5.GetPinName (output_pins[0]), output[i].x1, output[i].y1, 0);
     break;
    }


  };

 canvas.End ();

}

void
cpart_gamepad_an::PreProcess(void)
{
   Window5.SetAPin (output_pins[0], output_value_an);
}

void
cpart_gamepad_an::EvMouseButtonPress(uint button, uint x, uint y, uint state)
{
 int i;

 for (i = 0; i < inputc; i++)
  {
   if (PointInside(x, y, input[i]))
    {

     switch (input[i].id)
      {

      case I_B1: output_value_an = (active * 5.0 * (0.25 / 5.0))+((!active)*5.0 * (3.75 / 5.0));
       break;
      case I_B2: output_value_an = (active * 5.0 * (0.75 / 5.0))+((!active)*5.0 * (2.5 / 5.0));
       break;
      case I_B3: output_value_an = (active * 5.0 * (1.5 / 5.0))+((!active)*5.0  * (1.5 / 5.0));
       break;
      case I_B4: output_value_an = (active * 5.0 * (2.5 / 5.0))+((!active)*5.0  * (0.75 / 5.0));
       break;
      case I_B5: output_value_an = (active * 5.0 * (3.75 / 5.0))+((!active)*5.0 * (0.25 / 5.0));
       break;
      }
    }
  }
}

void
cpart_gamepad_an::EvMouseButtonRelease(uint button, uint x, uint y, uint state)
{
 int i;

 for (i = 0; i < inputc; i++)
  {
   if (PointInside(x, y, input[i]))
    {
     switch (input[i].id)
      {
      case I_B1: output_value_an = active * 5.0;
       break;
      case I_B2: output_value_an = active * 5.0;
       break;
      case I_B3: output_value_an = active * 5.0;
       break;
      case I_B4: output_value_an = active * 5.0;
       break;
      case I_B5: output_value_an = active * 5.0;
       break;
      }
    }
  }
}

void
cpart_gamepad_an::EvKeyPress(uint key, uint mask)
{

 switch (key)
  {
  case 'I':
  case 'i':
   output_value_an = (active * 5.0 * (0.75 / 5.0))+((!active)*5.0 * (2.5 / 5.0));
   break;
  case 'J':
  case 'j':
   output_value_an = (active * 5.0 * (2.5 / 5.0))+((!active)*5.0  * (0.75 / 5.0));
   break;
  case 'K':
  case 'k':
   output_value_an = (active * 5.0 * (1.5 / 5.0))+((!active)*5.0  * (1.5 / 5.0));
   break;
  case 'L':
  case 'l':
   output_value_an = (active * 5.0 * (0.25 / 5.0))+((!active)*5.0 * (3.75 / 5.0));
   break;
  case 'O':
  case 'o':
   output_value_an = (active * 5.0 * (3.75 / 5.0))+((!active)*5.0 * (0.25 / 5.0));
   break;
  }
}

void
cpart_gamepad_an::EvKeyRelease(uint key, uint mask)
{
 switch (key)
  {
  case 'I':
  case 'i':
   output_value_an = active * 5.0;
   break;
  case 'J':
  case 'j':
   output_value_an = active * 5.0;
   break;
  case 'K':
  case 'k':
   output_value_an = active * 5.0;
   break;
  case 'L':
  case 'l':
   output_value_an = active * 5.0;
   break;
  case 'O':
  case 'o':
   output_value_an = active * 5.0;
   break;
  }
}

unsigned short
cpart_gamepad_an::get_in_id(char * name)
{
 if (strcmp (name, "B1") == 0)return I_B1;
 if (strcmp (name, "B2") == 0)return I_B2;
 if (strcmp (name, "B3") == 0)return I_B3;
 if (strcmp (name, "B4") == 0)return I_B4;
 if (strcmp (name, "B5") == 0)return I_B5;

 printf ("Erro input '%s' don't have a valid id! \n", name);
 return -1;
}

unsigned short
cpart_gamepad_an::get_out_id(char * name)
{

 if (strcmp (name, "P1") == 0)return O_P1;

 printf ("Erro output '%s' don't have a valid id! \n", name);
 return 1;
}

lxString
cpart_gamepad_an::WritePreferences(void)
{
 char prefs[256];

 sprintf (prefs, "%hhu,%hhu", output_pins[0], active);

 return prefs;
}

void
cpart_gamepad_an::ReadPreferences(lxString value)
{
 sscanf (value.c_str (), "%hhu,%hhu", &output_pins[0], &active);
 output_value_an = active * 5.0;
}


void
cpart_gamepad_an::ConfigurePropertiesWindow(CPWindow * WProp)
{
 lxString Items = Window5.GetPinsNames ();
 lxString spin;

 ((CCombo*) WProp->GetChildByName ("combo1"))->SetItems (Items);
 if (output_pins[0] == 0)
  ((CCombo*) WProp->GetChildByName ("combo1"))->SetText ("0  NC");
 else
  {
   spin = Window5.GetPinName (output_pins[0]);
   ((CCombo*) WProp->GetChildByName ("combo1"))->SetText (itoa (output_pins[0]) + "  " + spin);
  }

 if (active)
  ((CCombo*) WProp->GetChildByName ("combo9"))->SetText ("HIGH");
 else
  ((CCombo*) WProp->GetChildByName ("combo9"))->SetText ("LOW ");


 ((CButton*) WProp->GetChildByName ("button1"))->EvMouseButtonRelease = EVMOUSEBUTTONRELEASE & CPWindow5::PropButtonRelease;
 ((CButton*) WProp->GetChildByName ("button1"))->SetTag (1);

 ((CButton*) WProp->GetChildByName ("button2"))->EvMouseButtonRelease = EVMOUSEBUTTONRELEASE & CPWindow5::PropButtonRelease;
}

void
cpart_gamepad_an::ReadPropertiesWindow(CPWindow * WProp)
{
 output_pins[0] = atoi (((CCombo*) WProp->GetChildByName ("combo1"))->GetText ());


 active = (((CCombo*) WProp->GetChildByName ("combo9"))->GetText ().compare ("HIGH") == 0);

 output_value_an = active * 5.0;

}

part_init("Gamepad (Analogic)", cpart_gamepad_an);

