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
#include"part_LCD_ssd1306.h"

/* outputs */
enum
{
 O_P1, O_P2, O_P3, O_P4, O_P5, O_F1, O_F2, O_LCD
};

cpart_LCD_ssd1306::cpart_LCD_ssd1306(unsigned x, unsigned y) :
font(8, lxFONTFAMILY_TELETYPE, lxFONTSTYLE_NORMAL, lxFONTWEIGHT_BOLD)
{
 X = x;
 Y = y;
 ReadMaps ();
 Bitmap = NULL;

 lxImage image (&Window5);

 image.LoadFile (lxGetLocalFile (Window1.GetSharePath () + lxT ("parts/") + GetPictureFileName ()), Orientation, Scale, Scale);

 Bitmap = new lxBitmap (&image, &Window5);
 image.Destroy ();
 canvas.Create (Window5.GetWWidget (), Bitmap);

 lcd_ssd1306_init (&lcd);
 lcd_ssd1306_rst (&lcd);

 input_pins[0] = 0;
 input_pins[1] = 0;
 input_pins[2] = 0;
 input_pins[3] = 0;
 input_pins[4] = 0;

 type_com = 0; //SPI
};

cpart_LCD_ssd1306::~cpart_LCD_ssd1306(void)
{
 delete Bitmap;
 canvas.Destroy ();
}

void
cpart_LCD_ssd1306::Draw(void)
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
       canvas.SetColor (49, 61, 99);
       canvas.Rectangle (1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);
       canvas.SetFgColor (255, 255, 255);
       if (input_pins[output[i].id - O_P1] == 0)
        canvas.RotatedText ("NC", output[i].x1, output[i].y2, 90.0);
       else
        canvas.RotatedText (Window5.GetPinName (input_pins[output[i].id - O_P1]), output[i].x1, output[i].y2, 90.0);
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
         canvas.SetColor (0, 90 + 40, 0);
         lcd_ssd1306_draw (&lcd, &canvas, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1, 1);
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
  }

 if (Update)
  {
   canvas.End ();
  }
}

unsigned short
cpart_LCD_ssd1306::get_in_id(char * name)
{
 printf ("Erro input '%s' don't have a valid id! \n", name);
 return -1;
};

unsigned short
cpart_LCD_ssd1306::get_out_id(char * name)
{

 if (strcmp (name, "PN_1") == 0)return O_P1;
 if (strcmp (name, "PN_2") == 0)return O_P2;
 if (strcmp (name, "PN_3") == 0)return O_P3;
 if (strcmp (name, "PN_4") == 0)return O_P4;
 if (strcmp (name, "PN_5") == 0)return O_P5;

 if (strcmp (name, "PN_F1") == 0)return O_F1;
 if (strcmp (name, "PN_F2") == 0)return O_F2;

 if (strcmp (name, "DS_LCD") == 0)return O_LCD;

 printf ("Erro output '%s' don't have a valid id! \n", name);
 return 1;
};

lxString
cpart_LCD_ssd1306::WritePreferences(void)
{
 char prefs[256];

 sprintf (prefs, "%hhu,%hhu,%hhu,%hhu,%hhu,%hhu", input_pins[0], input_pins[1], input_pins[2], input_pins[3], input_pins[4], type_com);
 return prefs;
}

void
cpart_LCD_ssd1306::ReadPreferences(lxString value)
{
 sscanf (value.c_str (), "%hhu,%hhu,%hhu,%hhu,%hhu,%hhu", &input_pins[0], &input_pins[1], &input_pins[2], &input_pins[3], &input_pins[4], &type_com);
 Reset ();
}

void
cpart_LCD_ssd1306::ConfigurePropertiesWindow(CPWindow * WProp)
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

 if (!type_com)
  {
   ((CCombo*) WProp->GetChildByName ("combo6"))->SetText ("SPI");
  }
 else
  {
   ((CCombo*) WProp->GetChildByName ("combo6"))->SetText ("I2C");
  }


 ((CButton*) WProp->GetChildByName ("button1"))->EvMouseButtonRelease = EVMOUSEBUTTONRELEASE & CPWindow5::PropButtonRelease;
 ((CButton*) WProp->GetChildByName ("button1"))->SetTag (1);

 ((CButton*) WProp->GetChildByName ("button2"))->EvMouseButtonRelease = EVMOUSEBUTTONRELEASE & CPWindow5::PropButtonRelease;
}

void
cpart_LCD_ssd1306::ReadPropertiesWindow(CPWindow * WProp)
{
 input_pins[0] = atoi (((CCombo*) WProp->GetChildByName ("combo1"))->GetText ());
 input_pins[1] = atoi (((CCombo*) WProp->GetChildByName ("combo2"))->GetText ());
 input_pins[2] = atoi (((CCombo*) WProp->GetChildByName ("combo3"))->GetText ());
 input_pins[3] = atoi (((CCombo*) WProp->GetChildByName ("combo4"))->GetText ());
 input_pins[4] = atoi (((CCombo*) WProp->GetChildByName ("combo5"))->GetText ());

 if (!((CCombo*) WProp->GetChildByName ("combo6"))->GetText ().Cmp ("SPI"))
  {
   type_com = 0;
  }
 else
  {
   type_com = 1;
  }

}

void
cpart_LCD_ssd1306::PreProcess(void)
{
 if (input_pins[1] > 0)
  {
   Window5.Reset_i2c_bus (input_pins[1] - 1);
  }
}

void
cpart_LCD_ssd1306::Process(void)
{
 const picpin * ppins = Window5.GetPinsValues ();

 if (type_com)
  {
   if ((input_pins[0] > 0)&&(input_pins[1] > 0))
    Window5.Set_i2c_bus (input_pins[1] - 1, lcd_ssd1306_I2C_io (&lcd, ppins[input_pins[1] - 1].value, ppins[input_pins[0] - 1].value));

   if (input_pins[1] > 0)
    Window5.SetPin (input_pins[1], Window5.Get_i2c_bus (input_pins[1] - 1));
  }
 else
  {
   if ((input_pins[0] > 0)
       &&(input_pins[1] > 0)
       &&(input_pins[2] > 0)
       &&(input_pins[3] > 0)
       &&(input_pins[4] > 0))
    {
     lcd_ssd1306_SPI_io (&lcd, ppins[input_pins[1] - 1].value, ppins[input_pins[0] - 1].value, ppins[input_pins[4] - 1].value, ppins[input_pins[2] - 1].value, ppins[input_pins[3] - 1].value);
    }
  }
}

void
cpart_LCD_ssd1306::PostProcess(void)
{
 if (lcd.update)output_ids[O_LCD]->update = 1;
}

void
cpart_LCD_ssd1306::SetOrientation(int _orientation)
{
 part::SetOrientation (_orientation);
 lcd_ssd1306_update (&lcd);
}

void
cpart_LCD_ssd1306::SetScale(double scale)
{
 part::SetScale (scale);
 lcd_ssd1306_update (&lcd);
}

part_init(PART_LCD_SSD1306_Name, cpart_LCD_ssd1306, "Output");

