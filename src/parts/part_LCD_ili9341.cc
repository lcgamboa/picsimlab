/* ########################################################################

   PICsimLab - PIC laboratory simulator

   ########################################################################

   Copyright (c) : 2020-2021  Luis Claudio Gambôa Lopes

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
#include"part_LCD_ili9341.h"

/* outputs */
enum
{
 O_P1, O_P2, O_P3, O_P4, O_P5, O_P6, O_P7, O_P8, O_P9, O_P10, O_P11, O_P12, O_P13, O_F1, O_F2, O_LCD,
 O_T1, O_T2, O_T3, O_T4, O_T5
};

/* inputs */
enum
{
 I_LCD
};

/* types */
enum
{
 TC_SPI = 0, TC_8BITS, TC_SPI_TOUCH, TC_8BITS_TOUCH
};

cpart_LCD_ili9341::cpart_LCD_ili9341(unsigned x, unsigned y):
font (8, lxFONTFAMILY_TELETYPE, lxFONTSTYLE_NORMAL, lxFONTWEIGHT_BOLD)
{
 X = x;
 Y = y;

 Bitmap = NULL;
 type_com = -1;
 ChangeType (TC_SPI);

 lcd_ili9341_init (&lcd);
 lcd_ili9341_rst (&lcd);

 tsc_XPT2046_init (&touch, 240, 320);
 tsc_XPT2046_rst (&touch);

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
 input_pins[12] = 0;

 touch_pins[0] = 0;
 touch_pins[1] = 0;
 touch_pins[2] = 0;
 touch_pins[3] = 0;
 touch_pins[4] = 0;

 active[0] = 0;
}

lxString
cpart_LCD_ili9341::GetPictureFileName(void)
{
 switch (type_com)
  {
  case TC_SPI:
   return lxT ("LCD_ili9341/LCD_ili9341.svg");
   break;
  case TC_SPI_TOUCH:
   return lxT ("LCD_ili9341/LCD_ili9341_t.svg");
   break;
  case TC_8BITS:
   return lxT ("LCD_ili9341/LCD_ili9341_8.svg");
   break;
  case TC_8BITS_TOUCH:
   return lxT ("LCD_ili9341/LCD_ili9341_8_t.svg");
   break;
  }
 return lxT ("LCD_ili9341/LCD_ili9341.svg");
}

lxString
cpart_LCD_ili9341::GetInputMapFile(void)
{
 switch (type_com)
  {
  case TC_SPI:
  case TC_SPI_TOUCH:
   return lxT ("LCD_ili9341/LCD_ili9341_i.map");
   break;
  case TC_8BITS:
   return lxT ("LCD_ili9341/LCD_ili9341_8_i.map");
   break;
  case TC_8BITS_TOUCH:
   return lxT ("LCD_ili9341/LCD_ili9341_8_t_i.map");
   break;
  }
 return lxT ("LCD_ili9341 / LCD_ili9341_i.map");
}

lxString
cpart_LCD_ili9341::GetOutputMapFile(void)
{
 switch (type_com)
  {
  case TC_SPI:
   return lxT ("LCD_ili9341/LCD_ili9341_o.map");
   break;
  case TC_SPI_TOUCH:
   return lxT ("LCD_ili9341/LCD_ili9341_t_o.map");
   break;
  case TC_8BITS:
   return lxT ("LCD_ili9341/LCD_ili9341_8_o.map");
   break;
  case TC_8BITS_TOUCH:
   return lxT ("LCD_ili9341/LCD_ili9341_8_t_o.map");
   break;
  }
 return lxT ("LCD_ili9341/LCD_ili9341_o.map");
}

cpart_LCD_ili9341::~cpart_LCD_ili9341(void)
{

 delete Bitmap;
 canvas.Destroy ();
}

void
cpart_LCD_ili9341::Draw(void)
{

 int i;

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
    case O_P5:
    case O_P6:
    case O_P7:
    case O_P8:
    case O_P9:
    case O_P10:
    case O_P11:
    case O_P12:
    case O_P13:
     canvas.SetColor (49, 61, 99);
     canvas.Rectangle (1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);
     canvas.SetFgColor (255, 255, 255);
     if (input_pins[output[i].id - O_P1] == 0)
      canvas.RotatedText ("NC", output[i].x1, output[i].y2, 90.0);
     else
      canvas.RotatedText (Window5.GetPinName (input_pins[output[i].id - O_P1]), output[i].x1, output[i].y2, 90.0);
     break;
    case O_T1:
    case O_T2:
    case O_T3:
    case O_T4:
    case O_T5:
     canvas.SetColor (49, 61, 99);
     canvas.Rectangle (1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);
     canvas.SetFgColor (255, 255, 255);
     if (touch_pins[output[i].id - O_T1] == 0)
      canvas.RotatedText ("NC", output[i].x1, output[i].y2, 90.0);
     else
      canvas.RotatedText (Window5.GetPinName (touch_pins[output[i].id - O_T1]), output[i].x1, output[i].y2, 90.0);
     break;
    case O_F2:
     canvas.SetColor (49, 61, 99);
     canvas.Rectangle (1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);
     canvas.SetFgColor (155, 155, 155);
     canvas.RotatedText ("3.3V", output[i].x1, output[i].y2, 90.0);
     break;
    case O_F1:
     canvas.SetColor (49, 61, 99);
     canvas.Rectangle (1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);
     canvas.SetFgColor (155, 155, 155);
     canvas.RotatedText ("GND", output[i].x1, output[i].y2, 90.0);
     break;
    case O_LCD:
     //draw lcd text
     if (lcd.update)
      {
       //canvas.SetColor (255, 255, 0);
       //canvas.Rectangle (1, output[i].x1-2, output[i].y1-2, output[i].x2 - output[i].x1+4, output[i].y2 - output[i].y1+4);

       canvas.SetColor (0, 90 + 40, 0);
       lcd_ili9341_draw (&lcd, &canvas, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1, 1);
      }
     /*
     else
     {
     canvas.Rectangle (1, output[i].x1, output[i].y1, output[i].x2-output[i].x1,output[i].y2-output[i].y1 );
     }
      */
     break;
    }
  }
 canvas.End ();
}

unsigned short
cpart_LCD_ili9341::get_in_id(char * name)
{

 if (strcmp (name, "DS_LCD") == 0)return I_LCD;

 printf ("Erro input '%s' don't have a valid id! \n", name);
 return -1;
};

unsigned short
cpart_LCD_ili9341::get_out_id(char * name)
{

 if (strcmp (name, "PN_1") == 0)return O_P1;
 if (strcmp (name, "PN_2") == 0)return O_P2;
 if (strcmp (name, "PN_3") == 0)return O_P3;
 if (strcmp (name, "PN_4") == 0)return O_P4;
 if (strcmp (name, "PN_5") == 0)return O_P5;

 if (strcmp (name, "PN_6") == 0)return O_P6;
 if (strcmp (name, "PN_7") == 0)return O_P7;
 if (strcmp (name, "PN_8") == 0)return O_P8;
 if (strcmp (name, "PN_9") == 0)return O_P9;
 if (strcmp (name, "PN_10") == 0)return O_P10;
 if (strcmp (name, "PN_11") == 0)return O_P11;
 if (strcmp (name, "PN_12") == 0)return O_P12;
 if (strcmp (name, "PN_13") == 0)return O_P13;

 if (strcmp (name, "PN_F1") == 0)return O_F1;
 if (strcmp (name, "PN_F2") == 0)return O_F2;

 if (strcmp (name, "DS_LCD") == 0)return O_LCD;

 if (strcmp (name, "PN_T1") == 0)return O_T1;
 if (strcmp (name, "PN_T2") == 0)return O_T2;
 if (strcmp (name, "PN_T3") == 0)return O_T3;
 if (strcmp (name, "PN_T4") == 0)return O_T4;
 if (strcmp (name, "PN_T5") == 0)return O_T5;

 printf ("Erro output '%s' don't have a valid id! \n", name);
 return 1;
};

lxString
cpart_LCD_ili9341::WritePreferences(void)
{

 char prefs[256];

 sprintf (prefs, "%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu",
          input_pins[0], input_pins[1], input_pins[2], input_pins[3], input_pins[4], type_com,
          input_pins[5], input_pins[6], input_pins[7], input_pins[8], input_pins[9], input_pins[10],
          input_pins[11], input_pins[12], touch_pins[0], touch_pins[1], touch_pins[2], touch_pins[3], touch_pins[4]);
 return prefs;
}

void
cpart_LCD_ili9341::ReadPreferences(lxString value)
{
 unsigned char tp;
 sscanf (value.c_str (), "%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu",
         &input_pins[0], &input_pins[1], &input_pins[2], &input_pins[3], &input_pins[4], &tp,
         &input_pins[5], &input_pins[6], &input_pins[7], &input_pins[8], &input_pins[9], &input_pins[10],
         &input_pins[11], &input_pins[12], &touch_pins[0], &touch_pins[1], &touch_pins[2], &touch_pins[3], &touch_pins[4]);
 Reset ();
 ChangeType (tp);
}

void
cpart_LCD_ili9341::ConfigurePropertiesWindow(CPWindow * WProp)
{
 lxString Items = Window5.GetPinsNames ();
 lxString spin;
 lxString name;

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


 switch (type_com)
  {
  case TC_SPI:
   WProp->SetWidth (389);
   WProp->SetHeight (370);

   ((CButton*) WProp->GetChildByName ("button1"))->SetX (104);
   ((CButton*) WProp->GetChildByName ("button2"))->SetX (185);
   ((CButton*) WProp->GetChildByName ("button1"))->SetY (286);
   ((CButton*) WProp->GetChildByName ("button2"))->SetY (286);

   ((CCombo*) WProp->GetChildByName ("combo6"))->SetText ("SPI");

   ((CCombo*) WProp->GetChildByName ("combo7"))->SetVisible (false);
   ((CCombo*) WProp->GetChildByName ("combo8"))->SetVisible (false);
   ((CCombo*) WProp->GetChildByName ("combo9"))->SetVisible (false);
   ((CCombo*) WProp->GetChildByName ("combo10"))->SetVisible (false);
   ((CCombo*) WProp->GetChildByName ("combo11"))->SetVisible (false);
   ((CCombo*) WProp->GetChildByName ("combo12"))->SetVisible (false);
   ((CCombo*) WProp->GetChildByName ("combo13"))->SetVisible (false);
   ((CCombo*) WProp->GetChildByName ("combo14"))->SetVisible (false);

   ((CCombo*) WProp->GetChildByName ("combo15"))->SetVisible (false);
   ((CCombo*) WProp->GetChildByName ("combo16"))->SetVisible (false);
   ((CCombo*) WProp->GetChildByName ("combo17"))->SetVisible (false);
   ((CCombo*) WProp->GetChildByName ("combo18"))->SetVisible (false);
   ((CCombo*) WProp->GetChildByName ("combo19"))->SetVisible (false);


   ((CLabel*) WProp->GetChildByName ("label9"))->SetVisible (false);
   ((CLabel*) WProp->GetChildByName ("label10"))->SetVisible (false);
   ((CLabel*) WProp->GetChildByName ("label11"))->SetVisible (false);
   ((CLabel*) WProp->GetChildByName ("label12"))->SetVisible (false);
   ((CLabel*) WProp->GetChildByName ("label13"))->SetVisible (false);
   ((CLabel*) WProp->GetChildByName ("label14"))->SetVisible (false);
   ((CLabel*) WProp->GetChildByName ("label15"))->SetVisible (false);
   ((CLabel*) WProp->GetChildByName ("label16"))->SetVisible (false);

   ((CLabel*) WProp->GetChildByName ("label17"))->SetVisible (false);
   ((CLabel*) WProp->GetChildByName ("label18"))->SetVisible (false);
   ((CLabel*) WProp->GetChildByName ("label19"))->SetVisible (false);
   ((CLabel*) WProp->GetChildByName ("label20"))->SetVisible (false);
   ((CLabel*) WProp->GetChildByName ("label21"))->SetVisible (false);

   ((CLabel*) WProp->GetChildByName ("label5"))->SetText ("Pin 3 - CLK");
   ((CLabel*) WProp->GetChildByName ("label4"))->SetText ("Pin 4 - DIN");
   break;
  case TC_8BITS:

   WProp->SetWidth (740);
   WProp->SetHeight (370);

   ((CButton*) WProp->GetChildByName ("button1"))->SetX (154);
   ((CButton*) WProp->GetChildByName ("button2"))->SetX (235);
   ((CButton*) WProp->GetChildByName ("button1"))->SetY (286);
   ((CButton*) WProp->GetChildByName ("button2"))->SetY (286);

   ((CCombo*) WProp->GetChildByName ("combo6"))->SetText ("8Bits");

   ((CCombo*) WProp->GetChildByName ("combo7"))->SetVisible (true);
   ((CCombo*) WProp->GetChildByName ("combo8"))->SetVisible (true);
   ((CCombo*) WProp->GetChildByName ("combo9"))->SetVisible (true);
   ((CCombo*) WProp->GetChildByName ("combo10"))->SetVisible (true);
   ((CCombo*) WProp->GetChildByName ("combo11"))->SetVisible (true);
   ((CCombo*) WProp->GetChildByName ("combo12"))->SetVisible (true);
   ((CCombo*) WProp->GetChildByName ("combo13"))->SetVisible (true);
   ((CCombo*) WProp->GetChildByName ("combo14"))->SetVisible (true);

   ((CCombo*) WProp->GetChildByName ("combo15"))->SetVisible (false);
   ((CCombo*) WProp->GetChildByName ("combo16"))->SetVisible (false);
   ((CCombo*) WProp->GetChildByName ("combo17"))->SetVisible (false);
   ((CCombo*) WProp->GetChildByName ("combo18"))->SetVisible (false);
   ((CCombo*) WProp->GetChildByName ("combo19"))->SetVisible (false);

   ((CLabel*) WProp->GetChildByName ("label9"))->SetVisible (true);
   ((CLabel*) WProp->GetChildByName ("label10"))->SetVisible (true);
   ((CLabel*) WProp->GetChildByName ("label11"))->SetVisible (true);
   ((CLabel*) WProp->GetChildByName ("label12"))->SetVisible (true);
   ((CLabel*) WProp->GetChildByName ("label13"))->SetVisible (true);
   ((CLabel*) WProp->GetChildByName ("label14"))->SetVisible (true);
   ((CLabel*) WProp->GetChildByName ("label15"))->SetVisible (true);
   ((CLabel*) WProp->GetChildByName ("label16"))->SetVisible (true);

   ((CLabel*) WProp->GetChildByName ("label17"))->SetVisible (false);
   ((CLabel*) WProp->GetChildByName ("label18"))->SetVisible (false);
   ((CLabel*) WProp->GetChildByName ("label19"))->SetVisible (false);
   ((CLabel*) WProp->GetChildByName ("label20"))->SetVisible (false);
   ((CLabel*) WProp->GetChildByName ("label21"))->SetVisible (false);

   ((CLabel*) WProp->GetChildByName ("label5"))->SetText ("Pin 3 - /WR");
   ((CLabel*) WProp->GetChildByName ("label4"))->SetText ("Pin 4 - /RD");

   for (int i = 0; i < 8; i++)
    {
     name.Printf ("combo%i", i + 7);
     ((CCombo*) WProp->GetChildByName (name))->SetItems (Items);
     if (input_pins[5 + i] == 0)
      ((CCombo*) WProp->GetChildByName (name))->SetText ("0  NC");
     else
      {
       spin = Window5.GetPinName (input_pins[5 + i]);
       ((CCombo*) WProp->GetChildByName (name))->SetText (itoa (input_pins[5 + i]) + "  " + spin);
      }
    }
   break;
  case TC_SPI_TOUCH:
   WProp->SetWidth (389);
   WProp->SetHeight (570);

   ((CButton*) WProp->GetChildByName ("button1"))->SetX (104);
   ((CButton*) WProp->GetChildByName ("button2"))->SetX (185);
   ((CButton*) WProp->GetChildByName ("button1"))->SetY (486);
   ((CButton*) WProp->GetChildByName ("button2"))->SetY (486);


   ((CCombo*) WProp->GetChildByName ("combo6"))->SetText ("SPI+Touch");

   ((CCombo*) WProp->GetChildByName ("combo7"))->SetVisible (false);
   ((CCombo*) WProp->GetChildByName ("combo8"))->SetVisible (false);
   ((CCombo*) WProp->GetChildByName ("combo9"))->SetVisible (false);
   ((CCombo*) WProp->GetChildByName ("combo10"))->SetVisible (false);
   ((CCombo*) WProp->GetChildByName ("combo11"))->SetVisible (false);
   ((CCombo*) WProp->GetChildByName ("combo12"))->SetVisible (false);
   ((CCombo*) WProp->GetChildByName ("combo13"))->SetVisible (false);
   ((CCombo*) WProp->GetChildByName ("combo14"))->SetVisible (false);

   ((CCombo*) WProp->GetChildByName ("combo15"))->SetVisible (true);
   ((CCombo*) WProp->GetChildByName ("combo16"))->SetVisible (true);
   ((CCombo*) WProp->GetChildByName ("combo17"))->SetVisible (true);
   ((CCombo*) WProp->GetChildByName ("combo18"))->SetVisible (true);
   ((CCombo*) WProp->GetChildByName ("combo19"))->SetVisible (true);


   ((CLabel*) WProp->GetChildByName ("label9"))->SetVisible (false);
   ((CLabel*) WProp->GetChildByName ("label10"))->SetVisible (false);
   ((CLabel*) WProp->GetChildByName ("label11"))->SetVisible (false);
   ((CLabel*) WProp->GetChildByName ("label12"))->SetVisible (false);
   ((CLabel*) WProp->GetChildByName ("label13"))->SetVisible (false);
   ((CLabel*) WProp->GetChildByName ("label14"))->SetVisible (false);
   ((CLabel*) WProp->GetChildByName ("label15"))->SetVisible (false);
   ((CLabel*) WProp->GetChildByName ("label16"))->SetVisible (false);

   ((CLabel*) WProp->GetChildByName ("label17"))->SetVisible (true);
   ((CLabel*) WProp->GetChildByName ("label18"))->SetVisible (true);
   ((CLabel*) WProp->GetChildByName ("label19"))->SetVisible (true);
   ((CLabel*) WProp->GetChildByName ("label20"))->SetVisible (true);
   ((CLabel*) WProp->GetChildByName ("label21"))->SetVisible (true);

   ((CLabel*) WProp->GetChildByName ("label5"))->SetText ("Pin 3 - CLK");
   ((CLabel*) WProp->GetChildByName ("label4"))->SetText ("Pin 4 - DIN");

   for (int i = 0; i < 5; i++)
    {
     name.Printf ("combo%i", i + 15);
     ((CCombo*) WProp->GetChildByName (name))->SetItems (Items);
     if (touch_pins[i] == 0)
      ((CCombo*) WProp->GetChildByName (name))->SetText ("0  NC");
     else
      {
       spin = Window5.GetPinName (touch_pins[i]);
       ((CCombo*) WProp->GetChildByName (name))->SetText (itoa (touch_pins[i]) + "  " + spin);
      }
    }

   break;
  case TC_8BITS_TOUCH:

   WProp->SetWidth (740);
   WProp->SetHeight (570);

   ((CButton*) WProp->GetChildByName ("button1"))->SetX (154);
   ((CButton*) WProp->GetChildByName ("button2"))->SetX (235);
   ((CButton*) WProp->GetChildByName ("button1"))->SetY (486);
   ((CButton*) WProp->GetChildByName ("button2"))->SetY (486);

   ((CCombo*) WProp->GetChildByName ("combo6"))->SetText ("8Bits+Touch");

   ((CCombo*) WProp->GetChildByName ("combo7"))->SetVisible (true);
   ((CCombo*) WProp->GetChildByName ("combo8"))->SetVisible (true);
   ((CCombo*) WProp->GetChildByName ("combo9"))->SetVisible (true);
   ((CCombo*) WProp->GetChildByName ("combo10"))->SetVisible (true);
   ((CCombo*) WProp->GetChildByName ("combo11"))->SetVisible (true);
   ((CCombo*) WProp->GetChildByName ("combo12"))->SetVisible (true);
   ((CCombo*) WProp->GetChildByName ("combo13"))->SetVisible (true);
   ((CCombo*) WProp->GetChildByName ("combo14"))->SetVisible (true);

   ((CCombo*) WProp->GetChildByName ("combo15"))->SetVisible (true);
   ((CCombo*) WProp->GetChildByName ("combo16"))->SetVisible (true);
   ((CCombo*) WProp->GetChildByName ("combo17"))->SetVisible (true);
   ((CCombo*) WProp->GetChildByName ("combo18"))->SetVisible (true);
   ((CCombo*) WProp->GetChildByName ("combo19"))->SetVisible (true);

   ((CLabel*) WProp->GetChildByName ("label9"))->SetVisible (true);
   ((CLabel*) WProp->GetChildByName ("label10"))->SetVisible (true);
   ((CLabel*) WProp->GetChildByName ("label11"))->SetVisible (true);
   ((CLabel*) WProp->GetChildByName ("label12"))->SetVisible (true);
   ((CLabel*) WProp->GetChildByName ("label13"))->SetVisible (true);
   ((CLabel*) WProp->GetChildByName ("label14"))->SetVisible (true);
   ((CLabel*) WProp->GetChildByName ("label15"))->SetVisible (true);
   ((CLabel*) WProp->GetChildByName ("label16"))->SetVisible (true);

   ((CLabel*) WProp->GetChildByName ("label17"))->SetVisible (true);
   ((CLabel*) WProp->GetChildByName ("label18"))->SetVisible (true);
   ((CLabel*) WProp->GetChildByName ("label19"))->SetVisible (true);
   ((CLabel*) WProp->GetChildByName ("label20"))->SetVisible (true);
   ((CLabel*) WProp->GetChildByName ("label21"))->SetVisible (true);

   ((CLabel*) WProp->GetChildByName ("label5"))->SetText ("Pin 3 - /WR");
   ((CLabel*) WProp->GetChildByName ("label4"))->SetText ("Pin 4 - /RD");

   for (int i = 0; i < 8; i++)
    {
     name.Printf ("combo%i", i + 7);
     ((CCombo*) WProp->GetChildByName (name))->SetItems (Items);
     if (input_pins[5 + i] == 0)
      ((CCombo*) WProp->GetChildByName (name))->SetText ("0  NC");
     else
      {
       spin = Window5.GetPinName (input_pins[5 + i]);
       ((CCombo*) WProp->GetChildByName (name))->SetText (itoa (input_pins[5 + i]) + "  " + spin);
      }
    }

   for (int i = 0; i < 5; i++)
    {
     name.Printf ("combo%i", i + 15);
     ((CCombo*) WProp->GetChildByName (name))->SetItems (Items);
     if (touch_pins[i] == 0)
      ((CCombo*) WProp->GetChildByName (name))->SetText ("0  NC");
     else
      {
       spin = Window5.GetPinName (touch_pins[i]);
       ((CCombo*) WProp->GetChildByName (name))->SetText (itoa (touch_pins[i]) + "  " + spin);
      }
    }
   break;
  }

 ((CCombo*) WProp->GetChildByName ("combo6"))->EvOnComboChange = EVONCOMBOCHANGE & CPWindow5::PropComboChange;

 ((CButton*) WProp->GetChildByName ("button1"))->EvMouseButtonRelease = EVMOUSEBUTTONRELEASE & CPWindow5::PropButtonRelease;
 ((CButton*) WProp->GetChildByName ("button1"))->SetTag (1);

 ((CButton*) WProp->GetChildByName ("button2"))->EvMouseButtonRelease = EVMOUSEBUTTONRELEASE & CPWindow5::PropButtonRelease;
}

void
cpart_LCD_ili9341::ReadPropertiesWindow(CPWindow * WProp)
{

 input_pins[0] = atoi (((CCombo*) WProp->GetChildByName ("combo1"))->GetText ());
 input_pins[1] = atoi (((CCombo*) WProp->GetChildByName ("combo2"))->GetText ());
 input_pins[2] = atoi (((CCombo*) WProp->GetChildByName ("combo3"))->GetText ());
 input_pins[3] = atoi (((CCombo*) WProp->GetChildByName ("combo4"))->GetText ());
 input_pins[4] = atoi (((CCombo*) WProp->GetChildByName ("combo5"))->GetText ());

 if (type_com == TC_8BITS)
  {
   input_pins[5] = atoi (((CCombo*) WProp->GetChildByName ("combo7"))->GetText ());
   input_pins[6] = atoi (((CCombo*) WProp->GetChildByName ("combo8"))->GetText ());
   input_pins[7] = atoi (((CCombo*) WProp->GetChildByName ("combo9"))->GetText ());
   input_pins[8] = atoi (((CCombo*) WProp->GetChildByName ("combo10"))->GetText ());
   input_pins[9] = atoi (((CCombo*) WProp->GetChildByName ("combo11"))->GetText ());
   input_pins[10] = atoi (((CCombo*) WProp->GetChildByName ("combo12"))->GetText ());
   input_pins[11] = atoi (((CCombo*) WProp->GetChildByName ("combo13"))->GetText ());
   input_pins[12] = atoi (((CCombo*) WProp->GetChildByName ("combo14"))->GetText ());
  }

 if ((type_com == TC_8BITS_TOUCH) || (type_com == TC_SPI_TOUCH))
  {
   touch_pins[0] = atoi (((CCombo*) WProp->GetChildByName ("combo15"))->GetText ());
   touch_pins[1] = atoi (((CCombo*) WProp->GetChildByName ("combo16"))->GetText ());
   touch_pins[2] = atoi (((CCombo*) WProp->GetChildByName ("combo17"))->GetText ());
   touch_pins[3] = atoi (((CCombo*) WProp->GetChildByName ("combo18"))->GetText ());
   touch_pins[4] = atoi (((CCombo*) WProp->GetChildByName ("combo19"))->GetText ());
  }

}

void
cpart_LCD_ili9341::Process(void)
{
 unsigned short ret;
 const picpin * ppins = Window5.GetPinsValues ();


 switch (type_com)
  {
  case TC_SPI:
  case TC_SPI_TOUCH:
   if ((input_pins[0] > 0)
       &&(input_pins[1] > 0)
       &&(input_pins[2] > 0)
       &&(input_pins[3] > 0)
       &&(input_pins[4] > 0))
    {

     lcd_ili9341_SPI_io (&lcd, ppins[input_pins[1] - 1].value, ppins[input_pins[0] - 1].value, ppins[input_pins[4] - 1].value, ppins[input_pins[2] - 1].value, ppins[input_pins[3] - 1].value);
    }
   break;
  case TC_8BITS:
  case TC_8BITS_TOUCH:
   if ((input_pins[0] > 0)
       &&(input_pins[1] > 0)
       &&(input_pins[2] > 0)
       &&(input_pins[3] > 0)
       &&(input_pins[4] > 0)
       &&(input_pins[5] > 0)
       &&(input_pins[6] > 0)
       &&(input_pins[7] > 0)
       &&(input_pins[8] > 0)
       &&(input_pins[9] > 0)
       &&(input_pins[10] > 0)
       &&(input_pins[11] > 0)
       &&(input_pins[12] > 0))
    {
     unsigned char d = 0;
     if ((ppins[input_pins[5] - 1].value)) d |= 0x01;
     if ((ppins[input_pins[6] - 1].value)) d |= 0x02;
     if ((ppins[input_pins[7] - 1].value)) d |= 0x04;
     if ((ppins[input_pins[8] - 1].value)) d |= 0x08;
     if ((ppins[input_pins[9] - 1].value)) d |= 0x10;
     if ((ppins[input_pins[10] - 1].value)) d |= 0x20;
     if ((ppins[input_pins[11] - 1].value)) d |= 0x40;
     if ((ppins[input_pins[12] - 1].value)) d |= 0x80;
     ret = lcd_ili9341_8_io (&lcd, d, ppins[input_pins[0] - 1].value, ppins[input_pins[1] - 1].value, ppins[input_pins[4] - 1].value, ppins[input_pins[2] - 1].value, ppins[input_pins[3] - 1].value);
     if (ret & 0x0100)
      {
       Window5.SetPin (input_pins[5], (ret & 0x01) > 0);
       Window5.SetPin (input_pins[6], (ret & 0x02) > 0);
       Window5.SetPin (input_pins[7], (ret & 0x04) > 0);
       Window5.SetPin (input_pins[8], (ret & 0x08) > 0);
       Window5.SetPin (input_pins[9], (ret & 0x10) > 0);
       Window5.SetPin (input_pins[10], (ret & 0x20) > 0);
       Window5.SetPin (input_pins[11], (ret & 0x40) > 0);
       Window5.SetPin (input_pins[12], (ret & 0x80) > 0);
      }

    }
   break;
  }

 //Touch constroller
 switch (type_com)
  {
  case TC_SPI_TOUCH:
  case TC_8BITS_TOUCH:
   if ((touch_pins[0] > 0)
       &&(touch_pins[1] > 0)
       &&(touch_pins[2] > 0))
    {
     ret = tsc_XPT2046_SPI_io (&touch, ppins[touch_pins[0] - 1].value, ppins[touch_pins[2] - 1].value, ppins[touch_pins[1] - 1].value);
     Window5.SetPin (touch_pins[3], (ret & 0x01) > 0);
     Window5.SetPin (touch_pins[4], (ret & 0x02) > 0);
    }
   break;
  }
}

void
cpart_LCD_ili9341::ComboChange(CCombo * control, lxString value)
{
 if (!value.Cmp ("SPI"))
  {
   ChangeType (TC_SPI);
  }
 else if (!value.Cmp ("8Bits"))
  {
   ChangeType (TC_8BITS);
  }
 else if (!value.Cmp ("SPI+Touch"))
  {
   ChangeType (TC_SPI_TOUCH);
  }
 else if (!value.Cmp ("8Bits+Touch"))
  {
   ChangeType (TC_8BITS_TOUCH);
  }
}

void
cpart_LCD_ili9341::ChangeType(unsigned char tp)
{

 if (tp > 3)tp = TC_SPI;

 //if same
 if (tp == type_com) return;

 if (Bitmap)
  {

   delete Bitmap;
   canvas.Destroy ();
  }

 type_com = tp;

 ReadMaps ();

 lxImage image(&Window5);
 image.LoadFile (Window1.GetSharePath () + lxT ("parts/") + GetPictureFileName (), Orientation, Scale, Scale);

 Bitmap = new lxBitmap (&image, &Window5);
 image.Destroy ();

 canvas.Create (Window5.GetWWidget (), Bitmap);

}

void
cpart_LCD_ili9341::EvMouseButtonPress(uint button, uint x, uint y, uint state)
{
 int i;

 for (i = 0; i < inputc; i++)
  {
   if (PointInside (x, y, input[i]))
    {
     RotateCoords (&x, &y);

     switch (input[i].id)
      {
      case I_LCD:
       tsc_XPT2046_set_pos (&touch, x - input[i].x1, y - input[i].y1);
       active[0] = 1;
       break;
      }
    }
  }

}

void
cpart_LCD_ili9341::EvMouseButtonRelease(uint button, uint x, uint y, uint state)
{
 int i;

 for (i = 0; i < inputc; i++)
  {
   if (PointInside (x, y, input[i]))
    {
     switch (input[i].id)
      {
      case I_LCD:
       tsc_XPT2046_set_pos (&touch, -1, -1);
       active[0] = 0;
       break;
      }
    }
  }
}

void
cpart_LCD_ili9341::EvMouseMove(uint button, uint x, uint y, uint state)
{
 int i;
 
 for (i = 0; i < inputc; i++)
  {
   if (PointInside (x, y, input[i]))
    {

     RotateCoords (&x, &y);

     if (active[input[i].id - I_LCD])
      {
       tsc_XPT2046_set_pos (&touch, x - input[i].x1, y - input[i].y1);
      }
    }
  }
}

void
cpart_LCD_ili9341::Reset(void)
{
 lcd_ili9341_rst (&lcd);
 tsc_XPT2046_rst (&touch);
}

void
cpart_LCD_ili9341::SetOrientation(int _orientation)
{
 part::SetOrientation (_orientation);
 lcd_ili9341_update(&lcd);
}

void
cpart_LCD_ili9341::SetScale(double scale)
{
 part::SetScale (scale);
 lcd_ili9341_update(&lcd);
}

part_init("LCD ili9341", cpart_LCD_ili9341, "Output");

