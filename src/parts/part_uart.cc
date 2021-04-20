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
#include"part_uart.h"

/* outputs */
enum
{
 O_RX, O_TX, O_FILE, O_LCON, O_LTX, O_LRX
};

/* inputs */
enum
{
 I_CONN
};

/*
const char pin_names[10][10] = {
 "GND",
 "VCC",
 "MISO",
 "MOSI",
 "SCK",
 "CS"
};

const char pin_values[10][10] = {
 "GND",
 "+5V",
 {0},
 {1},
 {2},
 {3}
};
 */


cpart_UART::cpart_UART(unsigned x, unsigned y):
font (8, lxFONTFAMILY_TELETYPE, lxFONTSTYLE_NORMAL, lxFONTWEIGHT_BOLD)
{
 X = x;
 Y = y;
 ReadMaps ();
 Bitmap = NULL;

 lxImage image(&Window5);

 image.LoadFile (Window1.GetSharePath () + lxT ("parts/") + GetPictureFileName (), Orientation, Scale, Scale);


 Bitmap = new lxBitmap (&image, &Window5);
 image.Destroy ();
 canvas.Create (Window5.GetWWidget (), Bitmap);

 uart_init (&sr);
 uart_rst (&sr);

 input_pins[0] = 0;

 output_pins[0] = 0;

 _ret = -1;

 uart_name[0] = '*';
 uart_name[1] = 0;
 uart_speed = 9600;
}

cpart_UART::~cpart_UART(void)
{
 delete Bitmap;
 canvas.Destroy ();
 uart_end (&sr);
}

void
cpart_UART::Reset(void)
{
 uart_rst (&sr);
}

void
cpart_UART::Draw(void)
{

 int i;

 //const picpin * ppins = Window5.GetPinsValues ();

 canvas.Init (Scale, Scale, Orientation);

 canvas.SetFont (font);

 for (i = 0; i < outputc; i++)
  {

   switch (output[i].id)
    {
    case O_LTX:
     canvas.SetColor (0, (sr.bb_uart.leds & 0x02)*125, 0);
     canvas.Rectangle (1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);
     sr.bb_uart.leds &= ~0x02;
     break;
    case O_LRX:
     canvas.SetColor (0, (sr.bb_uart.leds & 0x01)*250, 0);
     canvas.Rectangle (1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);
     sr.bb_uart.leds &= ~0x01;
     break;
    case O_LCON:
     canvas.SetColor (255, 0, 0);
     canvas.Rectangle (1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);
     break;
    case O_FILE:
     canvas.SetColor (49, 61, 99);
     canvas.Rectangle (1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);
     canvas.SetFgColor (255, 255, 255);
     canvas.RotatedText (lxT ("port:") + lxString (uart_name) + lxT ("   speed:") + itoa (uart_speed), output[i].x1, output[i].y1, 0);
     break;
    default:
     canvas.SetColor (49, 61, 99);
     canvas.Rectangle (1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);

     canvas.SetFgColor (155, 155, 155);

     int pinv = output[i].id - O_RX;
     int pin = 0;
     switch (pinv)
      {
      case 0:
       pin = pinv;
       if (input_pins[pin] == 0)
        canvas.RotatedText ("NC", output[i].x1, output[i].y2, 90.0);
       else
        canvas.RotatedText (Window5.GetPinName (input_pins[pin]), output[i].x1, output[i].y2, 90.0);
      case 1:
       pin = pinv - 1;
       if (output_pins[pin] == 0)
        canvas.RotatedText ("NC", output[i].x1, output[i].y2, 90.0);
       else
        canvas.RotatedText (Window5.GetPinName (output_pins[pin]), output[i].x1, output[i].y2, 90.0);
       break;

      }
     break;
    }

  }

 canvas.End ();

}

unsigned short
cpart_UART::get_in_id(char * name)
{
 if (strcmp (name, "CN_CONN") == 0)return I_CONN;

 printf ("Erro input '%s' don't have a valid id! \n", name);
 return -1;
}

unsigned short
cpart_UART::get_out_id(char * name)
{

 if (strcmp (name, "PN_RX") == 0)return O_RX;
 if (strcmp (name, "PN_TX") == 0)return O_TX;
 if (strcmp (name, "LD_CON") == 0)return O_LCON;
 if (strcmp (name, "LD_TX") == 0)return O_LTX;
 if (strcmp (name, "LD_RX") == 0)return O_LRX;
 if (strcmp (name, "DI_FILE") == 0)return O_FILE;


 printf ("Erro output '%s' don't have a valid id! \n", name);
 return 1;
}

lxString
cpart_UART::WritePreferences(void)
{
 char prefs[256];

 sprintf (prefs, "%hhu,%hhu,%u,%s", input_pins[0], output_pins[0], uart_speed, uart_name);

 return prefs;
}

void
cpart_UART::ReadPreferences(lxString value)
{
 sscanf (value.c_str (), "%hhu,%hhu,%u,%s", &input_pins[0], &output_pins[0], &uart_speed, uart_name);

 Reset ();
 if (uart_name[0] != '*')
  {
   uart_set_port (&sr, uart_name, uart_speed);
  }
}

void
cpart_UART::ConfigurePropertiesWindow(CPWindow * WProp)
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
 if (output_pins[0] == 0)
  ((CCombo*) WProp->GetChildByName ("combo2"))->SetText ("0  NC");
 else
  {
   spin = Window5.GetPinName (output_pins[0]);
   ((CCombo*) WProp->GetChildByName ("combo2"))->SetText (itoa (output_pins[0]) + "  " + spin);
  }

 char * resp = serial_port_list ();
 if (resp)
  {
   ((CCombo*) WProp->GetChildByName ("combo3"))->SetItems (resp);
   free (resp);
  }
 if (uart_name[0] == '*')
  ((CCombo*) WProp->GetChildByName ("combo3"))->SetText (" ");
 else
  {
   ((CCombo*) WProp->GetChildByName ("combo3"))->SetText (uart_name);
  }


 ((CCombo*) WProp->GetChildByName ("combo4"))->SetItems ("1200,2400,4800,9600,19200,38400,57600,115200,");
 ((CCombo*) WProp->GetChildByName ("combo4"))->SetText (itoa (uart_speed));


 ((CButton*) WProp->GetChildByName ("button1"))->EvMouseButtonRelease = EVMOUSEBUTTONRELEASE & CPWindow5::PropButtonRelease;
 ((CButton*) WProp->GetChildByName ("button1"))->SetTag (1);

 ((CButton*) WProp->GetChildByName ("button2"))->EvMouseButtonRelease = EVMOUSEBUTTONRELEASE & CPWindow5::PropButtonRelease;
}

void
cpart_UART::ReadPropertiesWindow(CPWindow * WProp)
{
 input_pins[0] = atoi (((CCombo*) WProp->GetChildByName ("combo1"))->GetText ());
 output_pins [0] = atoi (((CCombo*) WProp->GetChildByName ("combo2"))->GetText ());
 strncpy (uart_name, (((CCombo*) WProp->GetChildByName ("combo3"))->GetText ()).c_str (), 199);
 uart_speed = atoi (((CCombo*) WProp->GetChildByName ("combo4"))->GetText ());

 uart_set_port (&sr, uart_name, uart_speed);
}

void
cpart_UART::PreProcess(void)
{
 uart_set_clk_freq (&sr, Window1.GetBoard ()->MGetInstClockFreq ());
}

void
cpart_UART::Process(void)
{
 const picpin * ppins = Window5.GetPinsValues ();

 unsigned short ret = 0;

 unsigned char val;

 if (input_pins[0])
  {
   val = ppins[input_pins[0] - 1].value;
  }
 else
  {
   val = 1;
  }
 ret = uart_io (&sr, val);

 if (_ret != ret)
  {
   Window5.SetPin (output_pins[0], ret);
  }
 _ret = ret;

}

void
cpart_UART::EvMouseButtonPress(uint button, uint x, uint y, uint state)
{
 int i;

 for (i = 0; i < inputc; i++)
  {
   if (PointInside (x, y, input[i]))
    {
     switch (input[i].id)
      {
      case I_CONN:

       break;
      }
    }
  }
}

void
cpart_UART::PostProcess(void) { }

part_init("IO UART", cpart_UART, "Other");

