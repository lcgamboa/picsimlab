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
#include"part_d_transfer_function.h"

/* outputs */
enum {O_P1, O_P2} ;


cpart_dtfunc::cpart_dtfunc(unsigned x, unsigned y)
{
   X=x;
   Y=y;        
   ReadMaps();   
     
   wxImage image;
   image.LoadFile(Window1.GetSharePath()+GetPictureFileName());

   Bitmap = new wxBitmap(image);
   
   canvas.Create(Bitmap);
   
  
   input_pin=0;
   output_pin=0;

};

cpart_dtfunc::~cpart_dtfunc(void)
{
    delete Bitmap;
}

 


void cpart_dtfunc::Draw(void)
{ 
 
  int i;
  board *pboard=Window1.GetBoard();
  
  canvas.Init();
  
  wxFont font(12, wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL );
 
  for(i=0;i<outputc;i++)
  {

      switch(output[i].id)
        {
         case O_P1:
         case O_P2:   
           canvas.SetFont (font);
           canvas.SetColor (49, 61, 99);
           canvas.Rectangle (1,output[i].x1,output[i].y1,output[i].x2-output[i].x1,output[i].y2-output[i].y1);
           canvas.SetFgColor (255, 255, 255);
           if(output[i].id == O_P1)
           {
             if(input_pin == 0)
               canvas.Text ("NC",output[i].x1,output[i].y1);
             else
               canvas.Text (pboard->MGetPinName (input_pin),output[i].x1,output[i].y1);  
           }
           if(output[i].id == O_P2)
           {
             if(output_pin == 0)
               canvas.Text ("NC",output[i].x1,output[i].y1);
             else  
               canvas.Text (pboard->MGetPinName (output_pin),output[i].x1,output[i].y1);
           }
           break;
           /*
         case O_ROT:
           canvas.SetLineWidth (8);
           canvas.SetColor (250, 250, 250);
           canvas.Circle (1,output[i].x1,output[i].y1,output[i].r);
           canvas.SetColor (55, 55, 55);
           canvas.Circle (1,output[i].x1,output[i].y1,output[i].r/5);

           canvas.Line (output[i].x1,output[i].y1,output[i].x1+output[i].r*sin(angle),output[i].y1+output[i].r*cos(angle));
           canvas.SetLineWidth (1);  
         break;
         case O_L1:
         case O_L2:
         case O_L3:
         case O_L4: //FIXME it must use mean value!
           if(output[i].id == O_L1)
             canvas.SetColor (pic->pins[input_pin[0]-1].value*250, 0, 0);
           if(output[i].id == O_L2)
             canvas.SetColor (pic->pins[input_pin[1]-1].value*250, 0, 0);
           if(output[i].id == O_L3)
             canvas.SetColor (pic->pins[input_pin[2]-1].value*250, 0, 0);
           if(output[i].id == O_L4)
             canvas.SetColor (pic->pins[input_pin[3]-1].value*250, 0, 0);
           canvas.Circle (1,output[i].x1,output[i].y1,output[i].r);
           break;
           */
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

 
void cpart_dtfunc::Process(void)
{

   //b1[0]=pic->pins[input_pin[0]-1].value-pic->pins[input_pin[2]-1].value;

}
 

void 
cpart_dtfunc::Reset(void)
{          

};




void 
cpart_dtfunc::MouseButtonPress(uint button, uint x, uint y,uint state)
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
cpart_dtfunc::MouseButtonRelease(uint button, uint x, uint y,uint state)
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
cpart_dtfunc::KeyPress( uint key, uint x, uint y,uint mask)
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
cpart_dtfunc::KeyRelease(uint key, uint x, uint y,uint mask)
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
cpart_dtfunc::get_in_id(char * name)
{  
  printf("Erro input '%s' don't have a valid id! \n",name);
  return -1;
};

unsigned short 
cpart_dtfunc::get_out_id(char * name)
{

  if(strcmp(name,"P1")==0)return O_P1;
  if(strcmp(name,"P2")==0)return O_P2;

  printf("Erro output '%s' don't have a valid id! \n",name);
  return 1;
};


String 
cpart_dtfunc::WritePreferences(void)
{
  char prefs[256];
  
  sprintf(prefs,"%hhu,%hhu",input_pin,output_pin);
  
  return prefs;
};

void 
cpart_dtfunc::ReadPreferences(String value)
{
   sscanf(value.c_str (),"%hhu,%hhu",&input_pin,&output_pin);      
};

CPWindow * WProp_dtfunc;
      
void 
cpart_dtfunc::ConfigurePropiertsWindow(CPWindow *  wprop)
{
    String Items="0  NC,";
    String spin;
    WProp_dtfunc=wprop;
    board *pboard=Window1.GetBoard();
    
    for(int i=1; i<= pboard->MGetPinCount();i++ )
    {
       spin= pboard->MGetPinName(i);
      
      if(spin.Cmp(wxT("error")))
      {
        Items=Items+itoa(i)+"  "+spin+",";
      }
    }
    
    ((CCombo*)WProp_dtfunc->GetChildByName("combo1"))->SetItems(Items);
    if(input_pin == 0)
        ((CCombo*)WProp_dtfunc->GetChildByName("combo1"))->SetText("0  NC");
    else
    {
        spin= pboard->MGetPinName(input_pin);;
        ((CCombo*)WProp_dtfunc->GetChildByName("combo1"))->SetText(itoa(input_pin)+"  "+spin);
    }
    
    ((CCombo*)WProp_dtfunc->GetChildByName("combo2"))->SetItems(Items);
    if(output_pin == 0)
        ((CCombo*)WProp_dtfunc->GetChildByName("combo2"))->SetText("0  NC");
    else
    {
        spin= pboard->MGetPinName(output_pin);;
        ((CCombo*)WProp_dtfunc->GetChildByName("combo2"))->SetText(itoa(output_pin)+"  "+spin);
    }
    
    
    ((CButton*)WProp_dtfunc->GetChildByName("button1"))->EvMouseButtonRelease = EVMOUSEBUTTONRELEASE & cpart_dtfunc::PropButton;
    ((CButton*)WProp_dtfunc->GetChildByName("button1"))->SetTag(1);
    
    ((CButton*)WProp_dtfunc->GetChildByName("button2"))->EvMouseButtonRelease = EVMOUSEBUTTONRELEASE & cpart_dtfunc::PropButton;
}

void 
cpart_dtfunc::ReadPropiertsWindow(void)
{
   if(WProp_dtfunc->GetTag())
   {
      input_pin=atoi(((CCombo*)WProp_dtfunc->GetChildByName("combo1"))->GetText());
      output_pin=atoi(((CCombo*)WProp_dtfunc->GetChildByName("combo2"))->GetText());
   }
} 


void
cpart_dtfunc::PropButton (CControl * control, uint button, uint x, uint y, uint state)
{
   WProp_dtfunc->SetTag(control->GetTag()); 
   WProp_dtfunc->HideExclusive ();
};