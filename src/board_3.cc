/* ########################################################################

   PICsimLab - PIC laboratory simulator

   ########################################################################

   Copyright (c) : 2010-2015  Luis Claudio Gambôa Lopes

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
#include"board_3.h"

/* outputs */

#define O_RB0	1
#define O_RB1	2
#define O_RB2	3
#define O_RB3	4
#define O_LPWR	5
#define O_LCD	6
#define O_A1	7
#define O_B1	8
#define O_C1	9
#define O_D1	10
#define O_E1	11
#define O_F1	12
#define O_G1	13
#define O_P1	14
#define O_A2	15
#define O_B2	16
#define O_C2	17
#define O_D2	18
#define O_E2	19
#define O_F2	20
#define O_G2	21
#define O_P2	22
#define O_A3	23
#define O_B3	24
#define O_C3	25
#define O_D3	26
#define O_E3	27
#define O_F3	28
#define O_G3	29
#define O_P3	30
#define O_A4	31
#define O_B4	32
#define O_C4	33
#define O_D4	34
#define O_E4	35
#define O_F4	36
#define O_G4	37
#define O_P4	38
#define O_JP1	39
#define O_JP2	40
#define O_JP3	41
#define O_JP4	42
#define O_JP5	43
#define O_JP6	44

/*inputs*/
#define I_RST	1
#define I_PWR	2
#define I_ICSP	3
#define I_JP1	4
#define I_JP2	5
#define I_JP3	6
#define I_JP4	7
#define I_JP5	8
#define I_JP6	9
#define I_RB0	10
#define I_RB1	11
#define I_RB2	12
#define I_RB3	13


cboard_3::cboard_3(void)
{
      proc=P18F452;
          
      vp2in=2.5;
      vp2[0]=2.5;
      vp2[1]=2.5;
      temp[0]=27.5;
      temp[1]=27.5;
      ref=0;
      
      rpmstp=0;//(NSTEP*2)/100;
      rpmc=0;


      d=0;
      lcde=0;
      sound_on=0;
      
      buzz.Create(Window1.GetSharePath()+wxT("buzzer.wav"));
      
      lcd_init(&lcd,2);
      mi2c_init(&mi2c,4);
      
      ReadMaps();
      
      jmp[0]=0;
      jmp[1]=0;
      jmp[2]=0;
      jmp[3]=0;
      jmp[4]=0;
      jmp[5]=0;
      
       
  //scroll1
  scroll1=new CScroll();   
  scroll1->SetFOwner(&Window1);
  scroll1->SetName(wxT("scroll1_p3"));
  scroll1->SetX(12);
  scroll1->SetY(273);
  scroll1->SetWidth(140);
  scroll1->SetHeight(22);
  scroll1->SetEnable(1);
  scroll1->SetVisible(1);
  scroll1->SetRange(100);
  scroll1->SetPosition(50);
  scroll1->SetType(4);
  Window1.CreateChild(scroll1);
  //gauge1
  gauge1=new CGauge();
  gauge1->SetFOwner(&Window1);
  gauge1->SetName(wxT("gauge1_p3"));
  gauge1->SetX(13);
  gauge1->SetY(382);
  gauge1->SetWidth(140);
  gauge1->SetHeight(20);
  gauge1->SetEnable(1);
  gauge1->SetVisible(1);
  gauge1->SetRange(100);
  gauge1->SetValue(0);
  gauge1->SetType(4);
  Window1.CreateChild(gauge1);
  //gauge2
  gauge2=new CGauge();
  gauge2->SetFOwner(&Window1);
  gauge2->SetName(wxT("gauge2_p3"));
  gauge2->SetX(12);
  gauge2->SetY(330);
  gauge2->SetWidth(140);
  gauge2->SetHeight(20);
  gauge2->SetEnable(1);
  gauge2->SetVisible(1);
  gauge2->SetRange(100);
  gauge2->SetValue(0);
  gauge2->SetType(4);
  Window1.CreateChild(gauge2);   
  //label1
  label1=new CLabel();
  label1->SetFOwner(&Window1);
  label1->SetName(wxT("label1_p3"));
  label1->SetX(12);
  label1->SetY(249);
  label1->SetWidth(60);
  label1->SetHeight(20);
  label1->SetEnable(1);
  label1->SetVisible(1);
  label1->SetText(wxT("Pot. P2"));
  label1->SetAlign(1);
  Window1.CreateChild(label1);    
  //label2
  label2=new CLabel();
  label2->SetFOwner(&Window1);
  label2->SetName(wxT("label2_p3"));
  label2->SetX(12);
  label2->SetY(306);
  label2->SetWidth(60);
  label2->SetHeight(20);
  label2->SetEnable(1);
  label2->SetVisible(1);
  label2->SetText(wxT("Heater"));
  label2->SetAlign(1);
  Window1.CreateChild(label2);    
  //label4
  label3=new CLabel();
  label3->SetFOwner(&Window1);
  label3->SetName(wxT("label3_p3"));
  label3->SetX(13);
  label3->SetY(357);
  label3->SetWidth(60);
  label3->SetHeight(20);
  label3->SetEnable(1);
  label3->SetVisible(1);
  label3->SetText(wxT("Cooler"));
  label3->SetAlign(1);
  Window1.CreateChild(label3);
  //label5
  label4=new CLabel();
  label4->SetFOwner(&Window1);
  label4->SetName(wxT("label4_p3"));
  label4->SetX(13);
  label4->SetY(412);
  label4->SetWidth(120);
  label4->SetHeight(24);
  label4->SetEnable(1);
  label4->SetVisible(1);
  label4->SetText(wxT("Temp: 00.0°C"));
  label4->SetAlign(1);
  Window1.CreateChild(label4);
 
};

cboard_3::~cboard_3(void)
{
      buzz.Stop();
      mi2c_end(&mi2c);
      
      Window1.DestroyChild(scroll1);
      Window1.DestroyChild(gauge1);
      Window1.DestroyChild(gauge2);
      Window1.DestroyChild(label1);
      Window1.DestroyChild(label2);
      Window1.DestroyChild(label3);
      Window1.DestroyChild(label4);

}

void cboard_3::Draw(_pic *pic, CDraw *draw,double scale)
{
  int i;
  
  draw->Canvas.Init(scale,scale);
  
          

  lcd_blink(&lcd);

//lab3 draw
  for(i=0;i<outputc;i++)
  {
    if(!output[i].r)
    {
       
      draw->Canvas.SetFgColor (30, 0, 0);
      
      switch(output[i].id) 
      {
        case O_A1: draw->Canvas.SetColor (lm1[18], 0, 0);break;
        case O_B1: draw->Canvas.SetColor (lm1[19], 0, 0);break;
        case O_C1: draw->Canvas.SetColor (lm1[20], 0, 0);break;
        case O_D1: draw->Canvas.SetColor (lm1[21], 0, 0);break;
        case O_E1: draw->Canvas.SetColor (lm1[26], 0, 0);break;
        case O_F1: draw->Canvas.SetColor (lm1[27], 0, 0);break;
        case O_G1: draw->Canvas.SetColor (lm1[28], 0, 0);break;
        case O_P1: draw->Canvas.SetColor (lm1[29], 0, 0);break;
          
        case O_A2: draw->Canvas.SetColor (lm2[18], 0, 0);break;
        case O_B2: draw->Canvas.SetColor (lm2[19], 0, 0);break;
        case O_C2: draw->Canvas.SetColor (lm2[20], 0, 0);break;
        case O_D2: draw->Canvas.SetColor (lm2[21], 0, 0);break;
        case O_E2: draw->Canvas.SetColor (lm2[26], 0, 0);break;
        case O_F2: draw->Canvas.SetColor (lm2[27], 0, 0);break;
        case O_G2: draw->Canvas.SetColor (lm2[28], 0, 0);break;
        case O_P2: draw->Canvas.SetColor (lm2[29], 0, 0);break;
      
        case O_A3: draw->Canvas.SetColor (lm3[18], 0, 0);break;
        case O_B3: draw->Canvas.SetColor (lm3[19], 0, 0);break;
        case O_C3: draw->Canvas.SetColor (lm3[20], 0, 0);break;
        case O_D3: draw->Canvas.SetColor (lm3[21], 0, 0);break;
        case O_E3: draw->Canvas.SetColor (lm3[26], 0, 0);break;
        case O_F3: draw->Canvas.SetColor (lm3[27], 0, 0);break;
        case O_G3: draw->Canvas.SetColor (lm3[28], 0, 0);break;
        case O_P3: draw->Canvas.SetColor (lm3[29], 0, 0);break;
      
        case O_A4: draw->Canvas.SetColor (lm4[18], 0, 0);break;
        case O_B4: draw->Canvas.SetColor (lm4[19], 0, 0);break;
        case O_C4: draw->Canvas.SetColor (lm4[20], 0, 0);break;
        case O_D4: draw->Canvas.SetColor (lm4[21], 0, 0);break;
        case O_E4: draw->Canvas.SetColor (lm4[26], 0, 0);break;
        case O_F4: draw->Canvas.SetColor (lm4[27], 0, 0);break;
        case O_G4: draw->Canvas.SetColor (lm4[28], 0, 0);break;
        case O_P4: draw->Canvas.SetColor (lm4[29], 0, 0);break;
     
        case O_LCD: draw->Canvas.SetColor (0, 90*Window1.Get_picpwr()+40, 0);break;

        default : if((output[i].name[0]=='J')&&(output[i].name[1]=='P')) draw->Canvas.SetColor (150, 150, 150);break;
      }

      if(output[i].id != O_LCD)
        draw->Canvas.Rectangle (1, output[i].x1, output[i].y1, output[i].x2-output[i].x1,output[i].y2-output[i].y1 );
 
//draw lcd text 
   
      if((output[i].id == O_LCD)&&(lcd.update))
      {
        lcd_draw(&lcd,draw,output[i].x1, output[i].y1, output[i].x2-output[i].x1,output[i].y2-output[i].y1,Window1.Get_picpwr());           
      }

       
      if((output[i].name[0]=='J')&&(output[i].name[1]=='P'))
      {
       if(!jmp[output[i].name[2]-0x31])
       {
         draw->Canvas.SetColor (70, 70, 70);
         draw->Canvas.Rectangle (1, output[i].x1, output[i].y1, (int)((output[i].x2-output[i].x1)*0.65),output[i].y2-output[i].y1 );
         draw->Canvas.SetColor (220, 220, 0);
         draw->Canvas.Circle (1,output[i].x1+(int)((output[i].x2-output[i].x1)*0.80), output[i].y1+((output[i].y2-output[i].y1)/2),3);
       }
       else
       {
         draw->Canvas.SetColor (70, 70, 70);
         draw->Canvas.Rectangle (1, output[i].x1+((int)((output[i].x2-output[i].x1)*0.35)), output[i].y1, (int)((output[i].x2-output[i].x1)*0.65),output[i].y2-output[i].y1 );
         draw->Canvas.SetColor (220, 220, 0);
         draw->Canvas.Circle (1,output[i].x1+(int)((output[i].x2-output[i].x1)*0.20), output[i].y1+((output[i].y2-output[i].y1)/2),3);
       };
      }       
       
    }
    else
    {
      draw->Canvas.SetFgColor (0, 0, 0);
      
 
        switch(output[i].id)
        {
          case O_RB0: draw->Canvas.SetColor (lm[32], 0, 0);break;
          case O_RB1: draw->Canvas.SetColor (lm[33], 0, 0);break;
          case O_RB2: draw->Canvas.SetColor (lm[34], 0, 0);break;
          case O_RB3: draw->Canvas.SetColor (lm[35], 0, 0);break;
        }
      if(output[i].id == O_LPWR)draw->Canvas.SetColor (0,255*Window1.Get_picpwr(), 0);
      
      draw->Canvas.Circle (1,output[i].x1, output[i].y1,output[i].r );
    };    

  };
//end draw

  draw->Canvas.End();
  draw->Update ();
  
   

     if(( (0.4444*(lm[6]-30)) > 40)&& Window1.Get_picpwr())
     {
       if(!sound_on)
       {
         buzz.Play(wxSOUND_ASYNC|wxSOUND_LOOP); 
         sound_on=1;
       }
     }
     else
     {
       buzz.Stop(); 
       sound_on=0;
     }

     //Ventilador
     gauge1->SetValue(0.4444*(lm[15]-30)); 
     //Aquecedor
     gauge2->SetValue(0.4444*(lm[16]-30)); 

     //sensor ventilador)
     rpmstp=((float)Window1.GetNSTEPJ())/(0.64*(lm[15]-29));
   
     //tensão p2
     vp2in=((5.0*(scroll1->GetPosition()))/(scroll1->GetRange()-1));

     //temperatura 
     ref=(0.2222*(lm[16]-30))-(0.2222*(lm[15]-30)); 

     if(ref < 0)
       ref=0; 
 
       temp[1]=temp[0];    
       temp[0]=((27.5+ref)*0.003)+temp[1]*(0.997);

     pic_set_apin(pic,2,(10.0/255.0)*(temp[0]+15.0));
    
     //referencia
     pic_set_apin(pic,5,2.5);

};

void cboard_3::Run_CPU(_pic *pic)
{
  int i;   
  int j;
  unsigned char pi;
  unsigned char pinv;
  const picpin * pins;
  unsigned int alm[40]; //luminosidade media
  unsigned int alm1[40]; //luminosidade media display
  unsigned int alm2[40]; //luminosidade media display
  unsigned int alm3[40]; //luminosidade media display
  unsigned int alm4[40]; //luminosidade media display     
   
  
  int JUMPSTEPS = Window1.GetJUMPSTEPS();
  long int NSTEPJ=Window1.GetNSTEPJ();
   
  for(pi=0;pi < pic->PINCOUNT;pi++)
     {
       alm[pi]=0;
       alm1[pi]=0;
       alm2[pi]=0;
       alm3[pi]=0;
       alm4[pi]=0;
     };
   
        
 pins = pic->pins;


 j=JUMPSTEPS+1;
 if(Window1.Get_picpwr())
   for(i=0;i<Window1.GetNSTEP();i++)
      {
       
          pic_set_pin(pic,33,p_BT1); 
          pic_set_pin(pic,34,p_BT2); 
          pic_set_pin(pic,35,p_BT3); 
          pic_set_pin(pic,36,p_BT4);
 
          if(j > JUMPSTEPS)
          {  
          
          rpmc++;
          if(rpmc > rpmstp) 
          {
             rpmc=0;
             pic_set_pin(pic,15, !pic_get_pin(pic,15));
          }
          }
         
          if(!mplabxd_testbp(pic))pic_step(pic,0);

          if(j > JUMPSTEPS)
          {  
/*
        for(pi=0;pi < pic->PINCOUNT;pi++)
        {
           //if((!pins[pi].dir)&&(pins[pi].value)) alm[pi]++;
           alm[pi]+=pins[pi].value;
        }
*/       

//output 
          alm[32]+=pins[32].value;
          alm[33]+=pins[33].value;
          alm[34]+=pins[34].value;
          alm[35]+=pins[35].value;

          alm[6]+=pins[6].value;
          alm[15]+=pins[15].value;
          alm[16]+=pins[16].value;


//input
          if((pins[32].dir)&&(p_BT1))alm[32]++; 
          if((pins[33].dir)&&(p_BT2))alm[33]++; 
          if((pins[34].dir)&&(p_BT3))alm[34]++; 
          if((pins[35].dir)&&(p_BT4))alm[35]++; 

  
            for(pi=18;pi<30;pi++)
            {
              pinv=pins[pi].value;
              if((pinv)&&(pins[39].value)) alm1[pi]++;
              if((pinv)&&(pins[38].value)) alm2[pi]++;
              if((pinv)&&(pins[37].value)) alm3[pi]++;
              if((pinv)&&(pins[36].value)) alm4[pi]++;
            }
     

          j=0;
          } 
          j++;
          
//potênciometro p2
//p2 rc circuit

   if(!pins[2].dir)
     {
        //decarga por RA1  
        vp2[1]=vp2[0]=5*pins[2].value;
     }

     vp2[1]=vp2[0];

     vp2[0]=vp2in*0.00021+vp2[1]*0.99979;
 
     if(pins[2].ptype < 3)
      pic_set_pin(pic,3,vp2[0] > 1.25);
     else
      pic_set_apin(pic,3,vp2[0]);          
          
               
//lcd dipins[2].dirsplay code
    if((!pins[8].dir)&&(!pins[8].value))
    {
      if(!lcde)
      {
      d=0; 
      if(pins[29].value) d|= 0x80;
      if(pins[28].value) d|= 0x40;
      if(pins[27].value) d|= 0x20;
      if(pins[26].value) d|= 0x10;
      if(pins[21].value) d|= 0x08;
      if(pins[20].value) d|= 0x04;
      if(pins[19].value) d|= 0x02;
      if(pins[18].value) d|= 0x01;

      if((!pins[7].dir)&&(!pins[7].value))
      {
        lcd_cmd(&lcd,d);
      }
      else if((!pins[7].dir)&&(pins[7].value))
      {
        lcd_data(&lcd,d);
      }
      lcde=1;
      }    

    }
    else
    {
      lcde=0;
    }        
     
//i2c code
        if(pins[22].dir)
        {
          sda=1;
        }
        else
        {
          sda=pins[22].value;
        }
        
        if(pins[17].dir)
        {
          sck=1;
	  pic_set_pin(pic,18,1);
        }
        else
        {
          sck=pins[17].value;
        }
	pic_set_pin(pic,23,mi2c_io(&mi2c,sck,sda));

 }
   //fim STEP

      for(pi=0;pi < pic->PINCOUNT;pi++)
     { 
      lm[pi]= (int)(((225.0*alm[pi])/NSTEPJ)+30);

      lm1[pi]= (int)(((600.0*alm1[pi])/NSTEPJ)+30);
      lm2[pi]= (int)(((600.0*alm2[pi])/NSTEPJ)+30);
      lm3[pi]= (int)(((600.0*alm3[pi])/NSTEPJ)+30);
      lm4[pi]= (int)(((600.0*alm4[pi])/NSTEPJ)+30);
      if(lm1[pi] > 255)lm1[pi]=255;
      if(lm2[pi] > 255)lm2[pi]=255;
      if(lm3[pi] > 255)lm3[pi]=255;
      if(lm4[pi] > 255)lm4[pi]=255;
     }

}


void 
cboard_3::Reset(_pic *pic)
{
                
  lcd_rst(&lcd);   
  mi2c_rst(&mi2c);
  
  
    p_BT1=1; 
    p_BT2=1; 
    p_BT3=1; 
    p_BT4=1; 

  
    pic_set_pin(pic,33,p_BT1); 
    pic_set_pin(pic,34,p_BT2); 
    pic_set_pin(pic,35,p_BT3); 
    pic_set_pin(pic,36,p_BT4); 
    
#ifndef _WIN_
    if(pic->serialfd > 0)
#else
    if(pic->serialfd != INVALID_HANDLE_VALUE)
#endif
      Window1.statusbar1.SetField(2,wxT("Serial Port: ")+String::FromAscii(SERIALDEVICE)+wxT(":")+itoa(pic->serialbaud)+wxT("(")+String().Format("%4.1f",fabs((100.0*pic->serialexbaud-100.0*pic->serialbaud)/pic->serialexbaud))+wxT("%)"));
    else  
      Window1.statusbar1.SetField(2,wxT("Serial Port: ")+String::FromAscii(SERIALDEVICE)+wxT(" (ERROR)"));

  for(int pi=0;pi < pic->PINCOUNT;pi++)
     {
       lm[pi]=0;
       lm1[pi]=0;
       lm2[pi]=0;
       lm3[pi]=0;
       lm4[pi]=0;
     };
       
};

void 
cboard_3::MouseButtonPress(_pic *pic, uint button, uint x, uint y,uint state)
{
 
  int i;
    
  for(i=0;i<inputc;i++)
  {
    if(((input[i].x1 <= x)&&(input[i].x2 >= x))&&((input[i].y1 <= y)&&(input[i].y2 >= y)))
    {
   
      switch(input[i].id) 
      { 
      case I_ICSP:
      {
          Window1.menu1_File_LoadHex_EvMenuActive(NULL);
      };break;  
      
      case I_JP1:
      {
        jmp[0]^=0x01; 
      };break;
      case I_JP2:
      {
        jmp[1]^=0x01; 
      };break;
      case I_JP3:
      {
        jmp[2]^=0x01; 
      };break;
      case I_JP4:
      {
        jmp[3]^=0x01; 
      };break;
      case I_JP5:
      {
        jmp[4]^=0x01; 
      };break;
      case I_JP6:
      {
        jmp[5]^=0x01; 
      };break;
 

      case I_PWR:
      {
        if(Window1.Get_picpwr()) 
        { 
          Window1.Set_picrun(0); 
          Window1.Set_picpwr(0); 
          pic_reset(pic,1);
          Reset(pic);

          p_BT1=0; 
          p_BT2=0; 
          p_BT3=0; 
          p_BT4=0; 
          Window1.statusbar1.SetField(0,wxT("Stoped"));
        }
        else
        {
          Window1.Set_picpwr(1);
          Window1.Set_picrun(1);
          pic_reset(pic,1);          
          Reset(pic);
     
          
          Window1.statusbar1.SetField(0,wxT("Running..."));
        } 
      };break;
      
      case I_RST:
      {
          if(Window1.Get_picpwr())
          {
             Window1.Set_picpwr(0);
             Window1.Set_picrst(1);
          } 
      };break;
      
   
      case I_RB0:
        {
          p_BT1=0;
        };break;
      case I_RB1:
        {
          p_BT2=0; 
        };break;
      case I_RB2:
        {
          p_BT3=0; 
        };break;
      case I_RB3:
        {
          p_BT4=0; 
        };break;
 
      }
    }
  }
    
    
};

void 
cboard_3::MouseButtonRelease(_pic *pic, uint button, uint x, uint y,uint state)
{    
  int i;

  for(i=0;i<inputc;i++)
  {
    if(((input[i].x1 <= x)&&(input[i].x2 >= x))&&((input[i].y1 <= y)&&(input[i].y2 >= y)))
    {
      switch(input[i].id)
      {
      case I_RST:
      {
          if(Window1.Get_picrst())
          {
            Window1.Set_picpwr(1);
            Window1.Set_picrst(0);

            if(pic_reset(pic,-1))
            { 
              lcd_rst(&lcd);
	      mi2c_rst(&mi2c);

              
              Reset(pic);
        
            }
          } 
      };break;
      
        case I_RB0:
        {
          p_BT1=1; 
        };break;
        case I_RB1:
        {
          p_BT2=1; 
        };break;
        case I_RB2:
        {
          p_BT3=1; 
        };break;
        case I_RB3:
        {
          p_BT4=1; 
        };break;
     
      }
    } 
  }

};



void 
cboard_3::KeyPress(_pic *pic, uint key, uint x, uint y,uint mask)
{
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
};

void
cboard_3::KeyRelease(_pic *pic, uint key, uint x, uint y,uint mask)
{
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
};

void
cboard_3::OnShow(void)
{
    lcd.update=1;
}


unsigned short 
cboard_3::get_in_id(char * name)
{
  if(strcmp(name,"RST")==0)return I_RST; 

  if(strcmp(name,"PWR")==0)return I_PWR;
  if(strcmp(name,"ICSP")==0)return I_ICSP;
  
  if(strcmp(name,"RB0")==0)return I_RB0;
  if(strcmp(name,"RB1")==0)return I_RB1;
  if(strcmp(name,"RB2")==0)return I_RB2;
  if(strcmp(name,"RB3")==0)return I_RB3;

  if(strcmp(name,"JP1")==0)return I_JP1;
  if(strcmp(name,"JP2")==0)return I_JP2;
  if(strcmp(name,"JP3")==0)return I_JP3;
  if(strcmp(name,"JP4")==0)return I_JP4;
  if(strcmp(name,"JP5")==0)return I_JP5;
  if(strcmp(name,"JP6")==0)return I_JP6;
  
  printf("Erro input '%s' don't have a valid id! \n",name);
  return -1;
};

unsigned short 
cboard_3::get_out_id(char * name)
{

  if(strcmp(name,"RB0")==0)return O_RB0;
  if(strcmp(name,"RB1")==0)return O_RB1;
  if(strcmp(name,"RB2")==0)return O_RB2;
  if(strcmp(name,"RB3")==0)return O_RB3;

  if(strcmp(name,"A1")==0)return O_A1;	
  if(strcmp(name,"B1")==0)return O_B1;	
  if(strcmp(name,"C1")==0)return O_C1;	
  if(strcmp(name,"D1")==0)return O_D1;	
  if(strcmp(name,"E1")==0)return O_E1;	
  if(strcmp(name,"F1")==0)return O_F1;	
  if(strcmp(name,"G1")==0)return O_G1;	
  if(strcmp(name,"P1")==0)return O_P1;	

  if(strcmp(name,"A2")==0)return O_A2;	
  if(strcmp(name,"B2")==0)return O_B2;	
  if(strcmp(name,"C2")==0)return O_C2;	
  if(strcmp(name,"D2")==0)return O_D2;	
  if(strcmp(name,"E2")==0)return O_E2;	
  if(strcmp(name,"F2")==0)return O_F2;	
  if(strcmp(name,"G2")==0)return O_G2;	
  if(strcmp(name,"P2")==0)return O_P2;	

  if(strcmp(name,"LPWR")==0)return O_LPWR;	
  if(strcmp(name,"LCD")==0)return O_LCD;	

  if(strcmp(name,"A3")==0)return O_A3;	
  if(strcmp(name,"B3")==0)return O_B3;	
  if(strcmp(name,"C3")==0)return O_C3;	
  if(strcmp(name,"D3")==0)return O_D3;	
  if(strcmp(name,"E3")==0)return O_E3;	
  if(strcmp(name,"F3")==0)return O_F3;	
  if(strcmp(name,"G3")==0)return O_G3;	
  if(strcmp(name,"P3")==0)return O_P3;	
  
  if(strcmp(name,"A4")==0)return O_A4;	
  if(strcmp(name,"B4")==0)return O_B4;	
  if(strcmp(name,"C4")==0)return O_C4;	
  if(strcmp(name,"D4")==0)return O_D4;	
  if(strcmp(name,"E4")==0)return O_E4;	
  if(strcmp(name,"F4")==0)return O_F4;	
  if(strcmp(name,"G4")==0)return O_G4;	
  if(strcmp(name,"P4")==0)return O_P4;	
  
  if(strcmp(name,"JP1")==0)return O_JP1;
  if(strcmp(name,"JP2")==0)return O_JP2;
  if(strcmp(name,"JP3")==0)return O_JP3;
  if(strcmp(name,"JP4")==0)return O_JP4;
  if(strcmp(name,"JP5")==0)return O_JP5;
  if(strcmp(name,"JP6")==0)return O_JP6;
  
  printf("Erro output '%s' don't have a valid id! \n",name);
  return 1;
};


void 
cboard_3::RefreshStatus(_pic *pic)
{
    label4->SetText(wxT("Temp: ")+String().Format("%5.2f",temp[0])+wxT("°C"));
    
#ifndef _WIN_
    if(pic->serialfd > 0)
#else
    if(pic->serialfd != INVALID_HANDLE_VALUE)
#endif
      Window1.statusbar1.SetField(2,wxT("Serial Port: ")+String::FromAscii(SERIALDEVICE)+wxT(":")+itoa(pic->serialbaud)+wxT("(")+String().Format("%4.1f",fabs((100.0*pic->serialexbaud-100.0*pic->serialbaud)/pic->serialexbaud))+wxT("%)"));
    else  
      Window1.statusbar1.SetField(2,wxT("Serial Port: ")+String::FromAscii(SERIALDEVICE)+wxT(" (ERROR)"));

};

void 
cboard_3::WritePreferences(void)
{
    char line[100];
    Window1.saveprefs(wxT("p3_proc"),getnamebyproc(proc,line));
   
    line[0]=0;
    for(int i=0;i<6;i++)
      sprintf(line+i,"%i",jmp[i]);
    
    Window1.saveprefs(wxT("p3_jmp"),line);
};

void 
cboard_3::ReadPreferences(char *name,char *value)
{
    if(!strcmp(name,"p3_proc"))
    {
      proc=getprocbyname(value); 
    }
    
      int i;  
      if(!strcmp(name,"p3_jmp"))
      {
        for(i=0;i<6;i++) 
        {   
          if(value[i] == '0')      
            jmp[i]=0;
          else
            jmp[i]=1;
        }
      }
};