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

//Oscilloscope


#include"picsimlab4.h"
#include"picsimlab1.h"
#include"picsimlab5.h"

#include"picsimlab4_d.cc"


CPWindow4 Window4;


//Implementation


#define WMAX 350
#define HMAX 250

#define NPOINTS 700

//flip buffers+ 2 channels + 700 points
double data[2][2][NPOINTS];
int fp = 0;

double *ch[2];

int tch = 0;

void
CPWindow4::DrawScreen(void)
{
 draw1.Canvas.Init ();

 //Clear background
 draw1.Canvas.SetFgColor (0, 0, 0);
 draw1.Canvas.SetBgColor (0, 0, 0);
 draw1.Canvas.Rectangle (1, 0, 0, WMAX, HMAX + 30);
 //draw lines
 draw1.Canvas.SetFgColor (50, 50, 50);

 int inch = HMAX / 10;
 for (int i = 1; i < 10; i++)
  draw1.Canvas.Line (0, inch * i, WMAX, inch * i);

 int incw = WMAX / 10;
 for (int i = 1; i < 10; i++)
  draw1.Canvas.Line (incw * i, 0, incw * i, HMAX);

 draw1.Canvas.SetFgColor (140, 140, 140);
 draw1.Canvas.Line (0, HMAX / 2, WMAX, HMAX / 2);
 draw1.Canvas.Line (WMAX / 2, 0, WMAX / 2, HMAX);

 float gain[3], nivel[3];
 lxPoint pts[3];


 //draw ch 0
 if (togglebutton1.GetCheck ())
  {
   gain[0] = (HMAX / spind1.GetValue ()) / 10.0;
   nivel[0] = (gain[0]*-spind2.GetValue ())+(HMAX / 2.0);
   if (togglebutton3.GetCheck ())gain[0] = -gain[0];
   draw1.Canvas.SetFgColor (button1.GetColor ());

   pts[0].x = 0;
   pts[0].y = nivel[0] - 8;
   pts[1].x = 8;
   pts[1].y = nivel[0];
   pts[2].x = 0;
   pts[2].y = nivel[0] + 8;
   draw1.Canvas.Polygon (1, pts, 3);

   draw1.Canvas.SetLineWidth (2);
   for (int t = 0; t < (NPOINTS / 2) - 1; t++)
    {
     draw1.Canvas.Line ((t * xz + xz)-((NPOINTS * (xz - 1.0)) / 4.0), gain[0] * ch[0][t] + nivel[0], ((t + 1) * xz + xz)-((NPOINTS * (xz - 1.0)) / 4.0), gain[0] * ch[0][t + 1] + nivel[0]);
    }
  }
 draw1.Canvas.SetLineWidth (1);
 //draw ch 1
 if (togglebutton2.GetCheck ())
  {
   gain[1] = (HMAX / spind3.GetValue ()) / 10.0;
   nivel[1] = (gain[1]*-spind4.GetValue ())+(HMAX / 2.0);
   if (togglebutton4.GetCheck ())gain[1] = -gain[1];
   draw1.Canvas.SetFgColor (button2.GetColor ());

   pts[0].x = 0;
   pts[0].y = nivel[1] - 5;
   pts[1].x = 5;
   pts[1].y = nivel[1];
   pts[2].x = 0;
   pts[2].y = nivel[1] + 5;
   draw1.Canvas.Polygon (1, pts, 3);

   draw1.Canvas.SetLineWidth (2);
   for (int t = 0; t < (NPOINTS / 2) - 1; t++)
    {
     draw1.Canvas.Line ((t * xz + xz)-((NPOINTS * (xz - 1.0)) / 4.0), gain[1] * ch[1][t] + nivel[1], ((t + 1) * xz + xz)-((NPOINTS * (xz - 1.0)) / 4.0), gain[1] * ch[1][t + 1] + nivel[1]);
    }
  }
 draw1.Canvas.SetLineWidth (1);

 //draw trigger level
 if (usetrigger)
  {
   if (combo1.GetText ().compare (lxT ("1")) == 0)
    tch = 0;
   else
    tch = 1;

   draw1.Canvas.SetFgColor (255, 255, 0);
   nivel[2] = (gain[tch]*-spind7.GetValue ()) + nivel[tch];
   pts[0].x = 0;
   pts[0].y = nivel[2] - 3;
   pts[1].x = 3;
   pts[1].y = nivel[2];
   pts[2].x = 0;
   pts[2].y = nivel[2] + 3;
   draw1.Canvas.Polygon (1, pts, 3);
  }

 //draw toffset level

 draw1.Canvas.SetFgColor (255, 255, 0);
 nivel[2] = ((WMAX - toffset) * xz + xz)-((NPOINTS * (xz - 1.0)) / 4.0);
 pts[0].y = 1;
 pts[0].x = nivel[2] - 3;
 pts[1].y = 1 + 3;
 pts[1].x = nivel[2];
 pts[2].y = 1;
 pts[2].x = nivel[2] + 3;
 draw1.Canvas.Polygon (1, pts, 3);

 lxString text;

 draw1.Canvas.SetFgColor (0, 0, 0);
 draw1.Canvas.SetBgColor (0, 0, 0);
 draw1.Canvas.Rectangle (1, 0, HMAX, WMAX, HMAX + 30);

 draw1.Canvas.SetFgColor (button1.GetColor ());
 if (togglebutton1.GetCheck ())
  {
   text.Printf ("CH1 %6.2fV %6.2fV %c", spind1.GetValue (), spind2.GetValue (), ((togglebutton3.GetCheck () == 1) ? 'I' : 'N'));
  }
 else
  {
   text.Printf ("CH1 Off");
  }
 draw1.Canvas.Text (text, 10, HMAX + 1);

 draw1.Canvas.SetFgColor (button2.GetColor ());
 if (togglebutton2.GetCheck ())
  {
   text.Printf ("CH2 %6.2fV %6.2fV %c", spind3.GetValue (), spind4.GetValue (), ((togglebutton4.GetCheck () == 1) ? 'I' : 'N'));
  }
 else
  {
   text.Printf ("CH2 Off");
  }
 draw1.Canvas.Text (text, 10, HMAX + 14);

 draw1.Canvas.SetFgColor (200, 200, 200);
 text.Printf ("TM %6.3fms  %6.3fms", spind5.GetValue (), spind6.GetValue ());
 draw1.Canvas.Text (text, WMAX / 2, HMAX + 1);

 draw1.Canvas.SetFgColor (200, 200, 0);
 if (togglebutton5.GetCheck ())
  {
   text.Printf ("TRG CH%s %6.2fV ", combo1.GetText ().c_str (), spind7.GetValue ());
  }
 else
  {
   text.Printf ("TRG Off");
  }
 draw1.Canvas.Text (text, WMAX / 2, HMAX + 14);

 draw1.Canvas.End ();

}

void
CPWindow4::button1_EvMouseButtonClick(CControl * control, uint button, uint x, uint y, uint state)
{
#ifndef __WXX11__ 
 colordialog1.SetColor (button1.GetColor ());
 ctrl = &button1;
 colordialog1.Run ();
#endif  
}

void
CPWindow4::button2_EvMouseButtonClick(CControl * control, uint button, uint x, uint y, uint state)
{
#ifndef __WXX11__ 
 colordialog1.SetColor (button2.GetColor ());
 ctrl = &button2;
 colordialog1.Run ();
#endif
}

void
CPWindow4::draw1_EvMouseButtonClick(CControl * control, uint button, uint x, uint y, uint state)
{
 //code here:)
 mprint (lxT ("draw1_EvMouseButtonClick\n"));
}

void
CPWindow4::SetBaseTimer(void)
{
 board * pboard = Window1.GetBoard ();

 if (!pboard)return;

 int PinCount = pboard->MGetPinCount ();

 if (pboard->CpuInitialized () == 0)return;

 Dt = 1.0 / pboard->MGetInstClockFreq ();

 int chp[2];

 //printf("Dt=%e  Rt=%e  Rt/Dt=%f\n",Dt,Rt,Rt/Dt); 

 chp[0] = atoi (combo2.GetText ());
 chp[1] = atoi (combo3.GetText ());


 combo2.DeleteItems ();
 combo3.DeleteItems ();
 for (int i = 1; i <= PinCount; i++)
  {

   lxString spin;
   if (pboard->GetUseSpareParts ())
    {
     spin = Window5.GetPinName (i);
    }
   else
    {
     spin = pboard->MGetPinName (i);
    }
   if (spin.Cmp (lxT ("error")))
    {
     combo2.AddItem (itoa (i) + "  " + spin);
     combo3.AddItem (itoa (i) + "  " + spin);
    }
  }

 if (chp[0] <= PinCount)
  {
   lxString spin;
   if (pboard->GetUseSpareParts ())
    {
     spin = Window5.GetPinName (chp[0]);
    }
   else
    {
     spin = pboard->MGetPinName (chp[0]);
    }
   combo2.SetText (itoa (chp[0]) + "  " + spin);
  }
 else
  combo2.SetText ("1");

 if (chp[1] <= PinCount)
  {
   lxString spin;
   if (pboard->GetUseSpareParts ())
    {
     spin = Window5.GetPinName (chp[1]);
    }
   else
    {
     spin = pboard->MGetPinName (chp[1]);
    }
   combo3.SetText (itoa (chp[1]) + "  " + spin);
  }
 else
  combo3.SetText ("2");

 spind5_EvOnChangeSpinDouble (this);
}

int is = 0;
double t = 0;
int tr = 0;
double pins_[2];
int update = 0;

void
CPWindow4::SetSample(void)
{
 double pins[2];

 const picpin * ppins = Window1.GetBoard ()->MGetPinsValues ();

 if (!run)return;

 if ((ppins[chpin[0]].ptype == PT_ANALOG)&&(ppins[chpin[0]].dir == PD_IN))
  pins[0] = ppins[chpin[0]].avalue;
 else
  pins[0] = ppins[chpin[0]].value * 5.0;

 if ((ppins[chpin[1]].ptype == PT_ANALOG)&&(ppins[chpin[1]].dir == PD_IN))
  pins[1] = ppins[chpin[1]].avalue;
 else
  pins[1] = ppins[chpin[1]].value * 5.0;

 if (t > Rt)
  {
   t -= Rt;
   data[fp][0][is] = -pins[0]+((1.0 * rand () / RAND_MAX) - 0.5)*0.1;
   data[fp][1][is] = -pins[1]+((1.0 * rand () / RAND_MAX) - 0.5)*0.1;
   is++;
   if (is >= NPOINTS)
    {
     if (tr && togglebutton7.GetCheck ())
      {
       togglebutton6.SetCheck (1);
      }
     is = 0;
     tr = 0;
     ch[0] = &data[fp][0][toffset];
     ch[1] = &data[fp][1][toffset];
     fp = !fp; //togle fp
     update = 1;
     // DrawScreen();
    }

  }
 t += Dt;

 //trigger
 if (usetrigger)
  {
   if ((!tr)&&(is >= NPOINTS / 2))
    {
     if ((pins_[tch] < triggerlv)&&(pins[tch] >= triggerlv))
      {
       tr = 1;

       is = is - (NPOINTS / 2);

       for (int i = 0; i < NPOINTS / 2; i++)
        {
         data[fp][0][i] = data[fp][0][i + is];
         data[fp][1][i] = data[fp][1][i + is];
        }
       is = (NPOINTS / 2);
      }
    }
  }


 pins_[0] = pins[0];
 pins_[1] = pins[1];
}

void
CPWindow4::spind5_EvOnChangeSpinDouble(CControl * control)
{
 //spind5.SetMin ((Dt*WMAX)/10e-3);

 Rt = ((spind5.GetValue ()*1e-3 * 10) / WMAX);

 if ((Rt / Dt) < 1.0)
  {
   xz = Dt / Rt;
  }
 else
  xz = 1.0;

 spind6.SetMin (-5 * spind5.GetValue ());
 spind6.SetMax (5 * spind5.GetValue ());

 spind6.SetValue (0);
 toffset = WMAX / 2;
 //printf("Dt=%e Rt=%e  Rt/Dt=%f   xz=%f\n",Dt,Rt,Rt/Dt,xz);
}

void
CPWindow4::togglebutton5_EvOnToggleButton(CControl * control)
{
 usetrigger = togglebutton5.GetCheck ();
}

void
CPWindow4::spind7_EvOnChangeSpinDouble(CControl * control)
{
 triggerlv = spind7.GetValue ();
}

void
CPWindow4::timer1_EvOnTime(CControl * control)
{
 if (update)
  {
   update = 0;
   DrawScreen ();
   if (togglebutton6.GetCheck () && spind1.GetEnable ())
    togglebutton6_EvOnToggleButton (this);
  }
}

void
CPWindow4::combo2_EvOnComboChange(CControl * control)
{
 chpin[0] = atoi (combo2.GetText ()) - 1;
}

void
CPWindow4::combo3_EvOnComboChange(CControl * control)
{
 chpin[1] = atoi (combo3.GetText ()) - 1;
}

void
CPWindow4::WritePreferences(void)
{
 Window1.saveprefs (lxT ("osc_scale1"), ftoa (spind1.GetValue ()));
 Window1.saveprefs (lxT ("osc_offset1"), ftoa (spind2.GetValue ()));
 Window1.saveprefs (lxT ("osc_on1"), itoa (togglebutton1.GetCheck ()));
 Window1.saveprefs (lxT ("osc_color1"), button1.GetColor ().GetAsString (lxC2S_HTML_SYNTAX));
 Window1.saveprefs (lxT ("osc_inv"), itoa (togglebutton3.GetCheck ()));
 Window1.saveprefs (lxT ("osc_ch1"), combo2.GetText ());

 Window1.saveprefs (lxT ("osc_scale2"), ftoa (spind3.GetValue ()));
 Window1.saveprefs (lxT ("osc_offset2"), ftoa (spind4.GetValue ()));
 Window1.saveprefs (lxT ("osc_on2"), itoa (togglebutton2.GetCheck ()));
 Window1.saveprefs (lxT ("osc_color2"), button2.GetColor ().GetAsString (lxC2S_HTML_SYNTAX));
 Window1.saveprefs (lxT ("osc_inv2"), itoa (togglebutton4.GetCheck ()));
 Window1.saveprefs (lxT ("osc_ch2"), combo3.GetText ());

 Window1.saveprefs (lxT ("osc_tscale"), ftoa (spind5.GetValue ()));
 //Window1.saveprefs(lxT("osc_toffset"),ftoa(spind6.GetValue()));
 Window1.saveprefs (lxT ("osc_usetrigger"), itoa (togglebutton5.GetCheck ()));
 Window1.saveprefs (lxT ("osc_tch"), combo1.GetText ());
 Window1.saveprefs (lxT ("osc_tlevel"), ftoa (spind7.GetValue ()));
 Window1.saveprefs (lxT ("osc_position"), itoa (GetX ()) + lxT (",") + itoa (GetY ()));
}

void
CPWindow4::ReadPreferences(char *name, char *value)
{
 if (!strcmp (name, "osc_scale1"))
  {
   spind1.SetValue (atof (value));
  }

 if (!strcmp (name, "osc_offset1"))
  {
   spind2.SetValue (atof (value));
  }

 if (!strcmp (name, "osc_on1"))
  {
   togglebutton1.SetCheck (atoi (value));
  }

 if (!strcmp (name, "osc_color1"))
  {
   button1.SetColor (lxColor (value));
  }

 if (!strcmp (name, "osc_inv1"))
  {
   togglebutton3.SetCheck (atoi (value));
  }

 if (!strcmp (name, "osc_ch1"))
  {
   combo2.SetText (value);
   chpin[0] = atoi (combo2.GetText ()) - 1;
  }

 if (!strcmp (name, "osc_scale2"))
  {
   spind3.SetValue (atof (value));
  }

 if (!strcmp (name, "osc_offset2"))
  {
   spind4.SetValue (atof (value));
  }

 if (!strcmp (name, "osc_on2"))
  {
   togglebutton2.SetCheck (atoi (value));
  }

 if (!strcmp (name, "osc_color2"))
  {
   button2.SetColor (lxColor (value));
  }

 if (!strcmp (name, "osc_inv2"))
  {
   togglebutton4.SetCheck (atoi (value));
  }

 if (!strcmp (name, "osc_ch2"))
  {
   combo3.SetText (value);
   chpin[1] = atoi (combo3.GetText ()) - 1;
  }

 if (!strcmp (name, "osc_tscale"))
  {
   spind5.SetValue (atof (value));
  }

 if (!strcmp (name, "osc_usetrigger"))
  {
   togglebutton5.SetCheck (atoi (value));
   usetrigger = atoi (value);
  }

 if (!strcmp (name, "osc_tch"))
  {
   combo1.SetText (value);
  }

 if (!strcmp (name, "osc_tlevel"))
  {
   spind7.SetValue (atof (value));
  }

 if (!strcmp (name, "osc_position"))
  {
   int i, j;
   sscanf (value, "%i,%i", &i, &j);
   SetX (i);
   SetY (j);
  }
}

void
CPWindow4::_EvOnDestroy(CControl * control) { }

void
CPWindow4::_EvOnShow(CControl * control)
{
 timer1.SetRunState (1);
}

void
CPWindow4::_EvOnHide(CControl * control)
{
 timer1.SetRunState (0);
 board * pboard = Window1.GetBoard ();
 if (pboard)
  {
   pboard->SetUseOscilloscope (0);
  }
}

void
CPWindow4::spind6_EvOnChangeSpinDouble(CControl * control)
{


 toffset = (WMAX / 2) - (((WMAX / 2) * spind6.GetValue ()) / (5 * spind5.GetValue ()));
}

//autoset

void
CPWindow4::button3_EvMouseButtonClick(CControl * control, uint button, uint x, uint y, uint state)
{

 int ce = togglebutton1.GetCheck () + togglebutton2.GetCheck ()*2;


 togglebutton5.SetCheck (1); //trigguer
 spind5.SetValue (1.0); //time scale
 spind5_EvOnChangeSpinDouble (control);
 //spind6.SetValue(0.0);//time offset
 spind7.SetValue (2.5); //trigguer level
 switch (ce)
  {
  case 0:
   break;
  case 1:
   spind1.SetValue (2.0); //ch1 gain
   spind2.SetValue (0.0); //ch1 level
   togglebutton3.SetCheck (0); //ch1 inverse off
   combo1.SetText ("1"); //trigguer channel
   break;
  case 2:
   spind3.SetValue (2.0); //ch2 gain
   spind4.SetValue (0.0); //ch2 level
   togglebutton4.SetCheck (0); //ch2 inverse off
   combo1.SetText ("2"); //trigguer channel
   break;
  case 3:
   spind1.SetValue (2.0); //ch1 gain
   spind2.SetValue (0.0); //ch1 level
   spind3.SetValue (2.0); //ch2 gain
   spind4.SetValue (-6.0); //ch2 level
   togglebutton3.SetCheck (0); //ch1 inverse off
   togglebutton4.SetCheck (0); //ch2 inverse off
   //TODO select the better mode for channel trigguer
   combo1.SetText ("1"); //trigguer channel
   break;
  }

}


//stop

void
CPWindow4::togglebutton6_EvOnToggleButton(CControl * control)
{
 run = !togglebutton6.GetCheck ();
 spind1.SetEnable (run);
 spind2.SetEnable (run);
 spind3.SetEnable (run);
 spind4.SetEnable (run);
 spind5.SetEnable (run);
 spind6.SetEnable (run);
 spind7.SetEnable (run);
 button1.SetEnable (run);
 button2.SetEnable (run);
 button3.SetEnable (run);
 togglebutton1.SetEnable (run);
 togglebutton2.SetEnable (run);
 togglebutton3.SetEnable (run);
 togglebutton4.SetEnable (run);
 togglebutton5.SetEnable (run);
 combo1.SetEnable (run);
 combo2.SetEnable (run);
 combo3.SetEnable (run);
}

//save PNG

void
CPWindow4::button4_EvMouseButtonClick(CControl * control, uint button, uint x, uint y, uint state)
{

 filedialog1.SetType (lxFD_SAVE | lxFD_CHANGE_DIR);
 filedialog1.Run ();

}

void
CPWindow4::filedialog1_EvOnClose(int retId)
{
 if (retId)
  {
   draw1.WriteImgToFile (filedialog1.GetFileName ());
  }
}

void
CPWindow4::colordialog1_EvOnClose(int retId)
{

 if (retId)
  {
   ctrl->SetColor (colordialog1.GetColor ());
  }
}


