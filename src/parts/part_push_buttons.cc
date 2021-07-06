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
#include"part_push_buttons.h"

/* outputs */
enum
{
 O_P1, O_P2, O_P3, O_P4, O_P5, O_P6, O_P7, O_P8, O_B1, O_B2, O_B3, O_B4, O_B5, O_B6, O_B7, O_B8, O_J1
};

/* inputs */
enum
{
 I_B1, I_B2, I_B3, I_B4, I_B5, I_B6, I_B7, I_B8, I_J1
};

cpart_pbuttons::cpart_pbuttons(unsigned x, unsigned y) :
font(9, lxFONTFAMILY_TELETYPE, lxFONTSTYLE_NORMAL, lxFONTWEIGHT_BOLD)
{
 X = x;
 Y = y;
 active = 1;

 ReadMaps ();

 lxImage image (&Window5);
 image.LoadFile (lxGetLocalFile(Window1.GetSharePath () + lxT ("parts/") + GetPictureFileName ()), Orientation, Scale, Scale);

 Bitmap = new lxBitmap (&image, &Window5);
 image.Destroy ();

 canvas.Create (Window5.GetWWidget (), Bitmap);



 output_pins[0] = 0;
 output_pins[1] = 0;
 output_pins[2] = 0;
 output_pins[3] = 0;
 output_pins[4] = 0;
 output_pins[5] = 0;
 output_pins[6] = 0;
 output_pins[7] = 0;

 output_value[0] = !active;
 output_value[1] = !active;
 output_value[2] = !active;
 output_value[3] = !active;
 output_value[4] = !active;
 output_value[5] = !active;
 output_value[6] = !active;
 output_value[7] = !active;

 RegisterRemoteControl ();

}

void
cpart_pbuttons::RegisterRemoteControl(void)
{
 for (int i = 0; i < inputc; i++)
  {
   switch (input[i].id)
    {
    case I_B1:
     input[i].status = &output_value[0];
     break;
    case I_B2:
     input[i].status = &output_value[1];
     break;
    case I_B3:
     input[i].status = &output_value[2];
     break;
    case I_B4:
     input[i].status = &output_value[3];
     break;
    case I_B5:
     input[i].status = &output_value[4];
     break;
    case I_B6:
     input[i].status = &output_value[5];
     break;
    case I_B7:
     input[i].status = &output_value[6];
     break;
    case I_B8:
     input[i].status = &output_value[7];
     break;
    }
  }
}

void
cpart_pbuttons::Reset(void)
{
 //release all
 output_value[0] = !active;
 output_value[1] = !active;
 output_value[2] = !active;
 output_value[3] = !active;
 output_value[4] = !active;
 output_value[5] = !active;
 output_value[6] = !active;
 output_value[7] = !active;

 //force pin update
 Window5.SetPin (output_pins[0], !output_value[0]);
 Window5.SetPin (output_pins[1], !output_value[1]);
 Window5.SetPin (output_pins[2], !output_value[2]);
 Window5.SetPin (output_pins[3], !output_value[3]);
 Window5.SetPin (output_pins[4], !output_value[4]);
 Window5.SetPin (output_pins[5], !output_value[5]);
 Window5.SetPin (output_pins[6], !output_value[6]);
 Window5.SetPin (output_pins[7], !output_value[7]);

 Window5.SetPin (output_pins[0], output_value[0]);
 Window5.SetPin (output_pins[1], output_value[1]);
 Window5.SetPin (output_pins[2], output_value[2]);
 Window5.SetPin (output_pins[3], output_value[3]);
 Window5.SetPin (output_pins[4], output_value[4]);
 Window5.SetPin (output_pins[5], output_value[5]);
 Window5.SetPin (output_pins[6], output_value[6]);
 Window5.SetPin (output_pins[7], output_value[7]);

}

cpart_pbuttons::~cpart_pbuttons(void)
{
 delete Bitmap;
 canvas.Destroy ();
}

void
cpart_pbuttons::Draw(void)
{

 int i;

 Update = 0;

 for (i = 0; i < outputc; i++)
  {
   if (output[i].update)//only if need update
    {
     output[i].update = 0;

     if (!Update)
      {
       canvas.Init (Scale, Scale, Orientation);
       canvas.SetFont (font);
      }
     Update++; //set to update buffer

     switch (output[i].id)
      {
      case O_P1:
      case O_P2:
      case O_P3:
      case O_P4:
      case O_P5:
      case O_P6:
      case O_P7:
      case O_P8:
       canvas.SetColor (49, 61, 99);
       canvas.Rectangle (1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);
       canvas.SetFgColor (255, 255, 255);
       if (output_pins[output[i].id - O_P1] == 0)
        canvas.RotatedText ("NC", output[i].x1, output[i].y1, 0);
       else
        canvas.RotatedText (Window5.GetPinName (output_pins[output[i].id - O_P1]), output[i].x1, output[i].y1, 0);
       break;
      case O_B1:
      case O_B2:
      case O_B3:
      case O_B4:
      case O_B5:
      case O_B6:
      case O_B7:
      case O_B8:
       canvas.SetColor (100, 100, 100);
       canvas.Circle (1, output[i].cx, output[i].cy, 10);
       if (output_value[output[i].id - O_B1] == active)
        {
         canvas.SetColor (55, 55, 55);
        }
       else
        {
         canvas.SetColor (15, 15, 15);
        }
       canvas.Circle (1, output[i].cx, output[i].cy, 8);
       break;
      }
    }
  }

 if (Update)
  {
   canvas.End ();
  }
}

void
cpart_pbuttons::PreProcess(void)
{


 Window5.SetPin (output_pins[0], output_value[0]);
 Window5.SetPin (output_pins[1], output_value[1]);
 Window5.SetPin (output_pins[2], output_value[2]);
 Window5.SetPin (output_pins[3], output_value[3]);
 Window5.SetPin (output_pins[4], output_value[4]);
 Window5.SetPin (output_pins[5], output_value[5]);
 Window5.SetPin (output_pins[6], output_value[6]);
 Window5.SetPin (output_pins[7], output_value[7]);

}

void
cpart_pbuttons::EvMouseButtonPress(uint button, uint x, uint y, uint state)
{
 int i;

 for (i = 0; i < inputc; i++)
  {
   if (PointInside (x, y, input[i]))
    {

     switch (input[i].id)
      {
      case I_B1:
       output_value[0] = active;
       output_ids[O_B1]->update = 1;
       break;
      case I_B2:
       output_value[1] = active;
       output_ids[O_B2]->update = 1;
       break;
      case I_B3:
       output_value[2] = active;
       output_ids[O_B3]->update = 1;
       break;
      case I_B4:
       output_value[3] = active;
       output_ids[O_B4]->update = 1;
       break;
      case I_B5:
       output_value[4] = active;
       output_ids[O_B5]->update = 1;
       break;
      case I_B6:
       output_value[5] = active;
       output_ids[O_B6]->update = 1;
       break;
      case I_B7:
       output_value[6] = active;
       output_ids[O_B7]->update = 1;
       break;
      case I_B8:
       output_value[7] = active;
       output_ids[O_B8]->update = 1;
       break;
      }
    }
  }
}

void
cpart_pbuttons::EvMouseButtonRelease(uint button, uint x, uint y, uint state)
{
 int i;

 for (i = 0; i < inputc; i++)
  {
   if (PointInside (x, y, input[i]))
    {
     switch (input[i].id)
      {
      case I_B1:
       output_value[0] = !active;
       output_ids[O_B1]->update = 1;
       break;
      case I_B2:
       output_value[1] = !active;
       output_ids[O_B2]->update = 1;
       break;
      case I_B3:
       output_value[2] = !active;
       output_ids[O_B3]->update = 1;
       break;
      case I_B4:
       output_value[3] = !active;
       output_ids[O_B4]->update = 1;
       break;
      case I_B5:
       output_value[4] = !active;
       output_ids[O_B5]->update = 1;
       break;
      case I_B6:
       output_value[5] = !active;
       output_ids[O_B6]->update = 1;
       break;
      case I_B7:
       output_value[6] = !active;
       output_ids[O_B7]->update = 1;
       break;
      case I_B8:
       output_value[7] = !active;
       output_ids[O_B8]->update = 1;
       break;
      }
    }
  }
}

unsigned short
cpart_pbuttons::get_in_id(char * name)
{
 if (strcmp (name, "PB_1") == 0)return I_B1;
 if (strcmp (name, "PB_2") == 0)return I_B2;
 if (strcmp (name, "PB_3") == 0)return I_B3;
 if (strcmp (name, "PB_4") == 0)return I_B4;
 if (strcmp (name, "PB_5") == 0)return I_B5;
 if (strcmp (name, "PB_6") == 0)return I_B6;
 if (strcmp (name, "PB_7") == 0)return I_B7;
 if (strcmp (name, "PB_8") == 0)return I_B8;

 if (strcmp (name, "JP_1") == 0)return I_J1;

 printf ("Erro input '%s' don't have a valid id! \n", name);
 return -1;
}

unsigned short
cpart_pbuttons::get_out_id(char * name)
{

 if (strcmp (name, "PN_1") == 0)return O_P1;
 if (strcmp (name, "PN_2") == 0)return O_P2;
 if (strcmp (name, "PN_3") == 0)return O_P3;
 if (strcmp (name, "PN_4") == 0)return O_P4;
 if (strcmp (name, "PN_5") == 0)return O_P5;
 if (strcmp (name, "PN_6") == 0)return O_P6;
 if (strcmp (name, "PN_7") == 0)return O_P7;
 if (strcmp (name, "PN_8") == 0)return O_P8;

 if (strcmp (name, "PB_1") == 0)return O_B1;
 if (strcmp (name, "PB_2") == 0)return O_B2;
 if (strcmp (name, "PB_3") == 0)return O_B3;
 if (strcmp (name, "PB_4") == 0)return O_B4;
 if (strcmp (name, "PB_5") == 0)return O_B5;
 if (strcmp (name, "PB_6") == 0)return O_B6;
 if (strcmp (name, "PB_7") == 0)return O_B7;
 if (strcmp (name, "PB_8") == 0)return O_B8;

 if (strcmp (name, "JP_1") == 0)return O_J1;

 printf ("Erro output '%s' don't have a valid id! \n", name);
 return 1;
}

lxString
cpart_pbuttons::WritePreferences(void)
{
 char prefs[256];

 sprintf (prefs, "%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu", output_pins[0], output_pins[1], output_pins[2], output_pins[3], output_pins[4], output_pins[5], output_pins[6], output_pins[7], active);

 return prefs;
}

void
cpart_pbuttons::ReadPreferences(lxString value)
{
 sscanf (value.c_str (), "%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu", &output_pins[0], &output_pins[1], &output_pins[2], &output_pins[3], &output_pins[4], &output_pins[5], &output_pins[6], &output_pins[7], &active);
 output_value[0] = !active;
 output_value[1] = !active;
 output_value[2] = !active;
 output_value[3] = !active;
 output_value[4] = !active;
 output_value[5] = !active;
 output_value[6] = !active;
 output_value[7] = !active;
}

void
cpart_pbuttons::ConfigurePropertiesWindow(CPWindow * WProp)
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

 ((CCombo*) WProp->GetChildByName ("combo5"))->SetItems (Items);
 if (output_pins[4] == 0)
  ((CCombo*) WProp->GetChildByName ("combo5"))->SetText ("0  NC");
 else
  {
   spin = Window5.GetPinName (output_pins[4]);
   ((CCombo*) WProp->GetChildByName ("combo5"))->SetText (itoa (output_pins[4]) + "  " + spin);
  }

 ((CCombo*) WProp->GetChildByName ("combo6"))->SetItems (Items);
 if (output_pins[5] == 0)
  ((CCombo*) WProp->GetChildByName ("combo6"))->SetText ("0  NC");
 else
  {
   spin = Window5.GetPinName (output_pins[5]);
   ((CCombo*) WProp->GetChildByName ("combo6"))->SetText (itoa (output_pins[5]) + "  " + spin);
  }

 ((CCombo*) WProp->GetChildByName ("combo7"))->SetItems (Items);
 if (output_pins[6] == 0)
  ((CCombo*) WProp->GetChildByName ("combo7"))->SetText ("0  NC");
 else
  {
   spin = Window5.GetPinName (output_pins[6]);
   ((CCombo*) WProp->GetChildByName ("combo7"))->SetText (itoa (output_pins[6]) + "  " + spin);
  }

 ((CCombo*) WProp->GetChildByName ("combo8"))->SetItems (Items);
 if (output_pins[7] == 0)
  ((CCombo*) WProp->GetChildByName ("combo8"))->SetText ("0  NC");
 else
  {
   spin = Window5.GetPinName (output_pins[7]);
   ((CCombo*) WProp->GetChildByName ("combo8"))->SetText (itoa (output_pins[7]) + "  " + spin);
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
cpart_pbuttons::ReadPropertiesWindow(CPWindow * WProp)
{
 output_pins[0] = atoi (((CCombo*) WProp->GetChildByName ("combo1"))->GetText ());
 output_pins[1] = atoi (((CCombo*) WProp->GetChildByName ("combo2"))->GetText ());
 output_pins[2] = atoi (((CCombo*) WProp->GetChildByName ("combo3"))->GetText ());
 output_pins[3] = atoi (((CCombo*) WProp->GetChildByName ("combo4"))->GetText ());
 output_pins[4] = atoi (((CCombo*) WProp->GetChildByName ("combo5"))->GetText ());
 output_pins[5] = atoi (((CCombo*) WProp->GetChildByName ("combo6"))->GetText ());
 output_pins[6] = atoi (((CCombo*) WProp->GetChildByName ("combo7"))->GetText ());
 output_pins[7] = atoi (((CCombo*) WProp->GetChildByName ("combo8"))->GetText ());

 active = (((CCombo*) WProp->GetChildByName ("combo9"))->GetText ().compare ("HIGH") == 0);

 output_value[0] = !active;
 output_value[1] = !active;
 output_value[2] = !active;
 output_value[3] = !active;
 output_value[4] = !active;
 output_value[5] = !active;
 output_value[6] = !active;
 output_value[7] = !active;
}


part_init(PART_PUSH_BUTTONS_Name, cpart_pbuttons, "Input");

