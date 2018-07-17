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
#include"part_LCD_pcf8833.h"

/* outputs */
enum {O_P1, O_P2, O_P3, O_P4, O_F1, O_F2, O_F3, O_F4, O_F5, O_LCD} ;



cpart_LCD_pcf8833::cpart_LCD_pcf8833(unsigned x, unsigned y)
{
   X=x;
   Y=y;        
   ReadMaps();   
   Bitmap=NULL;

   lxImage image;

   image.LoadFile(Window1.GetSharePath()+lxT("parts/")+GetPictureFileName());

  
   Bitmap = new lxBitmap(image);
   image.Destroy(); 
   canvas.Create(Bitmap);

   lcd_pcf8833_init(&lcd);
   lcd_pcf8833_rst(&lcd);

   input_pins[0]=0;
   input_pins[1]=0;
   input_pins[2]=0;
   input_pins[3]=0;      
};

cpart_LCD_pcf8833::~cpart_LCD_pcf8833(void)
{
    delete Bitmap;
}


void cpart_LCD_pcf8833::Draw(void)
{ 
 
  int i;
  board *pboard=Window1.GetBoard();
  
  canvas.Init();
   
  lxFont font(8, lxFONTFAMILY_TELETYPE, lxFONTSTYLE_NORMAL, lxFONTWEIGHT_BOLD );
  canvas.SetFont (font);
  
  for(i=0;i<outputc;i++)
  {

      switch(output[i].id)
        {
         case O_P1:
         case O_P2:
         case O_P3:
         case O_P4:   
           canvas.SetColor (49, 61, 99);
           canvas.Rectangle (1,output[i].x1,output[i].y1,output[i].x2-output[i].x1,output[i].y2-output[i].y1);
           canvas.SetFgColor (255, 255, 255);
           if(input_pins[output[i].id - O_P1] == 0)
             canvas.RotatedText ("NC",output[i].x1,output[i].y2, 90.0);
           else
             canvas.RotatedText(pboard->MGetPinName(input_pins[output[i].id - O_P1]),output[i].x1,output[i].y2, 90.0);  
           break;
	 case O_F1:
           canvas.SetColor (49, 61, 99);
           canvas.Rectangle (1,output[i].x1,output[i].y1,output[i].x2-output[i].x1,output[i].y2-output[i].y1);
           canvas.SetFgColor (155, 155, 155);
           canvas.RotatedText ("3.3V",output[i].x1,output[i].y2, 90.0);
	   break;  
	 case O_F2:
           canvas.SetColor (49, 61, 99);
           canvas.Rectangle (1,output[i].x1,output[i].y1,output[i].x2-output[i].x1,output[i].y2-output[i].y1);
           canvas.SetFgColor (155, 155, 155);
           canvas.RotatedText ("3.3V",output[i].x1,output[i].y2, 90.0);
	   break;  
	 case O_F3:
           canvas.SetColor (49, 61, 99);
           canvas.Rectangle (1,output[i].x1,output[i].y1,output[i].x2-output[i].x1,output[i].y2-output[i].y1);
           canvas.SetFgColor (155, 155, 155);
           canvas.RotatedText ("GND",output[i].x1,output[i].y2, 90.0);
	   break;  
	 case O_F4:
           canvas.SetColor (49, 61, 99);
           canvas.Rectangle (1,output[i].x1,output[i].y1,output[i].x2-output[i].x1,output[i].y2-output[i].y1);
           canvas.SetFgColor (155, 155, 155);
           canvas.RotatedText ("GND",output[i].x1,output[i].y2, 90.0);
	   break; 
	 case O_F5:
           canvas.SetColor (49, 61, 99);
           canvas.Rectangle (1,output[i].x1,output[i].y1,output[i].x2-output[i].x1,output[i].y2-output[i].y1);
           canvas.SetFgColor (155, 155, 155);
           canvas.RotatedText ("6V",output[i].x1,output[i].y2, 90.0);
	   break;           
         case O_LCD:
         //draw lcd text 
           if(lcd.update)
           {  
             canvas.SetColor (0, 90+40, 0);    
             lcd_pcf8833_draw(&lcd,&canvas,output[i].x1, output[i].y1, output[i].x2-output[i].x1,output[i].y2-output[i].y1, 1);          
           } 
           /*
           else   
           {
              canvas.Rectangle (1, output[i].x1, output[i].y1, output[i].x2-output[i].x1,output[i].y2-output[i].y1 );
           }
           */
           break;
        }

      
  };
  
  
  canvas.End();
  
}


unsigned short 
cpart_LCD_pcf8833::get_in_id(char * name)
{  
  printf("Erro input '%s' don't have a valid id! \n",name);
  return -1;
};

unsigned short 
cpart_LCD_pcf8833::get_out_id(char * name)
{

  if(strcmp(name,"P1")==0)return O_P1;
  if(strcmp(name,"P2")==0)return O_P2;
  if(strcmp(name,"P3")==0)return O_P3;
  if(strcmp(name,"P4")==0)return O_P4;
    
  if(strcmp(name,"F1")==0)return O_F1;
  if(strcmp(name,"F2")==0)return O_F2;
  if(strcmp(name,"F3")==0)return O_F3;
  if(strcmp(name,"F4")==0)return O_F4;
  if(strcmp(name,"F5")==0)return O_F5;
  
  if(strcmp(name,"LCD")==0)return O_LCD;

  printf("Erro output '%s' don't have a valid id! \n",name);
  return 1;
};


String
cpart_LCD_pcf8833::WritePreferences(void)
{
  char prefs[256];
  
  sprintf(prefs,"%hhu,%hhu,%hhu,%hhu",input_pins[0],input_pins[1],input_pins[2],input_pins[3]);
  
  return prefs;
}

void 
cpart_LCD_pcf8833::ReadPreferences(String value)
{
   sscanf(value.c_str (),"%hhu,%hhu,%hhu,%hhu",&input_pins[0],&input_pins[1],&input_pins[2],&input_pins[3]);      
   Reset();      
}

CPWindow * WProp_LCD_pcf8833;
      
void 
cpart_LCD_pcf8833::ConfigurePropertiesWindow(CPWindow *  wprop)
{
    String Items="0  NC,";
    String spin;
    WProp_LCD_pcf8833=wprop;
    board *pboard=Window1.GetBoard();
    
    for(int i=1; i<= pboard->MGetPinCount();i++ )
    {
       spin= pboard->MGetPinName(i);
      
      if(spin.Cmp(lxT("error")))
      {
        Items=Items+itoa(i)+"  "+spin+",";
      }
    }
    
    ((CCombo*)WProp_LCD_pcf8833->GetChildByName("combo1"))->SetItems(Items);
    if(input_pins[0] == 0)
        ((CCombo*)WProp_LCD_pcf8833->GetChildByName("combo1"))->SetText("0  NC");
    else
    {
        spin= pboard->MGetPinName(input_pins[0]);
        ((CCombo*)WProp_LCD_pcf8833->GetChildByName("combo1"))->SetText(itoa(input_pins[0])+"  "+spin);
    }
    
    ((CCombo*)WProp_LCD_pcf8833->GetChildByName("combo2"))->SetItems(Items);
    if(input_pins[1] == 0)
        ((CCombo*)WProp_LCD_pcf8833->GetChildByName("combo2"))->SetText("0  NC");
    else
    {
        spin= pboard->MGetPinName(input_pins[1]);
        ((CCombo*)WProp_LCD_pcf8833->GetChildByName("combo2"))->SetText(itoa(input_pins[1])+"  "+spin);
    }
    
    ((CCombo*)WProp_LCD_pcf8833->GetChildByName("combo3"))->SetItems(Items);
    if(input_pins[2] == 0)
        ((CCombo*)WProp_LCD_pcf8833->GetChildByName("combo3"))->SetText("0  NC");
    else
    {
        spin= pboard->MGetPinName(input_pins[2]);
        ((CCombo*)WProp_LCD_pcf8833->GetChildByName("combo3"))->SetText(itoa(input_pins[2])+"  "+spin);
    }
    
    ((CCombo*)WProp_LCD_pcf8833->GetChildByName("combo4"))->SetItems(Items);
    if(input_pins[3] == 0)
        ((CCombo*)WProp_LCD_pcf8833->GetChildByName("combo4"))->SetText("0  NC");
    else
    {
        spin= pboard->MGetPinName(input_pins[3]);
        ((CCombo*)WProp_LCD_pcf8833->GetChildByName("combo4"))->SetText(itoa(input_pins[3])+"  "+spin);
    }
   
    ((CButton*)WProp_LCD_pcf8833->GetChildByName("button1"))->EvMouseButtonRelease = EVMOUSEBUTTONRELEASE & cpart_LCD_pcf8833::PropButton;
    ((CButton*)WProp_LCD_pcf8833->GetChildByName("button1"))->SetTag(1);
    
    ((CButton*)WProp_LCD_pcf8833->GetChildByName("button2"))->EvMouseButtonRelease = EVMOUSEBUTTONRELEASE & cpart_LCD_pcf8833::PropButton;
}

void 
cpart_LCD_pcf8833::ReadPropertiesWindow(void)
{
   if(WProp_LCD_pcf8833->GetTag())
   {	   
      input_pins[0]=atoi(((CCombo*)WProp_LCD_pcf8833->GetChildByName("combo1"))->GetText());
      input_pins[1]=atoi(((CCombo*)WProp_LCD_pcf8833->GetChildByName("combo2"))->GetText());
      input_pins[2]=atoi(((CCombo*)WProp_LCD_pcf8833->GetChildByName("combo3"))->GetText());
      input_pins[3]=atoi(((CCombo*)WProp_LCD_pcf8833->GetChildByName("combo4"))->GetText());
   }
} 


void
cpart_LCD_pcf8833::PropButton (CControl * control, uint button, uint x, uint y, uint state)
{
   WProp_LCD_pcf8833->SetTag(control->GetTag()); 
   WProp_LCD_pcf8833->HideExclusive ();
};

void
cpart_LCD_pcf8833::Process(void)
{
   const picpin * ppins=Window1.GetBoard()->MGetPinsValues();

   

    //if((!ppins[input_pins[3]-1].dir)&&(!ppins[input_pins[3]-1].value))
    {
      lcd_pcf8833_io(&lcd, ppins[input_pins[1]-1].value, ppins[input_pins[2]-1].value, ppins[input_pins[3]-1].value, ppins[input_pins[0]-1].value);
    }
   
}
