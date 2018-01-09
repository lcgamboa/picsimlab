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

#include"../picsimlab1.h"
#include"../picsimlab4.h"
#include"part_LEDs.h"

/* outputs */
enum {O_P1, O_P2, O_P3, O_P4, O_P5, O_P6, O_P7, O_P8, O_L1, O_L2, O_L3, O_L4, O_L5, O_L6, O_L7, O_L8} ;

cpart_leds::cpart_leds(unsigned x, unsigned y)
{
   X=x;
   Y=y;        
   ReadMaps();   
     
   wxImage image;
   image.LoadFile(Window1.GetSharePath()+GetPictureFileName());

   Bitmap = new wxBitmap(image);
   
   canvas.Create(Bitmap);
   

   
   input_pins[0]=0;
   input_pins[1]=0;
   input_pins[2]=0;
   input_pins[3]=0;      
   input_pins[4]=0;
   input_pins[5]=0;
   input_pins[6]=0;
   input_pins[7]=0;

};

cpart_leds::~cpart_leds(void)
{
    delete Bitmap;
}

 


void cpart_leds::Draw(void)
{ 
 
  int i;
  board *pboard=Window1.GetBoard();
  
  const picpin * ppins = pboard->MGetPinsValues();
   
  canvas.Init();
  
  wxFont font(12, wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL );
 
  for(i=0;i<outputc;i++)
  {

      switch(output[i].id)
        {
         case O_P1:
         case O_P2:
         case O_P3:
         case O_P4:    
         case O_P5:
         case O_P6:
         case O_P7:
         case O_P8:    
           canvas.SetFont (font);
           canvas.SetColor (49, 61, 99);
           canvas.Rectangle (1,output[i].x1,output[i].y1,output[i].x2-output[i].x1,output[i].y2-output[i].y1);
           canvas.SetFgColor (255, 255, 255);
           if(output[i].id == O_P1)
           {
             if(input_pins[0] == 0)
               canvas.Text ("NC",output[i].x1,output[i].y1);
             else
               canvas.Text (pboard->MGetPinName(input_pins[0]),output[i].x1,output[i].y1);  
           }
           if(output[i].id == O_P2)
           {
             if(input_pins[1] == 0)
               canvas.Text ("NC",output[i].x1,output[i].y1);
             else  
               canvas.Text (pboard->MGetPinName(input_pins[1]),output[i].x1,output[i].y1);
           }
           if(output[i].id == O_P3)
           {
             if(input_pins[2] == 0)
               canvas.Text ("NC",output[i].x1,output[i].y1);
             else  
               canvas.Text (pboard->MGetPinName(input_pins[2]),output[i].x1,output[i].y1);
           }
           if(output[i].id == O_P4)
           {
             if(input_pins[3] == 0)
               canvas.Text ("NC",output[i].x1,output[i].y1);
             else  
               canvas.Text (pboard->MGetPinName(input_pins[3]),output[i].x1,output[i].y1);
           }
           if(output[i].id == O_P5)
           {
             if(input_pins[4] == 0)
               canvas.Text ("NC",output[i].x1,output[i].y1);
             else
               canvas.Text (pboard->MGetPinName(input_pins[4]),output[i].x1,output[i].y1);  
           }
           if(output[i].id == O_P6)
           {
             if(input_pins[5] == 0)
               canvas.Text ("NC",output[i].x1,output[i].y1);
             else  
               canvas.Text (pboard->MGetPinName(input_pins[5]),output[i].x1,output[i].y1);
           }
           if(output[i].id == O_P7)
           {
             if(input_pins[6] == 0)
               canvas.Text ("NC",output[i].x1,output[i].y1);
             else  
               canvas.Text (pboard->MGetPinName(input_pins[6]),output[i].x1,output[i].y1);
           }
           if(output[i].id == O_P8)
           {
             if(input_pins[7] == 0)
               canvas.Text ("NC",output[i].x1,output[i].y1);
             else  
               canvas.Text (pboard->MGetPinName(input_pins[7]),output[i].x1,output[i].y1);
           }
           break;
         case O_L1:
         case O_L2:
         case O_L3:
         case O_L4: 
         case O_L5:
         case O_L6:
         case O_L7:
         case O_L8: 
           if(output[i].id == O_L1)
             canvas.SetColor (ppins[input_pins[0]-1].oavalue, 0, 0);
           if(output[i].id == O_L2)
             canvas.SetColor (ppins[input_pins[1]-1].oavalue, 0, 0);
           if(output[i].id == O_L3)
             canvas.SetColor (ppins[input_pins[2]-1].oavalue, 0, 0);
           if(output[i].id == O_L4)
             canvas.SetColor (ppins[input_pins[3]-1].oavalue, 0, 0);
           if(output[i].id == O_L5)
             canvas.SetColor (ppins[input_pins[4]-1].oavalue, 0, 0);
           if(output[i].id == O_L6)
             canvas.SetColor (ppins[input_pins[5]-1].oavalue, 0, 0);
           if(output[i].id == O_L7)
             canvas.SetColor (ppins[input_pins[6]-1].oavalue, 0, 0);
           if(output[i].id == O_L8)
             canvas.SetColor (ppins[input_pins[7]-1].oavalue, 0, 0);
           canvas.Circle (1,output[i].x1,output[i].y1,output[i].r);
           break;
        }

      
  };
  
  /*
  canvas.SetColor (0,0,0);
  canvas.Rectangle (1,0,0,100,20);
  canvas.SetFgColor (255,255,255);
  
  wxFont fonts(8, wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL );

  canvas.SetFont (fonts);
  canvas.Text (ftoa(180.0*angle/M_PI),1,1); 
  */
   
  canvas.End();
  
}

 
void cpart_leds::Process(void)
{

//   b1[0]=pic->pins[input_pin[0]-1].value-pic->pins[input_pin[2]-1].value;
 
     
}
 

void 
cpart_leds::Reset(void)
{          

};




void 
cpart_leds::MouseButtonPress(uint button, uint x, uint y,uint state)
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
cpart_leds::MouseButtonRelease(uint button, uint x, uint y,uint state)
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
cpart_leds::KeyPress( uint key, uint x, uint y,uint mask)
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
cpart_leds::KeyRelease(uint key, uint x, uint y,uint mask)
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
cpart_leds::get_in_id(char * name)
{  
  printf("Erro input '%s' don't have a valid id! \n",name);
  return -1;
};

unsigned short 
cpart_leds::get_out_id(char * name)
{

  if(strcmp(name,"P1")==0)return O_P1;
  if(strcmp(name,"P2")==0)return O_P2;
  if(strcmp(name,"P3")==0)return O_P3;
  if(strcmp(name,"P4")==0)return O_P4;
  if(strcmp(name,"P5")==0)return O_P5;
  if(strcmp(name,"P6")==0)return O_P6;
  if(strcmp(name,"P7")==0)return O_P7;
  if(strcmp(name,"P8")==0)return O_P8;

  if(strcmp(name,"L1")==0)return O_L1;
  if(strcmp(name,"L2")==0)return O_L2;
  if(strcmp(name,"L3")==0)return O_L3;
  if(strcmp(name,"L4")==0)return O_L4;
  if(strcmp(name,"L5")==0)return O_L5;
  if(strcmp(name,"L6")==0)return O_L6;
  if(strcmp(name,"L7")==0)return O_L7;
  if(strcmp(name,"L8")==0)return O_L8;

  printf("Erro output '%s' don't have a valid id! \n",name);
  return 1;
};


String
cpart_leds::WritePreferences(void)
{
  char prefs[256];
  
  sprintf(prefs,"%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu",input_pins[0],input_pins[1],input_pins[2],input_pins[3],input_pins[4],input_pins[5],input_pins[6],input_pins[7]);
  
  return prefs;
};

void 
cpart_leds::ReadPreferences(String value)
{
   sscanf(value.c_str (),"%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu",&input_pins[0],&input_pins[1],&input_pins[2],&input_pins[3],&input_pins[4],&input_pins[5],&input_pins[6],&input_pins[7]);      
};

CPWindow * WProp_leds;
      
void 
cpart_leds::ConfigurePropiertsWindow(CPWindow *  wprop)
{
    String Items="0  NC,";
    String spin;
    WProp_leds=wprop;
    board *pboard=Window1.GetBoard();
    
    for(int i=1; i<= pboard->MGetPinCount();i++ )
    {
       spin= pboard->MGetPinName(i);
      
      if(spin.Cmp(wxT("error")))
      {
        Items=Items+itoa(i)+"  "+spin+",";
      }
    }
    
    ((CCombo*)WProp_leds->GetChildByName("combo1"))->SetItems(Items);
    if(input_pins[0] == 0)
        ((CCombo*)WProp_leds->GetChildByName("combo1"))->SetText("0  NC");
    else
    {
        spin= pboard->MGetPinName(input_pins[0]);
        ((CCombo*)WProp_leds->GetChildByName("combo1"))->SetText(itoa(input_pins[0])+"  "+spin);
    }
    
    ((CCombo*)WProp_leds->GetChildByName("combo2"))->SetItems(Items);
    if(input_pins[1] == 0)
        ((CCombo*)WProp_leds->GetChildByName("combo2"))->SetText("0  NC");
    else
    {
        spin= pboard->MGetPinName(input_pins[1]);
        ((CCombo*)WProp_leds->GetChildByName("combo2"))->SetText(itoa(input_pins[1])+"  "+spin);
    }
    
    ((CCombo*)WProp_leds->GetChildByName("combo3"))->SetItems(Items);
    if(input_pins[3] == 0)
        ((CCombo*)WProp_leds->GetChildByName("combo3"))->SetText("0  NC");
    else
    {
        spin= pboard->MGetPinName(input_pins[2]);
        ((CCombo*)WProp_leds->GetChildByName("combo3"))->SetText(itoa(input_pins[2])+"  "+spin);
    }
    
    ((CCombo*)WProp_leds->GetChildByName("combo4"))->SetItems(Items);
    if(input_pins[3] == 0)
        ((CCombo*)WProp_leds->GetChildByName("combo4"))->SetText("0  NC");
    else
    {
        spin= pboard->MGetPinName(input_pins[3]);
        ((CCombo*)WProp_leds->GetChildByName("combo4"))->SetText(itoa(input_pins[3])+"  "+spin);
    }
    
    ((CCombo*)WProp_leds->GetChildByName("combo5"))->SetItems(Items);
    if(input_pins[4] == 0)
        ((CCombo*)WProp_leds->GetChildByName("combo5"))->SetText("0  NC");
    else
    {
        spin= pboard->MGetPinName(input_pins[4]);
        ((CCombo*)WProp_leds->GetChildByName("combo5"))->SetText(itoa(input_pins[4])+"  "+spin);
    }
    
    ((CCombo*)WProp_leds->GetChildByName("combo6"))->SetItems(Items);
    if(input_pins[5] == 0)
        ((CCombo*)WProp_leds->GetChildByName("combo6"))->SetText("0  NC");
    else
    {
        spin= pboard->MGetPinName(input_pins[5]);
        ((CCombo*)WProp_leds->GetChildByName("combo6"))->SetText(itoa(input_pins[5])+"  "+spin);
    }
    
    ((CCombo*)WProp_leds->GetChildByName("combo7"))->SetItems(Items);
    if(input_pins[6] == 0)
        ((CCombo*)WProp_leds->GetChildByName("combo7"))->SetText("0  NC");
    else
    {
        spin= pboard->MGetPinName(input_pins[6]);
        ((CCombo*)WProp_leds->GetChildByName("combo7"))->SetText(itoa(input_pins[6])+"  "+spin);
    }
    
    ((CCombo*)WProp_leds->GetChildByName("combo8"))->SetItems(Items);
    if(input_pins[7] == 0)
        ((CCombo*)WProp_leds->GetChildByName("combo8"))->SetText("0  NC");
    else
    {
        spin= pboard->MGetPinName(input_pins[7]);
        ((CCombo*)WProp_leds->GetChildByName("combo8"))->SetText(itoa(input_pins[7])+"  "+spin);
    }
    
    
    ((CButton*)WProp_leds->GetChildByName("button1"))->EvMouseButtonRelease = EVMOUSEBUTTONRELEASE & cpart_leds::PropButton;
    ((CButton*)WProp_leds->GetChildByName("button1"))->SetTag(1);
    
    ((CButton*)WProp_leds->GetChildByName("button2"))->EvMouseButtonRelease = EVMOUSEBUTTONRELEASE & cpart_leds::PropButton;
}

void 
cpart_leds::ReadPropiertsWindow(void)
{
   if(WProp_leds->GetTag())
   {
      input_pins[0]=atoi(((CCombo*)WProp_leds->GetChildByName("combo1"))->GetText());
      input_pins[1]=atoi(((CCombo*)WProp_leds->GetChildByName("combo2"))->GetText());
      input_pins[2]=atoi(((CCombo*)WProp_leds->GetChildByName("combo3"))->GetText());
      input_pins[3]=atoi(((CCombo*)WProp_leds->GetChildByName("combo4"))->GetText());
      input_pins[4]=atoi(((CCombo*)WProp_leds->GetChildByName("combo5"))->GetText());
      input_pins[5]=atoi(((CCombo*)WProp_leds->GetChildByName("combo6"))->GetText());
      input_pins[6]=atoi(((CCombo*)WProp_leds->GetChildByName("combo7"))->GetText());
      input_pins[7]=atoi(((CCombo*)WProp_leds->GetChildByName("combo8"))->GetText());
   }
} 


void
cpart_leds::PropButton (CControl * control, uint button, uint x, uint y, uint state)
{
   WProp_leds->SetTag(control->GetTag()); 
   WProp_leds->HideExclusive ();
};
