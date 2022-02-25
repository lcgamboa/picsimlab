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
#include"part_gamepad.h"

/* outputs */
enum
{
 O_P1, O_P2, O_P3, O_P4, O_P5, O_P6, O_P7, O_P8, O_B1, O_B2, O_B3, O_B4, O_B5, O_B6, O_J1
};

/* inputs */
enum
{
 I_B1, I_B2, I_B3, I_B4, I_B5, I_B6, I_J1
};

cpart_gamepad::cpart_gamepad(unsigned x, unsigned y)
: font(9, lxFONTFAMILY_TELETYPE, lxFONTSTYLE_NORMAL, lxFONTWEIGHT_BOLD)
{
 X = x;
 Y = y;
 ReadMaps ();

 LoadImage();

 output_pins[0] = 0;
 output_pins[1] = 0;
 output_pins[2] = 0;
 output_pins[3] = 0;
 output_pins[4] = 0;
 output_pins[5] = 0;
 output_pins[6] = 0;
 output_pins[7] = 0;

 output_value[0] = 1;
 output_value[1] = 1;
 output_value[2] = 1;
 output_value[3] = 1;
 output_value[4] = 1;
 output_value[5] = 1;

 active = 0;

 jr = 10;
 for (int i = 0; i < outputc; i++)
  {
   switch (output[i].id)
    {
    case O_J1:
     jr = ((output[i].x2 - output[i].x1) / 2) - 10;
     break;
    }
  }

 value[0] = jr;
 value[1] = jr;

 RegisterRemoteControl ();
}

void
cpart_gamepad::RegisterRemoteControl(void)
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
    case I_J1:
     input[i].status = value;
     break;
    }
  }
}

cpart_gamepad::~cpart_gamepad(void)
{
 delete Bitmap;
 canvas.Destroy ();
}

void
cpart_gamepad::Draw(void)
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
      case O_J1:
       canvas.SetColor (26, 26, 26);
       canvas.Circle (1, output[i].x1 + jr + 10, output[i].y1 + jr + 10, jr + 10);
       canvas.SetColor (51, 51, 51);
       canvas.Circle (1, output[i].x1 + jr + 10, output[i].y1 + jr + 10, jr - 4);
       canvas.SetColor (250, 250, 250);
       canvas.Circle (1, output[i].x1 + value[0] + 10, output[i].y1 + value[1] + 10, 8);
       break;
      case O_B5:
      case O_B6:
       canvas.SetColor (102, 102, 102);
       canvas.Circle (1, output[i].cx, output[i].cy, 10);
       if (output_value[output[i].id - O_B1])
        {
         canvas.SetColor (15, 15, 15);
        }
       else
        {
         canvas.SetColor (55, 55, 55);
        }
       canvas.Circle (1, output[i].cx, output[i].cy, 8);
       break;
      case O_B1:
      case O_B3:
       if (output_value[output[i].id - O_B1])
        {
         canvas.SetColor (244, 244, 0);
        }
       else
        {
         canvas.SetColor (0x9c, 0x94, 0x47);
        }
       canvas.Circle (1, output[i].cx, output[i].cy, 20);
       break;
      case O_B2:
      case O_B4:
       if (output_value[output[i].id - O_B1])
        {
         canvas.SetColor (0, 0, 214);
        }
       else
        {
         canvas.SetColor (64, 87, 106);
        }
       canvas.Circle (1, output[i].cx, output[i].cy, 20);
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
cpart_gamepad::PreProcess(void)
{

 Window5.SetPin (output_pins[0], output_value[0]);
 Window5.SetPin (output_pins[1], output_value[1]);
 Window5.SetPin (output_pins[2], output_value[2]);
 Window5.SetPin (output_pins[3], output_value[3]);
 Window5.SetPin (output_pins[4], output_value[4]);
 Window5.SetPin (output_pins[5], output_value[5]);
 Window5.SetAPin (output_pins[6], 2.5 * (value[0]) / jr);
 Window5.SetAPin (output_pins[7], 2.5 * (value[1]) / jr);
}

void
cpart_gamepad::EvMouseButtonPress(uint button, uint x, uint y, uint state)
{
 int i;

 for (i = 0; i < inputc; i++)
  {
   if (PointInside (x, y, input[i]))
    {

     switch (input[i].id)
      {
      case I_B1:
       output_value[0] = 0;
       output_ids[O_B1]->update = 1;
       break;
      case I_B2:
       output_value[1] = 0;
       output_ids[O_B2]->update = 1;
       break;
      case I_B3:
       output_value[2] = 0;
       output_ids[O_B3]->update = 1;
       break;
      case I_B4:
       output_value[3] = 0;
       output_ids[O_B4]->update = 1;
       break;
      case I_B5:
       output_value[4] = 0;
       output_ids[O_B5]->update = 1;
       break;
      case I_B6:
       output_value[5] = 0;
       output_ids[O_B6]->update = 1;
       break;
      case I_J1:
       RotateCoords (&x, &y);

       float cx = ((float) fabs (x - input[i].x1)) - jr;
       float cy = ((float) fabs (y - input[i].y1)) - jr;

       float module = sqrt (cx * cx + cy * cy);
       float angle = atan2 ((cy), (cx));

       if (module > jr)module = jr;
       value[0] = module * cos (angle) + jr;
       value[1] = module * sin (angle) + jr;

       active = 1;
       output_ids[O_J1]->update = 1;
       break;
      }
    }
  }
}

void
cpart_gamepad::EvMouseButtonRelease(uint button, uint x, uint y, uint state)
{
 int i;

 for (i = 0; i < inputc; i++)
  {
   if (PointInside (x, y, input[i]))
    {
     switch (input[i].id)
      {
      case I_B1:
       output_value[0] = 1;
       output_ids[O_B1]->update = 1;
       break;
      case I_B2:
       output_value[1] = 1;
       output_ids[O_B2]->update = 1;
       break;
      case I_B3:
       output_value[2] = 1;
       output_ids[O_B3]->update = 1;
       break;
      case I_B4:
       output_value[3] = 1;
       output_ids[O_B4]->update = 1;
       break;
      case I_B5:
       output_value[4] = 1;
       output_ids[O_B5]->update = 1;
       break;
      case I_B6:
       output_value[5] = 1;
       output_ids[O_B6]->update = 1;
       break;
      case I_J1: active = 0;
       value[0] = jr;
       value[1] = jr;
       output_ids[O_J1]->update = 1;
       break;
      }
    }
  }
}

void
cpart_gamepad::EvMouseMove(uint button, uint x, uint y, uint state)
{

 int i;

 for (i = 0; i < inputc; i++)
  {
   if (PointInside (x, y, input[i]))
    {
     switch (input[i].id)
      {
      case I_J1:
       if (active)
        {
         RotateCoords (&x, &y);

         float cx = ((float) fabs (x - input[i].x1)) - jr;
         float cy = ((float) fabs (y - input[i].y1)) - jr;

         float module = sqrt (cx * cx + cy * cy);
         float angle = atan2 ((cy), (cx));

         if (module > jr)module = jr;
         value[0] = module * cos (angle) + jr;
         value[1] = module * sin (angle) + jr;
         output_ids[O_J1]->update = 1;
        }
       break;
      }
    }
   /*
   else
    {
     if (input[i].id == I_J1)
      {
       active = 0;
      }
    }
    */ 
  }
}

void
cpart_gamepad::EvKeyPress(uint key, uint mask)
{

 switch (key)
  {
  case 'W':
  case 'w':
   value[0] = jr;
   value[1] = 0;
   if (!output_ids[O_J1]->value)
    {
     output_ids[O_J1]->value = 1;
     output_ids[O_J1]->update = 1;
    }
   break;
  case 'A':
  case 'a':
   value[0] = 0;
   value[1] = jr;
   if (!output_ids[O_J1]->value)
    {
     output_ids[O_J1]->value = 1;
     output_ids[O_J1]->update = 1;
    }
   break;
  case 'S':
  case 's':
   value[0] = jr;
   value[1] = 2 * jr;
   if (!output_ids[O_J1]->value)
    {
     output_ids[O_J1]->value = 1;
     output_ids[O_J1]->update = 1;
    }
   break;
  case 'D':
  case 'd':
   value[0] = 2 * jr;
   value[1] = jr;
   if (!output_ids[O_J1]->value)
    {
     output_ids[O_J1]->value = 1;
     output_ids[O_J1]->update = 1;
    }
   break;
  case 'I':
  case 'i':
   output_value[0] = 0;
   if (!output_ids[O_B1]->value)
    {
     output_ids[O_B1]->value = 1;
     output_ids[O_B1]->update = 1;
    }
   break;
  case 'J':
  case 'j':
   output_value[3] = 0;
   if (!output_ids[O_B4]->value)
    {
     output_ids[O_B4]->value = 1;
     output_ids[O_B4]->update = 1;
    }
   break;
  case 'K':
  case 'k':
   output_value[2] = 0;
   if (!output_ids[O_B3]->value)
    {
     output_ids[O_B3]->value = 1;
     output_ids[O_B3]->update = 1;
    }
   break;
  case 'L':
  case 'l':
   output_value[1] = 0;
   if (!output_ids[O_B2]->value)
    {
     output_ids[O_B2]->value = 1;
     output_ids[O_B2]->update = 1;
    }
   break;
  case 'E':
  case 'e':
   output_value[4] = 0;
   if (!output_ids[O_B5]->value)
    {
     output_ids[O_B5]->value = 1;
     output_ids[O_B5]->update = 1;
    }
   break;
  case 'O':
  case 'o':
   output_value[5] = 0;
   if (!output_ids[O_B6]->value)
    {
     output_ids[O_B6]->value = 1;
     output_ids[O_B6]->update = 1;
    }
   break;
  }
}

void
cpart_gamepad::EvKeyRelease(uint key, uint mask)
{
 switch (key)
  {
  case 'W':
  case 'A':
  case 'S':
  case 'D':
  case 'w':
  case 'a':
  case 's':
  case 'd':
   value[0] = jr;
   value[1] = jr;
   output_ids[O_J1]->update = 1;
   output_ids[O_J1]->value = 0;
   break;
  case 'I':
  case 'i':
   output_value[0] = 1;
   output_ids[O_B1]->update = 1;
   output_ids[O_B1]->value = 0;
   break;
  case 'J':
  case 'j':
   output_value[3] = 1;
   output_ids[O_B4]->update = 1;
   output_ids[O_B4]->value = 0;
   break;
  case 'K':
  case 'k':
   output_value[2] = 1;
   output_ids[O_B3]->update = 1;
   output_ids[O_B3]->value = 0;
   break;
  case 'L':
  case 'l':
   output_value[1] = 1;
   output_ids[O_B2]->update = 1;
   output_ids[O_B2]->value = 0;
   break;
  case 'E':
  case 'e':
   output_value[4] = 1;
   output_ids[O_B5]->update = 1;
   output_ids[O_B5]->value = 0;
   break;
  case 'O':
  case 'o':
   output_value[5] = 1;
   output_ids[O_B6]->update = 1;
   output_ids[O_B6]->value = 0;
   break;
  }
}

unsigned short
cpart_gamepad::get_in_id(char * name)
{
 if (strcmp (name, "PB_1") == 0)return I_B1;
 if (strcmp (name, "PB_2") == 0)return I_B2;
 if (strcmp (name, "PB_3") == 0)return I_B3;
 if (strcmp (name, "PB_4") == 0)return I_B4;
 if (strcmp (name, "PB_5") == 0)return I_B5;
 if (strcmp (name, "PB_6") == 0)return I_B6;

 if (strcmp (name, "AJ_1") == 0)return I_J1;

 printf ("Erro input '%s' don't have a valid id! \n", name);
 return -1;
};

unsigned short
cpart_gamepad::get_out_id(char * name)
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

 if (strcmp (name, "AJ_1") == 0)return O_J1;

 printf ("Erro output '%s' don't have a valid id! \n", name);
 return 1;
};

lxString
cpart_gamepad::WritePreferences(void)
{
 char prefs[256];

 sprintf (prefs, "%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu", output_pins[0], output_pins[1], output_pins[2], output_pins[3], output_pins[4], output_pins[5], output_pins[6], output_pins[7]);

 return prefs;
};

void
cpart_gamepad::ReadPreferences(lxString value)
{
 sscanf (value.c_str (), "%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu", &output_pins[0], &output_pins[1], &output_pins[2], &output_pins[3], &output_pins[4], &output_pins[5], &output_pins[6], &output_pins[7]);
}

void
cpart_gamepad::ConfigurePropertiesWindow(CPWindow * WProp)
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

 ((CButton*) WProp->GetChildByName ("button1"))->EvMouseButtonRelease = EVMOUSEBUTTONRELEASE & CPWindow5::PropButtonRelease;
 ((CButton*) WProp->GetChildByName ("button1"))->SetTag (1);

 ((CButton*) WProp->GetChildByName ("button2"))->EvMouseButtonRelease = EVMOUSEBUTTONRELEASE & CPWindow5::PropButtonRelease;
}

void
cpart_gamepad::ReadPropertiesWindow(CPWindow * WProp)
{
 output_pins[0] = atoi (((CCombo*) WProp->GetChildByName ("combo1"))->GetText ());
 output_pins[1] = atoi (((CCombo*) WProp->GetChildByName ("combo2"))->GetText ());
 output_pins[2] = atoi (((CCombo*) WProp->GetChildByName ("combo3"))->GetText ());
 output_pins[3] = atoi (((CCombo*) WProp->GetChildByName ("combo4"))->GetText ());
 output_pins[4] = atoi (((CCombo*) WProp->GetChildByName ("combo5"))->GetText ());
 output_pins[5] = atoi (((CCombo*) WProp->GetChildByName ("combo6"))->GetText ());
 output_pins[6] = atoi (((CCombo*) WProp->GetChildByName ("combo7"))->GetText ());
 output_pins[7] = atoi (((CCombo*) WProp->GetChildByName ("combo8"))->GetText ());
}

part_init(PART_GAMEPAD_Name, cpart_gamepad, "Input");

