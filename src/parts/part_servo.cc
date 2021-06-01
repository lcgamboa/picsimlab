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
#include"part_servo.h"

/* outputs */
enum
{
 O_P1, O_AXIS
};

cpart_servo::cpart_servo(unsigned x, unsigned y)
: font(9, lxFONTFAMILY_TELETYPE, lxFONTSTYLE_NORMAL, lxFONTWEIGHT_BOLD)
{
 X = x;
 Y = y;
 input_pin = 0;
 angle = 0;
 angle_ = 0;
 in_[0] = 0;
 in_[1] = 0;
 time = 0;

 ReadMaps ();

 lxImage image (&Window5);
 image.LoadFile (Window1.GetSharePath () + lxT ("parts/") + GetPictureFileName (), Orientation, Scale, Scale);

 Bitmap = new lxBitmap (&image, &Window5);
 image.Destroy ();

 image.LoadFile (Window1.GetSharePath () + lxT ("parts/") + GetPictureFileName (), Orientation, Scale, Scale);
 BackGround = new lxBitmap (&image, &Window5);
 image.Destroy ();

 canvas.Create (Window5.GetWWidget (), Bitmap);
}

cpart_servo::~cpart_servo(void)
{
 delete Bitmap;
 delete BackGround;
 canvas.Destroy ();
}

void
cpart_servo::Draw(void)
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
       canvas.SetBitmap (BackGround, 1.0, 1.0); //FIXME draw servo error on scale or rotate
       canvas.Init (Scale, Scale, Orientation);
       canvas.SetFont (font);
      }
     Update++; //set to update buffer

     if (output[i].id == O_P1)
      {
       canvas.SetFgColor (255, 255, 255);
       if (input_pin == 0)
        canvas.RotatedText ("NC", output[i].x1, output[i].y1, 0);
       else
        canvas.RotatedText (Window5.GetPinName (input_pin), output[i].x1, output[i].y1, 0);
      }

     if (output[i].id == O_AXIS)
      {
       float x2 = output[i].x1 + output[i].r * sin (angle);
       float y2 = output[i].y1 - output[i].r * cos (angle);
       canvas.SetFgColor (0, 0, 0);
       canvas.SetLineWidth (20);
       canvas.Line (output[i].x1, output[i].y1, x2, y2);
       canvas.SetFgColor (255, 255, 255);
       canvas.SetLineWidth (18);
       canvas.Line (output[i].x1, output[i].y1, x2, y2);
      }
    }
  }

 if (Update)
  {
   canvas.End ();
  }
}

void
cpart_servo::Process(void)
{

 const picpin * ppins = Window5.GetPinsValues ();

 if (input_pin == 0)return;

 in_[1] = in_[0];
 in_[0] = ppins[input_pin - 1].value;

 if ((in_[0] == 1)&&(in_[1] == 0))//rise
  {
   time = 0;
  }

 if ((in_[0] == 0)&&(in_[1] == 1))//low
  {
   angle_ = ((time / Window1.GetBoard ()->MGetInstClockFreq ()) - 0.0015)*3141.59265359;

   if (angle_ > M_PI / 2.0)angle_ = M_PI / 2.0;
   if (angle_ < -M_PI / 2.0)angle_ = -M_PI / 2.0;

  }

 time++;
}

void
cpart_servo::PostProcess(void)
{
 if (angle > angle_)
  {
   angle -= 0.2;
   if (angle < angle_) angle = angle_;
  }

 if (angle < angle_)
  {
   angle += 0.2;
   if (angle > angle_) angle = angle_;
  }
 
 
  if (output_ids[O_AXIS]->value_f != angle)
  {
   output_ids[O_AXIS]->value_f = angle;
   output_ids[O_AXIS]->update = 1;
   output_ids[O_P1]->update = 1; 
  }
}

unsigned short
cpart_servo::get_in_id(char * name)
{
 printf ("Erro input '%s' don't have a valid id! \n", name);
 return -1;
}

unsigned short
cpart_servo::get_out_id(char * name)
{

 if (strcmp (name, "PN_1") == 0)return O_P1;
 if (strcmp (name, "DG_AXIS") == 0)return O_AXIS;

 printf ("Erro output '%s' don't have a valid id! \n", name);
 return 1;
}

lxString
cpart_servo::WritePreferences(void)
{
 char prefs[256];

 sprintf (prefs, "%hhu", input_pin);

 return prefs;
}

void
cpart_servo::ReadPreferences(lxString value)
{
 sscanf (value.c_str (), "%hhu", &input_pin);
 RegisterRemoteControl ();
}

void
cpart_servo::RegisterRemoteControl(void)
{
 for (int i = 0; i < outputc; i++)
  {
   switch (output[i].id)
    {
    case O_AXIS:
     output[i].status = (void *) &angle;
     break;
    }
  }
}

void
cpart_servo::ConfigurePropertiesWindow(CPWindow * WProp)
{
 lxString Items = Window5.GetPinsNames ();
 lxString spin;

 ((CCombo*) WProp->GetChildByName ("combo1"))->SetItems (Items);
 if (input_pin == 0)
  ((CCombo*) WProp->GetChildByName ("combo1"))->SetText ("0  NC");
 else
  {
   spin = Window5.GetPinName (input_pin);
   ((CCombo*) WProp->GetChildByName ("combo1"))->SetText (itoa (input_pin) + "  " + spin);
  }


 ((CButton*) WProp->GetChildByName ("button1"))->EvMouseButtonRelease = EVMOUSEBUTTONRELEASE & CPWindow5::PropButtonRelease;
 ((CButton*) WProp->GetChildByName ("button1"))->SetTag (1);

 ((CButton*) WProp->GetChildByName ("button2"))->EvMouseButtonRelease = EVMOUSEBUTTONRELEASE & CPWindow5::PropButtonRelease;
}

void
cpart_servo::ReadPropertiesWindow(CPWindow * WProp)
{
 input_pin = atoi (((CCombo*) WProp->GetChildByName ("combo1"))->GetText ());
 RegisterRemoteControl ();
}

void
cpart_servo::SetOrientation(int orientation)
{
 if (Orientation == orientation)return;

 part::SetOrientation (orientation);

 delete BackGround;

 lxImage image (&Window5);

 image.LoadFile (Window1.GetSharePath () + lxT ("parts/") + GetPictureFileName (), Orientation, Scale, Scale);

 BackGround = new lxBitmap (&image, &Window5);

 image.Destroy ();
}

void
cpart_servo::SetScale(double scale)
{

 if (Scale == scale)return;

 part::SetScale (scale);

 delete BackGround;

 lxImage image (&Window5);

 image.LoadFile (Window1.GetSharePath () + lxT ("parts/") + GetPictureFileName (), Orientation, Scale, Scale);

 BackGround = new lxBitmap (&image, &Window5);

 image.Destroy ();
}

//Register the part in PICSimLab spare parts list
part_init("Servo motor", cpart_servo, "Output");


