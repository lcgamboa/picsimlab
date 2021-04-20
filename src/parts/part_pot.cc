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
#include"part_pot.h"

/* outputs */
enum
{
 O_P1, O_P2, O_P3, O_P4, O_PO1, O_PO2, O_PO3, O_PO4
};

/* inputs */
enum
{
 I_PO1, I_PO2, I_PO3, I_PO4
};

cpart_pot::cpart_pot(unsigned x, unsigned y):
font (9, lxFONTFAMILY_TELETYPE, lxFONTSTYLE_NORMAL, lxFONTWEIGHT_BOLD),
font_p (7, lxFONTFAMILY_TELETYPE, lxFONTSTYLE_NORMAL, lxFONTWEIGHT_BOLD)
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
cpart_pot::RegisterRemoteControl(void)
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

cpart_pot::~cpart_pot(void)
{
 delete Bitmap;
 canvas.Destroy ();
}

void
cpart_pot::Draw(void)
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
     canvas.SetColor (179, 179, 179);
     canvas.Rectangle (1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);
     canvas.SetFgColor (0, 0, 0);
     canvas.SetBgColor (96, 96, 96);
     canvas.Rectangle (1, output[i].x1 + 9, output[i].y1 + 9, output[i].x2 - output[i].x1 - 18, output[i].y2 - output[i].y1 - 18);
     canvas.SetBgColor (46, 46, 46);
     canvas.Rectangle (1, output[i].x1, output[i].y1 + values[output[i].id - O_PO1] / 1.66, 32, 20);
     snprintf (val, 10, "%4.2f", 5.0 * (200 - values[output[i].id - O_PO1]) / 200.0);
     canvas.SetColor (250, 250, 250);
     canvas.SetFont (font_p);
     canvas.RotatedText (val, output[i].x1 + 4, output[i].y1 + 5 + values[output[i].id - O_PO1] / 1.66, 0);
     canvas.SetFont (font);
    }
  }

 canvas.End ();

}

void
cpart_pot::PreProcess(void)
{
 Window5.SetAPin (output_pins[0], 5.0 * (200 - values[0]) / 200.0);
 Window5.SetAPin (output_pins[1], 5.0 * (200 - values[1]) / 200.0);
 Window5.SetAPin (output_pins[2], 5.0 * (200 - values[2]) / 200.0);
 Window5.SetAPin (output_pins[3], 5.0 * (200 - values[3]) / 200.0);
}

void
cpart_pot::EvMouseButtonPress(uint button, uint x, uint y, uint state)
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
       values[0] = (y - input[i].y1)*1.66;
       if (values[0] > 200)values[0] = 200;
       active[0] = 1;
       break;
      case I_PO2:
       values[1] = (y - input[i].y1)*1.66;
       if (values[1] > 200)values[1] = 200;
       active[1] = 1;
       break;
      case I_PO3:
       values[2] = (y - input[i].y1)*1.66;
       if (values[2] > 200)values[2] = 200;
       active[2] = 1;
       break;
      case I_PO4:
       values[3] = (y - input[i].y1)*1.66;
       if (values[3] > 200)values[3] = 200;
       active[3] = 1;
       break;
      }
    }
  }

}

void
cpart_pot::EvMouseButtonRelease(uint button, uint x, uint y, uint state)
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
cpart_pot::EvMouseMove(uint button, uint x, uint y, uint state)
{

 int i;

 for (i = 0; i < inputc; i++)
  {
   if (PointInside (x, y, input[i]))
    {
     RotateCoords (&x, &y);

     if (active[input[i].id - I_PO1])
      {
       values[input[i].id - I_PO1] = (y - input[i].y1)*1.66;
       if (values[input[i].id - I_PO1] > 200)values[input[i].id - I_PO1] = 200;
      }
    }
  }
}

unsigned short
cpart_pot::get_in_id(char * name)
{

 if (strcmp (name, "PO_1") == 0)return I_PO1;
 if (strcmp (name, "PO_2") == 0)return I_PO2;
 if (strcmp (name, "PO_3") == 0)return I_PO3;
 if (strcmp (name, "PO_4") == 0)return I_PO4;

 printf ("Erro input '%s' don't have a valid id! \n", name);
 return -1;
}

unsigned short
cpart_pot::get_out_id(char * name)
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
cpart_pot::WritePreferences(void)
{
 char prefs[256];

 sprintf (prefs, "%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu", output_pins[0], output_pins[1], output_pins[2], output_pins[3], values[0], values[1], values[2], values[3]);

 return prefs;
}

void
cpart_pot::ReadPreferences(lxString value)
{
 sscanf (value.c_str (), "%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu", &output_pins[0], &output_pins[1], &output_pins[2], &output_pins[3], &values[0], &values[1], &values[2], &values[3]);
}

void
cpart_pot::ConfigurePropertiesWindow(CPWindow * WProp)
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
cpart_pot::ReadPropertiesWindow(CPWindow * WProp)
{
 output_pins[0] = atoi (((CCombo*) WProp->GetChildByName ("combo1"))->GetText ());
 output_pins[1] = atoi (((CCombo*) WProp->GetChildByName ("combo2"))->GetText ());
 output_pins[2] = atoi (((CCombo*) WProp->GetChildByName ("combo3"))->GetText ());
 output_pins[3] = atoi (((CCombo*) WProp->GetChildByName ("combo4"))->GetText ());
}


part_init("Potentiometers", cpart_pot, "Input");

