/* ########################################################################

   PICsimLab - PIC laboratory simulator

   ########################################################################

   Copyright (c) : 2010-2017  Luis Claudio Gambôa Lopes

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

#include"picsimlab1.h"
#include"picsimlab4.h"
#include"part_servo.h"

/* outputs */
enum {O_P1,O_AXIS};



cpart_servo::cpart_servo(unsigned x, unsigned y)
{
   X=x;
   Y=y;      
   input_pin=0;
           
   ReadMaps();   
     
   wxImage image;
   image.LoadFile(Window1.GetSharePath()+GetPictureFileName());

   Bitmap = new wxBitmap(image);
   BackGround = new wxBitmap(image);
   
   canvas.Create(Bitmap);
};

cpart_servo::~cpart_servo(void)
{
    delete Bitmap;
    delete BackGround;
}

 
float angle;

void cpart_servo::Draw(void)
{ 
   board *pboard=Window1.GetBoard();
  
   canvas.SetBitmap (BackGround,1.0,1.0);
  
   angle+=0.1;
   
   if(angle >= 360)angle-=360;
   
   
  int i;
  
  canvas.Init();
  
 
  for(i=0;i<outputc;i++)
  {

      if(output[i].id == O_P1)
        {
         canvas.SetFgColor (255, 255, 255);
         if(input_pin == 0)
            canvas.Text ("NC",output[i].x1,output[i].y1);
         else
            canvas.Text (pboard->MGetPinName (input_pin),output[i].x1,output[i].y1);  
        }

      if(output[i].id == O_AXIS)
        {
         canvas.SetFgColor (255, 255, 255);
         canvas.SetLineWidth (20);
         canvas.Line (output[i].x1,output[i].y1,output[i].x1+output[i].r*sin(angle),output[i].y1+output[i].r*cos(angle));
        }

      
  };

  canvas.End();
    
}

 
void cpart_servo::Process(void)
{
  /*
  int i;
  int j;
  unsigned char pi;
  unsigned char pinv;
  const picpin * pins;
  unsigned int alm[18]; //luminosidade media
  unsigned int alm1[18]; //luminosidade media display
  unsigned int alm2[18]; //luminosidade media display
     
     
  int JUMPSTEPS = Window1.GetJUMPSTEPS();
  long int NSTEPJ=Window1.GetNSTEPJ();
  
  for(i=0;i < pic->PINCOUNT;i++)
  {
     alm[i]=0;
     alm1[i]=0;
     alm2[i]=0;
  };

    
 pins = pic->pins;
 
 j=JUMPSTEPS+1;
 if(Window1.Get_picpwr())
   for(i=0;i<Window1.GetNSTEP();i++)
      {
 
          if(j > JUMPSTEPS)
          {  
          pic_set_pin(18,p_BT1); 
          pic_set_pin(1,p_BT2); 
          pic_set_pin(2,p_BT3); 
          pic_set_pin(3,p_BT4);
          } 
        
        if(!mplabxd_testbp())pic_step();
        if(use_oscope)Window4.SetSample();
        
          if(j > JUMPSTEPS)
          {  
        for(pi=0;pi < pic->PINCOUNT;pi++)
        {
           alm[pi]+=pins[pi].value;
           //if((!pins[pi].dir)&&(pins[pi].value)) alm[pi]++;
        }
        
          //pull-up extern 
          if((pins[17].dir)&&(p_BT1))alm[17]++; 
          if((pins[0].dir)&&(p_BT2))alm[0]++; 
          if((pins[1].dir)&&(p_BT3))alm[1]++;

            if(jmp[0])
            {
              for(pi=5;pi<13;pi++)
              {
                pinv=pic_get_pin(pi+1);
                if((pinv)&&(!pins[9].value)) alm1[pi]++;
                if((pinv)&&(pins[9].value)) alm2[pi]++;
              }
            }
          j=0;
          } 
          j++;
     }
    
     for(i=0;i < pic->PINCOUNT;i++)
     { 
      lm[i]= (int)(((225.0*alm[i])/NSTEPJ)+30);
      lm1[i]= (int)(((600.0*alm1[i])/NSTEPJ)+30);
      lm2[i]= (int)(((600.0*alm2[i])/NSTEPJ)+30);
      if(lm1[i] > 255)lm1[i]=255;
      if(lm2[i] > 255)lm2[i]=255;
     }

     lm1[9]=30; 
     lm2[9]=30;
     */
}
 

void 
cpart_servo::Reset(void)
{          
  /*
    p_BT1=1; 
    p_BT2=1; 
    p_BT3=1; 
    p_BT4=1; 
          
    pic_set_pin(18,p_BT1); 
    pic_set_pin(1,p_BT2); 
    pic_set_pin(2,p_BT3); 
    pic_set_pin(3,p_BT4); 
    
    Window1.statusbar1.SetField(2,wxT(""));

   for(int i=0;i < pic->PINCOUNT;i++)
   {
     lm[i]=0;
     lm1[i]=0;
     lm2[i]=0;
   };
*/
};




void 
cpart_servo::MouseButtonPress(uint button, uint x, uint y,uint state)
{
 /*
  int i;
    
  for(i=0;i<inputc;i++)
  {
    if(((input[i].x1 <= x)&&(input[i].x2 >= x))&&((input[i].y1 <= y)&&(input[i].y2 >= y)))
    {
   
      switch(input[i].id) 
      {   
        case I_P1:
           Message("P1");
           break; 
      }
    }
  }
 */
};


void 
cpart_servo::MouseButtonRelease(uint button, uint x, uint y,uint state)
{
  /*  
  int i;

  for(i=0;i<inputc;i++)
  {
    if(((input[i].x1 <= x)&&(input[i].x2 >= x))&&((input[i].y1 <= y)&&(input[i].y2 >= y)))
    {
      switch(input[i].id)
      {
        case I_P1:
        break;
      }
    } 
  }
   */ 
};



void 
cpart_servo::KeyPress( uint key, uint x, uint y,uint mask)
{
  /*
  if(key == '1')
  {
    p_BT1=0; 
  }
  if(key == '2')
  {
    p_BT2=0; 
  }
  if(key == '3')
  {
    p_BT3=0; 
  }
  if(key == '4')
  {
    p_BT4=0; 
  }
  */
};

void
cpart_servo::KeyRelease(uint key, uint x, uint y,uint mask)
{
  /*
  if(key == '1')
  {
    p_BT1=1; 
  }
  
  if(key == '2')
  {
    p_BT2=1; 
  }
  
  if(key == '3')
  {
    p_BT3=1; 
  }
  
  if(key == '4')
  {
    p_BT4=1; 
  }
  */
};

unsigned short 
cpart_servo::get_in_id(char * name)
{
  printf("Erro input '%s' don't have a valid id! \n",name);
  return -1;
};

unsigned short 
cpart_servo::get_out_id(char * name)
{

  if(strcmp(name,"P1")==0)return O_P1;
  if(strcmp(name,"AXIS")==0)return O_AXIS;

  printf("Erro output '%s' don't have a valid id! \n",name);
  return 1;
};


String
cpart_servo::WritePreferences(void)
{
  /*
    char line[100];
    Window1.saveprefs(wxT("p1_proc"),getnamebyproc(proc,line));
    Window1.saveprefs(wxT("p1_jmp"),String::Format("%i",jmp[0]));
    */
  return "";
};

void 
cpart_servo::ReadPreferences(String value)
{
  /*
    int i;
    
     if(!strcmp(name,"p1_jmp"))
      {
        for(i=0;i<1;i++)
        {
          if(value[i] == '0')      
            jmp[i]=0;
          else
            jmp[i]=1;
        } 
      }
    
    if(!strcmp(name,"p1_proc"))
    {
      proc=getprocbyname(value); 
    }
    */
};

CPWindow * WProp_servo;

void 
cpart_servo::ConfigurePropiertsWindow(CPWindow *  wprop)
{
    String Items="0  NC,";
    String spin;
    WProp_servo=wprop;
    board *pboard=Window1.GetBoard();
    
    for(int i=1; i<= pboard->MGetPinCount();i++ )
    {
       spin= pboard->MGetPinName(i);
      
      if(spin.Cmp(wxT("error")))
      {
        Items=Items+itoa(i)+"  "+spin+",";
      }
    }
    
    ((CCombo*)WProp_servo->GetChildByName("combo1"))->SetItems(Items);
    if(input_pin == 0)
        ((CCombo*)WProp_servo->GetChildByName("combo1"))->SetText("0  NC");
    else
    {
        spin= pboard->MGetPinName(input_pin);
        ((CCombo*)WProp_servo->GetChildByName("combo1"))->SetText(itoa(input_pin)+"  "+spin);
    }
    
  
    ((CButton*)WProp_servo->GetChildByName("button1"))->EvMouseButtonRelease = EVMOUSEBUTTONRELEASE & cpart_servo::PropButton;
    ((CButton*)WProp_servo->GetChildByName("button1"))->SetTag(1);
    
    ((CButton*)WProp_servo->GetChildByName("button2"))->EvMouseButtonRelease = EVMOUSEBUTTONRELEASE & cpart_servo::PropButton;
}

void 
cpart_servo::ReadPropiertsWindow(void)
{
   if(WProp_servo->GetTag())
   {
      input_pin=atoi(((CCombo*)WProp_servo->GetChildByName("combo1"))->GetText());
   }
} 

void
cpart_servo::PropButton (CControl * control, uint button, uint x, uint y, uint state)
{
  WProp_servo->SetTag(control->GetTag());   
  WProp_servo->HideExclusive ();
};