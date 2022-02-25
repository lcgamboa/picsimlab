/* ########################################################################

   PICsimLab - PIC laboratory simulator

   ########################################################################

   Copyright (c) : 2019-2022  Luis Claudio Gambôa Lopes

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
#include"part_keypad.h"

/* outputs */
enum
{
 O_K1, O_K2, O_K3, O_KA, O_K4, O_K5, O_K6, O_KB, O_K7, O_K8, O_K9, O_KC, O_Ka, O_K0, O_KT, O_KD,
 O_L1, O_L2, O_L3, O_L4, O_C1, O_C2, O_C3, O_C4, O_C5
};

/* inputs */
enum
{
 I_K1, I_K2, I_K3, I_KA, I_K4, I_K5, I_K6, I_KB, I_K7, I_K8, I_K9, I_KC, I_Ka, I_K0, I_KT, I_KD
};

/* types */
enum
{
 KT4x4 = 1, KT4x3, KT2x5
};

lxString
cpart_keypad::GetPictureFileName(void)
{
 switch (type)
  {
  case KT4x4:
   return lxT ("Keypad/keypad_4x4.svg");
   break;
  case KT4x3:
   return lxT ("Keypad/keypad_4x3.svg");
   break;
  case KT2x5:
   return lxT ("Keypad/keypad_2x5.svg");
   break;
  }
 return lxT ("Keypad/keypad_4x4.svg");
}


lxString
cpart_keypad::GetMapFile(void)
{
 switch (type)
  {
  case KT4x4:
   return lxT ("Keypad/keypad_4x4.map");
   break;
  case KT4x3:
   return lxT ("Keypad/keypad_4x3.map");
   break;
  case KT2x5:
   return lxT ("Keypad/keypad_2x5.map");
   break;
  }
 return lxT ("Keypad/keypad_4x4.map");
}

cpart_keypad::cpart_keypad(unsigned x, unsigned y) :
font(9, lxFONTFAMILY_TELETYPE, lxFONTSTYLE_NORMAL, lxFONTWEIGHT_BOLD)
{
 X = x;
 Y = y;
 aways_update = 1;
 pull = 0;
 type = 0;

 output_pins[0] = 0;
 output_pins[1] = 0;
 output_pins[2] = 0;
 output_pins[3] = 0;
 output_pins[4] = 0;
 output_pins[5] = 0;
 output_pins[6] = 0;
 output_pins[7] = 0;

 memset (keys, 0, 16);
 memset (keys2, 0, 10);

 refresh = 0;

 Bitmap = NULL;

 ChangeType (KT4x4);

 RegisterRemoteControl ();
}

void
cpart_keypad::RegisterRemoteControl(void)
{

 if ((type == KT4x3) || (type == KT4x4))
  {
   for (int i = 0; i < inputc; i++)
    {
     switch (input[i].id)
      {
      case I_K1:
       input[i].status = &keys[0][0];
       break;
      case I_K2:
       input[i].status = &keys[0][1];
       break;
      case I_K3:
       input[i].status = &keys[0][2];
       break;
      case I_K4:
       input[i].status = &keys[1][0];
       break;
      case I_K5:
       input[i].status = &keys[1][1];
       break;
      case I_K6:
       input[i].status = &keys[1][2];
       break;
      case I_K7:
       input[i].status = &keys[2][0];
       break;
      case I_K8:
       input[i].status = &keys[2][1];
       break;
      case I_K9:
       input[i].status = &keys[2][2];
       break;
      case I_Ka:
       input[i].status = &keys[3][0];
       break;
      case I_K0:
       input[i].status = &keys[3][1];
       break;
      case I_KT:
       input[i].status = &keys[3][2];
       break;
       //4x4 
      case I_KA:
       input[i].status = &keys[0][3];
       break;
      case I_KB:
       input[i].status = &keys[1][3];
       break;
      case I_KC:
       input[i].status = &keys[2][3];
       break;
      case I_KD:
       input[i].status = &keys[3][3];
       break;
      }
    }
  }
 else if (type == KT2x5)
  {
   for (int i = 0; i < inputc; i++)
    {
     switch (input[i].id)
      {
      case I_K1:
       input[i].status = &keys2[0][0];
       break;
      case I_K2:
       input[i].status = &keys2[0][1];
       break;
      case I_K3:
       input[i].status = &keys2[0][2];
       break;
      case I_K4:
       input[i].status = &keys2[0][3];
       break;
      case I_K5:
       input[i].status = &keys2[0][4];
       break;
      case I_K6:
       input[i].status = &keys2[1][0];
       break;
      case I_K7:
       input[i].status = &keys2[1][1];
       break;
      case I_K8:
       input[i].status = &keys2[1][2];
       break;
      case I_K9:
       input[i].status = &keys2[1][3];
       break;
      case I_K0:
       input[i].status = &keys2[1][4];
       break;
      }
    }
  }

}

cpart_keypad::~cpart_keypad(void)
{
 delete Bitmap;
 Bitmap = NULL;
 canvas.Destroy ();
}

void
cpart_keypad::ChangeType(unsigned char tp)
{


 if (!tp)tp = KT4x4;
 if (tp > KT2x5)tp = KT4x4;

 //if same
 if (tp == type) return;

 if (Bitmap)
  {
   delete Bitmap;
  }

 type = tp;

 ReadMaps ();

 LoadImage();

}

void
cpart_keypad::Draw(void)
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
      case O_L1:
      case O_L2:
      case O_L3:
      case O_L4:
      case O_C1:
      case O_C2:
      case O_C3:
      case O_C4:
      case O_C5:
      {
       canvas.SetColor (49, 61, 99);
       canvas.Rectangle (1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);
       canvas.SetFgColor (255, 255, 255);

       int id = output[i].id - O_L1;
       if ((type == KT2x5)&&(id > 1))
        {
         id -= 2;
        }
       if (output_pins[id] == 0)
        canvas.RotatedText ("NC", output[i].x1, output[i].y2, 90.0);
       else
        canvas.RotatedText (Window5.GetPinName (output_pins[id]), output[i].x1, output[i].y2, 90.0);
       }
       break;
       case O_K1 ... O_KD:
         if(output[i].value)
         {
           canvas.SetLineWidth(4);
           canvas.SetColor (255, 255, 0);
         }
         else
         {
          canvas.SetLineWidth(6);
          if((output[i].id == O_Ka)||(output[i].id == O_KT)|| !((output[i].id + 1) % 4 ))
          {
           canvas.SetColor (190, 46, 37);    
          } 
          else
          {
            canvas.SetColor (50, 118, 179);
          }  
         }

         canvas.Rectangle (0, output[i].x1 + 5, output[i].y1 + 5, output[i].x2 - output[i].x1 -10,
                            output[i].y2 - output[i].y1 - 10 );
         canvas.SetLineWidth(1);                    
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
cpart_keypad::Process(void)
{

 if (refresh > 10)
  {
   const picpin * ppins = Window5.GetPinsValues ();
   refresh = 0;

   for (int i = 0; i < 8; i++)
    Window5.SetPin (output_pins[i], !pull);

   switch (type)
    {
    case KT4x4:
     for (int c = 0; c < 4; c++)
      {
       for (int l = 0; l < 4; l++)
        {
         if (keys[l][c])
          {
           Window5.SetPin (output_pins[l], ppins[output_pins[4 + c] - 1].value);
           Window5.SetPin (output_pins[4 + c], ppins[output_pins[l] - 1].value);
          }
        }
      }
     break;
    case KT4x3:
     for (int c = 0; c < 3; c++)
      {
       for (int l = 0; l < 4; l++)
        {
         if (keys[l][c])
          {
           Window5.SetPin (output_pins[l], ppins[output_pins[4 + c] - 1].value);
           Window5.SetPin (output_pins[4 + c], ppins[output_pins[l] - 1].value);
          }
        }
      }
     break;
    case KT2x5:
     for (int c = 0; c < 5; c++)
      {
       for (int l = 0; l < 2; l++)
        {
         if (keys2[l][c])
          {
           Window5.SetPin (output_pins[l], ppins[output_pins[2 + c] - 1].value);
           Window5.SetPin (output_pins[2 + c], ppins[output_pins[l] - 1].value);
          }
        }
      }
     break;
    }
  }
 refresh++;
}

void
cpart_keypad::EvMouseButtonPress(uint button, uint x, uint y, uint state)
{
 int i;

 for (i = 0; i < inputc; i++)
  {
   if (PointInside (x, y, input[i]))
    {

     switch (input[i].id)
      {
      case I_K1: keys[0][0] = 1;
       keys2[0][0] = 1;
       output_ids[O_K1]->value = 1;
       output_ids[O_K1]->update = 1;
       break;
      case I_K2: keys[0][1] = 1;
       keys2[0][1] = 1;
       output_ids[O_K2]->value = 1;
       output_ids[O_K2]->update = 1;
       break;
      case I_K3: keys[0][2] = 1;
       keys2[0][2] = 1;
       output_ids[O_K3]->value = 1;
       output_ids[O_K3]->update = 1;
       break;
      case I_KA: keys[0][3] = 1;
       output_ids[O_KA]->value = 1;
       output_ids[O_KA]->update = 1;
       break;
      case I_K4: keys[1][0] = 1;
       keys2[0][3] = 1;
       output_ids[O_K4]->value = 1;
       output_ids[O_K4]->update = 1;
       break;
      case I_K5: keys[1][1] = 1;
       keys2[0][4] = 1;
       output_ids[O_K5]->value = 1;
       output_ids[O_K5]->update = 1;
       break;
      case I_K6: keys[1][2] = 1;
       keys2[1][0] = 1;
       output_ids[O_K6]->value = 1;
       output_ids[O_K6]->update = 1;
       break;
      case I_KB: keys[1][3] = 1;
       output_ids[O_KB]->value = 1;
       output_ids[O_KB]->update = 1;
       break;
      case I_K7: keys[2][0] = 1;
       keys2[1][1] = 1;
       output_ids[O_K7]->value = 1;
       output_ids[O_K7]->update = 1;
       break;
      case I_K8: keys[2][1] = 1;
       keys2[1][2] = 1;
       output_ids[O_K8]->value = 1;
       output_ids[O_K8]->update = 1;
       break;
      case I_K9: keys[2][2] = 1;
       keys2[1][3] = 1;
       output_ids[O_K9]->value = 1;
       output_ids[O_K9]->update = 1;
       break;
      case I_KC: keys[2][3] = 1;
       output_ids[O_KC]->value = 1;
       output_ids[O_KC]->update = 1;
       break;
      case I_Ka: keys[3][0] = 1;
       output_ids[O_Ka]->value = 1;
       output_ids[O_Ka]->update = 1;
       break;
      case I_K0: keys[3][1] = 1;
       keys2[1][4] = 1;
       output_ids[O_K0]->value = 1;
       output_ids[O_K0]->update = 1;
       break;
      case I_KT: keys[3][2] = 1;
       output_ids[O_KT]->value = 1;
       output_ids[O_KT]->update = 1;
       break;
      case I_KD: keys[3][3] = 1;
       output_ids[O_KD]->value = 1;
       output_ids[O_KD]->update = 1;
       break;
      }
    }
  }
}

void
cpart_keypad::EvMouseButtonRelease(uint button, uint x, uint y, uint state)
{
 int i;

 for (i = 0; i < inputc; i++)
  {
   if (PointInside (x, y, input[i]))
    {
     switch (input[i].id)
      {
      case I_K1: keys[0][0] = 0;
       keys2[0][0] = 0;
       output_ids[O_K1]->value = 0;
       output_ids[O_K1]->update = 1;
       break;
      case I_K2: keys[0][1] = 0;
       keys2[0][1] = 0;
       output_ids[O_K2]->value = 0;
       output_ids[O_K2]->update = 1;
       break;
      case I_K3: keys[0][2] = 0;
       keys2[0][2] = 0;
       output_ids[O_K3]->value = 0;
       output_ids[O_K3]->update = 1;
       break;
      case I_KA: keys[0][3] = 0;
       output_ids[O_KA]->value = 0;
       output_ids[O_KA]->update = 1;
       break;
      case I_K4: keys[1][0] = 0;
       keys2[0][3] = 0;
       output_ids[O_K4]->value = 0;
       output_ids[O_K4]->update = 1; 
       break;
      case I_K5: keys[1][1] = 0;
       keys2[0][4] = 0;
       output_ids[O_K5]->value = 0;
       output_ids[O_K5]->update = 1;
       break;
      case I_K6: keys[1][2] = 0;
       keys2[1][0] = 0;
       output_ids[O_K6]->value = 0;
       output_ids[O_K6]->update = 1;
       break;
      case I_KB: keys[1][3] = 0;
       output_ids[O_KB]->value = 0;
       output_ids[O_KB]->update = 1;
       break;
      case I_K7: keys[2][0] = 0;
       keys2[1][1] = 0;
       output_ids[O_K7]->value = 0;
       output_ids[O_K7]->update = 1;
       break;
      case I_K8: keys[2][1] = 0;
       output_ids[O_K8]->value = 0;
       output_ids[O_K8]->update = 1;
       keys2[1][2] = 0;
       break;
      case I_K9: keys[2][2] = 0;
       keys2[1][3] = 0;
       output_ids[O_K9]->value = 0;
       output_ids[O_K9]->update = 1;
       break;
      case I_KC: keys[2][3] = 0;
       output_ids[O_KC]->value = 0;
       output_ids[O_KC]->update = 1;
       break;
      case I_Ka: keys[3][0] = 0;
       output_ids[O_Ka]->value = 0;
       output_ids[O_Ka]->update = 1;
       break;
      case I_K0: keys[3][1] = 0;
       keys2[1][4] = 0;
       output_ids[O_K0]->value = 0;
       output_ids[O_K0]->update = 1;
       break;
      case I_KT: keys[3][2] = 0;
       output_ids[O_KT]->value = 0;
       output_ids[O_KT]->update = 1;
       break;
      case I_KD: keys[3][3] = 0;
       output_ids[O_KD]->value = 0;
       output_ids[O_KD]->update = 1;
       break;
      }
    }
  }
}

unsigned short
cpart_keypad::get_in_id(char * name)
{
 if (strcmp (name, "KB_1") == 0)return I_K1;
 if (strcmp (name, "KB_2") == 0)return I_K2;
 if (strcmp (name, "KB_3") == 0)return I_K3;
 if (strcmp (name, "KB_A") == 0)return I_KA;

 if (strcmp (name, "KB_4") == 0)return I_K4;
 if (strcmp (name, "KB_5") == 0)return I_K5;
 if (strcmp (name, "KB_6") == 0)return I_K6;
 if (strcmp (name, "KB_B") == 0)return I_KB;

 if (strcmp (name, "KB_7") == 0)return I_K7;
 if (strcmp (name, "KB_8") == 0)return I_K8;
 if (strcmp (name, "KB_9") == 0)return I_K9;
 if (strcmp (name, "KB_C") == 0)return I_KC;

 if (strcmp (name, "KB_a") == 0)return I_Ka;
 if (strcmp (name, "KB_0") == 0)return I_K0;
 if (strcmp (name, "KB_T") == 0)return I_KT;
 if (strcmp (name, "KB_D") == 0)return I_KD;


 printf ("Erro input '%s' don't have a valid id! \n", name);
 return -1;
}

unsigned short
cpart_keypad::get_out_id(char * name)
{

 if (strcmp (name, "PN_L1") == 0)return O_L1;
 if (strcmp (name, "PN_L2") == 0)return O_L2;
 if (strcmp (name, "PN_L3") == 0)return O_L3;
 if (strcmp (name, "PN_L4") == 0)return O_L4;
 if (strcmp (name, "PN_C1") == 0)return O_C1;
 if (strcmp (name, "PN_C2") == 0)return O_C2;
 if (strcmp (name, "PN_C3") == 0)return O_C3;
 if (strcmp (name, "PN_C4") == 0)return O_C4;
 if (strcmp (name, "PN_C5") == 0)return O_C5;

 if (strcmp (name, "KB_1") == 0)return O_K1;
 if (strcmp (name, "KB_2") == 0)return O_K2;
 if (strcmp (name, "KB_3") == 0)return O_K3;
 if (strcmp (name, "KB_A") == 0)return O_KA;

 if (strcmp (name, "KB_4") == 0)return O_K4;
 if (strcmp (name, "KB_5") == 0)return O_K5;
 if (strcmp (name, "KB_6") == 0)return O_K6;
 if (strcmp (name, "KB_B") == 0)return O_KB;

 if (strcmp (name, "KB_7") == 0)return O_K7;
 if (strcmp (name, "KB_8") == 0)return O_K8;
 if (strcmp (name, "KB_9") == 0)return O_K9;
 if (strcmp (name, "KB_C") == 0)return O_KC;

 if (strcmp (name, "KB_a") == 0)return O_Ka;
 if (strcmp (name, "KB_0") == 0)return O_K0;
 if (strcmp (name, "KB_T") == 0)return O_KT;
 if (strcmp (name, "KB_D") == 0)return O_KD;

 printf ("Erro output '%s' don't have a valid id! \n", name);
 return 1;
}

lxString
cpart_keypad::WritePreferences(void)
{
 char prefs[256];

 sprintf (prefs, "%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu", output_pins[0], output_pins[1], output_pins[2], output_pins[3], output_pins[4], output_pins[5], output_pins[6], output_pins[7], pull, type);

 return prefs;
}

void
cpart_keypad::ReadPreferences(lxString value)
{
 unsigned char tp;
 sscanf (value.c_str (), "%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu", &output_pins[0], &output_pins[1], &output_pins[2], &output_pins[3], &output_pins[4], &output_pins[5], &output_pins[6], &output_pins[7], &pull, &tp);
 memset (keys, 0, 16);
 memset (keys2, 0, 10);
 ChangeType (tp);
 RegisterRemoteControl ();
}

void
cpart_keypad::ConfigurePropertiesWindow(CPWindow * WProp)
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

 if (!pull)
  ((CCombo*) WProp->GetChildByName ("combo9"))->SetText ("UP");
 else
  ((CCombo*) WProp->GetChildByName ("combo9"))->SetText ("DOWN");

 switch (type)
  {
  case KT4x4:
   ((CLabel*) WProp->GetChildByName ("label1"))->SetText (lxT ("P1 - L1"));
   ((CLabel*) WProp->GetChildByName ("label2"))->SetText (lxT ("P2 - L2"));
   ((CLabel*) WProp->GetChildByName ("label3"))->SetText (lxT ("P3 - L3"));
   ((CLabel*) WProp->GetChildByName ("label4"))->SetText (lxT ("P4 - L4"));
   ((CLabel*) WProp->GetChildByName ("label5"))->SetText (lxT ("P5 - C1"));
   ((CLabel*) WProp->GetChildByName ("label6"))->SetText (lxT ("P6 - C2"));
   ((CLabel*) WProp->GetChildByName ("label7"))->SetText (lxT ("P7 - C3"));
   ((CLabel*) WProp->GetChildByName ("label8"))->SetText (lxT ("P8 - C4"));
   ((CCombo*) WProp->GetChildByName ("combo8"))->SetEnable (true);
   ((CCombo*) WProp->GetChildByName ("combo10"))->SetText ("4x4");
   break;
  case KT4x3:
   ((CLabel*) WProp->GetChildByName ("label1"))->SetText (lxT ("P1 - L1"));
   ((CLabel*) WProp->GetChildByName ("label2"))->SetText (lxT ("P2 - L2"));
   ((CLabel*) WProp->GetChildByName ("label3"))->SetText (lxT ("P3 - L3"));
   ((CLabel*) WProp->GetChildByName ("label4"))->SetText (lxT ("P4 - L4"));
   ((CLabel*) WProp->GetChildByName ("label5"))->SetText (lxT ("P5 - C1"));
   ((CLabel*) WProp->GetChildByName ("label6"))->SetText (lxT ("P6 - C2"));
   ((CLabel*) WProp->GetChildByName ("label7"))->SetText (lxT ("P7 - C3"));
   ((CLabel*) WProp->GetChildByName ("label8"))->SetText (lxT ("P8 - NC"));
   ((CCombo*) WProp->GetChildByName ("combo8"))->SetEnable (false);
   ((CCombo*) WProp->GetChildByName ("combo10"))->SetText ("4x3");
   break;
  case KT2x5:
   ((CLabel*) WProp->GetChildByName ("label1"))->SetText (lxT ("P1 - L1"));
   ((CLabel*) WProp->GetChildByName ("label2"))->SetText (lxT ("P2 - L2"));
   ((CLabel*) WProp->GetChildByName ("label3"))->SetText (lxT ("P3 - C1"));
   ((CLabel*) WProp->GetChildByName ("label4"))->SetText (lxT ("P4 - C2"));
   ((CLabel*) WProp->GetChildByName ("label5"))->SetText (lxT ("P5 - C3"));
   ((CLabel*) WProp->GetChildByName ("label6"))->SetText (lxT ("P6 - C4"));
   ((CLabel*) WProp->GetChildByName ("label7"))->SetText (lxT ("P7 - C5"));
   ((CLabel*) WProp->GetChildByName ("label8"))->SetText (lxT ("P8 - NC"));
   ((CCombo*) WProp->GetChildByName ("combo8"))->SetEnable (false);
   ((CCombo*) WProp->GetChildByName ("combo10"))->SetText ("2x5");
   break;
  }

 ((CButton*) WProp->GetChildByName ("button1"))->EvMouseButtonRelease = EVMOUSEBUTTONRELEASE & CPWindow5::PropButtonRelease;
 ((CButton*) WProp->GetChildByName ("button1"))->SetTag (1);

 ((CButton*) WProp->GetChildByName ("button2"))->EvMouseButtonRelease = EVMOUSEBUTTONRELEASE & CPWindow5::PropButtonRelease;

 ((CCombo*) WProp->GetChildByName ("combo10"))->EvOnComboChange = EVONCOMBOCHANGE & CPWindow5::PropComboChange;
}

void
cpart_keypad::ReadPropertiesWindow(CPWindow * WProp)
{
 output_pins[0] = atoi (((CCombo*) WProp->GetChildByName ("combo1"))->GetText ());
 output_pins[1] = atoi (((CCombo*) WProp->GetChildByName ("combo2"))->GetText ());
 output_pins[2] = atoi (((CCombo*) WProp->GetChildByName ("combo3"))->GetText ());
 output_pins[3] = atoi (((CCombo*) WProp->GetChildByName ("combo4"))->GetText ());
 output_pins[4] = atoi (((CCombo*) WProp->GetChildByName ("combo5"))->GetText ());
 output_pins[5] = atoi (((CCombo*) WProp->GetChildByName ("combo6"))->GetText ());
 output_pins[6] = atoi (((CCombo*) WProp->GetChildByName ("combo7"))->GetText ());
 output_pins[7] = atoi (((CCombo*) WProp->GetChildByName ("combo8"))->GetText ());

 pull = !(((CCombo*) WProp->GetChildByName ("combo9"))->GetText ().compare ("UP") == 0);

 lxString tps = ((CCombo*) WProp->GetChildByName ("combo10"))->GetText ();

 ComboChange ((CCombo*) WProp->GetChildByName ("combo10"), tps);

 memset (keys, 0, 16);
 memset (keys2, 0, 10);

 RegisterRemoteControl ();
}

void
cpart_keypad::ComboChange(CCombo * control, lxString value)
{
 if (!value.compare (lxT ("4x3")))
  {
   ChangeType (KT4x3);
  }
 else if (!value.compare (lxT ("2x5")))
  {
   ChangeType (KT2x5);
  }
 else
  {
   ChangeType (KT4x4);
  }
}

part_init(PART_KEYPAD_Name, cpart_keypad, "Input");

