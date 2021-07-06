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
#include"part_ETH_w5500.h"

/* outputs */
enum
{
 O_P3, O_P5, O_P6, O_P7, O_P8, O_P9, O_LPWR, O_LLINK, O_LACT, O_STAT
};

/* inputs */
enum
{
 I_CONN, I_STAT
};

/*
const char pin_names[10][10] = {
 "3.3V",
 "5V",
 "MISO",
 "GND",
 "MOSI",
 "RST",
 "SCS",
 "INT",
 "SCLK",
 "NC"
};

const char pin_values[10][10] = {
 "+3.3V",
 "+5V",
 {4},
 "GND",
 {0},
 {1},
 {2},
 {5},
 {3},
 "NC"
};
 */


cpart_ETH_w5500::cpart_ETH_w5500(unsigned x, unsigned y) :
font(8, lxFONTFAMILY_TELETYPE, lxFONTSTYLE_NORMAL, lxFONTWEIGHT_BOLD)
{
 X = x;
 Y = y;
 ReadMaps ();
 Bitmap = NULL;

 lxImage image (&Window5);

 image.LoadFile (lxGetLocalFile(Window1.GetSharePath () + lxT ("parts/") + GetPictureFileName ()), Orientation, Scale, Scale);


 Bitmap = new lxBitmap (&image, &Window5);
 image.Destroy ();
 canvas.Create (Window5.GetWWidget (), Bitmap);

 eth_w5500_init (&ethw);
 eth_w5500_rst (&ethw);

 input_pins[0] = 0;
 input_pins[1] = 0;
 input_pins[2] = 0;
 input_pins[3] = 0;

 output_pins[0] = 0;
 output_pins[1] = 0;

 _ret = -1;

 link = 1;
}

cpart_ETH_w5500::~cpart_ETH_w5500(void)
{
 delete Bitmap;
 canvas.Destroy ();
 eth_w5500_end (&ethw);
}

void
cpart_ETH_w5500::Reset(void)
{
 eth_w5500_rst (&ethw);
}

void
cpart_ETH_w5500::Draw(void)
{

 int i;
 int c;
 int n;
 char status[10];
 char sport[10];

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
      case O_LPWR:
       canvas.SetColor (255, 0, 0);
       canvas.Rectangle (1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);
       break;
      case O_LACT:
       c = 255 * ((eth_w5500_get_leds (&ethw) & 0x08) > 0);
       canvas.SetColor (c, c, 0);
       canvas.Rectangle (1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);
       break;
      case O_LLINK:
       canvas.SetColor (0, 255 * (eth_w5500_get_leds (&ethw) & 0x01), 0);
       canvas.Rectangle (1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);
       break;
      case O_STAT:
       canvas.SetColor (9, 21, 59);
       canvas.Rectangle (1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);

       for (n = 0; n < 8; n++)
        {
         canvas.SetFgColor (155, 155, 155);
         memset (status, 0, 10);
         status[0] = '1' + n;
         switch (ethw.Socket[n][Sn_MR] & 0x0F)
          {
          case Sn_MR_CLOSE:
           status[1] = 'C';
           break;
          case Sn_MR_TCP:
           status[1] = 'T';
           break;
          case Sn_MR_UDP:
           status[1] = 'U';
           break;
          case S0_MR_MACRAW:
           status[1] = 'M';
           canvas.SetFgColor (255, 0, 0);
           break;
          }

         switch (ethw.Socket[n][Sn_SR])
          {
          case SOCK_CLOSED:
           status[2] = 'C';
           break;
          case SOCK_INIT:
           status[2] = 'I';
           canvas.SetFgColor (255, 255, 255);
           break;
          case SOCK_LISTEN:
           status[2] = 'L';
           canvas.SetFgColor (255, 255, 255);
           break;
          case SOCK_SYNSENT:
           status[2] = 'S';
           canvas.SetFgColor (255, 255, 255);
           break;
          case SOCK_ESTABLISHED:
           status[2] = 'E';
           canvas.SetFgColor (255, 255, 255);
           break;
          case SOCK_CLOSE_WAIT:
           status[2] = 'W';
           canvas.SetFgColor (255, 255, 255);
           break;
          case SOCK_UDP:
           status[2] = 'U';
           canvas.SetFgColor (0, 255, 0);
           break;
          case SOCK_MACRAW:
           status[2] = 'M';
           canvas.SetFgColor (255, 0, 0);
           break;
          }

         switch (ethw.status[n])
          {
          case ER_BIND:
           status[3] = 'B';
           canvas.SetFgColor (255, 0, 0);
           break;
          case ER_SEND:
           status[3] = 'S';
           canvas.SetFgColor (255, 0, 0);
           break;
          case ER_RECV:
           status[3] = 'R';
           canvas.SetFgColor (255, 0, 0);
           break;
          case ER_LIST:
           status[3] = 'L';
           canvas.SetFgColor (255, 0, 0);
           break;
          case ER_REUSE:
           status[3] = 'U';
           canvas.SetFgColor (255, 0, 0);
           break;
          case ER_CONN:
           status[3] = 'C';
           canvas.SetFgColor (255, 0, 0);
           break;
          case ER_SHUT:
           status[3] = 'D';
           canvas.SetFgColor (255, 0, 0);
           break;
          default:
           status[3] = ' ';
           break;
          }

         sprintf (sport, "%i", ethw.bindp[n]);
         if (n < 4)
          {
           canvas.RotatedText (status, output[i].x1 + (n * 30), output[i].y1, 0);
           canvas.RotatedText (sport, output[i].x1 + (n * 35), output[i].y1 + 30, 0);
          }
         else
          {
           canvas.RotatedText (status, output[i].x1 + ((n - 4) * 30), output[i].y1 + 15, 0);
           canvas.RotatedText (sport, output[i].x1 + ((n - 4) * 35), output[i].y1 + 45, 0);
          }
        }
       break;
      default:
       canvas.SetColor (49, 61, 99);
       canvas.Rectangle (1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);

       canvas.SetFgColor (155, 155, 155);

       int pinv = output[i].id - O_P3;
       int pin = 0;
       switch (pinv)
        {
        case 0:
        case 4:
         pin = pinv > 1;
         if (output_pins[pin] == 0)
          canvas.RotatedText ("NC", output[i].x1, output[i].y2, 90.0);
         else
          canvas.RotatedText (Window5.GetPinName (output_pins[pin]), output[i].x1, output[i].y2, 90.0);
         break;
        case 1:
        case 2:
        case 3:
         pinv++;
        case 5:
         pin = pinv - 2;
         if (input_pins[pin] == 0)
          canvas.RotatedText ("NC", output[i].x1, output[i].y2, 90.0);
         else
          canvas.RotatedText (Window5.GetPinName (input_pins[pin]), output[i].x1, output[i].y2, 90.0);
        }
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
cpart_ETH_w5500::get_in_id(char * name)
{
 if (strcmp (name, "CN_CONN") == 0)return I_CONN;
 if (strcmp (name, "ST_STAT") == 0)return I_STAT;

 printf ("Erro input '%s' don't have a valid id! \n", name);
 return -1;
}

unsigned short
cpart_ETH_w5500::get_out_id(char * name)
{

 if (strcmp (name, "PN_3") == 0)return O_P3;
 if (strcmp (name, "PN_5") == 0)return O_P5;
 if (strcmp (name, "PN_6") == 0)return O_P6;
 if (strcmp (name, "PN_7") == 0)return O_P7;
 if (strcmp (name, "PN_8") == 0)return O_P8;
 if (strcmp (name, "PN_9") == 0)return O_P9;
 if (strcmp (name, "LD_PWR") == 0)return O_LPWR;
 if (strcmp (name, "LD_LINK") == 0)return O_LLINK;
 if (strcmp (name, "LD_ACT") == 0)return O_LACT;
 if (strcmp (name, "ST_STAT") == 0)return O_STAT;

 printf ("Erro output '%s' don't have a valid id! \n", name);
 return 1;
}

lxString
cpart_ETH_w5500::WritePreferences(void)
{
 char prefs[256];

 sprintf (prefs, "%hhu,%hhu,%hhu,%hhu,%hhu,%hhu", input_pins[0], input_pins[1], input_pins[2], input_pins[3], output_pins[0], output_pins[1]);

 return prefs;
}

void
cpart_ETH_w5500::ReadPreferences(lxString value)
{
 sscanf (value.c_str (), "%hhu,%hhu,%hhu,%hhu,%hhu,%hhu", &input_pins[0], &input_pins[1], &input_pins[2], &input_pins[3], &output_pins[0], &output_pins[1]);

 Reset ();
 RegisterRemoteControl ();
}

void
cpart_ETH_w5500::RegisterRemoteControl(void)
{
 for (int i = 0; i < outputc; i++)
  {
   switch (output[i].id)
    {
    case O_STAT:
     output[i].status = (void *) &ethw;
     break;
    }
  }
}

void
cpart_ETH_w5500::ConfigurePropertiesWindow(CPWindow * WProp)
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
 if (input_pins[0] == 0)
  ((CCombo*) WProp->GetChildByName ("combo2"))->SetText ("0  NC");
 else
  {
   spin = Window5.GetPinName (input_pins[0]);
   ((CCombo*) WProp->GetChildByName ("combo2"))->SetText (itoa (input_pins[0]) + "  " + spin);
  }

 ((CCombo*) WProp->GetChildByName ("combo3"))->SetItems (Items);
 if (input_pins[1] == 0)
  ((CCombo*) WProp->GetChildByName ("combo3"))->SetText ("0  NC");
 else
  {
   spin = Window5.GetPinName (input_pins[1]);
   ((CCombo*) WProp->GetChildByName ("combo3"))->SetText (itoa (input_pins[1]) + "  " + spin);
  }


 ((CCombo*) WProp->GetChildByName ("combo4"))->SetItems (Items);
 if (input_pins[2] == 0)
  ((CCombo*) WProp->GetChildByName ("combo4"))->SetText ("0  NC");
 else
  {

   spin = Window5.GetPinName (input_pins[2]);
   ((CCombo*) WProp->GetChildByName ("combo4"))->SetText (itoa (input_pins[2]) + "  " + spin);
  }

 ((CCombo*) WProp->GetChildByName ("combo5"))->SetItems (Items);
 if (output_pins[1] == 0)
  ((CCombo*) WProp->GetChildByName ("combo5"))->SetText ("0  NC");
 else
  {
   spin = Window5.GetPinName (output_pins[1]);
   ((CCombo*) WProp->GetChildByName ("combo5"))->SetText (itoa (output_pins[1]) + "  " + spin);
  }

 ((CCombo*) WProp->GetChildByName ("combo6"))->SetItems (Items);
 if (input_pins[3] == 0)
  ((CCombo*) WProp->GetChildByName ("combo6"))->SetText ("0  NC");
 else
  {

   spin = Window5.GetPinName (input_pins[3]);
   ((CCombo*) WProp->GetChildByName ("combo6"))->SetText (itoa (input_pins[3]) + "  " + spin);
  }


 ((CButton*) WProp->GetChildByName ("button1"))->EvMouseButtonRelease = EVMOUSEBUTTONRELEASE & CPWindow5::PropButtonRelease;
 ((CButton*) WProp->GetChildByName ("button1"))->SetTag (1);

 ((CButton*) WProp->GetChildByName ("button2"))->EvMouseButtonRelease = EVMOUSEBUTTONRELEASE & CPWindow5::PropButtonRelease;
}

void
cpart_ETH_w5500::ReadPropertiesWindow(CPWindow * WProp)
{
 output_pins[0] = atoi (((CCombo*) WProp->GetChildByName ("combo1"))->GetText ());
 input_pins [0] = atoi (((CCombo*) WProp->GetChildByName ("combo2"))->GetText ());
 input_pins [1] = atoi (((CCombo*) WProp->GetChildByName ("combo3"))->GetText ());
 input_pins [2] = atoi (((CCombo*) WProp->GetChildByName ("combo4"))->GetText ());
 output_pins[1] = atoi (((CCombo*) WProp->GetChildByName ("combo5"))->GetText ());
 input_pins [3] = atoi (((CCombo*) WProp->GetChildByName ("combo6"))->GetText ());

 RegisterRemoteControl ();
}

void
cpart_ETH_w5500::PreProcess(void)
{

 eth_w5500_process (&ethw);
}

void
cpart_ETH_w5500::Process(void)
{
 const picpin * ppins = Window5.GetPinsValues ();

 unsigned short ret = 0;

 ret = eth_w5500_io (&ethw, ppins[input_pins[0] - 1].value, ppins[input_pins[3] - 1].value, ppins[input_pins[2] - 1].value, ppins[input_pins[1] - 1].value);

 if (!ppins[input_pins[2] - 1].value) //if CS is active, update output 
  {
   if (_ret != ret)
    {
     Window5.SetPin (output_pins[0], (ret & 0x01) > 0);
     Window5.SetPin (output_pins[1], (ret & 0x02) > 0);
    }
   _ret = ret;
  }
 else
  {
   _ret = 0xFF; //invalid value
  }

}

void
cpart_ETH_w5500::EvMouseButtonPress(uint button, uint x, uint y, uint state)
{
 int i;

 for (i = 0; i < inputc; i++)
  {
   if (PointInside (x, y, input[i]))
    {
     switch (input[i].id)
      {
      case I_CONN:
       link ^= 1;
       eth_w5500_set_link (&ethw, link);
       break;
      }
    }
  }
}

void
cpart_ETH_w5500::PostProcess(void)
{

 if (output_ids[O_LACT]->value != (eth_w5500_get_leds (&ethw) & 0x08))
  {
   output_ids[O_LACT]->value = eth_w5500_get_leds (&ethw) & 0x08;
   output_ids[O_LACT]->update = 1;
  }

 if (output_ids[O_LLINK]->value != (eth_w5500_get_leds (&ethw) & 0x01))
  {
   output_ids[O_LLINK]->value = eth_w5500_get_leds (&ethw) & 0x01;
   output_ids[O_LLINK]->update = 1;
  }


 for (int n = 0; n < 8; n++)
  {
   if ((sts[n][0] != (ethw.Socket[n][Sn_MR] & 0x0F)) ||
       (sts[n][1] != ethw.Socket[n][Sn_SR]) ||
       (sts[n][2] != ethw.status[n]) ||
       (sts[n][3] != ethw.bindp[n]))
    {
     sts[n][0] = ethw.Socket[n][Sn_MR] & 0x0F;
     sts[n][1] = ethw.Socket[n][Sn_SR];
     sts[n][2] = ethw.status[n];
     sts[n][3] = ethw.bindp[n];
     output_ids[O_STAT]->update = 1;
     break;
    }
  }


}

part_init(PART_ETH_W5500_Name, cpart_ETH_w5500, "Other");

