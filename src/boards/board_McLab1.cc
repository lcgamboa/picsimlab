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
#include"board_McLab1.h"

/* outputs */
enum
{
 O_SS1,
 O_SS2,
 O_RB0,
 O_RB1,
 O_RB2,
 O_RB3,
 O_RB4,
 O_RB5,
 O_RB6,
 O_RB7,
 O_RA0,
 O_RA1,
 O_RA2,
 O_RA3,
 O_RA0L,
 O_A1,
 O_B1,
 O_C1,
 O_D1,
 O_E1,
 O_F1,
 O_G1,
 O_P1,
 O_A2,
 O_B2,
 O_C2,
 O_D2,
 O_E2,
 O_F2,
 O_G2,
 O_P2,
 O_JP1,
 O_BRA1,
 O_BRA2,
 O_BRA3,
 O_BRA4,
 O_RST,
 O_MP
};

/*inputs*/
enum
{
 I_RST,
 I_RA1,
 I_RA2,
 I_RA3,
 I_RA4,
 I_PWR,
 I_ICSP,
 I_JP1
};

cboard_McLab1::cboard_McLab1(void) :
font(10, lxFONTFAMILY_TELETYPE, lxFONTSTYLE_NORMAL, lxFONTWEIGHT_BOLD)
{
 Proc = "PIC16F628A";
 ReadMaps ();
 jmp[0] = 0;

 //gauge1
 gauge1 = new CGauge ();
 gauge1->SetFOwner (&Window1);
 gauge1->SetName (lxT ("gauge1_p1"));
 gauge1->SetX (13);
 gauge1->SetY (102);
 gauge1->SetWidth (140);
 gauge1->SetHeight (20);
 gauge1->SetEnable (1);
 gauge1->SetVisible (1);
 gauge1->SetRange (100);
 gauge1->SetValue (0);
 gauge1->SetType (4);
 Window1.CreateChild (gauge1);
 //label1
 label1 = new CLabel ();
 label1->SetFOwner (&Window1);
 label1->SetName (lxT ("label1_p1"));
 label1->SetX (12);
 label1->SetY (80);
 label1->SetWidth (60);
 label1->SetHeight (20);
 label1->SetEnable (1);
 label1->SetVisible (1);
 label1->SetText (lxT ("LAMP"));
 label1->SetAlign (1);
 Window1.CreateChild (label1);
}

cboard_McLab1::~cboard_McLab1(void)
{
 Window1.DestroyChild (gauge1);
 Window1.DestroyChild (label1);
}

void
cboard_McLab1::Draw(CDraw *draw)
{
 int i;
 int update = 0; //verifiy if updated is needed

 //lab1 draw 
 for (i = 0; i < outputc; i++)
  {
   if (output[i].update)//only if need update
    {
     output[i].update = 0;

     if (!update)
      {
       draw->Canvas.Init (Scale, Scale);
      }
     update++; //set to update buffer

     if (!output[i].r)//rectangle
      {
       draw->Canvas.SetFgColor (0, 0, 0);

       if (jmp[0])
        {
         switch (output[i].id)
          {
          case O_A1: draw->Canvas.SetBgColor (lm1[7], 0, 0);
           break;
          case O_B1: draw->Canvas.SetBgColor (lm1[8], 0, 0);
           break;
          case O_C1: draw->Canvas.SetBgColor (lm1[10], 0, 0);
           break;
          case O_D1: draw->Canvas.SetBgColor (lm1[11], 0, 0);
           break;
          case O_E1: draw->Canvas.SetBgColor (lm1[12], 0, 0);
           break;
          case O_F1: draw->Canvas.SetBgColor (lm1[6], 0, 0);
           break;
          case O_G1: draw->Canvas.SetBgColor (lm1[5], 0, 0);
           break;
          case O_A2: draw->Canvas.SetBgColor (lm2[7], 0, 0);
           break;
          case O_B2: draw->Canvas.SetBgColor (lm2[8], 0, 0);
           break;
          case O_C2: draw->Canvas.SetBgColor (lm2[10], 0, 0);
           break;
          case O_D2: draw->Canvas.SetBgColor (lm2[11], 0, 0);
           break;
          case O_E2: draw->Canvas.SetBgColor (lm2[12], 0, 0);
           break;
          case O_F2: draw->Canvas.SetBgColor (lm2[6], 0, 0);
           break;
          case O_G2: draw->Canvas.SetBgColor (lm2[5], 0, 0);
           break;
          }
        }
       else
        {
         draw->Canvas.SetBgColor (55, 0, 0);
        }

       if (output[i].id == O_JP1) draw->Canvas.SetColor (150, 150, 150);

       if ((output[i].id >= O_BRA1)&&(output[i].id <= O_BRA4))
        {
         draw->Canvas.SetColor (100, 100, 100);
         draw->Canvas.Circle (1, output[i].cx, output[i].cy, 22);
         if (p_BT[output[i].id - O_BRA1])
          {
           draw->Canvas.SetColor (15, 15, 15);
          }
         else
          {
           draw->Canvas.SetColor (55, 55, 55);
          }
         draw->Canvas.Circle (1, output[i].cx, output[i].cy, 19);
        }
       else if (output[i].id == O_RST)
        {
         draw->Canvas.SetColor (100, 100, 100);
         draw->Canvas.Circle (1, output[i].cx, output[i].cy, 22);
         if (p_RST)
          {
           draw->Canvas.SetColor (15, 15, 15);
          }
         else
          {
           draw->Canvas.SetColor (55, 55, 55);
          }
         draw->Canvas.Circle (1, output[i].cx, output[i].cy, 19);
        }
       else if (output[i].id == O_MP)
        {
         draw->Canvas.SetFont (font);
         draw->Canvas.SetColor (26, 26, 26);
         draw->Canvas.Rectangle (1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);
         draw->Canvas.SetColor (230, 230, 230);
         draw->Canvas.RotatedText (Proc, output[i].x1 + 5, output[i].y1, 0);
        }
       else if (output[i].id == O_SS1)
        {
         output_ids[O_A1]->update = 1;
         output_ids[O_B1]->update = 1;
         output_ids[O_C1]->update = 1;
         output_ids[O_D1]->update = 1;
         output_ids[O_E1]->update = 1;
         output_ids[O_F1]->update = 1;
         output_ids[O_G1]->update = 1;
         output_ids[O_P1]->update = 1;

         draw->Canvas.SetColor (10, 10, 10);
         draw->Canvas.Rectangle (1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);
        }
       else if (output[i].id == O_SS2)
        {
         output_ids[O_A2]->update = 1;
         output_ids[O_B2]->update = 1;
         output_ids[O_C2]->update = 1;
         output_ids[O_D2]->update = 1;
         output_ids[O_E2]->update = 1;
         output_ids[O_F2]->update = 1;
         output_ids[O_G2]->update = 1;
         output_ids[O_P2]->update = 1;

         draw->Canvas.SetColor (10, 10, 10);
         draw->Canvas.Rectangle (1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);
        }
       else if (output[i].id == O_JP1)
        {
         if (!jmp[0])
          {
           draw->Canvas.SetColor (70, 70, 70);
           draw->Canvas.Rectangle (1, output[i].x1, output[i].y1, (int) ((output[i].x2 - output[i].x1)*0.65), output[i].y2 - output[i].y1);
           draw->Canvas.SetColor (220, 220, 0);
           draw->Canvas.Circle (1, output[i].x1 + (int) ((output[i].x2 - output[i].x1)*0.80), output[i].y1 + ((output[i].y2 - output[i].y1) / 2), 3);
          }
         else
          {
           draw->Canvas.SetColor (70, 70, 70);
           draw->Canvas.Rectangle (1, output[i].x1 + ((int) ((output[i].x2 - output[i].x1)*0.35)), output[i].y1, (int) ((output[i].x2 - output[i].x1)*0.65), output[i].y2 - output[i].y1);
           draw->Canvas.SetColor (220, 220, 0);
           draw->Canvas.Circle (1, output[i].x1 + (int) ((output[i].x2 - output[i].x1)*0.20), output[i].y1 + ((output[i].y2 - output[i].y1) / 2), 3);
          }
        }
       else
        {
         draw->Canvas.Rectangle (1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);
        }

      }//circle
     else
      {
       int led = 1;

       draw->Canvas.SetFgColor (55, 0, 0);

       if (!jmp[0])
        {
         switch (output[i].id)
          {
          case O_RB0: draw->Canvas.SetBgColor (pic.pins[5].oavalue, 0, 0);
           break;
          case O_RB1: draw->Canvas.SetBgColor (pic.pins[6].oavalue, 0, 0);
           break;
          case O_RB2: draw->Canvas.SetBgColor (pic.pins[7].oavalue, 0, 0);
           break;
          case O_RB3: draw->Canvas.SetBgColor (pic.pins[8].oavalue, 0, 0);
           break;
          case O_RB4: draw->Canvas.SetBgColor (pic.pins[9].oavalue, 0, 0);
           break;
          case O_RB5: draw->Canvas.SetBgColor (pic.pins[10].oavalue, 0, 0);
           break;
          case O_RB6: draw->Canvas.SetBgColor (pic.pins[11].oavalue, 0, 0);
           break;
          case O_RB7: draw->Canvas.SetBgColor (pic.pins[12].oavalue, 0, 0);
           break;
          }
        }
       else
        {
         if ((output[i].name[3] == 'R')&&(output[i].name[4] == 'B'))
          {
           draw->Canvas.SetBgColor (55, 0, 0);
          }
        }

       switch (output[i].id)
        {
        case O_RA0: draw->Canvas.SetFgColor (0, 55, 0);
         draw->Canvas.SetBgColor (0, pic.pins[16].oavalue, 0);
         break;
        case O_RA1: draw->Canvas.SetFgColor (0, 55, 0);
         draw->Canvas.SetBgColor (0, pic.pins[17].oavalue, 0);
         break;
        case O_RA2: draw->Canvas.SetFgColor (0, 55, 0);
         draw->Canvas.SetBgColor (0, pic.pins[0].oavalue, 0);
         break;
        case O_RA3: draw->Canvas.SetFgColor (0, 55, 0);
         draw->Canvas.SetBgColor (0, pic.pins[1].oavalue, 0);
         break;
        case O_RA0L: draw->Canvas.SetFgColor (55, 55, 0);
         draw->Canvas.SetBgColor (pic.pins[16].oavalue, pic.pins[16].oavalue, 0);
         break;
        case O_P1: led = 0;
         draw->Canvas.SetBgColor (lm1[9], 0, 0);
         break;
        case O_P2: led = 0;
         draw->Canvas.SetBgColor (lm2[9], 0, 0);
         break;
        }

       if (led)
        {
         //draw a LED
         color1 = draw->Canvas.GetBgColor ();
         int r = color1.Red () - 120;
         int g = color1.Green () - 120;
         int b = color1.Blue () - 120;
         if (r < 0)r = 0;
         if (g < 0)g = 0;
         if (b < 0)b = 0;
         color2.Set (r, g, b);
         draw->Canvas.SetBgColor (color2);
         draw->Canvas.Circle (1, output[i].x1, output[i].y1, output[i].r + 1);
         draw->Canvas.SetBgColor (color1);
         draw->Canvas.Circle (1, output[i].x1, output[i].y1, output[i].r - 2);
        }
       else
        {
         draw->Canvas.Circle (1, output[i].x1, output[i].y1, output[i].r);
        }
      }
    }
  }

 //end draw
 if (update)
  {
   draw->Canvas.End ();
   draw->Update ();
  }

 //Lamp
 gauge1->SetValue ((pic.pins[16].oavalue - 55) / 2);

}

void
cboard_McLab1::Run_CPU(void)
{
 int i;
 int j;
 unsigned char pi, pj;
 unsigned char pinv;
 const picpin * pins;
 unsigned int alm[18]; //luminosidade media
 unsigned int alm1[18]; //luminosidade media display
 unsigned int alm2[18]; //luminosidade media display


 const int JUMPSTEPS = Window1.GetJUMPSTEPS ();
 const long int NSTEPJ = Window1.GetNSTEPJ ();
 const long int NSTEP = Window1.GetNSTEP ();
 const float RNSTEP = 200.0 * pic.PINCOUNT / NSTEP;

 memset (alm, 0, 18 * sizeof (unsigned int));
 memset (alm1, 0, 18 * sizeof (unsigned int));
 memset (alm2, 0, 18 * sizeof (unsigned int));


 pins = pic.pins;

 if (use_spare)Window5.PreProcess ();

 j = JUMPSTEPS;
 pi = 0;
 if (Window1.Get_mcupwr ())
  for (i = 0; i < NSTEP; i++)
   {
    if (j >= JUMPSTEPS)
     {
      pic_set_pin (pic.mclr, p_RST);
      pic_set_pin (18, p_BT[0]);
      pic_set_pin (1, p_BT[1]);
      pic_set_pin (2, p_BT[2]);
      pic_set_pin (3, p_BT[3]);
     }

    if (!mplabxd_testbp ())pic_step ();
    ioupdated = pic.ioupdated;
    if (use_oscope)Window4.SetSample ();
    if (use_spare)Window5.Process ();

    //increment mean value counter if pin is high
    alm[pi] += pins[pi].value;
    pi++;
    if (pi == pic.PINCOUNT)pi = 0;

    if (j >= JUMPSTEPS)
     {

      //pull-up extern 
      /*
      if ((pins[17].dir)&&(p_BT[0]))alm[17]++;
      if ((pins[0].dir)&&(p_BT[1]))alm[0]++;
      if ((pins[1].dir)&&(p_BT[2]))alm[1]++;
       */
      if (jmp[0])
       {
        for (pj = 5; pj < 13; pj++)
         {
          pinv = pic_get_pin (pj + 1);
          if ((pinv)&&(!pins[9].value)) alm1[pj]++;
          if ((pinv)&&(pins[9].value)) alm2[pj]++;
         }
       }
      j = -1;
     }
    j++;
    pic.ioupdated = 0;
   }

 for (i = 0; i < pic.PINCOUNT; i++)
  {
   pic.pins[i].oavalue = (int) ((alm[i] * RNSTEP) + 55);
   lm1[i] = (int) (((600.0 * alm1[i]) / NSTEPJ) + 30);
   lm2[i] = (int) (((600.0 * alm2[i]) / NSTEPJ) + 30);
   if (lm1[i] > 255)lm1[i] = 255;
   if (lm2[i] > 255)lm2[i] = 255;
  }

 lm1[9] = 30;
 lm2[9] = 30;

 if (use_spare)Window5.PostProcess ();

 //verifiy if LEDS need update 
 for (i = 0; i < 8; i++)
  {
   if (jmp[0])
    {
     //7s DISP
     if (output_ids[O_G1 + i]->value != lm1[5 + i])
      {
       output_ids[O_G1 + i]->value = lm1[5 + i];
       output_ids[O_SS1]->update = 1;
      }
     if (output_ids[O_G2 + i]->value != lm2[5 + i])
      {
       output_ids[O_G2 + i]->value = lm2[5 + i];
       output_ids[O_SS2]->update = 1;
      }
    }
   else
    {
     //PORTB LEDS
     if (output_ids[O_RB0 + i]->value != pic.pins[5 + i].oavalue)
      {
       output_ids[O_RB0 + i]->value = pic.pins[5 + i].oavalue;
       output_ids[O_RB0 + i]->update = 1;
      }
    }
  }

 if (output_ids[O_RA0]->value != pic.pins[16].oavalue)
  {
   output_ids[O_RA0]->value = pic.pins[16].oavalue;
   output_ids[O_RA0]->update = 1;
   output_ids[O_RA0L]->update = 1;
  }
 if (output_ids[O_RA1]->value != pic.pins[17].oavalue)
  {
   output_ids[O_RA1]->value = pic.pins[17].oavalue;
   output_ids[O_RA1]->update = 1;
  }
 if (output_ids[O_RA2]->value != pic.pins[0].oavalue)
  {
   output_ids[O_RA2]->value = pic.pins[0].oavalue;
   output_ids[O_RA2]->update = 1;
  }
 if (output_ids[O_RA3]->value != pic.pins[1].oavalue)
  {
   output_ids[O_RA3]->value = pic.pins[1].oavalue;
   output_ids[O_RA3]->update = 1;
  }
}

void
cboard_McLab1::Reset(void)
{
 pic_reset (1);

 p_BT[0] = 1;
 p_BT[1] = 1;
 p_BT[2] = 1;
 p_BT[3] = 1;

 pic_set_pin (18, p_BT[0]);
 pic_set_pin (1, p_BT[1]);
 pic_set_pin (2, p_BT[2]);
 pic_set_pin (3, p_BT[3]);

 Window1.statusbar1.SetField (2, lxT (""));

 for (int i = 0; i < pic.PINCOUNT; i++)
  {
   lm1[i] = 0;
   lm2[i] = 0;
  }
 if (use_spare)Window5.Reset ();

 RegisterRemoteControl ();
}

void
cboard_McLab1::RegisterRemoteControl(void)
{
 for (int i = 0; i < inputc; i++)
  {
   switch (input[i].id)
    {
    case I_RA1:
     input[i].status = &p_BT[0];
     break;
    case I_RA2:
     input[i].status = &p_BT[1];
     break;
    case I_RA3:
     input[i].status = &p_BT[2];
     break;
    case I_RA4:
     input[i].status = &p_BT[3];
     break;
    case I_JP1:
     input[i].status = &jmp[0];
     break;
    }
  }

 for (int i = 0; i < outputc; i++)
  {
   switch (output[i].id)
    {
    case O_RB0:
     output[i].status = &pic.pins[5].oavalue;
     break;
    case O_RB1:
     output[i].status = &pic.pins[6].oavalue;
     break;
    case O_RB2:
     output[i].status = &pic.pins[7].oavalue;
     break;
    case O_RB3:
     output[i].status = &pic.pins[8].oavalue;
     break;
    case O_RB4:
     output[i].status = &pic.pins[9].oavalue;
     break;
    case O_RB5:
     output[i].status = &pic.pins[10].oavalue;
     break;
    case O_RB6:
     output[i].status = &pic.pins[11].oavalue;
     break;
    case O_RB7:
     output[i].status = &pic.pins[12].oavalue;
     break;
    case O_RA0L:
     output[i].status = &pic.pins[16].oavalue;
     break;
    case O_A1:
     output[i].status = &lm1[7];
     break;
    case O_B1:
     output[i].status = &lm1[8];
     break;
    case O_C1:
     output[i].status = &lm1[10];
     break;
    case O_D1:
     output[i].status = &lm1[11];
     break;
    case O_E1:
     output[i].status = &lm1[12];
     break;
    case O_F1:
     output[i].status = &lm1[6];
     break;
    case O_G1:
     output[i].status = &lm1[5];
     break;
    case O_P1:
     output[i].status = &lm1[9];
     break;
    case O_A2:
     output[i].status = &lm2[7];
     break;
    case O_B2:
     output[i].status = &lm2[8];
     break;
    case O_C2:
     output[i].status = &lm2[10];
     break;
    case O_D2:
     output[i].status = &lm2[11];
     break;
    case O_E2:
     output[i].status = &lm2[12];
     break;
    case O_F2:
     output[i].status = &lm2[6];
     break;
    case O_G2:
     output[i].status = &lm2[5];
     break;
    case O_P2:
     output[i].status = &lm2[9];
     break;
    }
  }
}

void
cboard_McLab1::EvMouseButtonPress(uint button, uint x, uint y, uint state)
{

 int i;

 for (i = 0; i < inputc; i++)
  {
   if (((input[i].x1 <= x)&&(input[i].x2 >= x))&&((input[i].y1 <= y)&&(input[i].y2 >= y)))
    {

     switch (input[i].id)
      {
      case I_ICSP:
       Window1.menu1_File_LoadHex_EvMenuActive (NULL);
       break;
      case I_JP1:
       jmp[0] ^= 0x01;
       output_ids[O_JP1]->update = 1;
       output_ids[O_SS1]->update = 1;
       output_ids[O_SS2]->update = 1;
       output_ids[O_RB0]->update = 1;
       output_ids[O_RB1]->update = 1;
       output_ids[O_RB2]->update = 1;
       output_ids[O_RB3]->update = 1;
       output_ids[O_RB4]->update = 1;
       output_ids[O_RB5]->update = 1;
       output_ids[O_RB6]->update = 1;
       output_ids[O_RB7]->update = 1;
       break;
      case I_PWR:
       {
        if (Window1.Get_mcupwr ())
         {
          Window1.Set_mcupwr (0);
          Reset ();
          p_BT[0] = 1;
          p_BT[1] = 1;
          p_BT[2] = 1;
          p_BT[3] = 1;
         }
        else
         {
          Window1.Set_mcupwr (1);
          Reset ();
         }
       }
       break;
      case I_RST:
       if (Window1.Get_mcupwr () && pic_reset (-1))
        {
         Window1.Set_mcupwr (0);
         Window1.Set_mcurst (1);
        }
       p_RST = 0;
       output_ids[O_RST]->update = 1;
       break;
      case I_RA1:
       p_BT[0] = 0;
       output_ids[O_BRA1]->update = 1;
       break;
      case I_RA2:
       p_BT[1] = 0;
       output_ids[O_BRA2]->update = 1;
       break;
      case I_RA3:
       p_BT[2] = 0;
       output_ids[O_BRA3]->update = 1;
       break;
      case I_RA4:
       p_BT[3] = 0;
       output_ids[O_BRA4]->update = 1;
       break;
      }
    }
  }

}

void
cboard_McLab1::EvMouseButtonRelease(uint button, uint x, uint y, uint state)
{
 int i;

 for (i = 0; i < inputc; i++)
  {
   if (((input[i].x1 <= x)&&(input[i].x2 >= x))&&((input[i].y1 <= y)&&(input[i].y2 >= y)))
    {
     switch (input[i].id)
      {
      case I_RST:
       {
        if (Window1.Get_mcurst ())
         {
          Window1.Set_mcupwr (1);
          Window1.Set_mcurst (0);

          if (pic_reset (-1))
           {
            Reset ();
           }
         }
        p_RST = 1;
        output_ids[O_RST]->update = 1;
       }
       break;

      case I_RA1:
       p_BT[0] = 1;
       output_ids[O_BRA1]->update = 1;
       break;
      case I_RA2:
       p_BT[1] = 1;
       output_ids[O_BRA2]->update = 1;
       break;
      case I_RA3:
       p_BT[2] = 1;
       output_ids[O_BRA3]->update = 1;
       break;
      case I_RA4:
       p_BT[3] = 1;
       output_ids[O_BRA4]->update = 1;
       break;
      }
    }
  }
}

void
cboard_McLab1::EvKeyPress(uint key, uint mask)
{
 if (key == '1')
  {
   p_BT[0] = 0;
   output_ids[O_BRA1]->update = 1;
  }
 if (key == '2')
  {
   p_BT[1] = 0;
   output_ids[O_BRA2]->update = 1;
  }
 if (key == '3')
  {
   p_BT[2] = 0;
   output_ids[O_BRA3]->update = 1;
  }
 if (key == '4')
  {
   p_BT[3] = 0;
   output_ids[O_BRA4]->update = 1;
  }
}

void
cboard_McLab1::EvKeyRelease(uint key, uint mask)
{
 if (key == '1')
  {
   p_BT[0] = 1;
   output_ids[O_BRA1]->update = 1;
  }

 if (key == '2')
  {
   p_BT[1] = 1;
   output_ids[O_BRA2]->update = 1;
  }

 if (key == '3')
  {
   p_BT[2] = 1;
   output_ids[O_BRA3]->update = 1;
  }

 if (key == '4')
  {
   p_BT[3] = 1;
   output_ids[O_BRA4]->update = 1;
  }
}

unsigned short
cboard_McLab1::get_in_id(char * name)
{
 if (strcmp (name, "PB_RST") == 0)return I_RST;
 if (strcmp (name, "PB_RA1") == 0)return I_RA1;
 if (strcmp (name, "PB_RA2") == 0)return I_RA2;
 if (strcmp (name, "PB_RA3") == 0)return I_RA3;
 if (strcmp (name, "PB_RA4") == 0)return I_RA4;
 if (strcmp (name, "SW_PWR") == 0)return I_PWR;
 if (strcmp (name, "PG_ICSP") == 0)return I_ICSP;
 if (strcmp (name, "JP_1") == 0)return I_JP1;

 printf ("Erro input '%s' don't have a valid id! \n", name);
 return -1;
}

unsigned short
cboard_McLab1::get_out_id(char * name)
{

 if (strcmp (name, "LD_RB0") == 0)return O_RB0;
 if (strcmp (name, "LD_RB1") == 0)return O_RB1;
 if (strcmp (name, "LD_RB2") == 0)return O_RB2;
 if (strcmp (name, "LD_RB3") == 0)return O_RB3;
 if (strcmp (name, "LD_RB4") == 0)return O_RB4;
 if (strcmp (name, "LD_RB5") == 0)return O_RB5;
 if (strcmp (name, "LD_RB6") == 0)return O_RB6;
 if (strcmp (name, "LD_RB7") == 0)return O_RB7;

 if (strcmp (name, "LD_RA0") == 0)return O_RA0;
 if (strcmp (name, "LD_RA1") == 0)return O_RA1;
 if (strcmp (name, "LD_RA2") == 0)return O_RA2;
 if (strcmp (name, "LD_RA3") == 0)return O_RA3;

 if (strcmp (name, "LD_RA0L") == 0)return O_RA0L;

 if (strcmp (name, "SS_1") == 0)return O_SS1;
 if (strcmp (name, "SS_2") == 0)return O_SS2;

 if (strcmp (name, "SS_A1") == 0)return O_A1;
 if (strcmp (name, "SS_B1") == 0)return O_B1;
 if (strcmp (name, "SS_C1") == 0)return O_C1;
 if (strcmp (name, "SS_D1") == 0)return O_D1;
 if (strcmp (name, "SS_E1") == 0)return O_E1;
 if (strcmp (name, "SS_F1") == 0)return O_F1;
 if (strcmp (name, "SS_G1") == 0)return O_G1;
 if (strcmp (name, "SS_P1") == 0)return O_P1;

 if (strcmp (name, "SS_A2") == 0)return O_A2;
 if (strcmp (name, "SS_B2") == 0)return O_B2;
 if (strcmp (name, "SS_C2") == 0)return O_C2;
 if (strcmp (name, "SS_D2") == 0)return O_D2;
 if (strcmp (name, "SS_E2") == 0)return O_E2;
 if (strcmp (name, "SS_F2") == 0)return O_F2;
 if (strcmp (name, "SS_G2") == 0)return O_G2;
 if (strcmp (name, "SS_P2") == 0)return O_P2;

 if (strcmp (name, "PB_RA1") == 0)return O_BRA1;
 if (strcmp (name, "PB_RA2") == 0)return O_BRA2;
 if (strcmp (name, "PB_RA3") == 0)return O_BRA3;
 if (strcmp (name, "PB_RA4") == 0)return O_BRA4;

 if (strcmp (name, "JP_1") == 0)return O_JP1;

 if (strcmp (name, "PB_RST") == 0)return O_RST;

 if (strcmp (name, "IC_CPU") == 0)return O_MP;

 printf ("Erro output '%s' don't have a valid id! \n", name);
 return 1;
}

void
cboard_McLab1::WritePreferences(void)
{
 Window1.saveprefs (lxT ("McLab1_proc"), Proc);
 Window1.saveprefs (lxT ("McLab1_jmp"), lxString ().Format ("%i", jmp[0]));
 Window1.saveprefs (lxT ("McLab1_clock"), lxString ().Format ("%2.1f", Window1.GetClock ()));
}

void
cboard_McLab1::ReadPreferences(char *name, char *value)
{
 int i;

 if (!strcmp (name, "McLab1_jmp"))
  {
   for (i = 0; i < 1; i++)
    {
     if (value[i] == '0')
      jmp[i] = 0;
     else
      jmp[i] = 1;
    }
  }

 if (!strcmp (name, "McLab1_proc"))
  {
   Proc = value;
  }

 if (!strcmp (name, "McLab1_clock"))
  {
   Window1.SetClock (atof (value));
  }
}

board_init(BOARD_McLab1_Name, cboard_McLab1);

