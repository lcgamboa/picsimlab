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


//Spare parts

#include"picsimlab1.h"
#include"picsimlab2.h"
#include"picsimlab5.h"
#include"picsimlab5_d.cc"



CPWindow5 Window5;


//Implementation

void
CPWindow5::_EvOnShow(CControl * control)
{
  draw1.SetWidth (GetWidth () - 15);
  draw1.SetHeight (GetHeight () - 40);
  timer1.SetRunState (1); 
};

void
CPWindow5::menu1_EvMenuActive(CControl * control)
{
  PartToCreate= ((CItemMenu*)control)->GetText ();
  wxSetCursor (wxCursor (wxCURSOR_CROSS));
};

void
CPWindow5::_EvOnCreate(CControl * control)
{

};


void
CPWindow5::draw1_EvMouseButtonPress(CControl * control, uint button, uint x, uint y,uint state)
{
  x=x/scale;
  y=y/scale;  

  for (int i = 0; i < partsc; i++)
    {
      if (parts[i]->PointInside ((int)x, (int)y))
        {
          parts[i]->MouseButtonPress (button, x - parts[i]->GetX (), y - parts[i]->GetY (), state);
          if (button == 3)
            {
               PartSelected=i;
               Widget->PopupMenu ((wxMenu*) (pmenu2.GetWidget ()), x*scale, y*scale);
            }
          return;
        }
    }

  //clique fora 
  if ((button == 1)&&(PartToCreate.size() > 0)&&(partsc < MAX_PARTS))
    {
       wxSetCursor (wxCursor (wxCURSOR_ARROW));   
       parts[partsc] = create_part ((char *)PartToCreate.char_str (), x, y);
       
       if(parts[partsc] == NULL)
         {
           Message(wxT("Erro creating part: ")+PartToCreate);
         }
       else
         {
           partsc++;
         }
       PartToCreate="";
      _EvOnShow (control);
    }


};



void
CPWindow5::draw1_EvMouseButtonRelease(CControl * control, uint button, uint x, uint y,uint state)
{
  x=x/scale;
  y=y/scale;  
  
  PartToMove=-1;
  
  
  wxSetCursor (wxCursor (wxCURSOR_ARROW));
  mdx=0;
  mdy=0;
  
    for (int i = 0; i < partsc; i++)
    {
      if (parts[i]->PointInside (x, y))
        {
          parts[i]->MouseButtonRelease (button, x - parts[i]->GetX (), y - parts[i]->GetY (), state);
          return;
        }
    }
};

void
CPWindow5::pmenu2_Propierties_EvMenuActive(CControl * control)
{  
  CPWindow * wd1= new CPWindow;
 
  wd1->SetName("window1");//must be the same as in xml 
  Application->ACreateWindow (wd1);
  
  if(wd1->LoadXMLContextAndCreateChilds (Window1.GetSharePath()+parts[PartSelected]->GetPropiertiesWindowFile()))
  {
    wd1->SetCanDestroy(false);

    parts[PartSelected]->ConfigurePropiertsWindow(wd1);
  
    wd1->Draw();
    wd1->ShowExclusive();
  
    while (wd1->GetCanExitExclusive())
    {
      Application->ProcessEvents(wd1->GetWWidget());
      wxMilliSleep(100);
    }
  
    parts[PartSelected]->ReadPropiertsWindow();
  
    wd1->SetCanDestroy (true);
    wd1->WDestroy ();
  }
};

void
CPWindow5::timer1_EvOnTime(CControl * control)
{

  draw1.Canvas.Init (1.0, 1.0);

  draw1.Canvas.SetFgColor (50, 50, 50);
  draw1.Canvas.SetBgColor (50, 50, 50);
  draw1.Canvas.Rectangle (1, 0, 0, draw1.GetWidth (), draw1.GetHeight ());
  
  //draw1.Canvas.End ();
  draw1.Canvas.Init (scale, scale);

  for (int i = 0; i < partsc; i++)
    {
      parts[i]->Draw ();
      draw1.Canvas.PutBitmap (parts[i]->GetBitmap (), parts[i]->GetX (), parts[i]->GetY ());
    }

  draw1.Canvas.End ();
  draw1.Update ();

};



void
CPWindow5::draw1_EvMouseMove(CControl * control, uint button, uint x, uint y,uint state)
{
  x=x/scale;
  y=y/scale;
  
  if(PartToMove >= 0)
  {

    
    if((mdx == 0)&&(mdy == 0))  
      {
        mdx=parts[PartToMove]->GetX()-x; 
        mdy=parts[PartToMove]->GetY()-y;
      }
    
     parts[PartToMove]->SetX (x+mdx); 
     parts[PartToMove]->SetY (y+mdy);
  }
  else
  {
    for (int i = 0; i < partsc; i++)
    {
      if (parts[i]->PointInside (x, y))
        {
          parts[i]->MouseMove (button, x - parts[i]->GetX (), y - parts[i]->GetY (), state);
          return;
        }
    }    
  }
  
};



void
CPWindow5::draw1_EvKeyboardPress(CControl * control, uint key, uint x, uint y,uint mask)
{

  switch(key)
  {
    case '='://+
      scale+=0.1;
      if(scale > 2)scale=2;
      break;  
    case '-':
      scale-=0.1;
      if(scale < 0.1)scale=0.1;
      break;
  }
  
};

void
CPWindow5::draw1_EvKeyboardRelease(CControl * control, uint key, uint x, uint y,uint mask)
{
  //code here:)
  //mprint(wxT("draw1_EvKeyboardRelease\n"));
};

bool 
CPWindow5::SaveConfig(String fname)
{
  String temp;
  
    CStringList prefs;
    prefs.Clear();
    for (int i = 0; i < partsc; i++)
    {
      temp.Printf ("%s,%i,%i:%s",parts[i]->GetName(),parts[i]->GetX(),parts[i]->GetY(),parts[i]->WritePreferences());  
      prefs.AddLine(temp);
    }
    
    return prefs.SaveToFile(fname);
}

bool 
CPWindow5::LoadConfig(String fname)
{
    char name[256];  
    char temp[256]; 
    unsigned int x,y;
    CStringList prefs;
    
    bool ret=wxFileExists(fname);
    
    if(ret)
    {
      int partsc_;  
      prefs.LoadFromFile(fname);
      
      DeleteParts();
      partsc_=0;
      for (unsigned int i = 0; i < prefs.GetLinesCount (); i++)
      {
        sscanf(prefs.GetLine (i).c_str (),"%256[^,],%i,%i:%256[^\n]",name,&x,&y,temp);
              
        if((parts[i]=create_part(name,x,y)))
        {
          parts[i]->ReadPreferences(temp);
          partsc_++;
        }
        else
        {
          printf("Erro loading part: %s \n",name);  
        }
      }
      partsc=partsc_;
    }
    
    return ret;
}
  
void 
CPWindow5::DeleteParts(void)
{
     int partsc_ = partsc;
     partsc=0; //for disable process
     
    //delete previous parts
   
    for (int i = 0; i < partsc_; i++)
    {
        delete parts[i];
    } 
    
}


void
CPWindow5::menu1_File_Saveconfiguration_EvMenuActive(CControl * control)
{
  filedialog1.SetType(wxFD_SAVE|wxFD_CHANGE_DIR);  
  if(filedialog1.Run())
  {
     if(wxFileExists(filedialog1.GetFileName()))
     {
       
         if(!Dialog("Overwriting file: "+basename(filedialog1.GetFileName())+"?"))
             return;
     } 
    SaveConfig(filedialog1.GetFileName());
  } 
};

void
CPWindow5::menu1_File_Loadconfiguration_EvMenuActive(CControl * control)
{
  filedialog1.SetType(wxFD_OPEN|wxFD_CHANGE_DIR);
  if(filedialog1.Run())
  {
    LoadConfig(filedialog1.GetFileName());
  }
};


void 
CPWindow5::Process(void)
{
    for (int i = 0; i < partsc; i++)
    {
        parts[i]->Process ();
    } 
};


void
CPWindow5::_EvOnHide(CControl * control)
{
  timer1.SetRunState (0);
  Window1.GetBoard ()->SetUseSpareParts (0);
};


void
CPWindow5::pmenu2_Move_EvMenuActive(CControl * control)
{
    PartToMove=PartSelected;
    wxSetCursor (wxCursor (wxCURSOR_SIZENWSE));           
};

void
CPWindow5::pmenu2_Delete_EvMenuActive(CControl * control)
{

    int partsc_=partsc;
    partsc=0;//disable process
    
    delete  parts[PartSelected];
  
    for (int i = PartSelected; i < partsc_-1; i++)
    {
        parts[i]=parts[i+1];
    } 
    partsc_--;

    partsc=partsc_;
};


void
CPWindow5::menu1_Help_Contents_EvMenuActive(CControl * control)
{
  Window2.html1.SetLoadFile(Window1.GetSharePath()+wxT("doc/picsimlab.html"));   
  Window2.Show();
};

void
CPWindow5::menu1_Help_About_EvMenuActive(CControl * control)
{
   Message(wxT("Developed by L.C. Gamboa\n <lcgamboa@yahoo.com>\n Version: ")+String(wxT(_VERSION_)));
};



void 
CPWindow5::WritePreferences(void)
{
    Window1.saveprefs(wxT("spare_position"),itoa(GetX())+wxT(",")+itoa(GetY())+wxT(",")+itoa(GetWidth())+wxT(",")+itoa(GetHeight()));
};


void 
CPWindow5::ReadPreferences(char *name,char *value)
{

    if(!strcmp(name,"spare_position"))
    {
      int x,y,w,h;     
      sscanf(value,"%i,%i,%i,%i",&x,&y,&w,&h);
      SetX(x);
      SetY(y);
      if(w > 5000)w=5000;
      if(h > 5000)h=5000;
      SetWidth(w);
      SetHeight(h);
    }
};
