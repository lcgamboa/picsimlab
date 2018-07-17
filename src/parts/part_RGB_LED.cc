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
#include"part_RGB_LED.h"

/* outputs */
enum {O_P1, O_P2, O_P3, O_L1} ;

cpart_rgb_led::cpart_rgb_led(unsigned x, unsigned y)
{
   X=x;
   Y=y;        
   ReadMaps();   
     
   lxImage image;
   image.LoadFile(Window1.GetSharePath()+lxT("parts/")+GetPictureFileName());

   Bitmap = new lxBitmap(image);
   image.Destroy();
   canvas.Create(Bitmap);
   

   
   input_pins[0]=0;
   input_pins[1]=0;
   input_pins[2]=0;


};

cpart_rgb_led::~cpart_rgb_led(void)
{
    delete Bitmap;
}

 


void cpart_rgb_led::Draw(void)
{ 
 
  int i;
  board *pboard=Window1.GetBoard();
  
  const picpin * ppins = pboard->MGetPinsValues();
   
  canvas.Init();
  
  lxFont font(9, lxFONTFAMILY_TELETYPE, lxFONTSTYLE_NORMAL, lxFONTWEIGHT_BOLD );
  canvas.SetFont (font);
 
  for(i=0;i<outputc;i++)
  {

      switch(output[i].id)
        {
         case O_P1:   
         case O_P2:
         case O_P3:  
           canvas.SetColor (49, 61, 99);
           canvas.Rectangle (1,output[i].x1,output[i].y1,output[i].x2-output[i].x1,output[i].y2-output[i].y1);
           canvas.SetFgColor (255, 255, 255);
           if(input_pins[output[i].id - O_P1] == 0)
             canvas.Text ("NC",output[i].x1,output[i].y1);
           else
             canvas.Text (pboard->MGetPinName(input_pins[output[i].id - O_P1]),output[i].x1,output[i].y1);         
           break;
         case O_L1:
           canvas.SetColor (ppins[input_pins[0]-1].oavalue, ppins[input_pins[1]-1].oavalue, ppins[input_pins[2]-1].oavalue);
           canvas.Circle (1,output[i].x1,output[i].y1,output[i].r);
           break;
        }

      
  };
  

  canvas.End();
  
}

unsigned short 
cpart_rgb_led::get_in_id(char * name)
{  
  printf("Erro input '%s' don't have a valid id! \n",name);
  return -1;
};

unsigned short 
cpart_rgb_led::get_out_id(char * name)
{

  if(strcmp(name,"P1")==0)return O_P1;
  if(strcmp(name,"P2")==0)return O_P2;
  if(strcmp(name,"P3")==0)return O_P3;
  
  if(strcmp(name,"L1")==0)return O_L1;

  printf("Erro output '%s' don't have a valid id! \n",name);
  return 1;
};


String
cpart_rgb_led::WritePreferences(void)
{
  char prefs[256];
  
  sprintf(prefs,"%hhu,%hhu,%hhu",input_pins[0],input_pins[1],input_pins[2]);
  
  return prefs;
};

void 
cpart_rgb_led::ReadPreferences(String value)
{
   sscanf(value.c_str (),"%hhu,%hhu,%hhu",&input_pins[0],&input_pins[1],&input_pins[2]);      
};

CPWindow * WProp_rgb_led;
      
void 
cpart_rgb_led::ConfigurePropertiesWindow(CPWindow *  wprop)
{
    String Items="0  NC,";
    String spin;
    WProp_rgb_led=wprop;
    board *pboard=Window1.GetBoard();
    
    for(int i=1; i<= pboard->MGetPinCount();i++ )
    {
       spin= pboard->MGetPinName(i);
      
      if(spin.Cmp(lxT("error")))
      {
        Items=Items+itoa(i)+"  "+spin+",";
      }
    }
    
    ((CCombo*)WProp_rgb_led->GetChildByName("combo1"))->SetItems(Items);
    if(input_pins[0] == 0)
        ((CCombo*)WProp_rgb_led->GetChildByName("combo1"))->SetText("0  NC");
    else
    {
        spin= pboard->MGetPinName(input_pins[0]);
        ((CCombo*)WProp_rgb_led->GetChildByName("combo1"))->SetText(itoa(input_pins[0])+"  "+spin);
    }
    
    ((CCombo*)WProp_rgb_led->GetChildByName("combo2"))->SetItems(Items);
    if(input_pins[1] == 0)
        ((CCombo*)WProp_rgb_led->GetChildByName("combo2"))->SetText("0  NC");
    else
    {
        spin= pboard->MGetPinName(input_pins[1]);
        ((CCombo*)WProp_rgb_led->GetChildByName("combo2"))->SetText(itoa(input_pins[1])+"  "+spin);
    }
    
    ((CCombo*)WProp_rgb_led->GetChildByName("combo3"))->SetItems(Items);
    if(input_pins[3] == 0)
        ((CCombo*)WProp_rgb_led->GetChildByName("combo3"))->SetText("0  NC");
    else
    {
        spin= pboard->MGetPinName(input_pins[2]);
        ((CCombo*)WProp_rgb_led->GetChildByName("combo3"))->SetText(itoa(input_pins[2])+"  "+spin);
    }
    
    
    ((CButton*)WProp_rgb_led->GetChildByName("button1"))->EvMouseButtonRelease = EVMOUSEBUTTONRELEASE & cpart_rgb_led::PropButton;
    ((CButton*)WProp_rgb_led->GetChildByName("button1"))->SetTag(1);
    
    ((CButton*)WProp_rgb_led->GetChildByName("button2"))->EvMouseButtonRelease = EVMOUSEBUTTONRELEASE & cpart_rgb_led::PropButton;
}

void 
cpart_rgb_led::ReadPropertiesWindow(void)
{
   if(WProp_rgb_led->GetTag())
   {
      input_pins[0]=atoi(((CCombo*)WProp_rgb_led->GetChildByName("combo1"))->GetText());
      input_pins[1]=atoi(((CCombo*)WProp_rgb_led->GetChildByName("combo2"))->GetText());
      input_pins[2]=atoi(((CCombo*)WProp_rgb_led->GetChildByName("combo3"))->GetText());
   }
} 


void
cpart_rgb_led::PropButton (CControl * control, uint button, uint x, uint y, uint state)
{
   WProp_rgb_led->SetTag(control->GetTag()); 
   WProp_rgb_led->HideExclusive ();
};
