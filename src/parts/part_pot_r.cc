/* ########################################################################

   PICsimLab - PIC laboratory simulator

   ########################################################################

   Copyright (c) : 2010-2021  Luis Claudio Gambôa Lopes

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
#include"part_pot_r.h"

/* outputs */
enum
{
 O_PO1, O_PO2, O_PO3, O_PO4, O_P1, O_P2, O_P3, O_P4
};

/* inputs */
enum
{
 I_PO1, I_PO2, I_PO3, I_PO4
};

cpart_pot_r::cpart_pot_r(unsigned x, unsigned y):
font (9, lxFONTFAMILY_TELETYPE, lxFONTSTYLE_NORMAL, lxFONTWEIGHT_BOLD),
font_p (8, lxFONTFAMILY_TELETYPE, lxFONTSTYLE_NORMAL, lxFONTWEIGHT_BOLD)
{
 X = x;
 Y = y;
 ReadMaps ();

 lxImage image (&Window5);
 image.LoadFile (Window1.GetSharePath () + lxT ("parts/") + GetPictureFileName (), Orientation, Scale, Scale);

 Bitmap = new lxBitmap (&image, &Window5);
 image.Destroy ();

 canvas.Create (Window5.GetWWidget (), Bitmap);


 output_pins[0] = 0;
 output_pins[1] = 0;
 output_pins[2] = 0;
 output_pins[3] = 0;

 values[0] = 0;
 values[1] = 0;
 values[2] = 0;
 values[3] = 0;

 active[0] = 0;
 active[1] = 0;
 active[2] = 0;
 active[3] = 0;

 RegisterRemoteControl ();

}

void
cpart_pot_r::RegisterRemoteControl(void)
{
 for (int i = 0; i < inputc; i++)
  {
   switch (input[i].id)
    {
    case I_PO1:
     input[i].status = &values[0];
     break;
    case I_PO2:
     input[i].status = &values[1];
     break;
    case I_PO3:
     input[i].status = &values[2];
     break;
    case I_PO4:
     input[i].status = &values[3];
     break;
    }
  }
}

cpart_pot_r::~cpart_pot_r(void)
{
 delete Bitmap;
 canvas.Destroy ();
}

void
cpart_pot_r::Draw(void)
{

 int i;
 char val[10];

 canvas.Init (Scale, Scale, Orientation);

 canvas.SetFont (font);

 for (i = 0; i < outputc; i++)
  {

   switch (output[i].id)
    {
    case O_P1:
    case O_P2:
    case O_P3:
    case O_P4:
     canvas.SetColor (49, 61, 99);
     canvas.Rectangle (1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);
     canvas.SetFgColor (255, 255, 255);
     if (output_pins[output[i].id - O_P1] == 0)
      canvas.RotatedText ("NC", output[i].x1, output[i].y1, 0);
     else
      canvas.RotatedText (Window5.GetPinName (output_pins[output[i].id - O_P1]), output[i].x1, output[i].y1, 0);
     break;
    case O_PO1:
    case O_PO2:
    case O_PO3:
    case O_PO4:
     canvas.SetFgColor (0, 0, 0);
     canvas.SetBgColor (66, 109, 246);
     canvas.Rectangle (1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);

     canvas.SetBgColor (250, 250, 250);
     canvas.Circle (1, output[i].cx, output[i].cy, 17);

     canvas.SetBgColor (150, 150, 150);
     int x = -13 * sin ((5.585 * (values[output[i].id - O_PO1] / 200.0)) + 0.349);
     int y = 13 * cos ((5.585 * (values[output[i].id - O_PO1] / 200.0)) + 0.349);
     canvas.Circle (1, output[i].cx + x, output[i].cy + y, 2);

     canvas.SetColor (49, 61, 99);
     canvas.Rectangle (1, output[i].x1 + 6, output[i].y2 + 6, 30, 15);
     snprintf (val, 10, "%4.2f", 5.0 * (values[output[i].id - O_PO1]) / 200.0);
     canvas.SetColor (250, 250, 250);
     canvas.SetFont (font_p);
     canvas.RotatedText (val, output[i].x1 + 6, output[i].y2 + 6, 0);
     break;
    }


  }

 canvas.End ();

}

void
cpart_pot_r::PreProcess(void)
{
 Window5.SetAPin (output_pins[0], 5.0 * (values[0]) / 200.0);
 Window5.SetAPin (output_pins[1], 5.0 * (values[1]) / 200.0);
 Window5.SetAPin (output_pins[2], 5.0 * (values[2]) / 200.0);
 Window5.SetAPin (output_pins[3], 5.0 * (values[3]) / 200.0);
}

void
cpart_pot_r::EvMouseButtonPress(uint button, uint x, uint y, uint state)
{

 int i;

 for (i = 0; i < inputc; i++)
  {
   if (PointInside (x, y, input[i]))
    {
     RotateCoords (&x, &y);
     switch (input[i].id)
      {
      case I_PO1:
       values[0] = CalcAngle (i, x, y);
       active[0] = 1;
       break;
      case I_PO2:
       values[1] = CalcAngle (i, x, y);
       active[1] = 1;
       break;
      case I_PO3:
       values[2] = CalcAngle (i, x, y);
       active[2] = 1;
       break;
      case I_PO4:
       values[3] = CalcAngle (i, x, y);
       active[3] = 1;
       break;
      }
    }
  }

}

void
cpart_pot_r::EvMouseButtonRelease(uint button, uint x, uint y, uint state)
{
 int i;

 for (i = 0; i < inputc; i++)
  {
   if (PointInside (x, y, input[i]))
    {
     switch (input[i].id)
      {
      case I_PO1:
       active[0] = 0;
       break;
      case I_PO2:
       active[1] = 0;
       break;
      case I_PO3:
       active[2] = 0;
       break;
      case I_PO4:
       active[3] = 0;
       break;
      }
    }
  }
}

void
cpart_pot_r::EvMouseMove(uint button, uint x, uint y, uint state)
{

 int i;

 for (i = 0; i < inputc; i++)
  {
   if (PointInside (x, y, input[i]))
    {
     RotateCoords (&x, &y);

     if (active[input[i].id - I_PO1])
      {
       values[input[i].id - I_PO1] = CalcAngle (i, x, y);
      }
    }
  }
}

unsigned char
cpart_pot_r::CalcAngle(int i, int x, int y)
{
 int dx = input[i].cx - x;
 int dy = y - input[i].cy;
 double angle = 0;

 if ((dx >= 0)&&(dy >= 0))
  {
   angle = atan2 (dx, dy) * 180 / M_PI;
  }
 else if ((dx >= 0)&&(dy < 0))
  {
   angle = 180 - (atan2 (dx, -dy) * 180 / M_PI);
  }
 else if ((dx < 0)&&(dy < 0))
  {
   angle = (atan2 (-dx, -dy) * 180 / M_PI) + 180;
  }
 else if ((dx < 0)&&(dy >= 0))
  {
   angle = 360 - (atan2 (-dx, dy) * 180 / M_PI);
  }

 if (angle > 340) angle = 340;
 if (angle < 20) angle = 20;

 return (200 * (angle - 20) / 320.0);
}

unsigned short
cpart_pot_r::get_in_id(char * name)
{

 if (strcmp (name, "PO_1") == 0)return I_PO1;
 if (strcmp (name, "PO_2") == 0)return I_PO2;
 if (strcmp (name, "PO_3") == 0)return I_PO3;
 if (strcmp (name, "PO_4") == 0)return I_PO4;

 printf ("Erro input '%s' don't have a valid id! \n", name);
 return -1;
}

unsigned short
cpart_pot_r::get_out_id(char * name)
{

 if (strcmp (name, "PN_1") == 0)return O_P1;
 if (strcmp (name, "PN_2") == 0)return O_P2;
 if (strcmp (name, "PN_3") == 0)return O_P3;
 if (strcmp (name, "PN_4") == 0)return O_P4;

 if (strcmp (name, "PO_1") == 0)return O_PO1;
 if (strcmp (name, "PO_2") == 0)return O_PO2;
 if (strcmp (name, "PO_3") == 0)return O_PO3;
 if (strcmp (name, "PO_4") == 0)return O_PO4;

 printf ("Erro output '%s' don't have a valid id! \n", name);
 return 1;
}

lxString
cpart_pot_r::WritePreferences(void)
{
 char prefs[256];

 sprintf (prefs, "%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu", output_pins[0], output_pins[1], output_pins[2], output_pins[3], values[0], values[1], values[2], values[3]);

 return prefs;
}

void
cpart_pot_r::ReadPreferences(lxString value)
{
 sscanf (value.c_str (), "%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu", &output_pins[0], &output_pins[1], &output_pins[2], &output_pins[3], &values[0], &values[1], &values[2], &values[3]);
}

void
cpart_pot_r::ConfigurePropertiesWindow(CPWindow * WProp)
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

 ((CCombo*) WProp->GetChildByName ("combo2"))->SetItems (Items);
 if (output_pins[1] == 0)
  ((CCombo*) WProp->GetChildByName ("combo2"))->SetText ("0  NC");
 else
  {
   spin = Window5.GetPinName (output_pins[1]);
   ((CCombo*) WProp->GetChildByName ("combo2"))->SetText (itoa (output_pins[1]) + "  " + spin);
  }

 ((CCombo*) WProp->GetChildByName ("combo3"))->SetItems (Items);
 if (output_pins[2] == 0)
  ((CCombo*) WProp->GetChildByName ("combo3"))->SetText ("0  NC");
 else
  {
   spin = Window5.GetPinName (output_pins[2]);
   ((CCombo*) WProp->GetChildByName ("combo3"))->SetText (itoa (output_pins[2]) + "  " + spin);
  }

 ((CCombo*) WProp->GetChildByName ("combo4"))->SetItems (Items);
 if (output_pins[3] == 0)
  ((CCombo*) WProp->GetChildByName ("combo4"))->SetText ("0  NC");
 else
  {
   spin = Window5.GetPinName (output_pins[3]);
   ((CCombo*) WProp->GetChildByName ("combo4"))->SetText (itoa (output_pins[3]) + "  " + spin);
  }


 ((CButton*) WProp->GetChildByName ("button1"))->EvMouseButtonRelease = EVMOUSEBUTTONRELEASE & CPWindow5::PropButtonRelease;
 ((CButton*) WProp->GetChildByName ("button1"))->SetTag (1);

 ((CButton*) WProp->GetChildByName ("button2"))->EvMouseButtonRelease = EVMOUSEBUTTONRELEASE & CPWindow5::PropButtonRelease;
}

void
cpart_pot_r::ReadPropertiesWindow(CPWindow * WProp)
{
 output_pins[0] = atoi (((CCombo*) WProp->GetChildByName ("combo1"))->GetText ());
 output_pins[1] = atoi (((CCombo*) WProp->GetChildByName ("combo2"))->GetText ());
 output_pins[2] = atoi (((CCombo*) WProp->GetChildByName ("combo3"))->GetText ());
 output_pins[3] = atoi (((CCombo*) WProp->GetChildByName ("combo4"))->GetText ());
}


part_init("Potentiometers (Rotary)", cpart_pot_r, "Input");

