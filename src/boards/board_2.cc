/* ########################################################################

   PICsimLab - PIC laboratory simulator

   ########################################################################

   Copyright (c) : 2010-2018  Luis Claudio Gambôa Lopes

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
#include"board_2.h"

/* outputs */
#define O_RA1	1
#define O_RA2	2
#define O_RA6	3
#define O_RA7	4
#define O_LPWR	5
#define O_LCD	6


/*inputs*/
#define I_RST	1
#define I_PWR	2
#define I_ICSP	3
#define I_TC1   4
#define I_TC2   5
#define I_TC3   6
#define I_TC4   7
#define I_TC5   8
#define I_TC6   9
#define I_TC7   10
#define I_TC8   11
#define I_TC9   12
#define I_TCA   13
#define I_TC0   14
#define I_TCT   15


cboard_2::cboard_2(void)
{
      proc="PIC16F628A";
          
      clko=0;
      d=0;
      lcde=0;
      
      lcd_init(&lcd,2);
      mi2c_init(&mi2c,512);
      rtc_init(&rtc);
      ReadMaps();
};

cboard_2::~cboard_2(void)
{
    mi2c_end(&mi2c);
    rtc_end(&rtc);
}

      
void cboard_2::Draw(CDraw *draw,double scale)
{
  int i;
  
  draw->Canvas.Init(scale,scale);
  

  lcd_blink(&lcd);

//lab2 draw 
  for(i=0;i<outputc;i++)
  {
    if(!output[i].r)
    {
       
      draw->Canvas.SetFgColor (30, 0, 0);
      
      switch(output[i].id) 
      {
        case O_LCD: draw->Canvas.SetColor (0, 90*Window1.Get_picpwr()+40, 0);break;

      }


      if(output[i].id != O_LCD)
        draw->Canvas.Rectangle (1, output[i].x1, output[i].y1, output[i].x2-output[i].x1,output[i].y2-output[i].y1 );

//draw lcd text 
   
      if((output[i].id == O_LCD)&&(lcd.update))
      {
        lcd_draw(&lcd,&draw->Canvas,output[i].x1, output[i].y1, output[i].x2-output[i].x1,output[i].y2-output[i].y1,Window1.Get_picpwr());   
      }
       
    }
    else
    {
      draw->Canvas.SetFgColor (0, 0, 0);
      
 
      switch(output[i].id)
      {
        case O_RA1: draw->Canvas.SetColor (pic.pins[17].oavalue, 0, 0);break;
        case O_RA2: draw->Canvas.SetColor (pic.pins[0].oavalue, 0, 0);break;
        case O_RA6: draw->Canvas.SetColor (pic.pins[14].oavalue, 0, 0);break;
        case O_RA7: draw->Canvas.SetColor (pic.pins[15].oavalue, 0, 0);break;
      }

      if(output[i].id == O_LPWR)draw->Canvas.SetColor (0,255*Window1.Get_picpwr(), 0);
      
      draw->Canvas.Circle (1,output[i].x1, output[i].y1,output[i].r );
    };
   };    

   rtc_update(&rtc);

//end draw

  draw->Canvas.End();
  draw->Update ();
  
 
};

void cboard_2::Run_CPU(void)
{
  int i;
  int j;
  unsigned char pi;
  const picpin * pins;
  unsigned char L[4];
  unsigned int alm[18]; //luminosidade media
  

  int JUMPSTEPS = Window1.GetJUMPSTEPS();
  long int NSTEPJ=Window1.GetNSTEPJ();  
    
pins = pic.pins;
  
/*
for(pi=0;pi < pic.PINCOUNT;pi++)
     {
       alm[pi]=0;
     };
*/
     memset(alm,0,18*sizeof(unsigned int));

 j=JUMPSTEPS;
 if(Window1.Get_picpwr())
   for(i=0;i<Window1.GetNSTEP();i++)
      {
          //if(j > JUMPSTEPS)
          //{  
 
          L[0]=pic_get_pin(18);
          L[1]=pic_get_pin(1);
          L[2]=pic_get_pin(15);
          L[3]=pic_get_pin(16);

          if(((L[0])&&(p_CL1 == 1))||((L[1])&&(p_CL1 == 2))||
           ((L[2])&&(p_CL1 == 3))||((L[3])&&(p_CL1 == 4)))
          {
             pic_set_pin(13,1); 
          }
          else
          {
             pic_set_pin(13,0);
          }

          if(((L[0])&&(p_CL2 == 1))||((L[1])&&(p_CL2 == 2))||
           ((L[2])&&(p_CL2 == 3))||((L[3])&&(p_CL2 == 4)))
          {
             pic_set_pin(12,1); 
          }
          else
          {
             pic_set_pin(12,0);
          }

          if(((L[0])&&(p_CL3 == 1))||((L[1])&&(p_CL3 == 2))||
           ((L[2])&&(p_CL3 == 3))||((L[3])&&(p_CL3 == 4)))
          {
             pic_set_pin(11,1); 
          }
          else
          {
             pic_set_pin(11,0);
          }
          //} 
  
        if(!mplabxd_testbp())pic_step();
        if(use_oscope)Window4.SetSample();
        if(use_spare)Window5.Process();  
        
        if( j < pic.PINCOUNT)  
          alm[j]+=pins[j].value;
        if(j >= JUMPSTEPS)
          {
        /*    
        for(pi=0;pi < pic.PINCOUNT;pi++)
        {
           alm[pi]+=pins[pi].value;
           //if((!pins[pi].dir)&&(pins[pi].value)) lm[pi]++;
        }
        */
          j=-1;
          } 
          j++;

//serial lcd display code
    if(( pins[9].value)&&(!clko))
    {
      d= (d<<1)|pins[8].value;
    };

    clko=pins[9].value;
 
    if((!pins[16].dir)&&(!pins[16].value))
    {
      if(!lcde)
      {

        if((!pins[8].dir)&&(!pins[8].value))
        {
          lcd_cmd(&lcd,d);
        }
        else if((!pins[8].dir)&&(pins[8].value))
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
        if(pins[2].dir)
        {
          sda=1;
        }
        else
        {
          sda=pins[2].value;
        }
        
        if(pins[1].dir)
        {
          sck=1;
	  pic_set_pin(2,1);
        }
        else
        {
          sck=pins[1].value;
        }
	pic_set_pin(3, mi2c_io(&mi2c,sck,sda) | rtc_io(&rtc,sck,sda));
  };
   //fim STEP
   

     for(pi=0;pi < pic.PINCOUNT;pi++)
     { 
      pic.pins[pi].oavalue= (int)(((225.0*alm[pi])/NSTEPJ)+30);
     }
       
}


void 
cboard_2::Reset(void)
{  

  pic_reset(1);
                
  lcd_rst(&lcd);  
  mi2c_rst(&mi2c);
  rtc_rst(&rtc);
  pic_set_pin_DOV(18,0);
  pic_set_pin_DOV(1,0);
  pic_set_pin_DOV(15,0);
  pic_set_pin_DOV(16,0);
  
    
  p_CL1=0; 
  p_CL2=0; 
  p_CL3=0; 
  pic_set_pin(13,p_CL1); 
  pic_set_pin(12,p_CL2); 
  pic_set_pin(11,p_CL3); 
  
#ifndef _WIN_
    if(pic.serialfd > 0)
#else
    if(pic.serialfd != INVALID_HANDLE_VALUE)
#endif
      Window1.statusbar1.SetField(2,lxT("Serial Port: ")+String::FromAscii(SERIALDEVICE)+lxT(":")+itoa(pic.serialbaud)+lxT("(")+String().Format("%4.1f",fabs((100.0*pic.serialexbaud-100.0*pic.serialbaud)/pic.serialexbaud))+lxT("%)"));
    else  
      Window1.statusbar1.SetField(2,lxT("Serial Port: ")+String::FromAscii(SERIALDEVICE)+lxT(" (ERROR)"));

  for(int pi=0;pi < pic.PINCOUNT;pi++)
     {
       pic.pins[pi].oavalue=0;
     };
    
};


void 
cboard_2::MouseButtonPress( uint button, uint x, uint y,uint state)
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
      
      case I_PWR:
      {
        if(Window1.Get_picpwr()) 
        { 
          Window1.Set_picrun(0); 
          Window1.Set_picpwr(0); 
          Reset();
          
          Window1.statusbar1.SetField(0,lxT("Stoped"));
        }
        else
        {
          Window1.Set_picpwr(1);
          Window1.Set_picrun(1);
          Reset();
     
          
          Window1.statusbar1.SetField(0,lxT("Running..."));
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
      

      case I_TC1:
        {
          p_CL1=1;
        };break;
      case I_TC2:
        {
          p_CL2=1; 
        };break;
      case I_TC3:
        {
          p_CL3=1; 
        };break;

      case I_TC4:
        {
          p_CL1=2; 
        };break;
      case I_TC5:
        {
          p_CL2=2; 
        };break;
      case I_TC6:
        {
          p_CL3=2; 
        };break;

      case I_TC7:
        {
          p_CL1=3; 
        };break;
      case I_TC8:
        {
          p_CL2=3; 
        };break;
      case I_TC9:
        {
          p_CL3=3; 
        };break;
      
      case I_TCA:
        {
          p_CL1=4; 
        };break;
      case I_TC0:
        {
          p_CL2=4; 
        };break;
      case I_TCT:
        {
          p_CL3=4; 
        };break;

      }
    }
  }
    
    
};

void 
cboard_2::MouseButtonRelease( uint button, uint x, uint y,uint state)
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

            if(pic_reset(-1))
            { 
              Reset();
            }
          } 
      };break;
      

        case I_TC1:
        case I_TC4:
        case I_TC7:
        case I_TCA:
        {
          p_CL1=0; 
        };break;

        case I_TC2:
        case I_TC5:
        case I_TC8:
        case I_TC0:
        {
          p_CL2=0; 
        };break;

        case I_TC3:
        case I_TC6:
        case I_TC9:
        case I_TCT:
        {
          p_CL3=0; 
        };break;
      }
    } 
  }    
};


void 
cboard_2::KeyPress( uint key, uint x, uint y,uint mask)
{
  if(key == '1')
  {
    p_CL1=1; 
  }
  if(key == '2')
  {
    p_CL2=1; 
  }
  if(key == '3')
  {
    p_CL3=1; 
  }

  if(key == '4')
  {
    p_CL1=2; 
  }
  if(key == '5')
  {
    p_CL2=2; 
  }
  if(key == '6')
  {
    p_CL3=2; 
  }

  if(key == '7')
  {
    p_CL1=3; 
  }
  if(key == '8')
  {
    p_CL2=3; 
  }
  if(key == '9')
  {
    p_CL3=3; 
  }

  if(key == '*')
  {
    p_CL1=4; 
  }
  if(key == '0')
  {
    p_CL2=4; 
  }
  if(key == '#')
  {
    p_CL3=4; 
  }

};

void
cboard_2::KeyRelease(uint key, uint x, uint y,uint mask)
{
  if((key == '1')||(key == '4')||(key == '7')||(key == '*'))
  {
    p_CL1=0; 
  }
  if((key == '2')||(key == '5')||(key == '8')||(key == '0'))
  {
    p_CL2=0; 
  }
  if((key == '3')||(key == '6')||(key == '9')||(key == '#'))
  {
    p_CL3=0; 
  }
};

void
cboard_2::OnShow(void)
{
    lcd.update=1;
}

unsigned short 
cboard_2::get_in_id(char * name)
{
  if(strcmp(name,"RST")==0)return I_RST; 
  if(strcmp(name,"PWR")==0)return I_PWR;
  if(strcmp(name,"ICSP")==0)return I_ICSP;
  if(strcmp(name,"TC1")==0)return I_TC1;
  if(strcmp(name,"TC2")==0)return I_TC2;
  if(strcmp(name,"TC3")==0)return I_TC3;
  if(strcmp(name,"TC4")==0)return I_TC4;
  if(strcmp(name,"TC5")==0)return I_TC5;
  if(strcmp(name,"TC6")==0)return I_TC6;
  if(strcmp(name,"TC7")==0)return I_TC7;
  if(strcmp(name,"TC8")==0)return I_TC8;
  if(strcmp(name,"TC9")==0)return I_TC9;
  if(strcmp(name,"TC*")==0)return I_TCA;
  if(strcmp(name,"TC0")==0)return I_TC0;
  if(strcmp(name,"TC#")==0)return I_TCT;
  

  printf("Erro input '%s' don't have a valid id! \n",name);
  return -1;
};

unsigned short 
cboard_2::get_out_id(char * name)
{
  	
  if(strcmp(name,"RA1")==0)return O_RA1;	
  if(strcmp(name,"RA2")==0)return O_RA2;	  
  if(strcmp(name,"RA6")==0)return O_RA6;	
  if(strcmp(name,"RA7")==0)return O_RA7;	
  if(strcmp(name,"LPWR")==0)return O_LPWR;	
  if(strcmp(name,"LCD")==0)return O_LCD;	


  printf("Erro output '%s' don't have a valid id! \n",name);
  return 1;
};


void 
cboard_2::WritePreferences(void)
{
    Window1.saveprefs(lxT("p2_proc"),proc);
};

void 
cboard_2::ReadPreferences(char *name,char *value)
{
    
      if(!strcmp(name,"p2_proc"))
      {
         proc=value; 
      }
};

void 
cboard_2::RefreshStatus(void)
{
    
#ifndef _WIN_
    if(pic.serialfd > 0)
#else
    if(pic.serialfd != INVALID_HANDLE_VALUE)
#endif
      Window1.statusbar1.SetField(2,lxT("Serial Port: ")+String::FromAscii(SERIALDEVICE)+lxT(":")+itoa(pic.serialbaud)+lxT("(")+String().Format("%4.1f",fabs((100.0*pic.serialexbaud-100.0*pic.serialbaud)/pic.serialexbaud))+lxT("%)"));
    else  
      Window1.statusbar1.SetField(2,lxT("Serial Port: ")+String::FromAscii(SERIALDEVICE)+lxT(" (ERROR)"));

};
