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
#include"part_7s_Display.h"

/* outputs */
enum
{
 O_A1, O_B1, O_C1, O_D1, O_E1, O_F1, O_G1, O_P1,
 O_A2, O_B2, O_C2, O_D2, O_E2, O_F2, O_G2, O_P2,
 O_A3, O_B3, O_C3, O_D3, O_E3, O_F3, O_G3, O_P3,
 O_A4, O_B4, O_C4, O_D4, O_E4, O_F4, O_G4, O_P4,
 O_SA, O_SB, O_SC, O_SD, O_SE, O_SF, O_SG, O_SP,
 O_DISP1, O_DISP2, O_DISP3, O_DISP4, O_FX1, O_FX2
};

cpart_7s_display::cpart_7s_display(unsigned x, unsigned y)
{
 X = x;
 Y = y;
 ReadMaps ();

 lxImage image;
 image.LoadFile (Window1.GetSharePath () + lxT ("parts/") + GetPictureFileName ());

 Bitmap = lxGetBitmapRotated(&image, &Window5, orientation); 
 image.Destroy ();
 canvas.Create (Window5.GetWWidget (), Bitmap);

 input_pins[0] = 0;
 input_pins[1] = 0;
 input_pins[2] = 0;
 input_pins[3] = 0;
 input_pins[4] = 0;
 input_pins[5] = 0;
 input_pins[6] = 0;
 input_pins[7] = 0;
 input_pins[8] = 0;
 input_pins[9] = 0;
 input_pins[10] = 0;
 input_pins[11] = 0;

 mcount = 0;

 memset (lm1, 0, 8 * sizeof (unsigned int));
 memset (lm2, 0, 8 * sizeof (unsigned int));
 memset (lm3, 0, 8 * sizeof (unsigned int));
 memset (lm4, 0, 8 * sizeof (unsigned int));

 memset (alm1, 0, 8 * sizeof (unsigned int));
 memset (alm2, 0, 8 * sizeof (unsigned int));
 memset (alm3, 0, 8 * sizeof (unsigned int));
 memset (alm4, 0, 8 * sizeof (unsigned int));
};

cpart_7s_display::~cpart_7s_display(void)
{
 delete Bitmap;
 canvas.Destroy ();
}

void
cpart_7s_display::Draw(void)
{

 int i;

 canvas.Init (1.0, 1.0, orientation);

 lxFont font (9, lxFONTFAMILY_TELETYPE, lxFONTSTYLE_NORMAL, lxFONTWEIGHT_BOLD);
 canvas.SetFont (font);

 for (i = 0; i < outputc; i++)
  {
   canvas.SetFgColor (30, 0, 0);

   switch (output[i].id)
    {
    case O_SA:
    case O_SB:
    case O_SC:
    case O_SD:
    case O_SE:
    case O_SF:
    case O_SG:
    case O_SP:
    case O_DISP1:
    case O_DISP2:
    case O_DISP3:
    case O_DISP4:
     canvas.SetColor (49, 61, 99);
     canvas.Rectangle (1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);
     canvas.SetFgColor (255, 255, 255);
     if (input_pins[output[i].id - O_SA] == 0)
      canvas.RotatedText ("NC", output[i].x1, output[i].y2, 90.0);
     else
      canvas.RotatedText (Window5.GetPinName (input_pins[output[i].id - O_SA]), output[i].x1, output[i].y2, 90.0);
     break;
    case O_FX1:
     canvas.SetColor (49, 61, 99);
     canvas.Rectangle (1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);
     canvas.SetFgColor (155, 155, 155);
     canvas.RotatedText ("GND", output[i].x1, output[i].y2, 90.0);
     break;
    case O_FX2:
     canvas.SetColor (49, 61, 99);
     canvas.Rectangle (1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);
     canvas.SetFgColor (155, 155, 155);
     canvas.RotatedText ("VCC", output[i].x1, output[i].y2, 90.0);
     break;
    case O_A1: canvas.SetColor (0, lm1[0], 0);
     canvas.Rectangle (1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);
     break;
    case O_B1: canvas.SetColor (0, lm1[1], 0);
     canvas.Rectangle (1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);
     break;
    case O_C1: canvas.SetColor (0, lm1[2], 0);
     canvas.Rectangle (1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);
     break;
    case O_D1: canvas.SetColor (0, lm1[3], 0);
     canvas.Rectangle (1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);
     break;
    case O_E1: canvas.SetColor (0, lm1[4], 0);
     canvas.Rectangle (1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);
     break;
    case O_F1: canvas.SetColor (0, lm1[5], 0);
     canvas.Rectangle (1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);
     break;
    case O_G1: canvas.SetColor (0, lm1[6], 0);
     canvas.Rectangle (1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);
     break;
    case O_P1: canvas.SetColor (0, lm1[7], 0);
     canvas.Rectangle (1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);
     break;

    case O_A2: canvas.SetColor (0, lm2[0], 0);
     canvas.Rectangle (1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);
     break;
    case O_B2: canvas.SetColor (0, lm2[1], 0);
     canvas.Rectangle (1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);
     break;
    case O_C2: canvas.SetColor (0, lm2[2], 0);
     canvas.Rectangle (1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);
     break;
    case O_D2: canvas.SetColor (0, lm2[3], 0);
     canvas.Rectangle (1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);
     break;
    case O_E2: canvas.SetColor (0, lm2[4], 0);
     canvas.Rectangle (1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);
     break;
    case O_F2: canvas.SetColor (0, lm2[5], 0);
     canvas.Rectangle (1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);
     break;
    case O_G2: canvas.SetColor (0, lm2[6], 0);
     canvas.Rectangle (1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);
     break;
    case O_P2: canvas.SetColor (0, lm2[7], 0);
     canvas.Rectangle (1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);
     break;

    case O_A3: canvas.SetColor (0, lm3[0], 0);
     canvas.Rectangle (1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);
     break;
    case O_B3: canvas.SetColor (0, lm3[1], 0);
     canvas.Rectangle (1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);
     break;
    case O_C3: canvas.SetColor (0, lm3[2], 0);
     canvas.Rectangle (1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);
     break;
    case O_D3: canvas.SetColor (0, lm3[3], 0);
     canvas.Rectangle (1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);
     break;
    case O_E3: canvas.SetColor (0, lm3[4], 0);
     canvas.Rectangle (1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);
     break;
    case O_F3: canvas.SetColor (0, lm3[5], 0);
     canvas.Rectangle (1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);
     break;
    case O_G3: canvas.SetColor (0, lm3[6], 0);
     canvas.Rectangle (1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);
     break;
    case O_P3: canvas.SetColor (0, lm3[7], 0);
     canvas.Rectangle (1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);
     break;

    case O_A4: canvas.SetColor (0, lm4[0], 0);
     canvas.Rectangle (1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);
     break;
    case O_B4: canvas.SetColor (0, lm4[1], 0);
     canvas.Rectangle (1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);
     break;
    case O_C4: canvas.SetColor (0, lm4[2], 0);
     canvas.Rectangle (1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);
     break;
    case O_D4: canvas.SetColor (0, lm4[3], 0);
     canvas.Rectangle (1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);
     break;
    case O_E4: canvas.SetColor (0, lm4[4], 0);
     canvas.Rectangle (1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);
     break;
    case O_F4: canvas.SetColor (0, lm4[5], 0);
     canvas.Rectangle (1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);
     break;
    case O_G4: canvas.SetColor (0, lm4[6], 0);
     canvas.Rectangle (1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);
     break;
    case O_P4: canvas.SetColor (0, lm4[7], 0);
     canvas.Rectangle (1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);
     break;

    }


  };


 canvas.End ();

}

unsigned short
cpart_7s_display::get_in_id(char * name)
{
 printf ("Erro input '%s' don't have a valid id! \n", name);
 return -1;
};

unsigned short
cpart_7s_display::get_out_id(char * name)
{

 if (strcmp (name, "A1") == 0)return O_A1;
 if (strcmp (name, "B1") == 0)return O_B1;
 if (strcmp (name, "C1") == 0)return O_C1;
 if (strcmp (name, "D1") == 0)return O_D1;
 if (strcmp (name, "E1") == 0)return O_E1;
 if (strcmp (name, "F1") == 0)return O_F1;
 if (strcmp (name, "G1") == 0)return O_G1;
 if (strcmp (name, "P1") == 0)return O_P1;

 if (strcmp (name, "A2") == 0)return O_A2;
 if (strcmp (name, "B2") == 0)return O_B2;
 if (strcmp (name, "C2") == 0)return O_C2;
 if (strcmp (name, "D2") == 0)return O_D2;
 if (strcmp (name, "E2") == 0)return O_E2;
 if (strcmp (name, "F2") == 0)return O_F2;
 if (strcmp (name, "G2") == 0)return O_G2;
 if (strcmp (name, "P2") == 0)return O_P2;

 if (strcmp (name, "A3") == 0)return O_A3;
 if (strcmp (name, "B3") == 0)return O_B3;
 if (strcmp (name, "C3") == 0)return O_C3;
 if (strcmp (name, "D3") == 0)return O_D3;
 if (strcmp (name, "E3") == 0)return O_E3;
 if (strcmp (name, "F3") == 0)return O_F3;
 if (strcmp (name, "G3") == 0)return O_G3;
 if (strcmp (name, "P3") == 0)return O_P3;

 if (strcmp (name, "A4") == 0)return O_A4;
 if (strcmp (name, "B4") == 0)return O_B4;
 if (strcmp (name, "C4") == 0)return O_C4;
 if (strcmp (name, "D4") == 0)return O_D4;
 if (strcmp (name, "E4") == 0)return O_E4;
 if (strcmp (name, "F4") == 0)return O_F4;
 if (strcmp (name, "G4") == 0)return O_G4;
 if (strcmp (name, "P4") == 0)return O_P4;

 if (strcmp (name, "SA") == 0)return O_SA;
 if (strcmp (name, "SB") == 0)return O_SB;
 if (strcmp (name, "SC") == 0)return O_SC;
 if (strcmp (name, "SD") == 0)return O_SD;
 if (strcmp (name, "SE") == 0)return O_SE;
 if (strcmp (name, "SF") == 0)return O_SF;
 if (strcmp (name, "SG") == 0)return O_SG;
 if (strcmp (name, "SP") == 0)return O_SP;

 if (strcmp (name, "DISP1") == 0)return O_DISP1;
 if (strcmp (name, "DISP2") == 0)return O_DISP2;
 if (strcmp (name, "DISP3") == 0)return O_DISP3;
 if (strcmp (name, "DISP4") == 0)return O_DISP4;

 if (strcmp (name, "FX1") == 0)return O_FX1;
 if (strcmp (name, "FX2") == 0)return O_FX2;

 printf ("Erro output '%s' don't have a valid id! \n", name);
 return 1;
};

lxString
cpart_7s_display::WritePreferences(void)
{
 char prefs[256];

 sprintf (prefs, "%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu", input_pins[0], input_pins[1], input_pins[2], input_pins[3], input_pins[4], input_pins[5], input_pins[6], input_pins[7], input_pins[8], input_pins[9], input_pins[10], input_pins[11]);

 return prefs;
};

void
cpart_7s_display::ReadPreferences(lxString value)
{
 sscanf (value.c_str (), "%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu", &input_pins[0], &input_pins[1], &input_pins[2], &input_pins[3], &input_pins[4], &input_pins[5], &input_pins[6], &input_pins[7], &input_pins[8], &input_pins[9], &input_pins[10], &input_pins[11]);
};


void
cpart_7s_display::ConfigurePropertiesWindow(CPWindow * WProp)
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

 ((CCombo*) WProp->GetChildByName ("combo9"))->SetItems (Items);
 if (input_pins[8] == 0)
  ((CCombo*) WProp->GetChildByName ("combo9"))->SetText ("0  NC");
 else
  {
   spin = Window5.GetPinName (input_pins[8]);
   ((CCombo*) WProp->GetChildByName ("combo9"))->SetText (itoa (input_pins[8]) + "  " + spin);
  }

 ((CCombo*) WProp->GetChildByName ("combo10"))->SetItems (Items);
 if (input_pins[9] == 0)
  ((CCombo*) WProp->GetChildByName ("combo10"))->SetText ("0  NC");
 else
  {
   spin = Window5.GetPinName (input_pins[9]);
   ((CCombo*) WProp->GetChildByName ("combo10"))->SetText (itoa (input_pins[9]) + "  " + spin);
  }

 ((CCombo*) WProp->GetChildByName ("combo11"))->SetItems (Items);
 if (input_pins[10] == 0)
  ((CCombo*) WProp->GetChildByName ("combo11"))->SetText ("0  NC");
 else
  {
   spin = Window5.GetPinName (input_pins[10]);
   ((CCombo*) WProp->GetChildByName ("combo11"))->SetText (itoa (input_pins[10]) + "  " + spin);
  }

 ((CCombo*) WProp->GetChildByName ("combo12"))->SetItems (Items);
 if (input_pins[11] == 0)
  ((CCombo*) WProp->GetChildByName ("combo12"))->SetText ("0  NC");
 else
  {
   spin = Window5.GetPinName (input_pins[11]);
   ((CCombo*) WProp->GetChildByName ("combo12"))->SetText (itoa (input_pins[11]) + "  " + spin);
  }

 ((CButton*) WProp->GetChildByName ("button1"))->EvMouseButtonRelease = EVMOUSEBUTTONRELEASE & CPWindow5::PropButtonRelease;
 ((CButton*) WProp->GetChildByName ("button1"))->SetTag (1);

 ((CButton*) WProp->GetChildByName ("button2"))->EvMouseButtonRelease = EVMOUSEBUTTONRELEASE & CPWindow5::PropButtonRelease;
}

void
cpart_7s_display::ReadPropertiesWindow(CPWindow * WProp)
{
 input_pins[0] = atoi (((CCombo*) WProp->GetChildByName ("combo1"))->GetText ());
 input_pins[1] = atoi (((CCombo*) WProp->GetChildByName ("combo2"))->GetText ());
 input_pins[2] = atoi (((CCombo*) WProp->GetChildByName ("combo3"))->GetText ());
 input_pins[3] = atoi (((CCombo*) WProp->GetChildByName ("combo4"))->GetText ());
 input_pins[4] = atoi (((CCombo*) WProp->GetChildByName ("combo5"))->GetText ());
 input_pins[5] = atoi (((CCombo*) WProp->GetChildByName ("combo6"))->GetText ());
 input_pins[6] = atoi (((CCombo*) WProp->GetChildByName ("combo7"))->GetText ());
 input_pins[7] = atoi (((CCombo*) WProp->GetChildByName ("combo8"))->GetText ());
 input_pins[8] = atoi (((CCombo*) WProp->GetChildByName ("combo9"))->GetText ());
 input_pins[9] = atoi (((CCombo*) WProp->GetChildByName ("combo10"))->GetText ());
 input_pins[10] = atoi (((CCombo*) WProp->GetChildByName ("combo11"))->GetText ());
 input_pins[11] = atoi (((CCombo*) WProp->GetChildByName ("combo12"))->GetText ());
}

void
cpart_7s_display::PreProcess(void)
{

 memset (alm1, 0, 8 * sizeof (unsigned int));
 memset (alm2, 0, 8 * sizeof (unsigned int));
 memset (alm3, 0, 8 * sizeof (unsigned int));
 memset (alm4, 0, 8 * sizeof (unsigned int));

 JUMPSTEPS_ = Window1.GetJUMPSTEPS ();
 mcount = JUMPSTEPS_;
}

void
cpart_7s_display::Process(void)
{
 int i;
 const picpin * ppins = Window5.GetPinsValues ();

 mcount++;

 if (mcount > JUMPSTEPS_)
  {
   for (i = 0; i < 8; i++)
    {
     if (input_pins[i])
      {
       if (ppins[input_pins[i] - 1].value)
        {
         if (ppins[input_pins[8] - 1].value) alm1[i]++;
         if (ppins[input_pins[9] - 1].value) alm2[i]++;
         if (ppins[input_pins[10] - 1].value) alm3[i]++;
         if (ppins[input_pins[11] - 1].value) alm4[i]++;
        }
      }
    }
   mcount = -1;
  }

}

void
cpart_7s_display::PostProcess(void)
{
 long int NSTEPJ = Window1.GetNSTEPJ ();

 for (int i = 0; i < 8; i++)
  {

   lm1[i] = (int) ((lm1[i]+(((600.0 * alm1[i]) / NSTEPJ) + 30)) / 2.0);
   lm2[i] = (int) ((lm2[i]+(((600.0 * alm2[i]) / NSTEPJ) + 30)) / 2.0);
   lm3[i] = (int) ((lm3[i]+(((600.0 * alm3[i]) / NSTEPJ) + 30)) / 2.0);
   lm4[i] = (int) ((lm4[i]+(((600.0 * alm4[i]) / NSTEPJ) + 30)) / 2.0);
   if (lm1[i] > 255)lm1[i] = 255;
   if (lm2[i] > 255)lm2[i] = 255;
   if (lm3[i] > 255)lm3[i] = 255;
   if (lm4[i] > 255)lm4[i] = 255;
  }
}


part_init("7 Segments Display", cpart_7s_display);



