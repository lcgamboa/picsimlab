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
#include"board_1.h"

/* outputs */
#define O_RB0	1
#define O_RB1	2
#define O_RB2	3
#define O_RB3	4
#define O_RB4	5
#define O_RB5	6
#define O_RB6	7
#define O_RB7	8

#define O_RA0	9
#define O_RA1	10
#define O_RA2	11
#define O_RA3	12

#define O_RA0L	13

#define O_A1	14
#define O_B1	15
#define O_C1	16
#define O_D1	17
#define O_E1	18
#define O_F1	19
#define O_G1	20
#define O_P1	21

#define O_A2	22
#define O_B2	23
#define O_C2	24
#define O_D2	25
#define O_E2	26
#define O_F2	27
#define O_G2	28
#define O_P2	29
#define O_JP1	30


/*inputs*/
#define I_RST	1
#define I_RA1	2
#define I_RA2	3
#define I_RA3	4
#define I_RA4	5
#define I_PWR	6 
#define I_ICSP	7
#define I_JP1	8



cboard_1::cboard_1(void)
{
    proc=P16F628A;
    ReadMaps();
    jmp[0]=0;    
    
  //gauge1
  gauge1=new CGauge();
  gauge1->SetFOwner(&Window1);
  gauge1->SetName(wxT("gauge1_p1"));
  gauge1->SetX(13);
  gauge1->SetY(242);
  gauge1->SetWidth(140);
  gauge1->SetHeight(20);
  gauge1->SetEnable(1);
  gauge1->SetVisible(1);
  gauge1->SetRange(100);
  gauge1->SetValue(0);
  gauge1->SetType(4);
  Window1.CreateChild(gauge1);
  //label1
  label1=new CLabel();
  label1->SetFOwner(&Window1);
  label1->SetName(wxT("label1_p1"));
  label1->SetX(12);
  label1->SetY(220);
  label1->SetWidth(60);
  label1->SetHeight(20);
  label1->SetEnable(1);
  label1->SetVisible(1);
  label1->SetText(wxT("LAMP"));
  label1->SetAlign(1);
  Window1.CreateChild(label1);   
};

cboard_1::~cboard_1(void)
{
      Window1.DestroyChild(gauge1);
      Window1.DestroyChild(label1);
}

void cboard_1::Draw(_pic *pic, CDraw *draw,double scale)
{
  int i;
  
  draw->Canvas.Init(scale,scale);
  


//lab1 draw 
  for(i=0;i<outputc;i++)
  {
    if(!output[i].r)
    {
      draw->Canvas.SetFgColor (30, 0, 0);
      
      if(jmp[0])
      {
          switch(output[i].id)
          {         
            case O_A1: draw->Canvas.SetColor (lm1[7], 0, 0);break;
            case O_B1: draw->Canvas.SetColor (lm1[8], 0, 0);break;
            case O_C1: draw->Canvas.SetColor (lm1[10], 0, 0);break;
            case O_D1: draw->Canvas.SetColor (lm1[11], 0, 0);break;
            case O_E1: draw->Canvas.SetColor (lm1[12], 0, 0);break;
            case O_F1: draw->Canvas.SetColor (lm1[6], 0, 0);break;
            case O_G1: draw->Canvas.SetColor (lm1[5], 0, 0);break;
            case O_P1: draw->Canvas.SetColor (lm1[9], 0, 0);break;
            case O_A2: draw->Canvas.SetColor (lm2[7], 0, 0);break;
            case O_B2: draw->Canvas.SetColor (lm2[8], 0, 0);break;
            case O_C2: draw->Canvas.SetColor (lm2[10], 0, 0);break;
            case O_D2: draw->Canvas.SetColor (lm2[11], 0, 0);break;
            case O_E2: draw->Canvas.SetColor (lm2[12], 0, 0);break;
            case O_F2: draw->Canvas.SetColor (lm2[6], 0, 0);break;
            case O_G2: draw->Canvas.SetColor (lm2[5], 0, 0);break;
            case O_P2: draw->Canvas.SetColor (lm2[9], 0, 0);break;
          }
      }
      else
      {
        draw->Canvas.SetColor (30, 0, 0);
      } 

      if(output[i].id == O_JP1) draw->Canvas.SetColor (150, 150, 150);

      draw->Canvas.Rectangle (1, output[i].x1, output[i].y1, output[i].x2-output[i].x1,output[i].y2-output[i].y1 );

       
      if(output[i].id == O_JP1)
      {
       if(!jmp[0])
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
      
 
      if(!jmp[0])
      {
        switch(output[i].id)
        {
          case O_RB0: draw->Canvas.SetColor (lm[5], 0, 0);break;
          case O_RB1: draw->Canvas.SetColor (lm[6], 0, 0);break;
          case O_RB2: draw->Canvas.SetColor (lm[7], 0, 0);break;
          case O_RB3: draw->Canvas.SetColor (lm[8], 0, 0);break;
          case O_RB4: draw->Canvas.SetColor (lm[9], 0, 0);break;
          case O_RB5: draw->Canvas.SetColor (lm[10], 0, 0);break;
          case O_RB6: draw->Canvas.SetColor (lm[11], 0, 0);break;
          case O_RB7: draw->Canvas.SetColor (lm[12], 0, 0);break;
        }
      }
      else
      {
         if((output[i].name[0]=='R')&&(output[i].name[1]=='B'))
            draw->Canvas.SetColor (30, 0, 0);
      }

      switch(output[i].id)
      {
        case O_RA0: draw->Canvas.SetColor (0, lm[16], 0);break;
        case O_RA1: draw->Canvas.SetColor (0, lm[17], 0);break;
        case O_RA2: draw->Canvas.SetColor (0, lm[0], 0);break;
        case O_RA3: draw->Canvas.SetColor (0, lm[1], 0);break;
        case O_RA0L: draw->Canvas.SetColor (lm[16],lm[16], 0);break;
      }
      
      draw->Canvas.Circle (1,output[i].x1, output[i].y1,output[i].r );
    };    

  };

//end draw

  draw->Canvas.End();
  draw->Update ();
  
//Lâmpada
  gauge1->SetValue(0.4444*(lm[16]-30)); 
  
}

 
void cboard_1::Run_CPU(_pic *pic)
{
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
          pic_set_pin(pic,18,p_BT1); 
          pic_set_pin(pic,1,p_BT2); 
          pic_set_pin(pic,2,p_BT3); 
          pic_set_pin(pic,3,p_BT4);
          } 
        
        if(!mplabxd_testbp(pic))pic_step(pic,0);

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
                pinv=pic_get_pin(pic,pi+1);
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
}
 

void 
cboard_1::Reset(_pic *pic)
{          
    p_BT1=1; 
    p_BT2=1; 
    p_BT3=1; 
    p_BT4=1; 
          
    pic_set_pin(pic,18,p_BT1); 
    pic_set_pin(pic,1,p_BT2); 
    pic_set_pin(pic,2,p_BT3); 
    pic_set_pin(pic,3,p_BT4); 
    
    Window1.statusbar1.SetField(2,wxT(""));

   for(int i=0;i < pic->PINCOUNT;i++)
   {
     lm[i]=0;
     lm1[i]=0;
     lm2[i]=0;
   };

};




void 
cboard_1::MouseButtonPress(_pic *pic, uint button, uint x, uint y,uint state)
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
      
      case I_RA1:
        {
          p_BT1=0;
        };break;
      case I_RA2:
        {
          p_BT2=0; 
        };break;
      case I_RA3:
        {
          p_BT3=0; 
        };break;
      case I_RA4:
        {
          p_BT4=0; 
        };break;
    
      }
    }
  }
    
};


void 
cboard_1::MouseButtonRelease(_pic *pic, uint button, uint x, uint y,uint state)
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
              Reset(pic);
            }
          } 
      };break;
      
        case I_RA1:
        {
          p_BT1=1; 
        };break;
        case I_RA2:
        {
          p_BT2=1; 
        };break;
        case I_RA3:
        {
          p_BT3=1; 
        };break;
        case I_RA4:
        {
          p_BT4=1; 
        };break;
   

      }
    } 
  }

};



void 
cboard_1::KeyPress(_pic *pic, uint key, uint x, uint y,uint mask)
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
cboard_1::KeyRelease(_pic *pic, uint key, uint x, uint y,uint mask)
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

unsigned short 
cboard_1::get_in_id(char * name)
{
  if(strcmp(name,"RST")==0)return I_RST; 
  if(strcmp(name,"RA1")==0)return I_RA1;
  if(strcmp(name,"RA2")==0)return I_RA2;
  if(strcmp(name,"RA3")==0)return I_RA3;
  if(strcmp(name,"RA4")==0)return I_RA4;
  if(strcmp(name,"PWR")==0)return I_PWR;
  if(strcmp(name,"ICSP")==0)return I_ICSP;
  if(strcmp(name,"JP1")==0)return I_JP1;
  
  printf("Erro input '%s' don't have a valid id! \n",name);
  return -1;
};

unsigned short 
cboard_1::get_out_id(char * name)
{

  if(strcmp(name,"RB0")==0)return O_RB0;
  if(strcmp(name,"RB1")==0)return O_RB1;
  if(strcmp(name,"RB2")==0)return O_RB2;
  if(strcmp(name,"RB3")==0)return O_RB3;
  if(strcmp(name,"RB4")==0)return O_RB4;
  if(strcmp(name,"RB5")==0)return O_RB5;
  if(strcmp(name,"RB6")==0)return O_RB6;
  if(strcmp(name,"RB7")==0)return O_RB7;
  
  if(strcmp(name,"RA0")==0)return O_RA0;	
  if(strcmp(name,"RA1")==0)return O_RA1;	
  if(strcmp(name,"RA2")==0)return O_RA2;	
  if(strcmp(name,"RA3")==0)return O_RA3;		

  if(strcmp(name,"RA0L")==0)return O_RA0L;	

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
  
  if(strcmp(name,"JP1")==0)return O_JP1;	

  printf("Erro output '%s' don't have a valid id! \n",name);
  return 1;
};


void 
cboard_1::WritePreferences(void)
{
    char line[100];
    Window1.saveprefs(wxT("p1_proc"),getnamebyproc(proc,line));
    Window1.saveprefs(wxT("p1_jmp"),String::Format("%i",jmp[0]));
};

void 
cboard_1::ReadPreferences(char *name,char *value)
{
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
};