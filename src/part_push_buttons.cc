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
#include"part_push_buttons.h"

/* outputs */
enum {O_P1, O_P2, O_P3, O_P4, O_P5, O_P6, O_P7, O_P8, O_B1, O_B2, O_B3, O_B4, O_B5, O_B6, O_B7, O_B8, O_J1} ;

/* inputs */
enum {I_B1, I_B2, I_B3, I_B4, I_B5, I_B6, I_B7, I_B8, I_J1} ;


cpart_pbuttons::cpart_pbuttons(unsigned x, unsigned y)
{
   X=x;
   Y=y;        
   ReadMaps();   
     
   wxImage image;
   image.LoadFile(Window1.GetSharePath()+GetPictureFileName());

   Bitmap = new wxBitmap(image);
   
   canvas.Create(Bitmap);
   

   
   output_pins[0]=0;
   output_pins[1]=0;
   output_pins[2]=0;
   output_pins[3]=0;      
   output_pins[4]=0;
   output_pins[5]=0;
   output_pins[6]=0;
   output_pins[7]=0;  

};

cpart_pbuttons::~cpart_pbuttons(void)
{
    delete Bitmap;
}

 


void cpart_pbuttons::Draw(void)
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
             if(output_pins[0] == 0)
               canvas.Text ("NC",output[i].x1,output[i].y1);
             else
               canvas.Text (pboard->MGetPinName(output_pins[0]),output[i].x1,output[i].y1);  
           }
           if(output[i].id == O_P2)
           {
             if(output_pins[1] == 0)
               canvas.Text ("NC",output[i].x1,output[i].y1);
             else  
               canvas.Text (pboard->MGetPinName(output_pins[1]),output[i].x1,output[i].y1);
           }
           if(output[i].id == O_P3)
           {
             if(output_pins[2] == 0)
               canvas.Text ("NC",output[i].x1,output[i].y1);
             else  
               canvas.Text (pboard->MGetPinName(output_pins[2]),output[i].x1,output[i].y1);
           }
           if(output[i].id == O_P4)
           {
             if(output_pins[3] == 0)
               canvas.Text ("NC",output[i].x1,output[i].y1);
             else  
               canvas.Text (pboard->MGetPinName(output_pins[3]),output[i].x1,output[i].y1);
           }
           if(output[i].id == O_P5)
           {
             if(output_pins[4] == 0)
               canvas.Text ("NC",output[i].x1,output[i].y1);
             else
               canvas.Text (pboard->MGetPinName(output_pins[4]),output[i].x1,output[i].y1);  
           }
           if(output[i].id == O_P6)
           {
             if(output_pins[5] == 0)
               canvas.Text ("NC",output[i].x1,output[i].y1);
             else  
               canvas.Text (pboard->MGetPinName(output_pins[5]),output[i].x1,output[i].y1);
           }
           if(output[i].id == O_P7)
           {
             if(output_pins[6] == 0)
               canvas.Text ("NC",output[i].x1,output[i].y1);
             else  
               canvas.Text (pboard->MGetPinName(output_pins[6]),output[i].x1,output[i].y1);
           }
           if(output[i].id == O_P8)
           {
             if(output_pins[7] == 0)
               canvas.Text ("NC",output[i].x1,output[i].y1);
             else  
               canvas.Text (pboard->MGetPinName(output_pins[7]),output[i].x1,output[i].y1);
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

 
void cpart_pbuttons::Process(void)
{

//   b1[0]=pic->pins[output_pins[0]-1].value-pic->pins[output_pins[2]-1].value;

}
 

void 
cpart_pbuttons::Reset(void)
{          

};




void 
cpart_pbuttons::MouseButtonPress(uint button, uint x, uint y,uint state)
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
cpart_pbuttons::MouseButtonRelease(uint button, uint x, uint y,uint state)
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
cpart_pbuttons::KeyPress( uint key, uint x, uint y,uint mask)
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
cpart_pbuttons::KeyRelease(uint key, uint x, uint y,uint mask)
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
cpart_pbuttons::get_in_id(char * name)
{  
  if(strcmp(name,"B1")==0)return I_B1;
  if(strcmp(name,"B2")==0)return I_B2;
  if(strcmp(name,"B3")==0)return I_B3;
  if(strcmp(name,"B4")==0)return I_B4;
  if(strcmp(name,"B5")==0)return I_B5;
  if(strcmp(name,"B6")==0)return I_B6;
  if(strcmp(name,"B7")==0)return I_B7;
  if(strcmp(name,"B8")==0)return I_B8;
  
  if(strcmp(name,"J1")==0)return O_J1;
    
  printf("Erro input '%s' don't have a valid id! \n",name);
  return -1;
};

unsigned short 
cpart_pbuttons::get_out_id(char * name)
{

  if(strcmp(name,"P1")==0)return O_P1;
  if(strcmp(name,"P2")==0)return O_P2;
  if(strcmp(name,"P3")==0)return O_P3;
  if(strcmp(name,"P4")==0)return O_P4;
  if(strcmp(name,"P5")==0)return O_P5;
  if(strcmp(name,"P6")==0)return O_P6;
  if(strcmp(name,"P7")==0)return O_P7;
  if(strcmp(name,"P8")==0)return O_P8;
  
  if(strcmp(name,"B1")==0)return O_B1;
  if(strcmp(name,"B2")==0)return O_B2;
  if(strcmp(name,"B3")==0)return O_B3;
  if(strcmp(name,"B4")==0)return O_B4;
  if(strcmp(name,"B5")==0)return O_B5;
  if(strcmp(name,"B6")==0)return O_B6;
  if(strcmp(name,"B7")==0)return O_B7;
  if(strcmp(name,"B8")==0)return O_B8;
  
  if(strcmp(name,"J1")==0)return O_J1;

  printf("Erro output '%s' don't have a valid id! \n",name);
  return 1;
};


String 
cpart_pbuttons::WritePreferences(void)
{
  /*
    char line[100];
    Window1.saveprefs(wxT("p1_proc"),getnamebyproc(proc,line));
    Window1.saveprefs(wxT("p1_jmp"),String::Format("%i",jmp[0]));
    */
  return "";
};

void 
cpart_pbuttons::ReadPreferences(String value)
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

CPWindow * WProp_pbuttons;
      
void 
cpart_pbuttons::ConfigurePropiertsWindow(CPWindow *  wprop)
{
    String Items="0  NC,";
    String spin;
    WProp_pbuttons=wprop;
    board *pboard=Window1.GetBoard();
    
    for(int i=1; i<= pboard->MGetPinCount();i++ )
    {
       spin= pboard->MGetPinName(i);
      
      if(spin.Cmp(wxT("error")))
      {
        Items=Items+itoa(i)+"  "+spin+",";
      }
    }
    
    ((CCombo*)WProp_pbuttons->GetChildByName("combo1"))->SetItems(Items);
    if(output_pins[0] == 0)
        ((CCombo*)WProp_pbuttons->GetChildByName("combo1"))->SetText("0  NC");
    else
    {
        spin= pboard->MGetPinName(output_pins[0]);
        ((CCombo*)WProp_pbuttons->GetChildByName("combo1"))->SetText(itoa(output_pins[0])+"  "+spin);
    }
    
    ((CCombo*)WProp_pbuttons->GetChildByName("combo2"))->SetItems(Items);
    if(output_pins[1] == 0)
        ((CCombo*)WProp_pbuttons->GetChildByName("combo2"))->SetText("0  NC");
    else
    {
        spin= pboard->MGetPinName(output_pins[1]);
        ((CCombo*)WProp_pbuttons->GetChildByName("combo2"))->SetText(itoa(output_pins[1])+"  "+spin);
    }
    
    ((CCombo*)WProp_pbuttons->GetChildByName("combo3"))->SetItems(Items);
    if(output_pins[3] == 0)
        ((CCombo*)WProp_pbuttons->GetChildByName("combo3"))->SetText("0  NC");
    else
    {
        spin= pboard->MGetPinName(output_pins[2]);
        ((CCombo*)WProp_pbuttons->GetChildByName("combo3"))->SetText(itoa(output_pins[2])+"  "+spin);
    }
    
    ((CCombo*)WProp_pbuttons->GetChildByName("combo4"))->SetItems(Items);
    if(output_pins[3] == 0)
        ((CCombo*)WProp_pbuttons->GetChildByName("combo4"))->SetText("0  NC");
    else
    {
        spin= pboard->MGetPinName(output_pins[3]);
        ((CCombo*)WProp_pbuttons->GetChildByName("combo4"))->SetText(itoa(output_pins[3])+"  "+spin);
    }
    
    ((CCombo*)WProp_pbuttons->GetChildByName("combo5"))->SetItems(Items);
    if(output_pins[4] == 0)
        ((CCombo*)WProp_pbuttons->GetChildByName("combo5"))->SetText("0  NC");
    else
    {
        spin= pboard->MGetPinName(output_pins[4]);
        ((CCombo*)WProp_pbuttons->GetChildByName("combo5"))->SetText(itoa(output_pins[4])+"  "+spin);
    }
    
    ((CCombo*)WProp_pbuttons->GetChildByName("combo6"))->SetItems(Items);
    if(output_pins[5] == 0)
        ((CCombo*)WProp_pbuttons->GetChildByName("combo6"))->SetText("0  NC");
    else
    {
        spin= pboard->MGetPinName(output_pins[5]);
        ((CCombo*)WProp_pbuttons->GetChildByName("combo6"))->SetText(itoa(output_pins[5])+"  "+spin);
    }
    
    ((CCombo*)WProp_pbuttons->GetChildByName("combo7"))->SetItems(Items);
    if(output_pins[6] == 0)
        ((CCombo*)WProp_pbuttons->GetChildByName("combo7"))->SetText("0  NC");
    else
    {
        spin= pboard->MGetPinName(output_pins[6]);
        ((CCombo*)WProp_pbuttons->GetChildByName("combo7"))->SetText(itoa(output_pins[6])+"  "+spin);
    }
    
    ((CCombo*)WProp_pbuttons->GetChildByName("combo8"))->SetItems(Items);
    if(output_pins[7] == 0)
        ((CCombo*)WProp_pbuttons->GetChildByName("combo8"))->SetText("0  NC");
    else
    {
        spin= pboard->MGetPinName(output_pins[7]);
        ((CCombo*)WProp_pbuttons->GetChildByName("combo8"))->SetText(itoa(output_pins[7])+"  "+spin);
    }
    
    ((CButton*)WProp_pbuttons->GetChildByName("button1"))->EvMouseButtonRelease = EVMOUSEBUTTONRELEASE & cpart_pbuttons::PropButton;
    ((CButton*)WProp_pbuttons->GetChildByName("button1"))->SetTag(1);
    
    ((CButton*)WProp_pbuttons->GetChildByName("button2"))->EvMouseButtonRelease = EVMOUSEBUTTONRELEASE & cpart_pbuttons::PropButton;
}

void 
cpart_pbuttons::ReadPropiertsWindow(void)
{
   if(WProp_pbuttons->GetTag())
   {
      output_pins[0]=atoi(((CCombo*)WProp_pbuttons->GetChildByName("combo1"))->GetText());
      output_pins[1]=atoi(((CCombo*)WProp_pbuttons->GetChildByName("combo2"))->GetText());
      output_pins[2]=atoi(((CCombo*)WProp_pbuttons->GetChildByName("combo3"))->GetText());
      output_pins[3]=atoi(((CCombo*)WProp_pbuttons->GetChildByName("combo4"))->GetText());
      output_pins[4]=atoi(((CCombo*)WProp_pbuttons->GetChildByName("combo5"))->GetText());
      output_pins[5]=atoi(((CCombo*)WProp_pbuttons->GetChildByName("combo6"))->GetText());
      output_pins[6]=atoi(((CCombo*)WProp_pbuttons->GetChildByName("combo7"))->GetText());
      output_pins[7]=atoi(((CCombo*)WProp_pbuttons->GetChildByName("combo8"))->GetText());      
   }
} 


void
cpart_pbuttons::PropButton (CControl * control, uint button, uint x, uint y, uint state)
{
   WProp_pbuttons->SetTag(control->GetTag()); 
   WProp_pbuttons->HideExclusive ();
};