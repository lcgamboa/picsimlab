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

#include"../picsimlab1.h"
#include"../picsimlab4.h"
#include"../picsimlab5.h"
#include"part_LEDs.h"

/* outputs */
enum
{
 O_P1, O_P2, O_P3, O_P4, O_P5, O_P6, O_P7, O_P8, O_L1, O_L2, O_L3, O_L4, O_L5, O_L6, O_L7, O_L8
};

cpart_leds::cpart_leds(unsigned x, unsigned y) :
font(9, lxFONTFAMILY_TELETYPE, lxFONTSTYLE_NORMAL, lxFONTWEIGHT_BOLD)
{
 X = x;
 Y = y;
 active = 1;

 ReadMaps ();

 LoadImage();

 input_pins[0] = 0;
 input_pins[1] = 0;
 input_pins[2] = 0;
 input_pins[3] = 0;
 input_pins[4] = 0;
 input_pins[5] = 0;
 input_pins[6] = 0;
 input_pins[7] = 0;

};

cpart_leds::~cpart_leds(void)
{
 delete Bitmap;
 canvas.Destroy ();
}

void
cpart_leds::Draw(void)
{

 int i;

 const picpin * ppins = Window5.GetPinsValues ();

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
       if (input_pins[output[i].id - O_P1] == 0)
        canvas.RotatedText ("NC", output[i].x1, output[i].y1, 0);
       else
        canvas.RotatedText (Window5.GetPinName (input_pins[output[i].id - O_P1]), output[i].x1, output[i].y1, 0);
       break;
      case O_L1:
      case O_L2:
      case O_L3:
      case O_L4:
      case O_L5:
      case O_L6:
      case O_L7:
      case O_L8:
       if (input_pins[output[i].id - O_L1] > 0)
        {
         if (active)
          {
           canvas.SetBgColor (ppins[input_pins[output[i].id - O_L1] - 1].oavalue, 0, 0);
          }
         else
          {
           canvas.SetBgColor (310 - ppins[input_pins[output[i].id - O_L1] - 1].oavalue, 0, 0);
          }
        }
       else
        {
         canvas.SetBgColor (55, 0, 0);
        }
       canvas.SetFgColor (0, 0, 0);
       //draw a circle
       color1 = canvas.GetBgColor ();
       int r = color1.Red () - 120;
       int g = color1.Green () - 120;
       int b = color1.Blue () - 120;
       if (r < 0)r = 0;
       if (g < 0)g = 0;
       if (b < 0)b = 0;
       color2.Set (r, g, b);
       canvas.SetBgColor (color2);
       canvas.Circle (1, output[i].x1, output[i].y1, output[i].r);
       canvas.SetBgColor (color1);
       canvas.Circle (1, output[i].x1, output[i].y1, output[i].r - 3);
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
cpart_leds::PostProcess(void)
{

 const picpin * ppins = Window5.GetPinsValues ();

 for (int i = 0; i < 8; i++)
  {
   if (input_pins[i] && (output_ids[O_L1 + i]->value != ppins[input_pins[i] - 1].oavalue))
    {
     output_ids[O_L1 + i]->value = ppins[input_pins[i] - 1].oavalue;
     output_ids[O_L1 + i]->update = 1;
    }
  }
}

unsigned short
cpart_leds::get_in_id(char * name)
{
 printf ("Erro input '%s' don't have a valid id! \n", name);
 return -1;
};

unsigned short
cpart_leds::get_out_id(char * name)
{

 if (strcmp (name, "PN_1") == 0)return O_P1;
 if (strcmp (name, "PN_2") == 0)return O_P2;
 if (strcmp (name, "PN_3") == 0)return O_P3;
 if (strcmp (name, "PN_4") == 0)return O_P4;
 if (strcmp (name, "PN_5") == 0)return O_P5;
 if (strcmp (name, "PN_6") == 0)return O_P6;
 if (strcmp (name, "PN_7") == 0)return O_P7;
 if (strcmp (name, "PN_8") == 0)return O_P8;

 if (strcmp (name, "LD_1") == 0)return O_L1;
 if (strcmp (name, "LD_2") == 0)return O_L2;
 if (strcmp (name, "LD_3") == 0)return O_L3;
 if (strcmp (name, "LD_4") == 0)return O_L4;
 if (strcmp (name, "LD_5") == 0)return O_L5;
 if (strcmp (name, "LD_6") == 0)return O_L6;
 if (strcmp (name, "LD_7") == 0)return O_L7;
 if (strcmp (name, "LD_8") == 0)return O_L8;

 printf ("Erro output '%s' don't have a valid id! \n", name);
 return 1;
};

lxString
cpart_leds::WritePreferences(void)
{
 char prefs[256];

 sprintf (prefs, "%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu", input_pins[0], input_pins[1], input_pins[2], input_pins[3], input_pins[4], input_pins[5], input_pins[6], input_pins[7], active);

 return prefs;
}

void
cpart_leds::ReadPreferences(lxString value)
{
 sscanf (value.c_str (), "%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu", &input_pins[0], &input_pins[1], &input_pins[2], &input_pins[3], &input_pins[4], &input_pins[5], &input_pins[6], &input_pins[7], &active);

 RegisterRemoteControl ();
}

void
cpart_leds::RegisterRemoteControl(void)
{
 const picpin * ppins = Window5.GetPinsValues ();
 for (int i = 0; i < outputc; i++)
  {
   switch (output[i].id)
    {
    case O_L1:
     if (input_pins[0])
      {
       output[i].status = (void *) &ppins[input_pins[0] - 1].oavalue;
      }
     break;
    case O_L2:
     if (input_pins[1])
      {
       output[i].status = (void *) &ppins[input_pins[1] - 1].oavalue;
      }
     break;
    case O_L3:
     if (input_pins[2])
      {
       output[i].status = (void *) &ppins[input_pins[2] - 1].oavalue;
      }
     break;
    case O_L4:
     if (input_pins[3])
      {
       output[i].status = (void *) &ppins[input_pins[3] - 1].oavalue;
      }
     break;
    case O_L5:
     if (input_pins[4])
      {
       output[i].status = (void *) &ppins[input_pins[4] - 1].oavalue;
      }
     break;
    case O_L6:
     if (input_pins[5])
      {
       output[i].status = (void *) &ppins[input_pins[5] - 1].oavalue;
      }
     break;
    case O_L7:
     if (input_pins[6])
      {
       output[i].status = (void *) &ppins[input_pins[6] - 1].oavalue;
      }
     break;
    case O_L8:
     if (input_pins[7])
      {
       output[i].status = (void *) &ppins[input_pins[7] - 1].oavalue;
      }
     break;
    }
  }
}

void
cpart_leds::ConfigurePropertiesWindow(CPWindow * WProp)
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

 ((CCombo*) WProp->GetChildByName ("combo4"))->SetItems (Items);
 if (input_pins[3] == 0)
  ((CCombo*) WProp->GetChildByName ("combo4"))->SetText ("0  NC");
 else
  {
   spin = Window5.GetPinName (input_pins[3]);
   ((CCombo*) WProp->GetChildByName ("combo4"))->SetText (itoa (input_pins[3]) + "  " + spin);
  }

 ((CCombo*) WProp->GetChildByName ("combo5"))->SetItems (Items);
 if (input_pins[4] == 0)
  ((CCombo*) WProp->GetChildByName ("combo5"))->SetText ("0  NC");
 else
  {
   spin = Window5.GetPinName (input_pins[4]);
   ((CCombo*) WProp->GetChildByName ("combo5"))->SetText (itoa (input_pins[4]) + "  " + spin);
  }

 ((CCombo*) WProp->GetChildByName ("combo6"))->SetItems (Items);
 if (input_pins[5] == 0)
  ((CCombo*) WProp->GetChildByName ("combo6"))->SetText ("0  NC");
 else
  {
   spin = Window5.GetPinName (input_pins[5]);
   ((CCombo*) WProp->GetChildByName ("combo6"))->SetText (itoa (input_pins[5]) + "  " + spin);
  }

 ((CCombo*) WProp->GetChildByName ("combo7"))->SetItems (Items);
 if (input_pins[6] == 0)
  ((CCombo*) WProp->GetChildByName ("combo7"))->SetText ("0  NC");
 else
  {
   spin = Window5.GetPinName (input_pins[6]);
   ((CCombo*) WProp->GetChildByName ("combo7"))->SetText (itoa (input_pins[6]) + "  " + spin);
  }

 ((CCombo*) WProp->GetChildByName ("combo8"))->SetItems (Items);
 if (input_pins[7] == 0)
  ((CCombo*) WProp->GetChildByName ("combo8"))->SetText ("0  NC");
 else
  {
   spin = Window5.GetPinName (input_pins[7]);
   ((CCombo*) WProp->GetChildByName ("combo8"))->SetText (itoa (input_pins[7]) + "  " + spin);
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
cpart_leds::ReadPropertiesWindow(CPWindow * WProp)
{
 input_pins[0] = atoi (((CCombo*) WProp->GetChildByName ("combo1"))->GetText ());
 input_pins[1] = atoi (((CCombo*) WProp->GetChildByName ("combo2"))->GetText ());
 input_pins[2] = atoi (((CCombo*) WProp->GetChildByName ("combo3"))->GetText ());
 input_pins[3] = atoi (((CCombo*) WProp->GetChildByName ("combo4"))->GetText ());
 input_pins[4] = atoi (((CCombo*) WProp->GetChildByName ("combo5"))->GetText ());
 input_pins[5] = atoi (((CCombo*) WProp->GetChildByName ("combo6"))->GetText ());
 input_pins[6] = atoi (((CCombo*) WProp->GetChildByName ("combo7"))->GetText ());
 input_pins[7] = atoi (((CCombo*) WProp->GetChildByName ("combo8"))->GetText ());

 active = (((CCombo*) WProp->GetChildByName ("combo9"))->GetText ().compare ("HIGH") == 0);

 RegisterRemoteControl ();
}


part_init(PART_LEDS_Name, cpart_leds, "Output");

