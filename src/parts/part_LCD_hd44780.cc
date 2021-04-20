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
#include"part_LCD_hd44780.h"

/* outputs */
enum
{
 O_P1, O_P2, O_P3, O_P4, O_P5, O_P6, O_P7, O_P8, O_P9, O_P10, O_P11, O_F1, O_F2, O_F3, O_LCD
};

void
cpart_LCD_hd44780::InitGraphics(void)
{
 lxImage image (&Window5);

 switch (model)
  {
  case LCD16x2:
   image.LoadFile (Window1.GetSharePath () + lxT ("parts/") + GetPictureFileName (), Orientation, Scale, Scale);
   break;
  case LCD16x4:
   image.LoadFile (Window1.GetSharePath () + lxT ("parts/") + GetPictureFileName_ (), Orientation, Scale, Scale);
   break;
  case LCD20x2:
   image.LoadFile (Window1.GetSharePath () + lxT ("parts/") + GetPictureFileName__ (), Orientation, Scale, Scale);
   break;
  case LCD20x4:
   image.LoadFile (Window1.GetSharePath () + lxT ("parts/") + GetPictureFileName___ (), Orientation, Scale, Scale);
   break;
  }

 if (Bitmap)delete Bitmap;

 Bitmap = new lxBitmap (&image, &Window5);
 canvas.Destroy ();
 canvas.Create (Window5.GetWWidget (), Bitmap);
 image.Destroy ();
}

void
cpart_LCD_hd44780::Reset(void)
{
 InitGraphics ();

 switch (model)
  {
  case LCD16x2:
   lcd_init (&lcd, 16, 2);
   break;
  case LCD16x4:
   lcd_init (&lcd, 16, 4);
   break;
  case LCD20x2:
   lcd_init (&lcd, 20, 2);
   break;
  case LCD20x4:
   lcd_init (&lcd, 20, 4);
   break;
  }
 lcd_rst (&lcd);
 lcde = 0;
}

cpart_LCD_hd44780::cpart_LCD_hd44780(unsigned x, unsigned y):
font (8, lxFONTFAMILY_TELETYPE, lxFONTSTYLE_NORMAL, lxFONTWEIGHT_BOLD)   
{
 X = x;
 Y = y;
 ReadMaps ();
 Bitmap = NULL;

 model = LCD16x2;
 Reset ();

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

};

cpart_LCD_hd44780::~cpart_LCD_hd44780(void)
{
 delete Bitmap;
 canvas.Destroy ();
}

void
cpart_LCD_hd44780::Draw(void)
{

 int i;

 int yoff = 0;

 if ((model == LCD16x4) || (model == LCD20x4)) yoff = 96;

 canvas.Init (Scale, Scale, Orientation);

 lcd_blink (&lcd);

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
     canvas.SetColor (49, 61, 99);
     canvas.Rectangle (1, output[i].x1, output[i].y1 + yoff, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);
     canvas.SetFgColor (255, 255, 255);
     if (input_pins[output[i].id - O_P1] == 0)
      canvas.RotatedText ("NC", output[i].x1, output[i].y2 + yoff, 90.0);
     else
      canvas.RotatedText (Window5.GetPinName (input_pins[output[i].id - O_P1]), output[i].x1, output[i].y2 + yoff, 90.0);
     break;
    case O_F1:
     canvas.SetColor (49, 61, 99);
     canvas.Rectangle (1, output[i].x1, output[i].y1 + yoff, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);
     canvas.SetFgColor (155, 155, 155);
     canvas.RotatedText ("GND", output[i].x1, output[i].y2 + yoff, 90.0);
     break;
    case O_F2:
     canvas.SetColor (49, 61, 99);
     canvas.Rectangle (1, output[i].x1, output[i].y1 + yoff, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);
     canvas.SetFgColor (155, 155, 155);
     canvas.RotatedText ("+5V", output[i].x1, output[i].y2 + yoff, 90.0);
     break;
    case O_F3:
     canvas.SetColor (49, 61, 99);
     canvas.Rectangle (1, output[i].x1, output[i].y1 + yoff, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);
     canvas.SetFgColor (155, 155, 155);
     canvas.RotatedText ("POT", output[i].x1, output[i].y2 + yoff, 90.0);
     break;
    case O_LCD:
     //draw lcd text 
     if (lcd.update)
      {
       canvas.SetColor (0, 90 + 40, 0);
       lcd_draw (&lcd, &canvas, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1, 1);
      }
     /*
     else   
     {
        canvas.Rectangle (1, output[i].x1, output[i].y1, output[i].x2-output[i].x1,output[i].y2-output[i].y1 );
     }
      */
     break;
    }


  };


 canvas.End ();

}

unsigned short
cpart_LCD_hd44780::get_in_id(char * name)
{
 printf ("Erro input '%s' don't have a valid id! \n", name);
 return -1;
};

unsigned short
cpart_LCD_hd44780::get_out_id(char * name)
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

 if (strcmp (name, "PN_F1") == 0)return O_F1;
 if (strcmp (name, "PN_F2") == 0)return O_F2;
 if (strcmp (name, "PN_F3") == 0)return O_F3;

 if (strcmp (name, "DS_LCD") == 0)return O_LCD;

 printf ("Erro output '%s' don't have a valid id! \n", name);
 return 1;
};

lxString
cpart_LCD_hd44780::WritePreferences(void)
{
 char prefs[256];

 sprintf (prefs, "%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu", input_pins[0], input_pins[1], input_pins[2], input_pins[3], input_pins[4], input_pins[5], input_pins[6], input_pins[7], input_pins[8], input_pins[9], input_pins[10], model);

 return prefs;
}

void
cpart_LCD_hd44780::ReadPreferences(lxString value)
{
 sscanf (value.c_str (), "%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu", &input_pins[0], &input_pins[1], &input_pins[2], &input_pins[3], &input_pins[4], &input_pins[5], &input_pins[6], &input_pins[7], &input_pins[8], &input_pins[9], &input_pins[10], &model);
 Reset ();
 RegisterRemoteControl ();
}

void
cpart_LCD_hd44780::RegisterRemoteControl(void)
{
 for (int i = 0; i < outputc; i++)
  {
   switch (output[i].id)
    {
    case O_LCD:
     output[i].status = (void *) &lcd;
     break;
    }
  }
}

void
cpart_LCD_hd44780::ConfigurePropertiesWindow(CPWindow * WProp)
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

 ((CCombo*) WProp->GetChildByName ("combo12"))->SetItems (Items);
 if (input_pins[10] == 0)
  ((CCombo*) WProp->GetChildByName ("combo12"))->SetText ("0  NC");
 else
  {
   spin = Window5.GetPinName (input_pins[10]);
   ((CCombo*) WProp->GetChildByName ("combo12"))->SetText (itoa (input_pins[10]) + "  " + spin);
  }

 switch (model)
  {
  case LCD16x2:
   ((CCombo*) WProp->GetChildByName ("combo11"))->SetText ("16x2");
   break;
  case LCD16x4:
   ((CCombo*) WProp->GetChildByName ("combo11"))->SetText ("16x4");
   break;
  case LCD20x2:
   ((CCombo*) WProp->GetChildByName ("combo11"))->SetText ("20x2");
   break;
  case LCD20x4:
   ((CCombo*) WProp->GetChildByName ("combo11"))->SetText ("20x4");
   break;
  }

 ((CButton*) WProp->GetChildByName ("button1"))->EvMouseButtonRelease = EVMOUSEBUTTONRELEASE & CPWindow5::PropButtonRelease;
 ((CButton*) WProp->GetChildByName ("button1"))->SetTag (1);

 ((CButton*) WProp->GetChildByName ("button2"))->EvMouseButtonRelease = EVMOUSEBUTTONRELEASE & CPWindow5::PropButtonRelease;
}

void
cpart_LCD_hd44780::ReadPropertiesWindow(CPWindow * WProp)
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
 input_pins[10] = atoi (((CCombo*) WProp->GetChildByName ("combo12"))->GetText ());

 model = LCD16x2;
 if (!((((CCombo*) WProp->GetChildByName ("combo11"))->GetText ()).Cmp (lxT ("16x4"))))
  model = LCD16x4;
 else if (!((((CCombo*) WProp->GetChildByName ("combo11"))->GetText ()).Cmp (lxT ("20x2"))))
  model = LCD20x2;
 else if (!((((CCombo*) WProp->GetChildByName ("combo11"))->GetText ()).Cmp (lxT ("20x4"))))
  model = LCD20x4;


 Reset ();

 RegisterRemoteControl ();
}

void
cpart_LCD_hd44780::Process(void)
{
 const picpin * ppins = Window5.GetPinsValues ();


 //lcd dipins[2].display code


 if ((input_pins[1] > 0) &&(!ppins[input_pins[1] - 1].value))
  {
   if (!lcde)
    {
     if (input_pins[10] > 0)
      {
       if (!ppins[input_pins[10] - 1].value)
        {
         //Write
         unsigned char d = 0;
         if ((input_pins[9] > 0)&&(ppins[input_pins[9] - 1].value)) d |= 0x80;
         if ((input_pins[8] > 0)&&(ppins[input_pins[8] - 1].value)) d |= 0x40;
         if ((input_pins[7] > 0)&&(ppins[input_pins[7] - 1].value)) d |= 0x20;
         if ((input_pins[6] > 0)&&(ppins[input_pins[6] - 1].value)) d |= 0x10;
         if ((input_pins[5] > 0)&&(ppins[input_pins[5] - 1].value)) d |= 0x08;
         if ((input_pins[4] > 0)&&(ppins[input_pins[4] - 1].value)) d |= 0x04;
         if ((input_pins[3] > 0)&&(ppins[input_pins[3] - 1].value)) d |= 0x02;
         if ((input_pins[2] > 0)&&(ppins[input_pins[2] - 1].value)) d |= 0x01;

         if (!ppins[input_pins[0] - 1].value)
          {
           lcd_cmd (&lcd, d);
          }
         else
          {
           lcd_data (&lcd, d);
          }
        }
       else //Read
        {
         unsigned char val = 0;
         if (!ppins[input_pins[0] - 1].value)
          {
           val = lcd_read_busyf_acounter (&lcd);
          }
         else
          {
           val = lcd_read_data (&lcd);
          }

         if (input_pins[9] > 0)Window5.SetPin (input_pins[9], (val & 0x80) > 0);
         if (input_pins[8] > 0)Window5.SetPin (input_pins[8], (val & 0x40) > 0);
         if (input_pins[7] > 0)Window5.SetPin (input_pins[7], (val & 0x20) > 0);
         if (input_pins[6] > 0)Window5.SetPin (input_pins[6], (val & 0x10) > 0);
         if (input_pins[5] > 0)Window5.SetPin (input_pins[5], (val & 0x08) > 0);
         if (input_pins[4] > 0)Window5.SetPin (input_pins[4], (val & 0x04) > 0);
         if (input_pins[3] > 0)Window5.SetPin (input_pins[3], (val & 0x02) > 0);
         if (input_pins[2] > 0)Window5.SetPin (input_pins[2], (val & 0x01) > 0);
        }
      }
     lcde = 1;
    }
  }
 else
  {
   lcde = 0;
  }
 //end display code

}

void
cpart_LCD_hd44780::SetOrientation(int _orientation)
{
 part::SetOrientation (_orientation);
 InitGraphics ();
 lcd.update=1;
}

void
cpart_LCD_hd44780::SetScale(double scale)
{
 part::SetScale (scale);
 InitGraphics ();
 lcd.update=1;
}

part_init("LCD hd44780", cpart_LCD_hd44780, "Output");

