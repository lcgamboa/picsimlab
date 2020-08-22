/* ########################################################################

   PICsimLab - PIC laboratory simulator

   ########################################################################

   Copyright (c) : 2019-2020  Luis Claudio Gambôa Lopes

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
   return lxT ("keypad/keypad_4x4.png");
   break;
  case KT4x3:
   return lxT ("keypad/keypad_4x3.png");
   break;
  case KT2x5:
   return lxT ("keypad/keypad_2x5.png");
   break;
  }
 return lxT ("keypad/keypad_4x4.png");
}

lxString
cpart_keypad::GetInputMapFile(void)
{
 switch (type)
  {
  case KT4x4:
   return lxT ("keypad/keypad_4x4_i.map");
   break;
  case KT4x3:
   return lxT ("keypad/keypad_4x3_i.map");
   break;
  case KT2x5:
   return lxT ("keypad/keypad_2x5_i.map");
   break;
  }
 return lxT ("keypad/keypad_4x4_i.map");
}

lxString
cpart_keypad::GetOutputMapFile(void)
{
 switch (type)
  {
  case KT4x4:
   return lxT ("keypad/keypad_4x4_o.map");
   break;
  case KT4x3:
   return lxT ("keypad/keypad_4x3_o.map");
   break;
  case KT2x5:
   return lxT ("keypad/keypad_2x5_o.map");
   break;
  }
 return lxT ("keypad/keypad_4x4_o.map");
}

cpart_keypad::cpart_keypad(unsigned x, unsigned y)
{
 X = x;
 Y = y;
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


 if(!tp)tp=KT4x4;
 if(tp > KT2x5)tp=KT4x4; 
 
 //if same
 if (tp == type) return;

 if (Bitmap)
  {
   delete Bitmap;
   canvas.Destroy ();
  }

 type = tp;

 ReadMaps ();

 lxImage image;
 image.LoadFile (Window1.GetSharePath () + lxT ("parts/") + GetPictureFileName ());

 Bitmap = new lxBitmap (image, &Window5);
 image.Destroy ();

 canvas.Create (Window5.GetWWidget (), Bitmap);

}

void
cpart_keypad::Draw(void)
{

 int i;

 canvas.Init ();

 lxFont font (9, lxFONTFAMILY_TELETYPE, lxFONTSTYLE_NORMAL, lxFONTWEIGHT_BOLD);
 canvas.SetFont (font);

 for (i = 0; i < outputc; i++)
  {

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
     break;
    }


  }

 canvas.End ();

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
   if (((input[i].x1 <= x)&&(input[i].x2 >= x))&&((input[i].y1 <= y)&&(input[i].y2 >= y)))
    {

     switch (input[i].id)
      {
      case I_K1: keys[0][0] = 1; keys2[0][0] = 1;
       break;
      case I_K2: keys[0][1] = 1; keys2[0][1] = 1;
       break;
      case I_K3: keys[0][2] = 1; keys2[0][2] = 1;
       break;
      case I_KA: keys[0][3] = 1;
       break;
      case I_K4: keys[1][0] = 1; keys2[0][3] = 1;
       break;
      case I_K5: keys[1][1] = 1; keys2[0][4] = 1;
       break;
      case I_K6: keys[1][2] = 1; keys2[1][0] = 1;
       break;
      case I_KB: keys[1][3] = 1;
       break;
      case I_K7: keys[2][0] = 1; keys2[1][1] = 1;
       break;
      case I_K8: keys[2][1] = 1; keys2[1][2] = 1;
       break;
      case I_K9: keys[2][2] = 1; keys2[1][3] = 1;
       break;
      case I_KC: keys[2][3] = 1;
       break;
      case I_Ka: keys[3][0] = 1;
       break;
      case I_K0: keys[3][1] = 1; keys2[1][4] = 1;
       break;
      case I_KT: keys[3][2] = 1;
       break;
      case I_KD: keys[3][3] = 1;
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
   if (((input[i].x1 <= x)&&(input[i].x2 >= x))&&((input[i].y1 <= y)&&(input[i].y2 >= y)))
    {
     switch (input[i].id)
      {
      case I_K1: keys[0][0] = 0; keys2[0][0] = 0;
       break;
      case I_K2: keys[0][1] = 0; keys2[0][1] = 0;
       break;
      case I_K3: keys[0][2] = 0; keys2[0][2] = 0;
       break;
      case I_KA: keys[0][3] = 0;
       break;
      case I_K4: keys[1][0] = 0; keys2[0][3] = 0;
       break;
      case I_K5: keys[1][1] = 0; keys2[0][4] = 0;
       break;
      case I_K6: keys[1][2] = 0; keys2[1][0] = 0;
       break;
      case I_KB: keys[1][3] = 0;
       break;
      case I_K7: keys[2][0] = 0; keys2[1][1] = 0;
       break;
      case I_K8: keys[2][1] = 0; keys2[1][2] = 0;
       break;
      case I_K9: keys[2][2] = 0; keys2[1][3] = 0;
       break;
      case I_KC: keys[2][3] = 0;
       break;
      case I_Ka: keys[3][0] = 0;
       break;
      case I_K0: keys[3][1] = 0; keys2[1][4] = 0;
       break;
      case I_KT: keys[3][2] = 0;
       break;
      case I_KD: keys[3][3] = 0;
       break;
      }
    }
  }
}

unsigned short
cpart_keypad::get_in_id(char * name)
{
 if (strcmp (name, "K1") == 0)return I_K1;
 if (strcmp (name, "K2") == 0)return I_K2;
 if (strcmp (name, "K3") == 0)return I_K3;
 if (strcmp (name, "KA") == 0)return I_KA;

 if (strcmp (name, "K4") == 0)return I_K4;
 if (strcmp (name, "K5") == 0)return I_K5;
 if (strcmp (name, "K6") == 0)return I_K6;
 if (strcmp (name, "KB") == 0)return I_KB;

 if (strcmp (name, "K7") == 0)return I_K7;
 if (strcmp (name, "K8") == 0)return I_K8;
 if (strcmp (name, "K9") == 0)return I_K9;
 if (strcmp (name, "KC") == 0)return I_KC;

 if (strcmp (name, "Ka") == 0)return I_Ka;
 if (strcmp (name, "K0") == 0)return I_K0;
 if (strcmp (name, "KT") == 0)return I_KT;
 if (strcmp (name, "KD") == 0)return I_KD;


 printf ("Erro input '%s' don't have a valid id! \n", name);
 return -1;
}

unsigned short
cpart_keypad::get_out_id(char * name)
{

 if (strcmp (name, "L1") == 0)return O_L1;
 if (strcmp (name, "L2") == 0)return O_L2;
 if (strcmp (name, "L3") == 0)return O_L3;
 if (strcmp (name, "L4") == 0)return O_L4;
 if (strcmp (name, "C1") == 0)return O_C1;
 if (strcmp (name, "C2") == 0)return O_C2;
 if (strcmp (name, "C3") == 0)return O_C3;
 if (strcmp (name, "C4") == 0)return O_C4;
 if (strcmp (name, "C5") == 0)return O_C5;

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

 ComboChange (tps);

 memset (keys, 0, 16);
 memset (keys2, 0, 10);

}

void
cpart_keypad::ComboChange(lxString value)
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

part_init("Keypad", cpart_keypad);

