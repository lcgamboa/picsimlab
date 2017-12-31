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
#include"part_pot.h"

/* outputs */
enum {O_P1, O_P2, O_P3, O_P4, O_PO1, O_PO2, O_PO3, O_PO4} ;

/* inputs */
enum {I_PO1, I_PO2, I_PO3, I_PO4};

cpart_pot::cpart_pot(unsigned x, unsigned y)
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
  
};

cpart_pot::~cpart_pot(void)
{
    delete Bitmap;
}

 


void cpart_pot::Draw(void)
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
         case O_P3:
         case O_P4:                 
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
             canvas.SetColor (pic->pins[output_pins[0]-1].value*250, 0, 0);
           if(output[i].id == O_L2)
             canvas.SetColor (pic->pins[output_pins[1]-1].value*250, 0, 0);
           if(output[i].id == O_L3)
             canvas.SetColor (pic->pins[output_pins[2]-1].value*250, 0, 0);
           if(output[i].id == O_L4)
             canvas.SetColor (pic->pins[output_pins[3]-1].value*250, 0, 0);
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

 
void cpart_pot::Process(void)
{

  // b1[0]=pic->pins[output_pins[0]-1].value-pic->pins[output_pins[2]-1].value;
    
     
}
 

void 
cpart_pot::Reset(void)
{          

};




void 
cpart_pot::MouseButtonPress(uint button, uint x, uint y,uint state)
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
cpart_pot::MouseButtonRelease(uint button, uint x, uint y,uint state)
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
cpart_pot::KeyPress( uint key, uint x, uint y,uint mask)
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
cpart_pot::KeyRelease(uint key, uint x, uint y,uint mask)
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
cpart_pot::get_in_id(char * name)
{  

  if(strcmp(name,"PO1")==0)return I_PO1;
  if(strcmp(name,"PO2")==0)return I_PO2;
  if(strcmp(name,"PO3")==0)return I_PO3;
  if(strcmp(name,"PO4")==0)return I_PO4;
  
  printf("Erro input '%s' don't have a valid id! \n",name);
  return -1;
};

unsigned short 
cpart_pot::get_out_id(char * name)
{

  if(strcmp(name,"P1")==0)return O_P1;
  if(strcmp(name,"P2")==0)return O_P2;
  if(strcmp(name,"P3")==0)return O_P3;
  if(strcmp(name,"P4")==0)return O_P4;
  
  if(strcmp(name,"PO1")==0)return O_PO1;
  if(strcmp(name,"PO2")==0)return O_PO2;
  if(strcmp(name,"PO3")==0)return O_PO3;
  if(strcmp(name,"PO4")==0)return O_PO4;

  printf("Erro output '%s' don't have a valid id! \n",name);
  return 1;
};


String 
cpart_pot::WritePreferences(void)
{
  char prefs[256];
  
  sprintf(prefs,"%hhu,%hhu,%hhu,%hhu",input_pins[0],input_pins[1],input_pins[2],input_pins[3]);
  
  return prefs;
};

void 
cpart_pot::ReadPreferences(String value)
{
    sscanf(value.c_str (),"%hhu,%hhu,%hhu,%hhu",&input_pins[0],&input_pins[1],&input_pins[2],&input_pins[3]);      
};

CPWindow * WProp_pot;
      
void 
cpart_pot::ConfigurePropiertsWindow(CPWindow *  wprop)
{
    String Items="0  NC,";
    String spin;
    WProp_pot=wprop;
    board *pboard=Window1.GetBoard();
    
    for(int i=1; i<= pboard->MGetPinCount();i++ )
    {
       spin= pboard->MGetPinName(i);
      
      if(spin.Cmp(wxT("error")))
      {
        Items=Items+itoa(i)+"  "+spin+",";
      }
    }
    
    ((CCombo*)WProp_pot->GetChildByName("combo1"))->SetItems(Items);
    if(input_pins[0] == 0)
        ((CCombo*)WProp_pot->GetChildByName("combo1"))->SetText("0  NC");
    else
    {
        spin= pboard->MGetPinName(input_pins[0]);
        ((CCombo*)WProp_pot->GetChildByName("combo1"))->SetText(itoa(input_pins[0])+"  "+spin);
    }
    
    ((CCombo*)WProp_pot->GetChildByName("combo2"))->SetItems(Items);
    if(input_pins[1] == 0)
        ((CCombo*)WProp_pot->GetChildByName("combo2"))->SetText("0  NC");
    else
    {
        spin= pboard->MGetPinName(input_pins[1]);
        ((CCombo*)WProp_pot->GetChildByName("combo2"))->SetText(itoa(input_pins[1])+"  "+spin);
    }
    
    ((CCombo*)WProp_pot->GetChildByName("combo3"))->SetItems(Items);
    if(input_pins[3] == 0)
        ((CCombo*)WProp_pot->GetChildByName("combo3"))->SetText("0  NC");
    else
    {
        spin= pboard->MGetPinName(input_pins[2]);
        ((CCombo*)WProp_pot->GetChildByName("combo3"))->SetText(itoa(input_pins[2])+"  "+spin);
    }
    
    ((CCombo*)WProp_pot->GetChildByName("combo4"))->SetItems(Items);
    if(input_pins[3] == 0)
        ((CCombo*)WProp_pot->GetChildByName("combo4"))->SetText("0  NC");
    else
    {
        spin= pboard->MGetPinName(input_pins[3]);
        ((CCombo*)WProp_pot->GetChildByName("combo4"))->SetText(itoa(input_pins[3])+"  "+spin);
    }
    
       
    ((CButton*)WProp_pot->GetChildByName("button1"))->EvMouseButtonRelease = EVMOUSEBUTTONRELEASE & cpart_pot::PropButton;
    ((CButton*)WProp_pot->GetChildByName("button1"))->SetTag(1);
    
    ((CButton*)WProp_pot->GetChildByName("button2"))->EvMouseButtonRelease = EVMOUSEBUTTONRELEASE & cpart_pot::PropButton;
}

void 
cpart_pot::ReadPropiertsWindow(void)
{
   if(WProp_pot->GetTag())
   {
      input_pins[0]=atoi(((CCombo*)WProp_pot->GetChildByName("combo1"))->GetText());
      input_pins[1]=atoi(((CCombo*)WProp_pot->GetChildByName("combo2"))->GetText());
      input_pins[2]=atoi(((CCombo*)WProp_pot->GetChildByName("combo3"))->GetText());
      input_pins[3]=atoi(((CCombo*)WProp_pot->GetChildByName("combo4"))->GetText());
   }
} 


void
cpart_pot::PropButton (CControl * control, uint button, uint x, uint y, uint state)
{
   WProp_pot->SetTag(control->GetTag()); 
   WProp_pot->HideExclusive ();
};